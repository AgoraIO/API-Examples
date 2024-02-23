package io.agora.api.example.examples.advanced.beauty

import android.content.Context
import android.util.Log
import com.softsugar.stmobile.STCommonNative
import com.softsugar.stmobile.STMobileAuthentificationNative
import com.softsugar.stmobile.STMobileEffectNative
import com.softsugar.stmobile.STMobileEffectParams
import com.softsugar.stmobile.STMobileHumanActionNative
import com.softsugar.stmobile.params.STEffectBeautyType
import com.softsugar.stmobile.params.STHumanActionParamsType
import io.agora.api.example.utils.FileKtUtils
import java.util.concurrent.Executors

/**
 * Sense time beauty s d k
 *
 * @constructor Create empty Sense time beauty s d k
 */
object SenseTimeBeautySDK {
    private val TAG = "SenseTimeBeautySDK"

    private val resourcePath = "beauty_sensetime"
    private val humanActionCreateConfig = 0
    private val packageMap = mutableMapOf<String, Int>()

    private val MODEL_106 = "models/M_SenseME_Face_Video_Template_p_3.9.0.3.model" // 106
    private val MODEL_FACE_EXTRA = "models/M_SenseME_Face_Extra_Advanced_Template_p_2.0.0.model" // 282
    private val MODEL_AVATAR_HELP = "models/M_SenseME_Avatar_Help_p_2.3.7.model" // avatar人脸驱动
    private val MODEL_LIPS_PARSING = "models/M_SenseME_MouthOcclusion_p_1.3.0.1.model" // 嘴唇分割
    private val MODEL_HAND = "models/M_SenseME_Hand_p_6.0.8.1.model" // 手势
    private val MODEL_SEGMENT = "models/M_SenseME_Segment_Figure_p_4.14.1.1.model" // 前后背景分割
    private val MODEL_SEGMENT_HAIR = "models/M_SenseME_Segment_Hair_p_4.4.0.model" // 头发分割
    private val MODEL_FACE_OCCLUSION = "models/M_SenseME_FaceOcclusion_p_1.0.7.1.model" // 妆容遮挡
    private val MODEL_SEGMENT_SKY = "models/M_SenseME_Segment_Sky_p_1.1.0.1.model" // 天空分割
    // private val MODEL_SEGMENT_SKIN = "models/M_SenseME_Segment_Skin_p_1.0.1.1.model" // 皮肤分割
    private val MODEL_3DMESH = "models/M_SenseME_3DMesh_Face2396pt_280kpts_Ear_p_1.1.0v2.model" // 3DMesh
    // private val MODEL_HEAD_P_EAR = "models/M_SenseME_Ear_p_1.0.1.1.model" // 搭配 mesh 耳朵模型
    private val MODEL_360HEAD_INSTANCE = "models/M_SenseME_3Dmesh_360Head2396pt_p_1.0.0.1.model" // 360度人头mesh
    private val MODEL_FOOT = "models/M_SenseME_Foot_p_2.10.7.model" // 鞋子检测模型
    private val MODEL_PANT = "models/M_SenseME_Segment_Trousers_p_1.1.10.model" // 裤腿的检测
    private val MODEL_WRIST = "models/M_SenseME_Wrist_p_1.4.0.model" // 试表
    private val MODEL_CLOTH = "models/M_SenseME_Segment_Clothes_p_1.0.2.2.model" // 衣服分割
    private val MODEL_HEAD_INSTANCE = "models/M_SenseME_Segment_Head_Instance_p_1.1.0.1.model" // 实例分割版本
    // private val MODEL_HEAD_P_INSTANCE = "models/M_SenseME_Head_p_1.3.0.1.model" // 360度人头-头部模型
    private val MODEL_NAIL = "models/M_SenseME_Nail_p_2.4.0.model" // 指甲检测

    private val workerThread = Executors.newSingleThreadExecutor()

    /**
     * Mobile effect native
     */
    val mobileEffectNative = STMobileEffectNative()

    /**
     * Human action native
     */
    val humanActionNative = STMobileHumanActionNative()


    /**
     * Init beauty s d k
     *
     * @param context
     */
    fun initBeautySDK(context: Context){
        workerThread.submit {
            checkLicense(context)
            initHumanAction(context)
        }
    }

    /**
     * Init mobile effect
     *
     * @param context
     */
    fun initMobileEffect(context: Context){
        val result =
            mobileEffectNative.createInstance(context, STMobileEffectNative.EFFECT_CONFIG_NONE)
        mobileEffectNative.setParam(STMobileEffectParams.EFFECT_PARAM_QUATERNION_SMOOTH_FRAME, 5f)
        Log.d(TAG, "SenseTime >> STMobileEffectNative create result : $result")
    }

