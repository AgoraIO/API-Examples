package io.agora.advancedvideo.externvideosource;

import android.content.Context;
import android.content.Intent;
import android.graphics.SurfaceTexture;
import android.opengl.EGLSurface;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLUtils;
import android.os.Build;
import android.util.Log;
import android.util.Size;
import android.view.Surface;

import androidx.annotation.RequiresApi;

import io.agora.advancedvideo.externvideosource.localvideo.LocalVideoInput;
import io.agora.advancedvideo.externvideosource.screenshare.ScreenShareInput;
import io.agora.api.component.gles.ProgramTextureOES;
import io.agora.api.component.gles.core.EglCore;
import io.agora.api.component.gles.core.GlUtil;
import io.agora.rtc.mediaio.IVideoFrameConsumer;
import io.agora.rtc.mediaio.IVideoSource;
import io.agora.rtc.mediaio.MediaIO;

import static io.agora.api.component.Constant.ENGINE;
import static io.agora.api.component.Constant.TEXTUREVIEW;

/**
 * {@link IVideoSource}
 * The IVideoSource interface defines a set of protocols to implement the custom video source and
 * pass it to the underlying media engine to replace the default video source.
 * By default, when enabling real-time communications, the Agora SDK enables the default video input
 * device (built-in camera) to start video streaming. The IVideoSource interface defines a set of
 * protocols to create customized video source objects and pass them to the media engine to replace
 * the default camera source so that you can take ownership of the video source and manipulate it.
 * Once you implement this interface, the Agora Media Engine automatically releases its ownership of
 * the current video input device and pass it on to you, so that you can use the same video input
 * device to capture the video stream.
 */
public class ExternalVideoInputManager implements IVideoSource {
    private static final String TAG = ExternalVideoInputManager.class.getSimpleName();

    public static final int TYPE_LOCAL_VIDEO = 1;
    public static final int TYPE_SCREEN_SHARE = 2;
    public static final int TYPE_AR_CORE = 3;

    public static final String FLAG_VIDEO_PATH = "flag-local-video";
    public static final String FLAG_SCREEN_WIDTH = "screen-width";
    public static final String FLAG_SCREEN_HEIGHT = "screen-height";
    public static final String FLAG_SCREEN_DPI = "screen-dpi";
    public static final String FLAG_FRAME_RATE = "screen-frame-rate";

    private static final int DEFAULT_SCREEN_WIDTH = 640;
    private static final int DEFAULT_SCREEN_HEIGHT = 480;
    private static final int DEFAULT_SCREEN_DPI = 3;
    private static final int DEFAULT_FRAME_RATE = 15;

    private ExternalVideoInputThread mThread;
    private int mCurInputType;
    private volatile IExternalVideoInput mCurVideoInput;
    private volatile IExternalVideoInput mNewVideoInput;

    // RTC video interface to send video
    private volatile IVideoFrameConsumer mConsumer;

    private Context context;

    public ExternalVideoInputManager(Context context) {
        this.context = context;
    }

    void start() {
        mThread = new ExternalVideoInputThread();
        mThread.start();
    }

    boolean setExternalVideoInput(int type, Intent intent) {
        // Do not reset current input if the target type is
        // the same as the current which is still running.
        if (mCurInputType == type && mCurVideoInput != null
                && mCurVideoInput.isRunning()) {
            return false;
        }

        IExternalVideoInput input;
        switch (type) {
            case TYPE_LOCAL_VIDEO:
                input = new LocalVideoInput(intent.getStringExtra(FLAG_VIDEO_PATH));
                if (TEXTUREVIEW != null) {
                    TEXTUREVIEW.setSurfaceTextureListener((LocalVideoInput) input);
                }
                break;
            case TYPE_SCREEN_SHARE:
                int width = intent.getIntExtra(FLAG_SCREEN_WIDTH, DEFAULT_SCREEN_WIDTH);
                int height = intent.getIntExtra(FLAG_SCREEN_HEIGHT, DEFAULT_SCREEN_HEIGHT);
                int dpi = intent.getIntExtra(FLAG_SCREEN_DPI, DEFAULT_SCREEN_DPI);
                int fps = intent.getIntExtra(FLAG_FRAME_RATE, DEFAULT_FRAME_RATE);
                Log.i(TAG, "ScreenShare:" + width + "|" + height + "|" + dpi + "|" + fps);
                input = new ScreenShareInput(context, width, height, dpi, fps, intent);
                break;
            default:
                input = null;
        }

        setExternalVideoInput(input);
        mCurInputType = type;
        return true;
    }

