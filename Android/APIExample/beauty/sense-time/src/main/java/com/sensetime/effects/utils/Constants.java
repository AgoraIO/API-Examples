package com.sensetime.effects.utils;

import android.content.Context;
import android.util.DisplayMetrics;

public class Constants {

    public static final String LICENSE_NAME = "license/LICENSE.lic";
    public static final boolean EFFECTS_LOG_SWITCH = true;

    public static final int[] beautyTypes = {
            EffectsBeautyParamsType.BEAUTIFY_REDDEN_STRENGTH,
            EffectsBeautyParamsType.BEAUTIFY_SMOOTH_STRENGTH,
            EffectsBeautyParamsType.BEAUTIFY_WHITEN_STRENGTH,
            EffectsBeautyParamsType.BEAUTIFY_ENLARGE_EYE_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_SHRINK_FACE_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_SHRINK_JAW_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_CONSTRACT_STRENGTH,
            EffectsBeautyParamsType.BEAUTIFY_SATURATION_STRENGTH,
            EffectsBeautyParamsType.BEAUTIFY_DEHIGHLIGHT_STRENGTH,
            EffectsBeautyParamsType.BEAUTIFY_NARROW_FACE_STRENGTH,
            EffectsBeautyParamsType.BEAUTIFY_3D_NARROW_NOSE_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_3D_NOSE_LENGTH_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_3D_CHIN_LENGTH_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_3D_MOUTH_SIZE_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_3D_PHILTRUM_LENGTH_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_3D_HAIRLINE_HEIGHT_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_3D_THIN_FACE_SHAPE_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_3D_EYE_DISTANCE_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_3D_EYE_ANGLE_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_3D_OPEN_CANTHUS_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_3D_PROFILE_RHINOPLASTY_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_3D_BRIGHT_EYE_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_3D_REMOVE_DARK_CIRCLES_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_3D_REMOVE_NASOLABIAL_FOLDS_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_3D_WHITE_TEETH_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_3D_APPLE_MUSLE_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_ROUND_EYE_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_SHRINK_CHEEKBONE_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_SHARPEN_STRENGTH,
            EffectsBeautyParamsType.BEAUTIFY_3D_OPEN_EXTERNAL_CANTHUS_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_THINNER_HEAD_RATIO,
            EffectsBeautyParamsType.BEAUTIFY_CLEAR_STRENGTH,
            EffectsBeautyParamsType.BEAUTIFY_WHITEN3_STRENGTH,//新美白
            EffectsBeautyParamsType.BEAUTIFY_SMOOTH2_STRENGTH,
            EffectsBeautyParamsType.BEAUTIFY_WHITEN_STRENGTH,
    };

