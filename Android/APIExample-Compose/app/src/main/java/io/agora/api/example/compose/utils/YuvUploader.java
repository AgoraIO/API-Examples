package io.agora.api.example.compose.utils;

import android.opengl.GLES20;

import androidx.annotation.Nullable;

import java.nio.ByteBuffer;

import io.agora.base.VideoFrame;
import io.agora.base.internal.video.GlUtil;
import io.agora.base.internal.video.YuvHelper;

/**
 * The type Yuv uploader.
 */
public class YuvUploader {
    // Intermediate copy buffer for uploading yuv frames that are not packed, i.e. stride > width.
    // TODO(magjed): Investigate when GL_UNPACK_ROW_LENGTH is available, or make a custom shader
    // that handles stride and compare performance with intermediate copy.
    @Nullable private ByteBuffer copyBuffer;
    @Nullable private int[] yuvTextures;

    /**
     * Upload |planes| into OpenGL textures, taking stride into consideration.
     *
     * @param width   the width
     * @param height  the height
     * @param strides the strides
     * @param planes  the planes
     * @return Array of three texture indices corresponding to Y-, U-, and V-plane respectively.
     */
    @Nullable
    public int[] uploadYuvData(int width, int height, int[] strides, ByteBuffer[] planes) {
        final int[] planeWidths = new int[] {width, width / 2, width / 2};
        final int[] planeHeights = new int[] {height, height / 2, height / 2};
        // Make a first pass to see if we need a temporary copy buffer.
        int copyCapacityNeeded = 0;
        for (int i = 0; i < 3; ++i) {
            if (strides[i] > planeWidths[i]) {
                copyCapacityNeeded = Math.max(copyCapacityNeeded, planeWidths[i] * planeHeights[i]);
            }
        }
        // Allocate copy buffer if necessary.
        if (copyCapacityNeeded > 0
                && (copyBuffer == null || copyBuffer.capacity() < copyCapacityNeeded)) {
            copyBuffer = ByteBuffer.allocateDirect(copyCapacityNeeded);
        }
        // Make sure YUV textures are allocated.
        if (yuvTextures == null) {
            yuvTextures = new int[3];
            for (int i = 0; i < 3; i++) {
                yuvTextures[i] = GlUtil.generateTexture(GLES20.GL_TEXTURE_2D);
            }
        }
        // Upload each plane.
        for (int i = 0; i < 3; ++i) {
            GLES20.glActiveTexture(GLES20.GL_TEXTURE0 + i);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, yuvTextures[i]);
            // GLES only accepts packed data, i.e. stride == planeWidth.
            final ByteBuffer packedByteBuffer;
            if (strides[i] == planeWidths[i]) {
                // Input is packed already.
                packedByteBuffer = planes[i];
            } else {
                YuvHelper.copyPlane(
                        planes[i], strides[i], copyBuffer, planeWidths[i], planeWidths[i], planeHeights[i]);
                packedByteBuffer = copyBuffer;
            }
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, planeWidths[i],
                    planeHeights[i], 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, packedByteBuffer);
        }
        return yuvTextures;
    }

    /**
     * Upload from buffer int [ ].
     *
     * @param buffer the buffer
     * @return the int [ ]
     */
    @Nullable
    public int[] uploadFromBuffer(VideoFrame.I420Buffer buffer) {
        int[] strides = {buffer.getStrideY(), buffer.getStrideU(), buffer.getStrideV()};
        ByteBuffer[] planes = {buffer.getDataY(), buffer.getDataU(), buffer.getDataV()};
        return uploadYuvData(buffer.getWidth(), buffer.getHeight(), strides, planes);
    }

    /**
     * Get yuv textures int [ ].
     *
     * @return the int [ ]
     */
    @Nullable
    public int[] getYuvTextures() {
        return yuvTextures;
    }

    /**
     * Releases cached resources. Uploader can still be used and the resources will be reallocated
     * on first use.
     */
    public void release() {
        copyBuffer = null;
        if (yuvTextures != null) {
            GLES20.glDeleteTextures(3, yuvTextures, 0);
            yuvTextures = null;
        }
    }
}