    /**
     * Un init mobile effect
     *
     */
    fun unInitMobileEffect(){
        mobileEffectNative.destroyInstance()
    }

    /**
     * Release
     *
     */
    fun release() {
        mobileEffectNative.destroyInstance()
    }

    private fun checkLicense(context: Context) {
        val license = FileKtUtils.getAssetsString(
            context,
            "$resourcePath/license/SenseME.lic"
        )
        val activeCode = STMobileAuthentificationNative.generateActiveCodeFromBuffer(
            context,
            license,
            license.length
        )
        val success = activeCode.isNotEmpty()
        if (success) {
            Log.d(TAG, "SenseTime >> checkLicense successfully!")
        } else {
            Log.e(TAG, "SenseTime >> checkLicense failed!")
        }
    }

    private fun initHumanAction(context: Context){
        val assets = context.assets
        val result = humanActionNative.createInstanceFromAssetFile(
            "$resourcePath/$MODEL_106",
            humanActionCreateConfig,
            assets
        )
        Log.d(TAG, "SenseTime >> STMobileHumanActionNative create result : $result")

        if(result != 0){
            return
        }

        humanActionNative.addSubModelFromAssetFile("$resourcePath/$MODEL_HAND", assets)
        humanActionNative.addSubModelFromAssetFile("$resourcePath/$MODEL_SEGMENT", assets)
        humanActionNative.addSubModelFromAssetFile("$resourcePath/$MODEL_FACE_EXTRA", assets)
        humanActionNative.addSubModelFromAssetFile("$resourcePath/$MODEL_SEGMENT_HAIR", assets)
        humanActionNative.addSubModelFromAssetFile("$resourcePath/$MODEL_LIPS_PARSING", assets)
        humanActionNative.addSubModelFromAssetFile("$resourcePath/$MODEL_FACE_OCCLUSION", assets)
        humanActionNative.addSubModelFromAssetFile("$resourcePath/$MODEL_SEGMENT_SKY", assets)
        humanActionNative.addSubModelFromAssetFile("$resourcePath/$MODEL_AVATAR_HELP", assets)
        humanActionNative.addSubModelFromAssetFile("$resourcePath/$MODEL_FOOT", assets)
        humanActionNative.addSubModelFromAssetFile("$resourcePath/$MODEL_PANT", assets)
        humanActionNative.addSubModelFromAssetFile("$resourcePath/$MODEL_3DMESH", assets)
        humanActionNative.addSubModelFromAssetFile("$resourcePath/$MODEL_WRIST", assets)
        humanActionNative.addSubModelFromAssetFile("$resourcePath/$MODEL_CLOTH", assets)
        humanActionNative.addSubModelFromAssetFile("$resourcePath/$MODEL_HEAD_INSTANCE", assets)
        humanActionNative.addSubModelFromAssetFile("$resourcePath/$MODEL_360HEAD_INSTANCE", assets)
        humanActionNative.addSubModelFromAssetFile("$resourcePath/$MODEL_NAIL", assets)

        // 背景分割羽化程度[0,1](默认值0.35),0 完全不羽化,1羽化程度最高,在strenth较小时,羽化程度基本不变.值越大,前景与背景之间的过度边缘部分越宽.
        humanActionNative.setParam(STHumanActionParamsType.ST_HUMAN_ACTION_PARAM_BACKGROUND_BLUR_STRENGTH, 0.35f)
        // 设置face mesh结果输出坐标系,(0: 屏幕坐标系， 1：3d世界坐标系， 2:3d摄像机坐标系,是摄像头透视投影坐标系, 原点在摄像机 默认是0）
        humanActionNative.setParam(STHumanActionParamsType.ST_HUMAN_ACTION_PARAM_FACE_MESH_OUTPUT_FORMAT, 1.0f)
        // 设置mesh渲染模式
        humanActionNative.setParam(STHumanActionParamsType.ST_HUMAN_ACTION_PARAM_MESH_MODE, STCommonNative.MESH_CONFIG.toFloat())
        // 设置人头实例分割
        humanActionNative.setParam(STHumanActionParamsType.ST_HUMAN_ACTION_PARAM_HEAD_SEGMENT_INSTANCE, 1.0f)
    }

    /**
     * Set make up item
     *
     * @param context
     * @param type
     * @param path
     * @param strength
     */
    fun setMakeUpItem(context: Context, type: Int, path: String = "", strength: Float = 1.0f) {
        if (path.isNotEmpty()) {
            val assets = context.assets
            mobileEffectNative.setBeautyFromAssetsFile(type, "$resourcePath/$path", assets)
            mobileEffectNative.setBeautyStrength(type, strength)
        } else {
            mobileEffectNative.setBeauty(type, null)
        }
    }

