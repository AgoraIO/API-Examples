package io.agora.api.example.examples.advanced.beauty

import android.content.Context
import android.text.TextUtils
import android.util.Log
import com.softsugar.stmobile.STMobileAuthentificationNative
import com.softsugar.stmobile.STMobileEffectNative
import com.softsugar.stmobile.STMobileEffectParams
import com.softsugar.stmobile.STMobileHumanActionNative
import com.softsugar.stmobile.params.STEffectBeautyType
import io.agora.api.example.examples.advanced.beauty.utils.FileUtils
import io.agora.beautyapi.sensetime.SenseTimeBeautyAPI

object SenseTimeBeautySDK {
    private const val TAG = "SenseTimeBeautySDK"

    private val resourcePath = "beauty_sensetime"
    private val humanActionCreateConfig = 0

    private const val MODEL_106 = "models/M_SenseME_Face_Video_Template_p_3.9.0.3.model" // 106
    // private const val MODEL_FACE_EXTRA = "models/M_SenseME_Face_Extra_Advanced_Template_p_2.0.0.model" // 282
    // private const val MODEL_AVATAR_HELP = "models/M_SenseME_Avatar_Help_p_2.3.7.model" // avatar face driving
    // private const val MODEL_LIPS_PARSING = "models/M_SenseME_MouthOcclusion_p_1.3.0.1.model" // lips segmentation
    // private const val MODEL_HAND = "models/M_SenseME_Hand_p_6.0.8.1.model" // gesture
    // private const val MODEL_SEGMENT = "models/M_SenseME_Segment_Figure_p_4.14.1.1.model" // foreground and background segmentation
    // private const val MODEL_SEGMENT_HAIR = "models/M_SenseME_Segment_Hair_p_4.4.0.model" // hair segmentation
    // private const val MODEL_FACE_OCCLUSION = "models/M_SenseME_FaceOcclusion_p_1.0.7.1.model" // makeup occlusion
    // private const val MODEL_SEGMENT_SKY = "models/M_SenseME_Segment_Sky_p_1.1.0.1.model" // sky segmentation
    // private const val MODEL_SEGMENT_SKIN = "models/M_SenseME_Segment_Skin_p_1.0.1.1.model" // skin segmentation
    // private const val MODEL_3DMESH = "models/M_SenseME_3DMesh_Face2396pt_280kpts_Ear_p_1.1.0v2.model" // 3DMesh
    // private const val MODEL_HEAD_P_EAR = "models/M_SenseME_Ear_p_1.0.1.1.model" // ear model with mesh
    // private const val MODEL_360HEAD_INSTANCE = "models/M_SenseME_3Dmesh_360Head2396pt_p_1.0.0.1.model" // 360-degree head mesh
    // private const val MODEL_FOOT = "models/M_SenseME_Foot_p_2.10.7.model" // shoe detection model
    // private const val MODEL_PANT = "models/M_SenseME_Segment_Trousers_p_1.1.10.model" // trouser leg detection
    // private const val MODEL_WRIST = "models/M_SenseME_Wrist_p_1.4.0.model" // watch try-on
    // private const val MODEL_CLOTH = "models/M_SenseME_Segment_Clothes_p_1.0.2.2.model" // clothing segmentation
    // private const val MODEL_HEAD_INSTANCE = "models/M_SenseME_Segment_Head_Instance_p_1.1.0.1.model" // instance segmentation version
    // private const val MODEL_HEAD_P_INSTANCE = "models/M_SenseME_Head_p_1.3.0.1.model" // 360-degree head model
    // private const val MODEL_NAIL = "models/M_SenseME_Nail_p_2.4.0.model" // nail detection

    private var stickerPackageId = 0

    // Effect handle
    private var _mobileEffectNative: STMobileEffectNative? = null
    val mobileEffectNative
        get() = _mobileEffectNative ?: throw RuntimeException("Please initMobileEffect firstly!")

    // Face recognition handle
    private var _humanActionNative: STMobileHumanActionNative? = null
    val humanActionNative
        get() = _humanActionNative ?: throw RuntimeException("Please initBeautySDK firstly!")

    // Beauty configuration
    val beautyConfig = BeautyConfig()

    private var beautyAPI: SenseTimeBeautyAPI? = null

    private var authSuccess = false

    fun initBeautySDK(context: Context): Boolean {
        if (checkLicense(context)) {
            initHumanAction(context)
            authSuccess = true
            return true
        }
        initHumanAction(context)
        return false
    }

    fun isAuthSuccess(): Boolean {
        return authSuccess
    }

    fun unInitBeautySDK() {
        beautyAPI = null
        authSuccess = false
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
        beautyConfig.resume()
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
        Log.d(TAG, "SenseTime >> checkLicense activeCode=$activeCode")
        return activeCode.isNotEmpty()
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

        // Other model configurations
        // _humanActionNative?.addSubModelFromAssetFile("$resourcePath/$MODEL_FACE_EXTRA", assets)

        // Background segmentation feathering degree [0,1] (default value 0.35), 0 means no feathering at all, 1 means maximum feathering.
        // When strength is small, feathering remains basically unchanged. The larger the value, the wider the transition edge between foreground and background.
        // _humanActionNative?.setParam(
        //     STHumanActionParamsType.ST_HUMAN_ACTION_PARAM_BACKGROUND_BLUR_STRENGTH,
        //     0.35f
        // )
        // Set face mesh result output coordinate system (0: screen coordinate system, 1: 3D world coordinate system, 
        // 2: 3D camera coordinate system, which is the camera perspective projection coordinate system, with origin at the camera, default is 0)
        // _humanActionNative?.setParam(
        //     STHumanActionParamsType.ST_HUMAN_ACTION_PARAM_FACE_MESH_OUTPUT_FORMAT,
        //     1.0f
        // )
        // Set mesh rendering mode
        // _humanActionNative?.setParam(
        //     STHumanActionParamsType.ST_HUMAN_ACTION_PARAM_MESH_MODE,
        //     STCommonNative.MESH_CONFIG.toFloat()
        // )
        // Set head instance segmentation
        // _humanActionNative?.setParam(
        //     STHumanActionParamsType.ST_HUMAN_ACTION_PARAM_HEAD_SEGMENT_INSTANCE,
        //     1.0f
        // )
    }


    private fun unInitHumanActionNative() {
        _humanActionNative?.destroyInstance()
        _humanActionNative = null
    }


    internal fun setBeautyAPI(beautyAPI: SenseTimeBeautyAPI?){
        SenseTimeBeautySDK.beautyAPI = beautyAPI
        beautyConfig.resume()
    }

    private fun runOnBeautyThread(run: () -> Unit) {
        beautyAPI?.runOnProcessThread(run) ?: run.invoke()
    }

    open class BeautyConfig {
        // Smoothing
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

        // Whitening
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

        // Face thinning
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

        // Eye enlargement
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

        // Skin redness
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

        // Cheekbone thinning
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

        // Jawbone thinning
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

        // Teeth whitening
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

        // Hairline height
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

        // Nose narrowing
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

        // Mouth size
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

        // Chin length
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

        // Bright eyes
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

        // Remove dark circles
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

        // Remove nasolabial folds
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

        // Saturation
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

        // Contrast
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

        // Sharpen
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

        // Clarity
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

        // Makeup
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

        // Sticker
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