    public static final int INDEX_OF_BEAUTIFY_REDDEN_STRENGTH = 0;
    public static final int INDEX_OF_BEAUTIFY_SMOOTH_STRENGTH = 1;
    public static final int INDEX_OF_BEAUTIFY_WHITEN_STRENGTH = 2;
    public static final int INDEX_OF_BEAUTIFY_ENLARGE_EYE_RATIO = 3;//大眼
    public static final int INDEX_OF_BEAUTIFY_SHRINK_FACE_RATIO = 4;//瘦脸
    public static final int INDEX_OF_BEAUTIFY_SHRINK_JAW_RATIO = 5;
    public static final int INDEX_OF_BEAUTIFY_CONSTRACT_STRENGTH = 6;
    public static final int INDEX_OF_BEAUTIFY_SATURATION_STRENGTH = 7;
    public static final int INDEX_OF_BEAUTIFY_DEHIGHLIGHT_STRENGTH = 8;
    public static final int INDEX_OF_BEAUTIFY_NARROW_FACE_STRENGTH = 9;
    public static final int INDEX_OF_BEAUTIFY_3D_NARROW_NOSE_RATIO = 10;
    public static final int INDEX_OF_BEAUTIFY_3D_NOSE_LENGTH_RATIO = 11;
    public static final int INDEX_OF_BEAUTIFY_3D_CHIN_LENGTH_RATIO = 12;
    public static final int INDEX_OF_BEAUTIFY_3D_MOUTH_SIZE_RATIO = 13;
    public static final int INDEX_OF_BEAUTIFY_3D_PHILTRUM_LENGTH_RATIO = 14;
    public static final int INDEX_OF_BEAUTIFY_3D_HAIRLINE_HEIGHT_RATIO = 15;
    public static final int INDEX_OF_BEAUTIFY_3D_THIN_FACE_SHAPE_RATIO = 16;
    public final static int INDEX_OF_BEAUTIFY_3D_EYE_DISTANCE_RATIO = 17;
    public final static int INDEX_OF_BEAUTIFY_3D_EYE_ANGLE_RATIO = 18;
    public final static int INDEX_OF_BEAUTIFY_3D_OPEN_CANTHUS_RATIO = 19;
    public final static int INDEX_OF_BEAUTIFY_3D_PROFILE_RHINOPLASTY_RATIO = 20;
    public final static int INDEX_OF_BEAUTIFY_3D_BRIGHT_EYE_RATIO = 21;
    public final static int INDEX_OF_BEAUTIFY_3D_REMOVE_DARK_CIRCLES_RATIO = 22;
    public final static int INDEX_OF_BEAUTIFY_3D_REMOVE_NASOLABIAL_FOLDS_RATIO = 23;
    public final static int INDEX_OF_BEAUTIFY_3D_WHITE_TEETH_RATIO = 24;
    public final static int INDEX_OF_BEAUTIFY_3D_APPLE_MUSLE_RATIO = 25;
    public final static int INDEX_OF_BEAUTIFY_ROUND_EYE_RATIO = 26;
    public final static int INDEX_OF_BEAUTIFY_SHRINK_CHEEKBONE_RATIO = 27;
    public final static int INDEX_OF_BEAUTIFY_SHARPEN_STRENGTH = 28;
    public final static int INDEX_OF_BEAUTIFY_3D_OPEN_EXTERNAL_CANTHUS_RATIO = 29;
    public final static int INDEX_OF_BEAUTIFY_THINNER_HEAD_RATIO = 30;
    public final static int INDEX_OF_BEAUTIFY_CLEAR_STRENGTH = 31;
    public static final int INDEX_OF_BEAUTIFY_WHITEN3_STRENGTH = 32;
    public static final int INDEX_OF_BEAUTIFY_SMOOTH2_STRENGTH = 33;
    public static final int INDEX_OF_BEAUTIFY_NEW_WHITEN3_STRENGTH = 34;

    public static final int MAKEUP_LIP = EffectsMakeupType.MAKEUP_TYPE_LIP;
    public static final int MAKEUP_HIGHLIGHT = EffectsMakeupType.MAKEUP_TYPE_HIGHLIGHT;
    public static final int MAKEUP_BLUSH = EffectsMakeupType.MAKEUP_TYPE_BLUSH;
    public static final int MAKEUP_BROW = EffectsMakeupType.MAKEUP_TYPE_BROW;
    public static final int MAKEUP_EYE = EffectsMakeupType.MAKEUP_TYPE_EYE;
    public static final int MAKEUP_EYELINER = EffectsMakeupType.MAKEUP_TYPE_EYELINER;
    public static final int MAKEUP_EYELASH = EffectsMakeupType.MAKEUP_TYPE_EYELASH;
    public static final int MAKEUP_EYEBALL = EffectsMakeupType.MAKEUP_TYPE_EYEBALL;
    public static final int MAKEUP_HAIR_DYE = EffectsMakeupType.MAKEUP_TYPE_HAIR_DYE;
    public static final int MAKEUP_ALL = EffectsMakeupType.MAKEUP_TYPE_ALL;

    public static final int MAKEUP_TYPE_COUNT = 11;

    /**
     * 屏幕宽高
     */
    public static int screenWidth;
    public static int screenHeight;
    /**
     * 三种画幅的具体显示尺寸
     */
    public static int mode_por_width_9_16;
    public static int mode_por_height_9_16;
    public static int mode_por_width_1_1;
    public static int mode_por_height_1_1;
    public static int mode_por_width_16_9;
    public static int mode_por_height_16_9;


    public static void init(Context context) {
        DisplayMetrics mDisplayMetrics = context.getResources()
                .getDisplayMetrics();
        screenWidth = mDisplayMetrics.widthPixels;
        screenHeight = mDisplayMetrics.heightPixels;
        mode_por_width_9_16 = screenWidth;
        mode_por_height_9_16 = screenHeight;
        mode_por_width_1_1 = screenWidth;
        mode_por_height_1_1 = screenWidth;
        mode_por_width_16_9 = screenWidth;
        mode_por_height_16_9 = screenWidth / 16 * 9;
    }
}
