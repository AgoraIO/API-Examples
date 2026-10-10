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

package io.agora.beautyapi.sensetime

import android.content.Context
import android.view.View
import com.softsugar.stmobile.STMobileEffectNative
import com.softsugar.stmobile.STMobileHumanActionNative
import io.agora.base.VideoFrame
import io.agora.rtc2.Constants
import io.agora.rtc2.RtcEngine

const val VERSION = "1.0.10"

enum class CaptureMode {
    /**
     * Use the internal raw data interface of Agora for processing.
     *
     */
    Agora,

    /**
     * In custom mode, you need to call the [io.agora.rtc2.video.IVideoFrameObserver] interface
     * yourself to pass the raw video frame to the BeautyAPI for processing.
     *
     */
    Custom
}

interface IEventCallback {

    /**
     * The statistics callback triggers once after each processed frame.
     *
     * @param stats Beauty statistics data.
     */
    fun onBeautyStats(stats: BeautyStats)
}

data class BeautyStats(
    val minCostMs: Long, // Minimum value within the statistics interval
    val maxCostMs: Long, // Maximum value within the statistics interval
    val averageCostMs: Long // Average value within the statistics interval
)

enum class MirrorMode {

    // Definition of no mirror normal view:
    // For the front camera, the captured view is flipped horizontally on the phone screen;
    // for the rear camera, it appears the same as seen on the phone.

    // Mirror both locally and remotely, default for front camera. Stickers are correctly oriented on both local and remote views.
    MIRROR_LOCAL_REMOTE,

    // Mirror only locally, no remote mirroring. Used in calling or e-commerce live streaming (ensures signage in the background appears correct).
    // Due to flipped local and remote views, one side's text/stickers will appear reversed.
    MIRROR_LOCAL_ONLY,

    // Mirror only remotely, no local mirroring. Stickers are correctly oriented on the remote view, mirrored locally.
    MIRROR_REMOTE_ONLY,

    // No mirroring for both local and remote views, default for rear camera. Stickers are correctly oriented on both views.
    MIRROR_NONE
}

data class CameraConfig(
    val frontMirror: MirrorMode = MirrorMode.MIRROR_LOCAL_REMOTE, // Default front camera mirror: both local and remote mirrored
    val backMirror: MirrorMode = MirrorMode.MIRROR_NONE // Default back camera mirror: neither local nor remote mirrored
)

data class Config(
    val context: Context, // Android context
    val rtcEngine: RtcEngine, // Agora RTC engine
    val stHandlers: STHandlers, // Beauty SDK handler
    val eventCallback: IEventCallback? = null, // Event callback
    val captureMode: CaptureMode = CaptureMode.Agora, // Capture mode
    val statsDuration: Long = 1000, // Stats interval duration
    val statsEnable: Boolean = false, // Enable stats or not
    val cameraConfig: CameraConfig = CameraConfig() // Camera mirror configuration
)

data class STHandlers(
    val effectNative: STMobileEffectNative,
    val humanActionNative: STMobileHumanActionNative
)

enum class ErrorCode(val value: Int) {
    ERROR_OK(0), // All operations are normal
    ERROR_HAS_NOT_INITIALIZED(101), // Called other APIs without initializing or after failed initialization
    ERROR_HAS_INITIALIZED(102), // Error when calling Initialize again after successful initialization
    ERROR_HAS_RELEASED(103), // Called other APIs after release has been invoked
    ERROR_PROCESS_NOT_CUSTOM(104), // Called onFrame to pass video frames externally when not in Custom processing mode
    ERROR_VIEW_TYPE_ERROR(105), // Error returned if the view type is incorrect when setupLocalVideo is called
    ERROR_FRAME_SKIPPED(106), // Returned in onFrame when a frame is skipped during processing
}

enum class BeautyPreset {
    CUSTOM, // Do not use the recommended beauty parameters
    DEFAULT // Default preset
}

fun createSenseTimeBeautyAPI(): SenseTimeBeautyAPI = SenseTimeBeautyAPIImpl()

interface SenseTimeBeautyAPI {

    /**
     * Initializes the API.
     *
     * @param config Configuration parameters
     * @return [ErrorCode] corresponding to the result of initialization
     */
    fun initialize(config: Config): Int

    /**
     * Enable/Disable beauty effects.
     *
     * @param enable true: Enable; false: Disable
     * @return [ErrorCode] corresponding to the result of the operation
     */
    fun enable(enable: Boolean): Int

    /**
     * Sets up local video rendering, with internal handling of mirror mode.
     *
     * @param view SurfaceView or TextureView for rendering the video
     * @param renderMode Scaling mode for rendering (e.g., Constants.RENDER_MODE_HIDDEN)
     * @return ErrorCode corresponding to the result of the operation
     */
    fun setupLocalVideo(view: View, renderMode: Int = Constants.RENDER_MODE_HIDDEN): Int

    /**
     * When ProcessMode == [CaptureMode.Custom], external input of raw video frames is required.
     *
     * @param videoFrame The raw video frame
     * @return [ErrorCode] corresponding to the result of the operation
     */
    fun onFrame(videoFrame: VideoFrame): Int

    /**
     * Sets the best default beauty parameters provided by Agora.
     *
     * @param preset The beauty preset, defaulting to [BeautyPreset.DEFAULT]
     * @return [ErrorCode] corresponding to the result of the operation
     */
    fun setBeautyPreset(preset: BeautyPreset = BeautyPreset.DEFAULT): Int

    /**
     * Updates the camera configuration.
     *
     * @param config New camera configuration to apply
     * @return [ErrorCode] corresponding to the result of the operation
     */
    fun updateCameraConfig(config: CameraConfig): Int

    /**
     * Checks if the current camera is the front camera.
     * Note: This returns an accurate value only during beauty processing; otherwise, it will always return true.
     *
     * @return true if the current camera is the front camera, false otherwise
     */
    fun isFrontCamera(): Boolean

    /**
     * Retrieves the current mirror status.
     *
     * @return true if mirroring is applied, false if it is not.
     */
    fun getMirrorApplied(): Boolean


    /**
     * Executes an operation within the processing thread.
     *
     * @param run The operation to execute.
     */
    fun runOnProcessThread(run: () -> Unit)

    /**
     * Private parameter configuration for internal API calls, primarily for testing.
     *
     * @param key The parameter key.
     * @param value The parameter value.
     */
    fun setParameters(key: String, value: String)

    /**
     * Releases resources. Once released, this instance can no longer be used.
     *
     * @return Refer to ErrorCode
     */
    fun release(): Int

}