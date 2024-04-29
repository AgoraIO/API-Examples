package io.agora.api.example.examples.advanced.beauty

import android.content.Context
import android.text.TextUtils
import android.util.Log
import com.softsugar.stmobile.STMobileAuthentificationNative
import com.softsugar.stmobile.STMobileEffectNative
import com.softsugar.stmobile.STMobileEffectParams
import com.softsugar.stmobile.STMobileHumanActionNative
import com.softsugar.stmobile.params.STEffectBeautyType
import io.agora.api.example.utils.FileUtils
import io.agora.beautyapi.sensetime.SenseTimeBeautyAPI

object SenseTimeBeautySDK {
    private const val TAG = "SenseTimeBeautySDK"

    private val resourcePath = "beauty_sensetime"
    private val humanActionCreateConfig = 0

    private const val MODEL_106 = "models/M_SenseME_Face_Video_Template_p_3.9.0.3.model" // 106
    // private const val MODEL_FACE_EXTRA = "models/M_SenseME_Face_Extra_Advanced_Template_p_2.0.0.model" // 282
    // private const val MODEL_AVATAR_HELP = "models/M_SenseME_Avatar_Help_p_2.3.7.model" // avatar人脸驱动
    // private const val MODEL_LIPS_PARSING = "models/M_SenseME_MouthOcclusion_p_1.3.0.1.model" // 嘴唇分割
    // private const val MODEL_HAND = "models/M_SenseME_Hand_p_6.0.8.1.model" // 手势
    // private const val MODEL_SEGMENT = "models/M_SenseME_Segment_Figure_p_4.14.1.1.model" // 前后背景分割
    // private const val MODEL_SEGMENT_HAIR = "models/M_SenseME_Segment_Hair_p_4.4.0.model" // 头发分割
    // private const val MODEL_FACE_OCCLUSION = "models/M_SenseME_FaceOcclusion_p_1.0.7.1.model" // 妆容遮挡
    // private const val MODEL_SEGMENT_SKY = "models/M_SenseME_Segment_Sky_p_1.1.0.1.model" // 天空分割
    // private const val MODEL_SEGMENT_SKIN = "models/M_SenseME_Segment_Skin_p_1.0.1.1.model" // 皮肤分割
    // private const val MODEL_3DMESH = "models/M_SenseME_3DMesh_Face2396pt_280kpts_Ear_p_1.1.0v2.model" // 3DMesh
    // private const val MODEL_HEAD_P_EAR = "models/M_SenseME_Ear_p_1.0.1.1.model" // 搭配 mesh 耳朵模型
    // private const val MODEL_360HEAD_INSTANCE = "models/M_SenseME_3Dmesh_360Head2396pt_p_1.0.0.1.model" // 360度人头mesh
    // private const val MODEL_FOOT = "models/M_SenseME_Foot_p_2.10.7.model" // 鞋子检测模型
    // private const val MODEL_PANT = "models/M_SenseME_Segment_Trousers_p_1.1.10.model" // 裤腿的检测
    // private const val MODEL_WRIST = "models/M_SenseME_Wrist_p_1.4.0.model" // 试表
    // private const val MODEL_CLOTH = "models/M_SenseME_Segment_Clothes_p_1.0.2.2.model" // 衣服分割
    // private const val MODEL_HEAD_INSTANCE = "models/M_SenseME_Segment_Head_Instance_p_1.1.0.1.model" // 实例分割版本
    // private const val MODEL_HEAD_P_INSTANCE = "models/M_SenseME_Head_p_1.3.0.1.model" // 360度人头-头部模型
    // private const val MODEL_NAIL = "models/M_SenseME_Nail_p_2.4.0.model" // 指甲检测

    private var stickerPackageId = 0

    // 特效句柄
    private var _mobileEffectNative: STMobileEffectNative? = null
    val mobileEffectNative
        get() = _mobileEffectNative ?: throw RuntimeException("Please initMobileEffect firstly!")

    // 人脸识别句柄
    private var _humanActionNative: STMobileHumanActionNative? = null
    val humanActionNative
        get() = _humanActionNative ?: throw RuntimeException("Please initBeautySDK firstly!")

    // 美颜配置
    val beautyConfig = BeautyConfig()

    private var beautyAPI: SenseTimeBeautyAPI? = null