    private void setExternalVideoInput(IExternalVideoInput source) {
        if (mThread != null && mThread.isAlive()) {
            mThread.pauseThread();
        }
        mNewVideoInput = source;
    }

    void stop() {
        mThread.setThreadStopped();
    }

    /**
     * This callback initializes the video source. You can enable the camera or initialize the video
     * source and then pass one of the following return values to inform the media engine whether
     * the video source is ready.
     *
     * @param consumer The IVideoFrameConsumer object which the media engine passes back. You need
     *                 to reserve this object and pass the video frame to the media engine through
     *                 this object once the video source is initialized. See the following contents
     *                 for the definition of IVideoFrameConsumer.
     * @return true: The external video source is initialized.
     * false: The external video source is not ready or fails to initialize, the media engine stops
     * and reports the error.
     * PS:
     * When initializing the video source, you need to specify a buffer type in the getBufferType
     * method and pass the video source in the specified type to the media engine.
     */
    @Override
    public boolean onInitialize(IVideoFrameConsumer consumer) {
        mConsumer = consumer;
        return true;
    }

    /**
     * The SDK triggers this callback when the underlying media engine is ready to start video streaming.
     * You should start the video source to capture the video frame. Once the frame is ready, use
     * IVideoFrameConsumer to consume the video frame.
     *
     * @return true: The external video source is enabled and the SDK calls IVideoFrameConsumer to receive
     * video frames.
     * false: The external video source is not ready or fails to enable, the media engine stops and
     * reports the error.
     */
    @Override
    public boolean onStart() {
        return true;
    }

    /**
     * The SDK triggers this callback when the media engine stops streaming. You should then stop
     * capturing and consuming the video frame. After calling this method, the video frames are
     * discarded by the media engine.
     */
    @Override
    public void onStop() {

    }

    /**
     * The SDK triggers this callback when IVideoFrameConsumer is released by the media engine. You
     * can now release the video source as well as IVideoFrameConsumer.
     */
    @Override
    public void onDispose() {
        Log.e(TAG, "SwitchExternalVideo-onDispose");
        mConsumer = null;
    }

    @Override
    public int getBufferType() {
        return MediaIO.BufferType.TEXTURE.intValue();
    }

    @Override
    public int getCaptureType() {
        return MediaIO.CaptureType.CAMERA.intValue();
    }

    @Override
    public int getContentHint() {
        return MediaIO.ContentHint.NONE.intValue();
    }


    private class ExternalVideoInputThread extends Thread
    {
        private final String TAG = ExternalVideoInputThread.class.getSimpleName();
        private final int DEFAULT_WAIT_TIME = 1;

        private EglCore mEglCore;
        private EGLSurface mEglSurface;
        private int mTextureId;
        private SurfaceTexture mSurfaceTexture;
        private Surface mSurface;
        private float[] mTransform = new float[16];
        private GLThreadContext mThreadContext;
        int mVideoWidth;
        int mVideoHeight;
        private volatile boolean mStopped;
        private volatile boolean mPaused;

        private void prepare() {
            mEglCore = new EglCore();
            mEglSurface = mEglCore.createOffscreenSurface(1, 1);
            mEglCore.makeCurrent(mEglSurface);
            mTextureId = GlUtil.createTextureObject(GLES11Ext.GL_TEXTURE_EXTERNAL_OES);
            mSurfaceTexture = new SurfaceTexture(mTextureId);
            mSurface = new Surface(mSurfaceTexture);
            mThreadContext = new GLThreadContext();
            mThreadContext.eglCore = mEglCore;
            mThreadContext.context = mEglCore.getEGLContext();
            mThreadContext.program = new ProgramTextureOES();
            /**Customizes the video source.
             * Call this method to add an external video source to the SDK.*/
            ENGINE.setVideoSource(ExternalVideoInputManager.this);
        }

