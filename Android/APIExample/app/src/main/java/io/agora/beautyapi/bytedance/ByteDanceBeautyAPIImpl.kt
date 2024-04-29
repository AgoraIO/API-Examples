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

package io.agora.beautyapi.bytedance

import android.graphics.Matrix
import android.view.SurfaceView
import android.view.TextureView
import android.view.View
import com.effectsar.labcv.effectsdk.EffectsSDKEffectConstants
import com.effectsar.labcv.effectsdk.RenderManager
import io.agora.base.TextureBufferHelper
import io.agora.base.VideoFrame
import io.agora.base.VideoFrame.I420Buffer
import io.agora.base.VideoFrame.TextureBuffer
import io.agora.base.internal.video.RendererCommon
import io.agora.base.internal.video.YuvHelper
import io.agora.beautyapi.bytedance.utils.AgoraImageHelper
import io.agora.beautyapi.bytedance.utils.ImageUtil
import io.agora.beautyapi.bytedance.utils.LogUtils
import io.agora.beautyapi.bytedance.utils.StatsHelper
import io.agora.rtc2.Constants
import io.agora.rtc2.gl.EglBaseProvider
import io.agora.rtc2.video.IVideoFrameObserver
import io.agora.rtc2.video.VideoCanvas
import java.nio.ByteBuffer
import java.util.Collections
import java.util.concurrent.Callable
import java.util.concurrent.Executors

class ByteDanceBeautyAPIImpl : ByteDanceBeautyAPI, IVideoFrameObserver {
    private val TAG = "ByteDanceBeautyAPIImpl"
    private val reportId = "scenarioAPI"
    private val reportCategory = "beauty_android_$VERSION"
    private var beautyMode = 0 // 0: 自动根据buffer类型切换，1：固定使用OES纹理，2：固定使用i420


    private var textureBufferHelper: TextureBufferHelper? = null
    private var imageUtils: ImageUtil? = null
    private var agoraImageHelper: AgoraImageHelper? = null
    private var nv21ByteBuffer: ByteBuffer? = null
    private var config: Config? = null
    private var enable: Boolean = false
    private var isReleased: Boolean = false
    private var captureMirror = true
    private var renderMirror = true
    private var statsHelper: StatsHelper? = null
    private var skipFrame = 0
    private val workerThreadExecutor = Executors.newSingleThreadExecutor()
    private var currBeautyProcessType = BeautyProcessType.UNKNOWN
    private var isFrontCamera = true
    private var cameraConfig = CameraConfig()
    private var localVideoRenderMode = Constants.RENDER_MODE_HIDDEN
    private val pendingProcessRunList = Collections.synchronizedList(mutableListOf<()->Unit>())
    private var frameWidth = 0
    private var frameHeight = 0

