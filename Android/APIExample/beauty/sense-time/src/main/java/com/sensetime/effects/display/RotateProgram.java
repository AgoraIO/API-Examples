package com.sensetime.effects.display;

import android.opengl.GLES20;

import com.sensetime.effects.glutils.GlUtil;
import com.sensetime.effects.glutils.OpenGLUtils;
import com.sensetime.effects.glutils.TextureRotationUtil;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class RotateProgram {
    // Simple vertex shader, used for all programs.
    private static final String CAMERA_INPUT_VERTEX_SHADER =
        "uniform mat4 uTexMatrix;\n" +
        "attribute vec4 position;\n" +
        "attribute vec4 inputTextureCoordinate;\n" +
        "varying vec2 textureCoordinate;\n" +
        "void main() {\n" +
        "    gl_Position = position;\n" +
        "    textureCoordinate = (uTexMatrix * inputTextureCoordinate).xy;\n" +
        "}\n";

    // Simple fragment shader for use with "normal" 2D textures.
    private static final String FRAGMENT_SHADER_2D =
        "precision mediump float;\n" +
        "varying vec2 textureCoordinate;\n" +
        "uniform sampler2D inputImageTexture;\n" +
        "void main() {\n" +
        "    gl_FragColor = vec4(texture2D(inputImageTexture, textureCoordinate).rgb, 1.0);\n" +
        "}\n";

    // Four vertices for the four corners of the triangle
    private final static String POSITION_COORDINATE = "position";

    // Corresponding four texture coordinates for the four vertices
    private final static String TEXTURE_COORDINATE = "inputTextureCoordinate";

    private final static String TEXTURE_UNIFORM = "inputImageTexture";

    private final static String TEXUTRE_MATRIX = "uTexMatrix";

    // locations in the shader mProgram
    private int mVertexCoordLocation;
    private int mTexCoordLocation;
    private int mTexSampleLocation;
    private int mTexMatrixLocation;

    private int mProgram;

    private FloatBuffer mVertexBuffer;
    private FloatBuffer mTextureBuffer;

    private int[] mFramebuffer = new int[1];
    private int[] mTargetTexture = new int[1];

    private int mWidth;
    private int mHeight;

    RotateProgram() {
        mProgram = OpenGLUtils.loadProgram(
                CAMERA_INPUT_VERTEX_SHADER,
                FRAGMENT_SHADER_2D);

        mVertexCoordLocation = GLES20.glGetAttribLocation(mProgram, POSITION_COORDINATE);
        mTexCoordLocation = GLES20.glGetAttribLocation(mProgram, TEXTURE_COORDINATE);
        mTexSampleLocation = GLES20.glGetUniformLocation(mProgram, TEXTURE_UNIFORM);
        mTexMatrixLocation = GLES20.glGetUniformLocation(mProgram, TEXUTRE_MATRIX);

        initMatrices();
    }

    private void initMatrices() {
        mVertexBuffer = ByteBuffer.allocateDirect(
                TextureRotationUtil.VERTEX_MATRIX.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer();
        mVertexBuffer.put(TextureRotationUtil.VERTEX_MATRIX).position(0);

        mTextureBuffer = ByteBuffer.
                allocateDirect(TextureRotationUtil.TEXTURE_MATRIX.length * 4).
                order(ByteOrder.nativeOrder()).
                asFloatBuffer();
        mTextureBuffer.put(TextureRotationUtil.TEXTURE_MATRIX).position(0);
    }

    void resize(int width, int height) {
        if (mWidth != width || mHeight != height) {
            mWidth = width;
            mHeight = height;
            deleteFramebuffer();
            bindFramebuffer(width, height);
        }
    }

    private void bindFramebuffer(int width, int height) {
        GLES20.glGenFramebuffers(1, mFramebuffer, 0);
        GlUtil.checkGlError("glGenFramebuffers");

        GLES20.glGenTextures(1, mTargetTexture, 0);
        GlUtil.checkGlError("glGenTextures");

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTargetTexture[0]);
        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, width, height, 0,
                GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);

        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, mFramebuffer[0]);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER,
                GLES20.GL_COLOR_ATTACHMENT0,
                GLES20.GL_TEXTURE_2D,
                mTargetTexture[0], 0);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
    }

    /**
     * Transform the given texture according to the texture
     * matrix passed, and the
     * @param textureId
     * @param texMatrix
     * @return the transformed texture id
     */
    int rotate(int textureId, float[] texMatrix) {
        GlUtil.checkGlError("preProcess");
        GLES20.glUseProgram(mProgram);
        GlUtil.checkGlError("glUseProgram");

        mVertexBuffer.position(0);
        GLES20.glVertexAttribPointer(mVertexCoordLocation, 2, GLES20.GL_FLOAT, false, 0, mVertexBuffer);
        GLES20.glEnableVertexAttribArray(mVertexCoordLocation);
        GlUtil.checkGlError("glEnableVertexAttribArray");

        mTextureBuffer.position(0);
        GLES20.glVertexAttribPointer(mTexCoordLocation, 2, GLES20.GL_FLOAT, false, 0, mTextureBuffer);
        GLES20.glEnableVertexAttribArray(mTexCoordLocation);
        GlUtil.checkGlError("glEnableVertexAttribArray");

        GLES20.glUniformMatrix4fv(mTexMatrixLocation, 1, false, texMatrix, 0);
        GlUtil.checkGlError("glUniformMatrix4fv");

        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId);
        GLES20.glUniform1i(mTexSampleLocation, 0);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, mFramebuffer[0]);
        GlUtil.checkGlError("glBindFramebuffer");
        GLES20.glViewport(0, 0, mWidth, mHeight);
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);

        GLES20.glDisableVertexAttribArray(mVertexCoordLocation);
        GLES20.glDisableVertexAttribArray(mTexCoordLocation);
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glUseProgram(0);

        return mTargetTexture[0];
    }

    private void deleteFramebuffer() {
        if (mTargetTexture[0] != 0) {
            GLES20.glDeleteTextures(1, mTargetTexture, 0);
            mTargetTexture[0] = 0;
        }

        if (mFramebuffer[0] != 0) {
            GLES20.glDeleteFramebuffers(1, mFramebuffer, 0);
            mFramebuffer[0] = 0;
        }
    }

    void destroyProgram() {
        deleteFramebuffer();

        if (mProgram != 0) {
            GLES20.glDeleteProgram(mProgram);
        }
    }
}
