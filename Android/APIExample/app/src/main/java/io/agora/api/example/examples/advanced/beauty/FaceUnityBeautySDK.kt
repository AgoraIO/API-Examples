package io.agora.api.example.examples.advanced.beauty

import android.content.Context
import android.util.Log
import com.faceunity.core.callback.OperateCallback
import com.faceunity.core.entity.FUBundleData
import com.faceunity.core.enumeration.FUAITypeEnum
import com.faceunity.core.faceunity.FUAIKit
import com.faceunity.core.faceunity.FURenderConfig
import com.faceunity.core.faceunity.FURenderKit
import com.faceunity.core.faceunity.FURenderManager
import com.faceunity.core.model.facebeauty.FaceBeauty
import com.faceunity.core.utils.FULogger
import com.faceunity.wrapper.faceunity
import java.io.File
import java.util.concurrent.Executors

/**
 * Face unity beauty s d k
 *
 * @constructor Create empty Face unity beauty s d k
 */
object FaceUnityBeautySDK {
    private val TAG = "FaceUnityBeautySDK"

    private val fuAIKit = FUAIKit.getInstance()

    /**
     * Fu render kit
     */
    val fuRenderKit = FURenderKit.getInstance()

    /* AI道具*/
    private val BUNDLE_AI_FACE = "model" + File.separator + "ai_face_processor.bundle"
    private val BUNDLE_AI_HUMAN = "model" + File.separator + "ai_human_processor.bundle"

    private val workerThread = Executors.newSingleThreadExecutor()

    /**
     * Init beauty
     *
     * @param context
     */
    fun initBeauty(context: Context) {
        FURenderManager.setKitDebug(FULogger.LogLevel.TRACE)
        FURenderManager.setCoreDebug(FULogger.LogLevel.ERROR)
        FURenderManager.registerFURender(context, getAuth(), object : OperateCallback {
            override fun onSuccess(code: Int, msg: String) {
                Log.i(TAG, "FURenderManager onSuccess -- code=$code, msg=$msg")
                if (code == FURenderConfig.OPERATE_SUCCESS_AUTH) {
                    faceunity.fuSetUseTexAsync(1)
                    workerThread.submit {
                        fuAIKit.loadAIProcessor(BUNDLE_AI_FACE, FUAITypeEnum.FUAITYPE_FACEPROCESSOR)
                        fuAIKit.loadAIProcessor(
                            BUNDLE_AI_HUMAN,
                            FUAITypeEnum.FUAITYPE_HUMAN_PROCESSOR
                        )
                    }
                }
            }

            override fun onFail(errCode: Int, errMsg: String) {
                Log.e(TAG, "FURenderManager onFail -- code=$errCode, msg=$errMsg")
            }
        })
    }

    /**
     * Release
     *
     */
    fun release() {
        FURenderKit.getInstance().release()
    }

    /**
     * Get auth
     *
     * @return
     */
    private fun getAuth(): ByteArray {
        try {
            val authpack = Class.forName("io.agora.api.example.examples.advanced.beauty.authpack")
            val aMethod = authpack.getDeclaredMethod("A")
            aMethod.isAccessible = true
            val authValue = aMethod.invoke(null) as? ByteArray
            return authValue ?: ByteArray(0)
        } catch (e: Exception){
            Log.e(TAG, "getAuth >> error : $e")
        }
        return ByteArray(0)
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
     */
    fun setBeauty(
        smooth: Double? = null,
        whiten: Double? = null,
        thinFace: Double? = null,
        enlargeEye: Double? = null,
        redden: Double? = null,
        shrinkCheekbone: Double? = null,
        shrinkJawbone: Double? = null,
        whiteTeeth: Double? = null,
        hairlineHeight: Double? = null,
        narrowNose: Double? = null,
        mouthSize: Double? = null,
        chinLength: Double? = null,
        brightEye: Double? = null,
        darkCircles: Double? = null,
        nasolabialFolds: Double? = null
    ) {
        if (fuRenderKit.faceBeauty == null) {
            fuRenderKit.faceBeauty =
                FaceBeauty(FUBundleData("graphics" + File.separator + "face_beautification.bundle"))
        }
        // 磨皮
        smooth?.let { fuRenderKit.faceBeauty?.blurIntensity = it * 6 }

        // 美白
        whiten?.let { fuRenderKit.faceBeauty?.colorIntensity = it * 2 }

        // 瘦脸
        thinFace?.let { fuRenderKit.faceBeauty?.cheekThinningIntensity = it }

        // 大眼
        enlargeEye?.let { fuRenderKit.faceBeauty?.eyeEnlargingIntensity = it }

        // 红润
        redden?.let { fuRenderKit.faceBeauty?.redIntensity = it * 2 }

        // 瘦颧骨
        shrinkCheekbone?.let { fuRenderKit.faceBeauty?.cheekBonesIntensity = it }

        // 下颌骨
        shrinkJawbone?.let { fuRenderKit.faceBeauty?.lowerJawIntensity = it }

        // 美牙
        whiteTeeth?.let { fuRenderKit.faceBeauty?.toothIntensity = it }

        // 额头
        hairlineHeight?.let { fuRenderKit.faceBeauty?.forHeadIntensity = it }

        // 瘦鼻
        narrowNose?.let { fuRenderKit.faceBeauty?.noseIntensity = it }

        // 嘴形
        mouthSize?.let { fuRenderKit.faceBeauty?.mouthIntensity = it }

        // 下巴
        chinLength?.let { fuRenderKit.faceBeauty?.chinIntensity = it }

        // 亮眼
        brightEye?.let { fuRenderKit.faceBeauty?.eyeBrightIntensity = it }

        // 祛黑眼圈
        darkCircles?.let { fuRenderKit.faceBeauty?.removePouchIntensity = it }

        // 祛法令纹
        nasolabialFolds?.let { fuRenderKit.faceBeauty?.removeLawPatternIntensity = it }

    }
}