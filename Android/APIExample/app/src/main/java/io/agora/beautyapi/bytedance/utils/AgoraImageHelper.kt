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

package io.agora.beautyapi.bytedance.utils

import android.opengl.GLES20
import io.agora.base.VideoFrame
import io.agora.base.internal.video.GlRectDrawer
import io.agora.base.internal.video.GlTextureFrameBuffer
import io.agora.base.internal.video.RendererCommon.GlDrawer

class AgoraImageHelper {
    private var glFrameBuffer: GlTextureFrameBuffer? = null
    private var drawer : GlDrawer? = null

    fun transformTexture(
        texId: Int,
        texType: VideoFrame.TextureBuffer.Type,
        width: Int,
        height: Int,
        transform: FloatArray
    ): Int {

        if (glFrameBuffer == null) {
            glFrameBuffer = GlTextureFrameBuffer(GLES20.GL_RGBA)
        }
        val frameBuffer = glFrameBuffer ?: return -1

        if(drawer == null){
            drawer = GlRectDrawer()
        }
        val drawer = this.drawer ?: return -1

        frameBuffer.setSize(width, height)

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, frameBuffer.frameBufferId)
        if(texType == VideoFrame.TextureBuffer.Type.OES){
            drawer.drawOes(texId, transform, width, height, 0, 0, width, height)
        }else{
            drawer.drawRgb(texId, transform, width, height, 0, 0, width, height)
        }
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0)
        GLES20.glFinish()

        return frameBuffer.textureId
    }

    fun release() {
        glFrameBuffer?.release()
        glFrameBuffer = null
        drawer?.release()
        drawer = null
    }

}