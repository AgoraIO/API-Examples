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
import android.renderscript.Type.Builder;
import android.renderscript.RenderScript;
import android.renderscript.ScriptIntrinsicBlur;
import android.renderscript.ScriptIntrinsicYuvToRGB;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

public class YUVUtils {

    public static void encodeI420(byte[] i420, int[] argb, int width, int height) {
        final int frameSize = width * height;

        int yIndex = 0;                   // Y start index
        int uIndex = frameSize;           // U statt index
        int vIndex = frameSize * 5 / 4; // V start index: w*h*5/4

        int a, R, G, B, Y, U, V;
        int index = 0;
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                a = (argb[index] & 0xff000000) >> 24; //  is not used obviously
                R = (argb[index] & 0xff0000) >> 16;
                G = (argb[index] & 0xff00) >> 8;
                B = (argb[index] & 0xff) >> 0;

                // well known RGB to YUV algorithm
                Y = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16;
                U = ((-38 * R - 74 * G + 112 * B + 128) >> 8) + 128;
                V = ((112 * R - 94 * G - 18 * B + 128) >> 8) + 128;

                // I420(YUV420p) -> YYYYYYYY UU VV
                i420[yIndex++] = (byte) ((Y < 0) ? 0 : ((Y > 255) ? 255 : Y));
                if (j % 2 == 0 && i % 2 == 0) {
                    i420[uIndex++] = (byte) ((U < 0) ? 0 : ((U > 255) ? 255 : U));
                    i420[vIndex++] = (byte) ((V < 0) ? 0 : ((V > 255) ? 255 : V));
                }
                index++;
            }
        }
    }

    public static void encodeNV21(byte[] yuv420sp, int[] argb, int width, int height) {
        final int frameSize = width * height;

        int yIndex = 0;
        int uvIndex = frameSize;

        int a, R, G, B, Y, U, V;
        int index = 0;
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                a = (argb[index] & 0xff000000) >> 24; // a is not used obviously
                R = (argb[index] & 0xff0000) >> 16;
                G = (argb[index] & 0xff00) >> 8;
                B = (argb[index] & 0xff) >> 0;

                // well known RGB to YUV algorithm
                Y = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16;
                U = ((-38 * R - 74 * G + 112 * B + 128) >> 8) + 128;
                V = ((112 * R - 94 * G - 18 * B + 128) >> 8) + 128;

                // NV21 has a plane of Y and interleaved planes of VU each sampled by a factor of 2
                //    meaning for every 4 Y pixels there are 1 V and 1 U.  Note the sampling is every other
                //    pixel AND every other scanline.
                yuv420sp[yIndex++] = (byte) ((Y < 0) ? 0 : ((Y > 255) ? 255 : Y));
                if (j % 2 == 0 && index % 2 == 0) {
                    yuv420sp[uvIndex++] = (byte) ((V < 0) ? 0 : ((V > 255) ? 255 : V));
                    yuv420sp[uvIndex++] = (byte) ((U < 0) ? 0 : ((U > 255) ? 255 : U));
                }
                index++;
            }
        }
    }

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

    public static Bitmap i420ToBitmap(int width, int height, int rotation, int bufferLength, byte[] buffer, int yStride, int uStride, int vStride) {
        byte[] NV21 = new byte[bufferLength];
        swapYU12toYUV420SP(buffer, NV21, width, height, yStride, uStride, vStride);

        ByteArrayOutputStream baos = new ByteArrayOutputStream();

        int[] strides = {yStride, yStride};
        YuvImage image = new YuvImage(NV21, ImageFormat.NV21, width, height, strides);

        image.compressToJpeg(
                new Rect(0, 0, image.getWidth(), image.getHeight()),
                100, baos);

        // rotate picture when saving to file
        Matrix matrix = new Matrix();
        matrix.postRotate(rotation);
        byte[] bytes = baos.toByteArray();
        try {
            baos.close();
        }
        catch (IOException e) {
            e.printStackTrace();
        }
        return BitmapFactory.decodeByteArray(bytes, 0, bytes.length);
    }

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

    public static byte[] bitmapToI420(int inputWidth, int inputHeight, Bitmap scaled) {
        int[] argb = new int[inputWidth * inputHeight];
        scaled.getPixels(argb, 0, inputWidth, 0, 0, inputWidth, inputHeight);
        byte[] yuv = new byte[inputWidth * inputHeight * 3 / 2];
        YUVUtils.encodeI420(yuv, argb, inputWidth, inputHeight);
        scaled.recycle();
        return yuv;
    }

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
     */
    public static byte[] I420ToNV21(byte[] data, int width, int height) {
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

    public static Bitmap NV21ToBitmap(Context context, byte[] nv21, int width, int height) {
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
