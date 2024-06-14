package io.agora.api.example.examples.advanced.videoRender;

import android.graphics.Matrix;
import android.opengl.GLES20;

import io.agora.base.JavaI420Buffer;
import io.agora.base.internal.video.GlRectDrawer;
import io.agora.base.internal.video.GlUtil;
import io.agora.base.internal.video.RendererCommon;

/**
 * The type Yuv fbo program.
 */
public class YuvFboProgram {

    private int[] mFboTextureId;
    private final YuvUploader yuvUploader;
    private final GlRectDrawer glRectDrawer;

    private int mWidth, mHeight;
    private volatile boolean isRelease;

    /**
     * Instantiates a new Yuv fbo program.
     */
// GL Thread
    public YuvFboProgram() {
        yuvUploader = new YuvUploader();
        glRectDrawer = new GlRectDrawer();
    }

    /**
     * Release.
     */
// GL Thread
    public void release() {
        isRelease = true;
        if (mFboTextureId != null) {
            GLES20.glDeleteFramebuffers(1, mFboTextureId, 0);
            GLES20.glDeleteTextures(1, mFboTextureId, 1);
            yuvUploader.release();
            glRectDrawer.release();
            mFboTextureId = null;
        }
    }

    /**
     * Draw yuv integer.
     *
     * @param yuv    the yuv
     * @param width  the width
     * @param height the height
     * @return the integer
     */
// GL Thread
    public Integer drawYuv(byte[] yuv, int width, int height) {
        if (isRelease) {
            return -1;
        }
        if (mFboTextureId == null) {
            mFboTextureId = new int[2];
            GLES20.glGenFramebuffers(1, mFboTextureId, 0);
            int fboId = mFboTextureId[0];

            int texture = GlUtil.generateTexture(GLES20.GL_TEXTURE_2D);
            mFboTextureId[1] = texture;

            mWidth = width;
            mHeight = height;
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texture);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, width, height, 0,
                    GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);

            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, fboId);
            GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER,
                    GLES20.GL_COLOR_ATTACHMENT0,
                    GLES20.GL_TEXTURE_2D, texture, 0);
        } else if (mWidth != width || mHeight != height) {
            GLES20.glDeleteFramebuffers(1, mFboTextureId, 0);
            GLES20.glDeleteTextures(1, mFboTextureId, 1);
            mFboTextureId = null;
            return drawYuv(yuv, width, height);
        } else {
            int fboId = mFboTextureId[0];
            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, fboId);
        }
        GLES20.glViewport(0, 0, mWidth, mHeight);

        JavaI420Buffer i420Buffer = JavaI420Buffer.allocate(width, height);
        i420Buffer.getDataY().put(yuv, 0, i420Buffer.getDataY().limit());
        i420Buffer.getDataU().put(yuv, i420Buffer.getDataY().limit(), i420Buffer.getDataU().limit());
        i420Buffer.getDataV().put(yuv, i420Buffer.getDataY().limit() + i420Buffer.getDataU().limit(), i420Buffer.getDataV().limit());

        yuvUploader.uploadFromBuffer(i420Buffer);
        Matrix matrix = new Matrix();
        matrix.preTranslate(0.5f, 0.5f);
        matrix.preScale(1f, -1f); // I420-frames are upside down
        matrix.preTranslate(-0.5f, -0.5f);
        glRectDrawer.drawYuv(yuvUploader.getYuvTextures(), 0, RendererCommon.convertMatrixFromAndroidGraphicsMatrix(matrix), width, height, 0, 0, width, height, 0);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glFlush();

        return mFboTextureId[1];
    }


}
