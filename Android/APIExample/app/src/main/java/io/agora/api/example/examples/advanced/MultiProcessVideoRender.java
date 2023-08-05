package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc2.Constants.CHANNEL_PROFILE_LIVE_BROADCASTING;
import static io.agora.rtc2.Constants.RENDER_MODE_HIDDEN;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Service;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Build;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import android.util.SparseArray;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.SurfaceHolder;
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

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import io.agora.api.example.IMultiProcessVideoRenderlInterface;
import io.agora.api.example.IRtcEventHandlerInterface;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.widget.VideoReportLayout;
import io.agora.api.example.examples.advanced.videoRender.SurfaceRender;
import io.agora.api.example.utils.CommonUtil;
import io.agora.api.example.utils.TokenUtils;
import io.agora.base.VideoFrame;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.video.IVideoFrameObserver;

/**
 * This demo demonstrates how to make a one-to-one video call
 */
@Example(
        index = 8,
        group = ADVANCED,
        name = R.string.item_multi_process_video_render,
        actionId = R.id.action_mainFragment_to_MutliProcessVideoRender,
        tipsId = R.string.joinchannelvideo
)
public class MultiProcessVideoRender extends BaseFragment implements View.OnClickListener {
    private static final String TAG = MultiProcessVideoRender.class.getSimpleName();

    private VideoReportLayout fl_local, fl_remote, fl_remote_2, fl_remote_3;
    private Button join, switch_camera;
    private EditText et_channel;
    private boolean joined = false;
    private Map<Integer, ViewGroup> remoteViews = new ConcurrentHashMap<Integer, ViewGroup>();

    private IMultiProcessVideoRenderlInterface mService;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_mutli_process_video_render, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        join = view.findViewById(R.id.btn_join);
        switch_camera = view.findViewById(R.id.btn_switch_camera);
        et_channel = view.findViewById(R.id.et_channel);
        view.findViewById(R.id.btn_join).setOnClickListener(this);
        switch_camera.setOnClickListener(this);
        fl_local = view.findViewById(R.id.fl_local);
        fl_remote = view.findViewById(R.id.fl_remote);
        fl_remote_2 = view.findViewById(R.id.fl_remote2);
        fl_remote_3 = view.findViewById(R.id.fl_remote3);
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        // Check if the context is valid
        Context context = getContext();
        if (context == null) {
            return;
        }

