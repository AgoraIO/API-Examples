package com.sensetime.effects.display;

import android.opengl.GLES20;

import com.sensetime.effects.glutils.GlUtil;
import com.sensetime.effects.glutils.OpenGLUtils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

/**
 * Used to draw points on the specific image or window surface.
 * Specifically it can be used to draw points of face/eye features
 * on images (debug mode, preferably).
 */
public class PointProgram {
    private static final String DRAW_POINTS_VERTEX_SHADER = "" +
        "attribute vec4 aPosition;\n" +
        "void main() {\n" +
        "  gl_PointSize = 2.0;" +
        "  gl_Position = aPosition;\n" +
        "}";

    private static final String DRAW_POINTS_FRAGMENT_SHADER = "" +
        "precision mediump float;\n" +
        "uniform vec4 uColor;\n" +
        "void main() {\n" +
        "  gl_FragColor = uColor;\n" +
        "}";

    private int mProgram;

    private final static String DRAW_POINTS_COLOR = "uColor";
    private final static String DRAW_POINTS_POSITION = "aPosition";

    private int mColorLocation;
    private int mVertexLocation;

    private int[] mFramebuffer = new int[1];

    PointProgram() {
        mProgram = OpenGLUtils.loadProgram(DRAW_POINTS_VERTEX_SHADER, DRAW_POINTS_FRAGMENT_SHADER);
        mColorLocation = GLES20.glGetAttribLocation(mProgram, DRAW_POINTS_POSITION);
        mVertexLocation = GLES20.glGetUniformLocation(mProgram, DRAW_POINTS_COLOR);
        GLES20.glGenFramebuffers(1, mFramebuffer, 0);
    }

    void drawPoints(int textureId, float[] points, int viewWidth, int viewHeight) {
        GLES20.glUseProgram(mProgram);
        GLES20.glUniform4f(mColorLocation, 0.0f, 1.0f, 0.0f, 1.0f);

        FloatBuffer buff = ByteBuffer.allocateDirect(points.length * 4)
                .order(ByteOrder.nativeOrder()).asFloatBuffer();

        buff.clear();
        buff.put(points).position(0);

        GLES20.glVertexAttribPointer(mVertexLocation, 2, GLES20.GL_FLOAT, false, 0, buff);
        GLES20.glEnableVertexAttribArray(mVertexLocation);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, mFramebuffer[0]);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0,
                GLES20.GL_TEXTURE_2D, textureId, 0);

        GlUtil.checkGlError("glBindFramebuffer");
        GLES20.glViewport(0, 0, viewWidth, viewHeight);

        GLES20.glDrawArrays(GLES20.GL_POINTS, 0, points.length / 2);

        GLES20.glDisableVertexAttribArray(mVertexLocation);
        GlUtil.checkGlError("drawPoints");
        GLES20.glUseProgram(0);
    }

    void destroyProgram() {
        if (mFramebuffer[0] != 0) {
            GLES20.glBindFramebuffer(GLES20.GL_TEXTURE_2D, 0);
        }

        if (mProgram != 0) {
            GLES20.glDeleteProgram(mProgram);
        }
    }
}
