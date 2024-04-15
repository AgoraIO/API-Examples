package io.agora.api.example.compose.data

import io.agora.rtc2.video.VideoEncoderConfiguration
import io.agora.rtc2.video.VideoEncoderConfiguration.FRAME_RATE
import io.agora.rtc2.video.VideoEncoderConfiguration.ORIENTATION_MODE
import io.agora.rtc2.video.VideoEncoderConfiguration.VideoDimensions

object SettingPreferences {

    private var videoDimensions : VideoDimensions? = null
    private var videoFrameRate : FRAME_RATE? = null
    private var orientationMode: ORIENTATION_MODE? = null

    fun setVideoDimensions(dimensions: VideoDimensions?) {
        videoDimensions = dimensions
    }

    fun getVideoDimensions(): VideoDimensions{
        return videoDimensions ?: VideoEncoderConfiguration.VD_960x540
    }

    fun setVideoFrameRate(frameRate: FRAME_RATE?) {
        videoFrameRate = frameRate
    }

    fun getVideoFrameRate(): FRAME_RATE{
        return videoFrameRate ?: FRAME_RATE.FRAME_RATE_FPS_15
    }

    fun setOrientationMode(mode: ORIENTATION_MODE?) {
        orientationMode = mode
    }

    fun getOrientationMode(): ORIENTATION_MODE {
        return orientationMode ?: ORIENTATION_MODE.ORIENTATION_MODE_ADAPTIVE
    }

}