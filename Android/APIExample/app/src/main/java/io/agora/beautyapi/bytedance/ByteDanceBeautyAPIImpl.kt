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
import com.bytedance.labcv.effectsdk.BytedEffectConstants
import io.agora.base.TextureBufferHelper
import io.agora.base.VideoFrame
import io.agora.base.VideoFrame.I420Buffer
import io.agora.base.VideoFrame.TextureBuffer
import io.agora.base.internal.video.RendererCommon
import io.agora.base.internal.video.YuvHelper
import io.agora.beautyapi.bytedance.utils.AgoraImageHelper
import io.agora.beautyapi.bytedance.utils.ImageUtil
import io.agora.beautyapi.bytedance.utils.StatsHelper
import io.agora.rtc2.Constants
import io.agora.rtc2.gl.EglBaseProvider
import io.agora.rtc2.video.IVideoFrameObserver
import io.agora.rtc2.video.VideoCanvas
import java.nio.ByteBuffer
import java.util.concurrent.Callable
import java.util.concurrent.Executors

class ByteDanceBeautyAPIImpl : ByteDanceBeautyAPI, IVideoFrameObserver {
    private var beautyMode = 0 // 0: 自动根据buffer类型切换，1：固定使用OES纹理，2：固定使用i420
    private val BEAUTY_NODE = "beauty_Android_lite"
    private val BEAUTY_4ITEMS_NODE = "beauty_4Items"
    private val RESHARP_LITE_NODE = "reshape_lite"

    private var textureBufferHelper: TextureBufferHelper? = null
    private var imageUtils: ImageUtil? = null
    private var agoraImageHelper: AgoraImageHelper? = null
    private var nv21ByteBuffer: ByteBuffer? = null
    private var config: Config? = null
    private var enable: Boolean = false
    private var isReleased: Boolean = false
    private var shouldMirror = true
    private var statsHelper: StatsHelper? = null
    private var skipFrame = 0
    private val workerThreadExecutor = Executors.newSingleThreadExecutor()
    private var currBeautyProcessType = BeautyProcessType.UNKNOWN

    private enum class BeautyProcessType{
        UNKNOWN, TEXTURE, I420
    }

    override fun initialize(config: Config): Int {
        if (this.config != null) {
            return ErrorCode.ERROR_HAS_INITIALIZED.value
        }
        this.config = config
        if (config.captureMode == CaptureMode.Agora) {
            config.rtcEngine.registerVideoFrameObserver(this)
        }
        statsHelper = StatsHelper(config.statsDuration) {
            this.config?.eventCallback?.onBeautyStats?.invoke(it)
        }
        return ErrorCode.ERROR_OK.value
    }

    override fun enable(enable: Boolean): Int {
        if (config == null) {
            return ErrorCode.ERROR_HAS_NOT_INITIALIZED.value
        }
        if (isReleased) {
            return ErrorCode.ERROR_HAS_RELEASED.value
        }
        if (config?.captureMode == CaptureMode.Custom) {
            skipFrame = 2
        }
        this.enable = enable
        return ErrorCode.ERROR_OK.value
    }

    override fun setupLocalVideo(view: View, renderMode: Int): Int {
        val rtcEngine = config?.rtcEngine ?: return ErrorCode.ERROR_HAS_NOT_INITIALIZED.value
        if (view is TextureView || view is SurfaceView) {
            val canvas = VideoCanvas(view, renderMode, 0)
            canvas.mirrorMode = Constants.VIDEO_MIRROR_MODE_DISABLED
            rtcEngine.setupLocalVideo(canvas)
            return ErrorCode.ERROR_OK.value
        }
        return ErrorCode.ERROR_VIEW_TYPE_ERROR.value
    }

    override fun onFrame(videoFrame: VideoFrame): Int {
        val conf = config ?: return ErrorCode.ERROR_HAS_NOT_INITIALIZED.value
        if (isReleased) {
            return ErrorCode.ERROR_HAS_RELEASED.value
        }
        if (conf.captureMode != CaptureMode.Custom) {
            return ErrorCode.ERROR_PROCESS_NOT_CUSTOM.value
        }
        if (!enable) {
            return ErrorCode.ERROR_PROCESS_DISABLE.value
        }
        if (processBeauty(videoFrame)) {
            return ErrorCode.ERROR_OK.value
        }
        return ErrorCode.ERROR_FRAME_SKIPPED.value
    }

