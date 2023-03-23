package io.agora.beauty.sensetime;

import android.content.Context;

import com.sensetime.effects.STRenderer;
import com.sensetime.effects.utils.FileUtils;
import com.sensetime.stmobile.STCommonNative;
import com.sensetime.stmobile.model.STMobileMakeupType;
import com.sensetime.stmobile.params.STEffectBeautyType;

import java.io.File;

import io.agora.beauty.base.IBeautySenseTime;

public class BeautySenseTimeImpl implements IBeautySenseTime {

    private final STRenderer mSTRenderer;
    private final Context mContext;

    public BeautySenseTimeImpl(Context context) {
        mContext = context.getApplicationContext();
        mSTRenderer = new STRenderer(mContext);
    }

    @Override
    public int process(byte[] nv21, int textureId, int texFormat, int width, int height, int rotation, float[] transformMatrix) {
        return mSTRenderer.preProcess(
                width, height, rotation,
                nv21, STCommonNative.ST_PIX_FMT_NV21,
                textureId, texFormat, transformMatrix
        );
    }

    @Override
    public int process(byte[] nv21, int width, int height, int rotation) {
        return mSTRenderer.preProcess(
                width, height, rotation,
                nv21, STCommonNative.ST_PIX_FMT_NV21
        );
    }

    @Override
    public void release() {
        mSTRenderer.release();
    }

    @Override
    public void setFaceBeautifyEnable(boolean enable) {
        if (enable) {
            mSTRenderer.setBeautyMode(STEffectBeautyType.EFFECT_BEAUTY_BASE_WHITTEN, STEffectBeautyType.WHITENING1_MODE);
            mSTRenderer.setBeautyStrength(STEffectBeautyType.EFFECT_BEAUTY_BASE_WHITTEN, 100f);
            mSTRenderer.setBeautyStrength(STEffectBeautyType.EFFECT_BEAUTY_RESHAPE_ENLARGE_EYE, 1.0f);
        }else{
            mSTRenderer.setBeautyMode(STEffectBeautyType.EFFECT_BEAUTY_BASE_WHITTEN, STEffectBeautyType.WHITENING1_MODE);
            mSTRenderer.setBeautyStrength(STEffectBeautyType.EFFECT_BEAUTY_BASE_WHITTEN, 0f);
            mSTRenderer.setBeautyStrength(STEffectBeautyType.EFFECT_BEAUTY_RESHAPE_ENLARGE_EYE, 0f);
        }
    }

    @Override
    public void setMakeUpEnable(boolean enable) {
        if (enable) {
            setMakeUpItem(STMobileMakeupType.ST_MAKEUP_TYPE_LIP, "makeup_lip" + File.separator + "12自然.zip", 1);
        } else {
            setMakeUpItem(STMobileMakeupType.ST_MAKEUP_TYPE_LIP, null, 0);
        }
    }

    @Override
    public void setStickerEnable(boolean enable) {
        setStickerItem("sticker_face_shape" + File.separator + "lianxingface.zip", enable);
    }

    @Override
    public void setFilterEnable(boolean enable) {
        if (enable) {
            setFilterItem("filter_portrait" + File.separator + "filter_style_babypink.model", 1);
        } else {
            setFilterItem("filter_portrait" + File.separator + "filter_style_babypink.model", 0);
        }
    }

    private void setMakeUpItem(int type, String typePath, float strength){
        if (typePath != null) {
            String[] split = typePath.split(File.separator);
            String className = split[0];
            String fileName = split[1];
            String _path = FileUtils.getFilePath(mContext, className + File.separator + fileName);
            FileUtils.copyFileIfNeed(mContext, fileName, className);
            mSTRenderer.setMakeupForType(type, _path);
            mSTRenderer.setMakeupStrength(type, strength);
        } else {
            mSTRenderer.removeMakeupByType(type);
        }
    }

    private void setStickerItem(String path, boolean attach){
        String[] split = path.split(File.separator);
        String className = split[0];
        String fileName = split[1];
        String _path = FileUtils.getFilePath(mContext, className + File.separator + fileName);
        FileUtils.copyFileIfNeed(mContext, fileName, className);
        if (mSTRenderer != null) {
            if (!attach) {
                mSTRenderer.removeSticker(_path);
            } else {
                mSTRenderer.changeSticker(_path);
            }
        }
    }

    private void setFilterItem(String filterPath, float strength){
        String[] split = filterPath.split(File.separator);
        String className = split[0];
        String fileName = split[1];
        String filterName = split[1].split("_")[2].split("\\.")[0];
        String path = FileUtils.getFilePath(mContext, className + File.separator + fileName);
        FileUtils.copyFileIfNeed(mContext, fileName, className);
        mSTRenderer.setFilterStyle(className, filterName, path);
        mSTRenderer.setFilterStrength(strength);
    }
}
