//
// Created by 张涛 on 2020/4/27.
//

#if defined(__ANDROID__) || defined(TARGET_OS_ANDROID)

#include "EGLCore.h"
#include <assert.h>

namespace agora {
    namespace extension {
        EglCore::EglCore() {
            init(NULL, 0);
        }


        EglCore::~EglCore() {
            release();
        }

        EglCore::EglCore(EGLContext sharedContext, int flags) {
            init(sharedContext, flags);
        }

        bool EglCore::init(EGLContext sharedContext, int flags) {
            PRINTF_INFO("init EglCore");
            assert(mEGLDisplay == EGL_NO_DISPLAY);
            if (mEGLDisplay != EGL_NO_DISPLAY) {
                PRINTF_ERROR("EGL already set up");
                return false;
            }
            if (sharedContext == NULL) {
                sharedContext = EGL_NO_CONTEXT;
            }

            mEGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            assert(mEGLDisplay != EGL_NO_DISPLAY);
            if (mEGLDisplay == EGL_NO_DISPLAY) {
                PRINTF_ERROR("unable to get EGL14 display.\n");
                return false;
            }

            if (!eglInitialize(mEGLDisplay, 0, 0)) {
                mEGLDisplay = EGL_NO_DISPLAY;
                PRINTF_ERROR("unable to initialize EGL14");
                return false;
            }

            if ((flags & FLAG_TRY_GLES3) != 0) {
                EGLConfig config = getConfig(flags, 3);
                if (config != NULL) {
                    int attrib3_list[] = {
                            EGL_CONTEXT_CLIENT_VERSION, 3,
                            EGL_NONE
                    };
                    EGLContext context = eglCreateContext(mEGLDisplay, config,
                                                          sharedContext, attrib3_list);

                    if (eglGetError() == EGL_SUCCESS) {
                        mEGLConfig = config;
                        mEGLContext = context;
                        mGlVersion = 3;
                    }
                }
            }

            if (mEGLContext == EGL_NO_CONTEXT) {
                EGLConfig config = getConfig(flags, 2);
                assert(config != NULL);
                int attrib2_list[] = {
                        EGL_CONTEXT_CLIENT_VERSION, 2,
                        EGL_NONE
                };
                EGLContext context = eglCreateContext(mEGLDisplay, config,
                                                      sharedContext, attrib2_list);
                if (eglGetError() == EGL_SUCCESS) {
                    mEGLConfig = config;
                    mEGLContext = context;
                    mGlVersion = 2;
                    PRINTF_INFO("EGLCore EGL_SUCCESS");
                }
            }

            int values[1] = {0};
            eglQueryContext(mEGLDisplay, mEGLContext, EGL_CONTEXT_CLIENT_VERSION, values);
            return true;
        }

        //
        EGLConfig EglCore::getConfig(int flags, int version) {
            int renderableType = EGL_OPENGL_ES2_BIT;
            if (version >= 3) {
                renderableType |= EGL_OPENGL_ES3_BIT_KHR;
            }
            int attribList[] = {
                    EGL_RED_SIZE, 8,
                    EGL_GREEN_SIZE, 8,
                    EGL_BLUE_SIZE, 8,
                    EGL_ALPHA_SIZE, 8,
                    //EGL_DEPTH_SIZE, 16,
                    //EGL_STENCIL_SIZE, 8,
                    EGL_RENDERABLE_TYPE, renderableType,
                    EGL_NONE, 0,      // placeholder for recordable [@-3]
                    EGL_NONE
            };
            int length = sizeof(attribList) / sizeof(attribList[0]);
            if ((flags & FLAG_RECORDABLE) != 0) {
                attribList[length - 3] = EGL_RECORDABLE_ANDROID;
                attribList[length - 2] = 1;
            }
            EGLConfig configs = NULL;
            int numConfigs;
            if (!eglChooseConfig(mEGLDisplay, attribList, &configs, 1, &numConfigs)) {
                PRINTF_ERROR("unable to find RGB8888 / %d  EGLConfig", version);
                return NULL;
            }
            return configs;
        }

