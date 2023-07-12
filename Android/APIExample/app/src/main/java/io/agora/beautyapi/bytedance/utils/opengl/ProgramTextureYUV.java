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

import android.opengl.GLES20;

import java.nio.ByteBuffer;

/**
 * Created on 2021/7/23 17:35
 */
public class ProgramTextureYUV extends Program {

    private int muMVPMatrixLoc;
    private int maPositionLoc;
    private int maTextureCoordLoc;
    private int mYTextureLoc;
    private int mUTextureLoc;
    private int mVTextureLoc;
    private int mVUTextureLoc;

    public ProgramTextureYUV() {
        super(VERTEX, FRAGMENT);
    }

    @Override
    protected Drawable2d getDrawable2d() {
        return new Drawable2d(Drawable2d.Prefab.FULL_RECTANGLE);
    }

    @Override
    protected void getLocations() {
        maPositionLoc = GLES20.glGetAttribLocation(mProgramHandle, "aPosition");
        GlUtil.checkLocation(maPositionLoc, "aPosition");
        maTextureCoordLoc = GLES20.glGetAttribLocation(mProgramHandle, "aTextureCoord");
        GlUtil.checkLocation(maTextureCoordLoc, "aTextureCoord");
        muMVPMatrixLoc = GLES20.glGetUniformLocation(mProgramHandle, "uMVPMatrix");
        GlUtil.checkLocation(muMVPMatrixLoc, "uMVPMatrix");
        mYTextureLoc = GLES20.glGetUniformLocation(mProgramHandle, "yTexture");
        GlUtil.checkLocation(muMVPMatrixLoc, "yTexture");
        mUTextureLoc = GLES20.glGetUniformLocation(mProgramHandle, "uTexture");
        GlUtil.checkLocation(muMVPMatrixLoc, "uTexture");
        mVTextureLoc = GLES20.glGetUniformLocation(mProgramHandle, "vTexture");
        GlUtil.checkLocation(muMVPMatrixLoc, "vTexture");

        mVUTextureLoc = GLES20.glGetUniformLocation(mProgramHandle, "vuTexture");
        GlUtil.checkLocation(muMVPMatrixLoc, "vuTexture");
    }

    public int drawFrameOffScreen(int yTexture, int uTexture, int vTexture, int width, int height, float[] mvpMatrix) {
        GlUtil.checkGlError("draw start");

        initFrameBufferIfNeed(width, height);
        GlUtil.checkGlError("initFrameBufferIfNeed");

        // Select the program.
        GLES20.glUseProgram(mProgramHandle);
        GlUtil.checkGlError("glUseProgram");

        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, yTexture);
        GlUtil.checkGlError("glBindTexture");

        GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, uTexture);
        GlUtil.checkGlError("glBindTexture");

        GLES20.glActiveTexture(GLES20.GL_TEXTURE2);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, vTexture);
        GlUtil.checkGlError("glBindTexture");

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, mFrameBuffers[0]);

        GLES20.glUniformMatrix4fv(muMVPMatrixLoc, 1, false, mvpMatrix, 0);

        GLES20.glEnableVertexAttribArray(maPositionLoc);
        GLES20.glVertexAttribPointer(maPositionLoc, Drawable2d.COORDS_PER_VERTEX,
                GLES20.GL_FLOAT, false, Drawable2d.VERTEXTURE_STRIDE, mDrawable2d.getVertexArray());

        GLES20.glEnableVertexAttribArray(maTextureCoordLoc);
        GLES20.glVertexAttribPointer(maTextureCoordLoc, 2, GLES20.GL_FLOAT,
                false, Drawable2d.TEXTURE_COORD_STRIDE, mDrawable2d.getTexCoorArrayFB());

        GLES20.glUniform1i(mYTextureLoc, 0);
        GLES20.glUniform1i(mUTextureLoc, 1);
        GLES20.glUniform1i(mVTextureLoc, 2);

        GLES20.glViewport(0, 0, width, height);
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, mDrawable2d.getVertexCount());

        GLES20.glDisableVertexAttribArray(maPositionLoc);
        GLES20.glDisableVertexAttribArray(maTextureCoordLoc);
        GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glActiveTexture(GLES20.GL_TEXTURE2);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glUseProgram(0);

        return mFrameBufferTextures[0];
    }

    public int drawFrameOffScreen(int yTexture, int vuTexture, int width, int height, float[] mvpMatrix) {
        GlUtil.checkGlError("draw start");

        initFrameBufferIfNeed(width, height);
        GlUtil.checkGlError("initFrameBufferIfNeed");

        // Select the program.
        GLES20.glUseProgram(mProgramHandle);
        GlUtil.checkGlError("glUseProgram");

        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, yTexture);
        GlUtil.checkGlError("glBindTexture");

        GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, vuTexture);
        GlUtil.checkGlError("glBindTexture");

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, mFrameBuffers[0]);

        GLES20.glUniformMatrix4fv(muMVPMatrixLoc, 1, false, mvpMatrix, 0);

        GLES20.glEnableVertexAttribArray(maPositionLoc);
        GLES20.glVertexAttribPointer(maPositionLoc, Drawable2d.COORDS_PER_VERTEX,
                GLES20.GL_FLOAT, false, Drawable2d.VERTEXTURE_STRIDE, mDrawable2d.getVertexArray());

        GLES20.glEnableVertexAttribArray(maTextureCoordLoc);
        GLES20.glVertexAttribPointer(maTextureCoordLoc, 2, GLES20.GL_FLOAT,
                false, Drawable2d.TEXTURE_COORD_STRIDE, mDrawable2d.getTexCoorArrayFB());

        GLES20.glUniform1i(mYTextureLoc, 0);
        GLES20.glUniform1i(mVUTextureLoc, 1);

        GLES20.glViewport(0, 0, width, height);
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, mDrawable2d.getVertexCount());

        GLES20.glDisableVertexAttribArray(maPositionLoc);
        GLES20.glDisableVertexAttribArray(maTextureCoordLoc);
        GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glUseProgram(0);

        return mFrameBufferTextures[0];
    }


    @Override
    public void drawFrameOnScreen(int textureId, int width, int height, float[] mvpMatrix) {

    }

    @Override
    public int drawFrameOffScreen(int textureId, int width, int height, float[] mvpMatrix) {
        return 0;
    }

    @Override
    public ByteBuffer readBuffer(int textureId, int width, int height) {
        return null;
    }

    public static final String VERTEX = "uniform mat4 uMVPMatrix;\n" +
            "attribute vec4 aPosition;\n" +
            "attribute vec2 aTextureCoord;\n" +
            "varying vec2 vTextureCoord;\n" +
            "void main() {\n" +
            "    gl_Position = uMVPMatrix * aPosition;\n" +
            "    vTextureCoord = aTextureCoord;\n" +
            "}\n";
    public static final String FRAGMENT = "varying highp vec2 vTextureCoord;\n" +
            " uniform sampler2D yTexture;\n" +
            " uniform sampler2D vuTexture;\n" +
            " uniform sampler2D uTexture;\n" +
            " uniform sampler2D vTexture;\n" +
            " void main()\n" +
            " {\n" +
            "     mediump vec3 yuv;\n" +
            "     lowp vec3 rgb;\n" +
            "     yuv.x = texture2D(yTexture, vTextureCoord).a - 0.065;\n" +
            "     yuv.y = texture2D(vuTexture, vTextureCoord).a - 0.5;\n" +
            "     yuv.z = texture2D(vuTexture, vTextureCoord).r - 0.5;\n" +
//            "     rgb = mat3(      1,       1,       1,\n" +
//            "                0, -.21482, 2.12798,\n" +
//            "                1.28033, -.38059,       0) * yuv;\n" +
            "     rgb.x = yuv.x + 1.4075 * yuv.z;\n" +
            "     rgb.y = yuv.x - 0.3455 * yuv.y - 0.7169 * yuv.z;\n" +
            "     rgb.z = yuv.x + 1.779 * yuv.y;\n" +
//            "     gl_FragColor = vec4(rgb.x, rgb.y, rgb.z, 1);\n" +
            "     gl_FragColor = vec4(rgb.x, rgb.y, rgb.z, 1);\n" +
            " }";
}
