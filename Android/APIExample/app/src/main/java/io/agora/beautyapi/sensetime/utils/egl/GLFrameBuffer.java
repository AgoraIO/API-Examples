package io.agora.beautyapi.sensetime.utils.egl;

import android.graphics.Matrix;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;

import io.agora.base.internal.video.GlRectDrawer;
import io.agora.base.internal.video.RendererCommon;

public class GLFrameBuffer {

    private int mFramebufferId = -1;
    private int mTextureId = -1;
    private int mWidth, mHeight, mRotation;
    private boolean isFlipV, isFlipH, isTextureInner, isTextureChanged, isSizeChanged;

    private RendererCommon.GlDrawer drawer;

    private float[] mTexMatrix = GlUtil.IDENTITY_MATRIX;

    public GLFrameBuffer() {

    }

    public boolean setSize(int width, int height) {
        if (mWidth != width || mHeight != height) {
            mWidth = width;
            mHeight = height;
            isSizeChanged = true;
            return true;
        }
        return false;
    }

    public void setRotation(int rotation) {
        if (mRotation != rotation) {
            mRotation = rotation;
        }
    }

    public void setFlipV(boolean flipV) {
        if (isFlipV != flipV) {
            isFlipV = flipV;
        }
    }

    public void setFlipH(boolean flipH) {
        if (isFlipH != flipH) {
            isFlipH = flipH;
        }
    }

    public void setTextureId(int textureId){
        if(mTextureId != textureId){
            deleteTexture();
            mTextureId = textureId;
            isTextureChanged = true;
        }
    }

    public int getTextureId(){
        return mTextureId;
    }

    public void setTexMatrix(float[] matrix) {
        if (matrix != null) {
            mTexMatrix = matrix;
        } else {
            mTexMatrix = GlUtil.IDENTITY_MATRIX;
        }
    }

    public void resetTransform(){
        mTexMatrix = GlUtil.IDENTITY_MATRIX;
        isFlipH = isFlipV = false;
        mRotation = 0;
    }

    public int process(int textureId, int textureType) {
        if (mWidth <= 0 && mHeight <= 0) {
            throw new RuntimeException("setSize firstly!");
        }

        if(mTextureId == -1){
            mTextureId = createTexture(mWidth, mHeight);
            bindFramebuffer(mTextureId);
            isTextureInner = true;
        }else if(isTextureInner && isSizeChanged){
            GLES20.glDeleteTextures(1, new int[]{mTextureId}, 0);
            mTextureId = createTexture(mWidth, mHeight);
            bindFramebuffer(mTextureId);
        }else if(isTextureChanged){
            bindFramebuffer(mTextureId);
        }
        isTextureChanged = false;
        isSizeChanged = false;

        if(drawer == null){
            drawer = new GlRectDrawer();
        }

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, mFramebufferId);
        GlUtil.checkGlError("glBindFramebuffer");

        Matrix transform = RendererCommon.convertMatrixToAndroidGraphicsMatrix(mTexMatrix);
        transform.preTranslate(0.5f, 0.5f);
        transform.preRotate(mRotation, 0.f, 0.f);
        transform.preScale(
                isFlipH ? -1.f: 1.f,
                isFlipV ? -1.f: 1.f
        );
        transform.preTranslate(-0.5f, -0.5f);
        float[] matrix = RendererCommon.convertMatrixFromAndroidGraphicsMatrix(transform);

        if(textureType == GLES11Ext.GL_TEXTURE_EXTERNAL_OES){
            drawer.drawOes(textureId, 0, matrix, mWidth, mHeight, 0, 0, mWidth, mHeight);
        }else{
            drawer.drawRgb(textureId, 0, matrix, mWidth, mHeight, 0, 0, mWidth, mHeight);
        }
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glFinish();

        return mTextureId;
    }

    public void release(){
        deleteTexture();
        deleteFramebuffer();

        if(drawer != null){
            drawer.release();
            drawer = null;
        }
    }


    private void deleteFramebuffer() {
        if (mFramebufferId != -1) {
            GLES20.glDeleteFramebuffers(1, new int[]{mFramebufferId}, 0);
            mFramebufferId = -1;
        }
    }

    public int createTexture(int width, int height){
        int[] textures = new int[1];
        GLES20.glGenTextures(1, textures, 0);
        GlUtil.checkGlError("glGenTextures");
        int textureId = textures[0];

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

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, GLES20.GL_NONE);

        return textureId;
    }

    public void resizeTexture(int textureId, int width, int height) {
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId);
        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, width, height, 0,
                GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, GLES20.GL_NONE);
    }

    private void deleteTexture() {
        if (isTextureInner && mTextureId != -1) {
            GLES20.glDeleteTextures(1, new int[]{mTextureId}, 0);
        }
        isTextureInner = false;
        mTextureId = -1;
    }

    private void bindFramebuffer(int textureId) {
        if(mFramebufferId == -1){
            int[] framebuffers = new int[1];
            GLES20.glGenFramebuffers(1, framebuffers, 0);
            GlUtil.checkGlError("glGenFramebuffers");
            mFramebufferId = framebuffers[0];
        }
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, mFramebufferId);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER,
                GLES20.GL_COLOR_ATTACHMENT0,
                GLES20.GL_TEXTURE_2D,
                textureId, 0);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, GLES20.GL_NONE);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, GLES20.GL_NONE);
    }

}