    private enum class BeautyProcessType{
        UNKNOWN, TEXTURE_OES, TEXTURE_2D, I420
    }

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
            this.config?.eventCallback?.onBeautyStats?.invoke(it)
        }
        LogUtils.i(TAG, "initialize >> config = $config")
        LogUtils.i(TAG, "initialize >> beauty api version=$VERSION, beauty sdk version=${RenderManager.getSDKVersion()}")
        config.rtcEngine.sendCustomReportMessage(reportId, reportCategory, "initialize", "$config", 0)
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
        if (config?.captureMode == CaptureMode.Custom) {
            skipFrame = 2
            LogUtils.i(TAG, "enable >> skipFrame = $skipFrame")
        }
        this.enable = enable
        config?.rtcEngine?.sendCustomReportMessage(reportId, reportCategory, "enable", "$enable", 0)
        return ErrorCode.ERROR_OK.value
    }

    override fun setupLocalVideo(view: View, renderMode: Int): Int {
        val rtcEngine = config?.rtcEngine
        if(rtcEngine == null){
            LogUtils.e(TAG, "setupLocalVideo >> The beauty api has not been initialized!")
            return ErrorCode.ERROR_HAS_NOT_INITIALIZED.value
        }
        LogUtils.i(TAG, "setupLocalVideo >> view=$view, renderMode=$renderMode")
        rtcEngine.sendCustomReportMessage(reportId, reportCategory, "enable", "view=$view, renderMode=$renderMode", 0)
        if (view is TextureView || view is SurfaceView) {
            val canvas = VideoCanvas(view, renderMode, 0)
            canvas.mirrorMode = Constants.VIDEO_MIRROR_MODE_DISABLED
            rtcEngine.setupLocalVideo(canvas)
            return ErrorCode.ERROR_OK.value
        }
        return ErrorCode.ERROR_VIEW_TYPE_ERROR.value
    }

    override fun onFrame(videoFrame: VideoFrame): Int {
        val conf = config
        if (conf == null) {
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

    override fun setBeautyPreset(
        preset: BeautyPreset,
        beautyNodePath: String,
        beauty4ItemNodePath: String,
        reSharpNodePath: String
    ): Int {
        val conf = config
        if(conf == null){
            LogUtils.e(TAG, "setBeautyPreset >> The beauty api has not been initialized!")
            return ErrorCode.ERROR_HAS_NOT_INITIALIZED.value
        }
        if (isReleased) {
            LogUtils.e(TAG, "setBeautyPreset >> The beauty api has been released!")
            return ErrorCode.ERROR_HAS_RELEASED.value
        }
        val initialized = textureBufferHelper != null
        if(!initialized){
            runOnProcessThread {
                setBeautyPreset(preset, beautyNodePath, beauty4ItemNodePath, reSharpNodePath)
            }
            return ErrorCode.ERROR_OK.value
        }

        LogUtils.i(TAG, "setBeautyPreset >> preset = $preset")
        conf.rtcEngine.sendCustomReportMessage(reportId, reportCategory, "enable", "preset=$preset, beautyNodePath=$beautyNodePath, beauty4ItemNodePath=$beauty4ItemNodePath, reSharpNodePath=$reSharpNodePath", 0)

        runOnProcessThread {
            val renderManager =
                config?.renderManager ?: return@runOnProcessThread

            val enable = preset == BeautyPreset.DEFAULT
            renderManager.updateComposerNodes(
                beautyNodePath,
                "smooth",
                if (enable) 0.3f else 0f
            )// 磨皮
            renderManager.updateComposerNodes(
                beautyNodePath,
                "whiten",
                if (enable) 0.5f else 0f
            )// 美白
            renderManager.updateComposerNodes(
                reSharpNodePath,
                "Internal_Deform_Overall",
                if (enable) 0.15f else 0f
            )//瘦脸
            renderManager.updateComposerNodes(
                reSharpNodePath,
                "Internal_Deform_Zoom_Cheekbone",
                if (enable) 0.3f else 0f
            )//瘦颧骨
            renderManager.updateComposerNodes(
                reSharpNodePath,
                "Internal_Deform_Zoom_Jawbone",
                if (enable) 0.46f else 0f
            )//下颌骨
            renderManager.updateComposerNodes(
                reSharpNodePath,
                "Internal_Deform_Eye",
                if (enable) 0.15f else 0f
            )//大眼
            renderManager.updateComposerNodes(
                beauty4ItemNodePath,
                "BEF_BEAUTY_WHITEN_TEETH",
                if (enable) 0.2f else 0f
            )//美牙
            renderManager.updateComposerNodes(
                reSharpNodePath,
                "Internal_Deform_Forehead",
                if (enable) 0.4f else 0f
            )//额头
            renderManager.updateComposerNodes(
                reSharpNodePath,
                "Internal_Deform_Nose",
                if (enable) 0.15f else 0f
            )//瘦鼻
            renderManager.updateComposerNodes(
                reSharpNodePath,
                "Internal_Deform_ZoomMouth",
                if (enable) 0.16f else 0f
            )//嘴形
            renderManager.updateComposerNodes(
                reSharpNodePath,
                "Internal_Deform_Chin",
                if (enable) 0.46f else 0f
            )//下巴
        }
        return ErrorCode.ERROR_OK.value
    }

    override fun setParameters(key: String, value: String) {
        when (key) {
            "beauty_mode" -> beautyMode = value.toInt()
        }
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
                imageUtils?.release()
                agoraImageHelper?.release()
                imageUtils = null
                agoraImageHelper = null
                config?.eventCallback?.onEffectDestroyed?.invoke()
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
            skipFrame = 2
            return false
        }

        val oldIsFrontCamera = isFrontCamera
        isFrontCamera = videoFrame.sourceType == VideoFrame.SourceType.kFrontCamera
        if(oldIsFrontCamera != isFrontCamera){
            LogUtils.w(TAG, "processBeauty >> oldIsFrontCamera=$oldIsFrontCamera, isFrontCamera=$isFrontCamera")
            return false
        }

        val oldFrameWidth = frameWidth
        val oldFrameHeight = frameHeight
        frameWidth = videoFrame.rotatedWidth
        frameHeight = videoFrame.rotatedHeight
        if (oldFrameWidth > 0 || oldFrameHeight > 0) {
            if(oldFrameWidth != frameWidth || oldFrameHeight != frameHeight){
                skipFrame = 2
                return false
            }
        }

        if(!enable){
            return true
        }

        if (textureBufferHelper == null) {
            textureBufferHelper = TextureBufferHelper.create(
                "ByteDanceRender",
                EglBaseProvider.instance().rootEglBase.eglBaseContext
            )
            textureBufferHelper?.invoke {
                imageUtils = ImageUtil()
                agoraImageHelper = AgoraImageHelper()
                config?.eventCallback?.onEffectInitialized?.invoke()
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

        val processTexId = when (beautyMode) {
            1 -> processBeautySingleTexture(videoFrame)
            2 -> processBeautySingleBuffer(videoFrame)
            else -> processBeautyAuto(videoFrame)
        }
        if (config?.statsEnable == true) {
            val costTime = System.currentTimeMillis() - startTime
            statsHelper?.once(costTime)
        }

        if (processTexId < 0) {
            LogUtils.w(TAG, "processBeauty >> processTexId < 0")
            return false
        }

        if (skipFrame > 0) {
            skipFrame--
            return false
        }

        val processBuffer: TextureBuffer = textureBufferHelper?.wrapTextureBuffer(
            videoFrame.rotatedWidth,
            videoFrame.rotatedHeight,
            TextureBuffer.Type.RGB,
            processTexId,
            Matrix().apply {
                preTranslate(0.5f, 0.5f)
                preScale(1.0f, -1.0f)
                preTranslate(-0.5f, -0.5f)
            }
        ) ?: return false
        videoFrame.replaceBuffer(processBuffer, 0, videoFrame.timestampNs)
        return true
    }

    private fun processBeautyAuto(videoFrame: VideoFrame): Int {
        val buffer = videoFrame.buffer
        return if (buffer is TextureBuffer) {
            processBeautySingleTexture(videoFrame)
        } else {
            processBeautySingleBuffer(videoFrame)
        }
    }

    private fun processBeautySingleTexture(videoFrame: VideoFrame): Int {
        val texBufferHelper = textureBufferHelper ?: return -1
        val imageUtils = imageUtils ?: return -1
        val agoraImageHelper = agoraImageHelper ?: return -1
        val buffer = videoFrame.buffer as? TextureBuffer ?: return -1
        val isFront = videoFrame.sourceType == VideoFrame.SourceType.kFrontCamera

        when(buffer.type){
            TextureBuffer.Type.OES -> {
                if(currBeautyProcessType != BeautyProcessType.TEXTURE_OES){
                    LogUtils.i(TAG, "processBeauty >> process source type change old=$currBeautyProcessType, new=${BeautyProcessType.TEXTURE_OES}")
                    currBeautyProcessType = BeautyProcessType.TEXTURE_OES
                    return -1
                }
            }
            else -> {
                if(currBeautyProcessType != BeautyProcessType.TEXTURE_2D){
                    LogUtils.i(TAG, "processBeauty >> process source type change old=$currBeautyProcessType, new=${BeautyProcessType.TEXTURE_2D}")
                    currBeautyProcessType = BeautyProcessType.TEXTURE_2D
                    return -1
                }
            }
        }

        return texBufferHelper.invoke(Callable {
            val renderManager = config?.renderManager ?: return@Callable -1
            var mirror = isFront
            if((isFrontCamera && !captureMirror) || (!isFrontCamera && captureMirror)){
                mirror = !mirror
            }

            val width = videoFrame.rotatedWidth
            val height = videoFrame.rotatedHeight

            val renderMatrix = Matrix()
            renderMatrix.preTranslate(0.5f, 0.5f)
            renderMatrix.preRotate(videoFrame.rotation.toFloat())
            renderMatrix.preScale(if (mirror) -1.0f else 1.0f, -1.0f)
            renderMatrix.preTranslate(-0.5f, -0.5f)
            val finalMatrix = Matrix(buffer.transformMatrix)
            finalMatrix.preConcat(renderMatrix)

            val transform =
                RendererCommon.convertMatrixFromAndroidGraphicsMatrix(finalMatrix)


            val dstTexture = imageUtils.prepareTexture(width, height)
            val srcTexture = agoraImageHelper.transformTexture(
                buffer.textureId,
                buffer.type,
                width,
                height,
                transform
            )
            renderManager.setCameraPostion(isFront)
            val success = renderManager.processTexture(
                srcTexture,
                dstTexture,
                width,
                height,
                EffectsSDKEffectConstants.Rotation.CLOCKWISE_ROTATE_0,
                videoFrame.timestampNs
            )
            if (!success) {
                return@Callable -1
            }
            return@Callable dstTexture
        })
    }

    private fun processBeautySingleBuffer(videoFrame: VideoFrame): Int {
        val texBufferHelper = textureBufferHelper ?: return -1
        val imageUtils = imageUtils ?: return -1
        val nv21Buffer = getNV21Buffer(videoFrame) ?: return -1
        val isFront = videoFrame.sourceType == VideoFrame.SourceType.kFrontCamera

        if (currBeautyProcessType != BeautyProcessType.I420) {
            LogUtils.i(TAG, "processBeauty >> process source type change old=$currBeautyProcessType, new=${BeautyProcessType.I420}")
            currBeautyProcessType = BeautyProcessType.I420
            return -1
        }

        return texBufferHelper.invoke(Callable {
            val renderManager = config?.renderManager ?: return@Callable -1

            val width = videoFrame.rotatedWidth
            val height = videoFrame.rotatedHeight

            val ySize = width * height
            val yBuffer = ByteBuffer.allocateDirect(ySize)
            yBuffer.put(nv21Buffer, 0, ySize)
            yBuffer.position(0)
            val vuBuffer = ByteBuffer.allocateDirect(ySize / 2)
            vuBuffer.put(nv21Buffer, ySize, ySize / 2)
            vuBuffer.position(0)

            var mirror = isFront
            if((isFrontCamera && !captureMirror) || (!isFrontCamera && captureMirror)){
                mirror = !mirror
            }
            val isScreenLandscape = videoFrame.rotation % 180 == 0
            val dstTexture = imageUtils.prepareTexture(width, height)
            val srcTexture = imageUtils.transferYUVToTexture(
                yBuffer,
                vuBuffer,
                if (isScreenLandscape) width else height,
                if (isScreenLandscape) height else width,
                ImageUtil.Transition().apply {
                    rotate(videoFrame.rotation.toFloat())
                    flip(
                        if (isScreenLandscape) mirror else false,
                        if (isScreenLandscape) false else mirror
                    )
                }
            )
            renderManager.setCameraPostion(isFront)
            val success = renderManager.processTexture(
                srcTexture,
                dstTexture,
                width,
                height,
                EffectsSDKEffectConstants.Rotation.CLOCKWISE_ROTATE_0,
                videoFrame.timestampNs
            )
            return@Callable if (success) {
                dstTexture
            } else {
                -1
            }
        })
    }

    private fun getNV21Buffer(videoFrame: VideoFrame): ByteArray? {
        val buffer = videoFrame.buffer
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

    override fun onPreEncodeVideoFrame(sourceType: Int, videoFrame: VideoFrame?) = false

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