        void EglCore::release() {
            PRINTF_INFO("release EglCore");
            if (mEGLDisplay != EGL_NO_DISPLAY) {
                eglMakeCurrent(mEGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
                eglDestroyContext(mEGLDisplay, mEGLContext);
                eglReleaseThread();
                eglTerminate(mEGLDisplay);
            }

            mEGLDisplay = EGL_NO_DISPLAY;
            mEGLContext = EGL_NO_CONTEXT;
            mEGLConfig = NULL;
        }

        EGLContext EglCore::getEGLContext() {
            return mEGLContext;
        }

        void EglCore::releaseSurface(EGLSurface eglSurface) {
            eglDestroySurface(mEGLDisplay, eglSurface);
        }

        EGLSurface EglCore::createWindowSurface(ANativeWindow *surface) {
            assert(surface != NULL);
            if (surface == NULL) {
                PRINTF_ERROR("ANativeWindow is NULL!");
                return NULL;
            }
            int surfaceAttribs[] = {
                    EGL_NONE
            };
            PRINTF_ERROR("eglCreateWindowSurface start");
            EGLSurface eglSurface = eglCreateWindowSurface(mEGLDisplay, mEGLConfig, surface,
                                                           surfaceAttribs);
            checkEglError("eglCreateWindowSurface");
            assert(eglSurface != NULL);
            if (eglSurface == NULL) {
                PRINTF_ERROR("EGLSurface is NULL!");
                return NULL;
            }
            return eglSurface;
        }

        EGLSurface EglCore::createOffscreenSurface(int width, int height) {
            int surfaceAttribs[] = {
                    EGL_WIDTH, width,
                    EGL_HEIGHT, height,
                    EGL_NONE
            };
            EGLSurface eglSurface = eglCreatePbufferSurface(mEGLDisplay, mEGLConfig,
                                                            surfaceAttribs);
            assert(eglSurface != NULL);
            if (eglSurface == NULL) {
                PRINTF_ERROR("Surface was null");
                return NULL;
            }
            return eglSurface;
        }

        void EglCore::makeCurrent(EGLSurface eglSurface) {
            if (mEGLDisplay == EGL_NO_DISPLAY) {
                PRINTF_ERROR("Note: makeCurrent w/o display.\n");
            }
            if (!eglMakeCurrent(mEGLDisplay, eglSurface, eglSurface, mEGLContext)) {

            }
        }

        void EglCore::makeCurrent(EGLSurface drawSurface, EGLSurface readSurface) {
            if (mEGLDisplay == EGL_NO_DISPLAY) {
                PRINTF_ERROR("Note: makeCurrent w/o display.\n");
            }
            if (!eglMakeCurrent(mEGLDisplay, drawSurface, readSurface, mEGLContext)) {

            }
        }

        void EglCore::makeNothingCurrent() {
            if (!eglMakeCurrent(mEGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
                // TODO 抛出异常
            }
        }

        bool EglCore::swapBuffers(EGLSurface eglSurface) {
            return eglSwapBuffers(mEGLDisplay, eglSurface);
        }

        void EglCore::setPresentationTime(EGLSurface eglSurface, long nsecs) {

        }

        bool EglCore::isCurrent(EGLSurface eglSurface) {
            return mEGLContext == eglGetCurrentContext() &&
                   eglSurface == eglGetCurrentSurface(EGL_DRAW);
        }

        int EglCore::querySurface(EGLSurface eglSurface, int what) {
            int value;
            eglQuerySurface(mEGLContext, eglSurface, what, &value);
            return value;
        }

        int EglCore::getGlVersion() {
            return mGlVersion;
        }

        void EglCore::checkEglError(const char *msg) {
            int error;
            if ((error = eglGetError()) != EGL_SUCCESS) {
                PRINTF_ERROR("%s: EGL error: %x", msg, error);
            }
        }
    }
}

#endif //defined(__ANDROID__) || defined(TARGET_OS_ANDROID)
