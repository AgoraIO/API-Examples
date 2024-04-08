package io.agora.beautyapi.sensetime.utils.processor

import android.annotation.TargetApi
import android.content.Context
import android.opengl.GLES20
import android.os.Build
import com.softsugar.hardwarebuffer.STMobileHardwareBufferNative
import com.softsugar.stmobile.STCommonNative
import com.softsugar.stmobile.STMobileColorConvertNative
import com.softsugar.stmobile.STMobileEffectNative
import com.softsugar.stmobile.STMobileHumanActionNative
import com.softsugar.stmobile.model.STEffectCustomParam
import com.softsugar.stmobile.model.STEffectRenderInParam
import com.softsugar.stmobile.model.STEffectRenderOutParam
import com.softsugar.stmobile.model.STEffectTexture
import com.softsugar.stmobile.model.STQuaternion
import com.softsugar.stmobile.params.STEffectParam
import com.softsugar.stmobile.sticker_module_types.STCustomEvent
import io.agora.beautyapi.sensetime.utils.LogUtils
import io.agora.beautyapi.sensetime.utils.egl.GLCopyHelper
import io.agora.beautyapi.sensetime.utils.egl.GLFrameBuffer
import io.agora.beautyapi.sensetime.utils.egl.GLTextureBufferQueue
import io.agora.beautyapi.sensetime.utils.processor.Accelerometer.CLOCKWISE_ANGLE

class BeautyProcessor : IBeautyProcessor {
    private val TAG = this::class.java.simpleName

    private val glCopyHelper = GLCopyHelper()
    private val glFrameBuffer = GLFrameBuffer()
    private val glTextureBufferQueue = GLTextureBufferQueue(glFrameBuffer)

    private var mProcessWidth = 0
    private var mProcessHeight = 0
    private var processInTextureId = -1
    private var beautyOutTextureId = -1
    private var finalOutTextureId = -1

    private lateinit var mSTMobileEffectNative: STMobileEffectNative
    private lateinit var mFaceDetector: FaceDetector
    private var mSTMobileColorConvertNative: STMobileColorConvertNative? = null
    private var mSTMobileHardwareBufferNative: STMobileHardwareBufferNative? = null

    private var mCustomEvent = 0
    private var mInputWidth = 0
    private var mInputHeight = 0
    private var mInputOrientation = 0
    private var isLastFrontCamera = false

    @Volatile
    private var isReleased = false

    override fun initialize(
        effectNative: STMobileEffectNative,
        humanActionNative: STMobileHumanActionNative
    ) {
        this.mSTMobileEffectNative = effectNative
        mFaceDetector = FaceDetector(humanActionNative, effectNative)
    }

    override fun release() {
        isReleased = true
        mFaceDetector.release()
        if (processInTextureId != -1) {
            GLES20.glDeleteTextures(1, intArrayOf(processInTextureId), 0)
            processInTextureId = -1
        }
        if (beautyOutTextureId != -1) {
            GLES20.glDeleteTextures(1, intArrayOf(beautyOutTextureId), 0)
            beautyOutTextureId = -1
        }
        if (finalOutTextureId != -1) {
            GLES20.glDeleteTextures(1, intArrayOf(finalOutTextureId), 0)
            finalOutTextureId = -1
        }
        if (mSTMobileColorConvertNative != null) {
            mSTMobileColorConvertNative?.destroyInstance()
            mSTMobileColorConvertNative = null
        }
        glFrameBuffer.release()
        glCopyHelper.release()
        glTextureBufferQueue.release()
        mSTMobileHardwareBufferNative?.release()
        mSTMobileHardwareBufferNative = null
    }

    override fun enableSensor(context: Context, enable: Boolean) {
        mFaceDetector.enableSensor(context, enable)
    }

    override fun triggerScreenTap(isDouble: Boolean) {
        LogUtils.d(
            TAG,
            "changeCustomEvent() called:" + mSTMobileEffectNative.customEventNeeded
        )
        mCustomEvent = mSTMobileEffectNative.customEventNeeded
        mCustomEvent = if (isDouble) {
            (mCustomEvent.toLong() and STCustomEvent.ST_CUSTOM_EVENT_SCREEN_TAP.inv()).toInt()
        } else {
            (mCustomEvent.toLong() and STCustomEvent.ST_CUSTOM_EVENT_SCREEN_DOUBLE_TAP.inv()).toInt()
        }
    }


    override fun process(input: InputInfo): OutputInfo? {
        if (isReleased) {
            return null
        }
        return if (input.bytes != null && input.textureId != null) {
            processDoubleInput(input)
        } else if (input.bytes != null) {
            processSingleBytesInput(input)
        } else if (input.textureId != null && Build.VERSION.SDK_INT >= 26) {
            processSingleTextureInput(input)
        } else {
            null
        }
    }

