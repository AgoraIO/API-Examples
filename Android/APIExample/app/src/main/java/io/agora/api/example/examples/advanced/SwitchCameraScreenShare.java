package io.agora.api.example.examples.advanced;

import static io.agora.rtc2.video.VideoCanvas.RENDER_MODE_HIDDEN;
import static io.agora.rtc2.video.VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15;
import static io.agora.rtc2.video.VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_ADAPTIVE;
import static io.agora.rtc2.video.VideoEncoderConfiguration.STANDARD_BITRATE;
import static io.agora.rtc2.video.VideoEncoderConfiguration.VD_640x360;

import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.Switch;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.Random;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.utils.CommonUtil;
import io.agora.api.example.utils.PermissonUtils;
import io.agora.api.example.utils.TokenUtils;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcConnection;
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
//@Example(
//        index = 10,
//        group = ADVANCED,
//        name = R.string.item_cameraorscreen,
//        actionId = R.id.action_mainFragment_to_SwitchCameraScreenShare,
//        tipsId = R.string.switchcamerascreen
//)
public class SwitchCameraScreenShare extends BaseFragment implements View.OnClickListener, CompoundButton.OnCheckedChangeListener {
    private static final String TAG = SwitchCameraScreenShare.class.getSimpleName();
    private static final int DEFAULT_SHARE_FRAME_RATE = 15;
    private FrameLayout fl_camera, fl_screen;
    private Button join;
    private Switch camera, screenShare, screenSharePreview;
    private EditText et_channel;
    private int myUid, remoteUid = -1;
    private boolean joined = false;
    private int curRenderMode = RENDER_MODE_HIDDEN;
    private ChannelMediaOptions options = new ChannelMediaOptions();
    private RtcEngineEx engine;
    private RtcConnection rtcConnection2 = new RtcConnection();


    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_switch_camera_screenshare, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        join = view.findViewById(R.id.btn_join);
        camera = view.findViewById(R.id.camera);
        screenShare = view.findViewById(R.id.screenShare);
        screenSharePreview = view.findViewById(R.id.screenSharePreview);
        et_channel = view.findViewById(R.id.et_channel);
        fl_camera = view.findViewById(R.id.fl_camera);
        fl_screen = view.findViewById(R.id.fl_screenshare);
        join.setOnClickListener(this);
        camera.setOnCheckedChangeListener(this);
        screenShare.setOnCheckedChangeListener(this);
        screenSharePreview.setOnCheckedChangeListener(this);
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
        /*leaveChannel and Destroy the RtcEngine instance*/
        if (engine != null) {
            if (camera.isChecked()) {
                engine.leaveChannelEx(rtcConnection2);
            }
            engine.leaveChannel();
            engine.stopPreview();
        }
        engine = null;
        super.onDestroy();
        handler.post(RtcEngine::destroy);
    }


    @Override
    public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
        if (compoundButton.getId() == R.id.screenShare) {
            if (Build.VERSION.SDK_INT > Build.VERSION_CODES.LOLLIPOP) {
                if(b){
                    DisplayMetrics metrics = new DisplayMetrics();
                    getActivity().getWindowManager().getDefaultDisplay().getRealMetrics(metrics);
                    ScreenCaptureParameters parameters = new ScreenCaptureParameters();
                    parameters.videoCaptureParameters.width = 720;
                    parameters.videoCaptureParameters.height = (int) (720 * 1.0f / metrics.widthPixels * metrics.heightPixels);
                    parameters.videoCaptureParameters.framerate = DEFAULT_SHARE_FRAME_RATE;
                    parameters.captureAudio = true;
                    // start screen capture and update options
                    engine.startScreenCapture(parameters);
                    options.publishScreenCaptureVideo = true;
                    options.publishCameraTrack = false;
                    options.publishScreenCaptureAudio = true;
                    engine.updateChannelMediaOptions(options);
                    addScreenSharePreview();
                } else {
                    // stop screen capture and update options
                    engine.stopScreenCapture();
                    options.publishScreenCaptureVideo = false;
                    engine.updateChannelMediaOptions(options);
                }
                screenSharePreview.setEnabled(b);
                screenSharePreview.setChecked(b);
            } else {
                showAlert(getString(R.string.lowversiontip));
            }
        } else if (compoundButton.getId() == R.id.camera) {
            if (b) {
                ChannelMediaOptions mediaOptions = new ChannelMediaOptions();
                mediaOptions.autoSubscribeAudio = false;
                mediaOptions.autoSubscribeVideo = false;
                mediaOptions.publishScreenCaptureVideo = false;
                mediaOptions.publishCameraTrack = true;
                mediaOptions.clientRoleType = Constants.CLIENT_ROLE_BROADCASTER;
                mediaOptions.channelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING;
                rtcConnection2.channelId = et_channel.getText().toString();
                rtcConnection2.localUid = new Random().nextInt(512) + 512;
                engine.joinChannelEx(null, rtcConnection2, mediaOptions, iRtcEngineEventHandler);
            } else {
                engine.leaveChannelEx(rtcConnection2);
                engine.startPreview(Constants.VideoSourceType.VIDEO_SOURCE_CAMERA_PRIMARY);
            }
        } else if (compoundButton.getId() == R.id.screenSharePreview) {
            if (b) {
                addScreenSharePreview();
            } else {
                fl_screen.removeAllViews();
                engine.stopPreview(Constants.VideoSourceType.VIDEO_SOURCE_SCREEN_PRIMARY);
            }
        }
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_join) {
            if (!joined) {
                CommonUtil.hideInputBoard(getActivity(), et_channel);
                // call when join button hit
                String channelId = et_channel.getText().toString();
                // Check permission
                checkOrRequestPermisson(new PermissonUtils.PermissionResultCallback() {
                    @Override
                    public void onPermissionsResult(boolean allPermissionsGranted, String[] permissions, int[] grantResults) {
                        // Permissions Granted
                        if (allPermissionsGranted) {
                            joinChannel(channelId);
                        }
                    }
                });
            } else {
                joined = false;
                join.setText(getString(R.string.join));
                camera.setEnabled(false);
                screenShare.setEnabled(false);
                screenSharePreview.setEnabled(false);
                fl_camera.removeAllViews();
                fl_screen.removeAllViews();

                if (camera.isChecked()) {
                    engine.leaveChannelEx(rtcConnection2);
                    camera.setChecked(false);
                }
                if (screenSharePreview.isChecked()) {
                    engine.stopPreview(Constants.VideoSourceType.VIDEO_SOURCE_SCREEN_PRIMARY);
                    screenSharePreview.setChecked(false);
                }
                if (screenShare.isChecked()) {
                    engine.stopScreenCapture();
                    screenShare.setChecked(false);
                }
                engine.stopPreview(Constants.VideoSourceType.VIDEO_SOURCE_CAMERA_PRIMARY);
                /*After joining a channel, the user must call the leaveChannel method to end the
                 * call before joining another channel. This method returns 0 if the user leaves the
                 * channel and releases all resources related to the call. This method call is
                 * asynchronous, and the user has not exited the channel when the method call returns.
                 * Once the user leaves the channel, the SDK triggers the onLeaveChannel callback.
                 * A successful leaveChannel method call triggers the following callbacks:
                 *      1:The local client: onLeaveChannel.
                 *      2:The remote client: onUserOffline, if the user leaving the channel is in the
                 *          Communication channel, or is a BROADCASTER in the Live Broadcast profile.
                 * @returns 0: Success.
                 *          < 0: Failure.
                 * PS:
                 *      1:If you call the destroy method immediately after calling the leaveChannel
                 *          method, the leaveChannel process interrupts, and the SDK does not trigger
                 *          the onLeaveChannel callback.
                 *      2:If you call the leaveChannel method during CDN live streaming, the SDK
                 *          triggers the removeInjectStreamUrl method.*/
                engine.leaveChannel();

                //requireActivity().finish();
            }
        }
    }

    private void addScreenSharePreview() {
        // Check if the context is valid
        Context context = getContext();
        if (context == null) {
            return;
        }

        // Create render view by RtcEngine
        SurfaceView surfaceView = new SurfaceView(context);
        if (fl_screen.getChildCount() > 0) {
            fl_screen.removeAllViews();
        }
        // Add to the local container
        fl_screen.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        // Setup local video to render your local camera preview
        VideoCanvas local = new VideoCanvas(surfaceView, Constants.RENDER_MODE_FIT, 0);
        local.mirrorMode = Constants.VIDEO_MIRROR_MODE_DISABLED;
        local.sourceType = Constants.VIDEO_SOURCE_SCREEN_PRIMARY;
        engine.setupLocalVideo(local);
        engine.startPreview(Constants.VideoSourceType.VIDEO_SOURCE_SCREEN_PRIMARY);
    }

    private void addCameraPreview() {
        // Check if the context is valid
        Context context = getContext();
        if (context == null) {
            return;
        }

        // Create render view by RtcEngine
        SurfaceView surfaceView = new SurfaceView(context);
        if (fl_camera.getChildCount() > 0) {
            fl_camera.removeAllViews();
        }
        // Add to the local container
        fl_camera.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        // Setup local video to render your local camera preview
        VideoCanvas local = new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, 0);
        local.sourceType = Constants.VIDEO_SOURCE_CAMERA_PRIMARY;
        engine.setupLocalVideo(local);
        engine.startPreview(Constants.VideoSourceType.VIDEO_SOURCE_CAMERA_PRIMARY);
    }

    private void joinChannel(String channelId) {
        engine.setParameters("{\"che.video.mobile_1080p\":true}");
        engine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
        // set options
        options = new ChannelMediaOptions();
        options.clientRoleType = Constants.CLIENT_ROLE_BROADCASTER;
        options.autoSubscribeVideo = true;
        options.autoSubscribeAudio = true;
        options.publishCameraTrack = false;
        options.publishScreenCaptureVideo = false;
        options.publishMicrophoneTrack = false;
        options.enableAudioRecordingOrPlayout = false;
        options.publishEncodedVideoTrack = false;

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

        /*Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        TokenUtils.gen(requireContext(), channelId, 0, accessToken -> {
            /* Allows a user to join a channel.
             if you do not specify the uid, we will generate the uid for you*/
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
            addCameraPreview();
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
                join.setEnabled(true);
                join.setText(getString(R.string.leave));
                camera.setEnabled(true);
                screenShare.setEnabled(true);
            });
        }

        @Override
        public void onLocalVideoStateChanged(Constants.VideoSourceType source, int state, int error) {
            super.onLocalVideoStateChanged(source, state, error);
            if (state == 1) {
                Log.i(TAG, "local view published successfully!");
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
        }
    };

}
