package io.agora.rtc.ss.gles;

import android.graphics.Bitmap;
import android.graphics.SurfaceTexture;
import android.opengl.EGL14;
import android.opengl.EGLContext;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;
import android.view.TextureView;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.concurrent.atomic.AtomicInteger;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLRender {
    private static final String TAG = "GLRender";
    private static final boolean DEBUG_ENABLED = true;

    public static final int STATE_IDLE = 0;
    public static final int STATE_READY = 1;
    public static final int STATE_RELEASED = 2;

    private static final int MSG_TYPE_SURFACE_CREATED = 0;
    private static final int MSG_TYPE_SURFACE_CHANGED = 1;
    private static final int MSG_TYPE_DRAW_FRAME = 2;
    private static final int MSG_TYPE_QUIT = 3;

    private HandlerThread mGLHandlerThread;
    private Handler mGLHandler;

    private TextureView mTextureView;
    private EglCore mEglCore;
    private WindowSurface mWindowSurface;
    private EGLContext mEGLContext;
    private GLSurfaceView mGLSurfaceView;

    private AtomicInteger mState;
    private long mThreadId;

    private LinkedList<GLRender.GLRenderListener> mGLRenderListenerList;
    private final Object mRenderListenerLock = new Object();

    private LinkedList<Runnable> mEventTaskList;
    private final Object mEventLock = new Object();

    private LinkedList<Runnable> mGLDrawTaskList;
    private final Object mDrawLock = new Object();

    private final static int frameRate = 30;
    private long mLastFrameTime;


    private Runnable runnableDrawFrame = new Runnable() {
        public void run() {
            doDrawFrame();
        }
    };

    private GLSurfaceView.Renderer mGLRenderer = new GLSurfaceView.Renderer() {
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            surfaceCreated(true);
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            surfaceChanged(width, height);
        }

        public void onDrawFrame(GL10 gl) {
            drawFrame();
        }
    };

    private TextureView.SurfaceTextureListener mTextureListener = new TextureView.SurfaceTextureListener() {
        public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
            Log.d(TAG, "onSurfaceTextureAvailable " + surface + " " + width + " " + height);
            initHandlerThread();

            Message msg = Message.obtain(mGLHandler, MSG_TYPE_SURFACE_CREATED, surface);
            mGLHandler.sendMessage(msg);
            msg = Message.obtain(mGLHandler, MSG_TYPE_SURFACE_CHANGED, width, height);
            mGLHandler.sendMessage(msg);
        }

        public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
            Log.d(TAG, "onSurfaceTextureSizeChanged " + surface + " " + width + " " + height);
            Message msg = Message.obtain(mGLHandler, MSG_TYPE_SURFACE_CHANGED, width, height);
            mGLHandler.sendMessage(msg);
        }

        public boolean onSurfaceTextureDestroyed(SurfaceTexture st) {
            Log.d(TAG, "onSurfaceTextureDestroyed " + st);
            quit(st);
            return false;
        }

        public void onSurfaceTextureUpdated(SurfaceTexture st) {
        }
    };

    public GLRender() {
        doInit(EGL14.EGL_NO_CONTEXT);
    }

    public GLRender(EGLContext ctx) {
        doInit(ctx);
    }

    private void doInit(EGLContext ctx) {
        mState = new AtomicInteger(STATE_RELEASED);
        mGLRenderListenerList = new LinkedList<>();
        mEventTaskList = new LinkedList<>();
        mGLDrawTaskList = new LinkedList<>();
        mEGLContext = ctx;
    }

    public void init(int width, int height) {
        mState.set(STATE_IDLE);
        initHandlerThread();

        Message msg = Message.obtain(mGLHandler, MSG_TYPE_SURFACE_CREATED, width, height);
        mGLHandler.sendMessage(msg);

        msg = Message.obtain(mGLHandler, MSG_TYPE_SURFACE_CHANGED, width, height);
        mGLHandler.sendMessage(msg);
    }

    public void update(int width, int height) {
        Message msg = Message.obtain(mGLHandler, MSG_TYPE_SURFACE_CHANGED, width, height);
        mGLHandler.sendMessage(msg);
    }

    public void init(GLSurfaceView sv) {
        mState.set(STATE_IDLE);
        sv.setEGLContextClientVersion(2); // GLES 2.0
        sv.setRenderer(mGLRenderer);
        sv.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        mGLSurfaceView = sv;
    }

    public void init(TextureView tv) {
        mState.set(STATE_IDLE);
        tv.setSurfaceTextureListener(mTextureListener);
        mTextureView = tv;
    }

    public void addListener(GLRender.GLRenderListener listener) {
        synchronized (mRenderListenerLock) {
            if (!mGLRenderListenerList.contains(listener)) {
                mGLRenderListenerList.add(listener);
            }
        }
    }

    public void removeListener(GLRenderListener listener) {
        synchronized (mRenderListenerLock) {
            mGLRenderListenerList.remove(listener);
        }
    }

    public int getState() {
        return mState.get();
    }

    public EGLContext getEGLContext() {
        return mEGLContext;
    }

    public boolean isGLRenderThread() {
        return mThreadId == Thread.currentThread().getId();
    }

    public void onPause() {
        if (mGLSurfaceView != null) {
            mState.set(STATE_RELEASED);
            mGLSurfaceView.queueEvent(new Runnable() {
                public void run() {
                    quit();
                }
            });
            mGLSurfaceView.onPause();
        }
    }

    public void onResume() {
        if (mState.get() == STATE_RELEASED) {
            mState.set(STATE_IDLE);
        }

        if (mGLSurfaceView != null) {
            mGLSurfaceView.onResume();
        }
    }

    public void requestRender() {
        long tm = System.currentTimeMillis();
        long tmDiff = tm - mLastFrameTime;
        if (tmDiff < (1000 / frameRate)) {
            Log.v(TAG, "drawing too often, drop this frame... ");
            return;
        }
        mLastFrameTime = tm;

        if (mGLSurfaceView != null) {
            mGLSurfaceView.requestRender();
        }

        if (mGLHandler != null) {
            mGLHandler.sendEmptyMessage(MSG_TYPE_DRAW_FRAME);
        }
    }

    public void queueEvent(Runnable runnable) {
        if (mState.get() == STATE_IDLE) {
            Log.d(TAG, "glContext not ready, queue event: " + runnable);
            synchronized (mEventLock) {
                mEventTaskList.add(runnable);
            }
        } else if (mState.get() == STATE_READY) {
            if (mGLSurfaceView != null) {
                mGLSurfaceView.queueEvent(runnable);
                mGLSurfaceView.queueEvent(runnableDrawFrame);
            } else if (mGLHandler != null) {
                mGLHandler.post(runnable);
                mGLHandler.post(runnableDrawFrame);
            }
        } else {
            Log.d(TAG, "glContext lost, drop event: " + runnable);
        }
    }

    public void queueDrawFrameAppends(Runnable runnable) {
        if (mState.get() == STATE_READY) {
            synchronized (mDrawLock) {
                mGLDrawTaskList.add(runnable);
            }
        }
    }

    public void quit() {
        if (mTextureView == null && mGLSurfaceView == null && mGLHandlerThread != null) {
            mState.set(STATE_RELEASED);
            quit(null);
        }
    }

    private void surfaceCreated(boolean reInitCtx) {
        mState.set(STATE_READY);
        mThreadId = Thread.currentThread().getId();

        GLES20.glEnable(GLES20.GL_BLEND);
        GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA);

        if (reInitCtx && Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
            mEGLContext = EGL14.eglGetCurrentContext();
        }

        synchronized (mRenderListenerLock) {
            Iterator<GLRender.GLRenderListener> it = mGLRenderListenerList.iterator();

            while (it.hasNext()) {
                GLRender.GLRenderListener listener = it.next();
                listener.onReady();
            }
        }
    }

    private void surfaceChanged(int width, int height) {
        GLES20.glViewport(0, 0, width, height);

        synchronized (mRenderListenerLock) {
            Iterator<GLRender.GLRenderListener> it = mGLRenderListenerList.iterator();

            while (it.hasNext()) {
                GLRender.GLRenderListener listener = it.next();
                listener.onSizeChanged(width, height);
            }
        }
    }

    private void drawFrame() {
        Iterator<?> it;
        synchronized (mEventLock) {
            it = mEventTaskList.iterator();

            while (true) {
                if (!it.hasNext()) {
                    mEventTaskList.clear();
                    break;
                }

                Runnable runnable = (Runnable) it.next();
                runnable.run();
            }
        }

        synchronized (mRenderListenerLock) {
            it = mGLRenderListenerList.iterator();

            while (true) {
                if (!it.hasNext()) {
                    break;
                }

                GLRender.GLRenderListener listener = (GLRender.GLRenderListener) it.next();
                listener.onDrawFrame();
            }
        }

        doDrawFrame();
    }

    private void release() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
            mEGLContext = EGL14.EGL_NO_CONTEXT;
        }

        mState.set(STATE_RELEASED);
        synchronized (mRenderListenerLock) {
            Iterator<GLRender.GLRenderListener> it = mGLRenderListenerList.iterator();

            while (it.hasNext()) {
                GLRenderListener listener = it.next();
                listener.onReleased();
            }
        }
    }

    private void doDrawFrame() {
        while (true) {
            Runnable runnable;
            synchronized (mDrawLock) {
                if (mGLDrawTaskList.isEmpty()) {
                    return;
                }

                runnable = mGLDrawTaskList.getFirst();
                mGLDrawTaskList.removeFirst();
            }

            runnable.run();
        }
    }

    private void prepareGlSurface(SurfaceTexture st, int width, int height) {
        mEglCore = new EglCore(mEGLContext, 0);

        if (st != null) {
            mWindowSurface = new WindowSurface(mEglCore, st);
        } else {
            mWindowSurface = new WindowSurface(mEglCore, width, height);
        }

        mWindowSurface.makeCurrent();
        GLES20.glViewport(0, 0, mWindowSurface.getWidth(), mWindowSurface.getHeight());
    }

    private void releaseGlSurface(SurfaceTexture st) {
        if (st != null) {
            st.release();
        }

        if (mWindowSurface != null) {
            mWindowSurface.release();
            mWindowSurface = null;
        }

        if (mEglCore != null) {
            mEglCore.release();
            mEglCore = null;
        }
    }

    private void initHandlerThread() {
        if (mGLHandlerThread == null) {
            mGLHandlerThread = new HandlerThread("MyGLThread");
            mGLHandlerThread.start();
            mGLHandler = new Handler(mGLHandlerThread.getLooper(), new Handler.Callback() {
                public boolean handleMessage(Message msg) {
                    switch (msg.what) {
                        case MSG_TYPE_SURFACE_CREATED:
                            prepareGlSurface((SurfaceTexture) msg.obj, msg.arg1, msg.arg2);
                            surfaceCreated(true);
                            break;
                        case MSG_TYPE_SURFACE_CHANGED:
                            surfaceChanged(msg.arg1, msg.arg2);
                            break;
                        case MSG_TYPE_DRAW_FRAME:
                            drawFrame();
                            mWindowSurface.swapBuffers();
                            break;
                        case MSG_TYPE_QUIT:
                            release();
                            releaseGlSurface((SurfaceTexture) msg.obj);
                            mGLHandlerThread.quit();
                    }

                    return true;
                }
            });
        }
    }

    private void quit(SurfaceTexture st) {
        if (mGLHandlerThread != null) {
            mGLHandler.removeCallbacksAndMessages(null);
            Message msg = Message.obtain(mGLHandler, MSG_TYPE_QUIT, st);
            mGLHandler.sendMessage(msg);

            try {
                mGLHandlerThread.join();
            } catch (InterruptedException e) {
                Log.d(TAG, "quit " + Log.getStackTraceString(e));
            } finally {
                mGLHandlerThread = null;
                mGLHandler = null;
            }
        }
    }

    public interface ScreenshotListener {
        void onBitmapAvailable(Bitmap screenshot);
    }

    public interface GLRenderListener {
        void onReady();

        void onSizeChanged(int width, int height);

        void onDrawFrame();

        void onReleased();
    }
}
