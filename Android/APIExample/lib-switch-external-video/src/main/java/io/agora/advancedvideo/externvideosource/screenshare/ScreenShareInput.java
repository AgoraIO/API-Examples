package io.agora.advancedvideo.externvideosource.screenshare;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.media.projection.MediaProjection;
import android.media.projection.MediaProjectionManager;
import android.util.Log;
import android.util.Size;
import android.view.Surface;

import io.agora.advancedvideo.externvideosource.GLThreadContext;
import io.agora.advancedvideo.externvideosource.IExternalVideoInput;

public class ScreenShareInput implements IExternalVideoInput {
    private static final String TAG = ScreenShareInput.class.getSimpleName();
    private static final String VIRTUAL_DISPLAY_NAME = "screen-share-display";

    private Context mContext;
    private int mSurfaceWidth;
    private int mSurfaceHeight;
    private int mScreenDpi;
    private int mFrameInterval;
    private Intent mIntent;
    private MediaProjection mMediaProjection;
    private VirtualDisplay mVirtualDisplay;
    private volatile boolean mStopped;

    public ScreenShareInput(Context context, int width, int height, int dpi, int framerate, Intent data) {
        mContext = context;
        mSurfaceWidth = width;
        mSurfaceHeight = height;
        mScreenDpi = dpi;
        mFrameInterval = 1000 / framerate;
        mIntent = data;
    }

    @Override
    public void onVideoInitialized(Surface target) {
        MediaProjectionManager pm = (MediaProjectionManager)
                mContext.getSystemService(Context.MEDIA_PROJECTION_SERVICE);
        mMediaProjection = pm.getMediaProjection(Activity.RESULT_OK, mIntent);

        if (mMediaProjection == null) {
            Log.e(TAG, "media projection start failed");
            return;
        }

        mVirtualDisplay = mMediaProjection.createVirtualDisplay(
                VIRTUAL_DISPLAY_NAME, mSurfaceWidth, mSurfaceHeight, mScreenDpi,
                DisplayManager.VIRTUAL_DISPLAY_FLAG_PUBLIC, target,
                null, null);
    }

    @Override
    public void onVideoStopped(GLThreadContext context) {
        mStopped = true;

        if (mVirtualDisplay != null) {
            mVirtualDisplay.release();
        }

        if (mMediaProjection != null) {
            mMediaProjection.stop();
        }
    }

    @Override
    public boolean isRunning() {
        return !mStopped;
    }

    @Override
    public void onFrameAvailable(GLThreadContext context, int textureId, float[] transform) {
        // Screen sharing do not process or show local preview
    }

    @Override
    public Size onGetFrameSize() {
        return new Size(mSurfaceWidth, mSurfaceHeight);
    }

    @Override
    public int timeToWait() {
        return mFrameInterval;
    }
}