    fun initBeautySDK(context: Context): Boolean {
        if (checkLicense(context)) {
            initHumanAction(context)
            return true
        }
        return false
    }

    fun unInitBeautySDK() {
        beautyAPI = null
        unInitHumanActionNative()
        beautyConfig.reset()
    }

    fun initMobileEffect(context: Context) {
        if (_mobileEffectNative != null) {
            return
        }
        _mobileEffectNative = STMobileEffectNative()
        val result =
            _mobileEffectNative?.createInstance(context, STMobileEffectNative.EFFECT_CONFIG_NONE)
        _mobileEffectNative?.setParam(STMobileEffectParams.EFFECT_PARAM_QUATERNION_SMOOTH_FRAME, 5f)
        Log.d(TAG, "SenseTime >> STMobileEffectNative create result : $result")
    }

    fun unInitMobileEffect() {
        _mobileEffectNative?.destroyInstance()
        _mobileEffectNative = null
    }

    private fun checkLicense(context: Context): Boolean {
        val license = FileUtils.getAssetsString(
            context,
            "$resourcePath/license/SenseME.lic"
        )
        if(TextUtils.isEmpty(license)){
            return false
        }
        val activeCode = STMobileAuthentificationNative.generateActiveCodeFromBuffer(
            context,
            license,
            license.length
        )
        Log.d(TAG, "SenseTime >> checkLicense successfully! activeCode=$activeCode")
        return true
    }

    private fun initHumanAction(context: Context) {
        if (_humanActionNative != null) {
            return
        }
        _humanActionNative = STMobileHumanActionNative()
        val assets = context.assets
        val result = _humanActionNative?.createInstanceFromAssetFile("$resourcePath/$MODEL_106", humanActionCreateConfig, assets)
        Log.d(TAG, "SenseTime >> STMobileHumanActionNative create result : $result")

        if (result != 0) {
            return
        }

        // 其他模型配置
        // _humanActionNative?.addSubModelFromAssetFile("$resourcePath/$MODEL_FACE_EXTRA", assets)

        // 背景分割羽化程度[0,1](默认值0.35),0 完全不羽化,1羽化程度最高,在strenth较小时,羽化程度基本不变.值越大,前景与背景之间的过度边缘部分越宽.
        // _humanActionNative?.setParam(
        //     STHumanActionParamsType.ST_HUMAN_ACTION_PARAM_BACKGROUND_BLUR_STRENGTH,
        //     0.35f
        // )
        // 设置face mesh结果输出坐标系,(0: 屏幕坐标系， 1：3d世界坐标系， 2:3d摄像机坐标系,是摄像头透视投影坐标系, 原点在摄像机 默认是0）
        // _humanActionNative?.setParam(
        //     STHumanActionParamsType.ST_HUMAN_ACTION_PARAM_FACE_MESH_OUTPUT_FORMAT,
        //     1.0f
        // )
        // 设置mesh渲染模式
        // _humanActionNative?.setParam(
        //     STHumanActionParamsType.ST_HUMAN_ACTION_PARAM_MESH_MODE,
        //     STCommonNative.MESH_CONFIG.toFloat()
        // )
        // 设置人头实例分割
        // _humanActionNative?.setParam(
        //     STHumanActionParamsType.ST_HUMAN_ACTION_PARAM_HEAD_SEGMENT_INSTANCE,
        //     1.0f
        // )
    }


    private fun unInitHumanActionNative() {
        _humanActionNative?.destroyInstance()
        _humanActionNative = null
    }


    internal fun setBeautyAPI(beautyAPI: SenseTimeBeautyAPI){
        this.beautyAPI = beautyAPI
        beautyConfig.resume()
    }

    private fun runOnBeautyThread(run: () -> Unit) {
        beautyAPI?.runOnProcessThread(run) ?: run.invoke()
    }

    open class BeautyConfig {
        // 磨皮
        var smooth = 0.75f
            set(value) {
                field = value
                val effectNative = _mobileEffectNative ?: return
                runOnBeautyThread {
                    effectNative.setBeautyMode(
                        STEffectBeautyType.EFFECT_BEAUTY_BASE_FACE_SMOOTH,
                        STEffectBeautyType.SMOOTH2_MODE
                    )
                    effectNative.setBeautyStrength(
                        STEffectBeautyType.EFFECT_BEAUTY_BASE_FACE_SMOOTH,
                        value
                    )
                }
            }

