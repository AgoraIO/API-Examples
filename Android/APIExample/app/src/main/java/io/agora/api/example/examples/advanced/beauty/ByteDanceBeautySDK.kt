package io.agora.api.example.examples.advanced.beauty

import android.app.Application
import android.content.Context
import android.util.Log
import com.bytedance.labcv.effectsdk.RenderManager
import io.agora.api.example.utils.FileKtUtils
import java.util.concurrent.Executors

/**
 * Byte dance beauty s d k
 *
 * @constructor Create empty Byte dance beauty s d k
 */
object ByteDanceBeautySDK {
    private val TAG = "ByteDanceBeautySDK"

    private val LICENSE_NAME = "Agora_test_20231116_20240116_io.agora.test.entfull_4.5.0_893.licbag"
    private val workerThread = Executors.newSingleThreadExecutor()
    private var context: Application?  = null
    private var storagePath  = ""
    private var assetsPath  = ""

    /**
     * Render manager
     */
    val renderManager = RenderManager()

    /**
     * License path
     */
    var licensePath = ""

    /**
     * Models path
     */
    var modelsPath = ""

    /**
     * Beauty node path
     */
    var beautyNodePath = ""

    /**
     * Beauty4items node path
     */
    var beauty4ItemsNodePath = ""

    /**
     * Re sharp node path
     */
    var reSharpNodePath = ""

    /**
     * Sticker path
     */
    var stickerPath = ""


    /**
     * Makeup tianmei node path
     */
    var makeupTianmeiNodePath = ""
        get() {
            if(field.isEmpty()){
                // copy makeup node
                field = "$storagePath/beauty_bytedance/ComposeMakeup.bundle/ComposeMakeup/style_makeup/tianmei"
                FileKtUtils.copyAssets(context!!, "$assetsPath/ComposeMakeup.bundle/ComposeMakeup/style_makeup/tianmei", field)
                renderManager.appendComposerNodes(arrayOf(field))
                renderManager.loadResourceWithTimeout(-1)
            }
            return field
        }

    /**
     * Makeup yuan qi node path
     */
    var makeupYuanQiNodePath = ""
        get() {
            if(field.isEmpty()){
                // copy makeup node
                field = "$storagePath/beauty_bytedance/ComposeMakeup.bundle/ComposeMakeup/style_makeup/yuanqi"
                FileKtUtils.copyAssets(context!!, "$assetsPath/ComposeMakeup.bundle/ComposeMakeup/style_makeup/yuanqi", field)
                renderManager.appendComposerNodes(arrayOf(field))
                renderManager.loadResourceWithTimeout(-1)
            }
            return field
        }

    /**
     * Init beauty s d k
     *
     * @param context
     */
    fun initBeautySDK(context: Context){
        this.context = context.applicationContext as? Application
        storagePath = context.getExternalFilesDir("")?.absolutePath ?: return
        assetsPath = "beauty_bytedance"

        workerThread.execute {
            // copy license
            licensePath = "$storagePath/beauty_bytedance/LicenseBag.bundle/$LICENSE_NAME"
            FileKtUtils.copyAssets(context, "$assetsPath/LicenseBag.bundle/$LICENSE_NAME", licensePath)

            // copy models
            modelsPath = "$storagePath/beauty_bytedance/ModelResource.bundle"
            FileKtUtils.copyAssets(context, "$assetsPath/ModelResource.bundle", modelsPath)

            // copy beauty node
            beautyNodePath = "$storagePath/beauty_bytedance/ComposeMakeup.bundle/ComposeMakeup/beauty_Android_lite"
            FileKtUtils.copyAssets(context, "$assetsPath/ComposeMakeup.bundle/ComposeMakeup/beauty_Android_lite", beautyNodePath)

            // copy beauty 4items node
            beauty4ItemsNodePath = "$storagePath/beauty_bytedance/ComposeMakeup.bundle/ComposeMakeup/beauty_4Items"
            FileKtUtils.copyAssets(context, "$assetsPath/ComposeMakeup.bundle/ComposeMakeup/beauty_4Items", beauty4ItemsNodePath)

            // copy resharp node
            reSharpNodePath = "$storagePath/beauty_bytedance/ComposeMakeup.bundle/ComposeMakeup/reshape_lite"
            FileKtUtils.copyAssets(context, "$assetsPath/ComposeMakeup.bundle/ComposeMakeup/reshape_lite", reSharpNodePath)


            // copy stickers
            stickerPath = "$storagePath/beauty_bytedance/StickerResource.bundle/stickers"
            FileKtUtils.copyAssets(context, "$assetsPath/StickerResource.bundle/stickers", stickerPath)
        }
    }

