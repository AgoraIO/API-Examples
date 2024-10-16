package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc2.video.VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15;
import static io.agora.rtc2.video.VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_ADAPTIVE;
import static io.agora.rtc2.video.VideoEncoderConfiguration.STANDARD_BITRATE;
import static io.agora.rtc2.video.VideoEncoderConfiguration.VD_640x360;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.media.projection.MediaProjection;
import android.media.projection.MediaProjectionManager;
import android.os.Build;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.Switch;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.service.MediaProjectionService;
import io.agora.api.example.utils.CommonUtil;
import io.agora.api.example.utils.PermissonUtils;
import io.agora.api.example.utils.TokenUtils;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.RtcEngineEx;
import io.agora.rtc2.ScreenCaptureParameters;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoEncoderConfiguration;

/**
 * This example demonstrates how video can be flexibly switched between the camera stream and the
 * screen share stream during an audio-video call.
 */
@Example(
        index = 18,
        group = ADVANCED,
        name = R.string.item_screensharing,
        actionId = R.id.action_mainFragment_to_ScreenSharing,
        tipsId = R.string.screensharing
)
public class ScreenSharing extends BaseFragment implements View.OnClickListener,
        CompoundButton.OnCheckedChangeListener, SeekBar.OnSeekBarChangeListener, AdapterView.OnItemSelectedListener {
    private static final String TAG = ScreenSharing.class.getSimpleName();
    private static final int PROJECTION_REQ_CODE = 1 << 2;
    private static final int DEFAULT_SHARE_FRAME_RATE = 15;
    private FrameLayout fl_local, fl_remote;
    private Button join;
    private Switch screenAudio, screenPreview, externalMediaPro;
    private SeekBar screenAudioVolume;
    private EditText et_channel;
    private int myUid, remoteUid = -1;
    private boolean joined = false;
    private RtcEngineEx engine;
    private final ScreenCaptureParameters screenCaptureParameters = new ScreenCaptureParameters();
    private String channelId;

    private Spinner screenScenarioType;

    private MediaProjectionManager mediaProjectionManager;
    private MediaProjection[] mediaProjection = new MediaProjection[1];
    private final ActivityResultLauncher<Intent> mediaProjectionLauncher = registerForActivityResult(
            new ActivityResultContracts.StartActivityForResult(),
            result -> {
                Log.d(TAG, "result-------------------result.getResultCode(): " + result.getResultCode());
                if (result.getResultCode() == Activity.RESULT_OK) {
                    try {
                        mediaProjection[0] = mediaProjectionManager
                                .getMediaProjection(result.getResultCode(), result.getData());
                        joinChannel();
                    } catch (Exception e) {
                        Log.e(TAG, "error msg: " + e.getMessage());
                    }
                }
            }
    );

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_screen_sharing, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        mediaProjectionManager = (MediaProjectionManager) getContext().getSystemService(Context.MEDIA_PROJECTION_SERVICE);

        join = view.findViewById(R.id.btn_join);
        et_channel = view.findViewById(R.id.et_channel);
        fl_local = view.findViewById(R.id.fl_camera);
        fl_remote = view.findViewById(R.id.fl_screenshare);
        join.setOnClickListener(this);

        screenPreview = view.findViewById(R.id.screen_preview);
        externalMediaPro = view.findViewById(R.id.media_projection_external);
        screenAudio = view.findViewById(R.id.screen_audio);
        screenAudioVolume = view.findViewById(R.id.screen_audio_volume);
        screenScenarioType = view.findViewById(R.id.spinner_screen_scenario_type);

        screenScenarioType.setOnItemSelectedListener(this);
        screenPreview.setOnCheckedChangeListener(this);
        screenAudio.setOnCheckedChangeListener(this);
        externalMediaPro.setOnCheckedChangeListener(this);
        screenAudioVolume.setOnSeekBarChangeListener(this);
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        // Check if the context is valid
        Context context = getContext();
        if (context == null) {
            return;
        }
        try {
            RtcEngineConfig config = new RtcEngineConfig();
            /*
             * The context of Android Activity
             */
            config.mContext = context.getApplicationContext();
            /*
             * The App ID issued to you by Agora. See <a href="https://docs.agora.io/en/Agora%20Platform/token#get-an-app-id"> How to get the App ID</a>
             */
            config.mAppId = getString(R.string.agora_app_id);
            /* Sets the channel profile of the Agora RtcEngine.
             CHANNEL_PROFILE_COMMUNICATION(0): (Default) The Communication profile.
             Use this profile in one-on-one calls or group calls, where all users can talk freely.
             CHANNEL_PROFILE_LIVE_BROADCASTING(1): The Live-Broadcast profile. Users in a live-broadcast
             channel have a role as either broadcaster or audience. A broadcaster can both send and receive streams;
             an audience can only receive streams.*/
            config.mChannelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING;
            /*
             * IRtcEngineEventHandler is an abstract class providing default implementation.
             * The SDK uses this class to report to the app on SDK runtime events.
             */
            config.mEventHandler = iRtcEngineEventHandler;
            config.mAudioScenario = Constants.AudioScenario.getValue(Constants.AudioScenario.DEFAULT);
            config.mAreaCode = ((MainApplication) getActivity().getApplication()).getGlobalSettings().getAreaCode();
            engine = (RtcEngineEx) RtcEngine.create(config);
            /*
             * This parameter is for reporting the usages of APIExample to agora background.
             * Generally, it is not necessary for you to set this parameter.
             */
            engine.setParameters("{"
                    + "\"rtc.report_app_scenario\":"
                    + "{"
                    + "\"appScenario\":" + 100 + ","
                    + "\"serviceType\":" + 11 + ","
                    + "\"appVersion\":\"" + RtcEngine.getSdkVersion() + "\""
                    + "}"
                    + "}");
            /* setting the local access point if the private cloud ip was set, otherwise the config will be invalid.*/
            LocalAccessPointConfiguration localAccessPointConfiguration = ((MainApplication) getActivity().getApplication()).getGlobalSettings().getPrivateCloudConfig();
            if (localAccessPointConfiguration != null) {
                // This api can only be used in the private media server scenario, otherwise some problems may occur.
                engine.setLocalAccessPoint(localAccessPointConfiguration);
            }
        } catch (Exception e) {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }

    @Override
    public void onDestroy() {
        stopService();
        /*leaveChannel and Destroy the RtcEngine instance*/
        if (engine != null) {
            engine.leaveChannel();
            engine.stopScreenCapture();
            engine.stopPreview();
        }
        engine = null;
        super.onDestroy();
        handler.post(RtcEngine::destroy);
    }


    @Override
    public void onCheckedChanged(CompoundButton compoundButton, boolean checked) {
        if (compoundButton == screenPreview) {
            if (!joined) {
                return;
            }
            if (checked) {
                startScreenSharePreview();
            } else {
                stopScreenSharePreview();
            }
        } else if (compoundButton == screenAudio) {
            if (!joined) {
                return;
            }
            screenCaptureParameters.captureAudio = checked;
            engine.updateScreenCaptureParameters(screenCaptureParameters);
        } else if (compoundButton == externalMediaPro) {
        }
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_join) {
            if (!joined) {
                CommonUtil.hideInputBoard(getActivity(), et_channel);
                // call when join button hit
                channelId = et_channel.getText().toString();
                // Check permission
                checkOrRequestPermisson(new PermissonUtils.PermissionResultCallback() {
                    @Override
                    public void onPermissionsResult(boolean allPermissionsGranted, String[] permissions, int[] grantResults) {
                        if (allPermissionsGranted) {
                            if (externalMediaPro.isChecked()) {
                                requestScreenCapture();
                            } else {
                                joinChannel();
                            }
                        }
                    }
                });
            } else {
                leaveChannel();
            }
        }
    }

    private void startScreenSharePreview() {
        // Check if the context is valid
        Context context = getContext();
        if (context == null) {
            return;
        }

        // Create render view by RtcEngine
        SurfaceView surfaceView = new SurfaceView(context);
        if (fl_local.getChildCount() > 0) {
            fl_local.removeAllViews();
        }
        // Add to the local container
        fl_local.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        // Setup local video to render your local camera preview
        VideoCanvas local = new VideoCanvas(surfaceView, Constants.RENDER_MODE_FIT, 0);
        local.mirrorMode = Constants.VIDEO_MIRROR_MODE_DISABLED;
        local.sourceType = Constants.VIDEO_SOURCE_SCREEN_PRIMARY;
        engine.setupLocalVideo(local);

        engine.startPreview(Constants.VideoSourceType.VIDEO_SOURCE_SCREEN_PRIMARY);
    }

    private void stopScreenSharePreview() {
        fl_local.removeAllViews();
        engine.setupLocalVideo(new VideoCanvas(null));
        engine.stopPreview(Constants.VideoSourceType.VIDEO_SOURCE_SCREEN_PRIMARY);
    }

    private void startService() {
//        if (joined) {
        Intent intent = new Intent(requireContext(), MediaProjectionService.class);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            requireContext().startForegroundService(intent);
        } else {
            requireContext().startService(intent);
        }