    @TargetApi(26)
    private fun processSingleTextureInput(input: InputInfo): OutputInfo? {
        if (isReleased) {
            return null
        }
        if (input.textureId == null) {
            return null
        }
        val width = input.width
        val height = input.height

        if(processInTextureId == -1){
            processInTextureId = glFrameBuffer.createTexture(width, height)
        }

        if (mSTMobileHardwareBufferNative == null) {
            mProcessWidth = width
            mProcessHeight = height
            glFrameBuffer.resizeTexture(processInTextureId, width, height)
            mSTMobileHardwareBufferNative = STMobileHardwareBufferNative().apply {
                init(
                    width,
                    height,
                    STMobileHardwareBufferNative.HARDWARE_BUFFER_FORMAT_RGBA,
                    STMobileHardwareBufferNative.HARDWARE_BUFFER_USAGE_DOWNLOAD
                )
            }
        } else if (mProcessWidth != width || mProcessHeight != height) {
            mSTMobileHardwareBufferNative?.release()
            mSTMobileHardwareBufferNative = null
            glFrameBuffer.resizeTexture(processInTextureId, width, height)
            return null
        }

        glFrameBuffer.textureId = processInTextureId
        glFrameBuffer.setSize(width, height)
        glFrameBuffer.resetTransform()
        glFrameBuffer.setTexMatrix(input.textureMatrix)
        glFrameBuffer.setFlipV(true)
        glFrameBuffer.process(input.textureId, input.textureType)

        val outBuffer = ByteArray(width * height * 4)
        mSTMobileHardwareBufferNative?.let {
            glCopyHelper.copy2DTextureToOesTexture(
                processInTextureId,
                it.textureId,
                width,
                height,
                0
            )
            it.downloadRgbaImage(
                width,
                height,
                outBuffer
            )
        }

        GLES20.glFinish()

        return processDoubleInput(
            InputInfo(
                outBuffer,
                STCommonNative.ST_PIX_FMT_RGBA8888,
                processInTextureId,
                GLES20.GL_TEXTURE_2D,
                null,
                1,
                width,
                height,
                input.isFrontCamera,
                input.isMirror,
                input.cameraOrientation,
                input.timestamp,
            )
        )
    }

    private fun processSingleBytesInput(input: InputInfo): OutputInfo? {
        if (isReleased) {
            return null
        }
        if (input.bytes == null) {
            return null
        }
        if (processInTextureId == -1) {
            processInTextureId = glFrameBuffer.createTexture(input.width, input.height)
        }

        if (mSTMobileColorConvertNative == null) {
            mProcessWidth = input.width
            mProcessHeight = input.height
            glFrameBuffer.resizeTexture(processInTextureId, input.width, input.height)
            mSTMobileColorConvertNative = STMobileColorConvertNative().apply {
                createInstance()
                setTextureSize(mProcessWidth, mProcessHeight)
            }
        } else if (mProcessWidth != input.width || mProcessHeight != input.height) {
            mSTMobileColorConvertNative?.destroyInstance()
            mSTMobileColorConvertNative = null
            return null
        }
        //上传nv21 buffer到纹理
        mSTMobileColorConvertNative?.nv21BufferToRgbaTexture(
            input.width,
            input.height,
            0,
            false,
            input.bytes,
            processInTextureId
        )
        return processDoubleInput(
            InputInfo(
                input.bytes,
                input.bytesType,
                processInTextureId,
                GLES20.GL_TEXTURE_2D,
                input.textureMatrix,
                0,
                input.width,
                input.height,
                input.isFrontCamera,
                input.isMirror,
                input.cameraOrientation,
                input.timestamp,
            )
        )
    }

