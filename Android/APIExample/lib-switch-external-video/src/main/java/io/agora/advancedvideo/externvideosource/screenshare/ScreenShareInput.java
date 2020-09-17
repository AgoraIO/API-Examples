package io.agora.advancedvideo.externvideosource.screenshare;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.media.Image;
import android.media.ImageReader;
import android.media.projection.MediaProjection;
import android.media.projection.MediaProjectionManager;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.os.Looper;
import android.util.Log;
import android.util.Size;
import android.view.Surface;
import android.widget.Toast;

import androidx.annotation.RequiresApi;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

import io.agora.advancedvideo.externvideosource.GLThreadContext;
import io.agora.advancedvideo.externvideosource.IExternalVideoInput;

public class ScreenShareInput implements IExternalVideoInput {
    private static final String TAG = ScreenShareInput.class.getSimpleName();
    private static final String VIRTUAL_DISPLAY_NAME = "screen-share-display";

    private Context mContext;
    private int mSurfaceWidth;
    private int mSurfaceHeight;
    private int mScreenDpi;
    private int mFrameInterval;
    private Intent mIntent;
    private MediaProjection mMediaProjection;
    private VirtualDisplay mVirtualDisplay;
    private volatile boolean mStopped;

    public ScreenShareInput(Context context, int width, int height, int dpi, int framerate, Intent data) {
        mContext = context;
        mSurfaceWidth = width;
        mSurfaceHeight = height;
        mScreenDpi = dpi;
        mFrameInterval = 1000 / framerate;
        mIntent = data;
    }

    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    @Override
    public void onVideoInitialized(Surface target) {
        MediaProjectionManager pm = (MediaProjectionManager)
                mContext.getSystemService(Context.MEDIA_PROJECTION_SERVICE);
        mMediaProjection = pm.getMediaProjection(Activity.RESULT_OK, mIntent);

        if (mMediaProjection == null) {
            Log.e(TAG, "media projection start failed");
            return;
        }




//        mWidth = mSurfaceWidth;
//        mHeight = mSurfaceHeight;
//        mImageReader = ImageReader.newInstance(mSurfaceWidth, mSurfaceHeight, 0x01, 2);
//        mVirtualDisplay = mMediaProjection.createVirtualDisplay(
//                VIRTUAL_DISPLAY_NAME, mSurfaceWidth, mSurfaceHeight, mScreenDpi,
//                DisplayManager.VIRTUAL_DISPLAY_FLAG_PUBLIC, mImageReader.getSurface(),
//                null, null);
//        Looper.prepare();
//        mImageReader.setOnImageAvailableListener(new ImageAvailableListener(), null);
//        Looper.loop();




        mVirtualDisplay = mMediaProjection.createVirtualDisplay(
                VIRTUAL_DISPLAY_NAME, mSurfaceWidth, mSurfaceHeight, mScreenDpi,
                DisplayManager.VIRTUAL_DISPLAY_FLAG_PUBLIC, target,
                null, null);
    }


    private ImageReader mImageReader;
    private static int IMAGES_PRODUCED;
    private static final String SCREENCAP_NAME = "screencap";
    private int mWidth;
    private int mHeight;
    private class ImageAvailableListener implements ImageReader.OnImageAvailableListener {
        @Override
        public void onImageAvailable(ImageReader reader) {
            try (Image image = reader.acquireLatestImage()) {
                if (image != null) {
                    String name = String.valueOf(System.currentTimeMillis());
                    IMAGES_PRODUCED++;
                    Log.e("captured image: ", String.valueOf(IMAGES_PRODUCED));

                    if (IMAGES_PRODUCED % 10 == 0) {
                        saveJpeg(image, name);
                    }
                    image.close();
                }
            }
            catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
    private void saveJpeg(Image image, String name) {
        Image.Plane[] planes = image.getPlanes();
        ByteBuffer buffer = planes[0].getBuffer();
        int pixelStride = planes[0].getPixelStride();
        int rowStride = planes[0].getRowStride();
        int rowPadding = rowStride - pixelStride * mWidth;

        Bitmap bitmap = Bitmap.createBitmap(mWidth + rowPadding / pixelStride, mHeight, Bitmap.Config.ARGB_8888);
        bitmap.copyPixelsFromBuffer(buffer);
        //bitmap.compress(Bitmap.CompressFormat.JPEG, 100, fos);
        saveBitmap2file(bitmap, mContext.getApplicationContext(), name);
    }
    private static final String SD_PATH = Environment.getExternalStorageDirectory().getPath() + "/MediaProjection/";
    private static void saveBitmap2file(Bitmap bmp, Context context, String num) {
        String savePath;
        String fileName = num + ".JPEG";
        if (Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
            savePath = SD_PATH;
        } else {
            Toast.makeText(context, "保存失败！", Toast.LENGTH_SHORT).show();
            return;
        }
        File filePic = new File(savePath + fileName);
        try {
            if (!filePic.exists()) {
                filePic.getParentFile().mkdirs();
                filePic.createNewFile();
            }
            FileOutputStream fos = new FileOutputStream(filePic);
            bmp.compress(Bitmap.CompressFormat.JPEG, 100, fos);
            fos.flush();
            fos.close();
            Toast.makeText(context, "保存成功,位置:" + filePic.getAbsolutePath(), Toast.LENGTH_SHORT).show();
        }
        catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        // 其次把文件插入到系统图库
//        try {
//            MediaStore.Images.Media.insertImage(context.getContentResolver(), filePic.getAbsolutePath(), fileName, null);
//        } catch (FileNotFoundException e) {
//            e.printStackTrace();
//        }
        // 最后通知图库更新
        context.sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.parse("file://" + savePath + fileName)));

    }





    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    @Override
    public void onVideoStopped(GLThreadContext context) {
        mStopped = true;

        if (mVirtualDisplay != null) {
            mVirtualDisplay.release();
        }

        if (mMediaProjection != null) {
            mMediaProjection.stop();
        }
    }

    @Override
    public boolean isRunning() {
        return !mStopped;
    }

    @Override
    public void onFrameAvailable(GLThreadContext context, int textureId, float[] transform) {
        // Screen sharing do not process or show local preview
    }

    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    @Override
    public Size onGetFrameSize() {
        return new Size(mSurfaceWidth, mSurfaceHeight);
    }

    @Override
    public int timeToWait() {
        return mFrameInterval;
    }
}
