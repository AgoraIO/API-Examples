package com.sensetime.effects;

import android.content.Context;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;
import android.util.SparseArray;

import com.sensetime.effects.display.STGLRender;
import com.sensetime.effects.glutils.GlUtil;
import com.sensetime.effects.glutils.STUtils;
import com.sensetime.effects.utils.FileUtils;
import com.sensetime.effects.utils.LogUtils;
import com.sensetime.effects.utils.STLicenseUtils;
import com.sensetime.stmobile.STCommonNative;
import com.sensetime.stmobile.STMobileAnimalNative;
import com.sensetime.stmobile.STMobileColorConvertNative;
import com.sensetime.stmobile.STMobileEffectNative;
import com.sensetime.stmobile.STMobileEffectParams;
import com.sensetime.stmobile.STMobileHumanActionNative;
import com.sensetime.stmobile.model.STAnimalFace;
import com.sensetime.stmobile.model.STAnimalFaceInfo;
import com.sensetime.stmobile.model.STEffectRenderInParam;
import com.sensetime.stmobile.model.STEffectRenderOutParam;
import com.sensetime.stmobile.model.STEffectTexture;
import com.sensetime.stmobile.model.STFaceMeshList;
import com.sensetime.stmobile.model.STHumanAction;
import com.sensetime.stmobile.params.STBeautyParamsType;
import com.sensetime.stmobile.params.STEffectBeautyType;
import com.sensetime.stmobile.params.STEffectParam;
import com.sensetime.stmobile.params.STHumanActionParamsType;
import com.sensetime.stmobile.params.STRotateType;

import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;

public class STRenderer {
    private static final String TAG = STRenderer.class.getSimpleName();
    public static final float MAKEUP_HAIRDYE_STRENGTH_RATIO = 0.22f;
    // Messages for submodule and sticker handler threads
    private static final int MESSAGE_NEED_CHANGE_STICKER = 1003;
    private static final int MESSAGE_NEED_REMOVE_STICKER = 1004;

    private final Context mContext;

    // Handler threads
    private HandlerThread mChangeStickerManagerThread;
    private Handler mChangeStickerManagerHandler;

    // Sense Tang Native Modules
    public STMobileEffectNative mSTMobileEffectNative = new STMobileEffectNative();
    protected STMobileHumanActionNative mSTHumanActionNative = new STMobileHumanActionNative();
    protected STMobileAnimalNative mStAnimalNative = new STMobileAnimalNative();
    private STMobileColorConvertNative mSTMobileColorConvertNative = new STMobileColorConvertNative();

    private boolean mAuthorized;

    private final Object mHumanActionHandleLock = new Object();
    private final int mHumanActionCreateConfig = STMobileHumanActionNative.ST_MOBILE_HUMAN_ACTION_DEFAULT_CONFIG_VIDEO;
    private boolean mIsCreateHumanActionHandleSucceeded = false;
    private long mDetectConfig = -1;

    private STGLRender mGLRenderBefore;
    private int[] mTextureOutId;
    private byte[] mImageDataBuffer = null;
    protected STHumanAction[] mSTHumanAction = new STHumanAction[2];

    protected STAnimalFaceInfo[] mAnimalFaceInfo = new STAnimalFaceInfo[2];
    private boolean mNeedAnimalDetect = false;

    private String mCurrentSticker;
    private final LinkedHashMap<Integer, String> mCurrentStickerMaps = new LinkedHashMap<>();

    private final STEffectParameters mEffectParams = new STEffectParameters();

    private STFaceMeshList faceMeshList;
    private int mImageWidth;
    private int mImageHeight;

    public STRenderer(Context context) {
        mContext = context;
        mSTHumanAction[0] = new STHumanAction();
        init();
    }

    private void init() {
        checkLicense();
        initColorConvert();
        initGLRender();
        initMobileEffect();
        initHumanAction();
        initHandlerManager();
    }

    private void checkLicense() {
        mAuthorized = STLicenseUtils.checkLicense(mContext);
    }

