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
import android.util.Size
import java.util.concurrent.ConcurrentLinkedQueue

class GLTextureBufferQueue(
    private val glFrameBuffer: GLFrameBuffer = GLFrameBuffer(),
    private val cacheCount: Int = 6,
    private val loggable: Boolean = false
) {
    private val TAG = "GLTextureBufferQueue"

    private var cacheIndex = 0
    private val cacheTextureOuts = arrayOfNulls<TextureOut>(cacheCount)
    private val textureIdQueue = ConcurrentLinkedQueue<TextureOut>()


    fun enqueue(iN: TextureIn): Int {
        var size = textureIdQueue.size
        if (size < cacheCount) {
            var out = cacheTextureOuts[cacheIndex]
            val outSize = when (iN.rotation) {
                90, 270 -> Size(iN.height, iN.width)
                else -> Size(iN.width, iN.height)
            }

            if (out == null) {
                val textureId = glFrameBuffer.createTexture(outSize.width, outSize.height)
                out = TextureOut(
                    0,
                    textureId,
                    GLES20.GL_TEXTURE_2D,
                    outSize.width,
                    outSize.height,
                    iN.isFrontCamera,
                    iN.isMirror,
                )
                cacheTextureOuts[cacheIndex] = out
            } else if (out.width != outSize.width || out.height != outSize.height) {
                glFrameBuffer.resizeTexture(out.textureId, outSize.width, outSize.height)
                out = TextureOut(
                    0,
                    out.textureId,
                    out.textureType,
                    outSize.width,
                    outSize.height,
                    iN.isFrontCamera,
                    iN.isMirror,
                )
                cacheTextureOuts[cacheIndex] = out
            } else if(out.isFrontCamera != iN.isFrontCamera){
                out = TextureOut(
                    0,
                    out.textureId,
                    out.textureType,
                    out.width,
                    out.height,
                    iN.isFrontCamera,
                    iN.isMirror,
                )
                cacheTextureOuts[cacheIndex] = out
            }

            glFrameBuffer.textureId = out.textureId
            glFrameBuffer.setSize(out.width, out.height)
            glFrameBuffer.resetTransform()
            glFrameBuffer.setRotation(iN.rotation)
            if (iN.transform != null) {
                glFrameBuffer.setTexMatrix(iN.transform)
                var flipH = iN.isFrontCamera
                if(iN.isMirror){
                    flipH = !flipH
                }
                glFrameBuffer.setFlipH(flipH)
            } else {
                var flipH = !iN.isFrontCamera
                if(iN.isMirror){
                    flipH = !flipH
                }
                glFrameBuffer.setFlipH(flipH)
            }
            glFrameBuffer.setFlipV(iN.flipV)
            glFrameBuffer.process(iN.textureId, iN.textureType)
            out.index = cacheIndex
            out.tag = iN.tag
            textureIdQueue.offer(out)
            if(loggable){
                Log.d(TAG, "TextureIdQueue enqueue index=$cacheIndex, size=$size")
            }
            cacheIndex = (cacheIndex + 1) % cacheCount
            size++

        } else {
            if(loggable){
                Log.e(TAG, "TextureIdQueue is full!!")
            }
        }

        return size
    }

    fun dequeue(remove: Boolean = true): TextureOut? {
        val size = textureIdQueue.size
        val poll = if(remove){
            textureIdQueue.poll()
        }else{
            textureIdQueue.peek()
        }
        if(loggable){
            Log.d(TAG, "TextureIdQueue dequeue index=${poll?.index}, size=$size")
        }
        return poll
    }

    fun reset() {
        cacheIndex = 0
        textureIdQueue.clear()
    }

    fun release() {
        cacheIndex = 0
        cacheTextureOuts.forEachIndexed { index, textureOut ->
            if (textureOut != null) {
                GLES20.glDeleteTextures(1, intArrayOf(textureOut.textureId), 0)
                cacheTextureOuts[index] = null
            }
        }
        textureIdQueue.clear()
        glFrameBuffer.release()
    }

    data class TextureIn(
        val textureId: Int,
        val textureType: Int,
        val width: Int,
        val height: Int,
        val rotation: Int,
        val flipV: Boolean,
        val isFrontCamera: Boolean,
        val isMirror: Boolean,
        val transform: FloatArray?,
        val tag: Any? = null
    )

    data class TextureOut(
        var index: Int = 0,
        val textureId: Int,
        val textureType: Int,
        val width: Int,
        val height: Int,
        val isFrontCamera: Boolean,
        var tag: Any? = null
    )
}