        private void release() {
            if (ENGINE == null) {
                return;
            }
            /**release external video source*/
            ENGINE.setVideoSource(null);
            mSurface.release();
            mEglCore.makeNothingCurrent();
            mEglCore.releaseSurface(mEglSurface);
            mSurfaceTexture.release();
            GlUtil.deleteTextureObject(mTextureId);
            mTextureId = 0;
            mEglCore.release();
        }

        @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
        @Override
        public void run() {
            prepare();

            while (!mStopped) {
                if (mCurVideoInput != mNewVideoInput) {
                    Log.i(TAG, "New video input selected");
                    // Current video input is running, but we now
                    // introducing a new video type.
                    // The new video input type may be null, referring
                    // that we are not using any video.
                    if (mCurVideoInput != null) {
                        mCurVideoInput.onVideoStopped(mThreadContext);
                        Log.i(TAG, "recycle stopped input");
                    }

                    mCurVideoInput = mNewVideoInput;
                    if (mCurVideoInput != null) {
                        mCurVideoInput.onVideoInitialized(mSurface);
                        Log.i(TAG, "initialize new input");
                    }

                    if (mCurVideoInput == null) {
                        continue;
                    }

                    Size size = mCurVideoInput.onGetFrameSize();
                    mVideoWidth = size.getWidth();
                    mVideoHeight = size.getHeight();
                    mSurfaceTexture.setDefaultBufferSize(mVideoWidth, mVideoHeight);

                    if (mPaused) {
                        // If current thread is in pause state, it must be paused
                        // because of switching external video sources.
                        mPaused = false;
                    }
                } else if (mCurVideoInput != null && !mCurVideoInput.isRunning()) {
                    // Current video source has been stopped by other
                    // mechanisms (video playing has completed, etc).
                    // A callback method is invoked to do some collect
                    // or release work.
                    // Note that we also set the new video source null,
                    // meaning at meantime, we are not introducing new
                    // video types.
                    Log.i(TAG, "current video input is not running");
                    mCurVideoInput.onVideoStopped(mThreadContext);
                    mCurVideoInput = null;
                    mNewVideoInput = null;
                }

                if (mPaused || mCurVideoInput == null) {
                    waitForTime(DEFAULT_WAIT_TIME);
                    continue;
                }

                try {
                    mSurfaceTexture.updateTexImage();
                    mSurfaceTexture.getTransformMatrix(mTransform);
                }
                catch (Exception e) {
                    e.printStackTrace();
                }

                if (mCurVideoInput != null) {
                    mCurVideoInput.onFrameAvailable(mThreadContext, mTextureId, mTransform);
                }

                mEglCore.makeCurrent(mEglSurface);
                GLES20.glViewport(0, 0, mVideoWidth, mVideoHeight);

                if (mConsumer != null) {
                    Log.e(TAG, "publish stream with ->width:" + mVideoWidth + ",height:" + mVideoHeight);
                    /**Receives the video frame in texture,and push it out
                     * @param textureId ID of the texture
                     * @param format Pixel format of the video frame
                     * @param width Width of the video frame
                     * @param height Height of the video frame
                     * @param rotation Clockwise rotating angle (0, 90, 180, and 270 degrees) of the video frame
                     * @param timestamp Timestamp of the video frame. For each video frame, you need to set a timestamp
                     * @param matrix Matrix of the texture. The float value is between 0 and 1, such as 0.1, 0.2, and so on*/
                    mConsumer.consumeTextureFrame(mTextureId,
                            MediaIO.PixelFormat.TEXTURE_OES.intValue(),
                            mVideoWidth, mVideoHeight, 0,
                            System.currentTimeMillis(), mTransform);
                }

                // The pace at which the output Surface is sampled
                // for video frames is controlled by the waiting
                // time returned from the external video source.
                waitForNextFrame();
            }

            if (mCurVideoInput != null) {
                // The manager will cause the current
                // video source to be stopped.
                mCurVideoInput.onVideoStopped(mThreadContext);
            }
            release();
        }

        void pauseThread() {
            mPaused = true;
        }

        void setThreadStopped() {
            mStopped = true;
        }

        private void waitForNextFrame() {
            int wait = mCurVideoInput != null
                    ? mCurVideoInput.timeToWait()
                    : DEFAULT_WAIT_TIME;
            waitForTime(wait);
        }

        private void waitForTime(int time) {
            try {
                Thread.sleep(time);
            }
            catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
