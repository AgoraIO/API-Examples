package io.agora.api.example.examples.audio;

import static com.google.android.exoplayer2.Player.REPEAT_MODE_ALL;
import static io.agora.rtc2.Constants.RENDER_MODE_HIDDEN;
import static io.agora.rtc2.video.VideoEncoderConfiguration.STANDARD_BITRATE;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.google.android.exoplayer2.ExoPlayer;
import com.google.android.exoplayer2.MediaItem;
import com.google.android.exoplayer2.Player;

import java.util.Locale;
import java.util.Random;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.Constant;
import io.agora.api.example.common.widget.VideoReportLayout;
import io.agora.api.example.databinding.FragmentAudiorouterPlayerDetailBinding;
import io.agora.api.example.utils.TokenUtils;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoEncoderConfiguration;

/**
 * The type Audio router player exo.
 */
public class AudioRouterPlayerExo extends BaseFragment {
    private static final String TAG = "AudioRouterPlayerExo";
    private FragmentAudiorouterPlayerDetailBinding mBinding;
    private RtcEngine mRtcEngine;

    private ExoPlayer mExoPlayer;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // Check if the context is valid
        Context context = getContext();
        if (context == null) {
            return;
        }

        String channelId = requireArguments().getString("channelId");
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
            mRtcEngine = RtcEngine.create(config);
            /*
             * This parameter is for reporting the usages of APIExample to agora background.
             * Generally, it is not necessary for you to set this parameter.
             */
            mRtcEngine.setParameters("{"
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
                mRtcEngine.setLocalAccessPoint(localAccessPointConfiguration);
            }

            // Set audio route to microPhone
            mRtcEngine.setDefaultAudioRoutetoSpeakerphone(true);
            /*In the demo, the default is to enter as the anchor.*/
            mRtcEngine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
            // Enable video module
            mRtcEngine.enableVideo();
            // Setup video encoding configs
            mRtcEngine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                    ((MainApplication) requireActivity().getApplication()).getGlobalSettings().getVideoEncodingDimensionObject(),
                    VideoEncoderConfiguration.FRAME_RATE.valueOf(((MainApplication) requireActivity().getApplication()).getGlobalSettings().getVideoEncodingFrameRate()),
                    STANDARD_BITRATE,
                    VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_ADAPTIVE
            ));

            ChannelMediaOptions option = new ChannelMediaOptions();
            option.autoSubscribeAudio = true;
            option.autoSubscribeVideo = true;
            option.publishMicrophoneTrack = true;
            option.publishCameraTrack = true;

            /*Please configure accessToken in the string_config file.
             * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
             *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
             * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
             *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
            int uid = new Random().nextInt(1000) + 100000;
            TokenUtils.gen(requireContext(), channelId, uid, ret -> {
                /* Allows a user to join a channel.
                 if you do not specify the uid, we will generate the uid for you*/
                int res = mRtcEngine.joinChannel(ret, channelId, uid, option);
                if (res != 0) {
                    // Usually happens with invalid parameters
                    // Error code description can be found at:
                    // en: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                    // cn: https://docs.agora.io/cn/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                    showAlert(RtcEngine.getErrorDescription(Math.abs(res)));
                }
            });
        } catch (Exception e) {
            e.printStackTrace();
            requireActivity().onBackPressed();
        }

        mExoPlayer = new ExoPlayer.Builder(requireContext()).build();
        mExoPlayer.addListener(new Player.Listener() {
            @Override
            public void onEvents(Player player, Player.Events events) {
                Player.Listener.super.onEvents(player, events);
            }
        });
        mExoPlayer.setMediaItem(new MediaItem.Builder().setUri(Constant.URL_VIDEO_SAMPLE).build());
        mExoPlayer.setRepeatMode(REPEAT_MODE_ALL);
        mExoPlayer.setPlayWhenReady(true);
        mExoPlayer.prepare();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (mRtcEngine != null) {
            mRtcEngine.leaveChannel();
        }
        handler.post(RtcEngine::destroy);
        mRtcEngine = null;
        if (mExoPlayer != null) {
            mExoPlayer.stop();
            mExoPlayer.release();
            mExoPlayer = null;
        }
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mBinding = FragmentAudiorouterPlayerDetailBinding.inflate(inflater);
        return mBinding.getRoot();
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        mBinding.swSpeaker.setOnCheckedChangeListener((buttonView, isChecked) -> mRtcEngine.setEnableSpeakerphone(isChecked));

        // Render Local Video
        SurfaceView localVideoView = new SurfaceView(requireContext());
        mBinding.localVideo.removeAllViews();
        // Add to the remote container
        mBinding.localVideo.addView(localVideoView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        // Setup remote video to render
        mRtcEngine.setupLocalVideo(new VideoCanvas(localVideoView, RENDER_MODE_HIDDEN, 0));

        // Render Player Video
        SurfaceView playerVideoView = new SurfaceView(requireContext());
        mBinding.playerVideo.addView(playerVideoView);
        mExoPlayer.setVideoSurfaceView(playerVideoView);
    }


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
            super.onError(err);
            showLongToast("Error code:" + err + ", msg:" + RtcEngine.getErrorDescription(err));
            if (Constants.ERR_INVALID_TOKEN == err) {
                showAlert(getString(R.string.token_invalid));
            } else if (Constants.ERR_TOKEN_EXPIRED == err) {
                showAlert(getString(R.string.token_expired));
            }
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
            runOnUIThread(() -> mBinding.localVideo.setReportUid(uid));
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
            /*Check if the context is correct*/
            Context context = getContext();
            if (context == null) {
                return;
            }
            runOnUIThread(() -> {
                /*Display remote video stream*/
                // Create render view by RtcEngine
                SurfaceView surfaceView = new SurfaceView(context);
                surfaceView.setZOrderMediaOverlay(true);
                VideoReportLayout view = mBinding.remoteVideo;
                view.setReportUid(uid);
                view.removeAllViews();
                // Add to the remote container
                view.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
                // Setup remote video to render
                mRtcEngine.setupRemoteVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, uid));
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
            showLongToast(String.format(Locale.US, "user %d offline! reason:%d", uid, reason));
            runOnUIThread(() -> {
                /*Clear render view
                 Note: The video will stay at its last frame, to completely remove it you will need to
                 remove the SurfaceView from its parent*/
                mRtcEngine.setupRemoteVideo(new VideoCanvas(null, RENDER_MODE_HIDDEN, uid));
                mBinding.remoteVideo.removeAllViews();
            });
        }

        @Override
        public void onAudioRouteChanged(int routing) {
            super.onAudioRouteChanged(routing);
            Log.i(TAG, String.format("audio router change to %d", routing));
            showLongToast(String.format(Locale.US, "audio router change to %d", routing));
        }

        @Override
        public void onLocalAudioStats(LocalAudioStats stats) {
            super.onLocalAudioStats(stats);
            mBinding.localVideo.setLocalAudioStats(stats);
        }

        @Override
        public void onRemoteAudioStats(RemoteAudioStats stats) {
            super.onRemoteAudioStats(stats);
            mBinding.remoteVideo.setRemoteAudioStats(stats);
        }

        @Override
        public void onLocalVideoStats(Constants.VideoSourceType source, LocalVideoStats stats) {
            super.onLocalVideoStats(source, stats);
            mBinding.localVideo.setLocalVideoStats(stats);
        }

        @Override
        public void onRemoteVideoStats(RemoteVideoStats stats) {
            super.onRemoteVideoStats(stats);
            mBinding.remoteVideo.setRemoteVideoStats(stats);
        }
    };
}
