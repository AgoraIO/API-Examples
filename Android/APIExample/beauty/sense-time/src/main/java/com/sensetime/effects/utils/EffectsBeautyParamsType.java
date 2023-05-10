package com.sensetime.effects.utils;

import com.ar.effects.EffectsParams;

/**
 * 定义可以美颜的类型
 */
public class EffectsBeautyParamsType {
    public final static int BEAUTIFY_REDDEN_STRENGTH = EffectsParams.BeautyParamsType.BEAUTIFY_REDDEN_STRENGTH.getValue(); // 红润强度, [0,1.0], 0.0不做红润
    public final static int BEAUTIFY_SMOOTH_MODE = EffectsParams.BeautyParamsType.BEAUTIFY_SMOOTH_MODE.getValue();;     /// 磨皮模式, 默认值1.0, 1.0表示对全图磨皮, 0.0表示只对人脸磨皮, 2.0表示精细化磨皮
    public final static int BEAUTIFY_SMOOTH_STRENGTH = EffectsParams.BeautyParamsType.BEAUTIFY_SMOOTH_STRENGTH.getValue();; // 磨皮强度, [0,1.0], 0.0不做磨皮
    public final static int BEAUTIFY_WHITEN_STRENGTH = EffectsParams.BeautyParamsType.BEAUTIFY_WHITEN_STRENGTH.getValue();;    /// 美白强度, [0,1.0], 0.0不做美白
    public final static int BEAUTIFY_ENLARGE_EYE_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_ENLARGE_EYE_RATIO.getValue();;    /// 大眼比例, [0,1.0], 0.0不做大眼效果
    public final static int BEAUTIFY_SHRINK_FACE_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_SHRINK_FACE_RATIO.getValue();;    /// 瘦脸比例, [0,1.0], 0.0不做瘦脸效果
    public final static int BEAUTIFY_SHRINK_JAW_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_SHRINK_JAW_RATIO.getValue();;     /// 小脸比例, [0,1.0], 0.0不做小脸效果
    public final static int BEAUTIFY_CONSTRACT_STRENGTH = EffectsParams.BeautyParamsType.BEAUTIFY_CONSTRACT_STRENGTH.getValue();; // 对比度
    public final static int BEAUTIFY_SATURATION_STRENGTH = EffectsParams.BeautyParamsType.BEAUTIFY_SATURATION_STRENGTH.getValue();; // 饱和度
    public final static int BEAUTIFY_DEHIGHLIGHT_STRENGTH = EffectsParams.BeautyParamsType.BEAUTIFY_DEHIGHLIGHT_STRENGTH.getValue();; // 去高光强度, [0,1.0], 默认值1, 0.0不做高光
    public final static int BEAUTIFY_NARROW_FACE_STRENGTH = EffectsParams.BeautyParamsType.BEAUTIFY_NARROW_FACE_STRENGTH.getValue();; // 窄脸强度, [0,1.0], 默认值0, 0.0不做窄脸
    public final static int BEAUTIFY_ROUND_EYE_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_ROUND_EYE_RATIO.getValue();;       /// 圆眼比例, [0,1.0], 默认值0.0, 0.0不做圆眼
    public final static int BEAUTIFY_SHRINK_CHEEKBONE_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_SHRINK_CHEEKBONE_RATIO.getValue();; /// 瘦颧骨比例， [0, 1.0], 默认值0.0， 0.0不做瘦颧骨
    public final static int BEAUTIFY_SHARPEN_STRENGTH = EffectsParams.BeautyParamsType.BEAUTIFY_SHARPEN_STRENGTH.getValue();;      /// 锐化强度，[0, 1.0], 默认值0.0， 0.0不做锐化
    public final static int BEAUTIFY_THINNER_HEAD_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_THINNER_HEAD_RATIO.getValue();;    /// 小头比例, [0, 1.0], 默认值0.0, 0.0不做小头效果
    public final static int BEAUTIFY_WHITEN2_STRENGTH = EffectsParams.BeautyParamsType.BEAUTIFY_WHITEN2_STRENGTH.getValue();;      /// 美白强度(2), [0,1.0], 默认值0.0, 0.0不做美白
    public final static int BEAUTIFY_WHITEN3_STRENGTH = EffectsParams.BeautyParamsType.BEAUTIFY_WHITEN3_STRENGTH.getValue();;      /// 美白强度(3), [0,1.0], 默认值0.0, 0.0不做美白
    public final static int BEAUTIFY_CLEAR_STRENGTH = EffectsParams.BeautyParamsType.BEAUTIFY_CLEAR_STRENGTH.getValue();;        /// 清晰强度, [0,1.0], 默认值0.0, 0.0不做清晰
    public final static int BEAUTIFY_SMOOTH2_STRENGTH = EffectsParams.BeautyParamsType.BEAUTIFY_SMOOTH2_STRENGTH.getValue();; // 磨皮强度, [0,1.0], 0.0不做磨皮


