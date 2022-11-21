package com.bytedance.labcv.core.effect;

/**
 * Created on 5/8/21 10:30 AM
 */
public interface EffectResourceProvider {
    String getModelPath();
    String getComposePath();
    String getFilterPath();
    String getFilterPath(String filter);
    String getStickerPath(String sticker);
}
