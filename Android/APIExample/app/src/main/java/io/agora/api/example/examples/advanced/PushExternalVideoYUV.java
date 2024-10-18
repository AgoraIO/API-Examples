package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;
import static io.agora.rtc2.video.VideoCanvas.RENDER_MODE_FIT;
import static io.agora.rtc2.video.VideoCanvas.RENDER_MODE_HIDDEN;
import static io.agora.rtc2.video.VideoEncoderConfiguration.STANDARD_BITRATE;

import android.content.Context;
import android.graphics.Matrix;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.Spinner;
import android.widget.Switch;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.nio.ByteBuffer;
import java.util.concurrent.Callable;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.gles.GLThread;
import io.agora.api.example.common.gles.core.EglCore;
import io.agora.api.example.examples.advanced.videoRender.YuvFboProgram;
import io.agora.api.example.utils.CommonUtil;
import io.agora.api.example.utils.PermissonUtils;
import io.agora.api.example.utils.TokenUtils;
import io.agora.api.example.utils.VideoFileReader;
import io.agora.base.JavaI420Buffer;
import io.agora.base.NV12Buffer;
import io.agora.base.NV21Buffer;
import io.agora.base.TextureBuffer;
import io.agora.base.TextureBufferHelper;
import io.agora.base.VideoFrame;
import io.agora.base.internal.video.YuvHelper;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.RtcEngineEx;
import io.agora.rtc2.gl.EglBaseProvider;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoEncoderConfiguration;

/**
 * The type Push external video yuv.
 */
@Example(
        index = 7,
        group = ADVANCED,
        name = R.string.item_pushexternal,
        actionId = R.id.action_mainFragment_to_PushExternalVideo,
        tipsId = R.string.pushexternalvideo
)
public class PushExternalVideoYUV extends BaseFragment implements View.OnClickListener {
    private static final String TAG = PushExternalVideoYUV.class.getSimpleName();

    private FrameLayout fl_local, fl_remote;
    private Button join;
    private Switch switchExEglContext;
    private EditText et_channel;
    private RtcEngineEx engine;
    private Spinner sp_push_buffer_type;
    private int myUid;
    private volatile boolean joined = false;

    private VideoFileReader videoFileReader;

