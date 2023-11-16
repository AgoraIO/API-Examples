package io.agora.api.example.utils;

import static android.renderscript.Element.RGBA_8888;
import static android.renderscript.Element.U8;
import static android.renderscript.Element.U8_4;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.renderscript.Allocation;
import android.renderscript.RenderScript;
import android.renderscript.ScriptIntrinsicBlur;
import android.renderscript.ScriptIntrinsicYuvToRGB;
import android.renderscript.Type.Builder;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * The type Yuv utils.
 */
public final class YUVUtils {

    private YUVUtils() {

    }

    /**
     * Encode i 420.
     *
     * @param i420   the 420
     * @param argb   the argb
     * @param width  the width
     * @param height the height
     */
    public static void encodeI420(byte[] i420, int[] argb, int width, int height) {
        final int frameSize = width * height;

        int yIndex = 0;                   // Y start index
        int uIndex = frameSize;           // U statt index
        int vIndex = frameSize * 5 / 4; // v start index: w*h*5/4

        int r, g, b, y, u, v;
        int index = 0;
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                r = (argb[index] & 0xff0000) >> 16;
                g = (argb[index] & 0xff00) >> 8;
                b = (argb[index] & 0xff) >> 0;

                // well known RGB to YUV algorithm
                y = ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
                u = ((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
                v = ((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;

                // I420(YUV420p) -> YYYYYYYY UU VV
                i420[yIndex++] = (byte) ((y < 0) ? 0 : ((y > 255) ? 255 : y));
                if (j % 2 == 0 && i % 2 == 0) {
                    i420[uIndex++] = (byte) ((u < 0) ? 0 : ((u > 255) ? 255 : u));
                    i420[vIndex++] = (byte) ((v < 0) ? 0 : ((v > 255) ? 255 : v));
                }
                index++;
            }
        }
    }

    /**
     * Encode nv 21.
     *
     * @param yuv420sp the yuv 420 sp
     * @param argb     the argb
     * @param width    the width
     * @param height   the height
     */
    public static void encodeNV21(byte[] yuv420sp, int[] argb, int width, int height) {
        final int frameSize = width * height;

        int yIndex = 0;
        int uvIndex = frameSize;

        int r, g, b, y, u, v;
        int index = 0;
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                r = (argb[index] & 0xff0000) >> 16;
                g = (argb[index] & 0xff00) >> 8;
                b = (argb[index] & 0xff) >> 0;

                // well known RGB to YUV algorithm
                y = ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
                u = ((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
                v = ((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;

                // NV21 has a plane of Y and interleaved planes of VU each sampled by a factor of 2
                //    meaning for every 4 Y pixels there are 1 V and 1 U.  Note the sampling is every other
                //    pixel AND every other scanline.
                yuv420sp[yIndex++] = (byte) ((y < 0) ? 0 : ((y > 255) ? 255 : y));
                if (j % 2 == 0 && index % 2 == 0) {
                    yuv420sp[uvIndex++] = (byte) ((v < 0) ? 0 : ((v > 255) ? 255 : v));
                    yuv420sp[uvIndex++] = (byte) ((u < 0) ? 0 : ((u > 255) ? 255 : u));
                }
                index++;
            }
        }
    }

    /**
     * Swap yu 12 to yuv 420 sp.
     *
     * @param yu12bytes the yu 12 bytes
     * @param i420bytes the 420 bytes
     * @param width     the width
     * @param height    the height
     * @param yStride   the y stride
     * @param uStride   the u stride
     * @param vStride   the v stride
     */
    public static void swapYU12toYUV420SP(byte[] yu12bytes, byte[] i420bytes, int width, int height, int yStride, int uStride, int vStride) {
        System.arraycopy(yu12bytes, 0, i420bytes, 0, yStride * height);
        int startPos = yStride * height;
        int yv_start_pos_u = startPos;
        int yv_start_pos_v = startPos + startPos / 4;
        for (int i = 0; i < startPos / 4; i++) {
            i420bytes[startPos + 2 * i + 0] = yu12bytes[yv_start_pos_v + i];
            i420bytes[startPos + 2 * i + 1] = yu12bytes[yv_start_pos_u + i];
        }
    }

    /**
     * 420 to bitmap bitmap.
     *
     * @param width        the width
     * @param height       the height
     * @param rotation     the rotation
     * @param bufferLength the buffer length
     * @param buffer       the buffer
     * @param yStride      the y stride
     * @param uStride      the u stride
     * @param vStride      the v stride
     * @return the bitmap
     */
    public static Bitmap i420ToBitmap(int width, int height, int rotation,
                                      int bufferLength, byte[] buffer,
                                      int yStride, int uStride, int vStride) {
        byte[] nv21 = new byte[bufferLength];
        swapYU12toYUV420SP(buffer, nv21, width, height, yStride, uStride, vStride);

        ByteArrayOutputStream baos = new ByteArrayOutputStream();

        int[] strides = {yStride, yStride};
        YuvImage image = new YuvImage(nv21, ImageFormat.NV21, width, height, strides);

        image.compressToJpeg(
                new Rect(0, 0, image.getWidth(), image.getHeight()),
                100, baos);

        // rotate picture when saving to file
        Matrix matrix = new Matrix();
        matrix.postRotate(rotation);
        byte[] bytes = baos.toByteArray();
        try {
            baos.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return BitmapFactory.decodeByteArray(bytes, 0, bytes.length);
    }

    /**
     * Blur bitmap.
     *
     * @param context the context
     * @param image   the image
     * @param radius  the radius
     * @return the bitmap
     */
    public static Bitmap blur(Context context, Bitmap image, float radius) {
        RenderScript rs = RenderScript.create(context);
        Bitmap outputBitmap = Bitmap.createBitmap(image.getWidth(), image.getHeight(), Bitmap.Config.ARGB_8888);
        Allocation in = Allocation.createFromBitmap(rs, image);
        Allocation out = Allocation.createFromBitmap(rs, outputBitmap);
        ScriptIntrinsicBlur intrinsicBlur = ScriptIntrinsicBlur.create(rs, U8_4(rs));
        intrinsicBlur.setRadius(radius);
        intrinsicBlur.setInput(in);
        intrinsicBlur.forEach(out);

        out.copyTo(outputBitmap);
        image.recycle();
        rs.destroy();

        return outputBitmap;
    }

    /**
     * Bitmap to i 420 byte [ ].
     *
     * @param inputWidth  the input width
     * @param inputHeight the input height
     * @param scaled      the scaled
     * @return the byte [ ]
     */
    public static byte[] bitmapToI420(int inputWidth, int inputHeight, Bitmap scaled) {
        int[] argb = new int[inputWidth * inputHeight];
        scaled.getPixels(argb, 0, inputWidth, 0, 0, inputWidth, inputHeight);
        byte[] yuv = new byte[inputWidth * inputHeight * 3 / 2];
        YUVUtils.encodeI420(yuv, argb, inputWidth, inputHeight);
        scaled.recycle();
        return yuv;
    }

    /**
     * To wrapped i 420 byte [ ].
     *
     * @param bufferY the buffer y
     * @param bufferU the buffer u
     * @param bufferV the buffer v
     * @param width   the width
     * @param height  the height
     * @return the byte [ ]
     */
    public static byte[] toWrappedI420(ByteBuffer bufferY,
                                       ByteBuffer bufferU,
                                       ByteBuffer bufferV,
                                       int width,
                                       int height) {
        int chromaWidth = (width + 1) / 2;
        int chromaHeight = (height + 1) / 2;
        int lengthY = width * height;
        int lengthU = chromaWidth * chromaHeight;
        int lengthV = lengthU;


        int size = lengthY + lengthU + lengthV;

        byte[] out = new byte[size];

        for (int i = 0; i < size; i++) {
            if (i < lengthY) {
                out[i] = bufferY.get(i);
            } else if (i < lengthY + lengthU) {
                int j = (i - lengthY) / chromaWidth;
                int k = (i - lengthY) % chromaWidth;
                out[i] = bufferU.get(j * width + k);
            } else {
                int j = (i - lengthY - lengthU) / chromaWidth;
                int k = (i - lengthY - lengthU) % chromaWidth;
                out[i] = bufferV.get(j * width + k);
            }
        }

        return out;
    }

    /**
     * I420转nv21
     *
     * @param data   the data
     * @param width  the width
     * @param height the height
     * @return the byte [ ]
     */
    public static byte[] i420ToNV21(byte[] data, int width, int height) {
        byte[] ret = new byte[data.length];
        int total = width * height;

        ByteBuffer bufferY = ByteBuffer.wrap(ret, 0, total);
        ByteBuffer bufferVU = ByteBuffer.wrap(ret, total, total / 2);

        bufferY.put(data, 0, total);
        for (int i = 0; i < total / 4; i += 1) {
            bufferVU.put(data[i + total + total / 4]);
            bufferVU.put(data[total + i]);
        }

        return ret;
    }

    /**
     * Nv 21 to bitmap bitmap.
     *
     * @param context the context
     * @param nv21    the nv 21
     * @param width   the width
     * @param height  the height
     * @return the bitmap
     */
    public static Bitmap nv21ToBitmap(Context context, byte[] nv21, int width, int height) {
        RenderScript rs = RenderScript.create(context);
        ScriptIntrinsicYuvToRGB yuvToRgbIntrinsic = ScriptIntrinsicYuvToRGB.create(rs, U8_4(rs));
        Builder yuvType = null;
        yuvType = (new Builder(rs, U8(rs))).setX(nv21.length);
        Allocation in = Allocation.createTyped(rs, yuvType.create(), 1);
        Builder rgbaType = (new Builder(rs, RGBA_8888(rs))).setX(width).setY(height);
        Allocation out = Allocation.createTyped(rs, rgbaType.create(), 1);
        in.copyFrom(nv21);
        yuvToRgbIntrinsic.setInput(in);
        yuvToRgbIntrinsic.forEach(out);
        Bitmap bmpout = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        out.copyTo(bmpout);
        return bmpout;
    }

}