    private void initHumanAction() {
        //初始化非OpengGL相关的句柄，包括人脸检测及人脸属性
        new Thread(() -> {
            synchronized (mHumanActionHandleLock) {
                long startLoadHumanActionModel = System.currentTimeMillis();

                //从asset资源文件夹读取model到内存，再使用底层st_mobile_human_action_create_from_buffer接口创建handle
                int result = mSTHumanActionNative.createInstanceFromAssetFile(FileUtils.getActionModelName(), mHumanActionCreateConfig, mContext.getAssets());
                LogUtils.i(TAG, "the result for createInstance for human_action is %d", result);
                Log.e(TAG, "load model name: " + FileUtils.getActionModelName() + " cost time: " + (System.currentTimeMillis() - startLoadHumanActionModel));

                if (result == 0) {
                    result = mSTHumanActionNative.addSubModelFromAssetFile(FileUtils.MODEL_NAME_HAND, mContext.getAssets());
                    LogUtils.i(TAG, "add hand model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(FileUtils.MODEL_NAME_SEGMENT, mContext.getAssets());
                    LogUtils.i(TAG, "add figure segment model result: %d", result);

                    mIsCreateHumanActionHandleSucceeded = true;

                    mSTHumanActionNative.setParam(STHumanActionParamsType.ST_HUMAN_ACTION_PARAM_BACKGROUND_BLUR_STRENGTH, 0.35f);

                    //240
                    result = mSTHumanActionNative.addSubModelFromAssetFile(FileUtils.MODEL_NAME_FACE_EXTRA, mContext.getAssets());
                    LogUtils.i(TAG, "add face extra model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(FileUtils.MODEL_NAME_HAIR, mContext.getAssets());
                    LogUtils.i(TAG, "add hair model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(FileUtils.MODEL_NAME_LIPS_PARSING, mContext.getAssets());
                    LogUtils.i(TAG, "add lips parsing model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(FileUtils.HEAD_SEGMENT_MODEL_NAME, mContext.getAssets());
                    LogUtils.i(TAG, "add head segment model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(FileUtils.HEAD_SEGMENT_DBL, mContext.getAssets());
                    LogUtils.i(TAG, "add dbl segment model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(FileUtils.MODEL_SEGMENT_SKY, mContext.getAssets());
                    LogUtils.i(TAG, "add sky segment model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(FileUtils.MODEL_NAME_AVATAR_HELP, mContext.getAssets());
                    LogUtils.i(TAG, "add avatar help model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(FileUtils.MODEL_FOOT, mContext.getAssets());
                    LogUtils.i(TAG, "add foot model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(FileUtils.MODEL_PANT, mContext.getAssets());
                    LogUtils.i(TAG, "add pant model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(FileUtils.MODEL_SEGMENT_SKIN, mContext.getAssets());
                    LogUtils.i(TAG, "add skin segment model result: %d", result);
                    //setHasSkinCapability(result != STResultCode.ST_E_NO_CAPABILITY.getResultCode());

                    result = mSTHumanActionNative.addSubModelFromAssetFile(FileUtils.MODEL_3DMESH, mContext.getAssets());
                    LogUtils.i(TAG, "add 3d mesh model result: %d", result);

                    // 开启face mesh的边缘线检测功能
                    mSTHumanActionNative.setParam(STHumanActionParamsType.ST_HUMAN_ACTION_PARAM_FACE_MESH_CONTOUR, 1.0f);
                    // 设置face mesh检测类别
                    mSTHumanActionNative.setParam(STHumanActionParamsType.ST_HUMAN_ACTION_PARAM_FACE_MESH_MODE, 0x111000);

                    this.faceMeshList = mSTHumanActionNative.getMeshList(STHumanActionParamsType.STMeshType.ST_MOBILE_FACE_MESH);
                }

            }
        }).start();
    }

    private void initColorConvert(){
        mSTMobileColorConvertNative.createInstance();
    }


    private void initMobileEffect() {
        int result = mSTMobileEffectNative.createInstance(mContext, STMobileEffectNative.EFFECT_CONFIG_NONE);
        mSTMobileEffectNative.setParam(STMobileEffectParams.EFFECT_PARAM_QUATERNION_SMOOTH_FRAME, 5);
        LogUtils.i(TAG, "the result is for initEffect:" + result);
    }


    private void initGLRender() {
        mGLRenderBefore = new STGLRender(GLES11Ext.GL_TEXTURE_EXTERNAL_OES);
    }


    private void initHandlerManager() {
        mChangeStickerManagerThread = new HandlerThread("ChangeStickerManagerThread");
        mChangeStickerManagerThread.start();
        mChangeStickerManagerHandler = new Handler(mChangeStickerManagerThread.getLooper()) {
            @Override
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case MESSAGE_NEED_CHANGE_STICKER:
                        String sticker = (String) msg.obj;
                        mCurrentSticker = sticker;
                        int packageId1 = mSTMobileEffectNative.changePackage(mCurrentSticker);
                        Log.d(TAG, "ST_XCZ STMobileEffectNative changePackage sticker=" + mCurrentSticker + ",packageId=" + packageId1);

                        Log.d(TAG, "change_package: packageId1:" + packageId1);
                        if (packageId1 > 0) {
                            Iterator<LinkedHashMap.Entry<Integer, String>> iterator = mCurrentStickerMaps.entrySet().iterator();
                            while (iterator.hasNext()) {
                                if (sticker.equals(iterator.next().getValue())) {
                                    iterator.remove();
                                }
                            }

                            mCurrentStickerMaps.put(packageId1, sticker);
                        }
                        updateAnimalDetectConfig();
                        break;
                    case MESSAGE_NEED_REMOVE_STICKER:
                        int packageId = (int) msg.obj;
                        int result = mSTMobileEffectNative.removeEffect(packageId);
                        Log.d(TAG, "ST_XCZ STMobileEffectNative removeEffect packageId=" + packageId);

                        if (mCurrentStickerMaps != null && result == 0) {
                            mCurrentStickerMaps.remove(packageId);
                        }
                        break;
                    default:
                        break;
                }
            }
        };
    }

    public void updateAnimalDetectConfig() {
        mNeedAnimalDetect = mSTMobileEffectNative.getAnimalDetectConfig() > 0;
    }

    /**
     * human action detect的配置选项,根据渲染接口需要配置
     */
    public void updateHumanActionDetectConfig() {
        long humanActionDetectConfig = mSTMobileEffectNative.getHumanActionDetectConfig();
        if(humanActionDetectConfig != mDetectConfig){
            mDetectConfig = humanActionDetectConfig;
            Log.d(TAG, "ST_XCZ updateHumanActionDetectConfig mDetectConfig=" + mDetectConfig);
        }
    }

    /**
     * Transform orientation in angles to STRotateType
     *
     * @param rotation
     * @return
     */
    private int getCurrentOrientation(int rotation) {
        switch (rotation) {
            case 90:
                return STRotateType.ST_CLOCKWISE_ROTATE_90;
            case 180:
                return STRotateType.ST_CLOCKWISE_ROTATE_180;
            case 270:
                return STRotateType.ST_CLOCKWISE_ROTATE_270;
            default:
                return STRotateType.ST_CLOCKWISE_ROTATE_0;
        }
    }

    public int preProcess(
            int width, int height, int rotation,
            byte[] cameraPixel, int pixelFormat
    ) {

        if (!mAuthorized || !mIsCreateHumanActionHandleSucceeded) {
            return -1;
        }

        boolean sizeChange = mImageWidth != width || mImageHeight != height;
        mImageWidth = width;
        mImageHeight = height;


        if (sizeChange || mImageDataBuffer == null) {
            mImageDataBuffer = new byte[cameraPixel.length];
        }
        System.arraycopy(cameraPixel, 0, mImageDataBuffer, 0, cameraPixel.length);


        // >>>>>> 1. detect human point info using cameraData

        // prepare params
        updateHumanActionDetectConfig();
        //mSTHumanActionNative.nativeHumanActionPtrCopy();

        int ret = mSTHumanActionNative.nativeHumanActionDetectPtr(
                mImageDataBuffer,
                pixelFormat,
                mDetectConfig,
                getCurrentOrientation(rotation),
                width,
                height);
        if (ret == 0) {
            if (mNeedAnimalDetect) {
                animalDetect(mImageDataBuffer, pixelFormat, getCurrentOrientation(rotation), width, height, 0);
            } else {
                mAnimalFaceInfo[0] = new STAnimalFaceInfo(null, 0);
            }
        }

        // >>>>>> 2. upload nv21 to texture
        if (mTextureOutId == null) {
            mTextureOutId = new int[2];
            GlUtil.initEffectTexture(mImageWidth, mImageHeight, mTextureOutId, GLES20.GL_TEXTURE_2D);
        } else if (sizeChange) {
            GLES20.glDeleteTextures(mTextureOutId.length, mTextureOutId, 0);
            mTextureOutId = null;
            return -1;
        }

        mSTMobileColorConvertNative.setTextureSize(mImageWidth, mImageHeight);
        mSTMobileColorConvertNative.nv21BufferToRgbaTexture(width, height,
                STRotateType.ST_CLOCKWISE_ROTATE_0,
                false,
                mImageDataBuffer,
                mTextureOutId[0]);
        int textureId = mTextureOutId[0];

        // >>>>>> 3. render texture

        //输入纹理，纹理只支持2D
        STEffectTexture stEffectTexture = new STEffectTexture(textureId, mImageWidth, mImageHeight, 0);
        //输出纹理，需要在上层初始化
        STEffectTexture stEffectTextureOut = new STEffectTexture(mTextureOutId[1], mImageWidth, mImageHeight, 0);

        //渲染接口输入参数
        STEffectRenderInParam sTEffectRenderInParam = new STEffectRenderInParam(
                mSTHumanActionNative.getNativeHumanActionResultPtr(),
                mAnimalFaceInfo[0],
                0,
                0,
                false,
                null,
                stEffectTexture,
                null
        );
        //渲染接口输出参数
        STEffectRenderOutParam stEffectRenderOutParam = new STEffectRenderOutParam(
                stEffectTextureOut,
                null,
                null);
        mSTMobileEffectNative.setParam(STEffectParam.EFFECT_PARAM_USE_INPUT_TIMESTAMP, 1);
        ret = mSTMobileEffectNative.render(sTEffectRenderInParam, stEffectRenderOutParam, false);
        if (ret == 0 && stEffectRenderOutParam.getTexture() != null) {
            textureId = stEffectRenderOutParam.getTexture().getId();
        }

        GLES20.glFinish();

        return textureId;
    }


    /**
     * @param width           camera preview width
     * @param height          camera preview height
     * @param rotation     camera preview orientation
     * @param cameraPixel     camera preview pixel data
     * @param pixelFormat     {@link STCommonNative#ST_PIX_FMT_NV21} and etc.
     * @param cameraTextureId camera preview texture id
     * @param texFormat       {@link GLES11Ext#GL_TEXTURE_EXTERNAL_OES} or {@link GLES20#GL_TEXTURE_2D}
     * @return new Texture ID to render
     */
    public int preProcess(
            int width, int height, int rotation,
            byte[] cameraPixel, int pixelFormat,
            int cameraTextureId, int texFormat, float[] transformMatrix) {

        if (!mAuthorized) {
            return -1;
        }


        boolean sizeChange = mImageWidth != width || mImageHeight != height;
        mImageWidth = width;
        mImageHeight = height;


        // >>>>>> 1. translate oes texture to 2d
        if (mTextureOutId == null) {
            mTextureOutId = new int[1];
            GlUtil.initEffectTexture(mImageWidth, mImageHeight, mTextureOutId, GLES20.GL_TEXTURE_2D);
        } else if (sizeChange) {
            GLES20.glDeleteTextures(mTextureOutId.length, mTextureOutId, 0);
            mTextureOutId = null;
            return -1;
        }

        int textureId = cameraTextureId;
        if (texFormat == GLES11Ext.GL_TEXTURE_EXTERNAL_OES) {
            mGLRenderBefore.adjustRenderSize(mImageWidth, mImageHeight, 0, false, true);
            textureId = mGLRenderBefore.process(cameraTextureId, transformMatrix);
        }

        // >>>>>> 2. detect human point info using cameraData
        if (mIsCreateHumanActionHandleSucceeded) {
            if (sizeChange || mImageDataBuffer == null) {
                mImageDataBuffer = new byte[cameraPixel.length];
            }
            System.arraycopy(cameraPixel, 0, mImageDataBuffer, 0, cameraPixel.length);

            // prepare params
            updateHumanActionDetectConfig();
            //mSTHumanActionNative.nativeHumanActionPtrCopy();

            int ret = mSTHumanActionNative.nativeHumanActionDetectPtr(mImageDataBuffer,
                    pixelFormat,
                    mDetectConfig,
                    getCurrentOrientation(rotation),
                    width,
                    height);
            //STHumanAction nativeHumanAction = mSTHumanActionNative.getNativeHumanAction();
            //LogUtils.i(TAG, "human action detect cost time: %d, ret: %d", System.currentTimeMillis() - startHumanAction, ret);
            if (ret == 0) {
                if (mNeedAnimalDetect) {
                    animalDetect(mImageDataBuffer, pixelFormat, getCurrentOrientation(rotation), width, height, 0);
                } else {
                    mAnimalFaceInfo[0] = new STAnimalFaceInfo(null, 0);
                }
            }
        }


        // >>>>>> 3. render texture

        //输入纹理，纹理只支持2D
        STEffectTexture stEffectTexture = new STEffectTexture(textureId, mImageWidth, mImageHeight, 0);
        //输出纹理，需要在上层初始化
        STEffectTexture stEffectTextureOut = new STEffectTexture(mTextureOutId[0], mImageWidth, mImageHeight, 0);

        //渲染接口输入参数
        STEffectRenderInParam sTEffectRenderInParam = new STEffectRenderInParam(
                mSTHumanActionNative.getNativeHumanActionResultPtr(),
                mAnimalFaceInfo[0],
                0,
                0,
                false,
                null,
                stEffectTexture,
                null);
        //渲染接口输出参数
        STEffectRenderOutParam stEffectRenderOutParam = new STEffectRenderOutParam(stEffectTextureOut, null, null);
        mSTMobileEffectNative.setParam(STEffectParam.EFFECT_PARAM_USE_INPUT_TIMESTAMP, 1);
        int ret = mSTMobileEffectNative.render(sTEffectRenderInParam, stEffectRenderOutParam, false);
        if (ret == 0 && stEffectRenderOutParam.getTexture() != null) {
            textureId = stEffectRenderOutParam.getTexture().getId();
        }

        GLES20.glFinish();

        return textureId;
    }

    protected void animalDetect(byte[] imageData, int format, int orientation, int width, int height, int index) {
        if (mNeedAnimalDetect) {
            long catDetectStartTime = System.currentTimeMillis();
            int animalDetectConfig = (int) mSTMobileEffectNative.getAnimalDetectConfig();
            Log.d(TAG, "test_animalDetect: " + animalDetectConfig);
            STAnimalFace[] animalFaces = mStAnimalNative.animalDetect(imageData, format, orientation, animalDetectConfig, width, height);
            LogUtils.i(TAG, "animal detect cost time: %d", System.currentTimeMillis() - catDetectStartTime);

            mAnimalFaceInfo[index] = new STAnimalFaceInfo(animalFaces, animalFaces == null ? 0 : animalFaces.length);
        } else {
            mAnimalFaceInfo[index] = new STAnimalFaceInfo(null, 0);
        }
    }

    public STEffectParameters getSTEffectParameters() {
        return mEffectParams;
    }


    public void setFilterStyle(String filterType, String filterName, String modelPath) {
        int ret = mSTMobileEffectNative.setBeauty(STEffectBeautyType.EFFECT_BEAUTY_FILTER, modelPath);
        Log.d(TAG, "ST_XCZ STMobileEffectNative setFilterStyle() called with: filterType = [" + filterType + "], filterName = [" + filterName + "], modelPath = [" + modelPath + "], ret=" + ret);

    }

    public void setFilterStrength(float strength) {
        mSTMobileEffectNative.setBeautyStrength(STEffectBeautyType.EFFECT_BEAUTY_FILTER, strength);
        Log.d(TAG, "ST_XCZ STMobileEffectNative setBeautyStrength type=" + STEffectBeautyType.EFFECT_BEAUTY_FILTER + ",strength=" + strength);

    }


    public int setBeautyMode(int param, int value) {
        int ret = mSTMobileEffectNative.setBeautyMode(param, value);
        Log.d(TAG, "ST_XCZ STMobileEffectNative setBeautyMode param=" + param + ",value=" + value);

        return ret;
    }

    /**
     * STRenderer itself does not any more maintain
     * the values of beauty params .
     *
     * @param param
     * @param value
     */
    public void setBeautyParam(int param, float value) {
        mEffectParams.setBeautyParam(param, value);
        int ret = mSTMobileEffectNative.setParam(param, value);
        Log.d(TAG, "ST_XCZ STMobileEffectNative setParam param=" + param + ",value=" + value);
    }

    public void setBeautyStrength(int type, float value) {
        mSTMobileEffectNative.setBeautyStrength(type, value);
        Log.d(TAG, "ST_XCZ STMobileEffectNative setBeautyStrength type=" + type + ",value=" + value);
    }

    public void setPackageBeautyGroupStrength(int packageId, int beautyGroup, float strength) {
        mSTMobileEffectNative.setPackageBeautyGroupStrength(packageId, beautyGroup, strength);
        Log.d(TAG, "ST_XCZ STMobileEffectNative setPackageBeautyGroupStrength packageId=" + packageId + ",beautyGroup=" + beautyGroup + ",strength=" + strength);
    }


    /**
     * Set makeup effect for a type from STMobileType
     *
     * @param type     makeup type
     * @param typePath sub path of this type
     * @return package id for this effect
     */
    public void setMakeupForType(int type, String typePath) {
        try {
            int ret = mSTMobileEffectNative.setBeauty(STUtils.convertMakeupTypeToNewType(type), typePath);
            Log.d(TAG, "ST_XCZ STMobileEffectNative setBeauty type=" + STUtils.convertMakeupTypeToNewType(type) + ",typePath=" + typePath + ",ret=" + ret);

        } catch (ArrayIndexOutOfBoundsException e) {
            e.printStackTrace();
        }
    }

    public void removeMakeupByType(int type) {
        int ret = mSTMobileEffectNative.setBeauty(STUtils.convertMakeupTypeToNewType(type), null);
        Log.d(TAG, "ST_XCZ STMobileEffectNative setBeauty type=" + STUtils.convertMakeupTypeToNewType(type) + ",path=" + null + ",ret=" + ret);
    }

    public void setMakeupStrength(int type, float strength) {
        if (strength >= 0) {
            if (STUtils.convertMakeupTypeToNewType(type) == STEffectBeautyType.EFFECT_BEAUTY_HAIR_DYE) {
                strength = strength * MAKEUP_HAIRDYE_STRENGTH_RATIO;
            }
            int ret = mSTMobileEffectNative.setBeautyStrength(STUtils.convertMakeupTypeToNewType(type), strength);
            Log.d(TAG, "ST_XCZ STMobileEffectNative setBeautyStrength type=" + STUtils.convertMakeupTypeToNewType(type) + ",strength=" + strength + ",ret=" + ret);

        }
    }

    public void changeSticker(String sticker) {
        mChangeStickerManagerHandler.removeMessages(MESSAGE_NEED_CHANGE_STICKER);
        Message msg = mChangeStickerManagerHandler.obtainMessage(MESSAGE_NEED_CHANGE_STICKER);
        msg.obj = sticker;

        mChangeStickerManagerHandler.sendMessage(msg);
    }

    public void removeSticker(String path) {
        int packageId = -1;
        for (Map.Entry<Integer, String> entry : mCurrentStickerMaps.entrySet()) {
            if (entry.getValue().equals(path)) {
                packageId = entry.getKey();
                break;
            }
        }
        if(packageId != -1){
            removeSticker(packageId);
        }
    }

    public void removeSticker(int packageId) {
        int result = mSTMobileEffectNative.removeEffect(packageId);

        if (mCurrentStickerMaps != null && result == 0) {
            mCurrentStickerMaps.remove(packageId);
        }
    }

    public void release() {
        //Log.e(TAG, "recycle preprocessor");
        synchronized (mHumanActionHandleLock) {
            mSTHumanActionNative.destroyInstance();
        }

        mSTHumanActionNative.reset();
        mSTMobileColorConvertNative.destroyInstance();

        mChangeStickerManagerHandler.removeCallbacksAndMessages(null);
        mChangeStickerManagerThread.quit();
        mChangeStickerManagerThread = null;
        deleteTextures();
        mGLRenderBefore.destroyPrograms();
        mImageWidth = mImageHeight = 0;
    }

    private void deleteTextures() {
        //LogUtils.i(TAG, "delete textures");
        deleteInternalTextures();
    }

    private void deleteInternalTextures() {
        if (mTextureOutId != null) {
            GLES20.glDeleteTextures(mTextureOutId.length, mTextureOutId, 0);
            mTextureOutId = null;
        }
    }

    public static class STEffectParameters {
        private static final int[] DEFAULT_BEAUTY_PARAMS = {
                STBeautyParamsType.ST_BEAUTIFY_REDDEN_STRENGTH,
                STBeautyParamsType.ST_BEAUTIFY_SMOOTH_STRENGTH,
                STBeautyParamsType.ST_BEAUTIFY_WHITEN_STRENGTH,
                STBeautyParamsType.ST_BEAUTIFY_ENLARGE_EYE_RATIO,
                STBeautyParamsType.ST_BEAUTIFY_SHRINK_FACE_RATIO,

                STBeautyParamsType.ST_BEAUTIFY_SHRINK_JAW_RATIO,
                STBeautyParamsType.ST_BEAUTIFY_CONSTRACT_STRENGTH,
                STBeautyParamsType.ST_BEAUTIFY_SATURATION_STRENGTH,
                STBeautyParamsType.ST_BEAUTIFY_DEHIGHLIGHT_STRENGTH,
                STBeautyParamsType.ST_BEAUTIFY_NARROW_FACE_STRENGTH,

                STBeautyParamsType.ST_BEAUTIFY_3D_NARROW_NOSE_RATIO,
                STBeautyParamsType.ST_BEAUTIFY_3D_NOSE_LENGTH_RATIO,
                STBeautyParamsType.ST_BEAUTIFY_3D_CHIN_LENGTH_RATIO,
                STBeautyParamsType.ST_BEAUTIFY_3D_MOUTH_SIZE_RATIO,
                STBeautyParamsType.ST_BEAUTIFY_3D_PHILTRUM_LENGTH_RATIO,

                STBeautyParamsType.ST_BEAUTIFY_3D_HAIRLINE_HEIGHT_RATIO,
                STBeautyParamsType.ST_BEAUTIFY_3D_THIN_FACE_SHAPE_RATIO,
                STBeautyParamsType.ST_BEAUTIFY_3D_EYE_DISTANCE_RATIO,
                STBeautyParamsType.ST_BEAUTIFY_3D_EYE_ANGLE_RATIO,
                STBeautyParamsType.ST_BEAUTIFY_3D_OPEN_CANTHUS_RATIO,

                STBeautyParamsType.ST_BEAUTIFY_3D_PROFILE_RHINOPLASTY_RATIO,
                STBeautyParamsType.ST_BEAUTIFY_3D_BRIGHT_EYE_RATIO,
                STBeautyParamsType.ST_BEAUTIFY_3D_REMOVE_DARK_CIRCLES_RATIO,
                STBeautyParamsType.ST_BEAUTIFY_3D_REMOVE_NASOLABIAL_FOLDS_RATIO,
                STBeautyParamsType.ST_BEAUTIFY_3D_WHITE_TEETH_RATIO,
                STBeautyParamsType.ST_BEAUTIFY_3D_APPLE_MUSLE_RATIO
        };

        private static float[] DEFAULT_BEAUTY_PARAMS_VALUES = {
                0.36f, 0.74f, 0.02f, 0.13f, 0.11f, 0.1f,
                0f, 0f, 0f, 0f, 0f,
                0f, 0f, 0f, 0f, 0f,
                0f, 0f, 0f, 0f, 0f,
                0f, 0f, 0f, 0f, 0f
        };

        // Index as the STBeautyParamsType
        private SparseArray<Float> mBeautyParams = new SparseArray<>();

        private String mCurrentFilter;
        private float mFilterStrength;
        private Map<String, String> mMakeupMap = new HashMap<>();
        private String mCurrentSticker;

        /**
         * Get current value of a beauty parameter
         *
         * @param param STBeautyParam values
         * @return current value of this param
         */
        public float getBeautyParam(int param) {
            return mBeautyParams.get(param, 0f);
        }

        public void setBeautyParam(int param, float value) {
            mBeautyParams.append(param, value);
        }

        void initDefaultBeautyParams(STMobileEffectNative handle) {
            if (DEFAULT_BEAUTY_PARAMS.length != DEFAULT_BEAUTY_PARAMS_VALUES.length) {
                return;
            }

            for (int i = 0; i < DEFAULT_BEAUTY_PARAMS.length; i++) {
                handle.setParam(DEFAULT_BEAUTY_PARAMS[i], DEFAULT_BEAUTY_PARAMS_VALUES[i]);
                mBeautyParams.append(DEFAULT_BEAUTY_PARAMS[i], DEFAULT_BEAUTY_PARAMS_VALUES[i]);
            }
        }

        void setFilter(String path, float strength) {
            mCurrentFilter = path;
            mFilterStrength = strength;
        }

        public String getFilter() {
            return mCurrentFilter;
        }

        public float getFilterStrength() {
            return mFilterStrength;
        }

        public void setMakeupPath(String index, String path) {
            mMakeupMap.put(index, path);
        }

        public String getMakeupPath(String index) {
            return mMakeupMap.get(index);
        }

        public void setSticker(String path) {
            mCurrentSticker = path;
        }

        public String getCurrentSticker() {
            return mCurrentSticker;
        }
    }
}
