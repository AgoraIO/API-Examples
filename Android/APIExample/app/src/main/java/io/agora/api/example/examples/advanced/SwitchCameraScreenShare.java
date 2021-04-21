package io.agora.api.example.examples.advanced;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.media.projection.MediaProjectionManager;
import android.os.Build;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.RelativeLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;

import io.agora.advancedvideo.externvideosource.ExternalVideoInputManager;
import io.agora.advancedvideo.externvideosource.ExternalVideoInputService;
import io.agora.advancedvideo.externvideosource.IExternalVideoInputService;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.utils.CommonUtil;
import io.agora.rtc.Constants;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.mediaio.AgoraDefaultSource;
import io.agora.rtc.models.ChannelMediaOptions;
import io.agora.rtc.video.VideoCanvas;
import io.agora.rtc.video.VideoEncoderConfiguration;

import static android.app.Activity.RESULT_OK;
import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc.Constants.REMOTE_VIDEO_STATE_STARTING;
import static io.agora.rtc.video.VideoCanvas.RENDER_MODE_FIT;
import static io.agora.rtc.video.VideoCanvas.RENDER_MODE_HIDDEN;
import static io.agora.api.component.Constant.ENGINE;
import static io.agora.api.component.Constant.TEXTUREVIEW;

/**
 * This example demonstrates how video can be flexibly switched between the camera stream and the
 * screen share stream during an audio-video call.
 */
@Example(
        index = 7,
        group = ADVANCED,
        name = R.string.item_cameraorscreen,
        actionId = R.id.action_mainFragment_to_SwitchCameraScreenShare,
        tipsId = R.string.switchcamerascreen
)
public class SwitchCameraScreenShare extends BaseFragment implements View.OnClickListener {
    private static final String TAG = SwitchCameraScreenShare.class.getSimpleName();

