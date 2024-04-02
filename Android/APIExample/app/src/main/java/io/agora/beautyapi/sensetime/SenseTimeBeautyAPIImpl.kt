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

import android.annotation.TargetApi
import android.graphics.Matrix
import android.opengl.GLES11Ext
import android.opengl.GLES20
import android.os.Build
import android.view.SurfaceView
import android.view.TextureView
import android.view.View
import com.softsugar.stmobile.STCommonNative
import com.softsugar.stmobile.params.STEffectBeautyType
import io.agora.base.TextureBufferHelper
import io.agora.base.VideoFrame
import io.agora.base.VideoFrame.I420Buffer
import io.agora.base.VideoFrame.SourceType
import io.agora.base.VideoFrame.TextureBuffer
import io.agora.base.internal.video.RendererCommon
import io.agora.base.internal.video.YuvConverter
import io.agora.base.internal.video.YuvHelper
import io.agora.beautyapi.sensetime.utils.LogUtils
import io.agora.beautyapi.sensetime.utils.StatsHelper
import io.agora.beautyapi.sensetime.utils.processor.IBeautyProcessor
import io.agora.beautyapi.sensetime.utils.processor.InputInfo
import io.agora.beautyapi.sensetime.utils.processor.createBeautyProcessor
import io.agora.rtc2.Constants
import io.agora.rtc2.gl.EglBaseProvider
import io.agora.rtc2.video.IVideoFrameObserver
import io.agora.rtc2.video.VideoCanvas
import java.nio.ByteBuffer
import java.util.Collections
import java.util.concurrent.Callable
import java.util.concurrent.Executors

class SenseTimeBeautyAPIImpl : SenseTimeBeautyAPI, IVideoFrameObserver {
    private val TAG = "SenseTimeBeautyAPIImpl"
    private val reportId = "scenarioAPI"
    private val reportCategory = "beauty_android_$VERSION"
    private var beautyMode = 0 // 0: 自动根据buffer类型切换，1：固定使用OES纹理，2：固定使用i420

    private var textureBufferHelper: TextureBufferHelper? = null
    private var nv21ByteBuffer: ByteBuffer? = null
    private var config: Config? = null
    private var enable: Boolean = false
    private var enableChange: Boolean = false
    private var isReleased: Boolean = false
    private var captureMirror = true
    private var renderMirror = false
    private var statsHelper: StatsHelper? = null
    private var skipFrame = 0
    private val workerThreadExecutor = Executors.newSingleThreadExecutor()
    private var beautyProcessor: IBeautyProcessor? = null
    private var isFrontCamera = true
    private var cameraConfig = CameraConfig()
    private var localVideoRenderMode = Constants.RENDER_MODE_HIDDEN
    private val pendingProcessRunList = Collections.synchronizedList(mutableListOf<()->Unit>())

    private enum class ProcessSourceType{
        UNKNOWN,
        TEXTURE_OES_API26,
        TEXTURE_2D_API26,
        TEXTURE_OES,
        TEXTURE_2D,
        I420,
    }
    private var currProcessSourceType = ProcessSourceType.UNKNOWN

    override fun initialize(config: Config): Int {
        if (this.config != null) {
            LogUtils.e(TAG, "initialize >> The beauty api has been initialized!")
            return ErrorCode.ERROR_HAS_INITIALIZED.value
        }
        this.config = config
        this.cameraConfig = config.cameraConfig
        if (config.captureMode == CaptureMode.Agora) {
            config.rtcEngine.registerVideoFrameObserver(this)
        }
        statsHelper = StatsHelper(config.statsDuration) {
            this.config?.eventCallback?.onBeautyStats(it)
        }
        cameraConfig = CameraConfig(config.cameraConfig.frontMirror, config.cameraConfig.backMirror)
        LogUtils.i(TAG, "initialize >> config = $config")
        LogUtils.i(TAG, "initialize >> beauty api version=$VERSION, beauty sdk version=${STCommonNative.getVersion()}")
        // config.rtcEngine.setParameters("{\"rtc.qos_for_test_purpose\":101}") // 实时上报
        config.rtcEngine.sendCustomReportMessage(reportId, reportCategory, "initialize", "config=$config", 0)
        return ErrorCode.ERROR_OK.value
    }

