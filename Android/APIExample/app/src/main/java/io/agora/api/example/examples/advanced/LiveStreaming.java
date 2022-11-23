package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc2.Constants.CLIENT_ROLE_AUDIENCE;
import static io.agora.rtc2.video.VideoCanvas.RENDER_MODE_HIDDEN;
import static io.agora.rtc2.video.VideoEncoderConfiguration.STANDARD_BITRATE;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.Spinner;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.widget.SwitchCompat;

import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;

import java.util.Locale;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.Constant;
import io.agora.api.example.common.widget.VideoReportLayout;
import io.agora.api.example.utils.CommonUtil;
import io.agora.api.example.utils.TokenUtils;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.ClientRoleOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoEncoderConfiguration;
import io.agora.rtc2.video.WatermarkOptions;

/**
 * This demo demonstrates how to make a one-to-one video call
 * <p>
 * By default, Everyone is a host, entered a channel will see yourself in the background( the big one ).
 * click the frame will switch the position.
 * When turn the Co-host on, others will see you.
 */
@Example(
        index = 0,
        group = ADVANCED,
        name = R.string.item_livestreaming,
        actionId = R.id.action_mainFragment_to_live_streaming,
        tipsId = R.string.livestreaming
)
public class LiveStreaming extends BaseFragment implements View.OnClickListener {
    private static final String TAG = LiveStreaming.class.getSimpleName();

    private VideoReportLayout foreGroundVideo, backGroundVideo;
    private Button join, publish, latency;
    private EditText et_channel;
    private RtcEngine engine;
    private int myUid;
    private int remoteUid;
    private boolean joined = false;
    private boolean isHost = false;
    private boolean isLowLatency = false;
    private boolean isLocalVideoForeground = true;
    private SwitchCompat watermarkSwitch;
    private SwitchCompat lowStreamSwitch;
    private Spinner spEncoderType;
    private SwitchCompat bFrame;

