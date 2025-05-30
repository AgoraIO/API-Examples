package io.agora.api.example.examples.advanced.beauty

import android.content.Context
import com.google.android.exoplayer2.util.Log
import io.agora.api.example.examples.advanced.beauty.utils.FileUtils.copyAssets
import io.agora.rtc2.Constants
import io.agora.rtc2.IVideoEffectObject
import io.agora.rtc2.RtcEngine

/**
 * Agora beauty 2.0
 */
object AgoraBeautySDK {
    private const val TAG = "AgoraBeautySDK"
    private var rtcEngine: RtcEngine? = null
    private var videoEffectObject: IVideoEffectObject? = null

    // Beauty config
    @JvmStatic
    val beautyConfig = BeautyConfig()


    @JvmStatic
    fun initBeautySDK(context: Context, rtcEngine: RtcEngine): Boolean {
        rtcEngine.enableExtension("agora_video_filters_clear_vision", "clear_vision", true)
        val storagePath = context.getExternalFilesDir("")?.absolutePath ?: return false
        val modelsPath = "$storagePath/beauty_agora/beauty_material.bundle"
        copyAssets(context, "beauty_agora/beauty_material.bundle", modelsPath)
        videoEffectObject = rtcEngine.createVideoEffectObject(
            "$modelsPath/beauty_material_v2.0.0",
            Constants.MediaSourceType.PRIMARY_CAMERA_SOURCE
        )
        // Fix lipstick ghosting issue
        rtcEngine.setParameters("{\"rtc.video.yuvconverter_enable_hardware_buffer\":true}")
        Log.d(TAG, "initBeautySDK called")
        return true
    }

    @JvmStatic
    fun unInitBeautySDK() {
        Log.d(TAG, "unInitBeautySDK called")
        beautyConfig.reset()
        rtcEngine?.let {
            videoEffectObject?.let { vEffectObject ->
                it.destroyVideoEffectObject(vEffectObject)
            }
            it.enableExtension(
                "agora_video_filters_clear_vision",
                "clear_vision",
                false,
                Constants.MediaSourceType.PRIMARY_CAMERA_SOURCE
            )
        }
    }

    @JvmStatic
    fun saveBeautyEffect() {
        videoEffectObject?.performVideoEffectAction(
            IVideoEffectObject.VIDEO_EFFECT_NODE_ID.BEAUTY.value,
            IVideoEffectObject.VIDEO_EFFECT_ACTION.SAVE
        )
    }

    @JvmStatic
    fun resetBeautyEffect() {
        videoEffectObject?.performVideoEffectAction(
            IVideoEffectObject.VIDEO_EFFECT_NODE_ID.BEAUTY.value,
            IVideoEffectObject.VIDEO_EFFECT_ACTION.RESET
        )
    }

    @JvmStatic
    fun saveMakeupEffect() {
        videoEffectObject?.performVideoEffectAction(
            IVideoEffectObject.VIDEO_EFFECT_NODE_ID.STYLE_MAKEUP.value,
            IVideoEffectObject.VIDEO_EFFECT_ACTION.SAVE
        )
    }

    @JvmStatic
    fun resetMakeupEffect() {
        videoEffectObject?.performVideoEffectAction(
            IVideoEffectObject.VIDEO_EFFECT_NODE_ID.STYLE_MAKEUP.value,
            IVideoEffectObject.VIDEO_EFFECT_ACTION.RESET
        )
    }

    @JvmStatic
    fun saveFilterEffect() {
        videoEffectObject?.performVideoEffectAction(
            IVideoEffectObject.VIDEO_EFFECT_NODE_ID.FILTER.value,
            IVideoEffectObject.VIDEO_EFFECT_ACTION.SAVE
        )
    }

    @JvmStatic
    fun resetFilterEffect() {
        videoEffectObject?.performVideoEffectAction(
            IVideoEffectObject.VIDEO_EFFECT_NODE_ID.FILTER.value,
            IVideoEffectObject.VIDEO_EFFECT_ACTION.RESET
        )
    }


    class BeautyConfig {

