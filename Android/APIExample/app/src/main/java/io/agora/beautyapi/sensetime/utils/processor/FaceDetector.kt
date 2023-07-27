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

package io.agora.beautyapi.sensetime.utils.processor

import android.content.Context
import android.hardware.Camera
import android.util.Log
import android.util.Size
import com.sensetime.stmobile.STMobileAnimalNative
import com.sensetime.stmobile.STMobileEffectNative
import com.sensetime.stmobile.STMobileHumanActionNative
import com.sensetime.stmobile.model.STAnimalFace
import com.sensetime.stmobile.model.STAnimalFaceInfo
import com.sensetime.stmobile.model.STHumanAction
import com.sensetime.stmobile.params.STRotateType
import io.agora.beautyapi.sensetime.utils.utils.Accelerometer
import io.agora.beautyapi.sensetime.utils.utils.LogUtils
import java.util.concurrent.Callable
import java.util.concurrent.ConcurrentLinkedQueue
import java.util.concurrent.Executors
import java.util.concurrent.Future

class FaceDetector(
    private val humanActionNative: STMobileHumanActionNative,
    private val effectNative: STMobileEffectNative,
    private val animalNative: STMobileAnimalNative?
) {
    private val TAG = "FaceDetector"

    private val workerThread = Executors.newSingleThreadExecutor()
    private var accelerometer: Accelerometer? = null

    private val cacheSize = 2
    private var cacheIndex = 0
    private var cacheAnimalFaceInfo = arrayOfNulls<STAnimalFaceInfo>(cacheSize)
    private val cacheFutureQueue = ConcurrentLinkedQueue<Future<Int>>()
    private var isDequeBegin = false

    fun enableSensor(context: Context, enable: Boolean) {
        if (enable) {
            if (accelerometer == null) {
                accelerometer = Accelerometer(context)
                accelerometer?.start()
            }
        } else {
            accelerometer?.stop()
            accelerometer = null
        }
    }

    fun getAccelerometer() = accelerometer

    fun reset() {
        cacheIndex = 0
        isDequeBegin = false
        var future = cacheFutureQueue.poll()
        while (future != null){
            future.cancel(true)
            future = cacheFutureQueue.poll()
        }
        cacheAnimalFaceInfo.forEachIndexed { index, _ ->
            cacheAnimalFaceInfo[index] = null
        }
    }

    fun release(){
        reset()
        accelerometer?.stop()
        workerThread.shutdownNow()
    }

    fun enqueue(iN: DetectorIn): Int {
        val index = cacheIndex
        val size = cacheFutureQueue.size
        if (size <= cacheSize) {
            cacheFutureQueue.offer(
                workerThread.submit(Callable {
                    detectHuman(iN, index)
                    detectAnim(iN, index)
                    return@Callable index
                })
            )
            Log.d(TAG, "Detector enqueue cacheIndex: $cacheIndex, queue size: $size")
            cacheIndex = (cacheIndex + 1) % cacheSize
        } else {
            Log.e(TAG, "Detector queue is full!!")
        }
        return size
    }

    fun dequeue(): DetectorOut? {
        val size = cacheFutureQueue.size
        if(isDequeBegin || size >= cacheSize){
            isDequeBegin = true
            val future = cacheFutureQueue.poll()
            if(future != null){
                try {
                    val ret = future.get()
                    Log.d(TAG, "Detector dequeue cacheIndex: $ret, queue size: $size")
                    return DetectorOut(
                        humanActionNative.getNativeHumanActionResultCache(ret),
                        cacheAnimalFaceInfo[ret]
                    )
                }catch (e: Exception){
                    Log.d(TAG, "Detector dequeue timeout: $e")
                }
            }
        }
        Log.d(TAG, "Detector dequeue null, queue size: $size")
        return null
    }

    fun size() = cacheFutureQueue.size

    private fun detectHuman(iN: DetectorIn, index: Int) {
        val orientation: Int =
            if (iN.orientation == 0) STRotateType.ST_CLOCKWISE_ROTATE_0 else getHumanActionOrientation(
                iN.isFront,
                iN.orientation
            )
        val deviceOrientation: Int =
            accelerometer?.direction ?: Accelerometer.CLOCKWISE_ANGLE.Deg90.value
        val startHumanAction = System.currentTimeMillis()
        //Log.e(TAG, "config: "+Long.toHexString(mDetectConfig) );
        val ret: Int = humanActionNative.nativeHumanActionDetectPtr(
            iN.bytes,
            iN.bytesType,
            effectNative.humanActionDetectConfig,
            orientation,
            iN.width,
            iN.height
        )

        LogUtils.i(
            TAG,
            "human action cost time: %d, ret: %d",
            System.currentTimeMillis() - startHumanAction, ret
        )
        //nv21数据为横向，相对于预览方向需要旋转处理，前置摄像头还需要镜像
        val rotatedSize = when (iN.orientation) {
            90, 270 -> Size(iN.height, iN.width)
            else -> Size(iN.width, iN.height)
        }
        STHumanAction.nativeHumanActionRotateAndMirror(
            humanActionNative,
            humanActionNative.nativeHumanActionResultPtr,
            rotatedSize.width,
            rotatedSize.height,
            if (iN.isFront) Camera.CameraInfo.CAMERA_FACING_FRONT else Camera.CameraInfo.CAMERA_FACING_BACK,
            iN.orientation,
            deviceOrientation
        )
        LogUtils.i("processDoubleInput index=$index nativeHumanActionDetect end")
        humanActionNative.updateNativeHumanActionCache(index)
    }

    private fun detectAnim(
        iN: DetectorIn,
        index: Int
    ) {
        if (animalNative != null) {
            animalDetect(
                iN.bytes,
                iN.bytesType,
                getHumanActionOrientation(iN.isFront, iN.orientation),
                iN.height,
                iN.width,
                index,
                iN.isFront,
                iN.orientation
            )
        } else {
            cacheAnimalFaceInfo[index] = STAnimalFaceInfo(null, 0)
        }
    }

    /**
     * 用于humanActionDetect接口。根据传感器方向计算出在不同设备朝向时，人脸在buffer中的朝向
     *
     * @return 人脸在buffer中的朝向
     */
    private fun getHumanActionOrientation(frontCamera: Boolean, cameraRotation: Int): Int {
        //获取重力传感器返回的方向
        var orientation: Int = accelerometer?.direction ?: Accelerometer.CLOCKWISE_ANGLE.Deg90.value

        //在使用后置摄像头，且传感器方向为0或2时，后置摄像头与前置orientation相反
        if (!frontCamera && orientation == STRotateType.ST_CLOCKWISE_ROTATE_0) {
            orientation = STRotateType.ST_CLOCKWISE_ROTATE_180
        } else if (!frontCamera && orientation == STRotateType.ST_CLOCKWISE_ROTATE_180) {
            orientation = STRotateType.ST_CLOCKWISE_ROTATE_0
        }

        // 请注意前置摄像头与后置摄像头旋转定义不同 && 不同手机摄像头旋转定义不同
        if (cameraRotation == 270 && orientation and STRotateType.ST_CLOCKWISE_ROTATE_90 == STRotateType.ST_CLOCKWISE_ROTATE_90
            || cameraRotation == 90 && orientation and STRotateType.ST_CLOCKWISE_ROTATE_90 == STRotateType.ST_CLOCKWISE_ROTATE_0
        ) {
            orientation = orientation xor STRotateType.ST_CLOCKWISE_ROTATE_180
        }

        return orientation
    }

    private fun animalDetect(
        imageData: ByteArray?,
        format: Int,
        orientation: Int,
        width: Int,
        height: Int,
        index: Int,
        isFrontCamera: Boolean,
        cameraRotation: Int
    ) {
        val need = animalNative != null
        if (need) {
            val catDetectStartTime = System.currentTimeMillis()
            val animalDetectConfig = effectNative.animalDetectConfig.toInt()
            Log.d(
                TAG,
                "test_animalDetect: $animalDetectConfig"
            )
            var animalFaces: Array<STAnimalFace?>? = animalNative?.animalDetect(
                imageData,
                format,
                orientation,
                animalDetectConfig,
                width,
                height
            )
            LogUtils.i(
                TAG,
                "animal detect cost time: %d",
                System.currentTimeMillis() - catDetectStartTime
            )
            if (!animalFaces.isNullOrEmpty()) {
                Log.d(
                    TAG,
                    "animalDetect: " + animalFaces.size
                )
                animalFaces = processAnimalFaceResult(
                    animalFaces,
                    width,
                    height,
                    isFrontCamera,
                    cameraRotation
                )
            }
            cacheAnimalFaceInfo[index] = STAnimalFaceInfo(animalFaces, animalFaces?.size ?: 0)
        } else {
            cacheAnimalFaceInfo[index] = STAnimalFaceInfo(null, 0)
        }
    }


    private fun processAnimalFaceResult(
        animalFaces: Array<STAnimalFace?>?,
        width: Int,
        height: Int,
        isFrontCamera: Boolean,
        cameraOrientation: Int
    ): Array<STAnimalFace?>? {
        var animalFacesRet = animalFaces ?: return null
        if (isFrontCamera && cameraOrientation == 90) {
            animalFacesRet = STMobileAnimalNative.animalRotate(
                height,
                width,
                STRotateType.ST_CLOCKWISE_ROTATE_90,
                animalFacesRet,
                animalFacesRet.size
            )
            animalFacesRet =
                STMobileAnimalNative.animalMirror(width, animalFacesRet, animalFacesRet.size)
        } else if (isFrontCamera && cameraOrientation == 270) {
            animalFacesRet = STMobileAnimalNative.animalRotate(
                height,
                width,
                STRotateType.ST_CLOCKWISE_ROTATE_270,
                animalFacesRet,
                animalFacesRet.size
            )
            animalFacesRet =
                STMobileAnimalNative.animalMirror(width, animalFacesRet, animalFacesRet.size)
        } else if (!isFrontCamera && cameraOrientation == 270) {
            animalFacesRet = STMobileAnimalNative.animalRotate(
                height,
                width,
                STRotateType.ST_CLOCKWISE_ROTATE_270,
                animalFacesRet,
                animalFacesRet.size
            )
        } else if (!isFrontCamera && cameraOrientation == 90) {
            animalFacesRet = STMobileAnimalNative.animalRotate(
                height,
                width,
                STRotateType.ST_CLOCKWISE_ROTATE_90,
                animalFacesRet,
                animalFacesRet.size
            )
        }
        return animalFacesRet
    }

    data class DetectorIn(
        val bytes: ByteArray,
        val bytesType: Int,
        val width: Int,
        val height: Int,
        val isFront: Boolean,
        val orientation: Int,
    )

    data class DetectorOut(
        val humanResult: Long,
        val animalResult: STAnimalFaceInfo?
    )
}