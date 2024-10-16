package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc2.video.VideoCanvas.RENDER_MODE_FIT;
import static io.agora.rtc2.video.VideoCanvas.RENDER_MODE_HIDDEN;
import static io.agora.rtc2.video.VideoEncoderConfiguration.STANDARD_BITRATE;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;

import java.io.File;
import java.util.Locale;
import java.util.Random;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.widget.VideoReportLayout;
import io.agora.api.example.databinding.DialogLeaveOptionsBinding;
import io.agora.api.example.utils.CommonUtil;
import io.agora.api.example.utils.PermissonUtils;
import io.agora.api.example.utils.TokenUtils;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.LeaveChannelOptions;
import io.agora.rtc2.RtcConnection;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.RtcEngineEx;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoEncoderConfiguration;

/**
 * The type Join multiple channel.
 */
@Example(
        index = 13,
        group = ADVANCED,
        name = R.string.item_joinmultichannel,
        actionId = R.id.action_mainFragment_to_MultiChannel,
        tipsId = R.string.joinmultichannel
)
public class JoinMultipleChannel extends BaseFragment implements View.OnClickListener {
    private static final String TAG = JoinMultipleChannel.class.getSimpleName();

    private VideoReportLayout fl_local, fl_remote, fl_remote2;
    private Button join, joinEx;
    private EditText et_channel;
    private RtcEngineEx engine;
    private boolean joined = false, joinedEx = false;
    private String channel1;
    private String channel2;
    private RtcConnection rtcConnection2 = new RtcConnection();

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_join_multi_channel, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        join = view.findViewById(R.id.btn_join);
        joinEx = view.findViewById(R.id.btn_join_ex);
        et_channel = view.findViewById(R.id.et_channel);
        view.findViewById(R.id.btn_join).setOnClickListener(this);
        view.findViewById(R.id.btn_take_snapshot_ex).setOnClickListener(this);
        joinEx.setOnClickListener(this);
        fl_local = view.findViewById(R.id.fl_local);
        fl_remote = view.findViewById(R.id.fl_remote);
        fl_remote2 = view.findViewById(R.id.fl_remote2);
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
        super.onDestroy();
        /*leaveChannel and Destroy the RtcEngine instance*/
        if (engine != null) {
            engine.leaveChannel();
            engine.stopPreview();
        }
        handler.post(RtcEngine::destroy);
        engine = null;
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_join) {
            if (!joined) {
                CommonUtil.hideInputBoard(getActivity(), et_channel);
                // call when join button hit
                channel1 = et_channel.getText().toString();
                channel2 = channel1 + "-2";
                // Check permission
                checkOrRequestPermisson(new PermissonUtils.PermissionResultCallback() {
                    @Override
                    public void onPermissionsResult(boolean allPermissionsGranted, String[] permissions, int[] grantResults) {
                        // Permissions Granted
                        if (allPermissionsGranted) {
                            joinChannel(channel1);
                        }
                    }
                });
            } else {
                joined = false;
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
                engine.stopPreview();
                join.setText(getString(R.string.join));
            }
        } else if (v == joinEx) {
            if (joinedEx) {
                DialogLeaveOptionsBinding binding = DialogLeaveOptionsBinding.inflate(LayoutInflater.from(requireContext()));
                new AlertDialog.Builder(requireContext())
                        .setTitle(R.string.leave_options)
                        .setView(binding.getRoot())
                        .setPositiveButton(R.string.confirm, (dialog, which) -> {
                            dialog.dismiss();
                            boolean stopMic = binding.swStopMic.isChecked();
                            LeaveChannelOptions options = new LeaveChannelOptions();
                            options.stopMicrophoneRecording = stopMic;
                            engine.leaveChannelEx(rtcConnection2, options);
                        })
                        .setNegativeButton(R.string.cancel, (dialog, which) -> {
                            dialog.dismiss();
                        })
                        .show();
            } else {
                joinSecondChannel();
            }
        } else if (v.getId() == R.id.btn_take_snapshot_ex) {
            takeSnapshotEx();
        }
    }

    private void takeSnapshotEx() {
        int remoteUid = fl_remote2.getReportUid();
        if (remoteUid <= 0 || !joinedEx) {
            showLongToast(getString(R.string.remote_screenshot_tip));
            return;
        }
        String filePath = requireContext().getExternalCacheDir().getAbsolutePath() + File.separator + "joinmultiplechannel_snapshot_ex.png";
        int ret = engine.takeSnapshotEx(rtcConnection2, remoteUid, filePath);
        if (ret != Constants.ERR_OK) {
            showLongToast("takeSnapshotEx error code=" + ret + ",msg=" + RtcEngine.getErrorDescription(ret));
        }
    }

    private void joinChannel(String channelId) {
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
        engine.setupLocalVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, 0));
        // Set audio route to microPhone
        engine.setDefaultAudioRoutetoSpeakerphone(true);

        /*In the demo, the default is to enter as the anchor.*/
        engine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
        // Enable video module
        engine.enableVideo();
        // start preview
        engine.startPreview();
        // Setup video encoding configs
        engine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                ((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingDimensionObject(),
                VideoEncoderConfiguration.FRAME_RATE.valueOf(((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingFrameRate()),
                STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.valueOf(((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingOrientation())
        ));

        /*Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        TokenUtils.gen(requireContext(), channelId, 0, ret -> {
            /* Allows a user to join a channel.
             if you do not specify the uid, we will generate the uid for you*/

            ChannelMediaOptions option = new ChannelMediaOptions();
            option.clientRoleType = Constants.CLIENT_ROLE_AUDIENCE;
            option.autoSubscribeAudio = true;
            option.autoSubscribeVideo = true;
            int res = engine.joinChannel(ret, channelId, 0, option);
            if (res != 0) {
                engine.stopPreview();
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                showAlert(RtcEngine.getErrorDescription(Math.abs(res)));
                return;
            }
            joinSecondChannel();
            // Prevent repeated entry
            join.setEnabled(false);
        });
    }

    private void joinSecondChannel() {
        ChannelMediaOptions mediaOptions = new ChannelMediaOptions();
        mediaOptions.clientRoleType = Constants.CLIENT_ROLE_BROADCASTER;
        mediaOptions.publishMicrophoneTrack = true;
        mediaOptions.publishCameraTrack = true;
        mediaOptions.autoSubscribeAudio = true;
        mediaOptions.autoSubscribeVideo = true;
        rtcConnection2.channelId = channel2;
        rtcConnection2.localUid = new Random().nextInt(512) + 512;
        TokenUtils.gen(requireContext(), rtcConnection2.channelId, rtcConnection2.localUid, new TokenUtils.OnTokenGenCallback<String>() {
            @Override
            public void onTokenGen(String token) {
                int res = engine.joinChannelEx(token, rtcConnection2, mediaOptions, iRtcEngineEventHandler2);
                if (res != Constants.ERR_OK) {
                    showAlert(RtcEngine.getErrorDescription(Math.abs(res)));
                }
            }
        });
    }


    private final IRtcEngineEventHandler iRtcEngineEventHandler2 = new IRtcEngineEventHandler() {
        private int myUid = 0;

        /**
         * Occurs when the local user joins a specified channel.
         * The channel name assignment is based on channelName specified in the joinChannel method.
         * If the uid is not specified when joinChannel is called, the server automatically assigns a uid.
         *
         * @param channel Channel name
         * @param uid     User ID
         * @param elapsed Time elapsed (ms) from the user calling joinChannel until this callback is triggered
         */
        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
            Log.i(TAG, String.format("channel2 onJoinChannelSuccess channel %s uid %d", channel2, uid));
            showLongToast(String.format("onJoinChannelSuccess channel %s uid %d", channel2, uid));
            joinedEx = true;
            myUid = uid;
            runOnUIThread(() -> {
                joinEx.setEnabled(true);
                joinEx.setText(getString(R.string.leave_ex_channel));
            });
        }

        @Override
        public void onLeaveChannel(RtcStats stats) {
            super.onLeaveChannel(stats);
            Log.i(TAG, String.format("local user %d leaveChannel!", myUid));
            showLongToast(String.format("local user %d leaveChannel!", myUid));
            joinedEx = false;
            runOnUIThread(() -> {
                joinEx.setText(R.string.join_ex_channel);
                fl_remote2.removeAllViews();
                fl_remote2.setReportUid(0);
            });
        }

        @Override
        public void onLocalAudioStateChanged(int state, int error) {
            super.onLocalAudioStateChanged(state, error);
            Log.i(TAG, String.format("onLocalAudioStateChanged state:%d!", state));
            showLongToast(String.format(Locale.US, "onLocalAudioStateChanged state:%d!", state));
            if (state == Constants.LOCAL_AUDIO_STREAM_STATE_STOPPED) {
                runOnUIThread(() -> fl_local.setLocalAudioStats(new LocalAudioStats()));
                showAlert(getString(R.string.microphone_stop_tip));
            }
        }

        @Override
        public void onLocalAudioStats(LocalAudioStats stats) {
            super.onLocalAudioStats(stats);
            runOnUIThread(() -> fl_local.setLocalAudioStats(stats));
        }

        @Override
        public void onLocalVideoStats(Constants.VideoSourceType source, LocalVideoStats stats) {
            super.onLocalVideoStats(source, stats);
            runOnUIThread(() -> fl_local.setLocalVideoStats(stats));
        }

        @Override
        public void onRemoteAudioStats(RemoteAudioStats stats) {
            super.onRemoteAudioStats(stats);
            runOnUIThread(() -> {
                if (fl_remote2.getReportUid() == stats.uid) {
                    fl_remote2.setRemoteAudioStats(stats);
                }
            });
        }

        @Override
        public void onRemoteVideoStats(RemoteVideoStats stats) {
            super.onRemoteVideoStats(stats);
            runOnUIThread(() -> {
                if (fl_remote2.getReportUid() == stats.uid) {
                    fl_remote2.setRemoteVideoStats(stats);
                }
            });
        }

        /**
         * Occurs when a remote user (Communication)/host (Live Broadcast) joins the channel.
         *
         * @param uid     ID of the user whose audio state changes.
         * @param elapsed Time delay (ms) from the local user calling joinChannel/setClientRole
         *                until this callback is triggered.
         */
        @Override
        public void onUserJoined(int uid, int elapsed) {
            Log.i(TAG, "channel2 onUserJoined->" + uid);
            showLongToast(String.format("user %d joined!", uid));
            /*Check if the context is correct*/
            Context context = getContext();
            if (context == null) {
                return;
            }
            runOnUIThread(() -> {
                /*Display remote video stream*/
                SurfaceView surfaceView = null;
                if (fl_remote2.getChildCount() > 0) {
                    fl_remote2.removeAllViews();
                }
                fl_remote2.setReportUid(uid);
                // Create render view by RtcEngine
                surfaceView = new SurfaceView(context);
                surfaceView.setZOrderMediaOverlay(true);
                // Add to the remote container
                fl_remote2.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));

                // Setup remote video to render
                engine.setupRemoteVideoEx(new VideoCanvas(surfaceView, RENDER_MODE_FIT, uid), rtcConnection2);
            });
        }

        @Override
        public void onUserOffline(int uid, int reason) {
            super.onUserOffline(uid, reason);
            runOnUIThread(() -> {
                if (fl_remote2.getReportUid() == uid) {
                    fl_remote2.removeAllViews();
                    fl_remote2.setReportUid(0);
                }
            });
        }

        @Override
        public void onSnapshotTaken(int uid, String filePath, int width, int height, int errCode) {
            super.onSnapshotTaken(uid, filePath, width, height, errCode);
            Log.d(TAG, String.format(Locale.US, "onSnapshotTaken uid=%d, filePath=%s, width=%d, height=%d, errorCode=%d", uid, filePath, width, height, errCode));
            if (errCode == 0) {
                showLongToast("SnapshotExTaken path=" + filePath);
            } else {
                showLongToast("SnapshotExTaken error=" + RtcEngine.getErrorDescription(errCode));
            }
        }
    };

    /**
     * IRtcEngineEventHandler is an abstract class providing default implementation.
     * The SDK uses this class to report to the app on SDK runtime events.
     */
    private final IRtcEngineEventHandler iRtcEngineEventHandler = new IRtcEngineEventHandler() {
        private int myUid = 0;

        /**
         * Error code description can be found at:
         * en: https://api-ref.agora.io/en/video-sdk/android/4.x/API/class_irtcengineeventhandler.html#callback_irtcengineeventhandler_onerror
         * cn: https://docs.agora.io/cn/video-call-4.x/API%20Reference/java_ng/API/class_irtcengineeventhandler.html#callback_irtcengineeventhandler_onerror
         */
        @Override
        public void onError(int err) {
            Log.w(TAG, String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
        }

        /**
         * Occurs when a user leaves the channel.
         *
         * @param stats With this callback, the application retrieves the channel information,
         *              such as the call duration and statistics.
         */
        @Override
        public void onLeaveChannel(RtcStats stats) {
            super.onLeaveChannel(stats);
            Log.i(TAG, String.format("local user %d leaveChannel!", myUid));
            joined = false;
            showLongToast(String.format("local user %d leaveChannel!", myUid));
            runOnUIThread(() -> joinEx.setEnabled(false));
        }

        /**
         * Occurs when the local user joins a specified channel.
         * The channel name assignment is based on channelName specified in the joinChannel method.
         * If the uid is not specified when joinChannel is called, the server automatically assigns a uid.
         *
         * @param channel Channel name
         * @param uid     User ID
         * @param elapsed Time elapsed (ms) from the user calling joinChannel until this callback is triggered
         */
        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
            Log.i(TAG, String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            showLongToast(String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            joined = true;
            myUid = uid;
            runOnUIThread(() -> {
                join.setEnabled(true);
                joinEx.setEnabled(true);
                join.setText(getString(R.string.leave));
            });
        }

        /**
         * Since v2.9.0.
         * This callback indicates the state change of the remote audio stream.
         * PS: This callback does not work properly when the number of users (in the Communication profile) or
         * broadcasters (in the Live-broadcast profile) in the channel exceeds 17.
         *
         * @param uid     ID of the user whose audio state changes.
         * @param state   State of the remote audio
         *                REMOTE_AUDIO_STATE_STOPPED(0): The remote audio is in the default state, probably due
         *                to REMOTE_AUDIO_REASON_LOCAL_MUTED(3), REMOTE_AUDIO_REASON_REMOTE_MUTED(5),
         *                or REMOTE_AUDIO_REASON_REMOTE_OFFLINE(7).
         *                REMOTE_AUDIO_STATE_STARTING(1): The first remote audio packet is received.
         *                REMOTE_AUDIO_STATE_DECODING(2): The remote audio stream is decoded and plays normally,
         *                probably due to REMOTE_AUDIO_REASON_NETWORK_RECOVERY(2),
         *                REMOTE_AUDIO_REASON_LOCAL_UNMUTED(4) or REMOTE_AUDIO_REASON_REMOTE_UNMUTED(6).
         *                REMOTE_AUDIO_STATE_FROZEN(3): The remote audio is frozen, probably due to
         *                REMOTE_AUDIO_REASON_NETWORK_CONGESTION(1).
         *                REMOTE_AUDIO_STATE_FAILED(4): The remote audio fails to start, probably due to
         *                REMOTE_AUDIO_REASON_INTERNAL(0).
         * @param reason  The reason of the remote audio state change.
         *                REMOTE_AUDIO_REASON_INTERNAL(0): Internal reasons.
         *                REMOTE_AUDIO_REASON_NETWORK_CONGESTION(1): Network congestion.
         *                REMOTE_AUDIO_REASON_NETWORK_RECOVERY(2): Network recovery.
         *                REMOTE_AUDIO_REASON_LOCAL_MUTED(3): The local user stops receiving the remote audio
         *                stream or disables the audio module.
         *                REMOTE_AUDIO_REASON_LOCAL_UNMUTED(4): The local user resumes receiving the remote audio
         *                stream or enables the audio module.
         *                REMOTE_AUDIO_REASON_REMOTE_MUTED(5): The remote user stops sending the audio stream or
         *                disables the audio module.
         *                REMOTE_AUDIO_REASON_REMOTE_UNMUTED(6): The remote user resumes sending the audio stream
         *                or enables the audio module.
         *                REMOTE_AUDIO_REASON_REMOTE_OFFLINE(7): The remote user leaves the channel.
         * @param elapsed Time elapsed (ms) from the local user calling the joinChannel method
         *                until the SDK triggers this callback.
         */
        @Override
        public void onRemoteAudioStateChanged(int uid, int state, int reason, int elapsed) {
            super.onRemoteAudioStateChanged(uid, state, reason, elapsed);
            Log.i(TAG, "onRemoteAudioStateChanged->" + uid + ", state->" + state + ", reason->" + reason);
        }

        /**
         * Since v2.9.0.
         * Occurs when the remote video state changes.
         * PS: This callback does not work properly when the number of users (in the Communication
         * profile) or broadcasters (in the Live-broadcast profile) in the channel exceeds 17.
         *
         * @param uid     ID of the remote user whose video state changes.
         * @param state   State of the remote video:
         *                REMOTE_VIDEO_STATE_STOPPED(0): The remote video is in the default state, probably due
         *                to REMOTE_VIDEO_STATE_REASON_LOCAL_MUTED(3), REMOTE_VIDEO_STATE_REASON_REMOTE_MUTED(5),
         *                or REMOTE_VIDEO_STATE_REASON_REMOTE_OFFLINE(7).
         *                REMOTE_VIDEO_STATE_STARTING(1): The first remote video packet is received.
         *                REMOTE_VIDEO_STATE_DECODING(2): The remote video stream is decoded and plays normally,
         *                probably due to REMOTE_VIDEO_STATE_REASON_NETWORK_RECOVERY (2),
         *                REMOTE_VIDEO_STATE_REASON_LOCAL_UNMUTED(4), REMOTE_VIDEO_STATE_REASON_REMOTE_UNMUTED(6),
         *                or REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK_RECOVERY(9).
         *                REMOTE_VIDEO_STATE_FROZEN(3): The remote video is frozen, probably due to
         *                REMOTE_VIDEO_STATE_REASON_NETWORK_CONGESTION(1) or REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK(8).
         *                REMOTE_VIDEO_STATE_FAILED(4): The remote video fails to start, probably due to
         *                REMOTE_VIDEO_STATE_REASON_INTERNAL(0).
         * @param reason  The reason of the remote video state change:
         *                REMOTE_VIDEO_STATE_REASON_INTERNAL(0): Internal reasons.
         *                REMOTE_VIDEO_STATE_REASON_NETWORK_CONGESTION(1): Network congestion.
         *                REMOTE_VIDEO_STATE_REASON_NETWORK_RECOVERY(2): Network recovery.
         *                REMOTE_VIDEO_STATE_REASON_LOCAL_MUTED(3): The local user stops receiving the remote
         *                video stream or disables the video module.
         *                REMOTE_VIDEO_STATE_REASON_LOCAL_UNMUTED(4): The local user resumes receiving the remote
         *                video stream or enables the video module.
         *                REMOTE_VIDEO_STATE_REASON_REMOTE_MUTED(5): The remote user stops sending the video
         *                stream or disables the video module.
         *                REMOTE_VIDEO_STATE_REASON_REMOTE_UNMUTED(6): The remote user resumes sending the video
         *                stream or enables the video module.
         *                REMOTE_VIDEO_STATE_REASON_REMOTE_OFFLINE(7): The remote user leaves the channel.
         *                REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK(8): The remote media stream falls back to the
         *                audio-only stream due to poor network conditions.
         *                REMOTE_VIDEO_STATE_REASON_AUDIO_FALLBACK_RECOVERY(9): The remote media stream switches
         *                back to the video stream after the network conditions improve.
         * @param elapsed Time elapsed (ms) from the local user calling the joinChannel method until
         *                the SDK triggers this callback.
         */
        @Override
        public void onRemoteVideoStateChanged(int uid, int state, int reason, int elapsed) {
            super.onRemoteVideoStateChanged(uid, state, reason, elapsed);
            Log.i(TAG, "onRemoteVideoStateChanged->" + uid + ", state->" + state + ", reason->" + reason);
        }

        @Override
        public void onRemoteAudioStats(RemoteAudioStats stats) {
            super.onRemoteAudioStats(stats);
            runOnUIThread(() -> {
                if (fl_remote.getReportUid() == stats.uid) {
                    fl_remote.setRemoteAudioStats(stats);
                }
            });
        }

        @Override
        public void onRemoteVideoStats(RemoteVideoStats stats) {
            super.onRemoteVideoStats(stats);
            runOnUIThread(() -> {
                if (fl_remote.getReportUid() == stats.uid) {
                    fl_remote.setRemoteVideoStats(stats);
                }
            });
        }

        /**
         * Occurs when a remote user (Communication)/host (Live Broadcast) joins the channel.
         *
         * @param uid     ID of the user whose audio state changes.
         * @param elapsed Time delay (ms) from the local user calling joinChannel/setClientRole
         *                until this callback is triggered.
         */
        @Override
        public void onUserJoined(int uid, int elapsed) {
            super.onUserJoined(uid, elapsed);
            Log.i(TAG, "onUserJoined->" + uid);
            showLongToast(String.format("user %d joined!", uid));
            /*Check if the context is correct*/
            Context context = getContext();
            if (context == null) {
                return;
            }
            handler.post(() -> {
                /*Display remote video stream*/
                SurfaceView surfaceView = null;
                if (fl_remote.getChildCount() > 0) {
                    fl_remote.removeAllViews();
                }
                fl_remote.setReportUid(uid);
                // Create render view by RtcEngine
                surfaceView = new SurfaceView(context);
                surfaceView.setZOrderMediaOverlay(true);
                // Add to the remote container
                fl_remote.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));

                // Setup remote video to render
                engine.setupRemoteVideo(new VideoCanvas(surfaceView, RENDER_MODE_FIT, uid));
            });
        }

        /**
         * Occurs when a remote user (Communication)/host (Live Broadcast) leaves the channel.
         *
         * @param uid    ID of the user whose audio state changes.
         * @param reason Reason why the user goes offline:
         *               USER_OFFLINE_QUIT(0): The user left the current channel.
         *               USER_OFFLINE_DROPPED(1): The SDK timed out and the user dropped offline because no data
         *               packet was received within a certain period of time. If a user quits the
         *               call and the message is not passed to the SDK (due to an unreliable channel),
         *               the SDK assumes the user dropped offline.
         *               USER_OFFLINE_BECOME_AUDIENCE(2): (Live broadcast only.) The client role switched from
         *               the host to the audience.
         */
        @Override
        public void onUserOffline(int uid, int reason) {
            Log.i(TAG, String.format("user %d offline! reason:%d", uid, reason));
            showLongToast(String.format("user %d offline! reason:%d", uid, reason));
            handler.post(new Runnable() {
                @Override
                public void run() {
                    /*Clear render view
                     Note: The video will stay at its last frame, to completely remove it you will need to
                     remove the SurfaceView from its parent*/
                    engine.setupRemoteVideo(new VideoCanvas(null, RENDER_MODE_HIDDEN, uid));
                }
            });
        }
    };
}
