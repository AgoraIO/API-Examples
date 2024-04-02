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
import io.agora.beautyapi.faceunity.utils.LogUtils
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
    private val glTextureBufferQueueIn = GLTextureBufferQueue(cacheCount = cacheCount, loggable = true)
    private val glTextureBufferQueueOut = GLTextureBufferQueue(cacheCount = cacheCount, loggable = false)
    private val glFrameBuffer = GLFrameBuffer()
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
        isFrontCamera: Boolean,
        isMirror: Boolean
    ): Int {
        if (isReleased) {
            return -1
        }
        val currGLContext = GLUtils.getCurrGLContext() ?: return -1

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
                false,
                isFrontCamera,
                isMirror,
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

            val frame = glTextureBufferQueueIn.dequeue(false) ?: return@Callable -2
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
                        false,
                        true,
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
                        false,
                        true,
                        null,
                        frame.tag
                    )
                )
            }
            glTextureBufferQueueIn.dequeue(true)
            return@Callable 0
        }))

        var ret = 0
        if (isBegin || futureQueue.size >= cacheCount) {
            isBegin = true
            try {
                val get =  futureQueue.poll()?.get() ?: -1
                if (get == 0) {
                    val dequeue = glTextureBufferQueueOut.dequeue() ?: return -1
                    glFrameBuffer.setSize(dequeue.width, dequeue.height)
                    ret = glFrameBuffer.process(dequeue.textureId, dequeue.textureType)
                }
            }catch (e: Exception){
                LogUtils.e(TAG, "process end with exception: $e")
            }
        }

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
        glTextureBufferQueueIn.reset()
        glFrameBuffer.release()
        executeSync {
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
        glTextureBufferQueueIn.release()
        glFrameBuffer.release()
        executeSync {
            glTextureBufferQueueOut.release()
            if (eglContextBase != null) {
                eglContextHelper.release()
                eglContextBase = null
            }
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