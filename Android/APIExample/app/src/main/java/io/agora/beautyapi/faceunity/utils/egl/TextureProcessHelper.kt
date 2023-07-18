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

package io.agora.beautyapi.faceunity.utils.egl

import android.opengl.GLES20
import android.util.Log
import java.util.concurrent.Callable
import java.util.concurrent.ConcurrentLinkedQueue
import java.util.concurrent.CountDownLatch
import java.util.concurrent.Executors
import java.util.concurrent.Future
import javax.microedition.khronos.egl.EGLContext

class TextureProcessHelper(
    private val cacheCount: Int = 2
) {
    private val TAG = "TextureProcessHelper"
    private val glTextureBufferQueueIn = GLTextureBufferQueue(cacheCount = cacheCount)
    private val glTextureBufferQueueOut = GLTextureBufferQueue(cacheCount = cacheCount)
    private val futureQueue = ConcurrentLinkedQueue<Future<Int>>()
    private val workerThread = Executors.newSingleThreadExecutor()
    private val eglContextHelper =
        EGLContextHelper()
    private var eglContextBase: EGLContext? = null
    private var isReleased = false
    private var filter: ((GLTextureBufferQueue.TextureOut) -> Int)? = null
    private var isBegin = false
    private var frameIndex = 0

    fun setFilter(filter: (GLTextureBufferQueue.TextureOut) -> Int) {
        this.filter = filter
    }

    fun process(
        texId: Int, texType: Int,
        width: Int, height: Int, rotation: Int,
        transform: FloatArray,
        isFrontCamera: Boolean
    ): Int {
        if (isReleased) {
            return -1
        }
        val currGLContext = GLUtils.getCurrGLContext() ?: return -1

        val startTime = System.currentTimeMillis()
        Log.d(TAG, "process start...")
        if (eglContextBase == null) {
            eglContextBase = currGLContext
            executeSync {
                eglContextHelper.initEGL(eglContextBase)
                eglContextHelper.eglMakeCurrent()
            }
        } else if (eglContextBase != currGLContext) {
            eglContextBase = currGLContext
            executeSync {
                eglContextHelper.release()
                eglContextHelper.initEGL(eglContextBase)
                eglContextHelper.eglMakeCurrent()
            }
        }

        glTextureBufferQueueIn.enqueue(
            GLTextureBufferQueue.TextureIn(
                texId,
                texType,
                width,
                height,
                rotation,
                isFrontCamera,
                transform,
                frameIndex
            )
        )
        frameIndex ++

        if (isReleased) {
            return -1
        }

        futureQueue.offer(workerThread.submit(Callable {
            if (isReleased) {
                return@Callable -2
            }
            val filterStartTime = System.currentTimeMillis()

            val frame = glTextureBufferQueueIn.dequeue() ?: return@Callable -2
            Log.d(TAG, "process filter start ... index=${frame.tag}")
            val filterTexId =  filter?.invoke(frame) ?: -1
            if (filterTexId >= 0) {
                glTextureBufferQueueOut.enqueue(
                    GLTextureBufferQueue.TextureIn(
                        filterTexId,
                        GLES20.GL_TEXTURE_2D,
                        frame.width,
                        frame.height,
                        0,
                        false,
                        null,
                        frame.tag
                    )
                )
            } else {
                glTextureBufferQueueOut.enqueue(
                    GLTextureBufferQueue.TextureIn(
                        frame.textureId,
                        frame.textureType,
                        frame.width,
                        frame.height,
                        0,
                        false,
                        null,
                        frame.tag
                    )
                )
            }
            Log.d(TAG, "process filter end cost=${System.currentTimeMillis() - filterStartTime}, index=${frame.tag}")

            return@Callable 0
        }))

        var ret = 0
        var tag = frameIndex
        if (isBegin || futureQueue.size >= cacheCount) {
            isBegin = true
            Log.d(TAG, "process get cost=${System.currentTimeMillis() - startTime}")
            try {
                val get =  futureQueue.poll()?.get() ?: -1
                if (get == 0) {
                    val dequeue = glTextureBufferQueueOut.dequeue()
                    ret = dequeue?.textureId ?: -1
                    tag = (dequeue?.tag as? Int) ?: -1
                }
            }catch (e: Exception){
                Log.d(TAG, "process end with exception: $e")
            }
        }

        Log.d(TAG, "process end with $ret. cost=${System.currentTimeMillis() - startTime}, index=$tag")
        return ret
    }

    fun reset(){
        if(frameIndex == 0){
            return
        }
        isBegin = false
        frameIndex = 0
        var future = futureQueue.poll()
        while (future != null) {
            future.cancel(true)
            future = futureQueue.poll()
        }
        executeSync {
            glTextureBufferQueueIn.reset()
            glTextureBufferQueueOut.reset()
        }
    }

    fun size() = futureQueue.size

    fun release() {
        isReleased = true
        filter = null
        isBegin = false
        frameIndex = 0
        var future = futureQueue.poll()
        while (future != null) {
            future.cancel(true)
            future = futureQueue.poll()
        }
        executeSync {
            if (eglContextBase != null) {
                eglContextHelper.release()
                eglContextBase = null
            }
            glTextureBufferQueueIn.release()
            glTextureBufferQueueOut.release()
        }
        workerThread.shutdown()
    }

    fun executeSync(run: () -> Unit) {
        val latch = CountDownLatch(1)
        workerThread.execute {
            run.invoke()
            latch.countDown()
        }
        latch.await()
    }
}