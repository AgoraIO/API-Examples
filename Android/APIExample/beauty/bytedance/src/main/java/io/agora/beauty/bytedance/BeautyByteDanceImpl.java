package io.agora.beauty.bytedance;

import android.content.Context;

import com.bytedance.labcv.core.Config;
import com.bytedance.labcv.core.effect.EffectManager;
import com.bytedance.labcv.core.effect.EffectResourceHelper;
import com.bytedance.labcv.core.util.ImageUtil;
import com.bytedance.labcv.effectsdk.BytedEffectConstants;

import java.io.File;
import java.nio.ByteBuffer;

import io.agora.beauty.base.IBeautyByteDance;

public class BeautyByteDanceImpl implements IBeautyByteDance {

    private final String BEAUTY_NODE = "beauty_Android_lite";
    private final String MAKEUP_NODE = "reshape_lite";
    private final String BODY_NODE = "body/allslim";

    private final Context mContext;
    private final String mResourcePath;
    private volatile boolean isReleased = false;

    private EffectManager mEffectManager;
    private ImageUtil mImageUtil;

    // sdk 初始化标记，仅用于用来标记SDK的初始化。
    private volatile boolean resourceReady = false;
    private volatile boolean sdkIsInit = false;

    private final AssetsCopyHelper assetsCopyHelper;

    private ByteBuffer yByteBuffer;
    private ByteBuffer vuByteBuffer;

    public BeautyByteDanceImpl(Context context) {
        mContext = context;
        mResourcePath = mContext.getExternalFilesDir("assets").getAbsolutePath() + File.separator + "resource";
        assetsCopyHelper = new AssetsCopyHelper(context, "resource", mContext.getExternalFilesDir("assets").getAbsolutePath());
        assetsCopyHelper.start(() -> resourceReady = true);
        cvSdkInit();
    }

    /**
     * EffectManager的初始化，包括各种资源路径配置
     */
    private void cvSdkInit() {
        String licensePath = new File(new File(mResourcePath, "LicenseBag.bundle"), Config.LICENSE_NAME).getAbsolutePath();
        mEffectManager = new EffectManager(mContext, new EffectResourceHelper(mContext), licensePath);
        mImageUtil = new ImageUtil();
    }

    private void configSdkDefault() {
        if (sdkIsInit) {
            return;
        }
        // 必须在gl 线程中运行。
        mEffectManager.init();
        mEffectManager.setComposeNodes(new String[]{BEAUTY_NODE, MAKEUP_NODE, BODY_NODE});
        sdkIsInit = true;
    }

    @Override
    public int process(byte[] nv21, int width, int height, int rotation) {
        if (isReleased) {
            return -1;
        }
        if (!resourceReady) {
            return -1;
        }
        configSdkDefault();
        // 是否为前置摄像头
        mEffectManager.setCameraPosition(true);
        // 生成目标承载纹理
        int dstTexture = mImageUtil.prepareTexture(width, height);
        // NV21转2D纹理
        ImageUtil.Transition transition = new ImageUtil.Transition();
        if(rotation == 270){
             transition.scale(1.f, -1.0f);
        }

        int yLength = width * height;
        if (yByteBuffer == null || yByteBuffer.capacity() != yLength) {
            if (yByteBuffer != null) {
                yByteBuffer.clear();
            }
            yByteBuffer = ByteBuffer.allocateDirect(yLength);
        }
        int vuLength = width * height / 2;
        if(vuByteBuffer == null || vuByteBuffer.capacity() != vuLength){
            if (vuByteBuffer != null) {
                vuByteBuffer.clear();
            }
            vuByteBuffer = ByteBuffer.allocateDirect(vuLength);
        }
        yByteBuffer.position(0);
        yByteBuffer.put(nv21, 0, yLength);
        yByteBuffer.position(0);
        vuByteBuffer.put(nv21, yLength, vuLength);
        vuByteBuffer.position(0);
        int texture2d = mImageUtil.transferYUVToTexture(yByteBuffer, vuByteBuffer, width, height, transition);

        // CV SDK 特效处理
        boolean process = mEffectManager.process(texture2d, dstTexture, width, height,
                BytedEffectConstants.Rotation.CLOCKWISE_ROTATE_0,
                System.currentTimeMillis());
        if (!process) {
            return -1;
        }
        if(rotation == 90){
            transition.scale(1.f, -1.0f);
        }
        return mImageUtil.transferTextureToTexture(dstTexture, BytedEffectConstants.TextureFormat.Texure2D, BytedEffectConstants.TextureFormat.Texure2D,
                width, height, transition);
    }