    private FrameLayout fl_remote;
    private RelativeLayout fl_local;
    private Button join, renderMode, camera, screenShare;
    private EditText et_channel;
    private int myUid, remoteUid = -1;
    private boolean joined = false;
    private static final int PROJECTION_REQ_CODE = 1 << 2;
    private static final int DEFAULT_SHARE_FRAME_RATE = 15;
    private IExternalVideoInputService mService;
    private VideoInputServiceConnection mServiceConnection;
    private int curRenderMode = RENDER_MODE_HIDDEN;
    private VideoEncoderConfiguration.ORIENTATION_MODE curMirrorMode =
            VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_ADAPTIVE;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_switch_camera_screenshare, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        join = view.findViewById(R.id.btn_join);
        renderMode = view.findViewById(R.id.renderMode);
        camera = view.findViewById(R.id.camera);
        screenShare = view.findViewById(R.id.screenShare);
        et_channel = view.findViewById(R.id.et_channel);
        fl_remote = view.findViewById(R.id.fl_remote);
        fl_local = view.findViewById(R.id.fl_local);
        join.setOnClickListener(this);
        renderMode.setOnClickListener(this);
        camera.setOnClickListener(this);
        screenShare.setOnClickListener(this);
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
            ENGINE = RtcEngine.create(context.getApplicationContext(), getString(R.string.agora_app_id), iRtcEngineEventHandler);
        }
        catch (Exception e) {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == PROJECTION_REQ_CODE && resultCode == RESULT_OK) {
            camera.setEnabled(true);
            screenShare.setEnabled(false);
            try {
                DisplayMetrics metrics = new DisplayMetrics();
                getActivity().getWindowManager().getDefaultDisplay().getMetrics(metrics);

                float percent = 0.f;
                float hp = ((float) metrics.heightPixels) - 1920.f;
                float wp = ((float) metrics.widthPixels) - 1080.f;

                if (hp < wp) {
                    percent = (((float) metrics.widthPixels) - 1080.f) / ((float) metrics.widthPixels);
                } else {
                    percent = (((float) metrics.heightPixels) - 1920.f) / ((float) metrics.heightPixels);
                }
                metrics.heightPixels = (int) (((float) metrics.heightPixels) - (metrics.heightPixels * percent));
                metrics.widthPixels = (int) (((float) metrics.widthPixels) - (metrics.widthPixels * percent));

                data.putExtra(ExternalVideoInputManager.FLAG_SCREEN_WIDTH, metrics.widthPixels);
                data.putExtra(ExternalVideoInputManager.FLAG_SCREEN_HEIGHT, metrics.heightPixels);
                data.putExtra(ExternalVideoInputManager.FLAG_SCREEN_DPI, (int) metrics.density);
                data.putExtra(ExternalVideoInputManager.FLAG_FRAME_RATE, DEFAULT_SHARE_FRAME_RATE);
                setVideoConfig(ExternalVideoInputManager.TYPE_SCREEN_SHARE, metrics.widthPixels, metrics.heightPixels);
                mService.setExternalVideoInput(ExternalVideoInputManager.TYPE_SCREEN_SHARE, data);
            }
            catch (RemoteException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void onDestroy() {
        unbindVideoService();
        TEXTUREVIEW = null;
        /**leaveChannel and Destroy the RtcEngine instance*/
        if (ENGINE != null) {
            ENGINE.leaveChannel();
        }
        handler.post(RtcEngine::destroy);
        ENGINE = null;
        super.onDestroy();
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_join) {
            if (!joined) {
                CommonUtil.hideInputBoard(getActivity(), et_channel);
                /**Instantiate the view ready to display the local preview screen*/
                TEXTUREVIEW = new TextureView(getContext());
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
                join.setText(getString(R.string.join));
                camera.setEnabled(false);
                screenShare.setEnabled(false);
                fl_remote.removeAllViews();
                fl_local.removeAllViews();
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
                ENGINE.leaveChannel();
                TEXTUREVIEW = null;
                unbindVideoService();
            }
        } else if (v.getId() == R.id.renderMode) {
            if (remoteUid == -1) {
                return;
            }
            if (curRenderMode == RENDER_MODE_HIDDEN) {
                curRenderMode = RENDER_MODE_FIT;
                renderMode.setText(String.format(getString(R.string.rendermode), getString(R.string.fit)));
            } else if (curRenderMode == RENDER_MODE_FIT) {
                curRenderMode = RENDER_MODE_HIDDEN;
                renderMode.setText(String.format(getString(R.string.rendermode), getString(R.string.hidden)));
            }
//            setRemotePreview(getContext());
//            ENGINE.setRemoteRenderMode(remoteUid, curRenderMode, curMirrorMode.getValue());
        } else if (v.getId() == R.id.camera) {
            unbindVideoService();
            handler.postDelayed(() -> {
                /**setVideoSource must be called in {@link ExternalVideoInputManager.ExternalVideoInputThread#release()}
                 * after calling. Here the handler delay is used to guarantee this process.
                 * Developers can flexibly call them according to their own business logic*/
                ENGINE.setVideoSource(new AgoraDefaultSource());
                addLocalPreview();
                camera.setEnabled(false);
                screenShare.setEnabled(true);
            }, 1000);
        } else if (v.getId() == R.id.screenShare) {
            if (Build.VERSION.SDK_INT > Build.VERSION_CODES.LOLLIPOP) {
                /**remove local preview*/
                fl_local.removeAllViews();
                /**start input service*/
                bindVideoService();
            } else {
                showAlert(getString(R.string.lowversiontip));
            }
        }
    }

    private void setVideoConfig(int sourceType, int width, int height) {
        switch (sourceType) {
            case ExternalVideoInputManager.TYPE_LOCAL_VIDEO:
            case ExternalVideoInputManager.TYPE_SCREEN_SHARE:
                curMirrorMode = VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_FIXED_PORTRAIT;
                break;
            default:
                curMirrorMode = VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_ADAPTIVE;
                break;
        }

        Log.e(TAG, "SDK encoding ->width:" + width + ",height:" + height);
        /**Setup video stream encoding configs*/
        ENGINE.setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                new VideoEncoderConfiguration.VideoDimensions(width, height),
                VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15,
                VideoEncoderConfiguration.STANDARD_BITRATE, curMirrorMode
        ));

    }

    private void addLocalPreview() {
        // Check if the context is valid
        Context context = getContext();
        if (context == null) {
            return;
        }

        // Create render view by RtcEngine
        SurfaceView surfaceView = RtcEngine.CreateRendererView(context);
        if (fl_local.getChildCount() > 0) {
            fl_local.removeAllViews();
        }
        // Add to the local container
        fl_local.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        // Setup local video to render your local camera preview
        ENGINE.setupLocalVideo(new VideoCanvas(surfaceView, RENDER_MODE_HIDDEN, 0));
    }

    private void setRemotePreview(Context context) {
        /**Display remote video stream*/
        SurfaceView remoteSurfaceView = RtcEngine.CreateRendererView(context);
        remoteSurfaceView.setZOrderMediaOverlay(true);
        if (fl_remote.getChildCount() > 0) {
            fl_remote.removeAllViews();
        }
        fl_remote.addView(remoteSurfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT));
        /**Setup remote video to render*/
        ENGINE.setupRemoteVideo(new VideoCanvas(remoteSurfaceView, curRenderMode, remoteUid));
    }

    private void joinChannel(String channelId) {
        addLocalPreview();

        ENGINE.setParameters("{\"che.video.mobile_1080p\":true}");
        /** Sets the channel profile of the Agora RtcEngine.
         CHANNEL_PROFILE_COMMUNICATION(0): (Default) The Communication profile.
         Use this profile in one-on-one calls or group calls, where all users can talk freely.
         CHANNEL_PROFILE_LIVE_BROADCASTING(1): The Live-Broadcast profile. Users in a live-broadcast
         channel have a role as either broadcaster or audience. A broadcaster can both send and receive streams;
         an audience can only receive streams.*/
        ENGINE.setChannelProfile(Constants.CHANNEL_PROFILE_LIVE_BROADCASTING);
        /**Sets the role of a user (Live Broadcast only).*/
        ENGINE.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
        /**Enable video module*/
        ENGINE.enableVideo();
        ENGINE.setVideoSource(new AgoraDefaultSource());
        /**Set up to play remote sound with receiver*/
        ENGINE.setDefaultAudioRoutetoSpeakerphone(false);
        ENGINE.setEnableSpeakerphone(false);

        /**Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        String accessToken = getString(R.string.agora_access_token);
        if (TextUtils.equals(accessToken, "") || TextUtils.equals(accessToken, "<#YOUR ACCESS TOKEN#>")) {
            accessToken = null;
        }
        /** Allows a user to join a channel.
         if you do not specify the uid, we will generate the uid for you*/

        ChannelMediaOptions option = new ChannelMediaOptions();
        option.autoSubscribeAudio = true;
        option.autoSubscribeVideo = true;
        int res = ENGINE.joinChannel(accessToken, channelId, "Extra Optional Data", 0, option);
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

    private void bindVideoService() {
        Intent intent = new Intent();
        intent.setClass(getContext(), ExternalVideoInputService.class);
        mServiceConnection = new VideoInputServiceConnection();
        getContext().bindService(intent, mServiceConnection, Context.BIND_AUTO_CREATE);
    }

    private void unbindVideoService() {
        if (mServiceConnection != null) {
            getContext().unbindService(mServiceConnection);
            mServiceConnection = null;
        }
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

        /**Reports an error during SDK runtime.
         * Error code: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html*/
        @Override
        public void onError(int err) {
            Log.e(TAG, String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
            showAlert(String.format("onError code %d message %s", err, RtcEngine.getErrorDescription(err)));
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
            handler.post(() -> {
                join.setEnabled(true);
                join.setText(getString(R.string.leave));
                camera.setEnabled(false);
                screenShare.setEnabled(true);
            });
        }

        @Override
        public void onLocalVideoStateChanged(int localVideoState, int error) {
            super.onLocalVideoStateChanged(localVideoState, error);
            if (localVideoState == 1) {
                Log.e(TAG, "launch successfully");
            }
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
            Log.i(TAG, "onRemoteVideoStateChanged:uid->" + uid + ", state->" + state);
            if (state == REMOTE_VIDEO_STATE_STARTING) {
                /**Check if the context is correct*/
                Context context = getContext();
                if (context == null) {
                    return;
                }
                handler.post(() ->
                {
                    remoteUid = uid;
                    renderMode.setEnabled(true);
                    renderMode.setText(String.format(getString(R.string.rendermode), getString(R.string.hidden)));
                    curRenderMode = RENDER_MODE_HIDDEN;
                    setRemotePreview(context);
                });
            }
        }

        @Override
        public void onRemoteVideoStats(RemoteVideoStats stats) {
            super.onRemoteVideoStats(stats);
            Log.d(TAG, "onRemoteVideoStats: width:" + stats.width + " x height:" + stats.height);
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
            handler.post(() -> {
                /**Clear render view
                 Note: The video will stay at its last frame, to completely remove it you will need to
                 remove the SurfaceView from its parent*/
                ENGINE.setupRemoteVideo(new VideoCanvas(null, RENDER_MODE_HIDDEN, uid));
                fl_remote.removeAllViews();
            });
        }
    };

    private class VideoInputServiceConnection implements ServiceConnection {
        @Override
        public void onServiceConnected(ComponentName componentName, IBinder iBinder) {
            mService = (IExternalVideoInputService) iBinder;
            /**Start the screen recording service of the system*/
            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP) {
                MediaProjectionManager mpm = (MediaProjectionManager)
                        getContext().getSystemService(Context.MEDIA_PROJECTION_SERVICE);
                Intent intent = mpm.createScreenCaptureIntent();
                startActivityForResult(intent, PROJECTION_REQ_CODE);
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mService = null;
        }
    }
}
