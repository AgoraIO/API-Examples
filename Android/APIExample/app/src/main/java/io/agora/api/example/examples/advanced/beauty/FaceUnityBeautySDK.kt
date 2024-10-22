package io.agora.api.example.examples.advanced.beauty

import android.content.Context
import android.text.TextUtils
import android.util.Log
import com.faceunity.core.callback.OperateCallback
import com.faceunity.core.entity.FUBundleData
import com.faceunity.core.enumeration.FUAITypeEnum
import com.faceunity.core.faceunity.AICommonData.FUAIFACE_DISABLE_ARMESHV2
import com.faceunity.core.faceunity.AICommonData.FUAIFACE_DISABLE_DEL_SPOT
import com.faceunity.core.faceunity.AICommonData.FUAIFACE_DISABLE_FACE_OCCU
import com.faceunity.core.faceunity.AICommonData.FUAIFACE_DISABLE_SKIN_SEG
import com.faceunity.core.faceunity.AICommonData.FUAIFACE_ENABLE_ALL
import com.faceunity.core.faceunity.FUAIKit
import com.faceunity.core.faceunity.FURenderConfig.OPERATE_SUCCESS_AUTH
import com.faceunity.core.faceunity.FURenderKit
import com.faceunity.core.faceunity.FURenderManager
import com.faceunity.core.model.facebeauty.FaceBeauty
import com.faceunity.core.model.makeup.SimpleMakeup
import com.faceunity.core.model.prop.sticker.Sticker
import com.faceunity.core.utils.FULogger
import com.faceunity.wrapper.faceunity
import io.agora.api.example.examples.advanced.beauty.faceunity.utils.FuDeviceUtils
import io.agora.beautyapi.faceunity.FaceUnityBeautyAPI
import java.io.File

object FaceUnityBeautySDK {

    private const val TAG = "FaceUnityBeautySDK"

    private const val BUNDLE_AI_FACE = "model/ai_face_processor.bundle"
    private const val BUNDLE_AI_HUMAN = "model/ai_human_processor.bundle"

    val beautyConfig = BeautyConfig()

    private var beautyAPI: FaceUnityBeautyAPI? = null

    private var authSuccess = false

    fun configureFURenderKit() {
        val fukit = FUAIKit.getInstance();
        fukit.setFaceDelayLeaveEnable(FuDeviceUtils.Config.FACE_DELAY_LEAVE_ENABLE)
        when (FuDeviceUtils.Config.DEVICE_LEVEL) {
            FuDeviceUtils.DEVICE_LEVEL_MINUS_ONE, FuDeviceUtils.DEVICE_LEVEL_ONE -> fukit.fuSetFaceAlgorithmConfig(
                FUAIFACE_DISABLE_FACE_OCCU or FUAIFACE_DISABLE_SKIN_SEG or FUAIFACE_DISABLE_DEL_SPOT or FUAIFACE_DISABLE_ARMESHV2
            )

            FuDeviceUtils.DEVICE_LEVEL_TWO -> fukit.fuSetFaceAlgorithmConfig(
                FUAIFACE_DISABLE_SKIN_SEG or FUAIFACE_DISABLE_DEL_SPOT or FUAIFACE_DISABLE_ARMESHV2
            )

            FuDeviceUtils.DEVICE_LEVEL_THREE -> fukit.fuSetFaceAlgorithmConfig(
                FUAIFACE_DISABLE_SKIN_SEG
            )

            FuDeviceUtils.DEVICE_LEVEL_FOUR -> fukit.fuSetFaceAlgorithmConfig(FUAIFACE_ENABLE_ALL)
        }
        fukit.loadAIProcessor(
            FuDeviceUtils.Config.BUNDLE_AI_FACE,
            FUAITypeEnum.FUAITYPE_FACEPROCESSOR
        )
        fukit.faceProcessorSetFaceLandmarkQuality(if (FuDeviceUtils.Config.DEVICE_LEVEL >= 2) 2 else 1)
        FURenderKit.getInstance()
            .setDynamicQualityControl(FuDeviceUtils.Config.DEVICE_LEVEL <= FuDeviceUtils.DEVICE_LEVEL_ONE)
        if (FuDeviceUtils.Config.DEVICE_LEVEL > FuDeviceUtils.DEVICE_LEVEL_ONE) fukit.fuFaceProcessorSetDetectSmallFace(
            true
        )
    }

