package io.agora.beautyapi.sensetime.utils.utils;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;

public class GLTestUtils {
    private static final String TAG = "GLUtils";

    public static Bitmap getTexture2DImage(int textureID, int width, int height) {
        try {
            int[] oldFboId = new int[1];
            GLES20.glGetIntegerv(GLES20.GL_FRAMEBUFFER_BINDING, IntBuffer.wrap(oldFboId));

            int[] framebuffers = new int[1];
            GLES20.glGenFramebuffers(1, framebuffers, 0);
            int framebufferId = framebuffers[0];
            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, framebufferId);

            int[] renderbuffers = new int[1];
            GLES20.glGenRenderbuffers(1, renderbuffers, 0);
            int renderId = renderbuffers[0];
            GLES20.glBindRenderbuffer(GLES20.GL_RENDERBUFFER, renderId);
            GLES20.glRenderbufferStorage(GLES20.GL_RENDERBUFFER, GLES20.GL_DEPTH_COMPONENT16, width, height);

            GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, textureID, 0);
            GLES20.glFramebufferRenderbuffer(GLES20.GL_FRAMEBUFFER, GLES20.GL_DEPTH_ATTACHMENT, GLES20.GL_RENDERBUFFER, renderId);
            if (GLES20.glCheckFramebufferStatus(GLES20.GL_FRAMEBUFFER) != GLES20.GL_FRAMEBUFFER_COMPLETE) {
                Log.d(TAG, "Framebuffer error");
            }

            ByteBuffer rgbaBuf = ByteBuffer.allocateDirect(width * height * 4);
            rgbaBuf.position(0);
            GLES20.glReadPixels(0, 0, width, height, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, rgbaBuf);

            Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
            bitmap.copyPixelsFromBuffer(rgbaBuf);

            GLES20.glDeleteRenderbuffers(1, IntBuffer.wrap(framebuffers));
            GLES20.glDeleteFramebuffers(1, IntBuffer.allocate(framebufferId));

            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, oldFboId[0]);

            return bitmap;
        } catch (Exception e) {
            Log.e(TAG, "", e);
        }
        return null;
    }

    public static Bitmap getTextureOESImage(int textureID, int width, int height) {
        try {
            int[] oldFboId = new int[1];
            GLES20.glGetIntegerv(GLES20.GL_FRAMEBUFFER_BINDING, IntBuffer.wrap(oldFboId));

            int[] framebuffers = new int[1];
            GLES20.glGenFramebuffers(1, framebuffers, 0);
            int framebufferId = framebuffers[0];
            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, framebufferId);

            int[] renderbuffers = new int[1];
            GLES20.glGenRenderbuffers(1, renderbuffers, 0);
            int renderId = renderbuffers[0];
            GLES20.glBindRenderbuffer(GLES20.GL_RENDERBUFFER, renderId);
            GLES20.glRenderbufferStorage(GLES20.GL_RENDERBUFFER, GLES20.GL_DEPTH_COMPONENT16, width, height);

            GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES11Ext.GL_TEXTURE_EXTERNAL_OES, textureID, 0);
            GLES20.glFramebufferRenderbuffer(GLES20.GL_FRAMEBUFFER, GLES20.GL_DEPTH_ATTACHMENT, GLES20.GL_RENDERBUFFER, renderId);
            if (GLES20.glCheckFramebufferStatus(GLES20.GL_FRAMEBUFFER) != GLES20.GL_FRAMEBUFFER_COMPLETE) {
                Log.d(TAG, "Framebuffer error");
            }

            ByteBuffer rgbaBuf = ByteBuffer.allocateDirect(width * height * 4);
            rgbaBuf.position(0);
            GLES20.glReadPixels(0, 0, width, height, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, rgbaBuf);

            Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
            bitmap.copyPixelsFromBuffer(rgbaBuf);

            GLES20.glDeleteRenderbuffers(1, IntBuffer.wrap(framebuffers));
            GLES20.glDeleteFramebuffers(1, IntBuffer.allocate(framebufferId));

            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, oldFboId[0]);

            return bitmap;
        } catch (Exception e) {
            Log.e(TAG, "", e);
        }
        return null;
    }

    public static Bitmap nv21ToBitmap(byte[] nv21, int width, int height) {
        Bitmap bitmap = null;
        try {
            YuvImage image = new YuvImage(nv21, ImageFormat.NV21, width, height, null);
            ByteArrayOutputStream stream = new ByteArrayOutputStream();
            image.compressToJpeg(new Rect(0, 0, width, height), 80, stream);
            bitmap = BitmapFactory.decodeByteArray(stream.toByteArray(), 0, stream.size());
            stream.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return bitmap;
    }

    private static Bitmap readBitmap(int width, int height){
        ByteBuffer rgbaBuf = ByteBuffer.allocateDirect(width * height * 4);
        rgbaBuf.position(0);
        GLES20.glReadPixels(0, 0, width, height, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, rgbaBuf);

        Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        bitmap.copyPixelsFromBuffer(rgbaBuf);
        return bitmap;
    }
}
