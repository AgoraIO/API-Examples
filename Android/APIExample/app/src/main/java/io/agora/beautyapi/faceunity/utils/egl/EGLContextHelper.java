/*
 * MIT License
 *
 * Copyright (c) 2023 Agora Community
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

package io.agora.beautyapi.faceunity.utils.egl;

import static android.opengl.EGL14.EGL_CONTEXT_CLIENT_VERSION;

import android.opengl.GLDebugHelper;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;

import io.agora.beautyapi.faceunity.utils.LogUtils;

public class EGLContextHelper {
    private static final String DEBUG_TAG = "EGLContextManager";
    private final int mRedSize = 8;
    private final int mGreenSize = 8;
    private final int mBlueSize = 8;
    private final int mAlphaSize = 0;
    private final int mDepthSize = 16;
    private final int mStencilSize = 0;
    private final int mRenderType = 4;
    public EGLContextHelper(){}

    public void initEGL(EGLContext shareContext) throws Exception {
        mEGL = (EGL10) GLDebugHelper.wrap(EGLContext.getEGL(),
                GLDebugHelper.CONFIG_CHECK_GL_ERROR
                        | GLDebugHelper.CONFIG_CHECK_THREAD,  null);

        if (mEGL == null) {
            throw new Exception("Couldn't get EGL");
        }

        mGLDisplay = mEGL.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);

        if (mGLDisplay == null) {
            throw new Exception("Couldn't get display for GL");
        }

        int[] curGLVersion = new int[2];
        mEGL.eglInitialize(mGLDisplay, curGLVersion);

        LogUtils.i(DEBUG_TAG, "GL version = " + curGLVersion[0] + "."
                + curGLVersion[1]);

        int[] num_config = new int[1];
        if(!mEGL.eglChooseConfig(mGLDisplay, mConfigSpec, null, 1,
                num_config)){
            throw new IllegalArgumentException("eglChooseConfig failed");
        }
        int numConfigs = num_config[0];
        if (numConfigs <= 0) {
            throw new IllegalArgumentException(
                    "No configs match configSpec");
        }

        EGLConfig[] configs = new EGLConfig[numConfigs];
        if (!mEGL.eglChooseConfig(mGLDisplay, mConfigSpec, configs, numConfigs,
                num_config)) {
            throw new IllegalArgumentException("eglChooseConfig#2 failed");
        }
        mGLConfig = chooseConfig(mEGL, mGLDisplay, configs);
        if (mGLConfig == null) {
            mGLConfig = configs[0];
        }

        int[] surfaceAttribs = {
                EGL10.EGL_WIDTH, 1,
                EGL10.EGL_HEIGHT, 1,
                EGL10.EGL_NONE
        };
        mGLSurface = mEGL.eglCreatePbufferSurface(mGLDisplay, mGLConfig, surfaceAttribs);

        if (mGLSurface == null) {
            throw new Exception("Couldn't create new surface");
        }

        int[] attrib_list = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE};
        mGLContext = mEGL.eglCreateContext(mGLDisplay, mGLConfig,
                shareContext, attrib_list);

        if (mGLContext == null) {
            throw new Exception("Couldn't create new context");
        }


//        if (!mEGL.eglMakeCurrent(mGLDisplay, mGLSurface, mGLSurface, mGLContext)) {
//            throw new Exception("Failed to eglMakeCurrent");
//        }

    }

    public EGLContext getEGLContext() {
        return mGLContext;
    }

    public EGLDisplay getGLDisplay() {
        return mGLDisplay;
    }

    public EGLConfig getGLConfig() {
        return mGLConfig;
    }

    public EGLSurface getGLSurface() {
        return mGLSurface;
    }

    public EGL10 getEGL() {
        return mEGL;
    }

    EGL10 mEGL;
    EGLDisplay mGLDisplay;
    EGLConfig mGLConfig;
    EGLSurface mGLSurface;
    EGLContext mGLContext;

    int[] mConfigSpec = new int[]{
            EGL10.EGL_RED_SIZE, mRedSize,
            EGL10.EGL_GREEN_SIZE, mGreenSize,
            EGL10.EGL_BLUE_SIZE, mBlueSize,
            EGL10.EGL_ALPHA_SIZE, mAlphaSize,
            EGL10.EGL_DEPTH_SIZE, mDepthSize,
            EGL10.EGL_STENCIL_SIZE, mStencilSize,
            EGL10.EGL_RENDERABLE_TYPE, mRenderType,// EGL version 2.0
            EGL10.EGL_NONE};

    public void release() {
        mEGL.eglMakeCurrent(mGLDisplay, EGL10.EGL_NO_SURFACE,
                EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT);
        mEGL.eglDestroySurface(mGLDisplay, mGLSurface);
        mEGL.eglDestroyContext(mGLDisplay, mGLContext);
        mEGL.eglTerminate(mGLDisplay);

        LogUtils.i(DEBUG_TAG, "GL Cleaned up");
    }

    public boolean eglMakeCurrent(){
        if(mGLContext == EGL10.EGL_NO_CONTEXT){
            return false;
        }else{
            return mEGL.eglMakeCurrent(mGLDisplay, mGLSurface, mGLSurface, mGLContext);
        }
    }

    public boolean eglMakeNoCurrent(){
        return mEGL.eglMakeCurrent(mGLDisplay, EGL10.EGL_NO_SURFACE,
                EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT);
    }

    private EGLConfig chooseConfig(EGL10 egl, EGLDisplay display,
                                   EGLConfig[] configs) {
        for (EGLConfig config : configs) {
            int d = findConfigAttrib(egl, display, config,
                    EGL10.EGL_DEPTH_SIZE, 0);
            int s = findConfigAttrib(egl, display, config,
                    EGL10.EGL_STENCIL_SIZE, 0);
            if ((d >= mDepthSize) && (s >= mStencilSize)) {
                int r = findConfigAttrib(egl, display, config,
                        EGL10.EGL_RED_SIZE, 0);
                int g = findConfigAttrib(egl, display, config,
                        EGL10.EGL_GREEN_SIZE, 0);
                int b = findConfigAttrib(egl, display, config,
                        EGL10.EGL_BLUE_SIZE, 0);
                int a = findConfigAttrib(egl, display, config,
                        EGL10.EGL_ALPHA_SIZE, 0);
                if ((r == mRedSize) && (g == mGreenSize)
                        && (b == mBlueSize) && (a == mAlphaSize)) {
                    return config;
                }
            }
        }
        return null;
    }

    private int findConfigAttrib(EGL10 egl, EGLDisplay display,
                                 EGLConfig config, int attribute, int defaultValue) {
        int[] value = new int[1];
        if (egl.eglGetConfigAttrib(display, config, attribute, value)) {
            return value[0];
        }
        return defaultValue;
    }
}
