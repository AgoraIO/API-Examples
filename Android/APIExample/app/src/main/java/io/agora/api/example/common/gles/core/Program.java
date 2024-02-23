package io.agora.api.example.common.gles.core;

import android.content.Context;
import android.opengl.GLES20;

/**
 * Created by tujh on 2018/1/24.
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
     * @param texMatrix the tex matrix
     * @param mvpMatrix the mvp matrix
     */
    public abstract void drawFrame(int textureId, float[] texMatrix, float[] mvpMatrix);

    /**
     * Draw frame.
     *
     * @param textureId the texture id
     * @param texMatrix the tex matrix
     */
    public void drawFrame(int textureId, float[] texMatrix) {
        drawFrame(textureId, texMatrix, GlUtil.IDENTITY_MATRIX);
    }

    /**
     * Draw frame.
     *
     * @param textureId the texture id
     * @param texMatrix the tex matrix
     * @param mvpMatrix the mvp matrix
     * @param x         the x
     * @param y         the y
     * @param width     the width
     * @param height    the height
     */
    public void drawFrame(int textureId, float[] texMatrix, float[] mvpMatrix, int x, int y, int width, int height) {
        int[] originalViewport = new int[4];
        GLES20.glGetIntegerv(GLES20.GL_VIEWPORT, originalViewport, 0);
        GLES20.glViewport(x, y, width, height);
        drawFrame(textureId, texMatrix, mvpMatrix);
        GLES20.glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);
    }

    /**
     * Releases the program.
     * <p>
     * The appropriate EGL context must be current (i.e. the one that was used to create
     * the program).
     */
    public void release() {
        GLES20.glDeleteProgram(mProgramHandle);
        mProgramHandle = -1;
    }
}