        /**
         * @param option face_shape_area_option:
         *               face_shape_beauty_option:
         *               beauty_effect_option: Basic beauty
         *               face_buffing_option: Basic beauty extension. if beauty_effect_option close, face_buffing_option will have no effect.
         *               makeup_options: makeup
         *               style_makeup_option：makeup style intensity
         *               filter_effect_option: filter
         * @param key
         * @param value
         */

        //================================ basic beauty  start ========================
        var basicBeautyEnable = false
            get() = videoEffectObject?.getVideoEffectBoolParam("beauty_effect_option", "enable") ?: false
            set(value) {
                field = value
                val vEffectObject = videoEffectObject ?: return
                // Need to add beauty node first if not added, only basic beauty without makeup, close makeup
                if (value) {
                    if (beautyShapeStyle == null) {
                        vEffectObject.addOrUpdateVideoEffect(IVideoEffectObject.VIDEO_EFFECT_NODE_ID.BEAUTY.value, "")
                        vEffectObject.setVideoEffectBoolParam("face_shape_beauty_option", "enable", false)
                    }
                }
                vEffectObject.setVideoEffectBoolParam("beauty_effect_option", "enable", value)
            }

        var smoothness = 0.9f
            get() = videoEffectObject?.getVideoEffectFloatParam("beauty_effect_option", "smoothness") ?: 0.9f
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectFloatParam("beauty_effect_option", "smoothness", value)
            }