    @Override
    public int process(int oesTexId, int width, int height, int rotation) {
        if (isReleased) {
            return -1;
        }
        if (!resourceReady) {
            return -1;
        }
        configSdkDefault();
        // 是否为前置摄像头
        mEffectManager.setCameraPosition(true);
        // 生成目标承载纹理
        int dstTexture = mImageUtil.prepareTexture(width, height);
        // OES 纹理转2D纹理
        ImageUtil.Transition transition = new ImageUtil.Transition();
        if(rotation == 270){
            transition.scale(1.f, -1.0f);
        }
        int texture2d = mImageUtil.transferTextureToTexture(oesTexId,
                BytedEffectConstants.TextureFormat.Texture_Oes,
                BytedEffectConstants.TextureFormat.Texure2D,
                width, height, transition);
        // CV SDK 特效处理
        boolean process = mEffectManager.process(texture2d, dstTexture, width, height,
                BytedEffectConstants.Rotation.CLOCKWISE_ROTATE_0,
                System.currentTimeMillis());
        if (!process) {
            return -1;
        }

        return dstTexture;
    }

    @Override
    public void release() {
        isReleased = true;
        sdkIsInit = false;
        mImageUtil.release();
        assetsCopyHelper.stop();
        if(yByteBuffer != null){
            yByteBuffer.clear();
            yByteBuffer = null;
        }
        if(vuByteBuffer != null){
            vuByteBuffer.clear();
            vuByteBuffer = null;
        }
    }

    @Override
    public void setFaceBeautifyEnable(boolean enable) {
        if (isReleased) {
            return;
        }
        if (enable) {
            mEffectManager.updateComposerNodeIntensity(BEAUTY_NODE, "smooth", 1.0f);// 磨皮0.8f
            mEffectManager.updateComposerNodeIntensity(BEAUTY_NODE, "whiten", 1.0f);// 美白 0.3f
            mEffectManager.updateComposerNodeIntensity(BEAUTY_NODE, "sharp", 1.0f);// 锐化 0.32f
        } else {
            mEffectManager.updateComposerNodeIntensity(BEAUTY_NODE, "smooth", 0.0f);// 磨皮0.8f
            mEffectManager.updateComposerNodeIntensity(BEAUTY_NODE, "whiten", 0.0f);// 美白 0.3f
            mEffectManager.updateComposerNodeIntensity(BEAUTY_NODE, "sharp", 0.0f);// 锐化 0.32f
        }
    }

    @Override
    public void setMakeUpEnable(boolean enable) {
        if (isReleased) {
            return;
        }
        if (enable) {
            mEffectManager.updateComposerNodeIntensity(MAKEUP_NODE, "Internal_Deform_Overall", 1.0f);//瘦脸 0.5f
            mEffectManager.updateComposerNodeIntensity(MAKEUP_NODE, "Internal_Deform_Eye", 3.3f);//大眼 0.3f
        } else {
            mEffectManager.updateComposerNodeIntensity(MAKEUP_NODE, "Internal_Deform_Overall", 0.0f);//瘦脸 0.5f
            mEffectManager.updateComposerNodeIntensity(MAKEUP_NODE, "Internal_Deform_Eye", 0.0f);//大眼 0.3f
        }
    }

    @Override
    public void setStickerEnable(boolean enable) {
        if (isReleased) {
            return;
        }
        if (enable) {
            String stickerPath = new EffectResourceHelper(mContext).getStickerPath("/stickers/wochaotian");
            mEffectManager.setStickerAbs(stickerPath);
        } else {
            mEffectManager.setStickerAbs(null);
        }
    }

    @Override
    public void setBodyBeautifyEnable(boolean enable) {
        if (enable) {
            mEffectManager.updateComposerNodeIntensity(BODY_NODE, "BEF_BEAUTY_BODY_THIN", 1.0f);
            mEffectManager.updateComposerNodeIntensity(BODY_NODE, "BEF_BEAUTY_BODY_LONG_LEG", 1.0f);
            mEffectManager.updateComposerNodeIntensity(BODY_NODE, "BEF_BEAUTY_BODY_SHRINK_HEAD", 1.0f);
        } else {
            mEffectManager.updateComposerNodeIntensity(BODY_NODE, "BEF_BEAUTY_BODY_THIN", 0.0f);
            mEffectManager.updateComposerNodeIntensity(BODY_NODE, "BEF_BEAUTY_BODY_LONG_LEG", 0.0f);
            mEffectManager.updateComposerNodeIntensity(BODY_NODE, "BEF_BEAUTY_BODY_SHRINK_HEAD", 0.0f);
        }
    }

}
