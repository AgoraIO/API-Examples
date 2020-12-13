package io.agora.rtc.ss.gles;

public class ImgTexFormat {
    public static final int COLOR_FORMAT_EXTERNAL_OES = 3;

    public final int mColorFormat;
    public final int mWidth;
    public final int mHeight;

    public ImgTexFormat(int cf, int width, int height) {
        this.mColorFormat = cf;
        this.mWidth = width;
        this.mHeight = height;
    }

    @Override
    public String toString() {
        return "ImgTexFormat{" +
                "mColorFormat=" + mColorFormat +
                ", mWidth=" + mWidth +
                ", mHeight=" + mHeight +
                '}';
    }
}
