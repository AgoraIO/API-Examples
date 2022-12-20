package com.bytedance.labcv.core.opengl;

import android.content.Context;
import android.graphics.Point;
import android.opengl.GLES20;

import java.nio.ByteBuffer;


public abstract class Program {
    private static final String TAG = GlUtil.TAG;

    // Handles to the GL program and various components of it.
    protected int mProgramHandle;

    protected Drawable2d mDrawable2d;


    protected int[] mFrameBuffers;
    protected int[] mFrameBufferTextures;
    protected int FRAME_BUFFER_NUM = 1;
    protected Point mFrameBufferShape;
    /**
     * Prepares the program in the current EGL context.
     */
    public Program(String VERTEX_SHADER, String FRAGMENT_SHADER_2D) {
        mProgramHandle = GlUtil.createProgram(VERTEX_SHADER, FRAGMENT_SHADER_2D);
        mDrawable2d = getDrawable2d();
        getLocations();
    }

    public Program(Context context, int vertexShaderResourceId, int fragmentShaderResourceId) {
        this(Extensions.readTextFileFromResource(context, vertexShaderResourceId), Extensions.readTextFileFromResource(context, fragmentShaderResourceId));
    }

    public void updateVertexArray(float[] FULL_RECTANGLE_COORDS) {
        mDrawable2d.updateVertexArray(FULL_RECTANGLE_COORDS);
    }

    public void updateTexCoordArray(float[] FULL_RECTANGLE_TEX_COORDS) {
        mDrawable2d.updateTexCoordArray(FULL_RECTANGLE_TEX_COORDS);
    }

    public void updateTexCoordArrayFB(float[] coords) {
        mDrawable2d.updateTexCoordArrayFB(coords);
    }

    protected abstract Drawable2d getDrawable2d();

    /**
     * get locations of attributes and uniforms
     */
    protected abstract void getLocations();

    /**
     * Issues the draw call.  Does the full setup on every call.
     */
    public abstract void drawFrameOnScreen(int textureId, int width, int height, float[] mvpMatrix);



    public abstract int drawFrameOffScreen(int textureId,int width, int height, float[] mvpMatrix);

    public abstract ByteBuffer readBuffer(int textureId, int width, int height);

    protected void initFrameBufferIfNeed(int width, int height) {
        boolean need = false;
        if (null == mFrameBufferShape || mFrameBufferShape.x != width || mFrameBufferShape.y != height) {
            need = true;
        }
        if (mFrameBuffers == null || mFrameBufferTextures == null) {
            need = true;
        }
        if (need) {
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

    /** {zh} 
     * 纹理参数设置+buffer绑定
     * set texture params
     * and bind buffer
     */
    /** {en} 
     * Texture parameter setting + buffer binding
     * set texture params
     * and binding buffer
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
