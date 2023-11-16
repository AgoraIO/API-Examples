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

package io.agora.beautyapi.bytedance.utils;

import static android.opengl.GLES20.GL_RGBA;

import android.graphics.Bitmap;
import android.graphics.Point;
import android.opengl.GLES20;
import android.opengl.Matrix;
import android.util.Log;
import android.widget.ImageView;

import com.bytedance.labcv.effectsdk.BytedEffectConstants;

import java.nio.ByteBuffer;

import io.agora.beautyapi.bytedance.utils.opengl.GlUtil;
import io.agora.beautyapi.bytedance.utils.opengl.ProgramManager;
import io.agora.beautyapi.bytedance.utils.opengl.ProgramTextureYUV;

/**
 * Created on 5/8/21 11:58 AM
 */
public class ImageUtil {
    private static final String TAG = "ImageUtil";

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

    private ProgramManager mProgramManager;


    /**
     * {en}
     * Default constructor
     * <p>
     * {zh}
     * 默认构造函数
     */
    public ImageUtil() {
    }

    /**
     * {zh}
     * 准备帧缓冲区纹理对象
     *
     * @param width  纹理宽度
     * @param height 纹理高度
     * @return 纹理ID int <p> {en} Prepare frame buffer texture object
     */
    public int prepareTexture(int width, int height) {
        initFrameBufferIfNeed(width, height);
        return mFrameBufferTextures[0];
    }

    /**
     * {zh}
     * 默认的离屏渲染绑定的纹理
     *
     * @return 纹理id output texture <p> {en} Default off-screen rendering bound texture
     */
    public int getOutputTexture() {
        if (mFrameBufferTextures == null) {
            return GlUtil.NO_TEXTURE;
        }
        return mFrameBufferTextures[0];
    }

    /**
     * {zh}
     * 初始化帧缓冲区
     *
     * @param width  缓冲的纹理宽度
     * @param height 缓冲的纹理高度
     */
    private void initFrameBufferIfNeed(int width, int height) {
        boolean need = false;
        if (null == mFrameBufferShape || mFrameBufferShape.x != width || mFrameBufferShape.y != height) {
            need = true;
        }
        if (mFrameBuffers == null || mFrameBufferTextures == null) {
            need = true;
        }
        if (need) {
            destroyFrameBuffers();
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

    /**
     * {zh}
     * 销毁帧缓冲区对象
     */
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
     * {zh}
     * 释放资源，包括帧缓冲区及Program对象
     * <p>
     * {en}
     * Free resources, including frame buffers and Program objects
     * {zh}
     * 释放资源，包括帧缓冲区及Program对象
     */
    public void release() {
        destroyFrameBuffers();
        if (null != mProgramManager) {
            mProgramManager.release();
        }
        if (mYUVProgram != null) {
            mYUVProgram.release();
            mYUVProgram = null;
        }
    }

    /**
     * {zh}
     * 读取渲染结果的buffer
     *
     * @param imageWidth  图像宽度
     * @param imageHeight 图像高度
     * @return 渲染结果的像素Buffer 格式RGBA <p> {en} Read the buffer
     */
    public ByteBuffer captureRenderResult(int imageWidth, int imageHeight) {
        if (mFrameBufferTextures == null) {
            return null;
        }
        int textureId = mFrameBufferTextures[0];
        if (null == mFrameBufferTextures || textureId == GlUtil.NO_TEXTURE) {
            return null;
        }
        if (imageWidth * imageHeight == 0) {
            return null;
        }
        ByteBuffer mCaptureBuffer = ByteBuffer.allocateDirect(imageWidth * imageHeight * 4);

        mCaptureBuffer.position(0);
        int[] frameBuffer = new int[1];
        GLES20.glGenFramebuffers(1, frameBuffer, 0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, frameBuffer[0]);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0,
                GLES20.GL_TEXTURE_2D, textureId, 0);
        GLES20.glReadPixels(0, 0, imageWidth, imageHeight,
                GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, mCaptureBuffer);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        if (null != frameBuffer) {
            GLES20.glDeleteFramebuffers(1, frameBuffer, 0);
        }
        return mCaptureBuffer;
    }

    /**
     * {zh}
     * 读取渲染结果的buffer
     *
     * @param textureId   the texture id
     * @param imageWidth  图像宽度
     * @param imageHeight 图像高度
     * @return 渲染结果的像素Buffer 格式RGBA <p> {en} Read the buffer
     */
    public ByteBuffer captureRenderResult(int textureId, int imageWidth, int imageHeight) {
        if (textureId == GlUtil.NO_TEXTURE) {
            return null;
        }
        if (imageWidth * imageHeight == 0) {
            return null;
        }
        ByteBuffer mCaptureBuffer = ByteBuffer.allocateDirect(imageWidth * imageHeight * 4);

        mCaptureBuffer.position(0);
        int[] frameBuffer = new int[1];
        GLES20.glGenFramebuffers(1, frameBuffer, 0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, frameBuffer[0]);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0,
                GLES20.GL_TEXTURE_2D, textureId, 0);
        GLES20.glReadPixels(0, 0, imageWidth, imageHeight,
                GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, mCaptureBuffer);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        if (null != frameBuffer) {
            GLES20.glDeleteFramebuffers(1, frameBuffer, 0);
        }
        return mCaptureBuffer;
    }

