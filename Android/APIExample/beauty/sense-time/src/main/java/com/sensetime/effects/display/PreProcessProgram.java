package com.sensetime.effects.display;

import android.opengl.GLES11Ext;
import android.opengl.GLES20;

import com.sensetime.effects.glutils.GlUtil;
import com.sensetime.effects.glutils.OpenGLUtils;
import com.sensetime.effects.glutils.TextureRotationUtil;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

/**
 * inherits the pro-processing operation from SenseTime
 * which transforms a textureOES to a texture2D
 */
class PreProcessProgram {
    private static final String CAMERA_INPUT_VERTEX_SHADER =
        "attribute vec4 position;\n" +
        "attribute vec4 inputTextureCoordinate;\n" +
        "varying vec2 textureCoordinate;\n" +
        "void main() {\n" +
        "	gl_Position = position;\n" +
        "	textureCoordinate = inputTextureCoordinate.xy;\n" +
        "}";

    private static final String CAMERA_INPUT_FRAGMENT_SHADER_OES =
        "#extension GL_OES_EGL_image_external : require\n" +
        "precision mediump float;\n" +
        "varying vec2 textureCoordinate;\n" +
        "uniform samplerExternalOES inputImageTexture;\n" +
        "void main() {\n" +
        "	gl_FragColor = texture2D(inputImageTexture, textureCoordinate);\n" +
        "}";

    // Four vertices for the four corners of the triangle
    private final static String POSITION_COORDINATE = "position";

    // Corresponding four texture coordinates for the four vertices
    private final static String TEXTURE_COORDINATE = "inputTextureCoordinate";

    private final static String TEXTURE_UNIFORM = "inputImageTexture";

    private int mProgram;

    // locations in the shader mProgram
    private int mVertexCoordLocation;
    private int mTexCoordLocation;
    private int mTexSampleLocation;

    private FloatBuffer mGLVertexMatrixBuffer;
    private FloatBuffer mTextureBuffer;

    // Frame buffer binds a texture which stores
    // the writing result to the frame buffer.
    private int[] mFramebuffer = new int[1];
    private int[] mTargetTexture = new int[1];
    private int mWidth;
    private int mHeight;
    private int mLastOrientation = -1;

    PreProcessProgram() {
        mProgram = OpenGLUtils.loadProgram(
                CAMERA_INPUT_VERTEX_SHADER,
                CAMERA_INPUT_FRAGMENT_SHADER_OES);

        mVertexCoordLocation = GLES20.glGetAttribLocation(mProgram, POSITION_COORDINATE);
        mTexCoordLocation = GLES20.glGetAttribLocation(mProgram, TEXTURE_COORDINATE);
        mTexSampleLocation = GLES20.glGetUniformLocation(mProgram, TEXTURE_UNIFORM);

        initBuffers();
    }

    private void initBuffers() {
        mGLVertexMatrixBuffer = ByteBuffer.allocateDirect(
                TextureRotationUtil.VERTEX_MATRIX.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer();
        mGLVertexMatrixBuffer.put(TextureRotationUtil.VERTEX_MATRIX).position(0);
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
     * Perform the transformation
     * @param textureId target texture id
     * @param buffer buffer to store the byte array of the image result
     * @return texture id of the result processed image
     */
    int preProcess(int textureId, ByteBuffer buffer) {
        GlUtil.checkGlError("preProcess");
        GLES20.glUseProgram(mProgram);
        GlUtil.checkGlError("glUseProgram");

        mGLVertexMatrixBuffer.position(0);
        GLES20.glVertexAttribPointer(mVertexCoordLocation, 2, GLES20.GL_FLOAT, false, 0, mGLVertexMatrixBuffer);
        GLES20.glEnableVertexAttribArray(mVertexCoordLocation);
        GlUtil.checkGlError("glEnableVertexAttribArray");

        mTextureBuffer.position(0);
        GLES20.glVertexAttribPointer(mTexCoordLocation, 2, GLES20.GL_FLOAT, false, 0, mTextureBuffer);
        GLES20.glEnableVertexAttribArray(mTexCoordLocation);
        GlUtil.checkGlError("glEnableVertexAttribArray");

        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, textureId);
        GLES20.glUniform1i(mTexSampleLocation, 0);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, mFramebuffer[0]);
        GlUtil.checkGlError("glBindFramebuffer");
        GLES20.glViewport(0, 0, mWidth, mHeight);
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);

        if (buffer != null) {
            GLES20.glReadPixels(0, 0, mWidth, mHeight, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, buffer);
        }

        GLES20.glDisableVertexAttribArray(mVertexCoordLocation);
        GLES20.glDisableVertexAttribArray(mTexCoordLocation);
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, 0);
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

    /**
     * Get a sequence of texture coordinates based on the
     * orientation and flipping requirement
     * @param orientation
     * @param flipHorizontal
     * @param flipVertical
     */
    void adjustTextureBuffer(int orientation, boolean flipHorizontal, boolean flipVertical) {
        if (orientation == mLastOrientation) {
            return;
        }

        mLastOrientation = orientation;
        float[] textureCords = TextureRotationUtil.getRotation(mLastOrientation, flipHorizontal, flipVertical);

        //float[] textureCords = TextureRotationUtil.TEXTURE_NO_ROTATION;

        if (mTextureBuffer == null) {
            mTextureBuffer = ByteBuffer.
                allocateDirect(textureCords.length * 4).
                order(ByteOrder.nativeOrder()).
                asFloatBuffer();
        }

        mTextureBuffer.clear();
        mTextureBuffer.put(textureCords).position(0);
    }
}
