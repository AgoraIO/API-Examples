package io.agora.api.example.examples.advanced;

import static io.agora.rtc2.video.VideoCanvas.RENDER_MODE_HIDDEN;
import static io.agora.rtc2.video.VideoEncoderConfiguration.STANDARD_BITRATE;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.EGLSurface;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.Matrix;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.io.IOException;
import java.util.concurrent.Callable;

import io.agora.api.example.MainApplication;
import io.agora.api.example.R;
import io.agora.api.example.common.BaseFragment;
import io.agora.api.example.common.gles.ProgramTextureOES;
import io.agora.api.example.common.gles.core.EglCore;
import io.agora.api.example.common.gles.core.GlUtil;
import io.agora.api.example.utils.CommonUtil;
import io.agora.api.example.utils.PermissonUtils;
import io.agora.api.example.utils.TokenUtils;
import io.agora.base.TextureBufferHelper;
import io.agora.base.VideoFrame;
import io.agora.base.internal.video.EglBase;
import io.agora.base.internal.video.EglBase14;
import io.agora.base.internal.video.RendererCommon;
import io.agora.base.internal.video.YuvConverter;
import io.agora.rtc2.ChannelMediaOptions;
import io.agora.rtc2.Constants;
import io.agora.rtc2.IRtcEngineEventHandler;
import io.agora.rtc2.RtcEngine;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;
import io.agora.rtc2.video.VideoCanvas;
import io.agora.rtc2.video.VideoEncoderConfiguration;

//@Example(
//        index = 7,
//        group = ADVANCED,
//        name = R.string.item_pushexternal,
//        actionId = R.id.action_mainFragment_to_PushExternalVideo,
//        tipsId = R.string.pushexternalvideo
//)

/**
 * The type Push external video.
 *
 * @deprecated The impletation of custom has been moved to {@link PushExternalVideoYUV}.             You can refer to {@link PushExternalVideoYUV} example.
 */
