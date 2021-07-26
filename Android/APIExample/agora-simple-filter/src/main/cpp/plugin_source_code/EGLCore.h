//
// Created by 张涛 on 2020/4/27.
//

#ifndef AGORAWITHBYTEDANCE_EGLCORE_H
#define AGORAWITHBYTEDANCE_EGLCORE_H

#if defined(__ANDROID__) || defined(TARGET_OS_ANDROID)
#include <android/native_window.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>
#include "../logutils.h"

/**
 * Constructor flag: surface must be recordable.  This discourages EGL from using a
 * pixel format that cannot be converted efficiently to something usable by the video
 * encoder.
 */
#define FLAG_RECORDABLE 0x01

/**
 * Constructor flag: ask for GLES3, fall back to GLES2 if not available.  Without this
 * flag, GLES2 is used.
 */
#define FLAG_TRY_GLES3 002

// Android-specific extension
#define EGL_RECORDABLE_ANDROID 0x3142

namespace agora {
    namespace extension {
        class EglCore {

        private:
            EGLDisplay mEGLDisplay = EGL_NO_DISPLAY;
            EGLConfig mEGLConfig = NULL;
            EGLContext mEGLContext = EGL_NO_CONTEXT;
            int mGlVersion = -1;

            EGLConfig getConfig(int flags, int version);
            bool init(EGLContext sharedContext, int flags);
            void release();

        public:
            EglCore();

            ~EglCore();

            EglCore(EGLContext sharedContext, int flags);

            EGLContext getEGLContext();

            void releaseSurface(EGLSurface eglSurface);

            EGLSurface createWindowSurface(ANativeWindow *surface);

            EGLSurface createOffscreenSurface(int width, int height);

            void makeCurrent(EGLSurface eglSurface);

            void makeCurrent(EGLSurface drawSurface, EGLSurface readSurface);

            void makeNothingCurrent();

            bool swapBuffers(EGLSurface eglSurface);

            void setPresentationTime(EGLSurface eglSurface, long nsecs);

            bool isCurrent(EGLSurface eglSurface);

            int querySurface(EGLSurface eglSurface, int what);

            int getGlVersion();

            void checkEglError(const char *msg);
        };
    }
}
#endif //defined(__ANDROID__) || defined(TARGET_OS_ANDROID)

#endif //AGORAWITHBYTEDANCE_EGLCORE_H
