package io.agora.advancedvideo.externvideosource.localvideo;

import android.graphics.SurfaceTexture;
import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.opengl.EGL14;
import android.opengl.EGLSurface;
import android.opengl.GLES20;
import android.util.Log;
import android.util.Size;
import android.view.Surface;
import android.view.TextureView;

import java.io.IOException;
import java.nio.ByteBuffer;

import io.agora.advancedvideo.externvideosource.GLThreadContext;
import io.agora.advancedvideo.externvideosource.IExternalVideoInput;

public class LocalVideoInput implements IExternalVideoInput, TextureView.SurfaceTextureListener {
    private static final String TAG = LocalVideoInput.class.getSimpleName();

    private LocalVideoThread mVideoThread;
    private volatile boolean mStopped;

    // The life cycle of SurfaceTexture only affects the
    // rendering of the local preview
    private volatile SurfaceTexture mLocalSurfaceTexture;

    private String mFilePath;
    private int mVideoWidth;
    private int mVideoHeight;
    private int mSurfaceWidth;
    private int mSurfaceHeight;
    private volatile long mFrameInterval;

    private EGLSurface mPreviewSurface = EGL14.EGL_NO_SURFACE;

    public LocalVideoInput(String filePath) {
        mFilePath = filePath;
    }

    @Override
    public void onVideoInitialized(Surface target) {
        mVideoThread = new LocalVideoThread(mFilePath, target);
        mVideoThread.start();
        mStopped = false;
    }

    @Override
    public void onVideoStopped(GLThreadContext context) {
        if (mPreviewSurface != EGL14.EGL_NO_SURFACE &&
                context.eglCore.isCurrent(mPreviewSurface)) {
            context.eglCore.makeNothingCurrent();
            context.eglCore.releaseSurface(mPreviewSurface);
            mPreviewSurface = EGL14.EGL_NO_SURFACE;
        }

        mVideoThread.setStopped();
    }

    @Override
    public boolean isRunning() {
        return !mStopped;
    }

    @Override
    public void onFrameAvailable(GLThreadContext context, int textureId, float[] transform) {
        // This method is called in an OpenGLES thread. Usually this
        // method will give the frame information to draw local preview
        if (mLocalSurfaceTexture == null || mStopped) {
            return;
        }

        if (mPreviewSurface == EGL14.EGL_NO_SURFACE) {
            try {
                mPreviewSurface = context.eglCore.createWindowSurface(mLocalSurfaceTexture);
            } catch (Exception e) {
                return;
            }
        }

        if (!context.eglCore.isCurrent(mPreviewSurface)) {
            context.eglCore.makeCurrent(mPreviewSurface);
            setViewPort(mVideoWidth, mVideoHeight, mSurfaceWidth, mSurfaceHeight);
        }

        context.program.drawFrame(textureId, transform);
        context.eglCore.swapBuffers(mPreviewSurface);
    }

    private void setViewPort(int videoW, int videoH, int surfaceW, int surfaceH) {
        float videoRatio = videoW / (float) videoH;
        float surfaceRatio = surfaceW / (float) surfaceH;
        if (videoRatio == surfaceRatio) {
            GLES20.glViewport(0, 0, videoW, videoH);
            return;
        }

        int startX;
        int startY;
        int viewPortW;
        int viewPortH;
        if (videoRatio > surfaceRatio) {
            // the video is wider than the surface
            viewPortW = surfaceW;
            viewPortH = (int) (surfaceW / videoRatio);
            startX = 0;
            startY = (surfaceH - viewPortH) / 2;
        } else {
            // surface is wider than the video
            viewPortH = surfaceH;
            viewPortW = (int) (viewPortH * videoRatio);
            startX = (surfaceW - viewPortW) / 2;
            startY = 0;
        }

        GLES20.glViewport(startX, startY, viewPortW, viewPortH);
    }

    @Override
    public Size onGetFrameSize() {
        return new Size(mVideoWidth, mVideoHeight);
    }