    /**
     * Set sticker item
     *
     * @param context
     * @param path
     * @param attach
     */
    fun setStickerItem(context: Context, path: String, attach: Boolean) {
        if(attach){
            val assets = context.assets
            packageMap[path] = mobileEffectNative.changePackageFromAssetsFile("$resourcePath/$path", assets)
        }else{
            packageMap.remove(path)?.let {
                mobileEffectNative.removeEffect(it)
            }
        }
    }

    /**
     * Clean sticker
     *
     */
    fun cleanSticker(){
        packageMap.values.forEach {
            mobileEffectNative.removeEffect(it)
        }
        packageMap.clear()
    }

    /**
     * Set beauty
     *
     * @param smooth
     * @param whiten
     * @param thinFace
     * @param enlargeEye
     * @param redden
     * @param shrinkCheekbone
     * @param shrinkJawbone
     * @param whiteTeeth
     * @param hairlineHeight
     * @param narrowNose
     * @param mouthSize
     * @param chinLength
     * @param brightEye
     * @param darkCircles
     * @param nasolabialFolds
     * @param saturation
     * @param contrast
     * @param sharpen
     * @param clear
     */
    fun setBeauty(
        smooth: Float? = null,
        whiten: Float? = null,
        thinFace: Float? = null,
        enlargeEye: Float? = null,
        redden: Float? = null,
        shrinkCheekbone: Float? = null,
        shrinkJawbone: Float? = null,
        whiteTeeth: Float? = null,
        hairlineHeight: Float? = null,
        narrowNose: Float? = null,
        mouthSize: Float? = null,
        chinLength: Float? = null,
        brightEye: Float? = null,
        darkCircles: Float? = null,
        nasolabialFolds: Float? = null,

        saturation: Float? = null,
        contrast: Float? = null,
        sharpen: Float? = null,
        clear: Float? = null
    ){
        val effectNative = mobileEffectNative
        // 锐化
        sharpen?.let { effectNative.setBeautyStrength(STEffectBeautyType.EFFECT_BEAUTY_TONE_SHARPEN, it) }

        // 清晰度
        clear?.let { effectNative.setBeautyStrength(STEffectBeautyType.EFFECT_BEAUTY_TONE_CLEAR, it) }

        // 磨皮
        smooth?.let {
            effectNative.setBeautyMode(
                STEffectBeautyType.EFFECT_BEAUTY_BASE_FACE_SMOOTH,
                STEffectBeautyType.SMOOTH2_MODE
            )
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_BASE_FACE_SMOOTH,
                it
            )
        }

        // 美白
        whiten?.let {
            effectNative.setBeautyMode(
                STEffectBeautyType.EFFECT_BEAUTY_BASE_WHITTEN,
                STEffectBeautyType.WHITENING3_MODE
            )
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_BASE_WHITTEN,
                it
            )
        }

        // 瘦脸
        thinFace?.let { effectNative.setBeautyStrength(STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_THIN_FACE, it) }

        // 大眼
        enlargeEye?.let { effectNative.setBeautyStrength(STEffectBeautyType.EFFECT_BEAUTY_RESHAPE_ENLARGE_EYE, it) }

        // 红润
        redden?.let { effectNative.setBeautyStrength(STEffectBeautyType.EFFECT_BEAUTY_BASE_REDDEN, it) }

        // 瘦颧骨
        shrinkCheekbone?.let { effectNative.setBeautyStrength(STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_SHRINK_CHEEKBONE, it) }

        // 下颌骨
        shrinkJawbone?.let { effectNative.setBeautyStrength(STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_SHRINK_JAWBONE, it) }

        // 美牙
        whiteTeeth?.let {
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_WHITE_TEETH,
                it
            )
        }

        // 额头
        hairlineHeight?.let { effectNative.setBeautyStrength(STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_HAIRLINE_HEIGHT, it) }

        // 瘦鼻
        narrowNose?.let {
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_NARROW_NOSE,
                it
            )
        }

        // 嘴形
        mouthSize?.let { effectNative.setBeautyStrength(STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_MOUTH_SIZE, it) }

        // 下巴
        chinLength?.let {
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_CHIN_LENGTH,it
            )
        }

        // 亮眼
        brightEye?.let {
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_BRIGHT_EYE,
                it
            )
        }

        // 祛黑眼圈
        darkCircles?.let { effectNative.setBeautyStrength(STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_REMOVE_DARK_CIRCLES, it) }

        // 祛法令纹
        nasolabialFolds?.let {
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_REMOVE_NASOLABIAL_FOLDS,
                it
            )
        }

        // 饱和度
        saturation?.let {
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_TONE_SATURATION,
                it
            )
        }

        // 对比度
        contrast?.let {
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_TONE_CONTRAST,
                it
            )
        }

    }
}