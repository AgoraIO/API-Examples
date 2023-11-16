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

package io.agora.beautyapi.sensetime.utils.egl;

import android.content.Context;
import android.graphics.Bitmap;
import android.opengl.GLES20;
import android.opengl.GLUtils;
import android.opengl.Matrix;

import androidx.annotation.Nullable;
import androidx.annotation.RawRes;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.Objects;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLContext;

import io.agora.beautyapi.sensetime.utils.LogUtils;

/**
 * The type Gl util.
 */
public final class GlUtil {
    private static final String TAG = "GlUtil";

    /**
     * Identity matrix for general use.  Don't modify or life will get weird.
     */
    public static final int NO_TEXTURE = -1;

    private static final int SIZEOF_FLOAT = 4;

    /**
     * The constant IDENTITY_MATRIX.
     */
    public static final float[] IDENTITY_MATRIX = new float[16];

    static {
        Matrix.setIdentityM(IDENTITY_MATRIX, 0);
    }

    private GlUtil() { // do not instantiate
    }

    /**
     * Create program int.
     *
     * @param applicationContext  the application context
     * @param vertexSourceRawId   the vertex source raw id
     * @param fragmentSourceRawId the fragment source raw id
     * @return the int
     */
    public static int createProgram(Context applicationContext, @RawRes int vertexSourceRawId,
                                    @RawRes int fragmentSourceRawId) {

        String vertexSource = readTextFromRawResource(applicationContext, vertexSourceRawId);
        String fragmentSource = readTextFromRawResource(applicationContext, fragmentSourceRawId);

        return createProgram(vertexSource, fragmentSource);
    }

    /**
     * Create program int.
     *
     * @param vertexSource   the vertex source
     * @param fragmentSource the fragment source
     * @return the int
     */
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

    /**
     * Load shader int.
     *
     * @param shaderType the shader type
     * @param source     the source
     * @return the int
     */
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

    /**
     * Create texture int.
     *
     * @param textureTarget the texture target
     * @param bitmap        the bitmap
     * @param minFilter     the min filter
     * @param magFilter     the mag filter
     * @param wrapS         the wrap s
     * @param wrapT         the wrap t
     * @return the int
     */
    public static int createTexture(int textureTarget, @Nullable Bitmap bitmap, int minFilter,
                                    int magFilter, int wrapS, int wrapT) {
        int[] textureHandle = new int[1];

        GLES20.glGenTextures(1, textureHandle, 0);
        GlUtil.checkGlError("glGenTextures");
        GLES20.glBindTexture(textureTarget, textureHandle[0]);
        GlUtil.checkGlError("glBindTexture " + textureHandle[0]);
        GLES20.glTexParameterf(textureTarget, GLES20.GL_TEXTURE_MIN_FILTER, minFilter);
        GLES20.glTexParameterf(textureTarget, GLES20.GL_TEXTURE_MAG_FILTER, magFilter); //线性插值
        GLES20.glTexParameteri(textureTarget, GLES20.GL_TEXTURE_WRAP_S, wrapS);
        GLES20.glTexParameteri(textureTarget, GLES20.GL_TEXTURE_WRAP_T, wrapT);

        if (bitmap != null) {
            GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);
        }

        GlUtil.checkGlError("glTexParameter");
        return textureHandle[0];
    }

    /**
     * Create texture int.
     *
     * @param textureTarget the texture target
     * @return the int
     */
    public static int createTexture(int textureTarget) {
        return createTexture(textureTarget, null, GLES20.GL_LINEAR, GLES20.GL_LINEAR,
                GLES20.GL_CLAMP_TO_EDGE, GLES20.GL_CLAMP_TO_EDGE);
    }

    /**
     * Create texture int.
     *
     * @param textureTarget the texture target
     * @param bitmap        the bitmap
     * @return the int
     */
    public static int createTexture(int textureTarget, Bitmap bitmap) {
        return createTexture(textureTarget, bitmap, GLES20.GL_LINEAR, GLES20.GL_LINEAR,
                GLES20.GL_CLAMP_TO_EDGE, GLES20.GL_CLAMP_TO_EDGE);
    }

    /**
     * Init effect texture.
     *
     * @param width     the width
     * @param height    the height
     * @param textureId the texture id
     * @param type      the type
     */
    public static void initEffectTexture(int width, int height, int[] textureId, int type) {
        int len = textureId.length;
        if (len > 0) {
            GLES20.glGenTextures(len, textureId, 0);
        }
        for (int i = 0; i < len; i++) {
            GLES20.glBindTexture(type, textureId[i]);
            GLES20.glTexParameterf(type,
                    GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameterf(type,
                    GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameterf(type,
                    GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameterf(type,
                    GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexImage2D(type, 0, GLES20.GL_RGBA, width, height, 0,
                    GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);
        }
    }

    /**
     * Checks to see if a GLES error has been raised.
     *
     * @param op the op
     */
    public static void checkGlError(String op) {
        int error = GLES20.glGetError();
        if (error != GLES20.GL_NO_ERROR) {
            String msg = op + ": glError 0x" + Integer.toHexString(error);
            LogUtils.e(TAG, msg);
            throw new RuntimeException(msg);
        }
    }

    /**
     * Allocates a direct float buffer, and populates it with the float array data.
     *
     * @param coords the coords
     * @return the float buffer
     */
    public static FloatBuffer createFloatBuffer(float[] coords) {
        // Allocate a direct ByteBuffer, using 4 bytes per float, and copy coords into it.
        ByteBuffer bb = ByteBuffer.allocateDirect(coords.length * SIZEOF_FLOAT);
        bb.order(ByteOrder.nativeOrder());
        FloatBuffer fb = bb.asFloatBuffer();
        fb.put(coords);
        fb.position(0);
        return fb;
    }

    /**
     * Read text from raw resource string.
     *
     * @param applicationContext the application context
     * @param resourceId         the resource id
     * @return the string
     */
    public static String readTextFromRawResource(final Context applicationContext,
                                                 @RawRes final int resourceId) {
        final InputStream inputStream =
                applicationContext.getResources().openRawResource(resourceId);
        final InputStreamReader inputStreamReader = new InputStreamReader(inputStream);
        final BufferedReader bufferedReader = new BufferedReader(inputStreamReader);
        String nextLine;
        final StringBuilder body = new StringBuilder();
        try {
            while ((nextLine = bufferedReader.readLine()) != null) {
                body.append(nextLine);
                body.append('\n');
            }
        } catch (IOException e) {
            return null;
        }

        return body.toString();
    }

    /**
     * Create transform matrix float [ ].
     *
     * @param rotation the rotation
     * @param flipH    the flip h
     * @param flipV    the flip v
     * @return the float [ ]
     */
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

    /**
     * Gets curr gl context.
     *
     * @return the curr gl context
     */
    public static EGLContext getCurrGLContext() {
        EGL10 egl = (EGL10) EGLContext.getEGL();
        if (egl != null && !Objects.equals(egl.eglGetCurrentContext(), EGL10.EGL_NO_CONTEXT)) {
            return egl.eglGetCurrentContext();
        }
        return null;
    }
}