    override fun enable(enable: Boolean): Int {
        LogUtils.i(TAG, "enable >> enable = $enable")
        if (config == null) {
            LogUtils.e(TAG, "enable >> The beauty api has not been initialized!")
            return ErrorCode.ERROR_HAS_NOT_INITIALIZED.value
        }
        if (isReleased) {
            LogUtils.e(TAG, "enable >> The beauty api has been released!")
            return ErrorCode.ERROR_HAS_RELEASED.value
        }
        if(config?.captureMode == CaptureMode.Custom){
            skipFrame = 2
            LogUtils.i(TAG, "enable >> skipFrame = $skipFrame")
        }
        config?.rtcEngine?.sendCustomReportMessage(reportId, reportCategory, "enable", "enable=$enable", 0)

        if(this.enable != enable){
            this.enable = enable
            this.enableChange = true
            LogUtils.i(TAG, "enable >> enableChange")
        }

        return ErrorCode.ERROR_OK.value
    }

    override fun setupLocalVideo(view: View, renderMode: Int): Int {
        val rtcEngine = config?.rtcEngine
        if(rtcEngine == null){
            LogUtils.e(TAG, "setupLocalVideo >> The beauty api has not been initialized!")
            return ErrorCode.ERROR_HAS_NOT_INITIALIZED.value
        }
        LogUtils.i(TAG, "setupLocalVideo >> view=$view, renderMode=$renderMode")
        localVideoRenderMode = renderMode
        rtcEngine.sendCustomReportMessage(reportId, reportCategory, "enable", "view=$view, renderMode=$renderMode", 0)
        if(view is TextureView || view is SurfaceView){
            val canvas = VideoCanvas(view, renderMode, 0)
            canvas.mirrorMode = Constants.VIDEO_MIRROR_MODE_DISABLED
            rtcEngine.setupLocalVideo(canvas)
            return ErrorCode.ERROR_OK.value
        }
        return ErrorCode.ERROR_VIEW_TYPE_ERROR.value
    }

    override fun onFrame(videoFrame: VideoFrame): Int {
        val conf = config
        if(conf == null){
            LogUtils.e(TAG, "onFrame >> The beauty api has not been initialized!")
            return ErrorCode.ERROR_HAS_NOT_INITIALIZED.value
        }
        if (isReleased) {
            LogUtils.e(TAG, "onFrame >> The beauty api has been released!")
            return ErrorCode.ERROR_HAS_RELEASED.value
        }
        if (conf.captureMode != CaptureMode.Custom) {
            LogUtils.e(TAG, "onFrame >> The capture mode is not Custom!")
            return ErrorCode.ERROR_PROCESS_NOT_CUSTOM.value
        }
        if (processBeauty(videoFrame)) {
            return ErrorCode.ERROR_OK.value
        }
        LogUtils.i(TAG, "onFrame >> Skip Frame.")
        return ErrorCode.ERROR_FRAME_SKIPPED.value
    }

