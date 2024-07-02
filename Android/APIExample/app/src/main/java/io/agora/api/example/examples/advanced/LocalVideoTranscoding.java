package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc2.Constants.RENDER_MODE_HIDDEN;
import static io.agora.rtc2.video.VideoEncoderConfiguration.STANDARD_BITRATE;

import android.annotation.SuppressLint;
import android.content.Context;
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

import java.util.ArrayList;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.widget.VideoReportLayout;
import io.agora.api.example.utils.CommonUtil;
import io.agora.api.example.utils.TokenUtils;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.LocalTranscoderConfiguration;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.ScreenCaptureParameters;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;
import io.agora.rtc2.video.CameraCapturerConfiguration;
import io.agora.rtc2.video.SegmentationProperty;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoEncoderConfiguration;
import io.agora.rtc2.video.VirtualBackgroundSource;

/**
 * This demo demonstrates how to make a one-to-one video call
 */
@Example(
        index = 19,
        group = ADVANCED,
        name = R.string.item_localvideotranscoding,
        actionId = R.id.action_mainFragment_to_LocalVideoTranscoding,
        tipsId = R.string.localvideotranscoding
)
public class LocalVideoTranscoding extends BaseFragment implements View.OnClickListener, CompoundButton.OnCheckedChangeListener {
    private static final String TAG = LocalVideoTranscoding.class.getSimpleName();

