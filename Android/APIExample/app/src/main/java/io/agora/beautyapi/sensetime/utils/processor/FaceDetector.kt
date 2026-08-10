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
import android.util.Size
import com.softsugar.stmobile.STMobileEffectNative
import com.softsugar.stmobile.STMobileHumanActionNative
import com.softsugar.stmobile.model.STHumanAction
import com.softsugar.stmobile.model.STMobileAnimalResult
import com.softsugar.stmobile.params.STRotateType
import io.agora.beautyapi.sensetime.utils.LogUtils
import java.util.concurrent.Callable
import java.util.concurrent.ConcurrentLinkedQueue
import java.util.concurrent.Executors
import java.util.concurrent.Future

class FaceDetector(
    private val humanActionNative: STMobileHumanActionNative,
    private val effectNative: STMobileEffectNative
) {
    private val TAG = "FaceDetector"

    private val workerThread = Executors.newSingleThreadExecutor()
    private var accelerometer: Accelerometer? = null

    private val cacheSize = 2
    private var cacheIndex = 0
    private val cacheFutureQueue = ConcurrentLinkedQueue<Future<Int>>()
    private var isDequeBegin = false

    fun enableSensor(context: Context, enable: Boolean) {
        if (enable) {
            if (accelerometer == null) {
                accelerometer =
                    Accelerometer(
                        context
                    )
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
                    return@Callable index
                })
            )
            cacheIndex = (cacheIndex + 1) % cacheSize
        } else {
            LogUtils.e(TAG, "Detector queue is full!!")
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
                    return DetectorOut(
                        humanActionNative.getNativeHumanActionResultCache(ret)
                    )
                }catch (e: Exception){
                    LogUtils.e(TAG, "Detector dequeue timeout: $e")
                }
            }
        }
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
        val ret: Int = humanActionNative.nativeHumanActionDetectPtr(
            iN.bytes,
            iN.bytesType,
            effectNative.humanActionDetectConfig,
            orientation,
            iN.width,
            iN.height
        )

        // NV21 data is horizontal, and needs to be rotated according to the preview direction.
        // For the front camera, mirroring is also required.
        val rotatedSize = when (iN.orientation) {
            90, 270 -> Size(iN.height, iN.width)
            else -> Size(iN.width, iN.height)
        }
        var mirror = iN.isFront
        if(iN.isMirror){
            mirror = !mirror
        }
        if (iN.orientation == 0 || iN.orientation == 180) {
            if (mirror) {
                humanActionNative.nativeHumanActionMirrorPtr(rotatedSize.width)
            }
            if(iN.orientation == 180){
                humanActionNative.nativeHumanActionRotatePtr(rotatedSize.width, rotatedSize.height, STRotateType.ST_CLOCKWISE_ROTATE_180, false)
            }
        } else {
            STHumanAction.nativeHumanActionRotateAndMirror(
                humanActionNative,
                humanActionNative.nativeHumanActionResultPtr,
                rotatedSize.width,
                rotatedSize.height,
                if (mirror) Camera.CameraInfo.CAMERA_FACING_FRONT else Camera.CameraInfo.CAMERA_FACING_BACK,
                iN.orientation,
                deviceOrientation
            )
        }


        humanActionNative.updateNativeHumanActionCache(index)
    }


    /**
     * Used for the humanActionDetect interface. Calculates the orientation of the face in the buffer
     * based on the sensor direction for different device orientations.
     *
     * @return The orientation of the face in the buffer.
     */
    private fun getHumanActionOrientation(frontCamera: Boolean, cameraRotation: Int): Int {
        // Get the direction from the gravity sensor
        var orientation: Int = accelerometer?.direction ?: Accelerometer.CLOCKWISE_ANGLE.Deg90.value

        // When using the rear camera, and the sensor direction is 0 or 2,
        // the rear camera's orientation is opposite to that of the front camera
        if (!frontCamera && orientation == STRotateType.ST_CLOCKWISE_ROTATE_0) {
            orientation = STRotateType.ST_CLOCKWISE_ROTATE_180
        } else if (!frontCamera && orientation == STRotateType.ST_CLOCKWISE_ROTATE_180) {
            orientation = STRotateType.ST_CLOCKWISE_ROTATE_0
        }

        // Note that the rotation definitions differ between the front and rear cameras,
        // and different phones have different camera rotation definitions
        if (cameraRotation == 270 && orientation and STRotateType.ST_CLOCKWISE_ROTATE_90 == STRotateType.ST_CLOCKWISE_ROTATE_90
            || cameraRotation == 90 && orientation and STRotateType.ST_CLOCKWISE_ROTATE_90 == STRotateType.ST_CLOCKWISE_ROTATE_0
        ) {
            orientation = orientation xor STRotateType.ST_CLOCKWISE_ROTATE_180
        }

        return orientation
    }


    data class DetectorIn(
        val bytes: ByteArray,
        val bytesType: Int,
        val width: Int,
        val height: Int,
        val isFront: Boolean,
        val isMirror: Boolean,
        val orientation: Int,
    )

    data class DetectorOut(
        val humanResult: Long,
        val animalResult: STMobileAnimalResult? = null
    )
}