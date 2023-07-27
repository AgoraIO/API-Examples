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

import android.graphics.Matrix
import android.opengl.GLES11Ext
import android.opengl.GLES20
import android.view.SurfaceView
import android.view.TextureView
import android.view.View
import com.faceunity.core.entity.FUBundleData
import com.faceunity.core.entity.FURenderInputData
import com.faceunity.core.enumeration.CameraFacingEnum
import com.faceunity.core.enumeration.FUInputBufferEnum
import com.faceunity.core.enumeration.FUInputTextureEnum
import com.faceunity.core.enumeration.FUTransformMatrixEnum
import com.faceunity.core.faceunity.FUAIKit
import com.faceunity.core.model.facebeauty.FaceBeauty
import com.faceunity.core.model.facebeauty.FaceBeautyFilterEnum
import io.agora.base.TextureBufferHelper
import io.agora.base.VideoFrame
import io.agora.base.VideoFrame.I420Buffer
import io.agora.base.VideoFrame.SourceType
import io.agora.base.VideoFrame.TextureBuffer
import io.agora.base.internal.video.YuvHelper
import io.agora.beautyapi.faceunity.utils.FUConfig
import io.agora.beautyapi.faceunity.utils.StatsHelper
import io.agora.beautyapi.faceunity.utils.egl.TextureProcessHelper
import io.agora.rtc2.Constants
import io.agora.rtc2.gl.EglBaseProvider
import io.agora.rtc2.video.IVideoFrameObserver
import io.agora.rtc2.video.VideoCanvas
import java.io.File
import java.nio.ByteBuffer
import java.util.concurrent.Callable

class FaceUnityBeautyAPIImpl : FaceUnityBeautyAPI, IVideoFrameObserver {
    private var beautyMode = 0 // 0: 自动根据buffer类型切换，1：固定使用OES纹理，2：固定使用i420，3: 单纹理异步模式(自创)

    private var textureBufferHelper: TextureBufferHelper? = null
    private var byteBuffer: ByteBuffer? = null
    private var byteArray: ByteArray? = null
    private var config: Config? = null
    private var enable: Boolean = false
    private var enableChange: Boolean = false
    private var isReleased: Boolean = false
    private var shouldMirror = false
    private val identityMatrix =  Matrix()
    private var mCameraIsFront = true
    private var mTextureProcessHelper: TextureProcessHelper? = null
    private var statsHelper: StatsHelper? = null
    private var skipFrame = 0

