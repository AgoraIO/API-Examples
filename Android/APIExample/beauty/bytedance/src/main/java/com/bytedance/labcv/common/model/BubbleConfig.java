package com.bytedance.labcv.common.model;

import android.graphics.Point;

/** {zh} 
 * Created  on 2021/5/19 8:20 下午
 */
/** {en} 
 * Created on 2021/5/19 8:20 pm
 */

public class BubbleConfig {
    public static final String Key_EffectType = "mEffectType";
    public static final String Key_Performance = "mPerformance";
    public static final String Key_Resolution = "mResolution";
    public static final String Key_EnableBeauty = "mEnableBeauty";
    private EffectType mEffectType;
    private boolean mPerformance;
    private Point mResolution;
    private boolean mEnableBeauty;

    public BubbleConfig() {
    }

    public BubbleConfig(EffectType mEffectType, boolean mPerformance, Point mResolution, boolean mEnableBeauty) {
        this.mEffectType = mEffectType;
        this.mPerformance = mPerformance;
        this.mResolution = mResolution;
        this.mEnableBeauty = mEnableBeauty;
    }

    public EffectType getEffectType() {
        return mEffectType;
    }

    public void setEffectType(EffectType mEffectType) {
        this.mEffectType = mEffectType;
    }

    public boolean isPerformance() {
        return mPerformance;
    }

    public void setPerformance(boolean mPerformance) {
        this.mPerformance = mPerformance;
    }

    public Point getResolution() {
        return mResolution;
    }

    public void setResolution(Point mResolution) {
        this.mResolution = mResolution;
    }

    public boolean isEnableBeauty() {
        return mEnableBeauty;
    }

    public void setEnableBeauty(boolean mEnableBeauty) {
        this.mEnableBeauty = mEnableBeauty;
    }
}