        // 美白
        var whiten = 0.75f
            set(value) {
                field = value
                val effectNative = _mobileEffectNative ?: return
                runOnBeautyThread {
                    effectNative.setBeautyMode(
                        STEffectBeautyType.EFFECT_BEAUTY_BASE_WHITTEN,
                        STEffectBeautyType.WHITENING3_MODE
                    )
                    effectNative.setBeautyStrength(
                        STEffectBeautyType.EFFECT_BEAUTY_BASE_WHITTEN,
                        value
                    )
                }
            }

        // 瘦脸
        var thinFace = 0.3f
            set(value) {
                field = value
                val effectNative = _mobileEffectNative ?: return
                runOnBeautyThread {
                    effectNative.setBeautyStrength(
                        STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_THIN_FACE,
                        value
                    )
                }
            }


        // 大眼
        var enlargeEye = 0.0f
            set(value) {
                field = value
                val effectNative = _mobileEffectNative ?: return
                runOnBeautyThread {
                    effectNative.setBeautyStrength(
                        STEffectBeautyType.EFFECT_BEAUTY_RESHAPE_ENLARGE_EYE,
                        value
                    )
                }
            }

        // 红润
        var redden = 0.0f
            set(value) {
                field = value
                val effectNative = _mobileEffectNative ?: return
                runOnBeautyThread {
                    effectNative.setBeautyStrength(
                        STEffectBeautyType.EFFECT_BEAUTY_BASE_REDDEN,
                        value
                    )
                }
            }


        // 瘦颧骨
        var shrinkCheekbone = 0.3f
            set(value) {
                field = value
                val effectNative = _mobileEffectNative ?: return
                runOnBeautyThread {
                    effectNative.setBeautyStrength(
                        STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_SHRINK_CHEEKBONE,
                        value
                    )
                }
            }

        // 下颌骨
        var shrinkJawbone = 0.0f
            set(value) {
                field = value
                val effectNative = _mobileEffectNative ?: return
                runOnBeautyThread {
                    effectNative.setBeautyStrength(
                        STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_SHRINK_JAWBONE,
                        value
                    )
                }
            }

        // 美牙
        var whiteTeeth = 0.0f
            set(value) {
                field = value
                val effectNative = _mobileEffectNative ?: return
                runOnBeautyThread {
                    effectNative.setBeautyStrength(
                        STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_WHITE_TEETH,
                        value
                    )
                }
            }

        // 额头
        var hairlineHeight = 0.0f
            set(value) {
                field = value
                val effectNative = _mobileEffectNative ?: return
                runOnBeautyThread {
                    effectNative.setBeautyStrength(
                        STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_HAIRLINE_HEIGHT,
                        value
                    )
                }
            }

        // 瘦鼻
        var narrowNose = 0.0f
            set(value) {
                field = value
                val effectNative = _mobileEffectNative ?: return
                runOnBeautyThread {
                    effectNative.setBeautyStrength(
                        STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_NARROW_NOSE,
                        value
                    )
                }
            }

        // 嘴形
        var mouthSize = 0.0f
            set(value) {
                field = value
                val effectNative = _mobileEffectNative ?: return
                runOnBeautyThread {
                    effectNative.setBeautyStrength(
                        STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_MOUTH_SIZE,
                        value
                    )
                }
            }


        // 下巴
        var chinLength = 0.0f
            set(value) {
                field = value
                val effectNative = _mobileEffectNative ?: return
                runOnBeautyThread {
                    effectNative.setBeautyStrength(
                        STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_CHIN_LENGTH, value
                    )
                }
            }

        // 亮眼
        var brightEye = 0.0f
            set(value) {
                field = value
                val effectNative = _mobileEffectNative ?: return
                runOnBeautyThread {
                    effectNative.setBeautyStrength(
                        STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_BRIGHT_EYE,
                        value
                    )
                }
            }

        // 祛黑眼圈
        var darkCircles = 0.0f
            set(value) {
                field = value
                val effectNative = _mobileEffectNative ?: return
                runOnBeautyThread {
                    effectNative.setBeautyStrength(
                        STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_REMOVE_DARK_CIRCLES,
                        value
                    )
                }
            }

        // 祛法令纹
        var nasolabialFolds = 0.0f
            set(value) {
                field = value
                val effectNative = _mobileEffectNative ?: return
                runOnBeautyThread {
                    effectNative.setBeautyStrength(
                        STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_REMOVE_NASOLABIAL_FOLDS,
                        value
                    )
                }
            }

