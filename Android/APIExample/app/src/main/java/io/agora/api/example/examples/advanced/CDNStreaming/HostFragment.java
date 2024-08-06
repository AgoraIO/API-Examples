package io.agora.api.example.examples.advanced.CDNStreaming;

import static io.agora.rtc2.Constants.CLIENT_ROLE_BROADCASTER;
import static io.agora.rtc2.Constants.RENDER_MODE_HIDDEN;
import static io.agora.rtc2.video.VideoEncoderConfiguration.STANDARD_BITRATE;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.Switch;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.Locale;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.common.BaseFragment;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.DirectCdnStreamingMediaOptions;
import io.agora.rtc2.DirectCdnStreamingReason;
import io.agora.rtc2.DirectCdnStreamingState;
import io.agora.rtc2.DirectCdnStreamingStats;
import io.agora.rtc2.IDirectCdnStreamingEventHandler;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.LeaveChannelOptions;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.live.LiveTranscoding;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;
import io.agora.rtc2.video.CameraCapturerConfiguration;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoEncoderConfiguration;

/**
 * The type Host fragment.
 */
public class HostFragment extends BaseFragment {
    private static final String TAG = HostFragment.class.getSimpleName();
    private static final String AGORA_CHANNEL_PREFIX = "rtmp://push.webdemo.agoraio.cn/lbhd/";

