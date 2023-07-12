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
import android.opengl.GLES30;

public class GLCopyHelper {
    private final int bufferCount;

    public GLCopyHelper(){
        this(1);
    }

    public GLCopyHelper(int bufferCount){
        this.bufferCount = bufferCount;
    }

    private int[] mDstFrameBuffer;
    private int[] mSrcFrameBuffer;

    public void copy2DTextureToOesTexture(
            int srcTexture,
            int dstTexture,
            int width, int height,
            int index){
        if(mDstFrameBuffer == null){
            mDstFrameBuffer = new int[bufferCount];
            GLES20.glGenFramebuffers(bufferCount, mDstFrameBuffer, 0);
        }

        if(mSrcFrameBuffer == null){
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

    public void release(){
        if(mDstFrameBuffer != null){
            GLES20.glDeleteFramebuffers(mDstFrameBuffer.length, mDstFrameBuffer, 0);
            mDstFrameBuffer = null;
        }

        if(mSrcFrameBuffer != null){
            GLES20.glDeleteFramebuffers(mSrcFrameBuffer.length, mSrcFrameBuffer, 0);
            mSrcFrameBuffer = null;
        }
    }
}
