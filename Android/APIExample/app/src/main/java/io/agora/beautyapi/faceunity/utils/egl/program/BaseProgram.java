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

package io.agora.beautyapi.faceunity.utils.egl.program;

import android.opengl.GLES20;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import io.agora.beautyapi.faceunity.utils.egl.GLUtils;

public abstract class BaseProgram {

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

    private static final float VERTEX_POSITION[] = {
            -1.0f, -1.0f,   // 0 bottom left
            1.0f, -1.0f,   // 1 bottom right
            -1.0f, 1.0f,   // 2 top left
            1.0f, 1.0f,   // 3 top right
    };
    private static final float TEXTURE_COORD[] = {
            0.0f, 0.0f,     // 2 bottom left
            1.0f, 0.0f,      // 3 bottom right
            0.0f, 1.0f,     // 0 top left
            1.0f, 1.0f,     // 1 top right
    };

    protected int mProgramHandle;

    protected FloatBuffer mVertexArray;

    protected FloatBuffer mTexCoordArray;

    // locations in the shader mProgram
    protected int muMVPMatrixLoc;
    protected int muTexMatrixLoc;
    protected int maPositionLoc;
    protected int maTextureCoordLoc;

    public BaseProgram(String fragmentSource){
        mProgramHandle = GLUtils.createProgram(VERTEX_SHADER, fragmentSource);
        muMVPMatrixLoc = GLES20.glGetUniformLocation(mProgramHandle, "uMVPMatrix");
        muTexMatrixLoc = GLES20.glGetUniformLocation(mProgramHandle, "uTexMatrix");
        maPositionLoc = GLES20.glGetAttribLocation(mProgramHandle, "aPosition");
        maTextureCoordLoc = GLES20.glGetAttribLocation(mProgramHandle, "aTextureCoord");
        initBuffers();
    }

    public abstract void draw(int textureId, float[] texMatrix, float[] mvpMatrix);

    public void release(){
        if (mProgramHandle != 0) {
            GLES20.glDeleteProgram(mProgramHandle);
        }
    }

    protected void drawReal(int textureId, int textureType, float[] texMatrix, float[] mvpMatrix){
        // Select the program.
        GLES20.glUseProgram(mProgramHandle);
        GLUtils.checkGlError("glUseProgram");

        // Set the texture.
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(textureType, textureId);

        // Copy the model / view / projection matrix over.
        GLES20.glUniformMatrix4fv(muMVPMatrixLoc, 1, false, mvpMatrix, 0);
        GLUtils.checkGlError("glUniformMatrix4fv");

        // Copy the texture transformation matrix over.
        GLES20.glUniformMatrix4fv(muTexMatrixLoc, 1, false, texMatrix, 0);
        GLUtils.checkGlError("glUniformMatrix4fv");

        // Enable the "aPosition" vertex attribute.
        GLES20.glEnableVertexAttribArray(maPositionLoc);
        GLUtils.checkGlError("glEnableVertexAttribArray");

        // Connect vertexBuffer to "aPosition".
        GLES20.glVertexAttribPointer(maPositionLoc, 2,
                GLES20.GL_FLOAT, false, 0, mVertexArray);
        GLUtils.checkGlError("glVertexAttribPointer");

        // Enable the "aTextureCoord" vertex attribute.
        GLES20.glEnableVertexAttribArray(maTextureCoordLoc);
        GLUtils.checkGlError("glEnableVertexAttribArray");

        // Connect texBuffer to "aTextureCoord".
        GLES20.glVertexAttribPointer(maTextureCoordLoc, 2,
                GLES20.GL_FLOAT, false, 0, mTexCoordArray);
        GLUtils.checkGlError("glVertexAttribPointer");

        // Draw the rect.
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
        GLUtils.checkGlError("glDrawArrays");

        GLES20.glDisableVertexAttribArray(maPositionLoc);
        GLES20.glDisableVertexAttribArray(maTextureCoordLoc);
        GLES20.glBindTexture(textureType, 0);

        GLES20.glUseProgram(0);
    }

    private void initBuffers(){
        mVertexArray = ByteBuffer.allocateDirect(VERTEX_POSITION.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer();
        mVertexArray.put(VERTEX_POSITION).position(0);

        mTexCoordArray = ByteBuffer.allocateDirect(TEXTURE_COORD.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer();
        mTexCoordArray.put(TEXTURE_COORD).position(0);
    }
}