    override fun setBeautyPreset(preset: BeautyPreset): Int {
        val effectNative = config?.stHandlers?.effectNative
        if(effectNative == null){
            LogUtils.e(TAG, "setBeautyPreset >> The beauty api has not been initialized!")
            return ErrorCode.ERROR_HAS_NOT_INITIALIZED.value
        }
        if (isReleased) {
            LogUtils.e(TAG, "setBeautyPreset >> The beauty api has been released!")
            return ErrorCode.ERROR_HAS_RELEASED.value
        }
        LogUtils.i(TAG, "setBeautyPreset >> preset = $preset")
        config?.rtcEngine?.sendCustomReportMessage(reportId, reportCategory, "enable", "preset=$preset", 0)

        val enable = preset == BeautyPreset.DEFAULT
        workerThreadExecutor.submit {
            // 锐化
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_TONE_SHARPEN,
                if(enable) 0.5f else 0.0f
            )
            // 清晰度
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_TONE_CLEAR,
                if(enable) 1.0f else 0.0f
            )
            // 磨皮
            effectNative.setBeautyMode(
                STEffectBeautyType.EFFECT_BEAUTY_BASE_FACE_SMOOTH,
                STEffectBeautyType.SMOOTH2_MODE
            )
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_BASE_FACE_SMOOTH,
                if(enable) 0.55f else 0.0f
            )
            // 美白
            effectNative.setBeautyMode(
                STEffectBeautyType.EFFECT_BEAUTY_BASE_WHITTEN,
                STEffectBeautyType.WHITENING3_MODE
            )
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_BASE_WHITTEN,
                if(enable) 0.2f else 0.0f
            )
            // 瘦脸
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_THIN_FACE,
                if(enable) 0.4f else 0.0f
            )
            // 大眼
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_RESHAPE_ENLARGE_EYE,
                if(enable) 0.3f else 0.0f
            )
            // 红润
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_BASE_REDDEN,
                if(enable) 0.0f else 0.0f
            )
            // 瘦颧骨
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_SHRINK_CHEEKBONE,
                if(enable) 0.0f else 0.0f
            )
            // 下颌骨
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_SHRINK_JAWBONE,
                if(enable) 0.0f else 0.0f
            )
            // 美牙
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_WHITE_TEETH,
                if(enable) 0.0f else 0.0f
            )
            // 额头
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_HAIRLINE_HEIGHT,
                if(enable) 0.0f else 0.0f
            )
            // 瘦鼻
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_NARROW_NOSE,
                if(enable) 0.0f  else 0.0f
            )
            // 嘴形
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_MOUTH_SIZE,
                if(enable) 0.0f else 0.0f
            )
            // 下巴
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_CHIN_LENGTH,
                if(enable) 0.0f else 0.0f
            )
            // 亮眼
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_BRIGHT_EYE,
                if(enable) 0.0f else 0.0f
            )
            // 祛黑眼圈
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_REMOVE_DARK_CIRCLES,
                if(enable) 0.0f else 0.0f
            )
            // 祛法令纹
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_PLASTIC_REMOVE_NASOLABIAL_FOLDS,
                if(enable) 0.0f else 0.0f
            )
            // 饱和度
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_TONE_SATURATION,
                if(enable) 0.0f else 0.0f
            )
            // 对比度
            effectNative.setBeautyStrength(
                STEffectBeautyType.EFFECT_BEAUTY_TONE_CONTRAST,
                if(enable) 0.0f else 0.0f
            )
        }
        return ErrorCode.ERROR_OK.value
    }

    override fun runOnProcessThread(run: () -> Unit) {
        if (config == null) {
            LogUtils.e(TAG, "runOnProcessThread >> The beauty api has not been initialized!")
            return
        }
        if (isReleased) {
            LogUtils.e(TAG, "runOnProcessThread >> The beauty api has been released!")
            return
        }
        if (textureBufferHelper?.handler?.looper?.thread == Thread.currentThread()) {
            run.invoke()
        } else if (textureBufferHelper != null) {
            textureBufferHelper?.handler?.post(run)
        } else {
            pendingProcessRunList.add(run)
        }
    }

    override fun updateCameraConfig(config: CameraConfig): Int {
        LogUtils.i(TAG, "updateCameraConfig >> oldCameraConfig=$cameraConfig, newCameraConfig=$config")
        cameraConfig = CameraConfig(config.frontMirror, config.backMirror)
        this.config?.rtcEngine?.sendCustomReportMessage(reportId, reportCategory, "updateCameraConfig", "config=$config", 0)

        return ErrorCode.ERROR_OK.value
    }

    override fun isFrontCamera() = isFrontCamera

    override fun setParameters(key: String, value: String) {
        when(key){
            "beauty_mode" -> beautyMode = value.toInt()
        }
    }

    override fun release(): Int {
        val conf = config
        if(conf == null){
            LogUtils.e(TAG, "release >> The beauty api has not been initialized!")
            return ErrorCode.ERROR_HAS_NOT_INITIALIZED.value
        }
        if (isReleased) {
            LogUtils.e(TAG, "setBeautyPreset >> The beauty api has been released!")
            return ErrorCode.ERROR_HAS_RELEASED.value
        }
        if (conf.captureMode == CaptureMode.Agora) {
            conf.rtcEngine.registerVideoFrameObserver(null)
        }
        conf.rtcEngine.sendCustomReportMessage(reportId, reportCategory, "release", "", 0)

        LogUtils.i(TAG, "release")
        isReleased = true
        workerThreadExecutor.shutdown()
        textureBufferHelper?.let {
            textureBufferHelper = null
            it.handler.removeCallbacksAndMessages(null)
            it.invoke {
                beautyProcessor?.release()
                null
            }
            it.dispose()
        }
        statsHelper?.reset()
        statsHelper = null
        pendingProcessRunList.clear()
        return ErrorCode.ERROR_OK.value
    }

    private fun processBeauty(videoFrame: VideoFrame): Boolean {
        if (isReleased) {
            LogUtils.e(TAG, "processBeauty >> The beauty api has been released!")
            return false
        }


        val cMirror =
            if (isFrontCamera) {
                when (cameraConfig.frontMirror) {
                    MirrorMode.MIRROR_LOCAL_REMOTE -> true
                    MirrorMode.MIRROR_LOCAL_ONLY -> false
                    MirrorMode.MIRROR_REMOTE_ONLY -> true
                    MirrorMode.MIRROR_NONE -> false
                }
            } else {
                when (cameraConfig.backMirror) {
                    MirrorMode.MIRROR_LOCAL_REMOTE -> true
                    MirrorMode.MIRROR_LOCAL_ONLY -> false
                    MirrorMode.MIRROR_REMOTE_ONLY -> true
                    MirrorMode.MIRROR_NONE -> false
                }
            }
        val rMirror =
            if (isFrontCamera) {
                when (cameraConfig.frontMirror) {
                    MirrorMode.MIRROR_LOCAL_REMOTE -> false
                    MirrorMode.MIRROR_LOCAL_ONLY -> true
                    MirrorMode.MIRROR_REMOTE_ONLY -> true
                    MirrorMode.MIRROR_NONE -> false
                }
            } else {
                when (cameraConfig.backMirror) {
                    MirrorMode.MIRROR_LOCAL_REMOTE -> false
                    MirrorMode.MIRROR_LOCAL_ONLY -> true
                    MirrorMode.MIRROR_REMOTE_ONLY -> true
                    MirrorMode.MIRROR_NONE -> false
                }
            }
        if (captureMirror != cMirror || renderMirror != rMirror) {
            LogUtils.w(TAG, "processBeauty >> enable=$enable, captureMirror=$captureMirror->$cMirror, renderMirror=$renderMirror->$rMirror")
            captureMirror = cMirror
            if(renderMirror != rMirror){
                renderMirror = rMirror
                config?.rtcEngine?.setLocalRenderMode(
                    localVideoRenderMode,
                    if(renderMirror) Constants.VIDEO_MIRROR_MODE_ENABLED else Constants.VIDEO_MIRROR_MODE_DISABLED
                )
            }
            textureBufferHelper?.invoke {
                beautyProcessor?.reset()
            }
            return false
        }

        val oldIsFrontCamera = isFrontCamera
        isFrontCamera = videoFrame.sourceType == SourceType.kFrontCamera
        if(oldIsFrontCamera != isFrontCamera){
            LogUtils.w(TAG, "processBeauty >> oldIsFrontCamera=$oldIsFrontCamera, isFrontCamera=$isFrontCamera")
            return false
        }

        if(enableChange){
            enableChange = false
            textureBufferHelper?.invoke {
                beautyProcessor?.reset()
            }
        }

        if(!enable){
            return true
        }

        if (textureBufferHelper == null) {
            textureBufferHelper = TextureBufferHelper.create(
                "STRender",
                EglBaseProvider.instance().rootEglBase.eglBaseContext
            )
            textureBufferHelper?.invoke {
                synchronized(pendingProcessRunList){
                    val iterator = pendingProcessRunList.iterator()
                    while (iterator.hasNext()){
                        iterator.next().invoke()
                        iterator.remove()
                    }
                }
            }
            LogUtils.i(TAG, "processBeauty >> create texture buffer, beautyMode=$beautyMode")
        }

        val startTime = System.currentTimeMillis()

        val processTexId = when(beautyMode){
            1 -> processBeautyTexture(videoFrame)
            2 -> processBeautyI420(videoFrame)
            else -> processBeautyAuto(videoFrame)
        }
        if(config?.statsEnable == true){
            val costTime = System.currentTimeMillis() - startTime
            statsHelper?.once(costTime)
        }

        if (processTexId < 0) {
            LogUtils.w(TAG, "processBeauty >> processTexId < 0")
            return false
        }

        if(skipFrame > 0){
            skipFrame --
            LogUtils.w(TAG, "processBeauty >> skipFrame=$skipFrame")
            return false
        }

        val processBuffer: TextureBuffer = textureBufferHelper?.wrapTextureBuffer(
            videoFrame.rotatedWidth,
            videoFrame.rotatedHeight,
            TextureBuffer.Type.RGB,
            processTexId,
            Matrix()
        ) ?: return false
        videoFrame.replaceBuffer(processBuffer, 0, videoFrame.timestampNs)
        return true
    }

    private fun processBeautyAuto(videoFrame: VideoFrame): Int {
        val buffer = videoFrame.buffer
        return if(buffer is TextureBuffer){
            processBeautyTexture(videoFrame)
        } else {
            processBeautyI420(videoFrame)
        }
    }

    private fun mayCreateBeautyProcess(){
        val stHandlers = config?.stHandlers ?: return

        if(beautyProcessor == null){
            beautyProcessor = createBeautyProcessor().apply {
                initialize(stHandlers.effectNative, stHandlers.humanActionNative)
            }
        }
    }

    @TargetApi(26)
    private fun processBeautyTextureAPI26(videoFrame: VideoFrame): Int{
        val texBufferHelper = textureBufferHelper ?: return -1
        val buffer = videoFrame.buffer as? TextureBuffer ?: return -1
        val width = buffer.width
        val height = buffer.height

        when(buffer.type){
            TextureBuffer.Type.OES -> {
                if(currProcessSourceType != ProcessSourceType.TEXTURE_OES_API26){
                    LogUtils.i(TAG, "processBeautyAuto >> process source type change old=$currProcessSourceType, new=${ProcessSourceType.TEXTURE_OES_API26}")
                    currProcessSourceType = ProcessSourceType.TEXTURE_OES_API26
                }
            }
            else -> {
                if(currProcessSourceType != ProcessSourceType.TEXTURE_2D_API26){
                    LogUtils.i(TAG, "processBeautyAuto >> process source type change old=$currProcessSourceType, new=${ProcessSourceType.TEXTURE_2D_API26}")
                    currProcessSourceType = ProcessSourceType.TEXTURE_2D_API26
                }
            }
        }

        val matrix = RendererCommon.convertMatrixFromAndroidGraphicsMatrix(buffer.transformMatrix)
        return texBufferHelper.invoke(Callable {
            mayCreateBeautyProcess()
            return@Callable beautyProcessor?.process(
                InputInfo(
                    width = width,
                    height = height,
                    cameraOrientation = videoFrame.rotation,
                    isFrontCamera = isFrontCamera,
                    isMirror = (isFrontCamera && !captureMirror) || (!isFrontCamera && captureMirror),
                    timestamp = videoFrame.timestampNs,
                    textureId = buffer.textureId,
                    textureType = when (buffer.type) {
                        TextureBuffer.Type.OES -> GLES11Ext.GL_TEXTURE_EXTERNAL_OES
                        else -> GLES20.GL_TEXTURE_2D
                    },
                    textureMatrix = matrix,
                )
            )?.textureId ?:  -1
        })
    }

    private fun processBeautyI420(videoFrame: VideoFrame): Int{
        val texBufferHelper = textureBufferHelper ?: return -1
        val nv21ByteArray = getNV21Buffer(videoFrame) ?: return -1
        val buffer = videoFrame.buffer
        val width = buffer.width
        val height = buffer.height

        if(currProcessSourceType != ProcessSourceType.I420){
            LogUtils.i(TAG, "processBeautyAuto >> process source type change old=$currProcessSourceType, new=${ProcessSourceType.I420}")
            currProcessSourceType = ProcessSourceType.I420
        }

        return texBufferHelper.invoke(Callable {
            mayCreateBeautyProcess()
            return@Callable beautyProcessor?.process(
                InputInfo(
                    width = width,
                    height = height,
                    cameraOrientation = videoFrame.rotation,
                    isFrontCamera = videoFrame.sourceType == SourceType.kFrontCamera,
                    isMirror = (isFrontCamera && !captureMirror) || (!isFrontCamera && captureMirror),
                    timestamp = videoFrame.timestampNs,
                    bytes = nv21ByteArray,
                    bytesType = STCommonNative.ST_PIX_FMT_NV21
                )
            )?.textureId ?: -1
        })
    }

    private fun processBeautyTexture(videoFrame: VideoFrame): Int{
        if (Build.VERSION.SDK_INT >= 26) {
            // Android 8.0以上使用单纹理输入，内部使用HardwareBuffer转nv21
            return processBeautyTextureAPI26(videoFrame)
        }
        val texBufferHelper = textureBufferHelper ?: return -1
        val buffer = videoFrame.buffer as? TextureBuffer ?: return -1
        val nv21ByteArray = getNV21Buffer(videoFrame) ?: return -1
        val width = buffer.width
        val height = buffer.height

        when(buffer.type){
            TextureBuffer.Type.OES -> {
                if(currProcessSourceType != ProcessSourceType.TEXTURE_OES){
                    LogUtils.i(TAG, "processBeautyAuto >> process source type change old=$currProcessSourceType, new=${ProcessSourceType.TEXTURE_OES}")
                    currProcessSourceType = ProcessSourceType.TEXTURE_OES
                }
            }
            else -> {
                if(currProcessSourceType != ProcessSourceType.TEXTURE_2D){
                    LogUtils.i(TAG, "processBeautyAuto >> process source type change old=$currProcessSourceType, new=${ProcessSourceType.TEXTURE_2D}")
                    currProcessSourceType = ProcessSourceType.TEXTURE_2D
                }
            }
        }

        val matrix =
            RendererCommon.convertMatrixFromAndroidGraphicsMatrix(buffer.transformMatrix)
        return texBufferHelper.invoke(Callable {
            mayCreateBeautyProcess()
            return@Callable beautyProcessor?.process(
                InputInfo(
                    width = width,
                    height = height,
                    cameraOrientation = videoFrame.rotation,
                    isFrontCamera = videoFrame.sourceType == SourceType.kFrontCamera,
                    isMirror = (isFrontCamera && !captureMirror) || (!isFrontCamera && captureMirror),
                    timestamp = videoFrame.timestampNs,
                    bytes = nv21ByteArray,
                    bytesType = STCommonNative.ST_PIX_FMT_NV21,
                    textureId = buffer.textureId,
                    textureType = when (buffer.type) {
                        TextureBuffer.Type.OES -> GLES11Ext.GL_TEXTURE_EXTERNAL_OES
                        else -> GLES20.GL_TEXTURE_2D
                    },
                    textureMatrix = matrix,
                    diffBetweenBytesAndTexture = 1
                )
            )?.textureId ?: -1
        })
    }

    private fun getNV21Buffer(videoFrame: VideoFrame) : ByteArray? {
        val buffer = videoFrame.buffer
        YuvConverter.setEnablePboOpt(true)
        YuvConverter.setEnableConvertPerLog(true)
        val i420Buffer = buffer as? I420Buffer ?: buffer.toI420()
        val width = i420Buffer.width
        val height = i420Buffer.height
        val nv21Size = (width * height * 3.0f / 2.0f + 0.5f).toInt()
        if (nv21ByteBuffer == null || nv21ByteBuffer?.capacity() != nv21Size) {
            nv21ByteBuffer?.clear()
            nv21ByteBuffer = ByteBuffer.allocateDirect(nv21Size)
            return null
        }
        val nv21ByteArray = ByteArray(nv21Size)

        YuvHelper.I420ToNV12(
            i420Buffer.dataY, i420Buffer.strideY,
            i420Buffer.dataV, i420Buffer.strideV,
            i420Buffer.dataU, i420Buffer.strideU,
            nv21ByteBuffer, width, height
        )
        nv21ByteBuffer?.position(0)
        nv21ByteBuffer?.get(nv21ByteArray)
        if (buffer !is I420Buffer) {
            i420Buffer.release()
        }
        return nv21ByteArray
    }

    // IVideoFrameObserver implements

    override fun onCaptureVideoFrame(sourceType: Int, videoFrame: VideoFrame?): Boolean {
        videoFrame ?: return false
        return processBeauty(videoFrame)
    }

    override fun onPreEncodeVideoFrame(sourceType: Int, videoFrame: VideoFrame?) : Boolean {

        return true
    }

    override fun onMediaPlayerVideoFrame(videoFrame: VideoFrame?, mediaPlayerId: Int) = false

    override fun onRenderVideoFrame(
        channelId: String?,
        uid: Int,
        videoFrame: VideoFrame?
    ) = false

    override fun getVideoFrameProcessMode() = IVideoFrameObserver.PROCESS_MODE_READ_WRITE

    override fun getVideoFormatPreference() = IVideoFrameObserver.VIDEO_PIXEL_DEFAULT

    override fun getRotationApplied() = false

    override fun getMirrorApplied() = captureMirror && !enable

    override fun getObservedFramePosition() = IVideoFrameObserver.POSITION_POST_CAPTURER

}