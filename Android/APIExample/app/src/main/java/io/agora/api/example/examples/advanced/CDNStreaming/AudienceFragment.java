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
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.CompoundButton;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.Switch;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.common.BaseFragment;
import io.agora.mediaplayer.IMediaPlayer;
import io.agora.mediaplayer.IMediaPlayerObserver;
import io.agora.mediaplayer.data.CacheStatistics;
import io.agora.mediaplayer.data.PlayerPlaybackStats;
import io.agora.mediaplayer.data.PlayerUpdatedInfo;
import io.agora.mediaplayer.data.SrcInfo;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoEncoderConfiguration;

/**
 * The type Audience fragment.
 */
public class AudienceFragment extends BaseFragment implements IMediaPlayerObserver {
    private static final String TAG = AudienceFragment.class.getSimpleName();
    private static final String AGORA_CHANNEL_PREFIX = "rtmp://pull.webdemo.agoraio.cn/lbhd/";
    private boolean isAgoraChannel = true;
    private boolean rtcStreaming = false;
    private String channel;
    private FrameLayout fl_local, fl_remote, fl_remote_2, fl_remote_3;
    private Map<Integer, ViewGroup> remoteViews = new ConcurrentHashMap<Integer, ViewGroup>();
    private LinearLayout rtc_control, video_row2, channel_control, vol_control;
    private RtcEngine engine;
    private IMediaPlayer mediaPlayer;
    private SeekBar volSeekBar;
    private Switch rtcSwitcher;
    private Spinner channelSpinner;
    private AlertDialog mPlayerFailDialog;
    private AlertDialog mPlayerCompletedDialog;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_cdn_audience, container, false);
        Bundle bundle = this.getArguments();
        isAgoraChannel = bundle.getBoolean(getString(R.string.key_is_agora_channel));
        channel = bundle.getString(getString(R.string.key_channel_name));
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        fl_local = view.findViewById(R.id.fl_local);
        fl_remote = view.findViewById(R.id.fl_remote);
        fl_remote_2 = view.findViewById(R.id.fl_remote2);
        fl_remote_3 = view.findViewById(R.id.fl_remote3);
        channel_control = view.findViewById(R.id.channel_ctrl);
        channel_control.setVisibility(isAgoraChannel ? View.VISIBLE : View.INVISIBLE);
        rtc_control = view.findViewById(R.id.rtc_ctrl);
        rtc_control.setVisibility(isAgoraChannel ? View.VISIBLE : View.INVISIBLE);
        vol_control = view.findViewById(R.id.vol_bar);
        vol_control.setVisibility(View.INVISIBLE);
        video_row2 = view.findViewById(R.id.video_container_row2);
        rtcSwitcher = view.findViewById(R.id.rtc_switch);
        rtcSwitcher.setOnCheckedChangeListener(checkedChangeListener);
        volSeekBar = view.findViewById(R.id.record_vol);
        volSeekBar.setOnSeekBarChangeListener(seekBarChangeListener);
        channelSpinner = view.findViewById(R.id.channels_spinner);
        channelSpinner.setOnItemSelectedListener(itemSelectedListener);
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
            // Setup video encoding configs
            engine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                    ((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingDimensionObject(),
                    VideoEncoderConfiguration.FRAME_RATE.valueOf(((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingFrameRate()),
                    STANDARD_BITRATE,
                    VideoEncoderConfiguration.ORIENTATION_MODE.valueOf(((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingOrientation())
            ));
            /* setting the local access point if the private cloud ip was set, otherwise the config will be invalid.*/
            LocalAccessPointConfiguration localAccessPointConfiguration = ((MainApplication) getActivity().getApplication()).getGlobalSettings().getPrivateCloudConfig();
            if (localAccessPointConfiguration != null) {
                // This api can only be used in the private media server scenario, otherwise some problems may occur.
                engine.setLocalAccessPoint(localAccessPointConfiguration);
            }
            engine.enableVideo();
            //prepare media player
            mediaPlayer = engine.createMediaPlayer();
            mediaPlayer.registerPlayerObserver(this);
            SurfaceView surfaceView = new SurfaceView(this.getActivity());
            surfaceView.setZOrderMediaOverlay(false);
            if (fl_local.getChildCount() > 0) {
                fl_local.removeAllViews();
            }
            fl_local.addView(surfaceView);
            // Setup local video to render your local media player view
            VideoCanvas videoCanvas = new VideoCanvas(surfaceView, Constants.RENDER_MODE_HIDDEN, 0);
            videoCanvas.sourceType = Constants.VIDEO_SOURCE_MEDIA_PLAYER;
            videoCanvas.mediaPlayerId = mediaPlayer.getMediaPlayerId();
            engine.setupLocalVideo(videoCanvas);
            // Your have to call startPreview to see player video
            engine.startPreview();
            // Set audio route to microPhone
            engine.setDefaultAudioRoutetoSpeakerphone(true);
            openPlayerWithUrl();
        } catch (Exception e) {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }

    private void openPlayerWithUrl() {
        if (isAgoraChannel) {
            mediaPlayer.openWithAgoraCDNSrc(getUrl(), 0);
        } else {
            mediaPlayer.open(getUrl(), 0);
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (rtcStreaming) {
            engine.leaveChannel();
        }
        mediaPlayer.stop();
        /*leaveChannel and Destroy the RtcEngine instance*/
        engine.stopPreview();
        handler.post(RtcEngine::destroy);
        engine = null;
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
            showLongToast(String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            handler.post(new Runnable() {
                @Override
                public void run() {
                    vol_control.setVisibility(View.VISIBLE);
                    volSeekBar.setProgress(100);
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
                        surfaceView.setZOrderOnTop(true);
                        ViewGroup view = getAvailableView();
                        remoteViews.put(uid, view);
                        // Add to the remote container
                        view.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
                        // Setup remote video to render
                        engine.setupRemoteVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, uid));
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
                }
            });
        }

        @Override
        public void onRtmpStreamingStateChanged(String url, int state, int errCode) {
            super.onRtmpStreamingStateChanged(url, state, errCode);
            showLongToast(String.format("onRtmpStreamingStateChanged state %s errCode %s", state, errCode));

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
                ChannelMediaOptions channelMediaOptions = new ChannelMediaOptions();
                channelMediaOptions.publishMicrophoneTrack = true;
                channelMediaOptions.publishCameraTrack = true;
                channelMediaOptions.clientRoleType = CLIENT_ROLE_BROADCASTER;
                int ret = engine.joinChannel(null, channel, 0, channelMediaOptions);
                if (ret != 0) {
                    showLongToast(String.format("Join Channel call failed! reason:%d", ret));
                }
            } else {
                remoteViews.clear();
                engine.leaveChannel();
                vol_control.setVisibility(View.INVISIBLE);
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
            // Create render view by RtcEngine
            SurfaceView surfaceView = new SurfaceView(getContext());
            if (fl_local.getChildCount() > 0) {
                fl_local.removeAllViews();
            }
            // Add to the local container
            fl_local.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
            // Setup local video to render your local camera preview
            engine.setupLocalVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, 0));
        } else {
            fl_remote.setLayoutParams(new LinearLayout.LayoutParams(0, FrameLayout.LayoutParams.MATCH_PARENT, 0));
            fl_remote_2.setLayoutParams(new LinearLayout.LayoutParams(0, FrameLayout.LayoutParams.MATCH_PARENT, 0));
            fl_remote_3.setLayoutParams(new LinearLayout.LayoutParams(0, FrameLayout.LayoutParams.MATCH_PARENT, 0));
            video_row2.setLayoutParams(new LinearLayout.LayoutParams(FrameLayout.LayoutParams.MATCH_PARENT, 0, 0));
            fl_remote.removeAllViews();
            fl_remote_2.removeAllViews();
            fl_remote_3.removeAllViews();
            SurfaceView surfaceView = new SurfaceView(getContext());
            surfaceView.setZOrderMediaOverlay(false);
            if (fl_local.getChildCount() > 0) {
                fl_local.removeAllViews();
            }
            fl_local.addView(surfaceView);
            // Setup local video to render your local media player view
            VideoCanvas videoCanvas = new VideoCanvas(surfaceView, Constants.RENDER_MODE_HIDDEN, 0);
            videoCanvas.sourceType = Constants.VIDEO_SOURCE_MEDIA_PLAYER;
            videoCanvas.mediaPlayerId = mediaPlayer.getMediaPlayerId();
            engine.setupLocalVideo(videoCanvas);
        }
        engine.startPreview();
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

    private String getUrl() {
        if (isAgoraChannel) {
            return AGORA_CHANNEL_PREFIX + channel;
        } else {
            return channel;
        }
    }

    @Override
    public void onPlayerStateChanged(io.agora.mediaplayer.Constants.MediaPlayerState state, io.agora.mediaplayer.Constants.MediaPlayerReason reason) {
        showShortToast("player state change to " + state.name());
        handler.post(new Runnable() {
            @Override
            public void run() {
                switch (state) {
                    case PLAYER_STATE_FAILED:
                        mediaPlayer.stop();
                        //showLongToast(String.format("media player error: %s", mediaPlayerError.name()));
                        if (mPlayerFailDialog == null) {
                            mPlayerFailDialog = new AlertDialog.Builder(requireContext())
                                    .setTitle(R.string.tip)
                                    .setCancelable(false)
                                    .setNegativeButton(R.string.cancel, (dialog, which) -> {
                                        dialog.dismiss();
                                        onBackPressed();
                                    })
                                    .setPositiveButton(R.string.confirm, (dialog, which) -> openPlayerWithUrl())
                                    .create();
                        }
                        mPlayerFailDialog.setMessage(getString(R.string.media_player_error, reason.name()) + "\n\n" + getString(R.string.reopen_url_again));
                        mPlayerFailDialog.show();
                        break;
                    case PLAYER_STATE_OPEN_COMPLETED:
                        mediaPlayer.play();
                        if (isAgoraChannel) {
                            loadAgoraChannels();
                        }
                        rtcSwitcher.setEnabled(true);
                        if (mPlayerFailDialog != null) {
                            mPlayerFailDialog.dismiss();
                        }
                        break;
                    case PLAYER_STATE_PLAYBACK_COMPLETED:
                        if (mPlayerCompletedDialog == null) {
                            mPlayerCompletedDialog = new AlertDialog.Builder(requireContext())
                                    .setTitle(R.string.tip)
                                    .setMessage(getString(R.string.media_player_complete) + "\n\n" + getString(R.string.reopen_url_again))
                                    .setNegativeButton(R.string.cancel, (dialog, which) -> {
                                        dialog.dismiss();
                                        onBackPressed();
                                    })
                                    .setCancelable(false)
                                    .setPositiveButton(R.string.confirm, (dialog, which) -> {
                                        mediaPlayer.stop();
                                        openPlayerWithUrl();
                                    })
                                    .create();
                        }
                        mPlayerCompletedDialog.show();
                        break;
                    case PLAYER_STATE_STOPPED:
                    default:
                        break;
                }
            }
        });
    }

    private void loadAgoraChannels() {
        int count = mediaPlayer.getAgoraCDNLineCount();
        ArrayAdapter<String> arrayAdapter = new ArrayAdapter<String>(getContext(), android.R.layout.simple_spinner_dropdown_item, getChannelArray(count));
        channelSpinner.setAdapter(arrayAdapter);
    }

    private List<String> getChannelArray(int count) {
        List<String> list = new ArrayList<>();
        for (int i = 0; i < count; i++) {
            list.add("Channel" + (i + 1));
        }
        return list;
    }

    @Override
    public void onPositionChanged(long positionMs, long timestampMs) {

    }

    @Override
    public void onPlayerEvent(io.agora.mediaplayer.Constants.MediaPlayerEvent mediaPlayerEvent, long l, String s) {
        Log.i(TAG, "onPlayerEvent " + mediaPlayerEvent.name());
        handler.post(new Runnable() {
            @Override
            public void run() {
                switch (mediaPlayerEvent) {
                    case PLAYER_EVENT_SWITCH_COMPLETE:
                        showLongToast(String.format("player switch channel completed"));
                        break;
                    case PLAYER_EVENT_SWITCH_ERROR:
                        showLongToast(String.format("player switch channel failed: %s", s));
                        break;
                    default:
                        break;
                }
            }
        });
    }

    @Override
    public void onMetaData(io.agora.mediaplayer.Constants.MediaPlayerMetadataType mediaPlayerMetadataType, byte[] bytes) {

    }

    @Override
    public void onPlayBufferUpdated(long l) {

    }

    @Override
    public void onPreloadEvent(String s, io.agora.mediaplayer.Constants.MediaPlayerPreloadEvent mediaPlayerPreloadEvent) {

    }


    @Override
    public void onAgoraCDNTokenWillExpire() {

    }

    @Override
    public void onPlayerSrcInfoChanged(SrcInfo srcInfo, SrcInfo srcInfo1) {

    }

    @Override
    public void onPlayerInfoUpdated(PlayerUpdatedInfo playerUpdatedInfo) {

    }

    @Override
    public void onPlayerCacheStats(CacheStatistics stats) {

    }

    @Override
    public void onPlayerPlaybackStats(PlayerPlaybackStats stats) {

    }

    @Override
    public void onAudioVolumeIndication(int i) {

    }

    private final AdapterView.OnItemSelectedListener itemSelectedListener = new AdapterView.OnItemSelectedListener() {
        @Override
        public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
            Log.i(TAG, "Start to switch cdn, current index is " + mediaPlayer.getAgoraCDNLineCount() + ". target index is " + i);
            mediaPlayer.switchAgoraCDNLineByIndex(i);
        }

        @Override
        public void onNothingSelected(AdapterView<?> adapterView) {

        }
    };

    @Override
    protected void onBackPressed() {

        if (rtcSwitcher.isChecked()) {
            rtcSwitcher.setChecked(false);
        } else {
            super.onBackPressed();
        }
    }
}