    override fun setBeautyPreset(preset: BeautyPreset): Int {
        if (isReleased) {
            return ErrorCode.ERROR_HAS_RELEASED.value
        }
        val effectManager =
            config?.effectManager ?: return ErrorCode.ERROR_HAS_NOT_INITIALIZED.value
        val enable = preset == BeautyPreset.DEFAULT

        effectManager.appendComposeNodes(
            arrayOf(
                BEAUTY_NODE,
                RESHARP_LITE_NODE,
                BEAUTY_4ITEMS_NODE
            )
        )
        effectManager.updateComposerNodeIntensity(
            BEAUTY_NODE,
            "smooth",
            if (enable) 0.3f else 0f
        )// 磨皮
        effectManager.updateComposerNodeIntensity(
            BEAUTY_NODE,
            "whiten",
            if (enable) 0.5f else 0f
        )// 美白
        effectManager.updateComposerNodeIntensity(
            RESHARP_LITE_NODE,
            "Internal_Deform_Overall",
            if (enable) 0.15f else 0f
        )//瘦脸
        effectManager.updateComposerNodeIntensity(
            RESHARP_LITE_NODE,
            "Internal_Deform_Zoom_Cheekbone",
            if (enable) 0.3f else 0f
        )//瘦颧骨
        effectManager.updateComposerNodeIntensity(
            RESHARP_LITE_NODE,
            "Internal_Deform_Zoom_Jawbone",
            if (enable) 0.46f else 0f
        )//下颌骨
        effectManager.updateComposerNodeIntensity(
            RESHARP_LITE_NODE,
            "Internal_Deform_Eye",
            if (enable) 0.15f else 0f
        )//大眼
        effectManager.updateComposerNodeIntensity(
            BEAUTY_4ITEMS_NODE,
            "BEF_BEAUTY_WHITEN_TEETH",
            if (enable) 0.2f else 0f
        )//美牙
        effectManager.updateComposerNodeIntensity(
            RESHARP_LITE_NODE,
            "Internal_Deform_Forehead",
            if (enable) 0.4f else 0f
        )//额头
        effectManager.updateComposerNodeIntensity(
            RESHARP_LITE_NODE,
            "Internal_Deform_Nose",
            if (enable) 0.15f else 0f
        )//瘦鼻
        effectManager.updateComposerNodeIntensity(
            RESHARP_LITE_NODE,
            "Internal_Deform_ZoomMouth",
            if (enable) 0.16f else 0f
        )//嘴形
        effectManager.updateComposerNodeIntensity(
            RESHARP_LITE_NODE,
            "Internal_Deform_Chin",
            if (enable) 0.46f else 0f
        )//下巴

        return ErrorCode.ERROR_OK.value
    }

    override fun setParameters(key: String, value: String) {
        when (key) {
            "beauty_mode" -> beautyMode = value.toInt()
        }
    }

    override fun release(): Int {
        if (isReleased) {
            return ErrorCode.ERROR_HAS_RELEASED.value
        }
        val effectManager =
            config?.effectManager ?: return ErrorCode.ERROR_HAS_NOT_INITIALIZED.value

        isReleased = true
        workerThreadExecutor.shutdown()
        textureBufferHelper?.let {
            textureBufferHelper = null
            it.invoke {
                imageUtils?.release()
                agoraImageHelper?.release()
                imageUtils = null
                agoraImageHelper = null
                effectManager.destroy()
                config?.eventCallback?.onEffectDestroyed?.invoke()
                null
            }
            it.dispose()
        }
        statsHelper?.reset()
        statsHelper = null
        return ErrorCode.ERROR_OK.value
    }