//        }
    }

    private void stopService() {
        Intent serviceIntent = new Intent(getContext(), MediaProjectionService.class);
        getContext().stopService(serviceIntent);
    }

    private void requestScreenCapture() {
        startService();
        Intent intent = mediaProjectionManager.createScreenCaptureIntent();
        mediaProjectionLauncher.launch(intent);
    }


    private void joinChannel() {
        engine.setParameters("{\"che.video.mobile_1080p\":true}");
        engine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);

        /*Enable video module*/
        engine.enableVideo();
        // Setup video encoding configs
        engine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                VD_640x360,
                FRAME_RATE_FPS_15,
                STANDARD_BITRATE,
                ORIENTATION_MODE_ADAPTIVE
        ));
        /*Set up to play remote sound with receiver*/
        engine.setDefaultAudioRoutetoSpeakerphone(true);

        DisplayMetrics metrics = new DisplayMetrics();
        getActivity().getWindowManager().getDefaultDisplay().getRealMetrics(metrics);
        screenCaptureParameters.captureVideo = true;
        screenCaptureParameters.videoCaptureParameters.width = 720;
        screenCaptureParameters.videoCaptureParameters.height = (int) (720 * 1.0f / metrics.widthPixels * metrics.heightPixels);
        screenCaptureParameters.videoCaptureParameters.framerate = DEFAULT_SHARE_FRAME_RATE;
        screenCaptureParameters.captureAudio = screenAudio.isChecked();
        screenCaptureParameters.audioCaptureParameters.captureSignalVolume = screenAudioVolume.getProgress();
        if (externalMediaPro.isChecked()) {
            engine.setExternalMediaProjection(mediaProjection[0]);
        } else {
            engine.setExternalMediaProjection(null);
        }
        engine.startScreenCapture(screenCaptureParameters);

        if (screenPreview.isChecked()) {
            startScreenSharePreview();
        }

        /*Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        TokenUtils.gen(requireContext(), channelId, 0, accessToken -> {
            /* Allows a user to join a channel.
             if you do not specify the uid, we will generate the uid for you*/
            // set options
            ChannelMediaOptions options = new ChannelMediaOptions();
            options.clientRoleType = Constants.CLIENT_ROLE_BROADCASTER;
            options.autoSubscribeVideo = true;
            options.autoSubscribeAudio = true;
            options.publishCameraTrack = false;
            options.publishMicrophoneTrack = false;
            options.publishScreenCaptureVideo = true;
            options.publishScreenCaptureAudio = true;
            int res = engine.joinChannel(accessToken, channelId, 0, options);
            if (res != 0) {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                showAlert(RtcEngine.getErrorDescription(Math.abs(res)));
                return;
            }
            // Prevent repeated entry
            join.setEnabled(false);
        });
    }

    /**
     * IRtcEngineEventHandler is an abstract class providing default implementation.
     * The SDK uses this class to report to the app on SDK runtime events.
     */
    private final IRtcEngineEventHandler iRtcEngineEventHandler = new IRtcEngineEventHandler() {
        /**
         * Error code description can be found at:
         * en: https://api-ref.agora.io/en/video-sdk/android/4.x/API/class_irtcengineeventhandler.html#callback_irtcengineeventhandler_onerror
         * cn: https://docs.agora.io/cn/video-call-4.x/API%20Reference/java_ng/API/class_irtcengineeventhandler.html#callback_irtcengineeventhandler_onerror
         */
        @Override
        public void onError(int err) {
            Log.e(TAG, String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
            showAlert(String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
        }

        /**Occurs when the local user joins a specified channel.
         * The channel name assignment is based on channelName specified in the joinChannel method.
         * If the uid is not specified when joinChannel is called, the server automatically assigns a uid.
         * @param channel Channel name
         * @param uid User ID
         * @param elapsed Time elapsed (ms) from the user calling joinChannel until this callback is triggered*/
        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
            Log.i(TAG, String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            showLongToast(String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            myUid = uid;
            joined = true;
            handler.post(() -> {
                externalMediaPro.setEnabled(false);
                join.setEnabled(true);
                join.setText(getString(R.string.leave));
            });
        }

        @Override
        public void onLocalVideoStateChanged(Constants.VideoSourceType source, int state, int error) {
            super.onLocalVideoStateChanged(source, state, error);
            Log.i(TAG, "onLocalVideoStateChanged source=" + source + ", state=" + state + ", error=" + error);
            if (source == Constants.VideoSourceType.VIDEO_SOURCE_SCREEN_PRIMARY) {
                if (state == Constants.LOCAL_VIDEO_STREAM_STATE_ENCODING) {
                    if (error == Constants.ERR_OK) {
                        showLongToast("Screen sharing start successfully.");
                    }
                } else if (state == Constants.LOCAL_VIDEO_STREAM_STATE_FAILED) {
                    if (error == Constants.ERR_SCREEN_CAPTURE_SYSTEM_NOT_SUPPORTED) {
                        showLongToast("Screen sharing has been cancelled");
                    } else {
                        showLongToast("Screen sharing start failed for error " + error);
                    }
                    runOnUIThread(() -> leaveChannel());
                }
            }
        }

        /**Since v2.9.0.
         * Occurs when the remote video state changes.
         * PS: This callback does not work properly when the number of users (in the Communication
         *     profile) or broadcasters (in the Live-broadcast profile) in the channel exceeds 17.
         * @param uid ID of the remote user whose video state changes.
         * @param state State of the remote video:
         *   REMOTE_VIDEO_STATE_STOPPED(0): The remote video is in the default state, probably due
         *              to REMOTE_VIDEO_STATE_REASON_LOCAL_MUTED(3), REMOTE_VIDEO_STATE_REASON_REMOTE_MUTED(5),
         *              or REMOTE_VIDEO_STATE_REASON_REMOTE_OFFLINE(7).
         *   REMOTE_VIDEO_STATE_STARTING(1): The first remote video packet is received.
         *   REMOTE_VIDEO_STATE_DECODING(2): The remote video stream is decoded and plays normally,
         *              probably due to REMOTE_VIDEO_STATE_REASON_NETWORK_RECOVERY (2),
         *              REMOTE_VIDEO_STATE_REASON_LOCAL_UNMUTED(4), REMOTE_VIDEO_STATE_REASON_REMOTE_UNMUTED(6),
         *              or REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK_RECOVERY(9).
         *   REMOTE_VIDEO_STATE_FROZEN(3): The remote video is frozen, probably due to
         *              REMOTE_VIDEO_STATE_REASON_NETWORK_CONGESTION(1) or REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK(8).
         *   REMOTE_VIDEO_STATE_FAILED(4): The remote video fails to start, probably due to
         *              REMOTE_VIDEO_STATE_REASON_INTERNAL(0).
         * @param reason The reason of the remote video state change:
         *   REMOTE_VIDEO_STATE_REASON_INTERNAL(0): Internal reasons.
         *   REMOTE_VIDEO_STATE_REASON_NETWORK_CONGESTION(1): Network congestion.
         *   REMOTE_VIDEO_STATE_REASON_NETWORK_RECOVERY(2): Network recovery.
         *   REMOTE_VIDEO_STATE_REASON_LOCAL_MUTED(3): The local user stops receiving the remote
         *               video stream or disables the video module.
         *   REMOTE_VIDEO_STATE_REASON_LOCAL_UNMUTED(4): The local user resumes receiving the remote
         *               video stream or enables the video module.
         *   REMOTE_VIDEO_STATE_REASON_REMOTE_MUTED(5): The remote user stops sending the video
         *               stream or disables the video module.
         *   REMOTE_VIDEO_STATE_REASON_REMOTE_UNMUTED(6): The remote user resumes sending the video
         *               stream or enables the video module.
         *   REMOTE_VIDEO_STATE_REASON_REMOTE_OFFLINE(7): The remote user leaves the channel.
         *   REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK(8): The remote media stream falls back to the
         *               audio-only stream due to poor network conditions.
         *   REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK_RECOVERY(9): The remote media stream switches
         *               back to the video stream after the network conditions improve.
         * @param elapsed Time elapsed (ms) from the local user calling the joinChannel method until
         *               the SDK triggers this callback.*/
        @Override
        public void onRemoteVideoStateChanged(int uid, int state, int reason, int elapsed) {
            super.onRemoteVideoStateChanged(uid, state, reason, elapsed);
            Log.i(TAG, "onRemoteVideoStateChanged:uid->" + uid + ", state->" + state);
        }

        @Override
        public void onRemoteVideoStats(RemoteVideoStats stats) {
            super.onRemoteVideoStats(stats);
            Log.d(TAG, "onRemoteVideoStats: width:" + stats.width + " x height:" + stats.height);
        }

        /**Occurs when a remote user (Communication)/host (Live Broadcast) joins the channel.
         * @param uid ID of the user whose audio state changes.
         * @param elapsed Time delay (ms) from the local user calling joinChannel/setClientRole
         *                until this callback is triggered.*/
        @Override
        public void onUserJoined(int uid, int elapsed) {
            super.onUserJoined(uid, elapsed);
            Log.i(TAG, "onUserJoined->" + uid);
            showLongToast(String.format("user %d joined!", uid));
            if (remoteUid > 0) {
                return;
            }
            remoteUid = uid;
            runOnUIThread(() -> {
                SurfaceView renderView = new SurfaceView(getContext());
                engine.setupRemoteVideo(new VideoCanvas(renderView, Constants.RENDER_MODE_FIT, uid));
                fl_remote.removeAllViews();
                fl_remote.addView(renderView);
            });
        }

        /**Occurs when a remote user (Communication)/host (Live Broadcast) leaves the channel.
         * @param uid ID of the user whose audio state changes.
         * @param reason Reason why the user goes offline:
         *   USER_OFFLINE_QUIT(0): The user left the current channel.
         *   USER_OFFLINE_DROPPED(1): The SDK timed out and the user dropped offline because no data
         *              packet was received within a certain period of time. If a user quits the
         *               call and the message is not passed to the SDK (due to an unreliable channel),
         *               the SDK assumes the user dropped offline.
         *   USER_OFFLINE_BECOME_AUDIENCE(2): (Live broadcast only.) The client role switched from
         *               the host to the audience.*/
        @Override
        public void onUserOffline(int uid, int reason) {
            Log.i(TAG, String.format("user %d offline! reason:%d", uid, reason));
            showLongToast(String.format("user %d offline! reason:%d", uid, reason));
            if (remoteUid == uid) {
                remoteUid = -1;
                runOnUIThread(() -> {
                    fl_remote.removeAllViews();
                    engine.setupRemoteVideo(new VideoCanvas(null, Constants.RENDER_MODE_FIT, uid));
                });
            }
        }

    };

    private void leaveChannel() {
        externalMediaPro.setEnabled(true);
        stopService();
        joined = false;
        join.setText(getString(R.string.join));
        fl_local.removeAllViews();
        fl_remote.removeAllViews();
        remoteUid = -1;
        myUid = -1;

        engine.leaveChannel();
        engine.stopScreenCapture();
        engine.stopPreview();
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        if (seekBar == screenAudioVolume) {
            if (!joined) {
                return;
            }
            screenCaptureParameters.audioCaptureParameters.captureSignalVolume = progress;
            engine.updateScreenCaptureParameters(screenCaptureParameters);
        }
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        if (parent == screenScenarioType) {
            engine.setScreenCaptureScenario(Constants.ScreenScenarioType.valueOf(screenScenarioType.getSelectedItem().toString()));
        }
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {

    }

}
