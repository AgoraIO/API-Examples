package io.agora.api.example.examples.advanced.customvideo;

import android.app.Activity;
import android.content.Context;
import android.hardware.display.DisplayManager;
import android.hardware.display.DisplayManager.DisplayListener;
import android.os.Build;
import android.view.Display;
import android.view.WindowManager;

import androidx.annotation.RequiresApi;

import com.google.ar.core.Session;

/**
 * Helper to track the display rotations. In particular, the 180 degree rotations are not notified
 * by the onSurfaceChanged() callback, and thus they require listening to the android display
 * events.
 */
public class DisplayRotationHelper implements DisplayListener {
    private boolean mViewportChanged;
    private int mViewportWidth;
    private int mViewportHeight;
    private final Context mContext;
    private final Display mDisplay;

    /**
     * Constructs the DisplayRotationHelper but does not register the listener yet.
     *
     * @param context the Android {@link Context}.
     */
    @RequiresApi(api = Build.VERSION_CODES.M)
    public DisplayRotationHelper(Context context) {
        mContext = context;
        mDisplay = context.getSystemService(WindowManager.class).getDefaultDisplay();
    }

    /** Registers the display listener. Should be called from . */
    @RequiresApi(api = Build.VERSION_CODES.M)
    public void onResume() {
        mContext.getSystemService(DisplayManager.class).registerDisplayListener(this, null);
    }

    /** Unregisters the display listener. Should be called from . */
    @RequiresApi(api = Build.VERSION_CODES.M)
    public void onPause() {
        mContext.getSystemService(DisplayManager.class).unregisterDisplayListener(this);
    }

    /**
     * Records a change in surface dimensions. This will be later used by
     * {@link #updateSessionIfNeeded(Session)}. Should be called from
     * {@link android.opengl.GLSurfaceView.Renderer
     *  #onSurfaceChanged(javax.microedition.khronos.opengles.GL10, int, int)}.
     *
     * @param width the updated width of the surface.
     * @param height the updated height of the surface.
     */
    public void onSurfaceChanged(int width, int height) {
        mViewportWidth = width;
        mViewportHeight = height;
        mViewportChanged = true;
    }

    /**
     * Updates the session display geometry if a change was posted either by
     * {@link #onSurfaceChanged(int, int)} call or by {@link #onDisplayChanged(int)} system
     * callback. This function should be called explicitly before each call to
     * {@link Session#update()}. This function will also clear the 'pending update'
     * (viewportChanged) flag.
     *
     * @param session the {@link Session} object to update if display geometry changed.
     */
    public void updateSessionIfNeeded(Session session) {
        if (mViewportChanged) {
            int displayRotation = mDisplay.getRotation();
            session.setDisplayGeometry(displayRotation, mViewportWidth, mViewportHeight);
            mViewportChanged = false;
        }
    }

    /**
     * Returns the current rotation state of android display.
     * Same as {@link Display#getRotation()}.
     */
    public int getRotation() {
        return mDisplay.getRotation();
    }

    @Override
    public void onDisplayAdded(int displayId) {}

    @Override
    public void onDisplayRemoved(int displayId) {}

    @Override
    public void onDisplayChanged(int displayId) {
        mViewportChanged = true;
    }
}