    override fun initialize(config: Config): Int {
        if (this.config != null) {
            return ErrorCode.ERROR_HAS_INITIALIZED.value
        }
        this.config = config
        if (config.captureMode == CaptureMode.Agora) {
            config.rtcEngine.registerVideoFrameObserver(this)
        }
        statsHelper = StatsHelper(config.statsDuration){
            this.config?.eventCallback?.onBeautyStats(it)
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
        if(config?.captureMode == CaptureMode.Custom){
            skipFrame = 2
        }
        if(this.enable != enable){
            this.enable = enable
            enableChange = true
        }
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

    override fun setParameters(key: String, value: String) {
        when(key){
            "beauty_mode" -> beautyMode = value.toInt()
        }
    }

    override fun setBeautyPreset(preset: BeautyPreset): Int {
        if (isReleased) {
            return ErrorCode.ERROR_HAS_RELEASED.value
        }
        val fuRenderer = config?.fuRenderKit ?: return ErrorCode.ERROR_HAS_NOT_INITIALIZED.value
        // config face beauty
        FUAIKit.getInstance().faceProcessorSetFaceLandmarkQuality(FUConfig.DEVICE_LEVEL)
        val recommendFaceBeauty =
            FaceBeauty(FUBundleData("graphics" + File.separator + "face_beautification.bundle"))
        if (preset == BeautyPreset.DEFAULT) {
            recommendFaceBeauty.filterName = FaceBeautyFilterEnum.FENNEN_1
            recommendFaceBeauty.filterIntensity = 0.7
            // 美牙
            recommendFaceBeauty.toothIntensity = 0.3
            // 亮眼
            recommendFaceBeauty.eyeBrightIntensity = 0.3
            // 大眼
            recommendFaceBeauty.eyeEnlargingIntensity = 0.5
            // 红润
            recommendFaceBeauty.redIntensity = 0.5 * 2
            // 美白
            recommendFaceBeauty.colorIntensity = 0.75 * 2
            // 磨皮
            recommendFaceBeauty.blurIntensity = 0.75 * 6
            // 嘴型
            recommendFaceBeauty.mouthIntensity = 0.3
            // 瘦鼻
            recommendFaceBeauty.noseIntensity = 0.1
            // 额头
            recommendFaceBeauty.forHeadIntensity = 0.3
            // 下巴
            recommendFaceBeauty.chinIntensity = 0.0
            // 瘦脸
            recommendFaceBeauty.cheekThinningIntensity = 0.3
            // 窄脸
            recommendFaceBeauty.cheekNarrowIntensity = 0.0
            // 小脸
            recommendFaceBeauty.cheekSmallIntensity = 0.0
            // v脸
            recommendFaceBeauty.cheekVIntensity = 0.0
        }
        fuRenderer.faceBeauty = recommendFaceBeauty
        return ErrorCode.ERROR_OK.value
    }

    override fun release(): Int {
        if (isReleased) {
            return ErrorCode.ERROR_HAS_RELEASED.value
        }
        val fuRenderer = config?.fuRenderKit ?: return ErrorCode.ERROR_HAS_NOT_INITIALIZED.value

        isReleased = true
        textureBufferHelper?.let {
            textureBufferHelper = null
            it.invoke {
                fuRenderer.releaseEGLContext()
                mTextureProcessHelper?.release()
                mTextureProcessHelper = null
                null
            }
            it.handler.looper.quit()
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
        if(skipFrame > 0){
            skipFrame --;
            return false
        }


        if (textureBufferHelper == null) {
            textureBufferHelper = TextureBufferHelper.create(
                "FURender",
                EglBaseProvider.instance().rootEglBase.eglBaseContext
            )
        }
        val startTime = System.currentTimeMillis()
        val processTexId = when (beautyMode) {
            1 -> processBeautySingleTexture(videoFrame)
            2 -> processBeautySingleBuffer(videoFrame)
            3 -> processBeautySingleTextureAsync(videoFrame)
            else -> processBeautyAuto(videoFrame)
        }

        if (enableChange) {
            enableChange = false
        }
        if(config?.statsEnable == true){
            val costTime = System.currentTimeMillis() - startTime
            statsHelper?.once(costTime)
        }

        val isFront = videoFrame.sourceType == VideoFrame.SourceType.kFrontCamera
        if(mCameraIsFront != isFront){
            mCameraIsFront = isFront;
            return false
        }

        if (processTexId <= 0) {
            return false
        }

        val processBuffer: TextureBuffer = textureBufferHelper?.wrapTextureBuffer(
            videoFrame.rotatedWidth,
            videoFrame.rotatedHeight,
            TextureBuffer.Type.RGB,
            processTexId,
            identityMatrix
        ) ?: return false
        videoFrame.replaceBuffer(processBuffer, 0, videoFrame.timestampNs)
        return true
    }

    private fun processBeautyAuto(videoFrame: VideoFrame): Int {
        val buffer = videoFrame.buffer
        return if (buffer is TextureBuffer) {
            processBeautySingleTextureAsync(videoFrame)
        } else {
            processBeautySingleBuffer(videoFrame)
        }
    }

    private fun processBeautySingleTextureAsync(videoFrame: VideoFrame): Int {
        val texBufferHelper = textureBufferHelper ?: return -1
        val textureBuffer = videoFrame.buffer as? TextureBuffer ?: return -1
        if(enableChange){
            enableChange = false
            texBufferHelper.invoke {
                mTextureProcessHelper?.reset()
            }
            return -1
        }
        return texBufferHelper.invoke {
            if(isReleased){
                return@invoke -1
            }
            var textureProcessHelper = mTextureProcessHelper
            if(textureProcessHelper == null){
                textureProcessHelper = TextureProcessHelper().apply {
                    mTextureProcessHelper = this
                    setFilter { frame ->
                        val fuRenderKit = config?.fuRenderKit ?: return@setFilter -1

                        val input = FURenderInputData(frame.width, frame.height)
                        input.texture = FURenderInputData.FUTexture(
                            FUInputTextureEnum.FU_ADM_FLAG_COMMON_TEXTURE,
                            frame.textureId
                        )
                        val isFront = frame.isFrontCamera
                        input.renderConfig.let {
                            if (isFront) {
                                it.cameraFacing = CameraFacingEnum.CAMERA_FRONT
                                it.inputBufferMatrix = FUTransformMatrixEnum.CCROT0
                                it.inputTextureMatrix = FUTransformMatrixEnum.CCROT0
                                it.outputMatrix = FUTransformMatrixEnum.CCROT0
                                it.deviceOrientation = 270
                            } else {
                                it.cameraFacing = CameraFacingEnum.CAMERA_BACK
                                it.inputBufferMatrix = FUTransformMatrixEnum.CCROT0
                                it.inputTextureMatrix = FUTransformMatrixEnum.CCROT0
                                it.outputMatrix = FUTransformMatrixEnum.CCROT0
                                it.deviceOrientation = 270
                            }
                        }
                        if (isReleased) {
                            return@setFilter -1
                        }
                        return@setFilter fuRenderKit.renderWithInput(input).texture?.texId ?: -1
                    }
                }
            }

            return@invoke textureProcessHelper.process(
                textureBuffer.textureId,
                when (textureBuffer.type) {
                    TextureBuffer.Type.OES -> GLES11Ext.GL_TEXTURE_EXTERNAL_OES
                    else -> GLES20.GL_TEXTURE_2D
                },
                textureBuffer.width,
                textureBuffer.height,
                videoFrame.rotation,
                textureBuffer.transformMatrixArray,
                videoFrame.sourceType == SourceType.kFrontCamera
            )
        }
    }

    private fun processBeautySingleBuffer(videoFrame: VideoFrame): Int {
        val texBufferHelper = textureBufferHelper ?: return -1
        val bufferArray = getNV21Buffer(videoFrame) ?: return -1
        val buffer = videoFrame.buffer
        val width = buffer.width
        val height = buffer.height
        val isFront = videoFrame.sourceType == VideoFrame.SourceType.kFrontCamera


        return texBufferHelper.invoke(Callable {
            if(isReleased){
                return@Callable -1
            }
            val fuRenderKit = config?.fuRenderKit ?: return@Callable -1
            val input = FURenderInputData(width, height)
            input.imageBuffer = FURenderInputData.FUImageBuffer(
                FUInputBufferEnum.FU_FORMAT_NV21_BUFFER,
                bufferArray
            )
            input.renderConfig.let {
                if (isFront) {
                    it.cameraFacing = CameraFacingEnum.CAMERA_FRONT
                    it.inputBufferMatrix = FUTransformMatrixEnum.CCROT90_FLIPHORIZONTAL
                    it.inputTextureMatrix = FUTransformMatrixEnum.CCROT90_FLIPHORIZONTAL
                    it.outputMatrix = FUTransformMatrixEnum.CCROT0
                } else {
                    it.cameraFacing = CameraFacingEnum.CAMERA_BACK
                    it.inputBufferMatrix = FUTransformMatrixEnum.CCROT270
                    it.inputTextureMatrix = FUTransformMatrixEnum.CCROT270
                    it.outputMatrix = FUTransformMatrixEnum.CCROT0
                }
            }
            var outTexId = -1
            val processHelper = mTextureProcessHelper
            if(processHelper != null){
                if(processHelper.size() > 0){
                    processHelper.reset()
                    return@Callable -1
                }
                processHelper.executeSync {
                    outTexId = fuRenderKit.renderWithInput(input).texture?.texId ?: -1
                }
            }else {
                outTexId = fuRenderKit.renderWithInput(input).texture?.texId ?: -1
            }
            return@Callable outTexId
        })
    }

    private fun processBeautyDoubleInput(videoFrame: VideoFrame): Int {
        val texBufferHelper = textureBufferHelper ?: return -1
        val buffer = videoFrame.buffer as? TextureBuffer ?: return -1
        val bufferArray = getNV21Buffer(videoFrame) ?: return -1

        val width = buffer.width
        val height = buffer.height
        val isFront = videoFrame.sourceType == VideoFrame.SourceType.kFrontCamera

        return texBufferHelper.invoke(Callable {
            if(isReleased){
                return@Callable -1
            }
            val fuRenderKit = config?.fuRenderKit ?: return@Callable -1
            val input = FURenderInputData(width, height)
            input.texture = FURenderInputData.FUTexture(
                when (buffer.type) {
                    TextureBuffer.Type.OES -> FUInputTextureEnum.FU_ADM_FLAG_EXTERNAL_OES_TEXTURE
                    else -> FUInputTextureEnum.FU_ADM_FLAG_COMMON_TEXTURE
                },
                buffer.textureId
            )
            input.imageBuffer = FURenderInputData.FUImageBuffer(
                FUInputBufferEnum.FU_FORMAT_NV21_BUFFER,
                bufferArray
            )
            input.renderConfig.let {
                if (isFront) {
                    it.inputBufferMatrix = FUTransformMatrixEnum.CCROT90
                    it.inputTextureMatrix = FUTransformMatrixEnum.CCROT90
                    it.outputMatrix = FUTransformMatrixEnum.CCROT0_FLIPHORIZONTAL
                } else {
                    it.inputBufferMatrix = FUTransformMatrixEnum.CCROT90_FLIPVERTICAL
                    it.inputTextureMatrix = FUTransformMatrixEnum.CCROT90_FLIPVERTICAL
                    it.outputMatrix = FUTransformMatrixEnum.CCROT0_FLIPHORIZONTAL
                }
            }
            return@Callable fuRenderKit.renderWithInput(input).texture?.texId ?: -1
        })
    }

    private fun processBeautySingleTexture(videoFrame: VideoFrame): Int {
        val texBufferHelper = textureBufferHelper ?: return -1
        val buffer = videoFrame.buffer as? TextureBuffer ?: return -1
        val width = buffer.width
        val height = buffer.height
        val isFront = videoFrame.sourceType == VideoFrame.SourceType.kFrontCamera

        return texBufferHelper.invoke(Callable {
            if(isReleased){
                return@Callable -1
            }
            val fuRenderKit = config?.fuRenderKit ?: return@Callable -1
            val input = FURenderInputData(width, height)
            input.texture = FURenderInputData.FUTexture(
                when (buffer.type) {
                    TextureBuffer.Type.OES -> FUInputTextureEnum.FU_ADM_FLAG_EXTERNAL_OES_TEXTURE
                    else -> FUInputTextureEnum.FU_ADM_FLAG_COMMON_TEXTURE
                },
                buffer.textureId
            )
            input.renderConfig.let {
                if (isFront) {
                    it.inputBufferMatrix = FUTransformMatrixEnum.CCROT90
                    it.inputTextureMatrix = FUTransformMatrixEnum.CCROT90
                    it.outputMatrix = FUTransformMatrixEnum.CCROT0_FLIPHORIZONTAL
                } else {
                    it.inputBufferMatrix = FUTransformMatrixEnum.CCROT90_FLIPVERTICAL
                    it.inputTextureMatrix = FUTransformMatrixEnum.CCROT90_FLIPVERTICAL
                    it.outputMatrix = FUTransformMatrixEnum.CCROT0_FLIPHORIZONTAL
                }
            }
            return@Callable fuRenderKit.renderWithInput(input).texture?.texId ?: -1
        })
    }

    private fun getNV21Buffer(videoFrame: VideoFrame): ByteArray? {
        val buffer = videoFrame.buffer
        val width = buffer.width
        val height = buffer.height
        val size = (width * height * 3.0f / 2.0f + 0.5f).toInt()
        if (byteBuffer == null || byteBuffer?.capacity() != size || byteArray == null || byteArray?.size != size) {
            byteBuffer?.clear()
            byteBuffer = ByteBuffer.allocateDirect(size)
            byteArray = ByteArray(size)
            return null
        }
        val outArray = byteArray ?: return null
        val outBuffer = byteBuffer ?: return null
        val i420Buffer = buffer as? I420Buffer ?: buffer.toI420()
        YuvHelper.I420ToNV12(
            i420Buffer.dataY, i420Buffer.strideY,
            i420Buffer.dataV, i420Buffer.strideV,
            i420Buffer.dataU, i420Buffer.strideU,
            outBuffer, width, height
        )
        outBuffer.position(0)
        outBuffer.get(outArray)
        if(buffer !is I420Buffer){
            i420Buffer.release()
        }
        return outArray
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