    private fun processBeauty(videoFrame: VideoFrame): Boolean {
        if (!enable || isReleased) {
            val isFront = videoFrame.sourceType == VideoFrame.SourceType.kFrontCamera
            if (shouldMirror != isFront) {
                shouldMirror = isFront
                return false
            }
            return true
        }
        if (shouldMirror) {
            shouldMirror = false
            return false
        }
        if (skipFrame > 0) {
            skipFrame--;
            return false
        }

        if (textureBufferHelper == null) {
            textureBufferHelper = TextureBufferHelper.create(
                "ByteDanceRender",
                EglBaseProvider.instance().rootEglBase.eglBaseContext
            )?.apply {
                invoke {
                    val effectManager = config?.effectManager ?: return@invoke
                    effectManager.init();
                    imageUtils =
                        ImageUtil()
                    agoraImageHelper = AgoraImageHelper()
                    config?.eventCallback?.onEffectInitialized?.invoke()
                }
            }
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
        if (currBeautyProcessType != BeautyProcessType.TEXTURE) {
            currBeautyProcessType = BeautyProcessType.TEXTURE
            return -1
        }

        return texBufferHelper.invoke(Callable {
            val effectManager = config?.effectManager ?: return@Callable -1


            // 根据Matrix反算纹理的真实宽高
            val renderMatrix = Matrix()
            renderMatrix.preTranslate(0.5f, 0.5f)
            renderMatrix.preRotate(videoFrame.rotation.toFloat())
            renderMatrix.preScale(if (isFront) -1.0f else 1.0f, -1.0f)
            renderMatrix.preTranslate(-0.5f, -0.5f)
            val finalMatrix = Matrix(buffer.transformMatrix)
            finalMatrix.preConcat(renderMatrix)

            val transform =
                RendererCommon.convertMatrixFromAndroidGraphicsMatrix(finalMatrix)

            val width = buffer.height
            val height = buffer.width
            val dstTexture = imageUtils.prepareTexture(width, height)
            val srcTexture = agoraImageHelper.transformTexture(
                buffer.textureId,
                buffer.type,
                width,
                height,
                transform
            )
            effectManager.setCameraPosition(isFront)
            val success = effectManager.process(
                srcTexture,
                dstTexture,
                width,
                height,
                BytedEffectConstants.Rotation.CLOCKWISE_ROTATE_90,
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
        val buffer = videoFrame.buffer
        val isFront = videoFrame.sourceType == VideoFrame.SourceType.kFrontCamera
        if (currBeautyProcessType != BeautyProcessType.I420) {
            currBeautyProcessType = BeautyProcessType.I420
            return -1
        }

        return texBufferHelper.invoke(Callable {
            val effectManager = config?.effectManager ?: return@Callable -1

            val width = buffer.height
            val height = buffer.width

            val ySize = width * height
            val yBuffer = ByteBuffer.allocateDirect(ySize)
            yBuffer.put(nv21Buffer, 0, ySize)
            yBuffer.position(0);
            val vuBuffer = ByteBuffer.allocateDirect(ySize / 2)
            vuBuffer.put(nv21Buffer, ySize, ySize / 2)
            vuBuffer.position(0);

            val dstTexture = imageUtils.prepareTexture(width, height)
            val srcTexture = imageUtils.transferYUVToTexture(
                yBuffer,
                vuBuffer,
                height,
                width,
                ImageUtil.Transition().apply {
                    rotate(videoFrame.rotation.toFloat())
                    flip(false, isFront)
                }
            )
            effectManager.setCameraPosition(isFront)
            val success = effectManager.process(
                srcTexture,
                dstTexture,
                width,
                height,
                BytedEffectConstants.Rotation.CLOCKWISE_ROTATE_0,
                videoFrame.timestampNs
            )
            return@Callable if (success) {
                dstTexture
            } else {
                -1
            }
        })
    }

    private fun getNV21Buffer(videoFrame: VideoFrame, rotate: Boolean = false): ByteArray? {
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

    private fun getI420Buffer(videoFrame: VideoFrame, rotate: Boolean = false): ByteArray? {
        val buffer = videoFrame.buffer
        val i420Buffer = buffer as? I420Buffer ?: buffer.toI420() ?: return null
        val width = i420Buffer.width
        val height = i420Buffer.height
        val size = (width * height * 3.0f / 2.0f + 0.5f).toInt()

        val byteArray = ByteArray(size)
        i420Buffer.dataY.get(byteArray, 0, width * height)
        i420Buffer.dataV.get(byteArray, width * height, width * height / 4)
        i420Buffer.dataU.get(byteArray, width * height + width * height / 4, width * height / 4)
        if (buffer !is I420Buffer) {
            i420Buffer.release()
        }
        return byteArray
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

    override fun getMirrorApplied() = shouldMirror

    override fun getObservedFramePosition() = IVideoFrameObserver.POSITION_POST_CAPTURER

}