    @Override
    public int timeToWait() {
        return (int) mFrameInterval;
    }

    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int width, int height) {
        mLocalSurfaceTexture = surfaceTexture;
        mSurfaceWidth = width;
        mSurfaceHeight = height;
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surfaceTexture, int width, int height) {
        mLocalSurfaceTexture = surfaceTexture;
        mSurfaceWidth = width;
        mSurfaceHeight = height;
    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surfaceTexture) {
        mLocalSurfaceTexture = null;
        if (mVideoThread != null && mVideoThread.isAlive()) mVideoThread.setStopped();
        return true;
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surfaceTexture) {

    }

    private class LocalVideoThread extends Thread {
        private static final int INDEX_NO_BUFFER = -1;

        private int mVideoTrackIdx = -1;
        private String mMineType;

        private MediaExtractor mExtractor;
        private MediaFormat mVideoFormat;
        private MediaCodec mDecoder;
        private MediaCodec.BufferInfo mCodecBufferInfo;

        private Surface mSurface;
        private VideoSync mVideoSync;

        LocalVideoThread(String filePath, Surface surface) {
            initMedia(filePath);
            mSurface = surface;
        }

        private void initMedia(String filePath) {
            mExtractor = new MediaExtractor();
            try {
                mExtractor.setDataSource(filePath);
            } catch (IOException e) {
                Log.e(TAG, "Wrong video file");
            }

            for (int i = 0; i < mExtractor.getTrackCount(); i++) {
                MediaFormat format = mExtractor.getTrackFormat(i);
                String mime = format.getString(MediaFormat.KEY_MIME);
                if (mime != null && mime.startsWith("video/")) {
                    mVideoFormat = format;
                    mMineType = mime;
                    mVideoTrackIdx = i;
                    break;
                }
            }

            if (mVideoTrackIdx == -1) {
                Log.e(TAG, "Cannot find a video track");
                return;
            }

            mExtractor.selectTrack(mVideoTrackIdx);
            mVideoWidth = mVideoFormat.getInteger(MediaFormat.KEY_WIDTH);
            mVideoHeight = mVideoFormat.getInteger(MediaFormat.KEY_HEIGHT);

            try {
                mDecoder = MediaCodec.createDecoderByType(mMineType);
                mCodecBufferInfo = new MediaCodec.BufferInfo();
            } catch (IOException e) {
                Log.e(TAG, "Failed to create decoder of mime type " + mMineType);
            }
        }

        private void startDecodeWithSurface() {
            mDecoder.configure(mVideoFormat, mSurface, null, 0);
            mDecoder.start();
        }

        private void initSync() {
            mVideoSync = new VideoSync();
        }

        @Override
        public void run() {
            startDecodeWithSurface();
            initSync();

            while (!mStopped && !isInterrupted()) {
                int inputBufferIndex = mDecoder.dequeueInputBuffer(0);

                long presentTS = 0;
                if (inputBufferIndex != INDEX_NO_BUFFER) {
                    presentTS = mExtractor.getSampleTime();
                    ByteBuffer[] inputBuffers = mDecoder.getInputBuffers();
                    int size = mExtractor.readSampleData(inputBuffers[inputBufferIndex], 0);

                    boolean isEndOfStream = false;
                    if (size == -1) {
                        Log.i(TAG, "Video has reached the end of stream");
                        isEndOfStream = true;
                        size = 0;
                    }

                    mDecoder.queueInputBuffer(inputBufferIndex, 0, size, presentTS,
                            isEndOfStream ? MediaCodec.BUFFER_FLAG_END_OF_STREAM : 0);
                    mExtractor.advance();
                }

                if (mStopped) break;

                int outputBufferIndex = mDecoder.dequeueOutputBuffer(mCodecBufferInfo, 0);
                if (isValidOutputBuffer(outputBufferIndex)) {
                    try {
                        mDecoder.releaseOutputBuffer(outputBufferIndex, mCodecBufferInfo.size > 0);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }

                if (mStopped ||  (mCodecBufferInfo.flags &
                        MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                    break;
                }

                try {
                    long timeToWait = mVideoSync.timeToWait(presentTS);
                    mFrameInterval = timeToWait;
                    Thread.sleep(timeToWait);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            Log.i("External", "local video input has been stopped.");
            setStopped();
            releaseDecoder();
        }

        private boolean isValidOutputBuffer(int index) {
            return index != MediaCodec.INFO_TRY_AGAIN_LATER &&
                    index != MediaCodec.INFO_OUTPUT_FORMAT_CHANGED &&
                    index != MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED;

        }

        void setStopped() {
            mStopped = true;
        }

        private void releaseDecoder() {
            if (mDecoder != null) {
                mDecoder.flush();
                mDecoder.stop();
            }

            if (mExtractor != null) {
                mExtractor.release();
            }
        }
    }

    /**
     * Synchronizes the pacing of sending video frames
     * to decoder according to their presentation timestamps
     */
    private class VideoSync {
        private long mLastPresent;
        private long mLastWait;

        long timeToWait(long presentTS) {
            long diff = (presentTS - mLastPresent) / 1000;
            mLastPresent = presentTS;
            mLastWait = diff > 0 ? diff : mLastWait;
            return mLastWait;
        }
    }
}
