package io.agora.beautyapi.sensetime.utils.egl;

import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLES30;

/**
 * The type Gl copy helper.
 */
public class GLCopyHelper {
    private final int bufferCount;

    /**
     * Instantiates a new Gl copy helper.
     */
    public GLCopyHelper() {
        this(1);
    }

    /**
     * Instantiates a new Gl copy helper.
     *
     * @param bufferCount the buffer count
     */
    public GLCopyHelper(int bufferCount) {
        this.bufferCount = bufferCount;
    }

    private int[] mDstFrameBuffer;
    private int[] mSrcFrameBuffer;

    /**
     * Copy 2 d texture to oes texture.
     *
     * @param srcTexture the src texture
     * @param dstTexture the dst texture
     * @param width      the width
     * @param height     the height
     * @param index      the index
     */
    public void copy2DTextureToOesTexture(
            int srcTexture,
            int dstTexture,
            int width, int height,
            int index) {
        if (mDstFrameBuffer == null) {
            mDstFrameBuffer = new int[bufferCount];
            GLES20.glGenFramebuffers(bufferCount, mDstFrameBuffer, 0);
        }

        if (mSrcFrameBuffer == null) {
            mSrcFrameBuffer = new int[bufferCount];
            GLES20.glGenFramebuffers(bufferCount, mSrcFrameBuffer, 0);
        }

        GLES30.glBindFramebuffer(GLES30.GL_READ_FRAMEBUFFER, mSrcFrameBuffer[index]);
        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, srcTexture);
        GLES30.glFramebufferTexture2D(GLES30.GL_READ_FRAMEBUFFER, GLES30.GL_COLOR_ATTACHMENT0, GLES30.GL_TEXTURE_2D, srcTexture, 0);
        GLES30.glBindFramebuffer(GLES30.GL_DRAW_FRAMEBUFFER, mDstFrameBuffer[index]);
        GLES30.glFramebufferTexture2D(GLES30.GL_DRAW_FRAMEBUFFER,
                GLES30.GL_COLOR_ATTACHMENT0, GLES11Ext.GL_TEXTURE_EXTERNAL_OES, dstTexture, 0);
        GLES30.glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GLES30.GL_COLOR_BUFFER_BIT, GLES30.GL_LINEAR);
        GLES30.glBindFramebuffer(GLES30.GL_DRAW_FRAMEBUFFER, 0);
        GLES30.glBindFramebuffer(GLES30.GL_READ_FRAMEBUFFER, 0);
        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, 0);
        GLES30.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, 0);
    }

    /**
     * Release.
     */
    public void release() {
        if (mDstFrameBuffer != null) {
            GLES20.glDeleteFramebuffers(mDstFrameBuffer.length, mDstFrameBuffer, 0);
            mDstFrameBuffer = null;
        }

        if (mSrcFrameBuffer != null) {
            GLES20.glDeleteFramebuffers(mSrcFrameBuffer.length, mSrcFrameBuffer, 0);
            mSrcFrameBuffer = null;
        }
    }
}
