package io.agora.api.example.examples.advanced.customvideo;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.Matrix;

import java.io.IOException;
import java.nio.FloatBuffer;

import io.agora.api.example.R;
import io.agora.api.example.common.model.Peer;
import io.agora.rtc.gl.GlUtil;

/**
 * Created by wyylling@gmail.com on 03/01/2018.
 */
public class PeerRenderer {
    private static final String TAG = PeerRenderer.class.getSimpleName();


    private static final int COORDS_PER_VERTEX = 3;

    private int mProgram;
    private int[] mTextures = new int[1];

    // Shader location: object attributes.
    private int mPositionAttribute;
    private int mTexCoordAttribute;
    //private int mTextureLocation;
    private int mModelViewProjectionUniform;

    // Temporary matrices allocated here to reduce number of allocations for each frame.
    private float[] mModelMatrix = new float[16];
    private float[] mModelViewMatrix = new float[16];
    private float[] mModelViewProjectionMatrix = new float[16];

    // Vertex coordinates in Normalized Device Coordinates, i.e. (-1, -1) is bottom-left and (1, 1) is
    // top-right.
    private static final FloatBuffer FULL_RECTANGLE_BUF = GlUtil.createFloatBuffer(new float[] {
            -0.16f, -0.16f, // Bottom left.
            0.16f, -0.16f, // Bottom right.
            -0.16f, 0.16f, // Top left.
            0.16f, 0.16f, // Top right.
    });

    // Texture coordinates - (0, 0) is bottom-left and (1, 1) is top-right.
    private static final FloatBuffer FULL_RECTANGLE_TEX_BUF = GlUtil.createFloatBuffer(new float[] {
            0.0f, 1.0f, // Top left.
            1.0f, 1.0f, // Top right.
            0.0f, 0.0f, // Bottom left.
            1.0f, 0.0f, // Bottom right.
    });

    public PeerRenderer() {
    }

    /**
     * Creates and initializes OpenGL resources needed for rendering the model.
     *
     * @param context Context for loading the shader and below-named model and texture assets.
     */
    public void createOnGlThread(Context context) throws IOException {;
        GLES20.glGenTextures(mTextures.length, mTextures, 0);
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextures[0]);

        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);

        ShaderUtil.checkGLError(TAG, "Texture loading");


        final int vertexShader = ShaderUtil.loadGLShader(TAG, context,
                GLES20.GL_VERTEX_SHADER, R.raw.peer_vertex);
        final int fragmentShader = ShaderUtil.loadGLShader(TAG, context,
                GLES20.GL_FRAGMENT_SHADER, R.raw.peer_fragment);

        mProgram = GLES20.glCreateProgram();
        GLES20.glAttachShader(mProgram, vertexShader);
        GLES20.glAttachShader(mProgram, fragmentShader);
        GLES20.glLinkProgram(mProgram);
        GLES20.glUseProgram(mProgram);

        ShaderUtil.checkGLError(TAG, "Program creation");

        mModelViewProjectionUniform = GLES20.glGetUniformLocation(mProgram, "u_ModelViewProjection");
        //mTextureLocation = GLES20.glGetUniformLocation(mProgram, "rgb_tex");
        //GLES20.glUniform1i(shader.glShader.getUniformLocation("rgb_tex"), 0);

        //mModelViewUniform = GLES20.glGetUniformLocation(mProgram, "u_ModelView");
        //mModelViewProjectionUniform = GLES20.glGetUniformLocation(mProgram, "u_ModelViewProjection");

        mPositionAttribute = GLES20.glGetAttribLocation(mProgram, "a_Position");
        mTexCoordAttribute = GLES20.glGetAttribLocation(mProgram, "a_TexCoord");

        ShaderUtil.checkGLError(TAG, "Program parameters");

        Matrix.setIdentityM(mModelMatrix, 0);
    }

    /**
     * Updates the object model matrix and applies scaling.
     *
     * @param modelMatrix A 4x4 model-to-world transformation matrix, stored in column-major order.
     * @param scaleFactor A separate scaling factor to apply before the {@code modelMatrix}.
     * @see Matrix
     */
    public void updateModelMatrix(float[] modelMatrix, float scaleFactor) {
        float[] scaleMatrix = new float[16];
        Matrix.setIdentityM(scaleMatrix, 0);
        scaleMatrix[0] = scaleFactor;
        scaleMatrix[5] = scaleFactor;
        scaleMatrix[10] = scaleFactor;
        Matrix.multiplyMM(mModelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);
    }

    /**
     * Draws the model.
     *
     * @param cameraView  A 4x4 view matrix, in column-major order.
     * @param cameraPerspective  A 4x4 projection matrix, in column-major order.
     * @see #updateModelMatrix(float[], float)
     * @see Matrix
     */
    public void draw(float[] cameraView, float[] cameraPerspective, Peer peer) {

        ShaderUtil.checkGLError(TAG, "Before draw");

        // Build the ModelView and ModelViewProjection matrices
        // for calculating object position and light.
        Matrix.multiplyMM(mModelViewMatrix, 0, cameraView, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mModelViewProjectionMatrix, 0, cameraPerspective, 0, mModelViewMatrix, 0);

        GLES20.glUseProgram(mProgram);

        GLES20.glUniformMatrix4fv(
                mModelViewProjectionUniform, 1, false, mModelViewProjectionMatrix, 0);
        //GLES20.glUniform1i(mTextureLocation, 0);

        // Attach the object texture.
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextures[0]);

        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, peer.width,
                peer.height, 0, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, peer.data);

        ShaderUtil.checkGLError(TAG, "upload remote peer data");

        GLES20.glVertexAttribPointer(
            mPositionAttribute, 2, GLES20.GL_FLOAT, false, 0, FULL_RECTANGLE_BUF);
        GLES20.glVertexAttribPointer(
            mTexCoordAttribute, 2, GLES20.GL_FLOAT, false, 0, FULL_RECTANGLE_TEX_BUF);

        // Enable vertex arrays
        GLES20.glEnableVertexAttribArray(mPositionAttribute);
        GLES20.glEnableVertexAttribArray(mTexCoordAttribute);

        drawRectangle(0, 0, 512, 512);

        // Disable vertex arrays
        GLES20.glDisableVertexAttribArray(mPositionAttribute);
        GLES20.glDisableVertexAttribArray(mTexCoordAttribute);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);

        ShaderUtil.checkGLError(TAG, "After draw");
    }

    private void drawRectangle(int x, int y, int width, int height) {
        // Draw quad.
        //GLES20.glViewport(x, y, width, height);
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
    }
}