        var lightness = 0.9f
            get() = videoEffectObject?.getVideoEffectFloatParam("beauty_effect_option", "lightness") ?: 0.9f
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectFloatParam("beauty_effect_option", "lightness", value)
            }

        var redness = 1.0f
            get() = videoEffectObject?.getVideoEffectFloatParam("beauty_effect_option", "redness") ?: 1.0f
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectFloatParam("beauty_effect_option", "redness", value)
            }

        var sharpness = 1.0f
            get() = videoEffectObject?.getVideoEffectFloatParam("beauty_effect_option", "sharpness") ?: 1.0f
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectFloatParam("beauty_effect_option", "sharpness", value)
            }

        /**
         * 0 Low contrast
         * 1 Normal contrast
         * 2 High contrast
         */
        var contrast = 1
            get() = videoEffectObject?.getVideoEffectIntParam("beauty_effect_option", "contrast") ?: 1
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectIntParam("beauty_effect_option", "contrast", value)
            }

        var contrastStrength = 1.0f
            get() = videoEffectObject?.getVideoEffectFloatParam("beauty_effect_option", "contrast_strength") ?: 1.0f
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectFloatParam(
                    "beauty_effect_option",
                    "contrast_strength",
                    value
                )
            }
        //================================ basic beauty  end ========================

        //================================ extension beauty  start ========================
        var eyePouch = 0.5f
            get() = videoEffectObject?.getVideoEffectFloatParam("face_buffing_option", "eye_pouch") ?: 0.5f
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectFloatParam("face_buffing_option", "eye_pouch", value)
            }

        var brightenEye = 0.9f
            get() = videoEffectObject?.getVideoEffectFloatParam("face_buffing_option", "brighten_eye") ?: 0.9f
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectFloatParam("face_buffing_option", "brighten_eye", value)
            }

        var nasolabialFold = 0.7f
            get() = videoEffectObject?.getVideoEffectFloatParam("face_buffing_option", "nasolabial_fold") ?: 0.7f
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectFloatParam("face_buffing_option", "nasolabial_fold", value)
            }

        var whitenTeeth = 0.7f
            get() = videoEffectObject?.getVideoEffectFloatParam("face_buffing_option", "whiten_teeth") ?: 0.7f
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectFloatParam("face_buffing_option", "whiten_teeth", value)
            }
        //================================ extension beauty  end ========================


        //================================ beauty shape start ========================
        // Face shape switch
        var beautyShapeEnable: Boolean = false
            get() = videoEffectObject?.getVideoEffectBoolParam("face_shape_beauty_option", "enable") ?: false
            set(value) {
                field = value
            }

        var beautyShapeStyle: String? = null
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                if (value == null) {
                    val ret = effectObj.removeVideoEffect(IVideoEffectObject.VIDEO_EFFECT_NODE_ID.BEAUTY.value)
                    Log.d(TAG, "beautyShapeStyle removeVideoEffect ret: $ret")
                } else {
                    val ret = effectObj.addOrUpdateVideoEffect(
                        IVideoEffectObject.VIDEO_EFFECT_NODE_ID.BEAUTY.value, value
                    )
                    Log.d(TAG, "beautyShapeStyle addOrUpdateVideoEffect ret: $ret")
                }
            }

        // Face shape style intensity
        var beautyShapeStrength = 50
            get() = videoEffectObject?.getVideoEffectIntParam("face_shape_beauty_option", "intensity") ?: 50
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectIntParam("face_shape_beauty_option", "intensity", value)
            }

        //================================ beauty shape end ========================

        // Makeup switch
        var makeUpEnable: Boolean = false
            get() = videoEffectObject?.getVideoEffectBoolParam("makeup_options", "enable_mu") ?: false
            set(value) {
                field = value
            }

        // Makeup
        var beautyMakeupStyle: String? = null
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                if (value == null) {
                    val ret = effectObj.removeVideoEffect(IVideoEffectObject.VIDEO_EFFECT_NODE_ID.STYLE_MAKEUP.value)
                    Log.d(TAG, "beautyMakeupStyle removeVideoEffect ret: $ret")
                } else {
                    val ret = effectObj.addOrUpdateVideoEffect(
                        IVideoEffectObject.VIDEO_EFFECT_NODE_ID.STYLE_MAKEUP.value, value
                    )
                    Log.d(TAG, "beautyMakeupStyle addOrUpdateVideoEffect ret: $ret")
                }
            }

        // Makeup style intensity
        var beautyMakeupStrength = 0.95f
            get() = videoEffectObject?.getVideoEffectFloatParam("style_makeup_option", "styleIntensity") ?: 0.95f
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectFloatParam("style_makeup_option", "styleIntensity", value)
            }

        // Facial style
        var facialStyle = 5
            get() = videoEffectObject?.getVideoEffectIntParam("makeup_options", "facialStyle") ?: 5
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectIntParam("makeup_options", "facialStyle", value)
            }

        // Facial intensity
        var facialStrength = 1.0f
            get() = videoEffectObject?.getVideoEffectFloatParam("makeup_options", "facialStrength") ?: 1.0f
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectFloatParam("makeup_options", "facialStrength", value)
            }

        // Wocan style
        var wocanStyle = 3
            get() = videoEffectObject?.getVideoEffectIntParam("makeup_options", "wocanStyle") ?: 3
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectIntParam("makeup_options", "wocanStyle", value)
            }

        // Wocan intensity
        var wocanStrength = 1.0f
            get() = videoEffectObject?.getVideoEffectFloatParam("makeup_options", "wocanStrength") ?: 1.0f
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectFloatParam("makeup_options", "wocanStrength", value)
            }

        // Eyebrow style
        var browStyle = 2
            get() = videoEffectObject?.getVideoEffectIntParam("makeup_options", "browStyle") ?: 2
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectIntParam("makeup_options", "browStyle", value)
            }

        // Eyebrow color
        var browColor = 2
            get() = videoEffectObject?.getVideoEffectIntParam("makeup_options", "browColor") ?: 2
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectIntParam("makeup_options", "browColor", value)
            }

        // Eyebrow intensity
        var browStrength = 1.0f
            get() = videoEffectObject?.getVideoEffectFloatParam("makeup_options", "browStrength") ?: 1.0f
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectFloatParam("makeup_options", "browStrength", value)
            }

        // Eyelash style
        var lashStyle = 5
            get() = videoEffectObject?.getVideoEffectIntParam("makeup_options", "lashStyle") ?: 5
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectIntParam("makeup_options", "lashStyle", value)
            }

        // Eyelash color
        var lashColor = 1
            get() = videoEffectObject?.getVideoEffectIntParam("makeup_options", "lashColor") ?: 1
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectIntParam("makeup_options", "lashColor", value)
            }

        // Eyelash intensity
        var lashStrength = 1.0f
            get() = videoEffectObject?.getVideoEffectFloatParam("makeup_options", "lashStrength") ?: 1.0f
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectFloatParam("makeup_options", "lashStrength", value)
            }

        // Eyeshadow style
        var shadowStyle = 6
            get() = videoEffectObject?.getVideoEffectIntParam("makeup_options", "shadowStyle") ?: 6
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectIntParam("makeup_options", "shadowStyle", value)
            }

        // Eyeshadow intensity
        var shadowStrength = 1.0f
            get() = videoEffectObject?.getVideoEffectFloatParam("makeup_options", "shadowStrength") ?: 1.0f
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectFloatParam("makeup_options", "shadowStrength", value)
            }

        // Pupil style
        var pupilStyle = 2
            get() = videoEffectObject?.getVideoEffectIntParam("makeup_options", "pupilStyle") ?: 2
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectIntParam("makeup_options", "pupilStyle", value)
            }

        // Pupil intensity
        var pupilStrength = 1.0f
            get() = videoEffectObject?.getVideoEffectFloatParam("makeup_options", "pupilStrength") ?: 1.0f
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectFloatParam("makeup_options", "pupilStrength", value)
            }

        // Blush style
        var blushStyle = 2
            get() = videoEffectObject?.getVideoEffectIntParam("makeup_options", "blushStyle") ?: 2
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectIntParam("makeup_options", "blushStyle", value)
            }

        // Blush color
        var blushColor = 5
            get() = videoEffectObject?.getVideoEffectIntParam("makeup_options", "blushColor") ?: 5
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectIntParam("makeup_options", "blushColor", value)
            }

        // Blush intensity
        var blushStrength = 1.0f
            get() = videoEffectObject?.getVideoEffectFloatParam("makeup_options", "blushStrength") ?: 1.0f
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectFloatParam("makeup_options", "blushStrength", value)
            }

        // Lipstick style
        var lipStyle = 2
            get() = videoEffectObject?.getVideoEffectIntParam("makeup_options", "lipStyle") ?: 2
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectIntParam("makeup_options", "lipStyle", value)
            }

        // Lipstick color
        var lipColor = 5
            get() = videoEffectObject?.getVideoEffectIntParam("makeup_options", "lipColor") ?: 5
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectIntParam("makeup_options", "lipColor", value)
            }

        // Lipstick intensity
        var lipStrength = 1.0f
            get() = videoEffectObject?.getVideoEffectFloatParam("makeup_options", "lipStrength") ?: 1.0f
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectFloatParam("makeup_options", "lipStrength", value)
            }

        // Filter switch
        var filterEnable: Boolean = false
            get() = videoEffectObject?.getVideoEffectBoolParam("filter_effect_option", "enable") ?: false
            set(value) {
                field = value
            }

        // Filter
        var beautyFilter: String? = null
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                if (value == null) {
                    val ret = effectObj.removeVideoEffect(IVideoEffectObject.VIDEO_EFFECT_NODE_ID.FILTER.value)
                    Log.d(TAG, "beautyFilter removeVideoEffect ret: $ret")
                } else {
                    val ret = effectObj.addOrUpdateVideoEffect(
                        IVideoEffectObject.VIDEO_EFFECT_NODE_ID.FILTER.value, value
                    )
                    Log.d(TAG, "beautyFilter addOrUpdateVideoEffect ret: $ret")
                }
            }

        // Filter intensity
        var filterStrength = 0.5f
            get() = videoEffectObject?.getVideoEffectFloatParam("filter_effect_option", "strength") ?: 0.5f
            set(value) {
                field = value
                val effectObj = videoEffectObject ?: return
                effectObj.setVideoEffectFloatParam("filter_effect_option", "strength", value)
            }

        internal fun reset() {
//            smoothness = 0.9f
//            lightness = 0.9f
//            redness = 1.0f
//            contrast = 1
//            sharpness = 1.0f
//            contrastStrength = 1.0f
//            eyePouch = 0.5f
//            brightenEye = 0.9f
//            nasolabialFold = 0.5f
//            whitenTeeth = 0.7f

            beautyShapeStyle = null
            beautyMakeupStyle = null
            beautyFilter = null
        }
    }
}