    private volatile boolean isAgoraChannel = true;
    private volatile boolean cdnStreaming = false;
    private volatile boolean rtcStreaming = false;
    private String channel;
    private FrameLayout fl_local, fl_remote, fl_remote_2, fl_remote_3;
    private Map<Integer, ViewGroup> remoteViews = new ConcurrentHashMap<Integer, ViewGroup>();
    private LinearLayout rtc_control, video_row2;
    private RtcEngine engine;
    private LiveTranscoding liveTranscoding = new LiveTranscoding();
    private Button streamingButton;
    private Switch rtcSwitcher;
    private SeekBar volSeekBar;
    private VideoEncoderConfiguration videoEncoderConfiguration;
    private int canvas_width = 480;
    private int canvas_height = 640;
    private int localUid = (int) (Math.random() * Integer.MAX_VALUE / 2);

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_cdn_host, container, false);
        Bundle bundle = this.getArguments();
        isAgoraChannel = bundle.getBoolean(getString(R.string.key_is_agora_channel));
        channel = bundle.getString(getString(R.string.key_channel_name));
        return view;
    }

    @Override
    public void onResume() {
        super.onResume();
        getView().setFocusableInTouchMode(true);
        getView().requestFocus();
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        fl_local = view.findViewById(R.id.fl_local);
        fl_remote = view.findViewById(R.id.fl_remote);
        fl_remote_2 = view.findViewById(R.id.fl_remote2);
        fl_remote_3 = view.findViewById(R.id.fl_remote3);
        rtc_control = view.findViewById(R.id.rtc_ctrl);
        rtc_control.setVisibility(isAgoraChannel ? View.VISIBLE : View.INVISIBLE);
        video_row2 = view.findViewById(R.id.video_container_row2);
        streamingButton = view.findViewById(R.id.streaming_btn);
        streamingButton.setOnClickListener(streamingOnCLickListener);
        rtcSwitcher = view.findViewById(R.id.rtc_switch);
        rtcSwitcher.setOnCheckedChangeListener(checkedChangeListener);
        volSeekBar = view.findViewById(R.id.record_vol);
        volSeekBar.setOnSeekBarChangeListener(seekBarChangeListener);
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

            CameraCapturerConfiguration.CaptureFormat captureFormat = new CameraCapturerConfiguration.CaptureFormat();
            captureFormat.fps = 30;
            engine.setCameraCapturerConfiguration(new CameraCapturerConfiguration(CameraCapturerConfiguration.CAMERA_DIRECTION.CAMERA_FRONT, captureFormat));

            engine.setVideoEncoderConfiguration(new VideoEncoderConfiguration());
            setupEngineConfig(context);
        } catch (Exception e) {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }

    private void setupEngineConfig(Context context) {
        // setup local video to render local camera preview
        SurfaceView surfaceView = new SurfaceView(context);
        if (fl_local.getChildCount() > 0) {
            fl_local.removeAllViews();
        }
        // Add to the local container
        fl_local.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        // Setup local video to render your local camera preview
        engine.setupLocalVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, 0));
        // You have to call startPreview to see local video
        engine.startPreview();
        // Set audio route to microPhone
        engine.setDefaultAudioRoutetoSpeakerphone(true);
        /*In the demo, the default is to enter as the anchor.*/
        engine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
        // Enable video module
        engine.enableVideo();
        // Setup video encoding configs
        VideoEncoderConfiguration.VideoDimensions videoDimensions = ((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingDimensionObject();
        canvas_height = Math.max(videoDimensions.height, videoDimensions.width);
        canvas_width = Math.min(videoDimensions.height, videoDimensions.width);
        videoEncoderConfiguration = new VideoEncoderConfiguration(
                videoDimensions, VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15, STANDARD_BITRATE, VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_FIXED_PORTRAIT
        );
        liveTranscoding.width = canvas_width;
        liveTranscoding.height = canvas_height;
        liveTranscoding.videoFramerate = 15;
        engine.setVideoEncoderConfiguration(videoEncoderConfiguration);
        engine.setDirectCdnStreamingVideoConfiguration(videoEncoderConfiguration);
    }

    private void stopStreaming() {
        rtcStreaming = false;
        cdnStreaming = false;
        rtcSwitcher.setChecked(false);
        rtcSwitcher.setEnabled(false);
        streamingButton.setText(getString(R.string.start_live_streaming));
    }

    private final View.OnClickListener streamingOnCLickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            if (rtcStreaming) {
                engine.stopRtmpStream(getUrl());
                engine.leaveChannel();
                stopStreaming();
            } else if (cdnStreaming) {
                engine.stopDirectCdnStreaming();
                engine.startPreview();
                rtcSwitcher.setChecked(false);
                rtcSwitcher.setEnabled(false);
            } else {
                engine.setDirectCdnStreamingVideoConfiguration(videoEncoderConfiguration);
                int ret = startCdnStreaming();
                if (ret == 0) {
                    streamingButton.setText(R.string.text_streaming);
                } else {
                    showLongToast(String.format("startCdnStreaming failed! error code: %d", ret));
                }
            }
        }
    };

    private int startCdnStreaming() {
        DirectCdnStreamingMediaOptions directCdnStreamingMediaOptions = new DirectCdnStreamingMediaOptions();
        directCdnStreamingMediaOptions.publishCameraTrack = true;
        directCdnStreamingMediaOptions.publishMicrophoneTrack = true;
        return engine.startDirectCdnStreaming(iDirectCdnStreamingEventHandler, getUrl(), directCdnStreamingMediaOptions);
    }

    private String getUrl() {
        if (isAgoraChannel) {
            return AGORA_CHANNEL_PREFIX + channel;
        } else {
            return channel;
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (engine != null) {
            if (rtcStreaming) {
                engine.leaveChannel();
            } else if (cdnStreaming) {
                engine.stopDirectCdnStreaming();
            }
            /*leaveChannel and Destroy the RtcEngine instance*/
            engine.stopPreview();
            handler.post(RtcEngine::destroy);
            engine = null;
        }
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
            Log.w(TAG, String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
        }

        /**Occurs when a user leaves the channel.
         * @param stats With this callback, the application retrieves the channel information,
         *              such as the call duration and statistics.*/
        @Override
        public void onLeaveChannel(RtcStats stats) {
            super.onLeaveChannel(stats);
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
            localUid = uid;
            LiveTranscoding.TranscodingUser user = new LiveTranscoding.TranscodingUser();
            user.x = 0;
            user.y = 0;
            user.width = canvas_width;
            user.height = canvas_height;
            user.uid = localUid;
            liveTranscoding.addUser(user);
            // engine.updateRtmpTranscoding(liveTranscoding);
            int ret = engine.startRtmpStreamWithTranscoding(getUrl(), liveTranscoding);
            if (ret != 0) {
                showLongToast(String.format(Locale.US, "startRtmpStreamWithTranscoding failed! reason:%d", ret));
            }
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
            /*Check if the context is correct*/
            Context context = getContext();
            if (context == null) {
                return;
            }

            if (remoteViews.containsKey(uid)) {
                return;
            } else {
                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        /*Display remote video stream*/
                        SurfaceView surfaceView = null;
                        // Create render view by RtcEngine
                        surfaceView = new SurfaceView(context);
                        surfaceView.setZOrderMediaOverlay(true);
                        ViewGroup view = getAvailableView();
                        remoteViews.put(uid, view);
                        // Add to the remote container
                        view.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
                        // Setup remote video to render
                        engine.setupRemoteVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, uid));
                        updateTranscodeLayout();
                    }
                });
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
                    remoteViews.get(uid).removeAllViews();
                    remoteViews.remove(uid);
                    updateTranscodeLayout();
                }
            });
        }

        @Override
        public void onRtmpStreamingStateChanged(String url, int state, int errCode) {
            super.onRtmpStreamingStateChanged(url, state, errCode);
            showShortToast(String.format("onRtmpStreamingStateChanged state %s errCode %s", state, errCode));
            if (state == Constants.RTMP_STREAM_PUBLISH_STATE_IDLE) {
                if (cdnStreaming) {
                    runOnUIThread(() -> {
                        LeaveChannelOptions leaveChannelOptions = new LeaveChannelOptions();
                        leaveChannelOptions.stopMicrophoneRecording = false;
                        engine.leaveChannel(leaveChannelOptions);
                        fl_remote.removeAllViews();
                        fl_remote_2.removeAllViews();
                        fl_remote_3.removeAllViews();
                        remoteViews.clear();
                        engine.startPreview();
                        engine.setDirectCdnStreamingVideoConfiguration(videoEncoderConfiguration);
                        int ret = startCdnStreaming();
                        if (ret != 0) {
                            showLongToast(String.format("startCdnStreaming failed! error code: %d", ret));
                            stopStreaming();
                        }
                    });
                }
            }
        }

        @Override
        public void onTranscodingUpdated() {
            showLongToast("RTMP transcoding updated successfully!");
        }
    };

    private void updateTranscodeLayout() {
        boolean hasRemote = remoteViews.size() > 0;
        LiveTranscoding.TranscodingUser user = new LiveTranscoding.TranscodingUser();
        user.x = 0;
        user.y = 0;
        user.width = hasRemote ? canvas_width / 2 : canvas_width;
        user.height = hasRemote ? canvas_height / 2 : canvas_height;
        user.uid = localUid;
        liveTranscoding.addUser(user);
        if (hasRemote) {
            int index = 0;
            for (int uid : remoteViews.keySet()) {
                index++;
                switch (index) {
                    case 1:
                        LiveTranscoding.TranscodingUser user1 = new LiveTranscoding.TranscodingUser();
                        user1.x = canvas_width / 2;
                        user1.y = 0;
                        user1.width = canvas_width / 2;
                        user1.height = canvas_height / 2;
                        user1.uid = uid;
                        liveTranscoding.addUser(user1);
                        break;
                    case 2:
                        LiveTranscoding.TranscodingUser user2 = new LiveTranscoding.TranscodingUser();
                        user2.x = 0;
                        user2.y = canvas_height / 2;
                        user2.width = canvas_width / 2;
                        user2.height = canvas_height / 2;
                        user2.uid = uid;
                        liveTranscoding.addUser(user2);
                        break;
                    case 3:
                        LiveTranscoding.TranscodingUser user3 = new LiveTranscoding.TranscodingUser();
                        user3.x = canvas_width / 2;
                        user3.y = canvas_height / 2;
                        user3.width = canvas_width / 2;
                        user3.height = canvas_height / 2;
                        user3.uid = uid;
                        liveTranscoding.addUser(user3);
                        break;
                    default:
                        Log.i(TAG, "ignored user as only 2x2 video layout supported in this demo. uid:" + uid);
                }
            }
        }
        engine.updateRtmpTranscoding(liveTranscoding);
    }

    private final IDirectCdnStreamingEventHandler iDirectCdnStreamingEventHandler = new IDirectCdnStreamingEventHandler() {


        @Override
        public void onDirectCdnStreamingStateChanged(DirectCdnStreamingState state, DirectCdnStreamingReason reason, String message) {
            showShortToast(String.format("onDirectCdnStreamingStateChanged state:%s, error:%s", state, reason));
            runOnUIThread(new Runnable() {
                @Override
                public void run() {
                    switch (state) {
                        case RUNNING:
                            streamingButton.setText(R.string.stop_streaming);
                            cdnStreaming = true;
                            break;
                        case STOPPED:
                            if (rtcStreaming) {
                                // Switch to RTC streaming when direct CDN streaming completely stopped.
                                ChannelMediaOptions channelMediaOptions = new ChannelMediaOptions();
                                channelMediaOptions.publishMicrophoneTrack = true;
                                channelMediaOptions.publishCameraTrack = true;
                                channelMediaOptions.clientRoleType = CLIENT_ROLE_BROADCASTER;
                                int ret = engine.joinChannel(null, channel, localUid, channelMediaOptions);
                                if (ret != 0) {
                                    showLongToast(String.format("Join Channel call failed! reason:%d", ret));
                                }
                            } else {
                                streamingButton.setText(getString(R.string.start_live_streaming));
                                cdnStreaming = false;
                            }
                            break;
                        case FAILED:
                            showLongToast(String.format("Start Streaming failed, please go back to previous page and check the settings."));
                        default:
                            Log.i(TAG, String.format("onDirectCdnStreamingStateChanged, state: %s error: %s message: %s", state.name(), reason.name(), message));
                    }
                    rtcSwitcher.setEnabled(true);
                }
            });
        }

        @Override
        public void onDirectCdnStreamingStats(DirectCdnStreamingStats directCdnStreamingStats) {

        }
    };

    private final SeekBar.OnSeekBarChangeListener seekBarChangeListener = new SeekBar.OnSeekBarChangeListener() {

        @Override
        public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
            engine.adjustRecordingSignalVolume(i);
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {

        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {

        }
    };

    private final CompoundButton.OnCheckedChangeListener checkedChangeListener = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
            rtcStreaming = b;
            if (rtcStreaming) {
                engine.stopDirectCdnStreaming();
            } else if (cdnStreaming) {
                engine.stopRtmpStream(getUrl());
            }
            handler.post(new Runnable() {
                @Override
                public void run() {
                    toggleVideoLayout(rtcStreaming);
                }
            });
        }
    };

    private void toggleVideoLayout(boolean isMultiple) {
        if (isMultiple) {
            fl_remote.setLayoutParams(new LinearLayout.LayoutParams(0, FrameLayout.LayoutParams.MATCH_PARENT, 0.5f));
            fl_remote_2.setLayoutParams(new LinearLayout.LayoutParams(0, FrameLayout.LayoutParams.MATCH_PARENT, 0.5f));
            fl_remote_3.setLayoutParams(new LinearLayout.LayoutParams(0, FrameLayout.LayoutParams.MATCH_PARENT, 0.5f));
            video_row2.setLayoutParams(new LinearLayout.LayoutParams(FrameLayout.LayoutParams.MATCH_PARENT, 0, 1));
        } else {
            fl_remote.setLayoutParams(new LinearLayout.LayoutParams(0, FrameLayout.LayoutParams.MATCH_PARENT, 0));
            fl_remote_2.setLayoutParams(new LinearLayout.LayoutParams(0, FrameLayout.LayoutParams.MATCH_PARENT, 0));
            fl_remote_3.setLayoutParams(new LinearLayout.LayoutParams(0, FrameLayout.LayoutParams.MATCH_PARENT, 0));
            video_row2.setLayoutParams(new LinearLayout.LayoutParams(FrameLayout.LayoutParams.MATCH_PARENT, 0, 0));
        }
    }

    private ViewGroup getAvailableView() {
        if (fl_remote.getChildCount() == 0) {
            return fl_remote;
        } else if (fl_remote_2.getChildCount() == 0) {
            return fl_remote_2;
        } else if (fl_remote_3.getChildCount() == 0) {
            return fl_remote_3;
        } else {
            return fl_remote;
        }
    }
}
