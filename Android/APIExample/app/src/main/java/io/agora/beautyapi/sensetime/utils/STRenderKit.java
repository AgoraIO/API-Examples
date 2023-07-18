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

package io.agora.beautyapi.sensetime.utils;

import android.annotation.TargetApi;
import android.content.Context;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;

import com.sensetime.stmobile.STCommonNative;
import com.sensetime.stmobile.STMobileAnimalNative;
import com.sensetime.stmobile.STMobileEffectNative;
import com.sensetime.stmobile.STMobileEffectParams;
import com.sensetime.stmobile.STMobileHumanActionNative;
import com.sensetime.stmobile.params.STEffectBeautyType;
import com.sensetime.stmobile.params.STHumanActionParamsType;

import org.jetbrains.annotations.NotNull;

import java.io.File;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;

import io.agora.beautyapi.sensetime.utils.processor.IBeautyProcessor;
import io.agora.beautyapi.sensetime.utils.processor.IBeautyProcessorKt;
import io.agora.beautyapi.sensetime.utils.processor.InputInfo;
import io.agora.beautyapi.sensetime.utils.processor.OutputInfo;
import io.agora.beautyapi.sensetime.utils.utils.FileUtils;
import io.agora.beautyapi.sensetime.utils.utils.LogUtils;
import io.agora.beautyapi.sensetime.utils.utils.STLicenseUtils;
import io.agora.beautyapi.sensetime.utils.utils.STUtils;

public class STRenderKit {
    private static final String TAG = STRenderKit.class.getSimpleName();
    public static final float MAKEUP_HAIRDYE_STRENGTH_RATIO = 0.22f;
    // Messages for submodule and sticker handler threads
    private static final int MESSAGE_NEED_CHANGE_STICKER = 1003;
    private static final int MESSAGE_NEED_REMOVE_STICKER = 1004;

    private final Context mContext;

    // Handler threads
    private HandlerThread mChangeStickerManagerThread;
    private Handler mChangeStickerManagerHandler;

    // Sense Tang Native Modules
    private final STMobileEffectNative mSTMobileEffectNative = new STMobileEffectNative();
    private final STMobileHumanActionNative mSTHumanActionNative = new STMobileHumanActionNative();
    private final STMobileAnimalNative mStAnimalNative = new STMobileAnimalNative();
    private boolean mAuthorized;

    private final Object mHumanActionHandleLock = new Object();
    private final int mHumanActionCreateConfig = STMobileHumanActionNative.ST_MOBILE_HUMAN_ACTION_DEFAULT_CONFIG_VIDEO;
    private IBeautyProcessor mProcessor;
    private String mCurrentSticker;
    private final LinkedHashMap<Integer, String> mCurrentStickerMaps = new LinkedHashMap<>();

    private boolean isSensorEnable;
    private final String mResourcePath;

    public STRenderKit(Context context, String resourcePath) {
        mContext = context;
        mResourcePath = resourcePath;
        init();
    }

    private void init() {
        checkLicense();
        initMobileEffect();
        initHumanAction();
        initHandlerManager();
    }

    public String getResourcePath(String filePath){
        if(TextUtils.isEmpty(mResourcePath)){
            return filePath;
        }
        if(mResourcePath.endsWith("///")){
            return mResourcePath + filePath;
        }
        return mResourcePath + File.separator + filePath;
    }

    private void checkLicense() {
        mAuthorized = STLicenseUtils.checkLicenseFromAssetFile(mContext, getResourcePath("license/SenseME.lic"), false);
    }

