package io.agora.api.example.examples.advanced;

import android.content.Context;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.Switch;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.utils.CommonUtil;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.live.LiveTranscoding;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoEncoderConfiguration;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc2.video.VideoCanvas.RENDER_MODE_HIDDEN;
import static io.agora.rtc2.video.VideoEncoderConfiguration.STANDARD_BITRATE;
import static io.agora.rtc2.video.VideoEncoderConfiguration.VD_640x360;

/**
 * This example demonstrates how to push a stream to an external address.
 * <p>
 * Important:
 * Users who push and pull streams cannot be in one channel,
 * otherwise unexpected errors will occur.
 */
@Example(
        index = 3,
        group = ADVANCED,
        name = R.string.item_rtmpstreaming,
        actionId = R.id.action_mainFragment_to_RTMPStreaming,
        tipsId = R.string.rtmpstreaming
)
public class RTMPStreaming extends BaseFragment implements View.OnClickListener
{
    private static final String TAG = RTMPStreaming.class.getSimpleName();

    private LinearLayout llTransCode;
    private Switch transCodeSwitch;
    private FrameLayout fl_local, fl_remote;
    private EditText et_url, et_channel;
    private Button join, publish;
    private RtcEngine engine;
    private int myUid;
    private boolean joined = false, publishing = false;
    private VideoEncoderConfiguration.VideoDimensions dimensions = VD_640x360;
    private LiveTranscoding transcoding;
    /**
     * Maximum number of users participating in transcoding (even number)
     */
    private final int MAXUserCount = 2;
    private LiveTranscoding.TranscodingUser localTranscodingUser;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState)
    {
        View view = inflater.inflate(R.layout.fragment_rtmp_streaming, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState)
    {
        super.onViewCreated(view, savedInstanceState);
        llTransCode = view.findViewById(R.id.ll_TransCode);
        transCodeSwitch = view.findViewById(R.id.transCode_Switch);
        fl_local = view.findViewById(R.id.fl_local);
        fl_remote = view.findViewById(R.id.fl_remote);
        et_channel = view.findViewById(R.id.et_channel);
        et_url = view.findViewById(R.id.et_url);
        join = view.findViewById(R.id.btn_join);
        join.setOnClickListener(this);
        publish = view.findViewById(R.id.btn_publish);
        publish.setOnClickListener(this);
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState)
    {
        super.onActivityCreated(savedInstanceState);
        // Check if the context is valid
        Context context = getContext();
        if (context == null)
        {
            return;
        }
        try
        {
            RtcEngineConfig config = new RtcEngineConfig();
            /**
             * The context of Android Activity
             */
            config.mContext = context.getApplicationContext();
            /**
             * The App ID issued to you by Agora. See <a href="https://docs.agora.io/en/Agora%20Platform/token#get-an-app-id"> How to get the App ID</a>
             */
            config.mAppId = getString(R.string.agora_app_id);
            /** Sets the channel profile of the Agora RtcEngine.
             CHANNEL_PROFILE_COMMUNICATION(0): (Default) The Communication profile.
             Use this profile in one-on-one calls or group calls, where all users can talk freely.
             CHANNEL_PROFILE_LIVE_BROADCASTING(1): The Live-Broadcast profile. Users in a live-broadcast
             channel have a role as either broadcaster or audience. A broadcaster can both send and receive streams;
             an audience can only receive streams.*/
            config.mChannelProfile = Constants.CHANNEL_PROFILE_LIVE_BROADCASTING;
            /**
             * IRtcEngineEventHandler is an abstract class providing default implementation.
             * The SDK uses this class to report to the app on SDK runtime events.
             */
            config.mEventHandler = iRtcEngineEventHandler;
            config.mAudioScenario = Constants.AudioScenario.getValue(Constants.AudioScenario.HIGH_DEFINITION);
            engine = RtcEngine.create(config);
        }
        catch (Exception e)
        {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
        /**leaveChannel and Destroy the RtcEngine instance*/
        if(engine != null)
        {
            engine.leaveChannel();
            engine.stopPreview();
        }
        handler.post(RtcEngine::destroy);
        engine = null;
    }

    @Override
    public void onClick(View v)
    {

        if (v.getId() == R.id.btn_join)
        {
            if(!joined)
            {
                CommonUtil.hideInputBoard(getActivity(), et_channel);
                // call when join button hit
                String channelId = et_channel.getText().toString();
                // Check permission
                if (AndPermission.hasPermissions(this, Permission.Group.STORAGE, Permission.Group.MICROPHONE, Permission.Group.CAMERA))
                {
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
            }
            else
            {
                engine.leaveChannel();
                transCodeSwitch.setEnabled(true);
                engine.stopPreview();
                joined = false;
                join.setText(getString(R.string.join));
                publishing = false;
                publish.setEnabled(false);
                publish.setText(getString(R.string.publish));
            }
        }
        else if (v.getId() == R.id.btn_publish)
        {
            /**Ensure that the user joins a channel before calling this method.*/
            if(joined && !publishing)
            {
                startPublish();
            }
            else if(joined && publishing)
            {
                stopPublish();
            }
        }
    }

    private void joinChannel(String channelId)
    {
        // Check if the context is valid
        Context context = getContext();
        if (context == null)
        {
            return;
        }

        // Create render view by RtcEngine
        SurfaceView surfaceView = RtcEngine.CreateRendererView(context);
        // Add to the local container
        fl_local.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        // Setup local video to render your local camera preview
        engine.setupLocalVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, 0));

        /** Sets the channel profile of the Agora RtcEngine.
         CHANNEL_PROFILE_COMMUNICATION(0): (Default) The Communication profile.
         Use this profile in one-on-one calls or group calls, where all users can talk freely.
         CHANNEL_PROFILE_LIVE_BROADCASTING(1): The Live-Broadcast profile. Users in a live-broadcast
         channel have a role as either broadcaster or audience. A broadcaster can both send and receive streams;
         an audience can only receive streams.*/
        engine.setChannelProfile(Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);
        /**In the demo, the default is to enter as the anchor.*/
        engine.setClientRole(IRtcEngineEventHandler.ClientRole.CLIENT_ROLE_BROADCASTER);
        // Enable video module
        engine.enableVideo();
        // Setup video encoding configs
        engine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                ((MainApplication)getActivity().getApplication()).getGlobalSettings().getVideoEncodingDimensionObject(),
                VideoEncoderConfiguration.FRAME_RATE.valueOf(((MainApplication)getActivity().getApplication()).getGlobalSettings().getVideoEncodingFrameRate()),
                STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.valueOf(((MainApplication)getActivity().getApplication()).getGlobalSettings().getVideoEncodingOrientation())
        ));
        /**Set up to play remote sound with receiver*/
        engine.setDefaultAudioRoutetoSpeakerphone(true);
        engine.setEnableSpeakerphone(true);

        /**Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        String accessToken = getString(R.string.agora_access_token);
        if (TextUtils.equals(accessToken, "") || TextUtils.equals(accessToken, "<#YOUR ACCESS TOKEN#>"))
        {
            accessToken = null;
        }
        engine.startPreview();
        /** Allows a user to join a channel.
         if you do not specify the uid, we will generate the uid for you*/

        ChannelMediaOptions option = new ChannelMediaOptions();
        option.autoSubscribeAudio = true;
        option.autoSubscribeVideo = true;
        int res = engine.joinChannel(accessToken, channelId, 0, option);
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
    }

    private void startPublish()
    {
        /**LiveTranscoding: A class for managing user-specific CDN live audio/video transcoding settings.
         * See <a href="https://docs.agora.io/en/Video/API%20Reference/java/classio_1_1agora_1_1rtc_1_1live_1_1_live_transcoding.html"></a>*/
        LiveTranscoding transcoding = new LiveTranscoding();
        /**The transcodingUser class which defines the video properties of the user displaying the
         * video in the CDN live. Agora supports a maximum of 17 transcoding users in a CDN live streaming channel.
         * See <a href="https://docs.agora.io/en/Video/API%20Reference/java/classio_1_1agora_1_1rtc_1_1live_1_1_live_transcoding_1_1_transcoding_user.html"></a>*/
        LiveTranscoding.TranscodingUser transcodingUser = new LiveTranscoding.TranscodingUser();
        transcodingUser.width = transcoding.width;
        transcodingUser.height = transcoding.height;
        transcodingUser.uid = myUid;
        /**Adds a user displaying the video in CDN live.
         * @return
         *  0: Success.
         *  <0: Failure.*/
        int ret = transcoding.addUser(transcodingUser);
        /**Sets the video layout and audio settings for CDN live.
         * The SDK triggers the onTranscodingUpdated callback when you call this method to update
         * the LiveTranscodingclass. If you call this method to set the LiveTranscoding class for
         * the first time, the SDK does not trigger the onTranscodingUpdated callback.
         * @param transcoding Sets the CDN live audio/video transcoding settings See
         *   <a href="https://docs.agora.io/en/Video/API%20Reference/java/classio_1_1agora_1_1rtc_1_1live_1_1_live_transcoding.html"></a>
         * @return
         *   0: Success.
         *   <0: Failure.
         * PS:
         *   This method applies to Live Broadcast only.
         *   Ensure that you enable the RTMP Converter service before using this function. See
         *      Prerequisites in Push Streams to CDN.
         *   Ensure that you call the setClientRole method and set the user role as the host.
         *   Ensure that you call the setLiveTranscoding method before calling the addPublishStreamUrl method.*/
        engine.setLiveTranscoding(transcoding);
        /**Publishes the local stream to the CDN.
         * The addPublishStreamUrl method call triggers the onRtmpStreamingStateChanged callback on
         * the local client to report the state of adding a local stream to the CDN.
         * @param url The CDN streaming URL in the RTMP format. The maximum length of this parameter
         *            is 1024 bytes. The URL address must not contain special characters, such as
         *            Chinese language characters.
         * @param transcodingEnabled Sets whether transcoding is enabled/disabled. If you set this
         *                           parameter as true, ensure that you call the setLiveTranscoding
         *                           method before this method.
         *                              true: Enable transcoding. To transcode the audio or video
         *                                 streams when publishing them to CDN live, often used for
         *                                 combining the audio and video streams of multiple hosts in CDN live.
         *                              false: Disable transcoding.
         * @return
         *   0: Success.
         *   < 0: Failure.
         *      ERR_INVALID_ARGUMENT(2): Invalid parameter, usually because the URL address is null or the string length is 0.
         *      ERR_NOT_INITIALIZED(7): You have not initialized RtcEngine when publishing the stream.
         * PS:
         *   Ensure that you enable the RTMP Converter service before using this function. See
         *      Prerequisites in Push Streams to CDN.
         *   This method applies to Live Broadcast only.
         *   Ensure that the user joins a channel before calling this method.
         *   This method adds only one stream HTTP/HTTPS URL address each time it is called.*/
        int code = engine.addPublishStreamUrl(et_url.getText().toString(), transCodeSwitch.isChecked());
        /**Prevent repeated entry*/
        publish.setEnabled(false);
        /**Prevent duplicate clicks*/
        transCodeSwitch.setEnabled(false);
    }

    private void stopPublish()
    {
        publishing = false;
        publish.setEnabled(true);
        publish.setText(getString(R.string.publish));
        /**Removes an RTMP stream from the CDN.
         * This method removes the RTMP URL address (added by addPublishStreamUrl) from a CDN live
         * stream. The SDK reports the result of this method call in the onRtmpStreamingStateChanged callback.
         * @param url The RTMP URL address to be removed. The maximum length of this parameter is
         *            1024 bytes. The URL address must not contain special characters, such as
         *            Chinese language characters.
         * @return
         *   0: Success.
         *   <0: Failure.
         * PS:
         *   Ensure that you enable the RTMP Converter service before using this function. See
         *      Prerequisites in Push Streams to CDN.
         *   Ensure that the user joins a channel before calling this method.
         *   This method applies to Live Broadcast only.
         *   This method removes only one stream RTMP URL address each time it is called.*/
        int ret = engine.removePublishStreamUrl(et_url.getText().toString());
    }

    /**
     * IRtcEngineEventHandler is an abstract class providing default implementation.
     * The SDK uses this class to report to the app on SDK runtime events.
     */
    private final IRtcEngineEventHandler iRtcEngineEventHandler = new IRtcEngineEventHandler()
    {
        /**Reports a warning during SDK runtime.
         * Warning code: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_warn_code.html*/
        @Override
        public void onWarning(int warn)
        {
            Log.w(TAG, String.format("onWarning code %d message %s", warn, RtcEngine.getErrorDescription(warn)));
        }

        /**Occurs when a user leaves the channel.
         * @param stats With this callback, the application retrieves the channel information,
         *              such as the call duration and statistics.*/
        @Override
        public void onLeaveChannel(RtcStats stats)
        {
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
        public void onJoinChannelSuccess(String channel, int uid, int elapsed)
        {
            Log.i(TAG, String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            showLongToast(String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            myUid = uid;
            joined = true;
            handler.post(new Runnable()
            {
                @Override
                public void run()
                {
                    join.setEnabled(true);
                    join.setText(getString(R.string.leave));
                    publish.setEnabled(true);
                    publish.setText(getString(R.string.publish));
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
        public void onRemoteAudioStateChanged(int uid, IRtcEngineEventHandler.REMOTE_AUDIO_STATE state, IRtcEngineEventHandler.REMOTE_AUDIO_STATE_REASON reason, int elapsed)
        {
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
        public void onRemoteVideoStateChanged(int uid, int state, int reason, int elapsed)
        {
            super.onRemoteVideoStateChanged(uid, state, reason, elapsed);
            Log.i(TAG, "onRemoteVideoStateChanged->" + uid + ", state->" + state + ", reason->" + reason);
        }

        /**Since v2.4.1
         * Occurs when the state of the RTMP streaming changes.
         * This callback indicates the state of the RTMP streaming. When exceptions occur, you can
         * troubleshoot issues by referring to the detailed error descriptions in the errCode parameter.
         * @param url The RTMP URL address.
         * @param state The RTMP streaming state:
         *   RTMP_STREAM_PUBLISH_STATE_IDLE(0): The RTMP streaming has not started or has ended.
         *              This state is also triggered after you remove an RTMP address from the CDN
         *              by calling removePublishStreamUrl.
         *   RTMP_STREAM_PUBLISH_STATE_CONNECTING(1): The SDK is connecting to Agora streaming server
         *              and the RTMP server. This state is triggered after you call the addPublishStreamUrl method.
         *   RTMP_STREAM_PUBLISH_STATE_RUNNING(2): The RTMP streaming publishes. The SDK successfully
         *              publishes the RTMP streaming and returns this state.
         *   RTMP_STREAM_PUBLISH_STATE_RECOVERING(3): The RTMP streaming is recovering. When exceptions
         *              occur to the CDN, or the streaming is interrupted, the SDK attempts to resume
         *              RTMP streaming and returns this state.
         *                1:If the SDK successfully resumes the streaming, RTMP_STREAM_PUBLISH_STATE_RUNNING(2)
         *                    returns.
         *                2:If the streaming does not resume within 60 seconds or server errors occur,
         *                    RTMP_STREAM_PUBLISH_STATE_FAILURE(4) returns. You can also reconnect to the
         *                    server by calling the removePublishStreamUrl and addPublishStreamUrl methods.
         *   RTMP_STREAM_PUBLISH_STATE_FAILURE(4): The RTMP streaming fails. See the errCode parameter
         *              for the detailed error information. You can also call the addPublishStreamUrl
         *              method to publish the RTMP streaming again.
         * @param errCode The detailed error information for streaming:
         *   RTMP_STREAM_PUBLISH_ERROR_OK(0): The RTMP streaming publishes successfully.
         *   RTMP_STREAM_PUBLISH_ERROR_INVALID_ARGUMEN(1): Invalid argument used. If, for example,
         *                you do not call the setLiveTranscoding method to configure the LiveTranscoding
         *                parameters before calling the addPublishStreamUrl method, the SDK returns
         *                this error. Check whether you set the parameters in the setLiveTranscoding method properly.
         *   RTMP_STREAM_PUBLISH_ERROR_ENCRYPTED_STREAM_NOT_ALLOWED(2): The RTMP streaming is
         *                encrypted and cannot be published.
         *   RTMP_STREAM_PUBLISH_ERROR_CONNECTION_TIMEOUT(3): Timeout for the RTMP streaming. Call
         *                the addPublishStreamUrl method to publish the streaming again.
         *   RTMP_STREAM_PUBLISH_ERROR_INTERNAL_SERVER_ERROR(4): An error occurs in Agora streaming
         *                server. Call the addPublishStreamUrl method to publish the streaming again.
         *   RTMP_STREAM_PUBLISH_ERROR_RTMP_SERVER_ERROR(5): An error occurs in the RTMP server.
         *   RTMP_STREAM_PUBLISH_ERROR_TOO_OFTEN(6): The RTMP streaming publishes too frequently.
         *   RTMP_STREAM_PUBLISH_ERROR_REACH_LIMIT(7): The host publishes more than 10 URLs. Delete
         *                the unnecessary URLs before adding new ones.
         *   RTMP_STREAM_PUBLISH_ERROR_NOT_AUTHORIZED(8): The host manipulates other hosts' URLs.
         *                Check your app logic.
         *   RTMP_STREAM_PUBLISH_ERROR_STREAM_NOT_FOUND(9): Agora server fails to find the RTMP
         *                streaming.
         *   RTMP_STREAM_PUBLISH_ERROR_FORMAT_NOT_SUPPORTED(10): The format of the RTMP streaming
         *                URL is not supported. Check whether the URL format is correct.*/
        @Override
        public void onRtmpStreamingStateChanged(String url, IRtcEngineEventHandler.RTMP_STREAM_PUBLISH_STATE state, IRtcEngineEventHandler.RTMP_STREAM_PUBLISH_ERROR errCode)
        {
            super.onRtmpStreamingStateChanged(url, state, errCode);
            Log.i(TAG, "onRtmpStreamingStateChanged->" + url + ", state->" + state + ", errCode->" + errCode);
            if(state == RTMP_STREAM_PUBLISH_STATE.RTMP_STREAM_PUBLISH_STATE_RUNNING)
            {
                /**After confirming the successful push, make changes to the UI.*/
                publishing = true;
                handler.post(new Runnable()
                {
                    @Override
                    public void run()
                    {
                        publish.setEnabled(true);
                        publish.setText(getString(R.string.stoppublish));
                    }
                });
            } else if (state == RTMP_STREAM_PUBLISH_STATE.RTMP_STREAM_PUBLISH_STATE_FAILURE) {
                /**if failed, make changes to the UI.*/
                publishing = true;
                handler.post(() -> {
                    publish.setEnabled(true);
                    publish.setText(getString(R.string.publish));
                    transCodeSwitch.setEnabled(true);
                    publishing = false;
                });
            } else if (state == RTMP_STREAM_PUBLISH_STATE.RTMP_STREAM_PUBLISH_STATE_IDLE) {
                /**Push stream not started or ended, make changes to the UI.*/
                publishing = true;
                handler.post(() -> {
                    publish.setEnabled(true);
                    publish.setText(getString(R.string.publish));
                    transCodeSwitch.setEnabled(true);
                    publishing = false;
                });
            }
        }

        /**Occurs when a remote user (Communication)/host (Live Broadcast) joins the channel.
         * @param uid ID of the user whose audio state changes.
         * @param elapsed Time delay (ms) from the local user calling joinChannel/setClientRole
         *                until this callback is triggered.*/
        @Override
        public void onUserJoined(int uid, int elapsed)
        {
            super.onUserJoined(uid, elapsed);
            Log.i(TAG, "onUserJoined->" + uid);
            showLongToast(String.format("user %d joined!", uid));
            /**Check if the context is correct*/
            Context context = getContext();
            if (context == null) {
                return;
            }
            handler.post(() ->
            {
                /**Display remote video stream*/
                SurfaceView surfaceView = RtcEngine.CreateRendererView(context);
                surfaceView.setZOrderMediaOverlay(true);
                if (fl_remote.getChildCount() > 0)
                {
                    fl_remote.removeAllViews();
                }
                // Add to the remote container
                fl_remote.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
                // Setup remote video to render
                engine.setupRemoteVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, uid));
            });
            /**Determine whether to open transcoding service and whether the current number of
             * transcoding users exceeds the maximum number of users*/
            if (transCodeSwitch.isChecked() && transcoding.getUserCount() < MAXUserCount) {
                /**The transcoding images are arranged vertically according to the adding order*/
                LiveTranscoding.TranscodingUser transcodingUser = new LiveTranscoding.TranscodingUser();
                transcodingUser.x = 0;
                transcodingUser.y = localTranscodingUser.height;
                transcodingUser.width = transcoding.width;
                transcodingUser.height = transcoding.height / MAXUserCount;
                transcodingUser.uid = uid;
                int ret = transcoding.addUser(transcodingUser);
                /**refresh transCoding configuration*/
                engine.setLiveTranscoding(transcoding);
            }
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
        public void onUserOffline(int uid, int reason)
        {
            Log.i(TAG, String.format("user %d offline! reason:%d", uid, reason));
            showLongToast(String.format("user %d offline! reason:%d", uid, reason));
            handler.post(new Runnable() {
                @Override
                public void run() {
                    /**Clear render view
                     Note: The video will stay at its last frame, to completely remove it you will need to
                     remove the SurfaceView from its parent*/
                    engine.setupRemoteVideo(new VideoCanvas(null, RENDER_MODE_HIDDEN, uid));
                    if(transcoding != null) {
                        /**Removes a user from CDN live.
                         * @return
                         * 0: Success.
                         * < 0: Failure.*/
                        int code = transcoding.removeUser(uid);
                        if (code == Constants.ERR_OK) {
                            /**refresh transCoding configuration*/
                            engine.setLiveTranscoding(transcoding);
                        }
                    }
                }
            });
        }
    };
}
