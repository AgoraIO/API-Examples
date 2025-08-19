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

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.Matrix;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import java.util.Objects;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLContext;

import io.agora.beautyapi.faceunity.utils.LogUtils;

public class GLUtils {
    private static final String TAG = "GLUtils";
    public static final float[] IDENTITY_MATRIX = new float[16];

    static {
        Matrix.setIdentityM(IDENTITY_MATRIX, 0);
    }

    private GLUtils() {
    }

    public static Bitmap getTexture2DImage(int textureID, int width, int height) {
        try {
            int[] oldFboId = new int[1];
            GLES20.glGetIntegerv(GLES20.GL_FRAMEBUFFER_BINDING, IntBuffer.wrap(oldFboId));

            int[] framebuffers = new int[1];
            GLES20.glGenFramebuffers(1, framebuffers, 0);
            int framebufferId = framebuffers[0];
            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, framebufferId);

            int[] renderbuffers = new int[1];
            GLES20.glGenRenderbuffers(1, renderbuffers, 0);
            int renderId = renderbuffers[0];
            GLES20.glBindRenderbuffer(GLES20.GL_RENDERBUFFER, renderId);
            GLES20.glRenderbufferStorage(GLES20.GL_RENDERBUFFER, GLES20.GL_DEPTH_COMPONENT16, width, height);

            GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, textureID, 0);
            GLES20.glFramebufferRenderbuffer(GLES20.GL_FRAMEBUFFER, GLES20.GL_DEPTH_ATTACHMENT, GLES20.GL_RENDERBUFFER, renderId);
            if (GLES20.glCheckFramebufferStatus(GLES20.GL_FRAMEBUFFER) != GLES20.GL_FRAMEBUFFER_COMPLETE) {
                LogUtils.e(TAG, "Framebuffer error");
            }

            ByteBuffer rgbaBuf = ByteBuffer.allocateDirect(width * height * 4);
            rgbaBuf.position(0);
            GLES20.glReadPixels(0, 0, width, height, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, rgbaBuf);

            Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
            bitmap.copyPixelsFromBuffer(rgbaBuf);

