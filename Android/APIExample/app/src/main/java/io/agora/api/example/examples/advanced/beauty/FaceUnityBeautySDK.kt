package io.agora.api.example.examples.advanced.beauty

import android.content.Context
import android.text.TextUtils
import android.util.Log
import com.faceunity.core.callback.OperateCallback
import com.faceunity.core.entity.FUBundleData
import com.faceunity.core.enumeration.FUAITypeEnum
import com.faceunity.core.faceunity.FUAIKit
import com.faceunity.core.faceunity.FURenderConfig.OPERATE_SUCCESS_AUTH
import com.faceunity.core.faceunity.FURenderKit
import com.faceunity.core.faceunity.FURenderManager
import com.faceunity.core.model.facebeauty.FaceBeauty
import com.faceunity.core.model.makeup.SimpleMakeup
import com.faceunity.core.model.prop.sticker.Sticker
import com.faceunity.core.utils.FULogger
import com.faceunity.wrapper.faceunity
import io.agora.beautyapi.faceunity.FaceUnityBeautyAPI
import java.io.File

object FaceUnityBeautySDK {

    private const val TAG = "FaceUnityBeautySDK"

    /* AI道具*/
    private const val BUNDLE_AI_FACE = "model/ai_face_processor.bundle"
    private const val BUNDLE_AI_HUMAN = "model/ai_human_processor.bundle"

    // 美颜配置
    val beautyConfig = BeautyConfig()

    private var beautyAPI: FaceUnityBeautyAPI? = null

    private var authSuccess = false

    fun initBeauty(context: Context): Boolean {
        val auth = try {
            getAuth()
        } catch (e: Exception) {
            Log.w(TAG, e)
            return false
        } ?: return false

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

        // 美颜配置
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


        // 资源基础路径
        private val resourceBase = "beauty_faceunity"

        // 磨皮
        var smooth = 0.65f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.blurIntensity = value * 6.0
                }
            }

        // 美白
        var whiten = 0.65f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.colorIntensity = value * 2.0
                }
            }

        // 瘦脸
        var thinFace = 0.3f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.cheekThinningIntensity = value.toDouble()
                }
            }

        // 大眼
        var enlargeEye = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.eyeEnlargingIntensity = value.toDouble()
                }
            }

        // 红润
        var redden = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.redIntensity = value * 2.0
                }
            }

        // 五官立体
        var faceThree = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.faceThreeIntensity = value.toDouble()
                }
            }

        // 瘦颧骨
        var shrinkCheekbone = 0.3f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.cheekBonesIntensity = value.toDouble()
                }
            }

        // 下颌骨
        var shrinkJawbone = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.lowerJawIntensity = value.toDouble()
                }
            }

        // 美牙
        var whiteTeeth = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.toothIntensity = value.toDouble()
                }
            }

        // 额头
        var hairlineHeight = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.forHeadIntensity = value.toDouble()
                }
            }

        // 瘦鼻
        var narrowNose = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.noseIntensity = value.toDouble()
                }
            }

        // 嘴形
        var mouthSize = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.mouthIntensity = value.toDouble()
                }
            }

        // 下巴
        var chinLength = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.chinIntensity = value.toDouble()
                }
            }

        // 亮眼
        var brightEye = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.eyeBrightIntensity = value.toDouble()
                }
            }

        // 祛黑眼圈
        var darkCircles = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.removePouchIntensity = value.toDouble()
                }
            }

        // 祛法令纹
        var nasolabialFolds = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.removeLawPatternIntensity = value.toDouble()
                }
            }

        // 锐化
        var sharpen = 0.0f
            set(value) {
                field = value
                runOnBeautyThread {
                    faceBeauty.sharpenIntensity = value.toDouble()
                }
            }

        // 贴纸
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

        // 美妆
        var makeUp: MakeUpItem? = null
            set(value) {
                field = value
                runOnBeautyThread {
                    if (value == null) {
                        fuRenderKit.makeup = null
                    } else {
                        val makeup =
                            SimpleMakeup(FUBundleData("graphics" + File.separator + "face_makeup.bundle"))
                        makeup.setCombinedConfig(FUBundleData("$resourceBase/${value.path}"))
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

        fun resume(){
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