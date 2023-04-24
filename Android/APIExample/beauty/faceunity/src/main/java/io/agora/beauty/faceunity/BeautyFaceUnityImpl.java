package io.agora.beauty.faceunity;

import android.content.Context;

import com.faceunity.FUConfig;
import com.faceunity.core.entity.FUBundleData;
import com.faceunity.core.enumeration.FUTransformMatrixEnum;
import com.faceunity.core.faceunity.FUAIKit;
import com.faceunity.core.faceunity.FURenderKit;
import com.faceunity.core.model.bodyBeauty.BodyBeauty;
import com.faceunity.core.model.facebeauty.FaceBeauty;
import com.faceunity.core.model.facebeauty.FaceBeautyBlurTypeEnum;
import com.faceunity.core.model.facebeauty.FaceBeautyFilterEnum;
import com.faceunity.core.model.makeup.SimpleMakeup;
import com.faceunity.core.model.prop.Prop;
import com.faceunity.core.model.prop.sticker.Sticker;
import com.faceunity.nama.FURenderer;

import java.io.File;

import io.agora.beauty.base.IBeautyFaceUnity;

public class BeautyFaceUnityImpl implements IBeautyFaceUnity {

    public static String BUNDLE_FACE_BEAUTIFICATION = "graphics" + File.separator + "face_beautification.bundle";
    public static String BUNDLE_FACE_MAKEUP = "graphics" + File.separator + "face_makeup.bundle";
    public static final String BUNDLE_BODY_BEAUTY = "graphics" + File.separator + "body_slim.bundle";


    /*渲染控制器*/
    private FURenderKit mFURenderKit = FURenderKit.getInstance();
    private final FURenderer fuRenderer = FURenderer.getInstance();

    private volatile boolean isReleased = false;

    public BeautyFaceUnityImpl(Context context) {
        fuRenderer.setup(context);

        initFaceBeauty();
        initBodyBeauty();
    }

    private void initFaceBeauty() {
        // config face beauty
        FUAIKit.getInstance().faceProcessorSetFaceLandmarkQuality(FUConfig.DEVICE_LEVEL);
        FaceBeauty recommendFaceBeauty = new FaceBeauty(new FUBundleData(BUNDLE_FACE_BEAUTIFICATION));
        recommendFaceBeauty.setFilterName(FaceBeautyFilterEnum.ZIRAN_1);
        recommendFaceBeauty.setFilterIntensity(0.4);
        /*美肤*/
        recommendFaceBeauty.setBlurType(FaceBeautyBlurTypeEnum.FineSkin);
        recommendFaceBeauty.setSharpenIntensity(0.2);
        recommendFaceBeauty.setColorIntensity(0.3);
        recommendFaceBeauty.setRedIntensity(0.3);
        recommendFaceBeauty.setBlurIntensity(4.2);
        /*美型*/
        recommendFaceBeauty.setFaceShapeIntensity(1.0);
        recommendFaceBeauty.setEyeEnlargingIntensityV2(0.4);
        recommendFaceBeauty.setCheekVIntensity(0.5);
        recommendFaceBeauty.setNoseIntensityV2(0.5);
        recommendFaceBeauty.setForHeadIntensityV2(0.3);
        recommendFaceBeauty.setMouthIntensityV2(0.4);
        recommendFaceBeauty.setChinIntensity(0.3);
        mFURenderKit.setFaceBeauty(recommendFaceBeauty);
    }

    private void initBodyBeauty() {
        BodyBeauty bodyBeauty = new BodyBeauty(new FUBundleData(BUNDLE_BODY_BEAUTY));
        bodyBeauty.setBodySlimIntensity(1);
        bodyBeauty.setLegStretchIntensity(1);
        bodyBeauty.setWaistSlimIntensity(1);
        bodyBeauty.setShoulderSlimIntensity(1);
        bodyBeauty.setHipSlimIntensity(1);
        bodyBeauty.setHeadSlimIntensity(1);
        bodyBeauty.setLegSlimIntensity(1);
        mFURenderKit.setBodyBeauty(bodyBeauty);
    }

    @Override
    public int process(byte[] nv21, int width, int height, boolean isFront) {
        if (isReleased) {
            return -1;
        }
        if(isFront){
            fuRenderer.setInputBufferMatrix(FUTransformMatrixEnum.CCROT0);
            fuRenderer.setInputTextureMatrix(FUTransformMatrixEnum.CCROT0);
            fuRenderer.setOutputMatrix(FUTransformMatrixEnum.CCROT0_FLIPVERTICAL);
        }else{
            fuRenderer.setInputBufferMatrix(FUTransformMatrixEnum.CCROT0_FLIPVERTICAL);
            fuRenderer.setInputTextureMatrix(FUTransformMatrixEnum.CCROT0_FLIPVERTICAL);
            fuRenderer.setOutputMatrix(FUTransformMatrixEnum.CCROT0_FLIPVERTICAL);
        }
        return fuRenderer.onDrawFrameInput(nv21, width, height);
    }

    @Override
    public int process(int oesTexId, int width, int height, boolean isFront) {
        if (isReleased) {
            return -1;
        }
        if (isFront) {
            fuRenderer.setInputBufferMatrix(FUTransformMatrixEnum.CCROT0);
            fuRenderer.setInputTextureMatrix(FUTransformMatrixEnum.CCROT0);
            fuRenderer.setOutputMatrix(FUTransformMatrixEnum.CCROT0_FLIPVERTICAL);
        } else {
            fuRenderer.setInputBufferMatrix(FUTransformMatrixEnum.CCROT0_FLIPVERTICAL);
            fuRenderer.setInputTextureMatrix(FUTransformMatrixEnum.CCROT0_FLIPVERTICAL);
            fuRenderer.setOutputMatrix(FUTransformMatrixEnum.CCROT0);
        }
        return fuRenderer.onDrawFrameDualInput(oesTexId, width, height);
    }

    @Override
    public void release() {
        isReleased = true;
        fuRenderer.release();
    }

    @Override
    public void setFaceBeautifyEnable(boolean enable) {
        if (isReleased) {
            return;
        }
        if (mFURenderKit.getFaceBeauty() != null) {
            mFURenderKit.getFaceBeauty().setEnable(enable);
        }
    }

    @Override
    public void setMakeUpEnable(boolean enable) {
        if (isReleased) {
            return;
        }
        if (enable) {
            SimpleMakeup makeup = new SimpleMakeup(new FUBundleData(BUNDLE_FACE_MAKEUP));
            makeup.setCombinedConfig(new FUBundleData("makeup/naicha.bundle"));
            makeup.setMakeupIntensity(1.0);
            mFURenderKit.setMakeup(makeup);
        } else {
            mFURenderKit.setMakeup(null);
        }
    }

    @Override
    public void setStickerEnable(boolean enable) {
        if (isReleased) {
            return;
        }
        if (enable) {
            Prop prop = new Sticker(new FUBundleData("sticker/fashi.bundle"));
            mFURenderKit.getPropContainer().replaceProp(null, prop);
        } else {
            mFURenderKit.getPropContainer().removeAllProp();
        }
    }

    @Override
    public void setBodyBeautifyEnable(boolean enable) {
        if (isReleased) {
            return;
        }
        if (mFURenderKit.getBodyBeauty() != null) {
            mFURenderKit.getBodyBeauty().setEnable(enable);
        }
    }
}
