package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc2.Constants.MediaSourceType.PRIMARY_CAMERA_SOURCE;
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

import java.util.Locale;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.utils.CommonUtil;
import io.agora.api.example.utils.TokenUtils;
import io.agora.base.VideoFrame;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.ExtensionContext;
import io.agora.rtc2.IMediaExtensionObserver;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;
import io.agora.rtc2.video.IFaceInfoObserver;
import io.agora.rtc2.video.IVideoFrameObserver;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoEncoderConfiguration;

/**
 * The type Process raw data.
 */
@Example(
        index = 12,
        group = ADVANCED,
        name = R.string.item_face_capture,
        actionId = R.id.action_mainFragment_to_face_capture,
        tipsId = R.string.face_capture_tip
)
public class FaceCapture extends BaseFragment implements View.OnClickListener {
    private static final String TAG = FaceCapture.class.getSimpleName();

    private static final String AUTHENTICATION = "i1RXlMcDyB5F6Yn9aPijy1MouDxdsI4ajOj+NrHsc99FntK1drIlJNWdodNzBjAeH8cSUSuRkX6LwoE0WDgEAnivcXbsYv8atcZYZFl4FoXONgI6uIOmi+r5SGjsgZe+yI6wdh5eW1pLLh6m/W02M35EX1wtBvP3l79q3KeGsDQ=";

    private FrameLayout fl_local;
    private Button join;
    private EditText et_channel, et_capture_info;
    private RtcEngine engine;
    private int myUid;
    private boolean joined = false;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
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
            config.mAreaCode = ((MainApplication) requireActivity().getApplication()).getGlobalSettings().getAreaCode();
            /* For Android, the agora_face_capture_extension will not load default. You must add it manually. */
            config.addExtension("agora_face_capture_extension");
            config.addExtension("agora_lip_sync_extension");
            /* Config extension observer to receive the events. */
            config.mExtensionObserver = iMediaExtensionObserver;
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
            LocalAccessPointConfiguration localAccessPointConfiguration = ((MainApplication) requireActivity().getApplication()).getGlobalSettings().getPrivateCloudConfig();
            if (localAccessPointConfiguration != null) {
                // This api can only be used in the private media server scenario, otherwise some problems may occur.
                engine.setLocalAccessPoint(localAccessPointConfiguration);
            }

            engine.registerVideoFrameObserver(iVideoFrameObserver);
            engine.registerFaceInfoObserver(iFaceInfoObserver);
            engine.enableExtension("agora_video_filters_face_capture", "face_capture", true, PRIMARY_CAMERA_SOURCE);
            engine.setExtensionProperty("agora_video_filters_face_capture",
                    "face_capture",
                    "authentication_information",
                    "{\"company_id\":\"agoraTest\","
                            + "\"license\":\""
                            + AUTHENTICATION
                            + "\"}"
            );

