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
import com.faceunity.core.entity.FURenderInputData
import com.faceunity.core.enumeration.CameraFacingEnum
import com.faceunity.core.enumeration.FUInputBufferEnum
import com.faceunity.core.enumeration.FUInputTextureEnum
import com.faceunity.core.enumeration.FUTransformMatrixEnum
import com.faceunity.core.faceunity.FURenderKit
import io.agora.base.TextureBufferHelper
import io.agora.base.VideoFrame
import io.agora.base.VideoFrame.I420Buffer
import io.agora.base.VideoFrame.SourceType
import io.agora.base.VideoFrame.TextureBuffer
import io.agora.base.internal.video.EglBase
import io.agora.base.internal.video.YuvHelper
import io.agora.beautyapi.faceunity.utils.APIReporter
import io.agora.beautyapi.faceunity.utils.APIType
import io.agora.beautyapi.faceunity.utils.LogUtils
import io.agora.beautyapi.faceunity.utils.StatsHelper
import io.agora.beautyapi.faceunity.utils.egl.GLFrameBuffer
import io.agora.beautyapi.faceunity.utils.egl.TextureProcessHelper
import io.agora.rtc2.Constants
import io.agora.rtc2.gl.EglBaseProvider
import io.agora.rtc2.video.IVideoFrameObserver
import io.agora.rtc2.video.VideoCanvas
import java.nio.ByteBuffer
import java.util.Collections
import java.util.concurrent.Callable

class FaceUnityBeautyAPIImpl : FaceUnityBeautyAPI, IVideoFrameObserver {
    private val TAG = "FaceUnityBeautyAPIImpl"
    private var beautyMode = 0 // 0: 自动根据buffer类型切换，1：固定使用OES纹理，2：固定使用i420，3: 单纹理模式
    private var enableTextureAsync = false // 是否开启纹理+异步缓存处理。对于GPU性能好的手机可以减小美颜处理耗时，对于中端机开启后效果也不明显。

    private var beautyTextureBufferHelper: TextureBufferHelper? = null
    private var byteBuffer: ByteBuffer? = null
    private var byteArray: ByteArray? = null
    private var config: Config? = null
    private var enable: Boolean = false
    private var enableChange: Boolean = false
    private var isReleased: Boolean = false
    private var captureMirror = false
    private var renderMirror = false
    private val identityMatrix =  Matrix()
    private var statsHelper: StatsHelper? = null
    private var skipFrame = 0
    private enum class ProcessSourceType{
        UNKNOWN,
        TEXTURE_OES_ASYNC,
        TEXTURE_2D_ASYNC,
        TEXTURE_OES,
        TEXTURE_2D,
        I420
    }
    private var currProcessSourceType = ProcessSourceType.UNKNOWN
    private var isFrontCamera = true
    private var cameraConfig = CameraConfig()
    private var localVideoRenderMode = Constants.RENDER_MODE_HIDDEN
    private val pendingProcessRunList = Collections.synchronizedList(mutableListOf<()->Unit>())
    private val transformGLFrameBuffer = GLFrameBuffer()
    private val outGLFrameBuffer = GLFrameBuffer()
    private val apiReporter by lazy {
        APIReporter(APIType.BEAUTY, VERSION, config!!.rtcEngine)
    }