    public final static int BEAUTIFY_3D_NARROW_NOSE_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_3D_NARROW_NOSE_RATIO.getValue();    // 瘦鼻比例，[0, 1.0], 默认值为0.0，0.0不做瘦鼻
    public final static int BEAUTIFY_3D_NOSE_LENGTH_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_3D_NOSE_LENGTH_RATIO.getValue();     // 鼻子长短比例，[-1, 1], 默认值为0.0, [-1, 0]为短鼻，[0, 1]为长鼻
    public final static int BEAUTIFY_3D_CHIN_LENGTH_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_3D_CHIN_LENGTH_RATIO.getValue();     // 下巴长短比例，[-1, 1], 默认值为0.0，[-1, 0]为短下巴，[0, 1]为长下巴
    public final static int BEAUTIFY_3D_MOUTH_SIZE_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_3D_MOUTH_SIZE_RATIO.getValue();      // 嘴型比例，[-1, 1]，默认值为0.0，[-1, 0]为放大嘴巴，[0, 1]为缩小嘴巴
    public final static int BEAUTIFY_3D_PHILTRUM_LENGTH_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_3D_PHILTRUM_LENGTH_RATIO.getValue(); // 人中长短比例，[-1, 1], 默认值为0.0，[-1, 0]为短人中，[0, 1]为长人中
    public final static int BEAUTIFY_3D_HAIRLINE_HEIGHT_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_3D_HAIRLINE_HEIGHT_RATIO.getValue();   // 发际线高低比例，[-1, 1], 默认值为0.0，[-1, 0]为高发际线，[0, 1]为低发际线
    public final static int BEAUTIFY_3D_THIN_FACE_SHAPE_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_3D_THIN_FACE_SHAPE_RATIO.getValue();   ///  瘦脸型比例， [0,1.0], 默认值0.0, 0.0不做瘦脸型效果
    public final static int BEAUTIFY_3D_EYE_DISTANCE_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_3D_EYE_DISTANCE_RATIO.getValue();             /// 眼距比例，[-1, 1]，默认值为0.0，[-1, 0]为减小眼距，[0, 1]为增加眼距
    public final static int BEAUTIFY_3D_EYE_ANGLE_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_3D_EYE_ANGLE_RATIO.getValue();                /// 眼睛角度调整比例，[-1, 1]，默认值为0.0，[-1, 0]为左眼逆时针旋转，[0, 1]为左眼顺时针旋转，右眼与左眼相对
    public final static int BEAUTIFY_3D_OPEN_CANTHUS_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_3D_OPEN_CANTHUS_RATIO.getValue();             /// 开眼角比例，[0, 1.0]，默认值为0.0， 0.0不做开眼角
    public final static int BEAUTIFY_3D_PROFILE_RHINOPLASTY_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_3D_PROFILE_RHINOPLASTY_RATIO.getValue();      /// 侧脸隆鼻比例，[0, 1.0]，默认值为0.0，0.0不做侧脸隆鼻效果
    public final static int BEAUTIFY_3D_BRIGHT_EYE_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_3D_BRIGHT_EYE_RATIO.getValue();               /// 亮眼比例，[0, 1.0]，默认值为0.0，0.0不做亮眼
    public final static int BEAUTIFY_3D_REMOVE_DARK_CIRCLES_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_3D_REMOVE_DARK_CIRCLES_RATIO.getValue();      /// 去黑眼圈比例，[0, 1.0]，默认值为0.0，0.0不做去黑眼圈
    public final static int BEAUTIFY_3D_REMOVE_NASOLABIAL_FOLDS_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_3D_REMOVE_NASOLABIAL_FOLDS_RATIO.getValue();  /// 去法令纹比例，[0, 1.0]，默认值为0.0，0.0不做去法令纹
    public final static int BEAUTIFY_3D_WHITE_TEETH_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_3D_WHITE_TEETH_RATIO.getValue();              /// 白牙比例，[0, 1.0]，默认值为0.0，0.0不做白牙
    public final static int BEAUTIFY_3D_APPLE_MUSLE_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_3D_APPLE_MUSLE_RATIO.getValue();              /// 苹果肌比例，[0, 1.0]，默认值为0.0，0.0不做苹果肌
    public final static int BEAUTIFY_3D_OPEN_EXTERNAL_CANTHUS_RATIO = EffectsParams.BeautyParamsType.BEAUTIFY_3D_OPEN_EXTERNAL_CANTHUS_RATIO.getValue();             /// 开外眼角比例，[0, 1.0]，默认值为0.0， 0.0不做开外眼角
}