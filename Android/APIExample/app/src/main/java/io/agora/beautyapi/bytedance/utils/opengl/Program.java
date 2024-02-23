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

package io.agora.beautyapi.bytedance.utils.opengl;

import android.content.Context;
import android.graphics.Point;
import android.opengl.GLES20;

import java.nio.ByteBuffer;


/**
 * The type Program.
 */
public abstract class Program {
    private static final String TAG = GlUtil.TAG;

    /**
     * The M program handle.
     */
// Handles to the GL program and various components of it.
    protected int mProgramHandle;

    /**
     * The M drawable 2 d.
     */
    protected Drawable2d mDrawable2d;


    /**
     * The M frame buffers.
     */
    protected int[] mFrameBuffers;
    /**
     * The M frame buffer textures.
     */
    protected int[] mFrameBufferTextures;
    /**
     * The Frame buffer num.
     */
    protected int frameBufferNum = 1;
    /**
     * The M frame buffer shape.
     */
    protected Point mFrameBufferShape;

    /**
     * Prepares the program in the current EGL context.
     *
     * @param vertexShader      the vertex shader
     * @param fragmentShader2D the fragment shader 2 d
     */
    public Program(String vertexShader, String fragmentShader2D) {
        mProgramHandle = GlUtil.createProgram(vertexShader, fragmentShader2D);
        mDrawable2d = getDrawable2d();
        getLocations();
    }

    /**
     * Instantiates a new Program.
     *
     * @param context                  the context
     * @param vertexShaderResourceId   the vertex shader resource id
     * @param fragmentShaderResourceId the fragment shader resource id
     */
    public Program(Context context, int vertexShaderResourceId, int fragmentShaderResourceId) {
        this(Extensions.readTextFileFromResource(context, vertexShaderResourceId), Extensions.readTextFileFromResource(context, fragmentShaderResourceId));
    }

    /**
     * Update vertex array.
     *
     * @param fullRectangleCoords the full rectangle coords
     */
    public void updateVertexArray(float[] fullRectangleCoords) {
        mDrawable2d.updateVertexArray(fullRectangleCoords);
    }

    /**
     * Update tex coord array.
     *
     * @param fullRectangleTexCoords the full rectangle tex coords
     */
    public void updateTexCoordArray(float[] fullRectangleTexCoords) {
        mDrawable2d.updateTexCoordArray(fullRectangleTexCoords);
    }

    /**
     * Update tex coord array fb.
     *
     * @param coords the coords
     */
    public void updateTexCoordArrayFB(float[] coords) {
        mDrawable2d.updateTexCoordArrayFB(coords);
    }

    /**
     * Gets drawable 2 d.
     *
     * @return the drawable 2 d
     */
    protected abstract Drawable2d getDrawable2d();

    /**
     * get locations of attributes and uniforms
     */
    protected abstract void getLocations();

    /**
     * Issues the draw call.  Does the full setup on every call.
     *
     * @param textureId the texture id
     * @param width     the width
     * @param height    the height
     * @param mvpMatrix the mvp matrix
     */
    public abstract void drawFrameOnScreen(int textureId, int width, int height, float[] mvpMatrix);


    /**
     * Draw frame off screen int.
     *
     * @param textureId the texture id
     * @param width     the width
     * @param height    the height
     * @param mvpMatrix the mvp matrix
     * @return the int
     */
    public abstract int drawFrameOffScreen(int textureId, int width, int height, float[] mvpMatrix);

    /**
     * Read buffer byte buffer.
     *
     * @param textureId the texture id
     * @param width     the width
     * @param height    the height
     * @return the byte buffer
     */
    public abstract ByteBuffer readBuffer(int textureId, int width, int height);

    /**
     * Init frame buffer if need.
     *
     * @param width  the width
     * @param height the height
     */
    protected void initFrameBufferIfNeed(int width, int height) {
        boolean need = false;
        if (null == mFrameBufferShape || mFrameBufferShape.x != width || mFrameBufferShape.y != height) {
            need = true;
        }
        if (mFrameBuffers == null || mFrameBufferTextures == null) {
            need = true;
        }
        if (need) {
            mFrameBuffers = new int[frameBufferNum];
            mFrameBufferTextures = new int[frameBufferNum];
            GLES20.glGenFramebuffers(frameBufferNum, mFrameBuffers, 0);
            GLES20.glGenTextures(frameBufferNum, mFrameBufferTextures, 0);
            for (int i = 0; i < frameBufferNum; i++) {
                bindFrameBuffer(mFrameBufferTextures[i], mFrameBuffers[i], width, height);
            }
            mFrameBufferShape = new Point(width, height);

        }

    }

    private void destroyFrameBuffers() {
        if (mFrameBufferTextures != null) {
            GLES20.glDeleteTextures(frameBufferNum, mFrameBufferTextures, 0);
            mFrameBufferTextures = null;
        }
        if (mFrameBuffers != null) {
            GLES20.glDeleteFramebuffers(frameBufferNum, mFrameBuffers, 0);
            mFrameBuffers = null;
        }
    }

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
     * Releases the program.
     * <p>
     * The appropriate EGL context must be current (i.e. the one that was used to create
     * the program).
     */
    public void release() {
        destroyFrameBuffers();
        GLES20.glDeleteProgram(mProgramHandle);
        mProgramHandle = -1;
    }
}
