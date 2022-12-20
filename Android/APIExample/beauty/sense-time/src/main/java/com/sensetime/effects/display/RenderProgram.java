package com.sensetime.effects.display;

import android.opengl.GLES11Ext;
import android.opengl.GLES20;

import com.sensetime.effects.glutils.GlUtil;
import com.sensetime.effects.glutils.OpenGLUtils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class RenderProgram {
    // Simple vertex shader, used for all programs.
    private static final String VERTEX_SHADER =
            "uniform mat4 uMVPMatrix;\n" +
                    "uniform mat4 uTexMatrix;\n" +
                    "attribute vec4 aPosition;\n" +
                    "attribute vec4 aTextureCoord;\n" +
                    "varying vec2 vTextureCoord;\n" +
                    "void main() {\n" +
                    "    gl_Position = uMVPMatrix * aPosition;\n" +
                    "    vTextureCoord = (uTexMatrix * aTextureCoord).xy;\n" +
                    "}\n";

    // Simple fragment shader for use with external 2D textures (e.g. what we get from SurfaceTexture).
    private static final String FRAGMENT_SHADER_EXT =
            "#extension GL_OES_EGL_image_external : require\n" +
                    "precision mediump float;\n" +
                    "varying vec2 vTextureCoord;\n" +
                    "uniform samplerExternalOES sTexture;\n" +
                    "void main() {\n" +
                    "    gl_FragColor = texture2D(sTexture, vTextureCoord);\n" +
                    "}\n";

    private static final String FRAGMENT_SHADER =
            "precision mediump float;\n" +
                    "varying vec2 vTextureCoord;\n" +
                    "uniform sampler2D sTexture;\n" +
                    "void main() {\n" +
                    "    gl_FragColor = texture2D(sTexture, vTextureCoord);\n" +
                    "}\n";

    public static final float VERTEX_POSITION[] = {
            -1.0f, -1.0f,   // 0 bottom left
            1.0f, -1.0f,   // 1 bottom right
            -1.0f, 1.0f,   // 2 top left
            1.0f, 1.0f,   // 3 top right
    };
    public static final float TEXTURE_COORD[] = {
            0.0f, 0.0f,     // 2 bottom left
            1.0f, 0.0f,      // 3 bottom right
            0.0f, 1.0f,     // 0 top left
            1.0f, 1.0f,     // 1 top right
    };

    private int mProgramHandle;

    // locations in the shader mProgram
    private int muMVPMatrixLoc;
    private int muTexMatrixLoc;
    private int maPositionLoc;
    private int maTextureCoordLoc;

    private FloatBuffer mVertexArray;
    private FloatBuffer mTexCoordArray;


    private int[] mFramebuffer = new int[1];
    private int[] mTargetTexture = new int[1];
    private int mWidth, mHeight;

    private final int textureType;

    public RenderProgram(int textureType) {
        this.textureType = textureType;
        mProgramHandle = OpenGLUtils.loadProgram(VERTEX_SHADER, textureType == GLES11Ext.GL_TEXTURE_EXTERNAL_OES ? FRAGMENT_SHADER_EXT: FRAGMENT_SHADER);
        muMVPMatrixLoc = GLES20.glGetUniformLocation(mProgramHandle, "uMVPMatrix");
        muTexMatrixLoc = GLES20.glGetUniformLocation(mProgramHandle, "uTexMatrix");
        maPositionLoc = GLES20.glGetAttribLocation(mProgramHandle, "aPosition");
        maTextureCoordLoc = GLES20.glGetAttribLocation(mProgramHandle, "aTextureCoord");
        initBuffers();
    }

    private void initBuffers() {
        mVertexArray = ByteBuffer.allocateDirect(VERTEX_POSITION.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer();
        mVertexArray.put(VERTEX_POSITION).position(0);

        mTexCoordArray = ByteBuffer.allocateDirect(TEXTURE_COORD.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer();
        mTexCoordArray.put(TEXTURE_COORD).position(0);
    }

    public boolean resize(int width, int height) {
        if (mWidth != width || mHeight != height) {
            mWidth = width;
            mHeight = height;
            deleteFramebuffer();
            bindFramebuffer(width, height);
            return true;
        }
        return false;
    }

    public int process(int textureId, float[] texMatrix, float[] mvpMatrix) {
        if (mFramebuffer[0] == 0) {
            throw new RuntimeException("resize firstly!");
        }

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, mFramebuffer[0]);
        GlUtil.checkGlError("glBindFramebuffer");
        GLES20.glViewport(0, 0, mWidth, mHeight);
        GLES20.glClearColor(1f, 0, 0, 1f);


        // Select the program.
        GLES20.glUseProgram(mProgramHandle);
        GlUtil.checkGlError("glUseProgram");

        // Set the texture.
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(textureType, textureId);

        // Copy the model / view / projection matrix over.
        GLES20.glUniformMatrix4fv(muMVPMatrixLoc, 1, false, mvpMatrix, 0);
        GlUtil.checkGlError("glUniformMatrix4fv");

        // Copy the texture transformation matrix over.
        GLES20.glUniformMatrix4fv(muTexMatrixLoc, 1, false, texMatrix, 0);
        GlUtil.checkGlError("glUniformMatrix4fv");

        // Enable the "aPosition" vertex attribute.
        GLES20.glEnableVertexAttribArray(maPositionLoc);
        GlUtil.checkGlError("glEnableVertexAttribArray");

        // Connect vertexBuffer to "aPosition".
        GLES20.glVertexAttribPointer(maPositionLoc, 2,
                GLES20.GL_FLOAT, false, 0, mVertexArray);
        GlUtil.checkGlError("glVertexAttribPointer");

        // Enable the "aTextureCoord" vertex attribute.
        GLES20.glEnableVertexAttribArray(maTextureCoordLoc);
        GlUtil.checkGlError("glEnableVertexAttribArray");

        // Connect texBuffer to "aTextureCoord".
        GLES20.glVertexAttribPointer(maTextureCoordLoc, 2,
                GLES20.GL_FLOAT, false, 0, mTexCoordArray);
        GlUtil.checkGlError("glVertexAttribPointer");

        // Draw the rect.
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
        GlUtil.checkGlError("glDrawArrays");

        GLES20.glDisableVertexAttribArray(maPositionLoc);
        GLES20.glDisableVertexAttribArray(maTextureCoordLoc);
        GLES20.glBindTexture(textureType, 0);


        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glUseProgram(0);

        return mTargetTexture[0];
    }

    private void bindFramebuffer(int width, int height) {
        GLES20.glGenFramebuffers(1, mFramebuffer, 0);
        GlUtil.checkGlError("glGenFramebuffers");

        GLES20.glGenTextures(1, mTargetTexture, 0);
        GlUtil.checkGlError("glGenTextures");

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTargetTexture[0]);
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

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, mFramebuffer[0]);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER,
                GLES20.GL_COLOR_ATTACHMENT0,
                GLES20.GL_TEXTURE_2D,
                mTargetTexture[0], 0);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
    }


    private void deleteFramebuffer() {
        if (mTargetTexture[0] != 0) {
            GLES20.glDeleteTextures(1, mTargetTexture, 0);
            mTargetTexture[0] = 0;
        }

        if (mFramebuffer[0] != 0) {
            GLES20.glDeleteFramebuffers(1, mFramebuffer, 0);
            mFramebuffer[0] = 0;
        }
    }

    void destroyProgram() {
        deleteFramebuffer();

        if (mProgramHandle != 0) {
            GLES20.glDeleteProgram(mProgramHandle);
        }
    }

}
