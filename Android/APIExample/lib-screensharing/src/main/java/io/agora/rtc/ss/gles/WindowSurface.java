package io.agora.rtc.ss.gles;

import android.annotation.TargetApi;
import android.graphics.SurfaceTexture;
import android.os.Build;
import android.view.Surface;

/**
 * Recordable EGL window surface.
 * <p>
 * It's good practice to explicitly quit() the surface, preferably from a "finally" block.
 */
@TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
public class WindowSurface extends EglSurfaceBase {
    private Surface mSurface;

    public WindowSurface(EglCore eglCore, int width, int height) {
        super(eglCore);
        this.createOffscreenSurface(width, height);
    }

    public WindowSurface(EglCore eglCore, Surface surface) {
        super(eglCore);
        this.createWindowSurface(surface);
        this.mSurface = surface;
    }

    public WindowSurface(EglCore eglCore, SurfaceTexture texture) {
        super(eglCore);
        this.createWindowSurface(texture);
    }

    public void release() {
        this.releaseEglSurface();

        if (this.mSurface != null) {
            this.mSurface.release();
            this.mSurface = null;
        }
    }

    /**
     * Recreate the EGLSurface, using the new EglBase.  The caller should have already
     * freed the old EGLSurface with releaseEglSurface().
     * <p>
     * This is useful when we want to update the EGLSurface associated with a Surface.
     * For example, if we want to share with a different EGLContext, which can only
     * be done by tearing down and recreating the context.  (That's handled by the caller;
     * this just creates a new EGLSurface for the Surface we were handed earlier.)
     * <p>
     * If the previous EGLSurface isn't fully destroyed, e.g. it's still current on a
     * context somewhere, the create call will fail with complaints from the Surface
     * about already being connected.
     */
    public void recreate(EglCore newEglCore) {
        if (this.mSurface == null) {
            throw new RuntimeException("not yet implemented for SurfaceTexture");
        } else {
            this.mEglCore = newEglCore; // switch to new context
            this.createWindowSurface(this.mSurface); // create new surface
        }
    }
}
