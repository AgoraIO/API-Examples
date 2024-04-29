package io.agora.beautyapi.sensetime.utils.processor

import android.content.Context
import android.opengl.GLES20
import com.softsugar.stmobile.STCommonNative
import com.softsugar.stmobile.STMobileEffectNative
import com.softsugar.stmobile.STMobileHumanActionNative

data class InputInfo(
    val bytes: ByteArray? = null,
    val bytesType: Int = STCommonNative.ST_PIX_FMT_NV21,
    val textureId: Int? = null,
    val textureType: Int = GLES20.GL_TEXTURE_2D,
    val textureMatrix: FloatArray? = null,
    val diffBetweenBytesAndTexture: Int = 0,
    val width: Int,
    val height: Int,
    val isFrontCamera: Boolean,
    val isMirror: Boolean,
    val cameraOrientation: Int,
    val timestamp: Long,

    )

class OutputInfo(
    val textureId: Int = 0,
    val textureType: Int = GLES20.GL_TEXTURE_2D,
    val width: Int = 0,
    val height: Int = 0,
    val timestamp: Long = 0,
    val errorCode: Int = 0,
    val errorMessage: String = "",
)

interface IBeautyProcessor {

    fun initialize(
        effectNative: STMobileEffectNative, // 美颜效果处理句柄
        humanActionNative: STMobileHumanActionNative, // 人脸检测句柄
    )

    fun process(input: InputInfo): OutputInfo?

    fun enableSensor(context: Context, enable: Boolean)

    fun triggerScreenTap(isDouble: Boolean)

    fun reset()
    
    fun release()

}

fun createBeautyProcessor(): IBeautyProcessor = BeautyProcessor()