            engine.enableExtension("agora_filters_lip_sync", "lip_sync", true, Constants.MediaSourceType.SPEECH_DRIVEN_VIDEO_SOURCE);
            engine.setExtensionProperty("agora_filters_lip_sync","lip_sync", "parameters",
                    "{\"company_id\":\"agoraTest\""
                            + ",\"license\":\"" + AUTHENTICATION + "\""
                            + ",\"open_agc\":true"
                            + "}",
                    Constants.MediaSourceType.SPEECH_DRIVEN_VIDEO_SOURCE);

        } catch (Exception e) {
            e.printStackTrace();
            requireActivity().onBackPressed();
        }
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_face_capture, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        join = view.findViewById(R.id.btn_join);
        et_channel = view.findViewById(R.id.et_channel);
        join.setOnClickListener(this);
        fl_local = view.findViewById(R.id.fl_local);
        et_capture_info = view.findViewById(R.id.et_capture_info);
    }


    @Override
    public void onDestroy() {
        /*leaveChannel and Destroy the RtcEngine instance*/
        if (engine != null) {
            engine.leaveChannel();
            engine.stopPreview();
        }
        engine = null;
        super.onDestroy();
        handler.post(RtcEngine::destroy);
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_join) {
            if (!joined) {
                CommonUtil.hideInputBoard(requireActivity(), et_channel);
                // call when join button hit
                String channelId = et_channel.getText().toString();
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
                engine.stopPreview();
                join.setText(getString(R.string.join));
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
        // Add to the local container
        fl_local.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        // Setup local video to render your local camera preview
        engine.setupLocalVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, 0));

        /*In the demo, the default is to enter as the anchor.*/
        engine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
        // Setup video encoding configs
        engine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                ((MainApplication) requireActivity().getApplication()).getGlobalSettings().getVideoEncodingDimensionObject(),
                VideoEncoderConfiguration.FRAME_RATE.valueOf(((MainApplication) requireActivity().getApplication()).getGlobalSettings().getVideoEncodingFrameRate()),
                STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.valueOf(((MainApplication) requireActivity().getApplication()).getGlobalSettings().getVideoEncodingOrientation())
        ));
        /*Set up to play remote sound with receiver*/
        engine.setDefaultAudioRoutetoSpeakerphone(true);

        engine.enableVideo();

        engine.startPreview();

        /*Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        TokenUtils.gen(requireContext(), channelId, 0, token -> {
            /* Allows a user to join a channel.
             if you do not specify the uid, we will generate the uid for you*/

            ChannelMediaOptions option = new ChannelMediaOptions();
            option.autoSubscribeAudio = true;
            option.autoSubscribeVideo = true;
            option.publishCameraTrack = true;
            int res = engine.joinChannel(token, channelId, 0, option);
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

    private final IVideoFrameObserver iVideoFrameObserver = new IVideoFrameObserver() {
        @Override
        public boolean onRenderVideoFrame(String channelId, int uid, VideoFrame videoFrame) {
            return false;
        }

        @Override
        public boolean onCaptureVideoFrame(int sourceType, VideoFrame videoFrame) {
            Log.i(TAG, String.format(Locale.US, "VideoFrameObserver >> onCaptureVideoFrame : metadata=%s",
                    videoFrame.getMetaInfo().toString()));
            // runOnUIThread(() -> et_capture_info.setText(videoFrame.getMetaInfo().toString()));
            return true;
        }

        @Override
        public boolean onPreEncodeVideoFrame(int sourceType, VideoFrame videoFrame) {
            return false;
        }

        @Override
        public boolean onMediaPlayerVideoFrame(VideoFrame videoFrame, int mediaPlayerId) {
            return false;
        }

        @Override
        public int getVideoFrameProcessMode() {
            return PROCESS_MODE_READ_ONLY;
        }

        @Override
        public int getVideoFormatPreference() {
            return VIDEO_PIXEL_DEFAULT;
        }

        @Override
        public boolean getRotationApplied() {
            return false;
        }

        @Override
        public boolean getMirrorApplied() {
            return false;
        }

        @Override
        public int getObservedFramePosition() {
            return POSITION_POST_CAPTURER;
        }
    };

    private final IMediaExtensionObserver iMediaExtensionObserver = new IMediaExtensionObserver() {
        @Override
        public void onEventWithContext(ExtensionContext extContext, String key, String value) {
            String provider = extContext.providerName;
            String extension = extContext.extensionName;
            Log.i(TAG, String.format(Locale.US, "ExtensionObserver >> onEvent : provider=%s, extension=%s, key=%s, value=%s",
                    provider, extension, key, value));
            if ("agora_video_filters_face_capture".equals(provider)
                    && "face_capture".equals(extension)
                    && "authentication_state".equals(key)) {
                if ("0".equals(value)) {
                    showShortToast("Face capture authentication successful.");
                } else if ("-1".equals(value)) {
                    showShortToast("Face capture authentication failed!");
                } else if ("-2".equals(value)) {
                    showShortToast("Face capture authentication information not set!");
                    showAlert(getString(R.string.face_capture_authentication), false);
                }
            } else if ("agora_filters_lip_sync".equals(provider)
                    && "lip_sync".equals(extension)
                    && "status_code".equals(key)) {
                if ("0".equals(value)) {
                    showShortToast("Speech driven authentication successful.");
                } else {
                    showShortToast("Speech driven authentication failed. code=" + value);
                }
            }
        }

        @Override
        public void onStartedWithContext(ExtensionContext extContext) {
            String provider = extContext.providerName;
            String extension = extContext.extensionName;
            Log.i(TAG, String.format(Locale.US, "ExtensionObserver >> onStarted : provider=%s, extension=%s",
                    provider, extension));
        }

        @Override
        public void onStoppedWithContext(ExtensionContext extContext) {
            String provider = extContext.providerName;
            String extension = extContext.extensionName;
            Log.i(TAG, String.format(Locale.US, "ExtensionObserver >> onStopped : provider=%s, extension=%s",
                    provider, extension));
        }

        @Override
        public void onErrorWithContext(ExtensionContext extContext, int error, String message) {
            String provider = extContext.providerName;
            String extension = extContext.extensionName;
            Log.i(TAG, String.format(Locale.US, "ExtensionObserver >> onError : provider=%s, extension=%s, error=%d, message=%s",
                    provider, extension, error, message));
        }
    };

    private final IFaceInfoObserver iFaceInfoObserver = new IFaceInfoObserver() {
        @Override
        public boolean onFaceInfo(String outFaceInfo) {
            runOnUIThread(() -> et_capture_info.setText(outFaceInfo));
            return false;
        }
    };

    /**
     * IRtcEngineEventHandler is an abstract class providing default implementation.
     * The SDK uses this class to report to the app on SDK runtime events.
     */
    private final IRtcEngineEventHandler iRtcEngineEventHandler = new IRtcEngineEventHandler() {
        /**
         * Error code description can be found at:
         * en: <a href="https://api-ref.agora.io/en/video-sdk/android/4.x/API/class_irtcengineeventhandler.html#callback_irtcengineeventhandler_onerror">...</a>
         * cn: <a href="https://docs.agora.io/cn/video-call-4.x/API%20Reference/java_ng/API/class_irtcengineeventhandler.html#callback_irtcengineeventhandler_onerror">...</a>
         */
        @Override
        public void onError(int err) {
            Log.w(TAG, String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
        }


        /**Occurs when a user leaves the channel.
         * @param stats With this callback, the application retrieves the channel information,
         *              such as the call duration and statistics.*/
        @Override
        public void onLeaveChannel(RtcStats stats) {
            super.onLeaveChannel(stats);
            Log.i(TAG, String.format("local user %d leaveChannel!", myUid));
            showLongToast(String.format(Locale.US, "local user %d leaveChannel!", myUid));
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
            showLongToast(String.format(Locale.US, "onJoinChannelSuccess channel %s uid %d", channel, uid));
            myUid = uid;
            joined = true;
            handler.post(new Runnable() {
                @Override
                public void run() {
                    join.setEnabled(true);
                    join.setText(getString(R.string.leave));
                }
            });
        }

        /**Occurs when a remote user (Communication)/host (Live Broadcast) joins the channel.
         * @param uid ID of the user whose audio state changes.
         * @param elapsed Time delay (ms) from the local user calling joinChannel/setClientRole
         *                until this callback is triggered.*/
        @Override
        public void onUserJoined(int uid, int elapsed) {
            super.onUserJoined(uid, elapsed);
            Log.i(TAG, "onUserJoined->" + uid);
            showLongToast(String.format(Locale.US, "user %d joined!", uid));
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
            showLongToast(String.format(Locale.US, "user %d offline! reason:%d", uid, reason));
        }
    };

}
