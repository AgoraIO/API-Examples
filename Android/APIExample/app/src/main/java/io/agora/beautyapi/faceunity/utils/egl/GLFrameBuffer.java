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

import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.Matrix;

import io.agora.beautyapi.faceunity.utils.egl.program.BaseProgram;
import io.agora.beautyapi.faceunity.utils.egl.program.GL2DProgram;
import io.agora.beautyapi.faceunity.utils.egl.program.OESProgram;

public class GLFrameBuffer {

    private int mFramebufferId = -1;
    private int mTextureId = -1;
    private int mWidth, mHeight, mRotation;
    private boolean isFlipV, isFlipH, isTextureInner, isTextureChanged, isSizeChanged;

    private OESProgram mOESProgram;
    private GL2DProgram mGL2DProgram;

    private float[] mMVPMatrix = new float[16];
    private float[] mTexMatrix = GLUtils.IDENTITY_MATRIX;

    public GLFrameBuffer() {
        Matrix.setIdentityM(mMVPMatrix, 0);
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
            mMVPMatrix = GLUtils.createTransformMatrix(rotation, isFlipH, isFlipV);
        }
    }

    public void setFlipV(boolean flipV) {
        if (isFlipV != flipV) {
            isFlipV = flipV;
            mMVPMatrix = GLUtils.createTransformMatrix(mRotation, isFlipH, flipV);
        }
    }

    public void setFlipH(boolean flipH) {
        if (isFlipH != flipH) {
            isFlipH = flipH;
            mMVPMatrix = GLUtils.createTransformMatrix(mRotation, flipH, isFlipV);
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
            mTexMatrix = GLUtils.IDENTITY_MATRIX;
        }
    }

    public void resetTransform(){
        mTexMatrix = GLUtils.IDENTITY_MATRIX;
        isFlipH = isFlipV = false;
        mRotation = 0;
        Matrix.setIdentityM(mMVPMatrix, 0);
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

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, mFramebufferId);
        GLUtils.checkGlError("glBindFramebuffer");
        GLES20.glViewport(0, 0, mWidth, mHeight);
        GLES20.glClearColor(1f, 0, 0, 1f);

        BaseProgram drawProgram;
        if(textureType == GLES11Ext.GL_TEXTURE_EXTERNAL_OES){
            if(mOESProgram == null){
                mOESProgram = new OESProgram();
            }
            drawProgram = mOESProgram;
        }else{
            if(mGL2DProgram == null){
                mGL2DProgram = new GL2DProgram();
            }
            drawProgram = mGL2DProgram;
        }
        drawProgram.draw(textureId, mTexMatrix, mMVPMatrix);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);

        return mTextureId;
    }

    public void release(){
        deleteTexture();
        deleteFramebuffer();
        if(mGL2DProgram != null){
            mGL2DProgram.release();
            mGL2DProgram = null;
        }
        if(mOESProgram != null){
            mOESProgram.release();
            mOESProgram = null;
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
        GLUtils.checkGlError("glGenTextures");
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
            GLUtils.checkGlError("glGenFramebuffers");
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
