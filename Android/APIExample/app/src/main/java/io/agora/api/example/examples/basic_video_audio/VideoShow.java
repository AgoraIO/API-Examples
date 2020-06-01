package io.agora.api.example.examples.basic_video_audio;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;

import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.video.VideoCanvas;
import io.agora.rtc.video.VideoEncoderConfiguration;

import static io.agora.rtc.video.VideoCanvas.RENDER_MODE_HIDDEN;
import static io.agora.rtc.video.VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15;
import static io.agora.rtc.video.VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_ADAPTIVE;
import static io.agora.rtc.video.VideoEncoderConfiguration.STANDARD_BITRATE;
import static io.agora.rtc.video.VideoEncoderConfiguration.VD_640x360;

@Example(
        group = "BASIC VIDEO/AUDIO",
        name = "VideoShow",
        actionId = R.id.action_mainFragment_to_videoShow
)
public class VideoShow extends JoinChannel implements View.OnClickListener {
    private FrameLayout fl_local, fl_remote;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        handler = new Handler();
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_video_show, container, false);
        initView(view);
        return view;
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState)
    {
        super.onActivityCreated(savedInstanceState);
    }

    @Override
    protected void initView(View view) {
        super.initView(view);
        fl_local = view.findViewById(R.id.fl_local);
        fl_remote = view.findViewById(R.id.fl_remote);
    }

    @Override
    protected void joinChannel(String channelId) {
        // Check if the context is valid
        Context context = getContext();
        if (context == null)
        {return;}

        // Enable video module
        engine.enableVideo();
        // Setup video encoding configs
        engine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                VD_640x360,
                FRAME_RATE_FPS_15,
                STANDARD_BITRATE,
                ORIENTATION_MODE_ADAPTIVE
        ));

        // Create render view by RtcEngine
        SurfaceView surfaceView = RtcEngine.CreateRendererView(context);
        // Local video is on the top
        surfaceView.setZOrderMediaOverlay(true);
        // Add to the local container
        fl_local.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        // Setup local video to render your local camera preview
        engine.setupLocalVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, 0));

        // Set audio route to speaker
        engine.setDefaultAudioRoutetoSpeakerphone(true);

        // join channel
        super.joinChannel(channelId);
    }

    @Override
    protected IRtcEngineEventHandler getEventHandler()
    {return eventHandler;}

    private IRtcEngineEventHandler eventHandler = new IRtcEngineEventHandler() {

        @Override
        public void onWarning(int warn)
        {
            joinChannelEventHandler.onWarning(warn);
        }

        @Override
        public void onError(int err)
        {
            joinChannelEventHandler.onError(err);
        }

        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed)
        {
            joinChannelEventHandler.onJoinChannelSuccess(channel, uid, elapsed);
        }

        @Override
        public void onLeaveChannel(RtcStats stats)
        {
            joinChannelEventHandler.onLeaveChannel(stats);
        }

        @Override
        public void onRemoteAudioStateChanged(int uid, int state, int reason, int elapsed)
        {
            joinChannelEventHandler.onRemoteAudioStateChanged(uid, state, reason, elapsed);
        }

        @Override
        public void onUserJoined(int uid, int elapsed) {
            // Priority call callback processing in the parent class
            joinChannelEventHandler.onUserJoined(uid, elapsed);

            Context context = getContext();
            if (context == null) return;
            handler.post(() -> {
                SurfaceView surfaceView = null;
                if (fl_remote.getChildCount() == 0) {
                    // Create render view by RtcEngine
                    surfaceView = RtcEngine.CreateRendererView(context);
                    // Add to the remote container
                    fl_remote.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
                } else {
                    View view = fl_remote.getChildAt(0);
                    if (view instanceof SurfaceView) {
                        surfaceView = (SurfaceView) view;
                    }
                }

                // Setup remote video to render
                engine.setupRemoteVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, uid));
            });
        }

        @Override
        public void onUserOffline(int uid, int reason) {
            // Priority call callback processing in the parent class
            joinChannelEventHandler.onUserOffline(uid, reason);

            // Clear render view
            // Note: The video will stay at its last frame, to completely remove it you will need to remove the SurfaceView from its parent
            engine.setupRemoteVideo(new VideoCanvas(null, RENDER_MODE_HIDDEN, uid));
        }
    };
}