    private YuvFboProgram yuvFboProgram;
    private TextureBufferHelper textureBufferHelper;
    private EglCore eglCore;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_push_externalvideo, container, false);
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        join = view.findViewById(R.id.btn_join);
        et_channel = view.findViewById(R.id.et_channel);
        view.findViewById(R.id.btn_join).setOnClickListener(this);
        fl_local = view.findViewById(R.id.fl_local);
        fl_remote = view.findViewById(R.id.fl_remote);
        sp_push_buffer_type = view.findViewById(R.id.sp_buffer_type);

        switchExEglContext = view.findViewById(R.id.switch_ex_context);
        view.findViewById(R.id.btn_confirm).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                view.findViewById(R.id.fl_container_egl_select).setVisibility(View.GONE);
                //init egl context option
                Log.d(TAG, "isChecked =" + switchExEglContext.isChecked());
                if (switchExEglContext.isChecked()) {
                    if (eglCore == null) {
                        eglCore = new EglCore();
                    }
                    //once set the external egl context, you should use it until engine destroyed
                    int ret = engine.setExternalRemoteEglContext(eglCore.getEGLContext());
                    Log.d(TAG, "setExternalRemoteEglContext:  ret = " + ret);
                }
            }
        });
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
            config.mAudioScenario = Constants.AudioScenario.getValue(Constants.AudioScenario.DEFAULT);
            config.mAreaCode = ((MainApplication) getActivity().getApplication()).getGlobalSettings().getAreaCode();
            engine = (RtcEngineEx) RtcEngine.create(config);
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

        } catch (Exception e) {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }


    @Override
    public void onDestroy() {
        if (videoFileReader != null) {
            videoFileReader.stop();
        }
        /*leaveChannel and Destroy the RtcEngine instance*/
        if (engine != null) {
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
        }
        releaseTextureBuffer();
        engine = null;
        super.onDestroy();
        handler.post(RtcEngine::destroy);
    }

    private void releaseTextureBuffer() {
        if (textureBufferHelper != null) {
            textureBufferHelper.invoke(() -> {
                if (yuvFboProgram != null) {
                    yuvFboProgram.release();
                    yuvFboProgram = null;
                }
                return null;
            });
            textureBufferHelper.dispose();
            textureBufferHelper = null;
        }
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_join) {
            if (!joined) {
                CommonUtil.hideInputBoard(getActivity(), et_channel);
                // call when join button hit
                String channelId = et_channel.getText().toString();
                // Check permission
                checkOrRequestPermisson(new PermissonUtils.PermissionResultCallback() {
                    @Override
                    public void onPermissionsResult(boolean allPermissionsGranted, String[] permissions, int[] grantResults) {
                        if (allPermissionsGranted) {
                            // Permissions Granted
                            joinChannel(channelId);
                        }
                    }
                });
            } else {
                joined = false;
                join.setText(getString(R.string.join));
                if (videoFileReader != null) {
                    videoFileReader.stop();
                }
                fl_remote.removeAllViews();
                fl_local.removeAllViews();
                engine.leaveChannel();
                engine.stopPreview();
                releaseTextureBuffer();
            }
        }
    }

    private void joinChannel(String channelId) {
//        engine.setParameters("{\"rtc.log_filter\":65535}");
        // Check if the context is valid
        Context context = getContext();
        if (context == null) {
            return;
        }

        /*Set up to play remote sound with receiver*/
        engine.setDefaultAudioRoutetoSpeakerphone(true);

        /*In the demo, the default is to enter as the anchor.*/
        engine.setClientRole(Constants.CLIENT_ROLE_BROADCASTER);
        // Enables the video module.
        engine.enableVideo();
        // Setup video encoding configs
        engine.setVideoEncoderConfiguration(new VideoEncoderConfiguration(
                ((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingDimensionObject(),
                VideoEncoderConfiguration.FRAME_RATE.valueOf(((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingFrameRate()),
                STANDARD_BITRATE,
                VideoEncoderConfiguration.ORIENTATION_MODE.valueOf(((MainApplication) getActivity().getApplication()).getGlobalSettings().getVideoEncodingOrientation())
        ));
        /*Configures the external video source.
         * @param enable Sets whether or not to use the external video source:
         *                 true: Use the external video source.
         *                 false: Do not use the external video source.
         * @param useTexture Sets whether or not to use texture as an input:
         *                     true: Use texture as an input.
         *                     false: (Default) Do not use texture as an input.
         * @param pushMode
         *                   VIDEO_FRAME: Use the ENCODED_VIDEO_FRAME.
         *                   ENCODED_VIDEO_FRAME: Use the ENCODED_VIDEO_FRAME*/
        engine.setExternalVideoSource(true, true, Constants.ExternalVideoSourceType.VIDEO_FRAME);

        TextureView textureView = new TextureView(getContext());
        VideoCanvas local = new VideoCanvas(textureView, Constants.RENDER_MODE_FIT, 0);
        local.mirrorMode = Constants.VIDEO_MIRROR_MODE_DISABLED;
        local.sourceType = Constants.VIDEO_SOURCE_CUSTOM;
        engine.setupLocalVideo(local);
        // Add to the local container
        fl_local.removeAllViews();
        fl_local.addView(textureView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT));
        engine.startPreview(Constants.VideoSourceType.VIDEO_SOURCE_CUSTOM);
        /*Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        TokenUtils.gen(requireContext(), channelId, 0, accessToken -> {
            /* Allows a user to join a channel.
             if you do not specify the uid, we will generate the uid for you*/

            ChannelMediaOptions option = new ChannelMediaOptions();
            option.autoSubscribeAudio = true;
            option.autoSubscribeVideo = true;
            option.publishCustomVideoTrack = true;
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
        });


    }

    /**
     * Push video frame by i420.
     *
     * @param yuv    i420 data
     * @param width  width
     * @param height height
     */
    private void pushVideoFrameByI420(byte[] yuv, int width, int height) {
        JavaI420Buffer i420Buffer = JavaI420Buffer.allocate(width, height);
        i420Buffer.getDataY().put(yuv, 0, i420Buffer.getDataY().limit());
        i420Buffer.getDataU().put(yuv, i420Buffer.getDataY().limit(), i420Buffer.getDataU().limit());
        i420Buffer.getDataV().put(yuv, i420Buffer.getDataY().limit() + i420Buffer.getDataU().limit(), i420Buffer.getDataV().limit());

        /*
         * Get monotonic time in ms which can be used by capture time,
         * typical scenario is as follows:
         */
        long currentMonotonicTimeInMs = engine.getCurrentMonotonicTimeInMs();
        /*
         * Create a video frame to push.
         */
        VideoFrame videoFrame = new VideoFrame(i420Buffer, 0, currentMonotonicTimeInMs * 1000000);

        /*
         * Pushes the external video frame to the app.
         */
        boolean success = engine.pushExternalVideoFrame(videoFrame);

        i420Buffer.release();

        if (!success) {
            Log.w(TAG, "pushExternalVideoFrame error");
        }
    }

    /**
     * Push video frame by nv21.
     *
     * @param nv21   nv21
     * @param width  width
     * @param height height
     */
    private void pushVideoFrameByNV21(byte[] nv21, int width, int height) {

        VideoFrame.Buffer frameBuffer = new NV21Buffer(nv21, width, height, null);

        /*
         * Get monotonic time in ms which can be used by capture time,
         * typical scenario is as follows:
         */
        long currentMonotonicTimeInMs = engine.getCurrentMonotonicTimeInMs();
        /*
         * Create a video frame to push.
         */
        VideoFrame videoFrame = new VideoFrame(frameBuffer, 0, currentMonotonicTimeInMs * 1000000);

        /*
         * Pushes the external video frame to the app.
         */
        boolean success = engine.pushExternalVideoFrame(videoFrame);

        if (!success) {
            Log.w(TAG, "pushExternalVideoFrame error");
        }
    }

    /**
     * Push video frame by nv12.
     *
     * @param nv12   nv12 buffer.
     * @param width  width.
     * @param height height.
     */
    private void pushVideoFrameByNV12(ByteBuffer nv12, int width, int height) {
        VideoFrame.Buffer frameBuffer = new NV12Buffer(width, height, width, height, nv12, null);

        /*
         * Get monotonic time in ms which can be used by capture time,
         * typical scenario is as follows:
         */
        long currentMonotonicTimeInMs = engine.getCurrentMonotonicTimeInMs();
        /*
         * Create a video frame to push.
         */
        VideoFrame videoFrame = new VideoFrame(frameBuffer, 0, currentMonotonicTimeInMs * 1000000);

        /*
         * Pushes the external video frame to the app.
         */
        boolean success = engine.pushExternalVideoFrame(videoFrame);

        if (!success) {
            Log.w(TAG, "pushExternalVideoFrame error");
        }
    }

    /**
     * Push video frame by texture id.
     *
     * @param textureId   texture id.
     * @param textureType texture type. rgb or oes.
     * @param width       width.
     * @param height      height.
     */
    @GLThread
    private void pushVideoFrameByTexture(int textureId, VideoFrame.TextureBuffer.Type textureType, int width, int height) {
        VideoFrame.Buffer frameBuffer = new TextureBuffer(
                EglBaseProvider.getCurrentEglContext(),
                width,
                height,
                textureType,
                textureId,
                new Matrix(),
                null,
                null,
                null
        );

        /*
         * Get monotonic time in ms which can be used by capture time,
         * typical scenario is as follows:
         */
        long currentMonotonicTimeInMs = engine.getCurrentMonotonicTimeInMs();
        /*
         * Create a video frame to push.
         */
        VideoFrame videoFrame = new VideoFrame(frameBuffer, 0, currentMonotonicTimeInMs * 1000000);

        /*
         * Pushes the external video frame to the app.
         */
        boolean success = engine.pushExternalVideoFrame(videoFrame);

        if (!success) {
            Log.w(TAG, "pushExternalVideoFrame error");
        }
    }


    /**
     * IRtcEngineEventHandler is an abstract class providing default implementation.
     * The SDK uses this class to report to the app on SDK runtime events.
     */
    private final IRtcEngineEventHandler iRtcEngineEventHandler = new IRtcEngineEventHandler() {

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

                    if (videoFileReader == null) {
                        /*
                         * VideoFileReader can get nv21 buffer data of sample.yuv file in assets cyclically.
                         */
                        videoFileReader = new VideoFileReader(requireContext(), (yuv, width, height) -> {
                            if (joined && engine != null) {
                                String selectedItem = (String) sp_push_buffer_type.getSelectedItem();
                                /*
                                 * Below show how to create different type buffers.
                                 */
                                if ("NV21".equals(selectedItem)) {
                                    byte[] nv21 = yuv2nv21(yuv, width, height);
                                    pushVideoFrameByNV21(nv21, width, height);
                                } else if ("NV12".equals(selectedItem)) {
                                    ByteBuffer des = yuv2nv12(yuv, width, height);
                                    pushVideoFrameByNV12(des, width, height);
                                } else if ("Texture2D".equals(selectedItem)) {
                                    if (textureBufferHelper == null) {
                                        textureBufferHelper = TextureBufferHelper.create("PushExternalVideoYUV", EglBaseProvider.instance().getRootEglBase().getEglBaseContext());
                                    }
                                    textureBufferHelper.invoke((Callable<Void>) () -> {
                                        int textureId = yuv2texture(yuv, width, height);
                                        pushVideoFrameByTexture(textureId, VideoFrame.TextureBuffer.Type.RGB, width, height);
                                        return null;
                                    });
                                } else {
                                    // I420 type default
                                    pushVideoFrameByI420(yuv, width, height);
                                }
                            }
                        });
                    }
                    videoFileReader.start();

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
            handler.post(() -> {
                /*Display remote video stream*/
                // Create render view by RtcEngine
                SurfaceView surfaceView = new SurfaceView(context);
                surfaceView.setZOrderMediaOverlay(true);
                if (fl_remote.getChildCount() > 0) {
                    fl_remote.removeAllViews();
                }
                // Add to the remote container
                fl_remote.addView(surfaceView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                        ViewGroup.LayoutParams.MATCH_PARENT));
                // Setup remote video to render
                engine.setupRemoteVideo(new VideoCanvas(surfaceView, RENDER_MODE_FIT, uid));
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
                    /*Clear render view
                     Note: The video will stay at its last frame, to completely remove it you will need to
                     remove the SurfaceView from its parent*/
                    fl_remote.removeAllViews();
                    engine.setupRemoteVideo(new VideoCanvas(null, RENDER_MODE_HIDDEN, uid));
                }
            });
        }
    };

    /**
     * Yuv 2 texture id.
     * Run on gl thread.
     *
     * @param yuv    yuv
     * @param width  width
     * @param height heigh
     * @return rgba texture id
     */
    @GLThread
    private int yuv2texture(byte[] yuv, int width, int height) {
        if (yuvFboProgram == null) {
            yuvFboProgram = new YuvFboProgram();
        }
        return yuvFboProgram.drawYuv(yuv, width, height);
    }

    /**
     * Transform yuv to nv12
     *
     * @param yuv    yuv
     * @param width  width
     * @param height height
     * @return nv12
     */
    @NonNull
    private static ByteBuffer yuv2nv12(byte[] yuv, int width, int height) {
        int srcStrideY = width;
        int srcHeightY = height;
        int srcSizeY = srcStrideY * srcHeightY;
        ByteBuffer srcY = ByteBuffer.allocateDirect(srcSizeY);
        srcY.put(yuv, 0, srcSizeY);

        int srcStrideU = width / 2;
        int srcHeightU = height / 2;
        int srcSizeU = srcStrideU * srcHeightU;
        ByteBuffer srcU = ByteBuffer.allocateDirect(srcSizeU);
        srcU.put(yuv, srcSizeY, srcSizeU);

        int srcStrideV = width / 2;
        int srcHeightV = height / 2;
        int srcSizeV = srcStrideV * srcHeightV;
        ByteBuffer srcV = ByteBuffer.allocateDirect(srcSizeV);
        srcV.put(yuv, srcSizeY + srcSizeU, srcSizeV);

        int desSize = srcSizeY + srcSizeU + srcSizeV;
        ByteBuffer des = ByteBuffer.allocateDirect(desSize);
        YuvHelper.I420ToNV12(srcY, srcStrideY, srcU, srcStrideU, srcV, srcStrideV, des, width, height);
        return des;
    }

    /**
     * Transform yuv to nv21.
     *
     * @param yuv    yuv
     * @param width  width
     * @param height height
     * @return nv21
     */
    @NonNull
    private static byte[] yuv2nv21(byte[] yuv, int width, int height) {
        int srcStrideY = width;
        int srcHeightY = height;
        int srcSizeY = srcStrideY * srcHeightY;
        ByteBuffer srcY = ByteBuffer.allocateDirect(srcSizeY);
        srcY.put(yuv, 0, srcSizeY);

        int srcStrideU = width / 2;
        int srcHeightU = height / 2;
        int srcSizeU = srcStrideU * srcHeightU;
        ByteBuffer srcU = ByteBuffer.allocateDirect(srcSizeU);
        srcU.put(yuv, srcSizeY, srcSizeU);

        int srcStrideV = width / 2;
        int srcHeightV = height / 2;
        int srcSizeV = srcStrideV * srcHeightV;
        ByteBuffer srcV = ByteBuffer.allocateDirect(srcSizeV);
        srcV.put(yuv, srcSizeY + srcSizeU, srcSizeV);

        int desSize = srcSizeY + srcSizeU + srcSizeV;
        ByteBuffer des = ByteBuffer.allocateDirect(desSize);
        YuvHelper.I420ToNV12(srcY, srcStrideY, srcV, srcStrideV, srcU, srcStrideU, des, width, height);

        byte[] nv21 = new byte[desSize];
        des.position(0);
        des.get(nv21);
        return nv21;
    }

}