            GLES20.glDeleteRenderbuffers(1, IntBuffer.wrap(framebuffers));
            GLES20.glDeleteFramebuffers(1, IntBuffer.allocate(framebufferId));

            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, oldFboId[0]);

            return bitmap;
        } catch (Exception e) {
            LogUtils.e(TAG, e.toString());
        }
        return null;
    }

    public static Bitmap getTextureOESImage(int textureID, int width, int height) {
        try {
            int[] oldFboId = new int[1];
            GLES20.glGetIntegerv(GLES20.GL_FRAMEBUFFER_BINDING, IntBuffer.wrap(oldFboId));

            int[] framebuffers = new int[1];
            GLES20.glGenFramebuffers(1, framebuffers, 0);
            int framebufferId = framebuffers[0];
            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, framebufferId);

            int[] renderbuffers = new int[1];
            GLES20.glGenRenderbuffers(1, renderbuffers, 0);
            int renderId = renderbuffers[0];
            GLES20.glBindRenderbuffer(GLES20.GL_RENDERBUFFER, renderId);
            GLES20.glRenderbufferStorage(GLES20.GL_RENDERBUFFER, GLES20.GL_DEPTH_COMPONENT16, width, height);

            GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES11Ext.GL_TEXTURE_EXTERNAL_OES, textureID, 0);
            GLES20.glFramebufferRenderbuffer(GLES20.GL_FRAMEBUFFER, GLES20.GL_DEPTH_ATTACHMENT, GLES20.GL_RENDERBUFFER, renderId);
            if (GLES20.glCheckFramebufferStatus(GLES20.GL_FRAMEBUFFER) != GLES20.GL_FRAMEBUFFER_COMPLETE) {
                LogUtils.e(TAG, "Framebuffer error");
            }

            ByteBuffer rgbaBuf = ByteBuffer.allocateDirect(width * height * 4);
            rgbaBuf.position(0);
            GLES20.glReadPixels(0, 0, width, height, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, rgbaBuf);

            Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
            bitmap.copyPixelsFromBuffer(rgbaBuf);

            GLES20.glDeleteRenderbuffers(1, IntBuffer.wrap(framebuffers));
            GLES20.glDeleteFramebuffers(1, IntBuffer.allocate(framebufferId));

            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, oldFboId[0]);

            return bitmap;
        } catch (Exception e) {
            LogUtils.e(TAG, e.toString());
        }
        return null;
    }

    public static Bitmap nv21ToBitmap(byte[] nv21, int width, int height) {
        Bitmap bitmap = null;
        try {
            YuvImage image = new YuvImage(nv21, ImageFormat.NV21, width, height, null);
            ByteArrayOutputStream stream = new ByteArrayOutputStream();
            image.compressToJpeg(new Rect(0, 0, width, height), 80, stream);
            bitmap = BitmapFactory.decodeByteArray(stream.toByteArray(), 0, stream.size());
            stream.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return bitmap;
    }

    private static Bitmap readBitmap(int width, int height) {
        ByteBuffer rgbaBuf = ByteBuffer.allocateDirect(width * height * 4);
        rgbaBuf.position(0);
        GLES20.glReadPixels(0, 0, width, height, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, rgbaBuf);

        Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        bitmap.copyPixelsFromBuffer(rgbaBuf);
        return bitmap;
    }

    public static float[] createTransformMatrix(int rotation, boolean flipH, boolean flipV) {
        float[] renderMVPMatrix = new float[16];
        float[] tmp = new float[16];
        Matrix.setIdentityM(tmp, 0);

        boolean _flipH = flipH;
        boolean _flipV = flipV;
        if (rotation % 180 != 0) {
            _flipH = flipV;
            _flipV = flipH;
        }

        if (_flipH) {
            Matrix.rotateM(tmp, 0, tmp, 0, 180, 0, 1f, 0);
        }
        if (_flipV) {
            Matrix.rotateM(tmp, 0, tmp, 0, 180, 1f, 0f, 0);
        }

        float _rotation = rotation;
        if (_rotation != 0) {
            if (_flipH != _flipV) {
                _rotation *= -1;
            }
            Matrix.rotateM(tmp, 0, tmp, 0, _rotation, 0, 0, 1);
        }

        Matrix.setIdentityM(renderMVPMatrix, 0);
        Matrix.multiplyMM(renderMVPMatrix, 0, tmp, 0, renderMVPMatrix, 0);
        return renderMVPMatrix;
    }

    public static EGLContext getCurrGLContext() {
        EGL10 egl = (EGL10) EGLContext.getEGL();
        if (egl != null && !Objects.equals(egl.eglGetCurrentContext(), EGL10.EGL_NO_CONTEXT)) {
            return egl.eglGetCurrentContext();
        }
        return null;
    }

    public static void checkGlError(String op) {
        int error = GLES20.glGetError();
        if (error != GLES20.GL_NO_ERROR) {
            String msg = op + ": glError 0x" + Integer.toHexString(error);
            LogUtils.e(TAG, msg);
            throw new RuntimeException(msg);
        }
    }

    public static int createProgram(String vertexSource, String fragmentSource) {
        int vertexShader = loadShader(GLES20.GL_VERTEX_SHADER, vertexSource);
        if (vertexShader == 0) {
            return 0;
        }
        int pixelShader = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentSource);
        if (pixelShader == 0) {
            return 0;
        }
        int program = GLES20.glCreateProgram();
        checkGlError("glCreateProgram");
        if (program == 0) {
            LogUtils.e(TAG, "Could not create program");
        }
        GLES20.glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        GLES20.glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        GLES20.glLinkProgram(program);
        int[] linkStatus = new int[1];
        GLES20.glGetProgramiv(program, GLES20.GL_LINK_STATUS, linkStatus, 0);
        if (linkStatus[0] != GLES20.GL_TRUE) {
            LogUtils.e(TAG, "Could not link program: ");
            LogUtils.e(TAG, GLES20.glGetProgramInfoLog(program));
            GLES20.glDeleteProgram(program);
            program = 0;
        }
        return program;
    }

    public static int loadShader(int shaderType, String source) {
        int shader = GLES20.glCreateShader(shaderType);
        checkGlError("glCreateShader type=" + shaderType);
        GLES20.glShaderSource(shader, source);
        GLES20.glCompileShader(shader);
        int[] compiled = new int[1];
        GLES20.glGetShaderiv(shader, GLES20.GL_COMPILE_STATUS, compiled, 0);
        if (compiled[0] == 0) {
            LogUtils.e(TAG, "Could not compile shader " + shaderType + ":");
            LogUtils.e(TAG, " " + GLES20.glGetShaderInfoLog(shader));
            GLES20.glDeleteShader(shader);
            shader = 0;
        }
        return shader;
    }

    public static int createTexture(int textureTarget, Bitmap bitmap, int minFilter,
                                    int magFilter, int wrapS, int wrapT) {
        int[] textureHandle = new int[1];

        GLES20.glGenTextures(1, textureHandle, 0);
        checkGlError("glGenTextures");
        GLES20.glBindTexture(textureTarget, textureHandle[0]);
        checkGlError("glBindTexture " + textureHandle[0]);
        GLES20.glTexParameterf(textureTarget, GLES20.GL_TEXTURE_MIN_FILTER, minFilter);
        GLES20.glTexParameterf(textureTarget, GLES20.GL_TEXTURE_MAG_FILTER, magFilter); // Linear interpolation
        GLES20.glTexParameteri(textureTarget, GLES20.GL_TEXTURE_WRAP_S, wrapS);
        GLES20.glTexParameteri(textureTarget, GLES20.GL_TEXTURE_WRAP_T, wrapT);

        if (bitmap != null) {
            android.opengl.GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);
        }

        checkGlError("glTexParameter");
        return textureHandle[0];
    }
}