    private final VideoEncoderConfiguration videoEncoderConfiguration = new VideoEncoderConfiguration();

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_live_streaming, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        join = view.findViewById(R.id.btn_join);
        publish = view.findViewById(R.id.btn_publish);
        latency = view.findViewById(R.id.btn_latency);
        et_channel = view.findViewById(R.id.et_channel);
        spEncoderType = view.findViewById(R.id.sp_encoder_type);
        bFrame = view.findViewById(R.id.switch_b_frame);
        latency.setEnabled(false);
        publish.setEnabled(false);
        view.findViewById(R.id.btn_join).setOnClickListener(this);
        view.findViewById(R.id.btn_publish).setOnClickListener(this);
        view.findViewById(R.id.btn_latency).setOnClickListener(this);
        view.findViewById(R.id.foreground_video).setOnClickListener(this);
        foreGroundVideo = view.findViewById(R.id.background_video);
        backGroundVideo = view.findViewById(R.id.foreground_video);
        view.findViewById(R.id.btn_take_shot).setOnClickListener(this);
        watermarkSwitch = view.findViewById(R.id.switch_watermark);
        watermarkSwitch.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked) {
                engine.enableVideo();
                WatermarkOptions watermarkOptions = new WatermarkOptions();
                int size = ((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingDimensionObject().width / 6;
                int height = ((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingDimensionObject().height;
                watermarkOptions.positionInPortraitMode = new WatermarkOptions.Rectangle(10, height / 2, size, size);
                watermarkOptions.positionInLandscapeMode = new WatermarkOptions.Rectangle(10, height / 2, size, size);
                watermarkOptions.visibleInPreview = true;
                int ret = engine.addVideoWatermark(Constant.WATER_MARK_FILE_PATH, watermarkOptions);
                if (ret != Constants.ERR_OK) {
                    Log.e(TAG, "addVideoWatermark error=" + ret + ", msg=" + RtcEngine.getErrorDescription(ret));
                }
            } else {
                engine.clearVideoWatermarks();
            }
        });
        lowStreamSwitch = view.findViewById(R.id.switch_low_stream);
        lowStreamSwitch.setEnabled(false);
        lowStreamSwitch.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if(remoteUid != 0){
                engine.setRemoteVideoStreamType(remoteUid, isChecked ? Constants.VIDEO_STREAM_LOW: Constants.VIDEO_STREAM_HIGH);
            }
        });

        bFrame.setOnCheckedChangeListener((buttonView, isChecked) -> {
            videoEncoderConfiguration.compressionPrefer  = isChecked ?
                    VideoEncoderConfiguration.COMPRESSION_PREFERENCE.PREFER_QUALITY :
                    VideoEncoderConfiguration.COMPRESSION_PREFERENCE.PREFER_LOW_LATENCY;
            engine.setVideoEncoderConfiguration(videoEncoderConfiguration);
        });

        spEncoderType.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                VideoEncoderConfiguration.AdvanceOptions advanceOptions = new VideoEncoderConfiguration.AdvanceOptions();
                if(position == 1){
                    advanceOptions.encodingPreference = VideoEncoderConfiguration.ENCODING_PREFERENCE.PREFER_HARDWARE;
                }else if(position == 2){
                    advanceOptions.encodingPreference = VideoEncoderConfiguration.ENCODING_PREFERENCE.PREFER_SOFTWARE;
                }else{
                    advanceOptions.encodingPreference = VideoEncoderConfiguration.ENCODING_PREFERENCE.PREFER_AUTO;
                }
                videoEncoderConfiguration.advanceOptions = advanceOptions;
                engine.setVideoEncoderConfiguration(videoEncoderConfiguration);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });
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

            /**Creates an RtcEngine instance.
             * @param context The context of Android Activity
             * @param appId The App ID issued to you by Agora. See <a href="https://docs.agora.io/en/Agora%20Platform/token#get-an-app-id">
             *              How to get the App ID</a>
             * @param handler IRtcEngineEventHandler is an abstract class providing default implementation.
             *                The SDK uses this class to report to the app on SDK runtime events.*/
            RtcEngineConfig rtcEngineConfig = new RtcEngineConfig();
            rtcEngineConfig.mAppId = getString(R.string.agora_app_id);
            rtcEngineConfig.mContext = context.getApplicationContext();
            rtcEngineConfig.mEventHandler = iRtcEngineEventHandler;
            /** Sets the channel profile of the Agora RtcEngine.
             */
            rtcEngineConfig.mChannelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING;
            rtcEngineConfig.mAudioScenario = Constants.AudioScenario.getValue(Constants.AudioScenario.DEFAULT);
            rtcEngineConfig.mAreaCode = ((MainApplication)getActivity().getApplication()).getGlobalSettings().getAreaCode();
            engine = RtcEngine.create(rtcEngineConfig);
            /**
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
            engine.setLocalAccessPoint(((MainApplication) getActivity().getApplication()).getGlobalSettings().getPrivateCloudConfig());

            engine.setVideoEncoderConfiguration(videoEncoderConfiguration);
            engine.enableDualStreamMode(true);
        } catch (Exception e) {
            requireActivity().onBackPressed();
            e.printStackTrace();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        /*leaveChannel and Destroy the RtcEngine instance*/
        if (engine != null) {
            engine.leaveChannel();
        }
        handler.post(RtcEngine::destroy);
        engine = null;
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_join) {
            if (!joined) {
                CommonUtil.hideInputBoard(requireActivity(), et_channel);
                // call when join button hit
                String channelId = et_channel.getText().toString();
                // Check permission
                if (AndPermission.hasPermissions(this, Permission.Group.STORAGE, Permission.Group.MICROPHONE, Permission.Group.CAMERA)) {
                    joinChannel(channelId);
                    return;
                }
                // Request permission
                AndPermission.with(this).runtime().permission(
                        Permission.Group.STORAGE,
                        Permission.Group.MICROPHONE,
                        Permission.Group.CAMERA
                ).onGranted(permissions ->
                {
                    // Permissions Granted
                    joinChannel(channelId);
                }).start();
            } else {
                joined = false;
                /**After joining a channel, the user must call the leaveChannel method to end the
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
                engine.stopPreview();
                engine.leaveChannel();
                join.setText(getString(R.string.join));
                watermarkSwitch.setEnabled(false);
                publish.setEnabled(false);
                latency.setEnabled(false);
                lowStreamSwitch.setChecked(false);
                lowStreamSwitch.setEnabled(false);
                remoteUid = 0;
            }
        } else if (v.getId() == R.id.btn_publish) {
            isHost = !isHost;
            if(isHost){
                engine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
                latency.setEnabled(false);
            }
            else{
                ClientRoleOptions clientRoleOptions = new ClientRoleOptions();
                clientRoleOptions.audienceLatencyLevel = isLowLatency ? Constants.AUDIENCE_LATENCY_LEVEL_ULTRA_LOW_LATENCY : Constants.AUDIENCE_LATENCY_LEVEL_LOW_LATENCY;
                engine.setClientRole(CLIENT_ROLE_AUDIENCE, clientRoleOptions);
                latency.setEnabled(true);
            }
            publish.setEnabled(false);
            publish.setText(isHost ? getString(R.string.disnable_publish) : getString(R.string.enable_publish));

        } else if (v.getId() == R.id.btn_latency) {
            isLowLatency = !isLowLatency;
            latency.setText(isLowLatency ? getString(R.string.disable_low_latency) : getString(R.string.enable_low_latency));
            ClientRoleOptions clientRoleOptions = new ClientRoleOptions();
            clientRoleOptions.audienceLatencyLevel = isLowLatency ? Constants.AUDIENCE_LATENCY_LEVEL_ULTRA_LOW_LATENCY : Constants.AUDIENCE_LATENCY_LEVEL_LOW_LATENCY;
            engine.setClientRole(CLIENT_ROLE_AUDIENCE, clientRoleOptions);
        } else if (v.getId() == R.id.foreground_video) {
            isLocalVideoForeground = !isLocalVideoForeground;
            int foreGroundReportId = foreGroundVideo.getReportUid();
            foreGroundVideo.setReportUid(backGroundVideo.getReportUid());
            backGroundVideo.setReportUid(foreGroundReportId);

            if (foreGroundVideo.getChildCount() > 0) {
                foreGroundVideo.removeAllViews();
            }
            if (backGroundVideo.getChildCount() > 0) {
                backGroundVideo.removeAllViews();
            }
            // Create render view by RtcEngine
            SurfaceView localView = RtcEngine.CreateRendererView(getContext());
            SurfaceView remoteView = RtcEngine.CreateRendererView(getContext());
            if (isLocalVideoForeground){
                // Add to the local container
                foreGroundVideo.addView(localView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
                // Add to the remote container
                backGroundVideo.addView(remoteView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
                // Setup remote video to render
                engine.setupRemoteVideo(new VideoCanvas(remoteView, RENDER_MODE_HIDDEN, remoteUid));
                // Setup local video to render your local camera preview
                engine.setupLocalVideo(new VideoCanvas(localView, RENDER_MODE_HIDDEN, 0));
                remoteView.setZOrderMediaOverlay(true);
                remoteView.setZOrderOnTop(true);
            }
            else{
                // Add to the local container
                foreGroundVideo.addView(remoteView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
                // Add to the remote container
                backGroundVideo.addView(localView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
                // Setup local video to render your local camera preview
                engine.setupLocalVideo(new VideoCanvas(localView, RENDER_MODE_HIDDEN, 0));
                // Setup remote video to render
                engine.setupRemoteVideo(new VideoCanvas(remoteView, RENDER_MODE_HIDDEN, remoteUid));
                localView.setZOrderMediaOverlay(true);
                localView.setZOrderOnTop(true);
            }
        } else if (v.getId() == R.id.btn_take_shot) {
            if (remoteUid != 0) {
                int ret = engine.takeSnapshot(remoteUid, "/sdcard/APIExample_snapshot_" + et_channel.getText().toString() + "_" + remoteUid + ".png");
                if(ret != Constants.ERR_OK){
                    showLongToast("takeSnapshot error code=" + ret + ",msg=" + RtcEngine.getErrorDescription(ret));
                }
            } else {
                showLongToast(getString(R.string.remote_screenshot_tip));
            }
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
        if (foreGroundVideo.getChildCount() > 0) {
            foreGroundVideo.removeAllViews();
        }
        // Add to the local container
        foreGroundVideo.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        // Setup local video to render your local camera preview
        engine.setupLocalVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, 0));
        // Set audio route to microPhone
        engine.setDefaultAudioRoutetoSpeakerphone(false);
        engine.startPreview();
        // Set audio route to microPhone
        engine.setDefaultAudioRoutetoSpeakerphone(true);

        /**In the demo, the default is to enter as the anchor.*/
        engine.setClientRole(IRtcEngineEventHandler.ClientRole.CLIENT_ROLE_AUDIENCE);
        // Enable video module
        engine.enableVideo();
        // Setup video encoding configs
        engine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                ((MainApplication)getActivity().getApplication()).getGlobalSettings().getVideoEncodingDimensionObject(),
                VideoEncoderConfiguration.FRAME_RATE.valueOf(((MainApplication)getActivity().getApplication()).getGlobalSettings().getVideoEncodingFrameRate()),
                STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.valueOf(((MainApplication)getActivity().getApplication()).getGlobalSettings().getVideoEncodingOrientation())
        ));

        /**Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        TokenUtils.gen(requireContext(), channelId, 0, ret -> {
            /** Allows a user to join a channel.
             if you do not specify the uid, we will generate the uid for you*/

            ChannelMediaOptions option = new ChannelMediaOptions();
            option.autoSubscribeAudio = true;
            option.autoSubscribeVideo = true;
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

        /**Occurs when a user leaves the channel.
         * @param stats With this callback, the application retrieves the channel information,
         *              such as the call duration and statistics.*/
        @Override
        public void onLeaveChannel(RtcStats stats) {
            super.onLeaveChannel(stats);
            Log.i(TAG, String.format("local user %d leaveChannel!", myUid));
            showLongToast(String.format("local user %d leaveChannel!", myUid));
            runOnUIThread(() -> lowStreamSwitch.setEnabled(false));
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
                    publish.setEnabled(true);
                    latency.setEnabled(true);
                    foreGroundVideo.setReportUid(uid);
                    lowStreamSwitch.setEnabled(true);
                }
            });
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

        /**Occurs when a remote user (Communication)/host (Live Broadcast) joins the channel.
         * @param uid ID of the user whose audio state changes.
         * @param elapsed Time delay (ms) from the local user calling joinChannel/setClientRole
         *                until this callback is triggered.*/
        @Override
        public void onUserJoined(int uid, int elapsed) {
            super.onUserJoined(uid, elapsed);
            Log.i(TAG, "onUserJoined->" + uid);
            showLongToast(String.format("user %d joined!", uid));
            /**Check if the context is correct*/
            Context context = getContext();
            if (context == null) {
                return;
            }
            if(remoteUid != 0) {
                return;
            }
            else{
                remoteUid = uid;
            }
            handler.post(() ->
            {
                /**Display remote video stream*/
                SurfaceView surfaceView = null;
                if (backGroundVideo.getChildCount() > 0) {
                    backGroundVideo.removeAllViews();
                }
                // Create render view by RtcEngine
                surfaceView = new SurfaceView(context);
                surfaceView.setZOrderMediaOverlay(true);
                // Add to the remote container
                backGroundVideo.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));

                backGroundVideo.setReportUid(remoteUid);
                // Setup remote video to render
                engine.setupRemoteVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, remoteUid));
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
            if(uid == remoteUid) {
                remoteUid = 0;
                runOnUIThread(new Runnable() {
                    @Override
                    public void run() {
                        /**Clear render view
                         Note: The video will stay at its last frame, to completely remove it you will need to
                         remove the SurfaceView from its parent*/
                        engine.setupRemoteVideo(new VideoCanvas(null, RENDER_MODE_HIDDEN, uid));
                        lowStreamSwitch.setChecked(false);
                        lowStreamSwitch.setEnabled(false);
                    }
                });
            }
        }

        /**
         * Occurs when the user role switches in a live streaming. For example, from a host to an audience or vice versa.
         *
         * The SDK triggers this callback when the local user switches the user role by calling the setClientRole method after joining the channel.
         * @param oldRole Role that the user switches from.
         * @param newRole Role that the user switches to.
         */
        @Override
        public void onClientRoleChanged(int oldRole, int newRole, ClientRoleOptions newRoleOptions) {
            super.onClientRoleChanged(oldRole, newRole, newRoleOptions);
            Log.i(TAG, String.format("client role changed from state %d to %d", oldRole, newRole));
            runOnUIThread(() -> {
                publish.setEnabled(true);
                watermarkSwitch.setEnabled(newRole == Constants.CLIENT_ROLE_BROADCASTER);
            });
        }

        @Override
        public void onSnapshotTaken(int uid, String filePath, int width, int height, int errCode) {
            super.onSnapshotTaken(uid, filePath, width, height, errCode);
            Log.d(TAG, String.format(Locale.US, "onSnapshotTaken uid=%d, filePath=%s, width=%d, height=%d, errorCode=%d", uid, filePath, width, height, errCode));
            if(errCode == 0){
                showLongToast("SnapshotTaken path=" + filePath);
            }else{
                showLongToast("SnapshotTaken error=" + RtcEngine.getErrorDescription(errCode));
            }
        }

        @Override
        public void onLocalVideoStats(Constants.VideoSourceType source, LocalVideoStats stats) {
            super.onLocalVideoStats(source, stats);
            if(isLocalVideoForeground){
                foreGroundVideo.setLocalVideoStats(stats);
            }else{
                backGroundVideo.setLocalVideoStats(stats);
            }
        }

        @Override
        public void onLocalAudioStats(LocalAudioStats stats) {
            super.onLocalAudioStats(stats);
            if(isLocalVideoForeground){
                foreGroundVideo.setLocalAudioStats(stats);
            }else{
                backGroundVideo.setLocalAudioStats(stats);
            }
        }

        @Override
        public void onRemoteVideoStats(RemoteVideoStats stats) {
            super.onRemoteVideoStats(stats);
            if(!isLocalVideoForeground){
                foreGroundVideo.setRemoteVideoStats(stats);
            }else{
                backGroundVideo.setRemoteVideoStats(stats);
            }
        }

        @Override
        public void onRemoteAudioStats(RemoteAudioStats stats) {
            super.onRemoteAudioStats(stats);
            if(!isLocalVideoForeground){
                foreGroundVideo.setRemoteAudioStats(stats);
            }else{
                backGroundVideo.setRemoteAudioStats(stats);
            }
        }
    };
}