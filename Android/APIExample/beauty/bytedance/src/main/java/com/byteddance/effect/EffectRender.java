package com.byteddance.effect;

import android.graphics.Point;
import android.opengl.GLES20;
import android.opengl.Matrix;

import com.byteddance.model.TextureFormat;
import com.byteddance.opengl.GlUtil;
import com.byteddance.opengl.ProgramManager;

import java.nio.ByteBuffer;

import static android.opengl.GLES20.GL_RGBA;


public class EffectRender {
    private volatile float[] mMVPMatrix = new float[16];
    protected int[] mFrameBuffers;
    protected int[] mFrameBufferTextures;
    protected int FRAME_BUFFER_NUM = 1;
    protected Point mFrameBufferShape;
    private ProgramManager mProgramManager;
    private int mSurfaceWidth, mSurfaceHeight;

    /**
     * 设置纹理绘制的目标界面的尺寸
     * @param surfaceWidth
     * @param surfaceHeight
     */
    public void setViewSize(int surfaceWidth, int surfaceHeight) {
        mSurfaceWidth = surfaceWidth;
        mSurfaceHeight = surfaceHeight;
    }

    /**
     * 准备帧缓冲区纹理对象
     * @param width 纹理宽度
     * @param height 纹理高度
     * @return 纹理ID
     */
    public int prepareTexture(int width, int height) {
        initFrameBufferIfNeed(width, height);
        return mFrameBufferTextures[0];
    }

    /**
     * 初始化帧缓冲区
     * @param width 缓冲的纹理宽度
     * @param height 缓冲的纹理高度
     */
    private void initFrameBufferIfNeed(int width, int height) {
        boolean need = false;
        if (null == mFrameBufferShape || mFrameBufferShape.x != width || mFrameBufferShape.y != height) {
            need = true;
        }
        if (mFrameBuffers == null || mFrameBufferTextures == null) {
            need = true;
        }
        if (need) {
            destroyFrameBuffers();
            mFrameBuffers = new int[FRAME_BUFFER_NUM];
            mFrameBufferTextures = new int[FRAME_BUFFER_NUM];
            GLES20.glGenFramebuffers(FRAME_BUFFER_NUM, mFrameBuffers, 0);
            GLES20.glGenTextures(FRAME_BUFFER_NUM, mFrameBufferTextures, 0);
            for (int i = 0; i < FRAME_BUFFER_NUM; i++) {
                bindFrameBuffer(mFrameBufferTextures[i], mFrameBuffers[i], width, height);
            }
            mFrameBufferShape = new Point(width, height);
        }

    }

    /**
     * 销毁帧缓冲区对象
     */
    private void destroyFrameBuffers() {
        if (mFrameBufferTextures != null) {
            GLES20.glDeleteTextures(FRAME_BUFFER_NUM, mFrameBufferTextures, 0);
            mFrameBufferTextures = null;
        }
        if (mFrameBuffers != null) {
            GLES20.glDeleteFramebuffers(FRAME_BUFFER_NUM, mFrameBuffers, 0);
            mFrameBuffers = null;
        }
    }