        // 饱和度
        var saturation = 0.0f
            set(value) {
                field = value
                val effectNative = _mobileEffectNative ?: return
                runOnBeautyThread {
                    effectNative.setBeautyStrength(
                        STEffectBeautyType.EFFECT_BEAUTY_TONE_SATURATION,
                        value
                    )
                }
            }

        // 对比度
        var contrast = 0.0f
            set(value) {
                field = value
                val effectNative = _mobileEffectNative ?: return
                runOnBeautyThread {
                    effectNative.setBeautyStrength(
                        STEffectBeautyType.EFFECT_BEAUTY_TONE_CONTRAST,
                        value
                    )
                }
            }

        // 锐化
        var sharpen = 0.0f
            set(value) {
                field = value
                val effectNative = _mobileEffectNative ?: return
                runOnBeautyThread {
                    effectNative.setBeautyStrength(
                        STEffectBeautyType.EFFECT_BEAUTY_TONE_SHARPEN,
                        value
                    )
                }
            }


        // 清晰度
        var clear = 0.0f
            set(value) {
                field = value
                val effectNative = _mobileEffectNative ?: return
                runOnBeautyThread {
                    effectNative.setBeautyStrength(
                        STEffectBeautyType.EFFECT_BEAUTY_TONE_CLEAR,
                        value
                    )
                }
            }

        // 美妆
        var makeUp: MakeUpItem? = null
            set(value) {
                field = value
                runOnBeautyThread {
                    if (value == null) {
                        _mobileEffectNative?.setBeauty(
                            STEffectBeautyType.EFFECT_BEAUTY_MAKEUP_ALL,
                            null
                        )
                    } else {
                        val assets = value.context.assets
                        _mobileEffectNative?.setBeautyFromAssetsFile(
                            STEffectBeautyType.EFFECT_BEAUTY_MAKEUP_ALL,
                            "$resourcePath/${value.path}",
                            assets
                        )
                        _mobileEffectNative?.setBeautyStrength(
                            STEffectBeautyType.EFFECT_BEAUTY_MAKEUP_ALL,
                            value.strength
                        )
                    }
                }
            }

        // 贴纸
        var sticker: StickerItem? = null
            set(value) {
                field = value
                runOnBeautyThread {
                    if (value == null) {
                        if (stickerPackageId > 0) {
                            _mobileEffectNative?.removeEffect(stickerPackageId)
                            stickerPackageId = 0
                        }
                    } else {
                        stickerPackageId = _mobileEffectNative?.changePackageFromAssetsFile(
                            "$resourcePath/${value.path}",
                            value.context.assets
                        ) ?: 0
                    }
                }
            }

        internal fun reset() {
            smooth = 0.75f
            whiten = 0.75f
            thinFace = 0.3f
            enlargeEye = 0.0f
            sharpen = 0.0f
            clear = 0.0f
            redden = 0.0f
            shrinkCheekbone = 0.3f
            shrinkJawbone = 0.0f
            whiteTeeth = 0.0f
            hairlineHeight = 0.0f
            narrowNose = 0.0f
            mouthSize = 0.0f
            chinLength = 0.0f
            brightEye = 0.0f
            darkCircles = 0.0f
            nasolabialFolds = 0.0f
            saturation = 0.0f
            contrast = 0.0f

            makeUp = null
            sticker = null
        }

        internal fun resume() {
            smooth = smooth
            whiten = whiten
            thinFace = thinFace
            enlargeEye = enlargeEye
            sharpen = sharpen
            clear = clear
            redden = redden
            shrinkCheekbone = shrinkCheekbone
            shrinkJawbone = shrinkJawbone
            whiteTeeth = whiteTeeth
            hairlineHeight = hairlineHeight
            narrowNose = narrowNose
            mouthSize = mouthSize
            chinLength = chinLength
            brightEye = brightEye
            darkCircles = darkCircles
            nasolabialFolds = nasolabialFolds
            saturation = saturation
            contrast = contrast

            makeUp = makeUp
            sticker = sticker
        }
    }

    data class MakeUpItem(
        val context: Context,
        val path: String,
        val strength: Float
    )

    data class StickerItem(
        val context: Context,
        val path: String
    )
}