package com.bytedance.labcv.core.effect;

import android.content.Context;

import java.io.File;

/**
 * Created on 2020/7/30 14:26
 */
public class EffectResourceHelper implements EffectResourceProvider {
    public static final String RESOURCE = "resource";
    private Context mContext;

    public EffectResourceHelper(Context mContext) {
        this.mContext = mContext;
    }

    @Override
    public String getModelPath() {
        return new File(new File(getResourcePath(), "ModelResource.bundle"), "").getAbsolutePath();
    }

    @Override
    public String getComposePath() {
        return new File(new File(getResourcePath(), "ComposeMakeup.bundle"), "ComposeMakeup").getAbsolutePath() + File.separator;
    }

    @Override
    public String getFilterPath() {
        return new File(new File(getResourcePath(), "FilterResource.bundle"), "Filter").getAbsolutePath();
    }

    @Override
    public String getFilterPath(String filter) {
        return new File(getFilterPath(), filter).getAbsolutePath();
    }

    @Override
    public String getStickerPath(String sticker) {
        return new File(new File(new File(getResourcePath()), "StickerResource.bundle"), sticker).getAbsolutePath();
    }

    private String getResourcePath() {
        return mContext.getExternalFilesDir("assets").getAbsolutePath() + File.separator + RESOURCE;
    }
}