    private var asyncTextureProcessHelper: TextureProcessHelper? = null
    private var asyncTextureBufferHelper: TextureBufferHelper? = null

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
        statsHelper = StatsHelper(config.statsDuration){
            this.config?.eventCallback?.onBeautyStats(it)
        }
        LogUtils.i(TAG, "initialize >> config = $config")
        LogUtils.i(TAG, "initialize >> beauty api version=$VERSION, beauty sdk version=${FURenderKit.getInstance().getVersion()}")
        apiReporter.reportFuncEvent(
            "initialize",
            mapOf(
                "captureMode" to config.cameraConfig,
                "statsDuration" to config.statsDuration,
                "statsEnable" to config.statsEnable,
                "cameraConfig" to config.cameraConfig,
            ),
            emptyMap()
        )
        apiReporter.startDurationEvent("initialize-release")
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
        apiReporter.reportFuncEvent(
            "enable",
            mapOf("enable" to enable),
            emptyMap()
        )
        if(this.enable != enable){
            this.enable = enable
            enableChange = true
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
        apiReporter.reportFuncEvent(
            "setupLocalVideo",
            mapOf("view" to view, "renderMode" to renderMode),
            emptyMap()
        )
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

    override fun updateCameraConfig(config: CameraConfig): Int {
        LogUtils.i(TAG, "updateCameraConfig >> oldCameraConfig=$cameraConfig, newCameraConfig=$config")
        cameraConfig = CameraConfig(config.frontMirror, config.backMirror)
        apiReporter.reportFuncEvent(
            "updateCameraConfig",
            mapOf("config" to config),
            emptyMap()
        )
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
        if (beautyTextureBufferHelper?.handler?.looper?.thread == Thread.currentThread()) {
            run.invoke()
        } else if (beautyTextureBufferHelper != null) {
            beautyTextureBufferHelper?.handler?.post(run)
        } else {
            pendingProcessRunList.add(run)
        }
    }

    override fun isFrontCamera() = isFrontCamera

    override fun setParameters(key: String, value: String) {
        apiReporter.reportFuncEvent("setParameters",
            mapOf("key" to key, "value" to value),
            emptyMap()
        )
        when(key){
            "beauty_mode" -> beautyMode = value.toInt()
            "enableTextureAsync" -> enableTextureAsync = value.toBoolean()
        }
    }

    override fun release(): Int {
        val conf = config
        val fuRenderer = conf?.fuRenderKit
        if(fuRenderer == null){
            LogUtils.e(TAG, "release >> The beauty api has not been initialized!")
            return ErrorCode.ERROR_HAS_NOT_INITIALIZED.value
        }
        if (isReleased) {
            LogUtils.e(TAG, "setBeautyPreset >> The beauty api has been released!")
            return ErrorCode.ERROR_HAS_RELEASED.value
        }
        LogUtils.i(TAG, "release")
        if (conf.captureMode == CaptureMode.Agora) {
            conf.rtcEngine.registerVideoFrameObserver(null)
        }
        apiReporter.reportFuncEvent("release", emptyMap(), emptyMap())
        apiReporter.endDurationEvent("initialize-release", emptyMap())

        isReleased = true
        beautyTextureBufferHelper?.let {
            beautyTextureBufferHelper = null
            it.handler.removeCallbacksAndMessages(null)
            it.invoke {
                fuRenderer.release()
                transformGLFrameBuffer.release()
                outGLFrameBuffer.release()
                null
            }
            it.dispose()
        }
        asyncTextureBufferHelper?.let {
            asyncTextureBufferHelper = null
            it.invoke {
                asyncTextureProcessHelper?.release()
                asyncTextureProcessHelper = null
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
            asyncTextureBufferHelper?.invoke {
                asyncTextureProcessHelper?.reset()
            }
            beautyTextureBufferHelper?.invoke {
                skipFrame = 2
                outGLFrameBuffer.resetTexture()
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
            asyncTextureBufferHelper?.invoke {
                asyncTextureProcessHelper?.reset()
            }
            return false
        }

        if(!enable){
            return true
        }

        if (beautyTextureBufferHelper == null) {
            beautyTextureBufferHelper = TextureBufferHelper.create(
                "FURender",
                EglBaseProvider.instance().rootEglBase.eglBaseContext
            )
            beautyTextureBufferHelper?.invoke {
                synchronized(pendingProcessRunList){
                    val iterator = pendingProcessRunList.iterator()
                    while (iterator.hasNext()){
                        iterator.next().invoke()
                        iterator.remove()
                    }
                }
            }
            LogUtils.i(TAG, "processBeauty >> create texture buffer, beautyMode=$beautyMode")
            apiReporter.startDurationEvent("first_beauty_frame")
        }
        val startTime = System.currentTimeMillis()
        val processTexId = when (beautyMode) {
            2 -> processBeautySingleBuffer(videoFrame)
            3 -> {
                if (enableTextureAsync) {
                    processBeautySingleTextureAsync(videoFrame)
                } else {
                    processBeautySingleTexture(videoFrame)
                }
            }
            else -> processBeautyAuto(videoFrame)
        }

        if(config?.statsEnable == true){
            val costTime = System.currentTimeMillis() - startTime
            statsHelper?.once(costTime)
        }

        if (processTexId <= 0) {
            LogUtils.w(TAG, "processBeauty >> processTexId <= 0")
            return false
        }

        if(skipFrame > 0){
            skipFrame --
            LogUtils.w(TAG, "processBeauty >> skipFrame=$skipFrame")
            return false
        }

        apiReporter.endDurationEvent("first_beauty_frame",
            mapOf(
                "width" to videoFrame.rotatedWidth,
                "height" to videoFrame.rotatedHeight,
                "camera_facing" to videoFrame.sourceType.name,
                "buffer_type" to videoFrame.buffer::class.java.simpleName,
            )
        )

        val processBuffer: TextureBuffer = beautyTextureBufferHelper?.wrapTextureBuffer(
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
            if (enableTextureAsync) {
                processBeautySingleTextureAsync(videoFrame)
            } else {
                processBeautySingleTexture(videoFrame)
            }
        } else {
            processBeautySingleBuffer(videoFrame)
        }
    }

    private fun processBeautySingleTextureAsync(videoFrame: VideoFrame): Int {
        val texBufferHelper = beautyTextureBufferHelper ?: return -1
        val textureBuffer = videoFrame.buffer as? TextureBuffer ?: return -1

        if (asyncTextureBufferHelper == null) {
            asyncTextureBufferHelper = TextureBufferHelper.create(
                "FURenderAsync",
                EglBaseProvider.instance().rootEglBase.eglBaseContext
            )
            LogUtils.i(TAG, "processBeauty >> create texture buffer wrap, beautyMode=$beautyMode")
        }

        when(textureBuffer.type){
            TextureBuffer.Type.OES -> {
                if(currProcessSourceType != ProcessSourceType.TEXTURE_OES_ASYNC){
                    LogUtils.i(TAG, "processBeauty >> process source type change old=$currProcessSourceType, new=${ProcessSourceType.TEXTURE_OES_ASYNC}")
                    currProcessSourceType = ProcessSourceType.TEXTURE_OES_ASYNC
                    return -1
                }
            }
            else -> {
                if(currProcessSourceType != ProcessSourceType.TEXTURE_2D_ASYNC){
                    LogUtils.i(TAG, "processBeauty >> process source type change old=$currProcessSourceType, new=${ProcessSourceType.TEXTURE_2D_ASYNC}")
                    currProcessSourceType = ProcessSourceType.TEXTURE_2D_ASYNC
                    return -1
                }
            }
        }

        if(asyncTextureProcessHelper == null) {
            asyncTextureProcessHelper = TextureProcessHelper()
            asyncTextureProcessHelper?.setFilter { frame ->
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
                        it.inputBufferMatrix = FUTransformMatrixEnum.CCROT0_FLIPVERTICAL
                        it.inputTextureMatrix = FUTransformMatrixEnum.CCROT0_FLIPVERTICAL
                        it.outputMatrix = FUTransformMatrixEnum.CCROT0
                        it.deviceOrientation = 90
                    } else {
                        it.cameraFacing = CameraFacingEnum.CAMERA_BACK
                        it.inputBufferMatrix = FUTransformMatrixEnum.CCROT0_FLIPVERTICAL
                        it.inputTextureMatrix = FUTransformMatrixEnum.CCROT0_FLIPVERTICAL
                        it.outputMatrix = FUTransformMatrixEnum.CCROT0
                        it.deviceOrientation = 90
                    }
                }
                if (isReleased) {
                    return@setFilter -1
                }
                val ret = texBufferHelper.invoke {
                    synchronized(EglBase.lock){
                        return@invoke fuRenderKit.renderWithInput(input).texture?.texId ?: -1
                    }
                }
                return@setFilter ret ?: -1
            }
        }

        return asyncTextureBufferHelper?.invoke {
            if(isReleased){
                return@invoke -1
            }

            return@invoke asyncTextureProcessHelper?.process(
                textureBuffer.textureId,
                when (textureBuffer.type) {
                    TextureBuffer.Type.OES -> GLES11Ext.GL_TEXTURE_EXTERNAL_OES
                    else -> GLES20.GL_TEXTURE_2D
                },
                textureBuffer.width,
                textureBuffer.height,
                videoFrame.rotation,
                textureBuffer.transformMatrixArray,
                isFrontCamera,
                (isFrontCamera && !captureMirror) || (!isFrontCamera && captureMirror)
            )?: -1
        } ?: -1
    }

    private fun processBeautySingleTexture(videoFrame: VideoFrame): Int {
        val texBufferHelper = beautyTextureBufferHelper ?: return -1
        val textureBuffer = videoFrame.buffer as? TextureBuffer ?: return -1

        when(textureBuffer.type){
            TextureBuffer.Type.OES -> {
                if(currProcessSourceType != ProcessSourceType.TEXTURE_OES){
                    LogUtils.i(TAG, "processBeauty >> process source type change old=$currProcessSourceType, new=${ProcessSourceType.TEXTURE_OES}")
                    currProcessSourceType = ProcessSourceType.TEXTURE_OES
                    return -1
                }
            }
            else -> {
                if(currProcessSourceType != ProcessSourceType.TEXTURE_2D){
                    LogUtils.i(TAG, "processBeauty >> process source type change old=$currProcessSourceType, new=${ProcessSourceType.TEXTURE_2D}")
                    currProcessSourceType = ProcessSourceType.TEXTURE_2D
                    return -1
                }
            }
        }

        val width = videoFrame.rotatedWidth
        val height = videoFrame.rotatedHeight
        val isFront = videoFrame.sourceType == SourceType.kFrontCamera
        val rotation = videoFrame.rotation

        return texBufferHelper.invoke {
            val fuRenderKit = config?.fuRenderKit ?: return@invoke -1

            transformGLFrameBuffer.setSize(width, height)
            transformGLFrameBuffer.resetTransform()
            transformGLFrameBuffer.setTexMatrix(textureBuffer.transformMatrixArray)
            transformGLFrameBuffer.setRotation(rotation)
            var flipH = isFront
            if((isFrontCamera && !captureMirror) || (!isFrontCamera && captureMirror)){
                flipH = !flipH
            }
            transformGLFrameBuffer.setFlipH(flipH)
            val transformTexId = transformGLFrameBuffer.process(
                textureBuffer.textureId, when (textureBuffer.type) {
                    TextureBuffer.Type.OES -> GLES11Ext.GL_TEXTURE_EXTERNAL_OES
                    else -> GLES20.GL_TEXTURE_2D
                }
            )

            val input = FURenderInputData(width, height)
            input.texture = FURenderInputData.FUTexture(
                FUInputTextureEnum.FU_ADM_FLAG_COMMON_TEXTURE,
                transformTexId
            )
            input.renderConfig.let {
                if (isFront) {
                    it.cameraFacing = CameraFacingEnum.CAMERA_FRONT
                    it.inputBufferMatrix = FUTransformMatrixEnum.CCROT0_FLIPVERTICAL
                    it.inputTextureMatrix = FUTransformMatrixEnum.CCROT0_FLIPVERTICAL
                    it.outputMatrix = FUTransformMatrixEnum.CCROT0
                    it.deviceOrientation = 90
                } else {
                    it.cameraFacing = CameraFacingEnum.CAMERA_BACK
                    it.inputBufferMatrix = FUTransformMatrixEnum.CCROT0_FLIPVERTICAL
                    it.inputTextureMatrix = FUTransformMatrixEnum.CCROT0_FLIPVERTICAL
                    it.outputMatrix = FUTransformMatrixEnum.CCROT0
                    it.deviceOrientation = 90
                }
            }
            if (isReleased) {
                return@invoke -1
            }
            var fuTexId = -1
            synchronized(EglBase.lock){
                fuTexId = fuRenderKit.renderWithInput(input).texture?.texId ?: -1
            }
            outGLFrameBuffer.setSize(videoFrame.rotatedWidth, videoFrame.rotatedHeight)
            outGLFrameBuffer.resetTransform()
            return@invoke outGLFrameBuffer.process(fuTexId, GLES20.GL_TEXTURE_2D)
        }
    }

    private fun processBeautySingleBuffer(videoFrame: VideoFrame): Int {
        val texBufferHelper = beautyTextureBufferHelper ?: return -1
        if(currProcessSourceType != ProcessSourceType.I420){
            LogUtils.i(TAG, "processBeauty >> process source type change old=$currProcessSourceType, new=${ProcessSourceType.I420}")
            currProcessSourceType = ProcessSourceType.I420
            return -1
        }
        val bufferArray = getNV21Buffer(videoFrame) ?: return -1
        val buffer = videoFrame.buffer
        val width = buffer.width
        val height = buffer.height
        val isFront = videoFrame.sourceType == SourceType.kFrontCamera
        val mirror = (isFrontCamera && !captureMirror) || (!isFrontCamera && captureMirror)
        val rotation = videoFrame.rotation

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
                    it.inputBufferMatrix = if(mirror) {
                        when (rotation) {
                            0 ->  FUTransformMatrixEnum.CCROT0
                            180 -> FUTransformMatrixEnum.CCROT180
                            else -> FUTransformMatrixEnum.CCROT90
                        }
                    } else {
                        when (rotation) {
                            0 -> FUTransformMatrixEnum.CCROT0_FLIPHORIZONTAL
                            180 -> FUTransformMatrixEnum.CCROT0_FLIPVERTICAL
                            else -> FUTransformMatrixEnum.CCROT90_FLIPHORIZONTAL
                        }
                    }
                    it.inputTextureMatrix = if(mirror) {
                        when (rotation) {
                            0 -> FUTransformMatrixEnum.CCROT0
                            180 -> FUTransformMatrixEnum.CCROT180
                            else -> FUTransformMatrixEnum.CCROT90
                        }
                    } else {
                        when (rotation) {
                            0 -> FUTransformMatrixEnum.CCROT0_FLIPHORIZONTAL
                            180 -> FUTransformMatrixEnum.CCROT0_FLIPVERTICAL
                            else -> FUTransformMatrixEnum.CCROT90_FLIPHORIZONTAL
                        }
                    }
                    it.deviceOrientation = when(rotation){
                        0 -> 270
                        180 -> 90
                        else -> 0
                    }
                    it.outputMatrix = FUTransformMatrixEnum.CCROT0
                } else {
                    it.cameraFacing = CameraFacingEnum.CAMERA_BACK
                    it.inputBufferMatrix = if(mirror) {
                        when (rotation) {
                            0 ->  FUTransformMatrixEnum.CCROT0_FLIPHORIZONTAL
                            180 -> FUTransformMatrixEnum.CCROT0_FLIPVERTICAL
                            else -> FUTransformMatrixEnum.CCROT90_FLIPVERTICAL
                        }
                    } else {
                        when (rotation) {
                            0 -> FUTransformMatrixEnum.CCROT0
                            180 -> FUTransformMatrixEnum.CCROT180
                            else -> FUTransformMatrixEnum.CCROT270
                        }
                    }
                    it.inputTextureMatrix = if(mirror) {
                        when (rotation) {
                            0 -> FUTransformMatrixEnum.CCROT0_FLIPHORIZONTAL
                            180 -> FUTransformMatrixEnum.CCROT0_FLIPVERTICAL
                            else -> FUTransformMatrixEnum.CCROT90_FLIPVERTICAL
                        }
                    } else {
                        when (rotation) {
                            0 -> FUTransformMatrixEnum.CCROT0
                            180 -> FUTransformMatrixEnum.CCROT180
                            else -> FUTransformMatrixEnum.CCROT270
                        }
                    }
                    it.deviceOrientation = when(rotation){
                        0 -> 270
                        180 -> 90
                        else -> 0
                    }
                    it.outputMatrix = FUTransformMatrixEnum.CCROT0
                }
            }

            if ((asyncTextureProcessHelper?.size() ?: 0) > 0) {
                asyncTextureBufferHelper?.invoke {
                    asyncTextureProcessHelper?.reset()
                }
                return@Callable -1
            }

            var fuTexId = -1
            synchronized(EglBase.lock) {
                fuTexId = fuRenderKit.renderWithInput(input).texture?.texId ?: -1
            }
            outGLFrameBuffer.setSize(videoFrame.rotatedWidth, videoFrame.rotatedHeight)
            outGLFrameBuffer.resetTransform()
            return@Callable outGLFrameBuffer.process(fuTexId, GLES20.GL_TEXTURE_2D);
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

    override fun getMirrorApplied() = captureMirror && !enable

    override fun getObservedFramePosition() = IVideoFrameObserver.POSITION_POST_CAPTURER

}