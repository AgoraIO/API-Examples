package io.agora.beauty.sensetime;

import android.content.Context;

import com.sensetime.effects.utils.EffectsBeautyParamsType;
import com.sensetime.effects.utils.EffectsMakeupType;
import com.sensetime.effects.utils.FileUtils;

import java.io.File;

import io.agora.beauty.base.IBeautySenseTime;
import io.agora.rtc2.video.IVideoFrameObserver;

public class BeautySenseTimeImpl implements IBeautySenseTime {

    private final AgoraSenseTimeBeauty senseTimeBeauty;
    private final Context mContext;

    public BeautySenseTimeImpl(Context context) {
        senseTimeBeauty = new AgoraSenseTimeBeauty(context);
        mContext = context.getApplicationContext();
    }

    @Override
    public IVideoFrameObserver getVideoFrameObserver() {
        return senseTimeBeauty;
    }

    @Override
    public void release() {
        senseTimeBeauty.release();
    }

    @Override
    public void setFaceBeautifyEnable(boolean enable) {
        if (enable) {
            senseTimeBeauty.getEffectsRender().setBeautyParams(EffectsBeautyParamsType.BEAUTIFY_SMOOTH_MODE, 2.0f);
            senseTimeBeauty.getEffectsRender().setBeautyParams(EffectsBeautyParamsType.BEAUTIFY_WHITEN_STRENGTH, 0.8f);
            senseTimeBeauty.getEffectsRender().setBeautyParams(EffectsBeautyParamsType.BEAUTIFY_SMOOTH_STRENGTH, 0.8f);
            senseTimeBeauty.getEffectsRender().setBeautyParams(EffectsBeautyParamsType.BEAUTIFY_SHARPEN_STRENGTH, 0.8f);
        }else{
            senseTimeBeauty.getEffectsRender().setBeautyParams(EffectsBeautyParamsType.BEAUTIFY_WHITEN_STRENGTH, 0f);
            senseTimeBeauty.getEffectsRender().setBeautyParams(EffectsBeautyParamsType.BEAUTIFY_SMOOTH_STRENGTH, 0f);
            senseTimeBeauty.getEffectsRender().setBeautyParams(EffectsBeautyParamsType.BEAUTIFY_SHARPEN_STRENGTH, 0f);
        }
    }

    @Override
    public void setMakeUpEnable(boolean enable) {
        if (enable) {
            String typePath = "makeup_lip" + File.separator + "12自然.zip";
            int type = EffectsMakeupType.MAKEUP_TYPE_LIP;
            float strength = 1.0f;

            String[] split = typePath.split(File.separator);
            String className = split[0];
            String fileName = split[1];
            String _path = FileUtils.getFilePath(mContext, className + File.separator + fileName);
            FileUtils.copyFileIfNeed(mContext, fileName, className);
            senseTimeBeauty.getEffectsRender().setMakeup(type, _path);
            senseTimeBeauty.getEffectsRender().setMakeupStrength(type, strength);
        } else {
            senseTimeBeauty.getEffectsRender().removeAllMakeup();
        }
    }

    @Override
    public void setStickerEnable(boolean enable) {
        if(enable){
            String path = "sticker_2d" + File.separator + "dv.zip";
            String[] split = path.split(File.separator);
            String className = split[0];
            String fileName = split[1];
            String _path = FileUtils.getFilePath(mContext, className + File.separator + fileName);
            FileUtils.copyFileIfNeed(mContext, fileName, className);
            senseTimeBeauty.getEffectsRender().addSticker(_path);
        }else{
            senseTimeBeauty.getEffectsRender().clearSticker();
        }
    }

    @Override
    public void setFilterEnable(boolean enable) {
        if (enable) {
            String filterPath = "filter_portrait" + File.separator + "filter_style_babypink.model";
            float strength = 1.0f;

            String[] split = filterPath.split(File.separator);
            String className = split[0];
            String fileName = split[1];
            String path = FileUtils.getFilePath(mContext, className + File.separator + fileName);
            FileUtils.copyFileIfNeed(mContext, fileName, className);
            senseTimeBeauty.getEffectsRender().setFilterStyle(path);
            senseTimeBeauty.getEffectsRender().setFilterStrength(strength);
        } else {
            senseTimeBeauty.getEffectsRender().setFilterStrength(0.f);
        }
    }
}
