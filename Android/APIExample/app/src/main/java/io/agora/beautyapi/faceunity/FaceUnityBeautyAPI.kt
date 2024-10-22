/*
 * MIT License
 *
 * Copyright (c) 2023 Agora Community
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

package io.agora.beautyapi.faceunity

import android.content.Context
import android.view.View
import com.faceunity.core.faceunity.FURenderKit
import io.agora.base.VideoFrame
import io.agora.rtc2.Constants
import io.agora.rtc2.RtcEngine

const val VERSION = "1.0.8"

enum class CaptureMode{
    Agora,
    Custom
}

interface IEventCallback{

    fun onBeautyStats(stats: BeautyStats)
}

data class BeautyStats(
    val minCostMs:Long,
    val maxCostMs: Long,
    val averageCostMs: Long
)

enum class MirrorMode {

    MIRROR_LOCAL_REMOTE,
    MIRROR_LOCAL_ONLY,
    MIRROR_REMOTE_ONLY,
    MIRROR_NONE
}

data class CameraConfig(
    val frontMirror: MirrorMode = MirrorMode.MIRROR_LOCAL_REMOTE,
    val backMirror: MirrorMode = MirrorMode.MIRROR_NONE
)

data class Config(
    val context: Context,
    val rtcEngine: RtcEngine,
    val fuRenderKit: FURenderKit,
    val eventCallback: IEventCallback? = null,
    val captureMode: CaptureMode = CaptureMode.Agora,
    val statsDuration: Long = 1000,
    val statsEnable: Boolean = false,
    val cameraConfig: CameraConfig = CameraConfig()
)

enum class ErrorCode(val value: Int) {
    ERROR_OK(0),
    ERROR_HAS_NOT_INITIALIZED(101),
    ERROR_HAS_INITIALIZED(102),
    ERROR_HAS_RELEASED(103),
    ERROR_PROCESS_NOT_CUSTOM(104),
    ERROR_VIEW_TYPE_ERROR(105),
    ERROR_FRAME_SKIPPED(106),
}

enum class BeautyPreset {
    CUSTOM,
    DEFAULT
}

fun createFaceUnityBeautyAPI(): FaceUnityBeautyAPI = FaceUnityBeautyAPIImpl()

interface FaceUnityBeautyAPI {

    fun initialize(config: Config): Int

    fun enable(enable: Boolean): Int

    fun setupLocalVideo(view: View, renderMode: Int = Constants.RENDER_MODE_HIDDEN): Int

    fun onFrame(videoFrame: VideoFrame): Int

    fun updateCameraConfig(config: CameraConfig): Int

    fun isFrontCamera(): Boolean

    fun getMirrorApplied(): Boolean

    fun runOnProcessThread(run: ()->Unit)

    fun setParameters(key: String, value: String)

    fun release(): Int

}