    /**
     * {zh}
     * 纹理拷贝
     *
     * @param srcTexture the src texture
     * @param dstTexture the dst texture
     * @param width      the width
     * @param height     the height
     * @return boolean <p> {en} Texture copy
     */
    public boolean copyTexture(int srcTexture, int dstTexture, int width, int height) {
        if (srcTexture == GlUtil.NO_TEXTURE || dstTexture == GlUtil.NO_TEXTURE) {
            return false;
        }
        if (width * height == 0) {
            return false;
        }
        int[] frameBuffer = new int[1];
        GLES20.glGenFramebuffers(1, frameBuffer, 0);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, frameBuffer[0]);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0,
                GLES20.GL_TEXTURE_2D, srcTexture, 0);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, dstTexture);
        GLES20.glCopyTexImage2D(GLES20.GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        if (null != frameBuffer) {
            GLES20.glDeleteFramebuffers(1, frameBuffer, 0);
        }
        int error = GLES20.glGetError();
        if (error != GLES20.GL_NO_ERROR) {
            String msg = "copyTexture glError 0x" + Integer.toHexString(error);
            Log.e(TAG, msg);
            return false;
        }
        return true;
    }


    /**
     * {zh}
     *
     * @param inputTexture        输入纹理
     * @param inputTextureFormat  输入纹理格式，2D/OES
     * @param outputTextureFormat 输出纹理格式，2D/OES
     * @param width               输入纹理的宽
     * @param height              输入纹理的高
     * @param transition          纹理变换方式
     * @return 输出纹理 int
     */
    public int transferTextureToTexture(int inputTexture, BytedEffectConstants.TextureFormat inputTextureFormat,
                                        BytedEffectConstants.TextureFormat outputTextureFormat,
                                        int width, int height, Transition transition) {
        if (outputTextureFormat != BytedEffectConstants.TextureFormat.Texure2D) {
            LogUtils.e(TAG, "the inputTexture is not supported,please use Texure2D as output texture format");
            return GlUtil.NO_TEXTURE;
        }
        if (null == mProgramManager) {
            mProgramManager = new ProgramManager();
        }

        boolean targetRoated = transition.getAngle() % 180 == 90;
        return mProgramManager.getProgram(inputTextureFormat).drawFrameOffScreen(inputTexture, targetRoated ? height : width, targetRoated ? width : height, transition.getMatrix());

    }

    private ProgramTextureYUV mYUVProgram;

    /**
     * Transfer yuv to texture int.
     *
     * @param yBuffer    the y buffer
     * @param vuBuffer   the vu buffer
     * @param width      the width
     * @param height     the height
     * @param transition the transition
     * @return the int
     */
    public int transferYUVToTexture(ByteBuffer yBuffer, ByteBuffer vuBuffer, int width, int height, Transition transition) {
        if (mYUVProgram == null) {
            mYUVProgram = new ProgramTextureYUV();
        }

        int yTexture = GlUtil.createImageTexture(yBuffer, width, height, GLES20.GL_ALPHA);
        int vuTexture = GlUtil.createImageTexture(vuBuffer, width / 2, height / 2, GLES20.GL_LUMINANCE_ALPHA);
        int rgbaTexture = mYUVProgram.drawFrameOffScreen(yTexture, vuTexture, width, height, transition.getMatrix());
        GlUtil.deleteTextureId(new int[]{yTexture, vuTexture});

        return rgbaTexture;
    }

    /**
     * {zh}
     *
     * @param texture            纹理
     * @param inputTextureFormat 纹理格式，2D/OES
     * @param outputFormat       输出 buffer 格式
     * @param width              宽
     * @param height             高
     * @param ratio              the ratio
     * @return 输出 buffer
     */
    public ByteBuffer transferTextureToBuffer(int texture, BytedEffectConstants.TextureFormat inputTextureFormat,
                                              BytedEffectConstants.PixlFormat outputFormat, int width, int height, float ratio) {
        if (outputFormat != BytedEffectConstants.PixlFormat.RGBA8888) {
            LogUtils.e(TAG, "the outputFormat is not supported,please use RGBA8888 as output texture format");
            return null;
        }
        if (null == mProgramManager) {
            mProgramManager = new ProgramManager();
        }
        return mProgramManager.getProgram(inputTextureFormat).readBuffer(texture, (int) (width * ratio), (int) (height * ratio));


    }

    /**
     * Transfer texture to bitmap bitmap.
     *
     * @param texture            the texture
     * @param inputTextureFormat the input texture format
     * @param width              the width
     * @param height             the height
     * @return the bitmap
     */
    public Bitmap transferTextureToBitmap(int texture, BytedEffectConstants.TextureFormat inputTextureFormat,
                                          int width, int height) {
        ByteBuffer buffer = transferTextureToBuffer(texture, inputTextureFormat, BytedEffectConstants.PixlFormat.RGBA8888,
                width, height, 1);
        if (buffer == null) {
            return null;
        }
        return transferBufferToBitmap(buffer, BytedEffectConstants.PixlFormat.RGBA8888, width, height);
    }

    /**
     * {zh}
     *
     * @param buffer       输入 buffer
     * @param inputFormat  buffer 格式
     * @param outputFormat 输出纹理格式
     * @param width        宽
     * @param height       高
     * @return 输出纹理 int
     */
    public int transferBufferToTexture(ByteBuffer buffer, BytedEffectConstants.PixlFormat inputFormat,
                                       BytedEffectConstants.TextureFormat outputFormat, int width, int height) {

        if (inputFormat != BytedEffectConstants.PixlFormat.RGBA8888) {
            LogUtils.e(TAG, "inputFormat support RGBA8888 only");
            return GlUtil.NO_TEXTURE;
        }

        if (outputFormat != BytedEffectConstants.TextureFormat.Texure2D) {
            LogUtils.e(TAG, "outputFormat support Texure2D only");
            return GlUtil.NO_TEXTURE;
        }

        return create2DTexture(buffer, width, height, GL_RGBA);


    }

    private int create2DTexture(ByteBuffer data, int width, int height, int format) {
        int[] textureHandles = new int[1];
        int textureHandle;

        GLES20.glGenTextures(1, textureHandles, 0);
        textureHandle = textureHandles[0];
        GlUtil.checkGlError("glGenTextures");

        // Bind the texture handle to the 2D texture target.
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureHandle);

        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S,
                GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T,
                GLES20.GL_CLAMP_TO_EDGE);
        // Configure min/mag filtering, i.e. what scaling method do we use if what we're rendering
        // is smaller or larger than the source image.
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER,
                GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER,
                GLES20.GL_LINEAR);
        GlUtil.checkGlError("loadImageTexture");

        // Load the data from the buffer into the texture handle.
        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, /*level*/ 0, format,
                width, height, /*border*/ 0, format, GLES20.GL_UNSIGNED_BYTE, data);
        GlUtil.checkGlError("loadImageTexture");

        return textureHandle;
    }

    /**
     * {zh}
     *
     * @param buffer       输入 buffer
     * @param inputFormat  输入 buffer 格式
     * @param outputFormat 输出 buffer 格式
     * @param width        宽
     * @param height       高
     * @return 输出 buffer
     */
    public ByteBuffer transferBufferToBuffer(ByteBuffer buffer, BytedEffectConstants.PixlFormat inputFormat,
                                             BytedEffectConstants.PixlFormat outputFormat, int width, int height) {
        return null;

    }

    /**
     * {zh}
     *
     * @param buffer 输入 buffer
     * @param format 输入 buffer 格式
     * @param width  宽
     * @param height 高
     * @return 输出 bitmap
     */
    public Bitmap transferBufferToBitmap(ByteBuffer buffer, BytedEffectConstants.PixlFormat format,
                                         int width, int height) {
        Bitmap mCameraBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);

        buffer.position(0);
        mCameraBitmap.copyPixelsFromBuffer(buffer);
        buffer.position(0);
        return mCameraBitmap;

    }

    /**
     * {zh}
     * 在屏幕上渲染纹理
     *
     * @param textureId        纹理ID
     * @param srcTetxureFormat 纹理格式
     * @param surfaceWidth     视口宽度
     * @param surfaceHeight    视口高度
     * @param mMVPMatrix       旋转矩阵                         <p>                         {en}                         Render texture on screen
     */
    public void drawFrameOnScreen(int textureId, BytedEffectConstants.TextureFormat srcTetxureFormat, int surfaceWidth, int surfaceHeight, float[] mMVPMatrix) {
        if (null == mProgramManager) {
            mProgramManager = new ProgramManager();
        }


        mProgramManager.getProgram(srcTetxureFormat).drawFrameOnScreen(textureId, surfaceWidth, surfaceHeight, mMVPMatrix);

    }

    /**
     * The type Transition.
     */
    public static class Transition {

        private float[] mMVPMatrix = new float[16];
        private int mAngle = 0;

        /**
         * Instantiates a new Transition.
         */
        public Transition() {
            Matrix.setIdentityM(mMVPMatrix, 0);
        }

        /**
         * Instantiates a new Transition.
         *
         * @param transformMatrixArray the transform matrix array
         */
        public Transition(float[] transformMatrixArray) {
            for (int i = 0; i < transformMatrixArray.length; i++) {
                mMVPMatrix[i] = transformMatrixArray[i];
            }
        }


        /**
         * {zh}
         *
         * @param x the x
         * @param y the y
         * @return the transition
         */
        public Transition flip(boolean x, boolean y) {
            GlUtil.flip(mMVPMatrix, x, y);
            return this;

        }

        /**
         * Gets angle.
         *
         * @return the angle
         */
        public int getAngle() {
            return mAngle % 360;
        }

        /**
         * {zh}
         *
         * @param angle 旋转角度，仅支持 0/90/180/270
         * @return the transition
         */
        public Transition rotate(float angle) {
            mAngle += angle;
            GlUtil.rotate(mMVPMatrix, angle);
            return this;

        }

        /**
         * Scale transition.
         *
         * @param sx the sx
         * @param sy the sy
         * @return the transition
         */
        public Transition scale(float sx, float sy) {
            GlUtil.scale(mMVPMatrix, sx, sy);
            return this;
        }


        /**
         * Crop transition.
         *
         * @param scaleType     the scale type
         * @param rotation      the rotation
         * @param textureWidth  the texture width
         * @param textureHeight the texture height
         * @param surfaceWidth  the surface width
         * @param surfaceHeight the surface height
         * @return the transition
         */
        public Transition crop(ImageView.ScaleType scaleType, int rotation, int textureWidth, int textureHeight, int surfaceWidth, int surfaceHeight) {
            if (rotation % 180 == 90) {
                GlUtil.getShowMatrix(mMVPMatrix, scaleType, textureHeight, textureWidth, surfaceWidth, surfaceHeight);
            } else {
                GlUtil.getShowMatrix(mMVPMatrix, scaleType, textureWidth, textureHeight, surfaceWidth, surfaceHeight);
            }
            return this;
        }

        /**
         * {zh}
         *
         * @return 逆向后的 transition
         */
        public Transition reverse() {
            float[] invertedMatrix = new float[16];

            if (Matrix.invertM(invertedMatrix, 0, mMVPMatrix, 0)) {
                mMVPMatrix = invertedMatrix;
            }
            return this;


        }

        /**
         * Get matrix float [ ].
         *
         * @return the float [ ]
         */
        public float[] getMatrix() {
            return mMVPMatrix;
        }

        /**
         * To String.
         *
         * @return string
         */
        public String toString() {
            StringBuilder sb = new StringBuilder();
            for (float value : mMVPMatrix) {
                sb.append(value).append("  ");
            }
            return sb.toString();

        }
    }
}
