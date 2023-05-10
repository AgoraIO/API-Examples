package com.sensetime.effects;

import android.content.Context;
import android.opengl.GLES20;

import com.ar.effects.BuildConfig;
import com.ar.effects.EffectsImage;
import com.ar.effects.EffectsParams;
import com.ar.effects.EffectsRender;
import com.ar.effects.RenderInputParams;
import com.ar.effects.RenderOutputParams;
import com.ar.effects.ResultCode;
import com.sensetime.effects.display.STGLRender;
import com.sensetime.effects.glutils.GlUtil;
import com.sensetime.effects.utils.Accelerometer;
import com.sensetime.effects.utils.Constants;
import com.sensetime.effects.utils.FileUtils;
import com.sensetime.effects.utils.LogUtils;

import java.util.List;

public class STRender {
    private final String TAG = this.getClass().getSimpleName();

    public static final int ERROR_AUTHORIZE_FAILURE = -1;
    public static final int ERROR_RELEASED = -2;
    public static final int ERROR_TEXTURE_INVALID = -3;

    private final Accelerometer mAccelerometer;
    private EffectsRender mEffectsRender = new EffectsRender();
    private boolean authorized = false;
    private STGLRender mGLRenderBefore;
    private STGLRender mGLRenderAfter;
    private byte[] mImageData;
    private int[] mTextureOutId;
    private int mImageWidth, mImageHeight;
    private volatile boolean isReleased = false;

    public STRender(Context context){
        // 证书认证
        List<String> licenses = FileUtils.getLicenseFilesFromAssets(context, "license");
        authorized = licenses.size() > 0 && EffectsRender.authorize(context, licenses.get(0));
        // 传感器
        mAccelerometer = new Accelerometer(context);
        mAccelerometer.start();
        // init effetes render
        mEffectsRender.createHandle(context, EffectsParams.PreviewMode.MODE_PREVIEW.ordinal(), false);
        mEffectsRender.loadModelsFromAssetsFile(context, "models", context.getAssets());
        if (BuildConfig.DEBUG) {
            mEffectsRender.enableLog(Constants.EFFECTS_LOG_SWITCH);//打开耗时log
        }
    }

    public void release() {
        mAccelerometer.stop();
        mEffectsRender.destroyHandle();
        isReleased = true;
        authorized = false;
        if(mGLRenderAfter != null){
            mGLRenderAfter.destroyPrograms();
            mGLRenderAfter = null;
        }
        if(mGLRenderBefore != null){
            mGLRenderBefore.destroyPrograms();
            mGLRenderBefore = null;
        }
    }

    /**
     *
     * @param imageBuffer buffer data
     * @param pixelFormat {@link EffectsParams.PixelFormat#PIX_FMT_NV21}
     * @param width image width
     * @param height image width
     * @param isFrontCamera is front camera
     * @return out texture id
     */
    public int preProcess(byte[] imageBuffer, int pixelFormat, int width, int height, boolean isFrontCamera){
        if(!authorized){
            LogUtils.e(TAG, "pre process error : EffectsRender authorize failed!");
            return ERROR_AUTHORIZE_FAILURE;
        }
        if(isReleased){
            LogUtils.e(TAG, "pre process error : STRender has been released!");
            return ERROR_RELEASED;
        }

        int textureId = -1;

        boolean sizeChanged = mImageWidth != height || mImageHeight != width;
        if (sizeChanged) {
            mImageWidth = height;
            mImageHeight = width;
        }
        if (mImageData == null || sizeChanged) {
            mImageData = new byte[imageBuffer.length];
        }
        if (mTextureOutId == null) {
            mTextureOutId = new int[1];
            GlUtil.initEffectTexture(mImageWidth, mImageHeight, mTextureOutId, GLES20.GL_TEXTURE_2D);
        } else if (sizeChanged) {
            GLES20.glDeleteTextures(mTextureOutId.length, mTextureOutId, 0);
            GlUtil.initEffectTexture(mImageWidth, mImageHeight, mTextureOutId, GLES20.GL_TEXTURE_2D);
        }

        System.arraycopy(imageBuffer, 0, mImageData, 0, imageBuffer.length);
        EffectsImage inputImage = new EffectsImage(mImageData, pixelFormat, width, height);
        RenderInputParams inputParams = new RenderInputParams(inputImage, -1,
                getEffectsDetectOrientation(isFrontCamera),
                getEffectsRotate(isFrontCamera),
                isFrontCamera,
                false,
                0);
        RenderOutputParams outputParams = new RenderOutputParams(mTextureOutId[0], null);

        int ret = mEffectsRender.render(inputParams, outputParams);

        if(ret == ResultCode.RESULT_OK.ordinal()){
            textureId = outputParams.getTextureId();
            if(mGLRenderAfter == null){
                mGLRenderAfter = new STGLRender(GLES20.GL_TEXTURE_2D);
            }
            mGLRenderAfter.adjustRenderSize(mImageWidth, mImageHeight, 0, true, false);
            textureId = mGLRenderAfter.process(textureId, STGLRender.IDENTITY_MATRIX);
        }

        if (textureId < 0) {
            LogUtils.e(TAG, "pre process error : outTextureId < 0");
            return ERROR_TEXTURE_INVALID;
        }


        return textureId;
    }

