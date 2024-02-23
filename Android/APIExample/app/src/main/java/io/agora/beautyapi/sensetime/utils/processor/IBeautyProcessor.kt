package io.agora.beautyapi.sensetime.utils.processor

import android.content.Context
import android.opengl.GLES20
import com.softsugar.stmobile.STCommonNative
import com.softsugar.stmobile.STMobileEffectNative
import com.softsugar.stmobile.STMobileHumanActionNative

/**
 * Input info
 *
 * @property bytes
 * @property bytesType
 * @property textureId
 * @property textureType
 * @property textureMatrix
 * @property diffBetweenBytesAndTexture
 * @property width
 * @property height
 * @property isFrontCamera
 * @property isMirror
 * @property cameraOrientation
 * @property timestamp
 * @constructor Create empty Input info
 */
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
    val timestamp: Long

    )

/**
 * Output info
 *
 * @property textureId
 * @property textureType
 * @property width
 * @property height
 * @property timestamp
 * @property errorCode
 * @property errorMessage
 * @constructor Create empty Output info
 */
class OutputInfo(
    val textureId: Int = 0,
    val textureType: Int = GLES20.GL_TEXTURE_2D,
    val width: Int = 0,
    val height: Int = 0,
    val timestamp: Long = 0,
    val errorCode: Int = 0,
    val errorMessage: String = ""
)

/**
 * I beauty processor
 *
 * @constructor Create empty I beauty processor
 */
interface IBeautyProcessor {

    /**
     * Initialize
     *
     * @param effectNative
     * @param humanActionNative
     */
    fun initialize(
        effectNative: STMobileEffectNative, // 美颜效果处理句柄
        humanActionNative: STMobileHumanActionNative // 人脸检测句柄
    )

    /**
     * Process
     *
     * @param input
     * @return
     */
    fun process(input: InputInfo): OutputInfo?

    /**
     * Enable sensor
     *
     * @param context
     * @param enable
     */
    fun enableSensor(context: Context, enable: Boolean)

    /**
     * Trigger screen tap
     *
     * @param isDouble
     */
    fun triggerScreenTap(isDouble: Boolean)

    /**
     * Reset
     *
     */
    fun reset()

    /**
     * Release
     *
     */
    fun release()

}

/**
 * Create beauty processor
 *
 * @return
 */
fun createBeautyProcessor(): IBeautyProcessor = BeautyProcessor()