    /**
     * Init effect
     *
     * @param context
     */// GL Thread
    fun initEffect(context: Context){
        val ret = renderManager.init(
            context,
            modelsPath, licensePath, false, false, 0
        )
        if(!checkResult("RenderManager init ", ret)){
            return
        }
        renderManager.useBuiltinSensor(true)
        renderManager.set3Buffer(false)
        renderManager.appendComposerNodes(arrayOf(beautyNodePath, beauty4ItemsNodePath, reSharpNodePath))
        renderManager.loadResourceWithTimeout(-1)
    }

    /**
     * Un init effect
     *
     */// GL Thread
    fun unInitEffect(){
        renderManager.release()
    }

    private fun checkResult(msg: String, ret: Int): Boolean {
        if (ret != 0 && ret != -11 && ret != 1) {
            val log = "$msg error: $ret"
            Log.e(TAG, log)
            return false
        }
        return true
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
        nasolabialFolds: Float? = null
    ){
        // 磨皮
        smooth?.let { renderManager.updateComposerNodes(beautyNodePath, "smooth", it) }

        // 美白
        whiten?.let { renderManager.updateComposerNodes(beautyNodePath, "whiten", it) }

        // 红润
        redden?.let { renderManager.updateComposerNodes(beautyNodePath, "sharp", it) }


        // 瘦脸
        thinFace?.let { renderManager.updateComposerNodes(reSharpNodePath, "Internal_Deform_Overall", it) }

        // 大眼
        enlargeEye?.let { renderManager.updateComposerNodes(reSharpNodePath, "Internal_Deform_Eye", it) }


        // 瘦颧骨
        shrinkCheekbone?.let { renderManager.updateComposerNodes(reSharpNodePath, "Internal_Deform_Zoom_Cheekbone", it) }

        // 下颌骨
        shrinkJawbone?.let { renderManager.updateComposerNodes(reSharpNodePath, "Internal_Deform_Zoom_Jawbone", it) }

        // 美牙
        whiteTeeth?.let { renderManager.updateComposerNodes(reSharpNodePath, "BEF_BEAUTY_WHITEN_TEETH", it) }

        // 额头
        hairlineHeight?.let { renderManager.updateComposerNodes(reSharpNodePath, "Internal_Deform_Forehead", it) }

        // 瘦鼻
        narrowNose?.let { renderManager.updateComposerNodes(reSharpNodePath, "Internal_Deform_Nose", it) }

        // 嘴形
        mouthSize?.let { renderManager.updateComposerNodes(reSharpNodePath, "Internal_Deform_ZoomMouth", it) }

        // 下巴
        chinLength?.let { renderManager.updateComposerNodes(reSharpNodePath, "Internal_Deform_Chin", it) }

        // 亮眼
        brightEye?.let { renderManager.updateComposerNodes(beauty4ItemsNodePath, "BEF_BEAUTY_BRIGHTEN_EYE", it) }

        // 祛黑眼圈
        darkCircles?.let { renderManager.updateComposerNodes(beauty4ItemsNodePath, "BEF_BEAUTY_REMOVE_POUCH", it) }

        // 祛法令纹
        nasolabialFolds?.let { renderManager.updateComposerNodes(beauty4ItemsNodePath, "BEF_BEAUTY_SMILES_FOLDS", it) }

    }
}