    private VideoReportLayout videoReportLayout;
    private Button join;
    private Switch switchTransparentBackground;
    private EditText et_channel;
    private RtcEngine engine;
    private int myUid;
    private boolean joined = false;


    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_localvideotranscoding, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        join = view.findViewById(R.id.btn_join);
        switchTransparentBackground = view.findViewById(R.id.btn_transparent_background);
        et_channel = view.findViewById(R.id.et_channel);
        view.findViewById(R.id.btn_join).setOnClickListener(this);
        switchTransparentBackground.setOnCheckedChangeListener(this);
        videoReportLayout = view.findViewById(R.id.videoReportLayout);
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
            engine = RtcEngine.create(config);
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
        }
        catch (Exception e) {
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
            engine.stopPreview(Constants.VideoSourceType.VIDEO_SOURCE_TRANSCODED);
            engine.stopCameraCapture(Constants.VideoSourceType.VIDEO_SOURCE_CAMERA_PRIMARY);
            engine.stopScreenCapture();
        }
        handler.post(RtcEngine::destroy);
        engine = null;
    }

    @SuppressLint("WrongConstant")
    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_join) {
            if (!joined) {
                CommonUtil.hideInputBoard(getActivity(), et_channel);
                // call when join button hit
                String channelId = et_channel.getText().toString();
                // Check permission
                joinChannel(channelId);
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
                engine.stopPreview(Constants.VideoSourceType.VIDEO_SOURCE_TRANSCODED);
                engine.stopCameraCapture(Constants.VideoSourceType.VIDEO_SOURCE_CAMERA_PRIMARY);
                engine.stopScreenCapture();
                join.setText(getString(R.string.join));
                videoReportLayout.removeAllViews();
            }
        }
    }

    private void joinChannel(String channelId) {
        // Check if the context is valid
        Context context = getContext();
        if (context == null) {
            return;
        }
        DisplayMetrics metrics = new DisplayMetrics();
        requireActivity().getWindowManager().getDefaultDisplay().getRealMetrics(metrics);
        int width = 720;
        int height = (int) (width * 1.0f / metrics.widthPixels * metrics.heightPixels);

        /*In the demo, the default is to enter as the anchor.*/
        engine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
        // Enable video module
        engine.enableVideo();
        // Setup video encoding configs
        engine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                new VideoEncoderConfiguration.VideoDimensions(width, height),
                VideoEncoderConfiguration.FRAME_RATE.valueOf(((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingFrameRate()),
                STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.valueOf(((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingOrientation())
        ));

        // Set audio route to microPhone
        engine.setDefaultAudioRoutetoSpeakerphone(true);


        engine.startCameraCapture(Constants.VideoSourceType.VIDEO_SOURCE_CAMERA_PRIMARY, new CameraCapturerConfiguration(CameraCapturerConfiguration.CAMERA_DIRECTION.CAMERA_FRONT));
        ScreenCaptureParameters screenCaptureParameters = new ScreenCaptureParameters();
        screenCaptureParameters.captureVideo = true;
        screenCaptureParameters.videoCaptureParameters.width = width;
        screenCaptureParameters.videoCaptureParameters.height = height;
        engine.startScreenCapture(screenCaptureParameters);


        LocalTranscoderConfiguration config = new LocalTranscoderConfiguration();
        config.videoOutputConfiguration = new VideoEncoderConfiguration(
                new VideoEncoderConfiguration.VideoDimensions(width, height),
                VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_24,
                STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_ADAPTIVE
        );
        config.transcodingVideoStreams = new ArrayList<>();


        LocalTranscoderConfiguration.TranscodingVideoStream screenStream = new LocalTranscoderConfiguration.TranscodingVideoStream();
        screenStream.sourceType = Constants.VideoSourceType.VIDEO_SOURCE_SCREEN_PRIMARY;
        screenStream.width = width;
        screenStream.height = height;
        screenStream.zOrder = 1;
        config.transcodingVideoStreams.add(screenStream);

        LocalTranscoderConfiguration.TranscodingVideoStream cameraStream = new LocalTranscoderConfiguration.TranscodingVideoStream();
        cameraStream.sourceType = Constants.VideoSourceType.VIDEO_SOURCE_CAMERA_PRIMARY;
        cameraStream.width = width / 2;
        cameraStream.height = height / 2;
        cameraStream.x = 0;
        cameraStream.y = height / 2;
        cameraStream.zOrder = 2;
        cameraStream.mirror = true;
        config.transcodingVideoStreams.add(cameraStream);

        engine.startLocalVideoTranscoder(config);

        // Create render view by RtcEngine
        SurfaceView surfaceView = new SurfaceView(context);
        if (videoReportLayout.getChildCount() > 0) {
            videoReportLayout.removeAllViews();
        }
        // Setup local video to render your local camera preview
        VideoCanvas local = new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, 0);
        local.sourceType = Constants.VideoSourceType.VIDEO_SOURCE_TRANSCODED.getValue();
        local.mirrorMode = Constants.VIDEO_MIRROR_MODE_DISABLED;
        engine.setupLocalVideo(local);
        // Add to the local container
        videoReportLayout.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));

        engine.startPreview(Constants.VideoSourceType.VIDEO_SOURCE_TRANSCODED);

        ChannelMediaOptions option = new ChannelMediaOptions();
        option.autoSubscribeAudio = true;
        option.autoSubscribeVideo = true;
        option.publishMicrophoneTrack = true;
        option.publishTranscodedVideoTrack = true;

        /*Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        TokenUtils.gen(requireContext(), channelId, 0, ret -> {

            /* Allows a user to join a channel.
             if you do not specify the uid, we will generate the uid for you*/
            int res = engine.joinChannel(ret, channelId, 0, option);
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
        @Override
        public void onLocalVideoTranscoderError(LocalTranscoderConfiguration.TranscodingVideoStream stream, int error) {
            super.onLocalVideoTranscoderError(stream, error);
            Log.i(TAG, "LocalVideoTranscoding -- onLocalVideoTranscoderError stream=" + stream + ", error=" + error);
        }

        /**
         * Error code description can be found at:
         * en: https://api-ref.agora.io/en/video-sdk/android/4.x/API/class_irtcengineeventhandler.html#callback_irtcengineeventhandler_onerror
         * cn: https://docs.agora.io/cn/video-call-4.x/API%20Reference/java_ng/API/class_irtcengineeventhandler.html#callback_irtcengineeventhandler_onerror
         */
        @Override
        public void onError(int err) {
            super.onError(err);
            showLongToast("Error code:" + err + ", msg:" + RtcEngine.getErrorDescription(err));
            if (err == Constants.ERR_INVALID_TOKEN || err == Constants.ERR_TOKEN_EXPIRED) {
                engine.leaveChannel();
                runOnUIThread(() -> join.setEnabled(true));

                if (Constants.ERR_INVALID_TOKEN == err) {
                    showAlert(getString(R.string.token_invalid));
                }
                if (Constants.ERR_TOKEN_EXPIRED == err) {
                    showAlert(getString(R.string.token_expired));
                }
            }
        }

        /**Occurs when a user leaves the channel.
         * @param stats With this callback, the application retrieves the channel information,
         *              such as the call duration and statistics.*/
        @Override
        public void onLeaveChannel(RtcStats stats) {
            super.onLeaveChannel(stats);
            Log.i(TAG, String.format("local user %d leaveChannel!", myUid));
            showLongToast(String.format("local user %d leaveChannel!", myUid));
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
            handler.post(new Runnable() {
                @Override
                public void run() {
                    join.setEnabled(true);
                    join.setText(getString(R.string.leave));
                    videoReportLayout.setReportUid(uid);
                }
            });
        }

        /**Since v2.9.0.
         * This callback indicates the state change of the remote audio stream.
         * PS: This callback does not work properly when the number of users (in the Communication profile) or
         *     broadcasters (in the Live-broadcast profile) in the channel exceeds 17.
         * @param uid ID of the user whose audio state changes.
         * @param state State of the remote audio
         *   REMOTE_AUDIO_STATE_STOPPED(0): The remote audio is in the default state, probably due
         *              to REMOTE_AUDIO_REASON_LOCAL_MUTED(3), REMOTE_AUDIO_REASON_REMOTE_MUTED(5),
         *              or REMOTE_AUDIO_REASON_REMOTE_OFFLINE(7).
         *   REMOTE_AUDIO_STATE_STARTING(1): The first remote audio packet is received.
         *   REMOTE_AUDIO_STATE_DECODING(2): The remote audio stream is decoded and plays normally,
         *              probably due to REMOTE_AUDIO_REASON_NETWORK_RECOVERY(2),
         *              REMOTE_AUDIO_REASON_LOCAL_UNMUTED(4) or REMOTE_AUDIO_REASON_REMOTE_UNMUTED(6).
         *   REMOTE_AUDIO_STATE_FROZEN(3): The remote audio is frozen, probably due to
         *              REMOTE_AUDIO_REASON_NETWORK_CONGESTION(1).
         *   REMOTE_AUDIO_STATE_FAILED(4): The remote audio fails to start, probably due to
         *              REMOTE_AUDIO_REASON_INTERNAL(0).
         * @param reason The reason of the remote audio state change.
         *   REMOTE_AUDIO_REASON_INTERNAL(0): Internal reasons.
         *   REMOTE_AUDIO_REASON_NETWORK_CONGESTION(1): Network congestion.
         *   REMOTE_AUDIO_REASON_NETWORK_RECOVERY(2): Network recovery.
         *   REMOTE_AUDIO_REASON_LOCAL_MUTED(3): The local user stops receiving the remote audio
         *               stream or disables the audio module.
         *   REMOTE_AUDIO_REASON_LOCAL_UNMUTED(4): The local user resumes receiving the remote audio
         *              stream or enables the audio module.
         *   REMOTE_AUDIO_REASON_REMOTE_MUTED(5): The remote user stops sending the audio stream or
         *               disables the audio module.
         *   REMOTE_AUDIO_REASON_REMOTE_UNMUTED(6): The remote user resumes sending the audio stream
         *              or enables the audio module.
         *   REMOTE_AUDIO_REASON_REMOTE_OFFLINE(7): The remote user leaves the channel.
         * @param elapsed Time elapsed (ms) from the local user calling the joinChannel method
         *                  until the SDK triggers this callback.*/
        @Override
        public void onRemoteAudioStateChanged(int uid, int state, int reason, int elapsed) {
            super.onRemoteAudioStateChanged(uid, state, reason, elapsed);
            Log.i(TAG, "onRemoteAudioStateChanged->" + uid + ", state->" + state + ", reason->" + reason);
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
            Log.i(TAG, "onRemoteVideoStateChanged->" + uid + ", state->" + state + ", reason->" + reason);
        }

        @Override
        public void onLocalAudioStats(LocalAudioStats stats) {
            super.onLocalAudioStats(stats);
            videoReportLayout.setLocalAudioStats(stats);
        }


        @Override
        public void onLocalVideoStats(Constants.VideoSourceType source, LocalVideoStats stats) {
            super.onLocalVideoStats(source, stats);
            videoReportLayout.setLocalVideoStats(stats);
        }

    };

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        if (buttonView == switchTransparentBackground) {
            engine.enableVirtualBackground(isChecked,
                    new VirtualBackgroundSource(VirtualBackgroundSource.BACKGROUND_COLOR,
                            0x00000000, "",
                            VirtualBackgroundSource.BLUR_DEGREE_HIGH),
                    new SegmentationProperty());
        }
    }
}