    /**
     * 纹理参数设置+buffer绑定
     * set texture params
     * and bind buffer
     */
    private void bindFrameBuffer(int textureId, int frameBuffer, int width, int height) {
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId);
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

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, frameBuffer);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0,
                GLES20.GL_TEXTURE_2D, textureId, 0);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
    }

    /**
     * 在屏幕上渲染纹理
     * @param textureId 纹理ID
     * @param format 纹理格式
     * @param textureWidth 纹理宽度
     * @param textureHeight 纹理高度
     * @param rotation 旋转角度
     * @param flipHorizontal 是否水平翻转
     * @param flipVertical 是否垂直翻转
     */
    public void drawFrameOnScreen(int textureId, TextureFormat format, int textureWidth, int textureHeight, int rotation, boolean flipHorizontal, boolean flipVertical) {
        if (null == mProgramManager) {
            mProgramManager = new ProgramManager();
        }
        Matrix.setIdentityM(mMVPMatrix, 0);
        if (rotation % 180 == 90){
             GlUtil.getShowMatrix(mMVPMatrix,textureHeight, textureWidth, mSurfaceWidth, mSurfaceHeight);

        }else {
             GlUtil.getShowMatrix(mMVPMatrix, textureWidth, textureHeight, mSurfaceWidth, mSurfaceHeight);

        }
        GlUtil.flip(mMVPMatrix, flipHorizontal, flipVertical);

        GlUtil.rotate(mMVPMatrix, rotation);

        mProgramManager.getProgram(format).drawFrameOnScreen(textureId, mSurfaceWidth, mSurfaceHeight, mMVPMatrix);

    }


    public void drawFrameCentnerInside(int textureId, TextureFormat format, int textureWidth, int textureHeight){
        if (null == mProgramManager) {
            mProgramManager = new ProgramManager();
        }
        Matrix.setIdentityM(mMVPMatrix, 0);

        mMVPMatrix=  GlUtil.changeMVPMatrixInside(mSurfaceWidth, mSurfaceHeight, textureWidth, textureHeight);

        mProgramManager.getProgram(format).drawFrameOnScreen(textureId, mSurfaceWidth, mSurfaceHeight, mMVPMatrix);
    }


    /**
     * 在帧缓冲区进行纹理旋转&翻转等操作
     * @param srcTextureId 输入纹理
     * @param format 输入纹理格式
     * @param dstWidth 输出纹理的宽度
     * @param dstHeight 输出纹理的高度
     * @param cameraRotation 输入纹理的旋转角
     * @param flipHorizontal 是否水平翻转
     * @param flipVertical 是否垂直翻转
     * @return 输出处理后的纹理
     */
    public int drawFrameOffScreen(int srcTextureId, TextureFormat format, int dstWidth, int dstHeight, int cameraRotation, boolean flipHorizontal, boolean flipVertical) {
        if (null == mProgramManager) {
            mProgramManager = new ProgramManager();
        }
        Matrix.setIdentityM(mMVPMatrix, 0);
        // GlUtil.flip(mMVPMatrix, flipHorizontal, flipVertical);
        GlUtil.rotate(mMVPMatrix, cameraRotation);
        return mProgramManager.getProgram(format).drawFrameOffScreen(srcTextureId, dstWidth, dstHeight, mMVPMatrix);
    }

    public int drawFrameOffScreen(int srcTextureId, TextureFormat format, int width, int height) {
        if (null == mProgramManager) mProgramManager = new ProgramManager();
        Matrix.setIdentityM(mMVPMatrix, 0);
        return mProgramManager.getProgram(format).drawFrameOffScreen(srcTextureId, width, height, mMVPMatrix);
    }

    /**
     * 释放资源，包括帧缓冲区及Program对象
     */
    public void release() {
        destroyFrameBuffers();
        if (null != mProgramManager) {
            mProgramManager.release();
        }

    }

    /**
     * 读取渲染结果的buffer
     * @param imageWidth 图像宽度
     * @param imageHeight 图像高度
     * @return 渲染结果的像素Buffer 格式RGBA
     */
    public ByteBuffer captureRenderResult(int imageWidth, int imageHeight){
        if (mFrameBufferTextures == null) return null;
        int textureId = mFrameBufferTextures[0];
        if (null == mFrameBufferTextures || textureId == GlUtil.NO_TEXTURE) {
            return null;
        }
        if (imageWidth* imageHeight == 0){
            return  null;
        }
        ByteBuffer mCaptureBuffer = ByteBuffer.allocateDirect(imageWidth* imageHeight*4);

        mCaptureBuffer.position(0);
        int[] frameBuffer = new int[1];
        GLES20.glGenFramebuffers(1,frameBuffer,0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, frameBuffer[0]);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0,
                GLES20.GL_TEXTURE_2D, textureId, 0);
        GLES20.glReadPixels(0, 0, imageWidth, imageHeight,
                GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, mCaptureBuffer);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        if (null != frameBuffer) {
            GLES20.glDeleteFramebuffers(1, frameBuffer, 0);
        }
        return mCaptureBuffer;
    }

    /**
     * 读取渲染结果的buffer
     * @param imageWidth 图像宽度
     * @param imageHeight 图像高度
     * @return 渲染结果的像素Buffer 格式RGBA
     */
    public ByteBuffer captureRenderResult(int textureId, int imageWidth, int imageHeight){
        if ( textureId == GlUtil.NO_TEXTURE) {
            return null;
        }
        if (imageWidth* imageHeight == 0){
            return  null;
        }
        ByteBuffer mCaptureBuffer = ByteBuffer.allocateDirect(imageWidth* imageHeight*4);

        mCaptureBuffer.position(0);
        int[] frameBuffer = new int[1];
        GLES20.glGenFramebuffers(1,frameBuffer,0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, frameBuffer[0]);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0,
                GLES20.GL_TEXTURE_2D, textureId, 0);
        GLES20.glReadPixels(0, 0, imageWidth, imageHeight,
                GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, mCaptureBuffer);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        if (null != frameBuffer) {
            GLES20.glDeleteFramebuffers(1, frameBuffer, 0);
        }
        return mCaptureBuffer;
    }

    /**
     * 纹理拷贝
     * @param srcTexture
     * @param dstTexture
     * @param width
     * @param height
     * @return
     */
    public boolean copyTexture(int srcTexture,int dstTexture, int width, int height){
        if (srcTexture == GlUtil.NO_TEXTURE || dstTexture == GlUtil.NO_TEXTURE) {
            return false;
        }
        if (width* height == 0){
            return  false;
        }
        int[] frameBuffer = new int[1];
        GLES20.glGenFramebuffers(1,frameBuffer,0);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, frameBuffer[0]);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0,
                GLES20.GL_TEXTURE_2D, srcTexture, 0);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, dstTexture);
        GLES20.glCopyTexImage2D(GLES20.GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        if (null != frameBuffer) {
            GLES20.glDeleteFramebuffers(1, frameBuffer, 0);
        }
        int error = GLES20.glGetError();
        if (error != GLES20.GL_NO_ERROR) {
            String msg ="copyTexture glError 0x" + Integer.toHexString(error);
            return false;
        }
        return true;


    }

}