    private fun processDoubleInput(input: InputInfo): OutputInfo? {
        if (isReleased) {
            return null
        }
        if (input.bytes == null || input.textureId == null) {
            return null
        }
        if (mInputWidth != input.width || mInputHeight != input.height || mInputOrientation != input.cameraOrientation || isLastFrontCamera != input.isFrontCamera) {
            mInputWidth = input.width
            mInputHeight = input.height
            mInputOrientation = input.cameraOrientation
            isLastFrontCamera = input.isFrontCamera
            reset()
            return null
        }

        val diff = glTextureBufferQueue.size() - mFaceDetector.size()
        if(diff < input.diffBetweenBytesAndTexture){
            glTextureBufferQueue.enqueue(
                GLTextureBufferQueue.TextureIn(
                    input.textureId,
                    input.textureType,
                    input.width,
                    input.height,
                    input.cameraOrientation,
                    input.isFrontCamera,
                    input.isMirror,
                    input.textureMatrix
                )
            )
            return null
        } else if(diff > input.diffBetweenBytesAndTexture){
            mFaceDetector.reset()
            glTextureBufferQueue.reset()
            return null
        } else {
            glTextureBufferQueue.enqueue(
                GLTextureBufferQueue.TextureIn(
                    input.textureId,
                    input.textureType,
                    input.width,
                    input.height,
                    input.cameraOrientation,
                    input.isFrontCamera,
                    input.isMirror,
                    input.textureMatrix
                )
            )
        }

        val detectorOut = mFaceDetector.dequeue()
        var out : OutputInfo? = null
        if (detectorOut != null) {
            val textureOut = glTextureBufferQueue.dequeue()
            if (textureOut != null) {
                val outTextureId = effectApply(
                    textureOut.textureId,
                    detectorOut,
                    textureOut.width,
                    textureOut.height,
                    getCurrentOrientation(),
                    textureOut.isFrontCamera
                )
                out = OutputInfo(
                    outTextureId,
                    GLES20.GL_TEXTURE_2D,
                    textureOut.width,
                    textureOut.height,
                    System.nanoTime()
                )
            } else {
                LogUtils.e(TAG, "The face detector out can not found its texture out!")
            }
        }
        mFaceDetector.enqueue(
            FaceDetector.DetectorIn(
                input.bytes,
                input.bytesType,
                input.width,
                input.height,
                input.isFrontCamera,
                input.isMirror,
                input.cameraOrientation
            )
        )

        return out
    }

    private fun effectApply(
        textureId: Int,
        detectorInfo: FaceDetector.DetectorOut,
        width: Int,
        height: Int,
        orientation: Int,
        isFrontCamera: Boolean
    ): Int {
        var beautyOutTextureId = this@BeautyProcessor.beautyOutTextureId
        if (beautyOutTextureId == -1) {
            beautyOutTextureId = glFrameBuffer.createTexture(width, height)
            this@BeautyProcessor.beautyOutTextureId = beautyOutTextureId
        }
        var finalOutTextureId = this@BeautyProcessor.finalOutTextureId
        if (finalOutTextureId == -1) {
            finalOutTextureId = glFrameBuffer.createTexture(width, height)
            this@BeautyProcessor.finalOutTextureId = finalOutTextureId
        }

        //输入纹理
        val stEffectTexture =
            STEffectTexture(
                textureId,
                width,
                height,
                0
            )
        //输出纹理，需要在上层初始化
        val stEffectTextureOut =
            STEffectTexture(beautyOutTextureId, width, height, 0)

        //用户自定义参数设置
        val event: Int = mCustomEvent
        val customParam: STEffectCustomParam
        val sensorEvent = mFaceDetector.getAccelerometer()?.sensorEvent
        customParam =
            if (sensorEvent?.values != null && sensorEvent.values.isNotEmpty()) {
                STEffectCustomParam(
                    STQuaternion(sensorEvent.values),
                    isFrontCamera,
                    event
                )
            } else {
                STEffectCustomParam(
                    STQuaternion(0f, 0f, 0f, 1f),
                    isFrontCamera,
                    event
                )
            }

        //渲染接口输入参数
        val sTEffectRenderInParam = STEffectRenderInParam(
            detectorInfo.humanResult,
            detectorInfo.animalResult,
            orientation,
            orientation,
            false,
            customParam,
            stEffectTexture,
            null
        )
        //渲染接口输出参数
        val stEffectRenderOutParam = STEffectRenderOutParam(
            stEffectTextureOut,
            null,
            null
        )
        mSTMobileEffectNative.setParam(
            STEffectParam.EFFECT_PARAM_USE_INPUT_TIMESTAMP,
            1.0f
        )
        mSTMobileEffectNative.render(
            sTEffectRenderInParam,
            stEffectRenderOutParam,
            false
        )

        if (event == mCustomEvent) {
            mCustomEvent = 0
        }

        glFrameBuffer.setSize(width, height)
        glFrameBuffer.resetTransform()
        glFrameBuffer.setFlipV(true)
        glFrameBuffer.textureId = finalOutTextureId
        glFrameBuffer.process(
            stEffectRenderOutParam.texture?.id ?: 0,
            GLES20.GL_TEXTURE_2D
        )
        GLES20.glFinish()
        return finalOutTextureId
    }

    override fun reset() {
        mFaceDetector.reset()
        glTextureBufferQueue.reset()
        if (beautyOutTextureId != -1) {
            GLES20.glDeleteTextures(1, intArrayOf(beautyOutTextureId), 0)
            beautyOutTextureId = -1
        }
        if (finalOutTextureId != -1) {
            GLES20.glDeleteTextures(1, intArrayOf(finalOutTextureId), 0)
            finalOutTextureId = -1
        }
    }


    private fun getCurrentOrientation(): Int {
        val dir = mFaceDetector.getAccelerometer()?.direction ?: CLOCKWISE_ANGLE.Deg90.value
        var orientation = dir - 1
        if (orientation < 0) {
            orientation = dir xor 3
        }
        return orientation
    }


}