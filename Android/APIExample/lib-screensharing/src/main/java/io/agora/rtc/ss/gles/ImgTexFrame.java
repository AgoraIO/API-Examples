package io.agora.rtc.ss.gles;

import android.opengl.Matrix;

import java.util.Arrays;

public class ImgTexFrame extends AVFrameBase {
    public static final int NO_TEXTURE = -1;
    public static final float[] DEFAULT_MATRIX = new float[16];
    public ImgTexFormat mFormat;
    public int mTextureId = NO_TEXTURE;
    public final float[] mTexMatrix;

    public ImgTexFrame(ImgTexFormat format, int textureId, float[] matrix, long ts) {
        this.mFormat = format;
        this.mTextureId = textureId;
        this.pts = ts;
        this.dts = ts;

        if (matrix != null && matrix.length == 16) {
            this.mTexMatrix = matrix;
        } else {
            this.mTexMatrix = DEFAULT_MATRIX;
            Matrix.setIdentityM(this.mTexMatrix, 0);
        }
    }

    @Override
    public String toString() {
        return "ImgTexFrame{" +
                "mFormat=" + mFormat +
                ", mTextureId=" + mTextureId +
                ", mTexMatrix=" + Arrays.toString(mTexMatrix) +
                '}';
    }
}