        Intent intent = new Intent(getContext(), MultiProcessVideoRenderService.class);
        context.bindService(intent, serviceConnection, Service.BIND_AUTO_CREATE);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        requireContext().unbindService(serviceConnection);

    }

    @SuppressLint("WrongConstant")
    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_join) {
            if (!joined) {
                CommonUtil.hideInputBoard(getActivity(), et_channel);
                // call when join button hit
                String channelId = et_channel.getText().toString();
                // Check permission
                List<String> permissionList = new ArrayList<>();
                permissionList.add(Permission.READ_EXTERNAL_STORAGE);
                permissionList.add(Permission.WRITE_EXTERNAL_STORAGE);
                permissionList.add(Permission.RECORD_AUDIO);
                permissionList.add(Permission.CAMERA);
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                    permissionList.add(Manifest.permission.BLUETOOTH_CONNECT);
                }

                String[] permissionArray = new String[permissionList.size()];
                permissionList.toArray(permissionArray);

                if (AndPermission.hasPermissions(this, permissionArray)) {
                    joinChannel(channelId);
                    return;
                }
                // Request permission
                AndPermission.with(this).runtime().permission(
                        permissionArray
                ).onGranted(permissions ->
                {
                    // Permissions Granted
                    joinChannel(channelId);
                }).start();
            } else {
                joined = false;
                try {
                    mService.leaveChannel();
                } catch (RemoteException e) {
                    throw new RuntimeException(e);
                }
                join.setText(getString(R.string.join));
                for (ViewGroup value : remoteViews.values()) {
                    value.removeAllViews();
                }
                remoteViews.clear();
            }
        } else if (v.getId() == switch_camera.getId()) {
            if (mService != null && joined) {
                try {
                    mService.switchCamera();
                } catch (RemoteException e) {
                    throw new RuntimeException(e);
                }
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
        if (fl_local.getChildCount() > 0) {
            fl_local.removeAllViews();
        }
        // Add to the local container
        fl_local.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                try {
                    mService.setupLocalVideo(holder.getSurface(), 0, 0, Constants.VIDEO_MIRROR_MODE_AUTO, RENDER_MODE_HIDDEN);
                } catch (RemoteException e) {
                    throw new RuntimeException(e);
                }
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
                try {
                    mService.setupLocalVideo(holder.getSurface(), width, height, Constants.VIDEO_MIRROR_MODE_AUTO, RENDER_MODE_HIDDEN);
                } catch (RemoteException e) {
                    throw new RuntimeException(e);
                }
            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                try {
                    mService.setupLocalVideo(null, 0, 0, Constants.VIDEO_MIRROR_MODE_AUTO, Constants.RENDER_MODE_HIDDEN);
                } catch (RemoteException e) {
                    throw new RuntimeException(e);
                }
            }
        });

        if (mService != null) {
            try {
                mService.joinChannel(channelId, 0, Constants.CLIENT_ROLE_BROADCASTER, iRtcEventHandlerInterface);
            } catch (RemoteException e) {
                throw new RuntimeException(e);
            }
        }
    }

    private VideoReportLayout getAvailableView() {
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

    private final ServiceConnection serviceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mService = IMultiProcessVideoRenderlInterface.Stub.asInterface(service);
            try {
                mService.initRtcEngine();
            } catch (RemoteException e) {
                throw new RuntimeException(e);
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            try {
                if (joined) {
                    mService.leaveChannel();
                }
                mService.destroyRtcEngine();
            } catch (RemoteException e) {
                throw new RuntimeException(e);
            }
        }
    };

    private final IRtcEventHandlerInterface.Stub iRtcEventHandlerInterface = new IRtcEventHandlerInterface.Stub() {

        @Override
        public void onUserJoined(int uid) throws RemoteException {
            showLongToast(String.format("user %d joined!", uid));
            /**Check if the context is correct*/
            Context context = getContext();
            if (context == null) {
                return;
            }
            if (remoteViews.containsKey(uid)) {
                return;
            }
            runOnUIThread(() ->
            {
                /**Display remote video stream*/
                SurfaceView surfaceView = null;
                // Create render view by RtcEngine
                surfaceView = new SurfaceView(context);
                surfaceView.setZOrderMediaOverlay(true);
                VideoReportLayout view = getAvailableView();
                view.setReportUid(uid);
                remoteViews.put(uid, view);
                // Add to the remote container
                view.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));

                surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
                    @Override
                    public void surfaceCreated(@NonNull SurfaceHolder holder) {
                        try {
                            mService.setupRemoteVideo(holder.getSurface(), 0, 0, uid, Constants.VIDEO_MIRROR_MODE_AUTO, Constants.RENDER_MODE_HIDDEN);
                        } catch (RemoteException e) {
                            throw new RuntimeException(e);
                        }
                    }

                    @Override
                    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
                        try {
                            mService.setupRemoteVideo(holder.getSurface(), width, height, uid, Constants.VIDEO_MIRROR_MODE_AUTO, Constants.RENDER_MODE_HIDDEN);
                        } catch (RemoteException e) {
                            throw new RuntimeException(e);
                        }
                    }

                    @Override
                    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                        try {
                            mService.setupRemoteVideo(null, 0, 0, uid, Constants.VIDEO_MIRROR_MODE_AUTO, Constants.RENDER_MODE_HIDDEN);
                        } catch (RemoteException e) {
                            throw new RuntimeException(e);
                        }
                    }
                });
            });
        }

        @Override
        public void onUserOffline(int uid) throws RemoteException {
            showLongToast(String.format("user %d offline!", uid));
            runOnUIThread(() -> {
                /**Clear render view
                 Note: The video will stay at its last frame, to completely remove it you will need to
                 remove the SurfaceView from its parent*/
                try {
                    mService.setupRemoteVideo(null, 0, 0, uid, Constants.VIDEO_MIRROR_MODE_AUTO, Constants.RENDER_MODE_FIT);
                } catch (RemoteException e) {
                    throw new RuntimeException(e);
                }
                remoteViews.get(uid).removeAllViews();
                remoteViews.remove(uid);
            });
        }

        @Override
        public void onJoinChannelSuccess(String channelName, int uid) throws RemoteException {
            showLongToast(String.format("onJoinChannelSuccess channel %s uid %d", channelName, uid));
            joined = true;
            runOnUIThread(() -> {
                join.setEnabled(true);
                join.setText(getString(R.string.leave));
                fl_local.setReportUid(uid);
            });
        }

    };

    public static class MultiProcessVideoRenderService extends Service {

        private RtcEngine engine;
        private IRtcEventHandlerInterface rtcEventHandlerInterface;
        private final SparseArray<SurfaceRender> surfaceRenders = new SparseArray<>();
        private static final int localSurfaceRenderKey = 0;


        @Nullable
        @Override
        public IBinder onBind(Intent intent) {
            return iMultiProcessVideoRenderlInterface;
        }


        private final IMultiProcessVideoRenderlInterface.Stub iMultiProcessVideoRenderlInterface = new IMultiProcessVideoRenderlInterface.Stub() {

            @Override
            public void initRtcEngine() throws RemoteException {
                try {
                    RtcEngineConfig config = new RtcEngineConfig();
                    /**
                     * The context of Android Activity
                     */
                    config.mContext = getApplicationContext();
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
                    config.mAudioScenario = Constants.AudioScenario.getValue(Constants.AudioScenario.DEFAULT);
                    engine = RtcEngine.create(config);

                    engine.registerVideoFrameObserver(iVideoFrameObserver);
                    // Enable video module
                    engine.enableVideo();

                } catch (Exception e) {
                    e.printStackTrace();
                }
            }

            @Override
            public void switchCamera() throws RemoteException {
                engine.switchCamera();
            }

            @Override
            public void destroyRtcEngine() throws RemoteException {
                RtcEngine.destroy();
                for (int i = 0; i < surfaceRenders.size(); i++) {
                    SurfaceRender surfaceRender = surfaceRenders.valueAt(i);
                    surfaceRender.release();
                }
                surfaceRenders.clear();
            }

            @Override
            public void joinChannel(String channelName, int uid, int roleType, IRtcEventHandlerInterface rtcEventHandler) throws RemoteException {

                ChannelMediaOptions option = new ChannelMediaOptions();
                option.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
                option.clientRoleType = roleType;
                option.autoSubscribeAudio = true;
                option.autoSubscribeVideo = true;

                /**Please configure accessToken in the string_config file.
                 * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
                 *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
                 * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
                 *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
                TokenUtils.gen(getApplicationContext(), channelName, uid, ret -> {

                    /** Allows a user to join a channel.
                     if you do not specify the uid, we will generate the uid for you*/
                    int res = engine.joinChannel(ret, channelName, uid, option);
                    if (res != 0) {
                        // Usually happens with invalid parameters
                        // Error code description can be found at:
                        // en: https://docs.agora.io/en/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                        // cn: https://docs.agora.io/cn/Voice/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_i_rtc_engine_event_handler_1_1_error_code.html
                        Log.e(TAG, RtcEngine.getErrorDescription(Math.abs(res)));
                        return;
                    }
                });

                rtcEventHandlerInterface = rtcEventHandler;
            }

            @Override
            public void leaveChannel() throws RemoteException {
                engine.leaveChannel();
            }

            @Override
            public void setupLocalVideo(Surface surface, int width, int height, int mirrorMode, int renderMode) throws RemoteException {
                SurfaceRender surfaceRender = surfaceRenders.get(localSurfaceRenderKey);
                if (surface == null) {
                    if (surfaceRender != null) {
                        surfaceRender.release();
                        surfaceRenders.remove(localSurfaceRenderKey);
                    }
                    return;
                }
                if (surfaceRender == null) {
                    surfaceRender = new SurfaceRender(surface);
                } else if (surfaceRender.getSurface() != surface) {
                    surfaceRender.release();
                    surfaceRender = new SurfaceRender(surface);
                }

                surfaceRender.updateSurfaceSize(width, height);
                surfaceRender.setMirrorMode(mirrorMode);
                surfaceRender.setRenderMode(renderMode);

                surfaceRenders.put(localSurfaceRenderKey, surfaceRender);
            }

            @Override
            public void setupRemoteVideo(Surface surface, int width, int height, int uid, int mirrorMode, int renderMode) throws RemoteException {
                SurfaceRender surfaceRender = surfaceRenders.get(uid);
                if (surface == null) {
                    if (surfaceRender != null) {
                        surfaceRender.release();
                        surfaceRenders.remove(uid);
                    }
                    return;
                }
                if (surfaceRender == null) {
                    surfaceRender = new SurfaceRender(surface);
                } else if (surfaceRender.getSurface() != surface) {
                    surfaceRender.release();
                    surfaceRender = new SurfaceRender(surface);
                }

                surfaceRender.updateSurfaceSize(width, height);
                surfaceRender.setMirrorMode(mirrorMode);
                surfaceRender.setRenderMode(renderMode);

                surfaceRenders.put(uid, surfaceRender);
            }

        };


        private final IVideoFrameObserver iVideoFrameObserver = new IVideoFrameObserver() {
            @Override
            public boolean onCaptureVideoFrame(int sourceType, VideoFrame videoFrame) {
                SurfaceRender surfaceRender = surfaceRenders.get(localSurfaceRenderKey);
                if (surfaceRender != null) {
                    surfaceRender.render(videoFrame);
                }
                return true;
            }

            @Override
            public boolean onPreEncodeVideoFrame(int sourceType, VideoFrame videoFrame) {
                return true;
            }

            @Override
            public boolean onMediaPlayerVideoFrame(VideoFrame videoFrame, int mediaPlayerId) {
                return false;
            }

            @Override
            public boolean onRenderVideoFrame(String channelId, int uid, VideoFrame videoFrame) {
                SurfaceRender surfaceRender = surfaceRenders.get(uid);
                if (surfaceRender != null) {
                    surfaceRender.render(videoFrame);
                }
                return true;
            }

            @Override
            public int getVideoFrameProcessMode() {
                return IVideoFrameObserver.PROCESS_MODE_READ_ONLY;
            }

            @Override
            public int getVideoFormatPreference() {
                return 0;
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
                return IVideoFrameObserver.POSITION_POST_CAPTURER | IVideoFrameObserver.POSITION_PRE_RENDERER;
            }
        };

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
                super.onError(err);
                Log.e(TAG, "Error code:" + err + ", msg:" + RtcEngine.getErrorDescription(err));
            }

            /**Occurs when a user leaves the channel.
             * @param stats With this callback, the application retrieves the channel information,
             *              such as the call duration and statistics.*/
            @Override
            public void onLeaveChannel(RtcStats stats) {
                super.onLeaveChannel(stats);
                Log.i(TAG, "local user leaveChannel!");
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
                if (rtcEventHandlerInterface != null) {
                    try {
                        rtcEventHandlerInterface.onJoinChannelSuccess(channel, uid);
                    } catch (RemoteException e) {
                        throw new RuntimeException(e);
                    }
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
                if (rtcEventHandlerInterface != null) {
                    try {
                        rtcEventHandlerInterface.onUserJoined(uid);
                    } catch (RemoteException e) {
                        throw new RuntimeException(e);
                    }
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
                if (rtcEventHandlerInterface != null) {
                    try {
                        rtcEventHandlerInterface.onUserOffline(uid);
                    } catch (RemoteException e) {
                        throw new RuntimeException(e);
                    }
                }
            }

        };

    }
}