    public int preProcess(int textureId, int textureType, int width, int height, boolean isFrontCamera) {
        if(!authorized){
            LogUtils.e(TAG, "pre process error : EffectsRender authorize failed!");
            return -1;
        }
        if(isReleased){
            LogUtils.e(TAG, "pre process error : STRender has been released!");
            return -2;
        }

        int outTextureId = -1;

        boolean sizeChanged = mImageWidth != height || mImageHeight != width ;
        if (sizeChanged) {
            mImageWidth = height;
            mImageHeight = width;
        }

        if (mTextureOutId == null) {
            mTextureOutId = new int[1];
            GlUtil.initEffectTexture(mImageWidth, mImageHeight, mTextureOutId, GLES20.GL_TEXTURE_2D);
        } else if (sizeChanged) {
            GLES20.glDeleteTextures(mTextureOutId.length, mTextureOutId, 0);
            GlUtil.initEffectTexture(mImageWidth, mImageHeight, mTextureOutId, GLES20.GL_TEXTURE_2D);
        }

        if(mGLRenderBefore == null){
            mGLRenderBefore = new STGLRender(textureType);
        }
        mGLRenderBefore.adjustRenderSize(mImageWidth, mImageHeight, getCameraOrientation(isFrontCamera), isFrontCamera, false);
        textureId = mGLRenderBefore.process(textureId, STGLRender.IDENTITY_MATRIX);

        EffectsImage inputImage = new EffectsImage(null, EffectsParams.PixelFormat.PIX_FMT_RGBA8888.ordinal(), mImageWidth, mImageHeight);
        RenderInputParams inputParams = new RenderInputParams(
                inputImage,
                textureId,
                getCurrentOrientation(),
                EffectsParams.RotateType.ROTATE_0.ordinal(),
                false,
                false, 0);

        RenderOutputParams outputParams = new RenderOutputParams(mTextureOutId[0], null);

        int ret = mEffectsRender.render(inputParams, outputParams);

        if (ret == ResultCode.RESULT_OK.ordinal()) {
            outTextureId = outputParams.getTextureId();
            if(mGLRenderAfter == null){
                mGLRenderAfter = new STGLRender(GLES20.GL_TEXTURE_2D);
            }
            mGLRenderAfter.adjustRenderSize(mImageWidth, mImageHeight, 0, true, false);
            outTextureId = mGLRenderAfter.process(outTextureId, STGLRender.IDENTITY_MATRIX);
        }

        if(outTextureId < 0){
            LogUtils.e(TAG, "pre process error : outTextureId < 0");
            return ERROR_TEXTURE_INVALID;
        }

        return outTextureId;
    }

    private int getCurrentOrientation() {
        int dir = Accelerometer.getDirection();
        int orientation = dir - 1;
        if (orientation < 0) {
            orientation = dir ^ 3;
        }

        return orientation;
    }

    private int getCameraOrientation(boolean isFrontCamera){
        if(isFrontCamera){
            return 270;
        }else{
            return 90;
        }
    }

    private int getEffectsDetectOrientation(boolean isFrontCamera){
        int dir = Accelerometer.getDirection();

        if(isFrontCamera && (dir == EffectsParams.RotateType.ROTATE_90.ordinal() || dir == EffectsParams.RotateType.ROTATE_270.ordinal())){
            return 4 - dir;
        }

        return dir;
    }

    private int getEffectsRotate(boolean isFrontCamera){
        int orientation = EffectsParams.RotateType.ROTATE_90.ordinal();
        if(isFrontCamera){
            return orientation;
        }

        return EffectsParams.RotateType.ROTATE_270.ordinal();
    }

    public EffectsRender getEffectsRender(){
        return mEffectsRender;
    }
}
