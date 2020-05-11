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
        name = "JoinChannel",
        actionId = R.id.action_mainFragment_to_joinChannel
)
public class JoinChannel extends BaseFragment implements View.OnClickListener {
    private FrameLayout fl_local, fl_remote;
    private EditText et_channel;

    private Handler handler;
    protected RtcEngine engine;
    protected int myUid;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        handler = new Handler();
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_join_channel, container, false);
        initView(view);
        return view;
    }

    protected void initView(View view) {
        fl_local = view.findViewById(R.id.fl_local);
        fl_remote = view.findViewById(R.id.fl_remote);
        et_channel = view.findViewById(R.id.et_channel);
        view.findViewById(R.id.btn_join).setOnClickListener(this);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        // release the RtcEngine
        handler.post(RtcEngine::destroy);
        engine = null;
    }

    protected void joinChannel(String channelId) {
        Context context = getContext();
        if (context == null) return;

        try {
            // 1. Create the RtcEngine with Agora app id
            // Note: Please use ApplicationContext instead of ActivityContext
            engine = RtcEngine.create(context.getApplicationContext(), getString(R.string.agora_app_id), eventHandler);
        } catch (Exception e) {
            e.printStackTrace();
        }

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

        // Start joining channel
        // 1. Users can only see each other after they join the same channel successfully using the same app id
        // 2. If app certificate is turned on at dashboard, token is needed when joining channel
        // The channel name and uid used to calculate the token has to match the ones used for channel join
        int res = engine.joinChannel(getString(R.string.agora_access_token), channelId, null, 0);
        if (res != 0) {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
            showAlert(RtcEngine.getErrorDescription(Math.abs(res)));
        }
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_join) { // call when join button hit
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
            ).onGranted(permissions -> { // Permissions Granted
                joinChannel(channelId);
            }).start();
        }
    }

    private IRtcEngineEventHandler eventHandler = new IRtcEngineEventHandler() {
        private final String TAG = IRtcEngineEventHandler.class.getSimpleName();

        @Override
        public void onWarning(int warn) {
            Log.w(TAG, String.format("onWarning code %d message %s", warn, RtcEngine.getErrorDescription(warn)));
        }

        @Override
        public void onError(int err) {
            Log.e(TAG, String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
        }

        @Override
        public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
            Log.i(TAG, String.format("onJoinChannelSuccess channel %s uid %d", channel, uid));
            myUid = uid;
        }

        @Override
        public void onLeaveChannel(RtcStats stats) {
            Log.i(TAG, "onLeaveChannel");
        }

        @Override
        public void onUserJoined(int uid, int elapsed) {
            Log.i(TAG, String.format("onUserJoined uid %d", uid));

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
            Log.i(TAG, String.format("onUserOffline uid %d reason %d", uid, reason));

            // Clear render view
            // Note: The video will stay at its last frame, to completely remove it you will need to remove the SurfaceView from its parent
            engine.setupRemoteVideo(new VideoCanvas(null, RENDER_MODE_HIDDEN, uid));
        }
    };
}
