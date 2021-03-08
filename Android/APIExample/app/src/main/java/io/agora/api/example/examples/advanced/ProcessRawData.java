package io.agora.api.example.examples.advanced;

import android.content.Context;
import android.graphics.Bitmap;
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

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;

import io.agora.advancedvideo.rawdata.MediaDataAudioObserver;
import io.agora.advancedvideo.rawdata.MediaDataObserverPlugin;
import io.agora.advancedvideo.rawdata.MediaDataVideoObserver;
import io.agora.advancedvideo.rawdata.MediaPreProcessing;
import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.utils.CommonUtil;
import io.agora.api.example.utils.YUVUtils;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoEncoderConfiguration;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc2.Constants.RAW_AUDIO_FRAME_OP_MODE_READ_ONLY;
import static io.agora.rtc2.video.VideoCanvas.RENDER_MODE_HIDDEN;
import static io.agora.rtc2.video.VideoEncoderConfiguration.STANDARD_BITRATE;

@Example(
        index = 9,
        group = ADVANCED,
        name = R.string.item_processraw,
        actionId = R.id.action_mainFragment_to_ProcessRawData,
        tipsId = R.string.processrawdata
)
public class ProcessRawData extends BaseFragment implements View.OnClickListener, MediaDataVideoObserver,
        MediaDataAudioObserver {
    private static final String TAG = ProcessRawData.class.getSimpleName();

    private FrameLayout fl_local, fl_remote;
    private Button join, blurBtn;
    private EditText et_channel;
    private RtcEngine engine;
    private int myUid;
    private boolean joined = false, blur = true;
    private MediaDataObserverPlugin mediaDataObserverPlugin;

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
        catch (Exception e) {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_process_rawdata, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        join = view.findViewById(R.id.btn_join);
        blurBtn = view.findViewById(R.id.btn_blur);
        et_channel = view.findViewById(R.id.et_channel);
        join.setOnClickListener(this);
        blurBtn.setOnClickListener(this);
        fl_local = view.findViewById(R.id.fl_local);
        fl_remote = view.findViewById(R.id.fl_remote);
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mediaDataObserverPlugin = MediaDataObserverPlugin.the();
        MediaPreProcessing.setCallback(mediaDataObserverPlugin);
        MediaPreProcessing.setVideoCaptureByteBuffer(mediaDataObserverPlugin.byteBufferCapture);
        MediaPreProcessing.setVideoCaptureByteBuffer(mediaDataObserverPlugin.byteBufferRender);
        mediaDataObserverPlugin.addVideoObserver(this);
    }

    @Override
    public void onDestroy() {
        if (mediaDataObserverPlugin != null) {
            mediaDataObserverPlugin.removeVideoObserver(this);
            mediaDataObserverPlugin.removeAllBuffer();
        }
        MediaPreProcessing.releasePoint();
        /**leaveChannel and Destroy the RtcEngine instance*/
        if (engine != null) {
            engine.leaveChannel();
            engine.stopPreview();
        }
        handler.post(RtcEngine::destroy);
        engine = null;
        super.onDestroy();
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_join) {
            if (!joined) {
                CommonUtil.hideInputBoard(getActivity(), et_channel);
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
                engine.leaveChannel();
                engine.stopPreview();
                join.setText(getString(R.string.join));
            }
        }
        else if(v.getId() == R.id.btn_blur)
        {
            if(!blur)
            {
                blur = true;
                blurBtn.setText(getString(R.string.blur));
            }
            else
            {
                blur = false;
                blurBtn.setText(getString(R.string.closeblur));
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

        /**
         * Sets the audio recording format for the onRecordAudioFrame callback.
         * sampleRate	Sets the sample rate (samplesPerSec) returned in the onRecordAudioFrame callback, which can be set as 8000, 16000, 32000, 44100, or 48000 Hz.
         * channel	Sets the number of audio channels (channels) returned in the onRecordAudioFrame callback:
         * 1: Mono
         * 2: Stereo
         * mode	Sets the use mode (see RAW_AUDIO_FRAME_OP_MODE_TYPE) of the onRecordAudioFrame callback.
         * samplesPerCall	Sets the number of samples returned in the onRecordAudioFrame callback. samplesPerCall is usually set as 1024 for RTMP streaming.
         * The SDK triggers the onRecordAudioFrame callback according to the sample interval. Ensure that the sample interval = 0.01 (s). And, Sample interval (sec) = samplePerCall/(sampleRate � channel).
         */
        engine.setRecordingAudioFrameParameters(4000, 1, RAW_AUDIO_FRAME_OP_MODE_READ_ONLY, 1024);

        /**
         * Sets the audio playback format for the onPlaybackAudioFrame callback.
         * sampleRate	Sets the sample rate (samplesPerSec) returned in the onRecordAudioFrame callback, which can be set as 8000, 16000, 32000, 44100, or 48000 Hz.
         * channel	Sets the number of audio channels (channels) returned in the onRecordAudioFrame callback:
         * 1: Mono
         * 2: Stereo
         * mode	Sets the use mode (see RAW_AUDIO_FRAME_OP_MODE_TYPE) of the onRecordAudioFrame callback.
         * samplesPerCall	Sets the number of samples returned in the onRecordAudioFrame callback. samplesPerCall is usually set as 1024 for RTMP streaming.
         * The SDK triggers the onRecordAudioFrame callback according to the sample interval. Ensure that the sample interval = 0.01 (s). And, Sample interval (sec) = samplePerCall/(sampleRate � channel).
         */
        engine.setPlaybackAudioFrameParameters(4000, 1, RAW_AUDIO_FRAME_OP_MODE_READ_ONLY, 1024);

        /**
         * Sets the mixed audio format for the onMixedAudioFrame callback.
         * sampleRate	Sets the sample rate (samplesPerSec) returned in the onMixedAudioFrame callback, which can be set as 8000, 16000, 32000, 44100, or 48000 Hz.
         * samplesPerCall	Sets the number of samples (samples) returned in the onMixedAudioFrame callback. samplesPerCall is usually set as 1024 for RTMP streaming.
         */
        engine.setMixedAudioFrameParameters(8000, 1, 1024);

        /**Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        String accessToken = getString(R.string.agora_access_token);
        if (TextUtils.equals(accessToken, "") || TextUtils.equals(accessToken, "<#YOUR ACCESS TOKEN#>")) {
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

    /**
     * IRtcEngineEventHandler is an abstract class providing default implementation.
     * The SDK uses this class to report to the app on SDK runtime events.
     */
    private final IRtcEngineEventHandler iRtcEngineEventHandler = new IRtcEngineEventHandler() {
        /**Reports a warning during SDK runtime.
         * Warning code: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_warn_code.html*/
        @Override
        public void onWarning(int warn) {
            Log.w(TAG, String.format("onWarning code %d message %s", warn, RtcEngine.getErrorDescription(warn)));
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
            showLongToast(String.format("user %d joined!", uid));
            /**Check if the context is correct*/
            Context context = getContext();
            if (context == null) {
                return;
            }
            handler.post(() ->
            {
                if (mediaDataObserverPlugin != null) {
                    mediaDataObserverPlugin.addDecodeBuffer(uid);
                }

                /**Display remote video stream*/
                // Create render view by RtcEngine
                SurfaceView surfaceView = RtcEngine.CreateRendererView(context);
                surfaceView.setZOrderMediaOverlay(true);
                if (fl_remote.getChildCount() > 0) {
                    fl_remote.removeAllViews();
                }
                // Add to the remote container
                fl_remote.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                        ViewGroup.LayoutParams.MATCH_PARENT));
                // Setup remote video to render
                engine.setupRemoteVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, uid));
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
            handler.post(new Runnable() {
                @Override
                public void run() {
                    if (mediaDataObserverPlugin != null) {
                        mediaDataObserverPlugin.removeDecodeBuffer(uid);
                    }
                    /**Clear render view
                     Note: The video will stay at its last frame, to completely remove it you will need to
                     remove the SurfaceView from its parent*/
                    engine.setupRemoteVideo(new VideoCanvas(null, RENDER_MODE_HIDDEN, uid));
                }
            });
        }
    };

    @Override
    public void onCaptureVideoFrame(byte[] data, int frameType, int width, int height, int bufferLength, int yStride, int uStride, int vStride, int rotation, long renderTimeMs) {
        /**You can do some processing on the video frame here*/
        Log.e(TAG, "onCaptureVideoFrame0");
        if(blur)
        {return;}
        Bitmap bmp = YUVUtils.blur(getContext(), YUVUtils.i420ToBitmap(width, height, rotation, bufferLength, data, yStride, uStride, vStride), 10);
        System.arraycopy(YUVUtils.bitmapToI420(width, height, bmp), 0, data, 0, bufferLength);
    }

    @Override
    public void onRenderVideoFrame(int uid, byte[] data, int frameType, int width, int height, int bufferLength, int yStride, int uStride, int vStride, int rotation, long renderTimeMs) {
        if(blur)
        {return;}
        Bitmap bmp = YUVUtils.blur(getContext(), YUVUtils.i420ToBitmap(width, height, rotation, bufferLength, data, yStride, uStride, vStride), 10);
        System.arraycopy(YUVUtils.bitmapToI420(width, height, bmp), 0, data, 0, bufferLength);
    }

    /**
     * Retrieves the recorded audio frame.
     * @param audioFrameType only support FRAME_TYPE_PCM16
     * @param samples The number of samples per channel in the audio frame.
     * @param bytesPerSample The number of bytes per audio sample, which is usually 16-bit (2-byte).
     * @param channels The number of audio channels.
     *                      1: Mono
     *                      2: Stereo (the data is interleaved)
     * @param samplesPerSec The sample rate.
     * @param renderTimeMs The timestamp of the external audio frame.
     * @param bufferLength audio frame size*/
    @Override
    public void onRecordAudioFrame(byte[] data, int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {

    }

    /**
     * Retrieves the audio playback frame for getting the audio.
     * @param audioFrameType only support FRAME_TYPE_PCM16
     * @param samples The number of samples per channel in the audio frame.
     * @param bytesPerSample The number of bytes per audio sample, which is usually 16-bit (2-byte).
     * @param channels The number of audio channels.
     *                      1: Mono
     *                      2: Stereo (the data is interleaved)
     * @param samplesPerSec The sample rate.
     * @param renderTimeMs The timestamp of the external audio frame.
     * @param bufferLength audio frame size*/
    @Override
    public void onPlaybackAudioFrame(byte[] data, int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {

    }


    /**
     * Retrieves the audio frame of a specified user before mixing.
     * The SDK triggers this callback if isMultipleChannelFrameWanted returns false.
     * @param uid remote user id
     * @param audioFrameType only support FRAME_TYPE_PCM16
     * @param samples The number of samples per channel in the audio frame.
     * @param bytesPerSample The number of bytes per audio sample, which is usually 16-bit (2-byte).
     * @param channels The number of audio channels.
     *                      1: Mono
     *                      2: Stereo (the data is interleaved)
     * @param samplesPerSec The sample rate.
     * @param renderTimeMs The timestamp of the external audio frame.
     * @param bufferLength audio frame size*/
    @Override
    public void onPlaybackAudioFrameBeforeMixing(int uid, byte[] data, int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {

    }

    /**
     * Retrieves the mixed recorded and playback audio frame.
     * @param audioFrameType only support FRAME_TYPE_PCM16
     * @param samples The number of samples per channel in the audio frame.
     * @param bytesPerSample The number of bytes per audio sample, which is usually 16-bit (2-byte).
     * @param channels The number of audio channels.
     *                      1: Mono
     *                      2: Stereo (the data is interleaved)
     * @param samplesPerSec The sample rate.
     * @param renderTimeMs The timestamp of the external audio frame.
     * @param bufferLength audio frame size*/
    @Override
    public void onMixedAudioFrame(byte[] data, int audioFrameType, int samples, int bytesPerSample, int channels, int samplesPerSec, long renderTimeMs, int bufferLength) {

    }
}