    fun initBeauty(context: Context): Boolean {
        val auth = try {
            getAuth()
        } catch (e: Exception) {
            Log.w(TAG, e)
            return false
        } ?: return false
        FuDeviceUtils.Config.DEVICE_LEVEL = FuDeviceUtils.judgeDeviceLevel(context)
        FuDeviceUtils.Config.DEVICE_NAME = FuDeviceUtils.getDeviceName()
        FURenderManager.setKitDebug(FULogger.LogLevel.TRACE)
        FURenderManager.setCoreDebug(FULogger.LogLevel.ERROR)
        FURenderManager.registerFURender(context, auth, object : OperateCallback {
            override fun onSuccess(code: Int, msg: String) {
                Log.i(TAG, "FURenderManager onSuccess -- code=$code, msg=$msg")
                if (code == OPERATE_SUCCESS_AUTH) {
                    authSuccess = true
                    faceunity.fuSetUseTexAsync(0)
                    FUAIKit.getInstance()
                        .loadAIProcessor(BUNDLE_AI_FACE, FUAITypeEnum.FUAITYPE_FACEPROCESSOR)
                    // FUAIKit.getInstance().loadAIProcessor(
                    //     BUNDLE_AI_HUMAN,
                    //     FUAITypeEnum.FUAITYPE_HUMAN_PROCESSOR
                    // )

                }
            }

            override fun onFail(errCode: Int, errMsg: String) {
                Log.e(TAG, "FURenderManager onFail -- code=$errCode, msg=$errMsg")
            }
        })
        return true
    }

    fun isAuthSuccess(): Boolean {
        return authSuccess
    }

    fun unInitBeauty() {
        beautyAPI = null
        beautyConfig.reset()
        authSuccess = false
        FUAIKit.getInstance().releaseAllAIProcessor()
        FURenderKit.getInstance().release()
    }

    private fun getAuth(): ByteArray? {
        val authpack = Class.forName("io.agora.api.example.examples.advanced.beauty.authpack")
        val aMethod = authpack.getDeclaredMethod("A")
        aMethod.isAccessible = true
        return aMethod.invoke(null) as? ByteArray
    }

    internal fun setBeautyAPI(beautyAPI: FaceUnityBeautyAPI?) {
        FaceUnityBeautySDK.beautyAPI = beautyAPI
        beautyConfig.resume()
    }

    private fun runOnBeautyThread(run: () -> Unit) {
        beautyAPI?.runOnProcessThread(run) ?: run.invoke()
    }


    class BeautyConfig {

        private val fuRenderKit = FURenderKit.getInstance()

        private val faceBeauty: FaceBeauty
            get() {
                var faceBeauty = fuRenderKit.faceBeauty
                if (faceBeauty == null) {
                    faceBeauty =
                        FaceBeauty(FUBundleData("graphics" + File.separator + "face_beautification.bundle"))
                    fuRenderKit.faceBeauty = faceBeauty
                }
                return faceBeauty
            }


        private val resourceBase = "beauty_faceunity"

        var smooth = 0.65f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.blurIntensity = value * 6.0
                }
            }

        var whiten = 0.65f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.colorIntensity = value * 2.0
                }
            }

        var thinFace = 0.3f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.cheekThinningIntensity = value.toDouble()
                }
            }

        var enlargeEye = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.eyeEnlargingIntensity = value.toDouble()
                }
            }

        var redden = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.redIntensity = value * 2.0
                }
            }

        var faceThree = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.faceThreeIntensity = value.toDouble()
                }
            }

        var shrinkCheekbone = 0.3f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.cheekBonesIntensity = value.toDouble()
                }
            }

        var shrinkJawbone = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.lowerJawIntensity = value.toDouble()
                }
            }

        var whiteTeeth = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.toothIntensity = value.toDouble()
                }
            }

        var hairlineHeight = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.forHeadIntensity = value.toDouble()
                }
            }

        var narrowNose = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.noseIntensity = value.toDouble()
                }
            }

        var mouthSize = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.mouthIntensity = value.toDouble()
                }
            }

        var chinLength = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.chinIntensity = value.toDouble()
                }
            }

        var brightEye = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.eyeBrightIntensity = value.toDouble()
                }
            }

        var darkCircles = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.removePouchIntensity = value.toDouble()
                }
            }

        var nasolabialFolds = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.removeLawPatternIntensity = value.toDouble()
                }
            }

        var sharpen = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.sharpenIntensity = value.toDouble()
                }
            }

        var sticker: String? = null
            set(value) {
                field = value
                runOnBeautyThread {
                    fuRenderKit.propContainer.removeAllProp()
                    if (!TextUtils.isEmpty(value)) {
                        val prop = Sticker(FUBundleData("$resourceBase/$sticker"))
                        fuRenderKit.propContainer.addProp(prop)
                    }
                }
            }

        var makeUp: MakeUpItem? = null
            set(value) {
                field = value
                runOnBeautyThread {
                    if (value == null) {
                        fuRenderKit.makeup = null
                    } else {
                        val makeup =
                            SimpleMakeup(FUBundleData("$resourceBase/${value.path}"))
                        makeup.makeupIntensity = value.intensity.toDouble()
                        fuRenderKit.makeup = makeup
                    }
                }
            }


        fun reset() {
            smooth = 0.65f
            whiten = 0.65f
            thinFace = 0.3f
            enlargeEye = 0.0f
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
            faceThree = 0.0f

            makeUp = null
            sticker = null
        }

        fun resume() {
            smooth = smooth
            whiten = whiten
            thinFace = thinFace
            enlargeEye = enlargeEye
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
            faceThree = faceThree

            makeUp = makeUp
            sticker = sticker
        }

    }

    data class MakeUpItem(
        val path: String,
        val intensity: Float
    )


}