public class PushExternalVideo extends BaseFragment implements View.OnClickListener, TextureView.SurfaceTextureListener,
        SurfaceTexture.OnFrameAvailableListener {
    private static final String TAG = PushExternalVideo.class.getSimpleName();
    private final int DEFAULT_CAPTURE_WIDTH = 640;
    private final int DEFAULT_CAPTURE_HEIGHT = 480;

    private FrameLayout fl_local, fl_remote;
    private Button join;
    private EditText et_channel;
    private RtcEngine engine;
    private int myUid;
    private volatile boolean joined = false;

    private YuvConverter mYuvConverter = new YuvConverter();
    private Handler mHandler;
    private int mPreviewTexture;
    private SurfaceTexture mPreviewSurfaceTexture;
    private EglCore mEglCore;
    private EGLSurface mDummySurface;
    private EGLSurface mDrawSurface;
    private ProgramTextureOES mProgram;
    private float[] mTransform = new float[16];
    private float[] mMVPMatrix = new float[16];
    private boolean mMVPMatrixInit = false;
    private Camera mCamera;
    private int mFacing = Camera.CameraInfo.CAMERA_FACING_FRONT;
    private boolean mPreviewing = false;
    private int mSurfaceWidth;
    private int mSurfaceHeight;
    private boolean mTextureDestroyed;
    private volatile boolean glPrepared;
    private volatile TextureBufferHelper textureBufferHelper;

    private boolean prepareGl(EglBase.Context eglContext, final int width, final int height) {
        Log.d(TAG, "prepareGl");
        textureBufferHelper = TextureBufferHelper.create("STProcess", eglContext);
        if (textureBufferHelper == null) {
            return false;
        }
        Log.d(TAG, "prepareGl completed");
        return true;
    }

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
        }
        catch (Exception e) {
            e.printStackTrace();
            getActivity().onBackPressed();
        }
    }


    @Override
    public void onDestroy() {
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
            if (textureBufferHelper != null) {
                textureBufferHelper.dispose();
                textureBufferHelper = null;
            }
        }
        engine = null;
        super.onDestroy();
        handler.post(RtcEngine::destroy);
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
                        // Permissions Granted
                        if (allPermissionsGranted) {
                            joinChannel(channelId);
                        }
                    }
                });
            } else {
                fl_local.setVisibility(View.GONE);
                getActivity().onBackPressed();
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

        // Create render view by RtcEngine
        TextureView textureView = new TextureView(getContext());
        //add SurfaceTextureListener
        textureView.setSurfaceTextureListener(this);
        // Add to the local container
        fl_local.addView(textureView, new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT));
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

        /*Please configure accessToken in the string_config file.
         * A temporary token generated in Console. A temporary token is valid for 24 hours. For details, see
         *      https://docs.agora.io/en/Agora%20Platform/token?platform=All%20Platforms#get-a-temporary-token
         * A token generated at the server. This applies to scenarios with high-security requirements. For details, see
         *      https://docs.agora.io/en/cloud-recording/token_server_java?platform=Java*/
        TokenUtils.gen(requireContext(), channelId, 0, token -> {
            /* Allows a user to join a channel.
             if you do not specify the uid, we will generate the uid for you*/

            ChannelMediaOptions option = new ChannelMediaOptions();
            option.autoSubscribeAudio = true;
            option.autoSubscribeVideo = true;
            int res = engine.joinChannel(token, channelId, 0, option);
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

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        if (mTextureDestroyed) {
            return;
        }

        if (!mEglCore.isCurrent(mDrawSurface)) {
            mEglCore.makeCurrent(mDrawSurface);
        }
        try {
            surfaceTexture.updateTexImage();
            surfaceTexture.getTransformMatrix(mTransform);
        } catch (Exception e) {
            e.printStackTrace();
        }

        /*The rectangle ratio of frames and the screen surface may be different, so cropping may
         *  happen when display frames to the screen.
         * The display transformation matrix does not change for the same camera when the screen
         *  orientation remains the same.*/
        if (!mMVPMatrixInit) {
            /*For simplicity, we only consider the activity as portrait mode. In this case, the captured
             * images should be rotated 90 degrees (left or right).Thus the frame width and height
             * should be swapped.*/
            float frameRatio = DEFAULT_CAPTURE_HEIGHT / (float) DEFAULT_CAPTURE_WIDTH;
            float surfaceRatio = mSurfaceWidth / (float) mSurfaceHeight;
            Matrix.setIdentityM(mMVPMatrix, 0);

            if (frameRatio >= surfaceRatio) {
                float w = DEFAULT_CAPTURE_WIDTH * surfaceRatio;
                float scaleW = DEFAULT_CAPTURE_HEIGHT / w;
                Matrix.scaleM(mMVPMatrix, 0, scaleW, 1, 1);
            } else {
                float h = DEFAULT_CAPTURE_HEIGHT / surfaceRatio;
                float scaleH = DEFAULT_CAPTURE_WIDTH / h;
                Matrix.scaleM(mMVPMatrix, 0, 1, scaleH, 1);
            }
            mMVPMatrixInit = true;
        }
        GLES20.glViewport(0, 0, mSurfaceWidth, mSurfaceHeight);
        mProgram.drawFrame(mPreviewTexture, mTransform, mMVPMatrix);
        mEglCore.swapBuffers(mDrawSurface);

        if (joined) {
            VideoFrame.Buffer buffer = textureBufferHelper.invoke(new Callable<VideoFrame.Buffer>() {
                @Override
                public VideoFrame.Buffer call() throws Exception {
                    return textureBufferHelper.wrapTextureBuffer(DEFAULT_CAPTURE_HEIGHT,
                            DEFAULT_CAPTURE_WIDTH, VideoFrame.TextureBuffer.Type.OES, mPreviewTexture,
                            RendererCommon.convertMatrixToAndroidGraphicsMatrix(mTransform));
                }
            });
            VideoFrame frame = new VideoFrame(buffer, 0, 0);
            /*Pushes the video frame using the AgoraVideoFrame class and passes the video frame to the Agora SDK.
             * Call the setExternalVideoSource method and set pushMode as true before calling this
             * method. Otherwise, a failure returns after calling this method.
             * @param frame AgoraVideoFrame
             * @return
             *   true: The frame is pushed successfully.
             *   false: Failed to push the frame.
             * PS:
             *   In the Communication profile, the SDK does not support textured video frames.*/
            boolean a = engine.pushExternalVideoFrame(frame);
            Log.d(TAG, "pushExternalVideoFrame:" + a);
        }
    }

    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        Log.i(TAG, "onSurfaceTextureAvailable");
        mTextureDestroyed = false;
        mSurfaceWidth = width;
        mSurfaceHeight = height;
        /* handler associate to the GL thread which creates the texture.
         * in some condition SDK need to convert from texture format to YUV format, in this case,
         * SDK will use this handler to switch into the GL thread to complete the conversion.
         * */
        mHandler = new Handler(Looper.myLooper());
        mEglCore = new EglCore();
        if (!glPrepared) {
            // setup egl context
            EglBase.Context eglContext = new EglBase14.Context(mEglCore.getEGLContext());
            glPrepared = prepareGl(eglContext, width, height);
        }
        mDummySurface = mEglCore.createOffscreenSurface(1, 1);
        mEglCore.makeCurrent(mDummySurface);
        mPreviewTexture = GlUtil.createTextureObject(GLES11Ext.GL_TEXTURE_EXTERNAL_OES);
        mPreviewSurfaceTexture = new SurfaceTexture(mPreviewTexture);
        mPreviewSurfaceTexture.setOnFrameAvailableListener(this);
        mDrawSurface = mEglCore.createWindowSurface(surface);
        mProgram = new ProgramTextureOES();
        if (mCamera != null || mPreviewing) {
            Log.e(TAG, "Camera preview has been started");
            return;
        }
        try {
            mCamera = Camera.open(mFacing);
            /*It is assumed to capture images of resolution 640x480. During development, it should
             * be the most suitable supported resolution that best fits the scenario.*/
            Camera.Parameters parameters = mCamera.getParameters();
            parameters.setPreviewSize(DEFAULT_CAPTURE_WIDTH, DEFAULT_CAPTURE_HEIGHT);
            mCamera.setParameters(parameters);
            mCamera.setPreviewTexture(mPreviewSurfaceTexture);
            /*The display orientation is 90 for both front and back facing cameras using a surface
             * texture for the preview when the screen is in portrait mode.*/
            mCamera.setDisplayOrientation(90);
            mCamera.startPreview();
            mPreviewing = true;
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {

    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        Log.i(TAG, "onSurfaceTextureDestroyed");
        mTextureDestroyed = true;
        if (mCamera != null && mPreviewing) {
            mCamera.stopPreview();
            mPreviewing = false;
            mCamera.release();
            mCamera = null;
        }
        mProgram.release();
        mEglCore.releaseSurface(mDummySurface);
        mEglCore.releaseSurface(mDrawSurface);
        mEglCore.release();
        return true;
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {

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
                    /*Clear render view
                     Note: The video will stay at its last frame, to completely remove it you will need to
                     remove the SurfaceView from its parent*/
                    engine.setupRemoteVideo(new VideoCanvas(null, RENDER_MODE_HIDDEN, uid));
                }
            });
        }
    };
}