    private void initHumanAction() {
        //初始化非OpengGL相关的句柄，包括人脸检测及人脸属性
        new Thread(() -> {
            synchronized (mHumanActionHandleLock) {
                long startLoadHumanActionModel = System.currentTimeMillis();

                //从asset资源文件夹读取model到内存，再使用底层st_mobile_human_action_create_from_buffer接口创建handle
                int result = mSTHumanActionNative.createInstanceFromAssetFile(getResourcePath(FileUtils.getActionModelName()), mHumanActionCreateConfig, mContext.getAssets());
                LogUtils.i(TAG, "the result for createInstance for human_action is %d", result);
                Log.e(TAG, "load model name: " + getResourcePath(FileUtils.getActionModelName()) + " cost time: " + (System.currentTimeMillis() - startLoadHumanActionModel));

                if (result == 0) {
                    result = mSTHumanActionNative.addSubModelFromAssetFile(getResourcePath(FileUtils.MODEL_NAME_HAND), mContext.getAssets());
                    LogUtils.i(TAG, "add hand model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(getResourcePath(FileUtils.MODEL_NAME_SEGMENT), mContext.getAssets());
                    LogUtils.i(TAG, "add figure segment model result: %d", result);

                    mSTHumanActionNative.setParam(STHumanActionParamsType.ST_HUMAN_ACTION_PARAM_BACKGROUND_BLUR_STRENGTH, 0.35f);

                    //240
                    result = mSTHumanActionNative.addSubModelFromAssetFile(getResourcePath(FileUtils.MODEL_NAME_FACE_EXTRA), mContext.getAssets());
                    LogUtils.i(TAG, "add face extra model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(getResourcePath(FileUtils.MODEL_NAME_HAIR), mContext.getAssets());
                    LogUtils.i(TAG, "add hair model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(getResourcePath(FileUtils.MODEL_NAME_LIPS_PARSING), mContext.getAssets());
                    LogUtils.i(TAG, "add lips parsing model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(getResourcePath(FileUtils.HEAD_SEGMENT_MODEL_NAME), mContext.getAssets());
                    LogUtils.i(TAG, "add head segment model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(getResourcePath(FileUtils.HEAD_SEGMENT_DBL), mContext.getAssets());
                    LogUtils.i(TAG, "add dbl segment model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(getResourcePath(FileUtils.MODEL_SEGMENT_SKY), mContext.getAssets());
                    LogUtils.i(TAG, "add sky segment model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(getResourcePath(FileUtils.MODEL_NAME_AVATAR_HELP), mContext.getAssets());
                    LogUtils.i(TAG, "add avatar help model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(getResourcePath(FileUtils.MODEL_FOOT), mContext.getAssets());
                    LogUtils.i(TAG, "add foot model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(getResourcePath(FileUtils.MODEL_PANT), mContext.getAssets());
                    LogUtils.i(TAG, "add pant model result: %d", result);

                    result = mSTHumanActionNative.addSubModelFromAssetFile(getResourcePath(FileUtils.MODEL_SEGMENT_SKIN), mContext.getAssets());
                    LogUtils.i(TAG, "add skin segment model result: %d", result);
                    //setHasSkinCapability(result != STResultCode.ST_E_NO_CAPABILITY.getResultCode());

                    result = mSTHumanActionNative.addSubModelFromAssetFile(getResourcePath(FileUtils.MODEL_3DMESH), mContext.getAssets());
                    LogUtils.i(TAG, "add 3d mesh model result: %d", result);

                    // 开启face mesh的边缘线检测功能
                    mSTHumanActionNative.setParam(STHumanActionParamsType.ST_HUMAN_ACTION_PARAM_FACE_MESH_CONTOUR, 1.0f);
                    // 设置face mesh检测类别
                    mSTHumanActionNative.setParam(STHumanActionParamsType.ST_HUMAN_ACTION_PARAM_FACE_MESH_MODE, 0x111000);

                    // this.faceMeshList = mSTHumanActionNative.getMeshList(STHumanActionParamsType.STMeshType.ST_MOBILE_FACE_MESH);
                }

                long startAnimalTime = System.currentTimeMillis();
                result = mStAnimalNative.createInstanceFromAssetFile(getResourcePath(FileUtils.MODEL_NAME_CATFACE_CORE), STCommonNative.ST_MOBILE_TRACKING_MULTI_THREAD, mContext.getAssets());
                LogUtils.i(TAG, "create cat model result: %d", result);
                result = mStAnimalNative.addSubModelFromAssetFile(getResourcePath(FileUtils.MODEL_NAME_DOG), mContext.getAssets());
                LogUtils.i(TAG, "add dog model result: %d", result);
                Log.i(TAG, "create animal handle total cost time:" + (System.currentTimeMillis() - startAnimalTime));

            }
        }).start();
    }


    private void initMobileEffect() {
        int result = mSTMobileEffectNative.createInstance(mContext, STMobileEffectNative.EFFECT_CONFIG_NONE);
        mSTMobileEffectNative.setParam(STMobileEffectParams.EFFECT_PARAM_QUATERNION_SMOOTH_FRAME, 5);
        LogUtils.i(TAG, "the result is for initEffect:" + result);
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

    public void setFilterStyle(String filterType, String filterName, String modelPath) {
        mSTMobileEffectNative.setBeauty(STEffectBeautyType.EFFECT_BEAUTY_FILTER, modelPath);
    }

    public void setFilterStrength(float strength) {
        mSTMobileEffectNative.setBeautyStrength(STEffectBeautyType.EFFECT_BEAUTY_FILTER, strength);
    }


    public int setBeautyMode(int param, int value) {
        return mSTMobileEffectNative.setBeautyMode(param, value);
    }


    public void setBeautyStrength(int type, float value) {
        mSTMobileEffectNative.setBeautyStrength(type, value);
    }

    public void setPackageBeautyGroupStrength(int packageId, int beautyGroup, float strength) {
        mSTMobileEffectNative.setPackageBeautyGroupStrength(packageId, beautyGroup, strength);
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
            mSTMobileEffectNative.setBeauty(STUtils.convertMakeupTypeToNewType(type), typePath);
        } catch (ArrayIndexOutOfBoundsException e) {
            e.printStackTrace();
        }
    }

    public void removeMakeupByType(int type) {
        mSTMobileEffectNative.setBeauty(STUtils.convertMakeupTypeToNewType(type), null);
    }

    public void setMakeupStrength(int type, float strength) {
        if (strength >= 0) {
            if (STUtils.convertMakeupTypeToNewType(type) == STEffectBeautyType.EFFECT_BEAUTY_HAIR_DYE) {
                strength = strength * MAKEUP_HAIRDYE_STRENGTH_RATIO;
            }
            mSTMobileEffectNative.setBeautyStrength(STUtils.convertMakeupTypeToNewType(type), strength);
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
        if (packageId != -1) {
            removeSticker(packageId);
        }
    }

    public void removeSticker(int packageId) {
        int result = mSTMobileEffectNative.removeEffect(packageId);
        if (result == 0) {
            mCurrentStickerMaps.remove(packageId);
        }
    }

    public void release() {
        synchronized (mHumanActionHandleLock) {
            mSTHumanActionNative.destroyInstance();
        }
        mSTHumanActionNative.reset();
        mChangeStickerManagerHandler.removeCallbacksAndMessages(null);
        mChangeStickerManagerThread.quit();
        mChangeStickerManagerThread = null;
        if (mProcessor != null) {
            mProcessor.release();
            mProcessor = null;
        }
    }

    public void resetProcessor() {
        if (mProcessor != null) {
            mProcessor.reset();
        }
    }

    public void enableSensor(boolean enable) {
        isSensorEnable = enable;
        if (mProcessor != null) {
            mProcessor.enableSensor(mContext, enable);
        }
    }

    public void triggerScreenTap(boolean isDouble) {
        if (mProcessor != null) {
            mProcessor.triggerScreenTap(isDouble);
        }
    }

    public int preProcess(int width, int height, int rotation, @NotNull byte[] bytes, int bytesType) {
        if (!mAuthorized) {
            return -2;
        }
        if (mProcessor == null) {
            mProcessor = IBeautyProcessorKt.createBeautyProcessor();
            mProcessor.initialize(mSTMobileEffectNative,
                    mSTHumanActionNative, mStAnimalNative);
            mProcessor.enableSensor(mContext, isSensorEnable);
        }
        OutputInfo process = mProcessor.process(new InputInfo(
                bytes,
                bytesType,
                null,
                0,
                null,
                0,
                width,
                height,
                rotation == 270,
                rotation,
                System.currentTimeMillis()
        ));
        if (process == null) {
            return -1;
        }
        return process.getTextureId();
    }

    public int preProcess(int width, int height, int rotation,
                          @NotNull byte[] bytes, int bytesType,
                          int textureId, int textureType,
                          float[] textureMatrix,
                          int diffBetweenBytesAndTexture) {
        if (!mAuthorized) {
            return -2;
        }
        if (mProcessor == null) {
            mProcessor = IBeautyProcessorKt.createBeautyProcessor();
            mProcessor.initialize(mSTMobileEffectNative,
                    mSTHumanActionNative, mStAnimalNative);
            mProcessor.enableSensor(mContext, isSensorEnable);
        }
        OutputInfo process = mProcessor.process(new InputInfo(
                bytes,
                bytesType,
                textureId,
                textureType,
                textureMatrix,
                diffBetweenBytesAndTexture,
                width,
                height,
                rotation == 270,
                rotation,
                System.currentTimeMillis()
        ));
        if (process == null) {
            return -1;
        }
        return process.getTextureId();
    }

    @TargetApi(26)
    public int preProcess(int width, int height, int rotation, int textureId, int textureType, float[] textureMatrix) {
        if (!mAuthorized) {
            return -2;
        }
        if (mProcessor == null) {
            mProcessor = IBeautyProcessorKt.createBeautyProcessor();
            mProcessor.initialize(mSTMobileEffectNative,
                    mSTHumanActionNative, mStAnimalNative);
            mProcessor.enableSensor(mContext, isSensorEnable);
        }
        OutputInfo process = mProcessor.process(new InputInfo(
                null,
                STCommonNative.ST_PIX_FMT_NV21,
                textureId,
                textureType,
                textureMatrix,
                1,
                width,
                height,
                rotation == 270,
                rotation,
                System.currentTimeMillis()
        ));
        if (process == null) {
            return -1;
        }
        return process.getTextureId();
    }

}
