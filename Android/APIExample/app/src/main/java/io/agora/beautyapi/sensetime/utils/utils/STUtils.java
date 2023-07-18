package io.agora.beautyapi.sensetime.utils.utils;

import static com.sensetime.stmobile.params.STEffectBeautyType.EFFECT_BEAUTY_HAIR_DYE;
import static com.sensetime.stmobile.params.STEffectBeautyType.EFFECT_BEAUTY_MAKEUP_ALL;
import static com.sensetime.stmobile.params.STEffectBeautyType.EFFECT_BEAUTY_MAKEUP_CHEEK;
import static com.sensetime.stmobile.params.STEffectBeautyType.EFFECT_BEAUTY_MAKEUP_EYE_BALL;
import static com.sensetime.stmobile.params.STEffectBeautyType.EFFECT_BEAUTY_MAKEUP_EYE_BROW;
import static com.sensetime.stmobile.params.STEffectBeautyType.EFFECT_BEAUTY_MAKEUP_EYE_LASH;
import static com.sensetime.stmobile.params.STEffectBeautyType.EFFECT_BEAUTY_MAKEUP_EYE_LINE;
import static com.sensetime.stmobile.params.STEffectBeautyType.EFFECT_BEAUTY_MAKEUP_EYE_SHADOW;
import static com.sensetime.stmobile.params.STEffectBeautyType.EFFECT_BEAUTY_MAKEUP_LIP;
import static com.sensetime.stmobile.params.STEffectBeautyType.EFFECT_BEAUTY_MAKEUP_NOSE;

import android.content.Context;
import android.content.res.AssetManager;
import android.database.Cursor;
import android.database.sqlite.SQLiteException;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PointF;
import android.graphics.Rect;
import android.hardware.Camera;
import android.media.ExifInterface;
import android.net.Uri;
import android.provider.MediaStore;
import android.util.Log;

import androidx.annotation.ColorInt;

import com.sensetime.stmobile.model.STAnimalFace;
import com.sensetime.stmobile.model.STEffectBeautyInfo;
import com.sensetime.stmobile.model.STHumanAction;
import com.sensetime.stmobile.model.STPoint;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Arrays;

/**
 * @Description
 * @Author Lu Guoqiang
 * @Time 6/7/21 7:45 PM
 */
public class STUtils {
    private String TAG = "STUtils";
    private Context mContext;

    // track config
    private static final int ST_MOBILE_TRACKING_MULTI_THREAD = 0x00000000;
    private static final int ST_MOBILE_TRACKING_ENABLE_DEBOUNCE = 0x00000010;
    private static final int ST_MOBILE_TRACKING_ENABLE_FACE_ACTION = 0x00000020;
    private static final int ST_MOBILE_TRACKING_DEFAULT_CONFIG = ST_MOBILE_TRACKING_MULTI_THREAD;

    public STUtils(Context context) {
        this.mContext = context;
    }

    public static int[] getBGRAImageByte(Bitmap image) {
        int width = image.getWidth();
        int height = image.getHeight();

        if (image.getConfig().equals(Bitmap.Config.ARGB_8888)) {
            int[] imgData = new int[width * height];
            image.getPixels(imgData, 0, width, 0, 0, width, height);
            return imgData;
        } else {
            // TODO
        }

        return null;
    }

    public static String findLibrary(Context context, String libName) {
        String result = null;
        ClassLoader classLoader = (context.getClassLoader());
        if (classLoader != null) {
            try {
                Method findLibraryMethod = classLoader.getClass().getMethod("findLibrary", new Class<?>[]{String.class});
                if (findLibraryMethod != null) {
                    Object objPath = findLibraryMethod.invoke(classLoader, new Object[]{libName});
                    if (objPath != null && objPath instanceof String) {
                        result = (String) objPath;
                    }
                }
            } catch (NoSuchMethodException e) {
                Log.e("findLibrary1", e.toString());
            } catch (IllegalAccessException e) {
                Log.e("findLibrary1", e.toString());
            } catch (IllegalArgumentException e) {
                Log.e("findLibrary1", e.toString());
            } catch (InvocationTargetException e) {
                Log.e("findLibrary1", e.toString());
            } catch (Exception e) {
                Log.e("findLibrary1", e.toString());
            }
        }

        return result;
    }

    /**
     * Draw points in the screen
     */
    static public void drawFaceKeyPoints(Canvas canvas, Paint paint, PointF[] points, float[] visibles, int width, int height, int color) {

        if (canvas == null) return;

        int strokeWidth = 3;
        if (canvas.getWidth() != 1080) {
            strokeWidth = (int) (strokeWidth * canvas.getWidth() / 1080);
        }

        //眼球中心点
        if (points.length < 3) {
            strokeWidth = (int) (strokeWidth * 1.5);
        }

        points = adjustToScreenPointsMin(points, canvas.getWidth(), canvas.getHeight(), width, height);

        for (int i = 0; i < points.length; i++) {
            PointF p = points[i];

            if (visibles != null && visibles[i] < 0.5) {
                paint.setColor(Color.rgb(255, 20, 20));
            } else {
                paint.setColor(color);
            }
            paint.setStyle(Paint.Style.FILL);
            canvas.drawCircle(p.x, p.y, strokeWidth, paint);
        }
        paint.setColor(color);
    }

    public static PointF[] adjustToScreenPointsMin(PointF[] points, int screenWidth, int screenHeight, int imageWidth, int imageHeight) {
        PointF[] intputPoints = points;

        if (points == null || points.length == 0) {
            return null;
        }

        if ((float) screenHeight / screenWidth >= (float) imageHeight / imageWidth) {
            float rate = (float) screenHeight / imageHeight;

            for (int i = 0; i < intputPoints.length; i++) {
                float x = (float) screenWidth / 2 - ((float) imageWidth / 2 - intputPoints[i].x) * rate;
                float y = intputPoints[i].y * rate;

                intputPoints[i].set(x, y);
            }
        } else {
            float rate = (float) screenWidth / imageWidth;

            for (int i = 0; i < intputPoints.length; i++) {
                float x = intputPoints[i].x * rate;
                float y = (float) screenHeight / 2 - ((float) imageHeight / 2 - intputPoints[i].y) * rate;

                intputPoints[i].set(x, y);
            }
        }

        return intputPoints;
    }

    public static byte[] getBGRFromBitmap(Bitmap bitmap) {
        int width = bitmap.getWidth();
        int height = bitmap.getHeight();
        int componentsPerPixel = 3;
        int totalPixels = width * height;
        int totalBytes = totalPixels * componentsPerPixel;

        byte[] rgbValues = new byte[totalBytes];
        @ColorInt int[] argbPixels = new int[totalPixels];
        bitmap.getPixels(argbPixels, 0, width, 0, 0, width, height);
        for (int i = 0; i < totalPixels; i++) {
            @ColorInt int argbPixel = argbPixels[i];
            int red = Color.red(argbPixel);
            int green = Color.green(argbPixel);
            int blue = Color.blue(argbPixel);
            rgbValues[i * componentsPerPixel + 0] = (byte) blue;
            rgbValues[i * componentsPerPixel + 1] = (byte) green;
            rgbValues[i * componentsPerPixel + 2] = (byte) red;
        }

        return rgbValues;
    }

    public static byte[] getRGBFromBitmap(Bitmap bitmap) {
        int width = bitmap.getWidth();
        int height = bitmap.getHeight();
        int componentsPerPixel = 3;
        int totalPixels = width * height;
        int totalBytes = totalPixels * componentsPerPixel;

        byte[] rgbValues = new byte[totalBytes];
        @ColorInt int[] argbPixels = new int[totalPixels];
        bitmap.getPixels(argbPixels, 0, width, 0, 0, width, height);
        for (int i = 0; i < totalPixels; i++) {
            @ColorInt int argbPixel = argbPixels[i];
            int red = Color.red(argbPixel);
            int green = Color.green(argbPixel);
            int blue = Color.blue(argbPixel);
            int alpha = Color.alpha(argbPixel);
            rgbValues[i * componentsPerPixel + 0] = (byte) red;
            rgbValues[i * componentsPerPixel + 1] = (byte) green;
            rgbValues[i * componentsPerPixel + 2] = (byte) blue;
        }

        return rgbValues;
    }

    public static byte[] getRGBAFromBitmap2(Bitmap bitmap) {
        int width = bitmap.getWidth();
        int height = bitmap.getHeight();
        int componentsPerPixel = 4;
        int totalPixels = width * height;
        int totalBytes = totalPixels * componentsPerPixel;

        byte[] rgbValues = new byte[totalBytes];
        @ColorInt int[] argbPixels = new int[totalPixels];
        bitmap.getPixels(argbPixels, 0, width, 0, 0, width, height);
        for (int i = 0; i < totalPixels; i++) {
            @ColorInt int argbPixel = argbPixels[i];
            int red = Color.red(argbPixel);
            int green = Color.green(argbPixel);
            int blue = Color.blue(argbPixel);
            int alpha = Color.alpha(argbPixel);
            rgbValues[i * componentsPerPixel + 0] = (byte) red;
            rgbValues[i * componentsPerPixel + 1] = (byte) green;
            rgbValues[i * componentsPerPixel + 2] = (byte) blue;
            rgbValues[i * componentsPerPixel + 3] = (byte) alpha;
        }

        return rgbValues;
    }

    public static Bitmap getBitmapFromBGR(byte[] data, int width, int height) {
        int[] argb = new int[width * height];
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                argb[i * width + j] = data[(i * width + j) * 3 + 0] & 0xFF  //b
                        | (data[(i * width + j) * 3 + 1] & 0xFF) << 8       //g
                        | (data[(i * width + j) * 3 + 2] & 0xFF) << 16      //r
                        | (0xFF) << 24;                                   //a
            }
        }

        return Bitmap.createBitmap(argb, width, height, Bitmap.Config.ARGB_8888);
    }

    public static Bitmap getBitmapFromRGBA(byte[] data, int width, int height) {
        int[] argb = new int[width * height];
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                argb[i * width + j] = 255;
                argb[i * width + j] = (argb[i * width + j] << 8) + data[(i * width + j) * 4 + 0]; //+r
                argb[i * width + j] = (argb[i * width + j] << 8) + data[(i * width + j) * 4 + 1]; //+g
                argb[i * width + j] = (argb[i * width + j] << 8) + data[(i * width + j) * 4 + 2]; //+b
            }
        }

        return Bitmap.createBitmap(argb, width, height, Bitmap.Config.ARGB_8888);
    }

    public static byte[] Bitmap2Bytes(Bitmap bm) {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        bm.compress(Bitmap.CompressFormat.PNG, 100, baos);
        return baos.toByteArray();
    }

    public static Bitmap getBitmapFromFile(Uri uri) {
        if (uri == null) {
            return null;
        }

        Bitmap bmp = null;
        BitmapFactory.Options opts = new BitmapFactory.Options();
        opts.inJustDecodeBounds = true;
        bmp = BitmapFactory.decodeFile(uri.getPath(), opts);
        opts.inSampleSize = computeSampleSize(opts);
        opts.inJustDecodeBounds = false;
        bmp = BitmapFactory.decodeFile(uri.getPath(), opts);

        return bmp;
    }

    public static Bitmap getBitmapFromFile2(String pathName) {
        if (pathName == null) {
            return null;
        }

        Bitmap bmp = null;
        BitmapFactory.Options opts = new BitmapFactory.Options();
        opts.inJustDecodeBounds = true;
        bmp = BitmapFactory.decodeFile(pathName, opts);
        opts.inSampleSize = computeSampleSize(opts);
        opts.inJustDecodeBounds = false;
        bmp = BitmapFactory.decodeFile(pathName, opts);

        return bmp;
    }

    public static Bitmap getBitmapAfterRotate(Uri uri, Context context) {
        Bitmap rotatebitmap = null;
        Bitmap srcbitmap = null;
        String[] filePathColumn = {MediaStore.Images.Media.DATA, MediaStore.Images.Media.ORIENTATION};
        Cursor cursor = null;
        String picturePath = null;
        String orientation = null;

        try {
            cursor = context.getContentResolver().query(uri, filePathColumn, null, null, null);

            if (cursor != null) {
                cursor.moveToFirst();
                int columnIndex = cursor.getColumnIndex(filePathColumn[0]);
                picturePath = cursor.getString(columnIndex);
                orientation = cursor.getString(cursor.getColumnIndex(filePathColumn[1]));
            }
        } catch (SQLiteException e) {
            // Do nothing
        } catch (IllegalArgumentException e) {
            // Do nothing
        } catch (IllegalStateException e) {
            // Do nothing
        } finally {
            if (cursor != null)
                cursor.close();
        }
        if (picturePath != null) {
            int angle = 0;
            if (orientation != null && !"".equals(orientation)) {
                angle = Integer.parseInt(orientation);
            }

            BitmapFactory.Options opts = new BitmapFactory.Options();
            opts.inJustDecodeBounds = true;
            srcbitmap = BitmapFactory.decodeFile(picturePath, opts);

            opts.inSampleSize = computeSampleSize(opts);
            opts.inJustDecodeBounds = false;
            srcbitmap = BitmapFactory.decodeFile(picturePath, opts);
            if (angle != 0) {
                // 下面的方法主要作用是把图片转一个角度，也可以放大缩小等
                Matrix m = new Matrix();
                int width = srcbitmap.getWidth();
                int height = srcbitmap.getHeight();
                m.setRotate(angle); // 旋转angle度
                try {
                    rotatebitmap = Bitmap.createBitmap(srcbitmap, 0, 0, width, height, m, true);// 新生成图片
                } catch (Exception e) {

                }

                if (rotatebitmap == null) {
                    rotatebitmap = srcbitmap;
                }

                if (srcbitmap != rotatebitmap) {
                    srcbitmap.recycle();
                }
            } else {
                rotatebitmap = srcbitmap;
            }
        }

        return rotatebitmap;
    }

    public static Bitmap getBitmapFromFileAfterRotate(Uri uri) {
        if (uri == null) {
            return null;
        }

        Bitmap rotatebitmap = null;
        Bitmap srcbitmap = null;

        String picturePath = uri.getPath();
        if (picturePath != null) {
            int angle = 0;

            angle = getBitmapDegree(picturePath);

            BitmapFactory.Options opts = new BitmapFactory.Options();
            opts.inJustDecodeBounds = true;
            srcbitmap = BitmapFactory.decodeFile(picturePath, opts);

            opts.inSampleSize = computeSampleSize(opts);
            opts.inJustDecodeBounds = false;
            srcbitmap = BitmapFactory.decodeFile(picturePath, opts);
            if (angle != 0) {
                // 下面的方法主要作用是把图片转一个角度，也可以放大缩小等
                Matrix m = new Matrix();
                int width = srcbitmap.getWidth();
                int height = srcbitmap.getHeight();
                m.setRotate(angle); // 旋转angle度
                try {
                    rotatebitmap = Bitmap.createBitmap(srcbitmap, 0, 0, width, height, m, true);// 新生成图片
                } catch (Exception e) {

                }

                if (rotatebitmap == null) {
                    rotatebitmap = srcbitmap;
                }

                if (srcbitmap != rotatebitmap) {
                    srcbitmap.recycle();
                }
            } else {
                rotatebitmap = srcbitmap;
            }
        }

        return rotatebitmap;
    }

    public static int getBitmapDegree(String path) {
        int degree = 0;
        try {
            // 从指定路径下读取图片，并获取其EXIF信息
            ExifInterface exifInterface = new ExifInterface(path);
            // 获取图片的旋转信息
            int orientation = exifInterface.getAttributeInt(ExifInterface.TAG_ORIENTATION,
                    ExifInterface.ORIENTATION_NORMAL);
            switch (orientation) {
                case ExifInterface.ORIENTATION_ROTATE_90:
                    degree = 90;
                    break;
                case ExifInterface.ORIENTATION_ROTATE_180:
                    degree = 180;
                    break;
                case ExifInterface.ORIENTATION_ROTATE_270:
                    degree = 270;
                    break;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return degree;
    }

    public static Bitmap getBitmapFromDrawable(Context context, int resId) {
        BitmapFactory.Options opt = new BitmapFactory.Options();
        opt.inPreferredConfig = Bitmap.Config.ARGB_8888;
        opt.inSampleSize = 2;
        opt.inPurgeable = true;
        opt.inInputShareable = true;
        //获取资源图片
        InputStream is = context.getResources().openRawResource(resId);
        return BitmapFactory.decodeStream(is, null, opt);
    }

    public static int computeSampleSize(BitmapFactory.Options opts) {
        int sampleSize = 1;
        int width = opts.outWidth;
        int height = opts.outHeight;
        if (width > 2048 || height > 2048) {
            sampleSize = 4;
        } else if (width > 1024 || height > 1024) {
            sampleSize = 2;
        }

        return sampleSize;
    }

    public static void copyFilesToLocalIfNeed(Context context) {
        String assertPathDir = "filter";
        String dirPath = context.getFilesDir().getAbsolutePath() + File.separator + "filter";

        File pictureDir = new File(dirPath);
        if (!pictureDir.exists() || !pictureDir.isDirectory()) {
            pictureDir.mkdirs();
        }
        try {
            String[] fileNames = context.getAssets().list(assertPathDir);
            if (fileNames.length == 0)
                return;
            for (int i = 0; i < fileNames.length; i++) {
                File file = new File(dirPath + File.separator + fileNames[i]);
                if (file.exists() && file.isFile()) {
                    if (compareFile(context, dirPath + File.separator + fileNames[i], assertPathDir + File.separator + fileNames[i])) {
                        //printLog("-->copyAssertDirToLocalIfNeed " + file.getName() + " exists");
                        continue;
                    }
                }
                InputStream is = context.getAssets().open(assertPathDir + File.separator + fileNames[i]);
                int size = is.available();
                byte[] buffer = new byte[size];
                is.read(buffer);
                is.close();
                String mypath = dirPath + File.separator + fileNames[i];
                FileOutputStream fop = new FileOutputStream(mypath);
                fop.write(buffer);
                fop.flush();
                fop.close();
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static boolean compareFile(Context context, String filePath, String assetPath) {
        boolean isSameFile = false;
        File file = new File(filePath);
        if (!file.exists() || file.isDirectory()) {
            isSameFile = false;
        }
        if (getFileSize(file) == getAssertFileSize(context, assetPath)) {
            isSameFile = true;
        }
        return isSameFile;
    }

    /**
     * 获取指定文件大小
     *
     * @param file
     * @return
     * @throws Exception
     */
    public static long getFileSize(File file) {
        long size = 0;
        if (!file.exists() || file.isDirectory()) {
            //printLog("getFileSize file is not exists or isDirectory !");
            return 0;
        }
        if (file.exists()) {
            FileInputStream fis = null;
            try {
                fis = new FileInputStream(file);
                size = fis.available();
                fis.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return size;
    }

    /**
     * 获取Asset目录下某个文件的大小，非目录
     *
     * @param context
     * @param path
     * @return
     */
    public static long getAssertFileSize(Context context, String path) {
        if (context == null || path == null || "".equals(path)) {
            //printLog("getAssertFileSize context is null or path is null !");
            return 0;
        }
        //printLog("getAssertFileSize path:"+path);
        AssetManager assetManager = context.getAssets();
        int size = 0;
        try {
            InputStream inStream = assetManager.open(path);
            size = inStream.available();
            inStream.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return size;
    }

    public void copyModelIfNeed(String modelName) {
        String path = getModelPath(modelName);
        if (path != null) {
            File modelFile = new File(path);
            if (!modelFile.exists()) {
                //如果模型文件不存在或者当前模型文件的版本跟sdcard中的版本不一样
                try {
                    if (modelFile.exists())
                        modelFile.delete();
                    modelFile.createNewFile();
                    InputStream in = mContext.getApplicationContext().getAssets().open(modelName);
                    if (in == null) {
                        Log.e(TAG, "the src module is not existed");
                    }
                    OutputStream out = new FileOutputStream(modelFile);
                    byte[] buffer = new byte[4096];
                    int n;
                    while ((n = in.read(buffer)) > 0) {
                        out.write(buffer, 0, n);
                    }
                    in.close();
                    out.close();
                } catch (IOException e) {
                    modelFile.delete();
                }
            }
        }
    }

    public String getModelPath(String modelName) {
        String path = null;
        File dataDir = mContext.getApplicationContext().getExternalFilesDir(null);
        if (dataDir != null) {
            path = dataDir.getAbsolutePath() + File.separator + modelName;
        }
        return path;
    }

    public int getCameraTrackConfig() {
        int config = ST_MOBILE_TRACKING_DEFAULT_CONFIG | ST_MOBILE_TRACKING_ENABLE_DEBOUNCE | ST_MOBILE_TRACKING_ENABLE_FACE_ACTION;
        return config;
    }

    public static Rect adjustToImageRect(Rect intputRect, int screenWidth, int screenHeight, int imageWidth, int imageHeight) {
        Rect rect = new Rect();

        if ((float) screenHeight / screenWidth >= (float) imageHeight / imageWidth) {
            int gap = (screenHeight - screenWidth * imageHeight / imageWidth) / 2;

            if (intputRect.top <= gap) {
                rect.top = 0;
                rect.bottom = intputRect.height() * imageWidth / screenWidth;
            } else if (intputRect.bottom + gap >= screenHeight) {
                rect.bottom = imageHeight - 1;
                rect.top = rect.bottom - intputRect.height() * imageWidth / screenWidth;
            } else {
                rect.top = (intputRect.top - gap) * imageWidth / screenWidth;
                rect.bottom = (intputRect.bottom - gap) * imageWidth / screenWidth;
            }

            if (intputRect.left < 0) {
                rect.left = 0;
                rect.right = intputRect.width() * imageWidth / screenWidth;
            } else if (intputRect.right >= screenWidth) {
                rect.right = imageWidth - 1;
                rect.left = rect.right - intputRect.width() * imageWidth / screenWidth;
            } else {
                rect.left = intputRect.left * imageWidth / screenWidth;
                rect.right = intputRect.right * imageWidth / screenWidth;
            }
        } else {
            int gap = (screenWidth - (screenHeight * imageWidth / imageHeight)) / 2;

            if (intputRect.top < 0) {
                rect.top = 0;
                rect.bottom = intputRect.height() * imageHeight / screenHeight;
            } else if (intputRect.bottom >= screenHeight) {
                rect.bottom = imageHeight - 1;
                rect.top = rect.bottom - intputRect.height() * imageHeight / screenHeight;
            } else {
                rect.top = intputRect.top * imageHeight / screenHeight;
                rect.bottom = intputRect.bottom * imageHeight / screenHeight;
            }

            if (intputRect.left <= gap) {
                rect.left = 0;
                rect.right = intputRect.width() * imageHeight / screenHeight;
            } else if (intputRect.right + gap >= screenWidth) {
                rect.right = imageWidth - 1;
                rect.left = rect.right - intputRect.width() * imageHeight / screenHeight;
            } else {
                rect.left = (intputRect.left - gap) * imageHeight / screenHeight;
                rect.right = (intputRect.right - gap) * imageHeight / screenHeight;
            }
        }

        return rect;
    }

    public static Rect adjustToScreenRect(Rect intputRect, int screenWidth, int screenHeight, int imageWidth, int imageHeight) {
        Rect rect = intputRect;

        if ((float) screenHeight / screenWidth >= (float) imageHeight / imageWidth) {
            int gap = (int) (screenHeight - ((float) screenWidth / imageWidth * imageHeight)) / 2;

            rect.top = (int) (intputRect.top * (float) screenWidth / imageWidth) + gap;
            rect.bottom = (int) (intputRect.bottom * (float) screenWidth / imageWidth) + gap;

            rect.left = (int) (intputRect.left * (float) screenWidth / imageWidth);
            rect.right = (int) (intputRect.right * (float) screenWidth / imageWidth);
        } else {
            int gap = (int) (screenWidth - ((float) screenHeight / imageHeight * imageWidth)) / 2;

            rect.top = (int) (intputRect.top * (float) screenHeight / imageHeight);
            rect.bottom = (int) (intputRect.bottom * (float) screenHeight / imageHeight);

            rect.left = (int) (intputRect.left * (float) screenHeight / imageHeight) + gap;
            rect.right = (int) (intputRect.right * (float) screenHeight / imageHeight) + gap;
        }

        return rect;
    }

    public static Rect adjustToImageRectMin(Rect intputRect, int screenWidth, int screenHeight, int imageWidth, int imageHeight) {
        Rect rect = new Rect();

        if ((float) screenHeight / screenWidth >= (float) imageHeight / imageWidth) {
            float rate = (float) screenHeight / imageHeight;
            int gap = (int) (imageWidth - screenWidth / rate) / 2;

            if (intputRect.left <= 0) {
                rect.left = gap;
                rect.right = (int) (intputRect.width() / rate) + gap;
            } else if (intputRect.right >= screenWidth + gap * rate) {
                rect.left = (int) (imageWidth - gap - intputRect.width() / rate);
                rect.right = imageWidth - gap - 1;
            } else {
                rect.left = (int) (intputRect.left / rate + gap);
                rect.right = (int) (intputRect.right / rate + gap);
            }

            if (intputRect.top <= 0) {
                rect.top = 0;
                rect.bottom = (int) (intputRect.height() / rate);
            } else if (intputRect.bottom >= screenHeight) {
                rect.bottom = imageHeight - 1;
                rect.top = (int) (rect.bottom - intputRect.height() / rate);
            } else {
                rect.top = (int) (intputRect.top / rate);
                rect.bottom = (int) (intputRect.bottom / rate);
            }
        } else {
            float rate = (float) screenWidth / imageWidth;
            int gap = (int) (imageHeight - (screenHeight / rate)) / 2;

            if (intputRect.top <= 0) {
                rect.top = gap;
                rect.bottom = (int) (intputRect.height() / rate) + gap;
            } else if (intputRect.bottom >= screenHeight + gap * rate) {
                rect.bottom = imageHeight - 1;
                rect.top = (int) (rect.bottom - intputRect.height() / rate);
            } else {
                rect.top = (int) (intputRect.top / rate + gap);
                rect.bottom = (int) (intputRect.bottom / rate + gap);
            }

            if (intputRect.left <= 0) {
                rect.left = 0;
                rect.right = (int) (intputRect.width() / rate);
            } else if (intputRect.right + gap * rate >= screenWidth) {
                rect.right = imageWidth - 1;
                rect.left = (int) (rect.right - intputRect.width() / rate);
            } else {
                rect.left = (int) (intputRect.left / rate);
                rect.right = (int) (intputRect.right / rate);
            }
        }

        return rect;
    }

    public static Rect adjustToScreenRectMin(Rect intputRect, int screenWidth, int screenHeight, int imageWidth, int imageHeight) {
        Rect rect = intputRect;

        if ((float) screenHeight / screenWidth >= (float) imageHeight / imageWidth) {
            float rate = (float) screenHeight / imageHeight;
            int gap = (int) ((imageWidth - (float) screenWidth / rate) * rate / 2);

            rect.left = (int) (intputRect.left * rate - gap);
            rect.top = (int) (intputRect.top * rate);

            rect.right = (int) (intputRect.right * rate - gap);
            rect.bottom = (int) (intputRect.bottom * rate);
        } else {
            float rate = (float) screenWidth / imageWidth;
            int gap = (int) ((imageHeight - (float) screenHeight / rate) * rate / 2);

            rect.left = (int) (intputRect.left * rate);
            rect.top = (int) (intputRect.top * rate - gap);

            rect.right = (int) (intputRect.right * rate);
            rect.bottom = (int) (intputRect.bottom * rate - gap);
        }

        return rect;
    }

    public static STPoint[] adjustToScreenPoints(STPoint[] intputPoints, int screenWidth, int screenHeight, int imageWidth, int imageHeight) {
        if (intputPoints == null || intputPoints.length == 0) {
            return null;
        }
        STPoint[] points = intputPoints;

        if ((float) screenHeight / screenWidth >= (float) imageHeight / imageWidth) {
            float gap = (screenHeight - ((float) screenWidth / imageWidth * imageHeight)) / 2;

            for (int i = 0; i < intputPoints.length; i++) {
                points[i].setY(intputPoints[i].getY() * (float) screenWidth / imageWidth + gap);
                points[i].setX(intputPoints[i].getX() * (float) screenWidth / imageWidth);
            }
        } else {
            float gap = (screenWidth - ((float) screenHeight / imageHeight * imageWidth)) / 2;

            for (int i = 0; i < intputPoints.length; i++) {
                points[i].setX(intputPoints[i].getX() * (float) screenHeight / imageHeight);
                points[i].setY(intputPoints[i].getY() * (float) screenHeight / imageHeight + gap);
            }
        }

        return points;
    }

    static public void drawPoints(Canvas canvas, Paint paint, STPoint[] points) {

        if (canvas == null) return;
        int strokeWidth = 5;

        for (int i = 0; i < points.length; i++) {
            PointF p = new PointF(points[i].getX(), points[i].getY());

            paint.setColor(Color.rgb(0, 0, 255));
            canvas.drawCircle(p.x, p.y, strokeWidth, paint);
        }
    }

    public boolean deleteDirectory(String filePath) {
        File dirFile = new File(filePath);
        if (!dirFile.exists() || !dirFile.isDirectory()) {
            return false;
        }
        return true;
    }

    public static float[] get106Points(STHumanAction humanAction, int faceId, int width, int height) {
        if (humanAction == null || humanAction.getFaceCount() < faceId + 1) {
            return null;
        }

        int count = 106;

        float[] points = new float[count * 2];

        for (int i = 0; i < 106; i++) {
            points[2 * i] = humanAction.getFaces()[faceId].getFace().getPoints_array()[i].getX() / width * 2 - 1;
            points[2 * i + 1] = humanAction.getFaces()[faceId].getFace().getPoints_array()[i].getY() / height * 2 - 1;
        }

        return points;
    }

    public static float[] get240Points(STHumanAction humanAction, int size, int faceId, int width, int height) {
        if (humanAction == null || humanAction.getFaceCount() < faceId + 1) {
            return null;
        }
        int count = size;
        float[] points = new float[count * 2];
        for (int i = 0; i < size; i++) {
            points[2 * i] = humanAction.getFaces()[faceId].getExtraFacePoints()[i].getX() / width * 2 - 1;
            points[2 * i + 1] = humanAction.getFaces()[faceId].getExtraFacePoints()[i].getY() / height * 2 - 1;
        }
        return points;
    }

    public static float[] getFootPoints(STHumanAction humanAction, int size, int faceId, int width, int height) {
        if (humanAction == null || humanAction.getFootCount() < faceId + 1) {
            return null;
        }
        int count = size;
        float[] points = new float[count * 2];
        for (int i = 0; i < size; i++) {
            points[2 * i] = humanAction.getFeets()[faceId].getpKeyPoints()[i].getX() / width * 2 - 1;
            points[2 * i + 1] = humanAction.getFeets()[faceId].getpKeyPoints()[i].getY() / height * 2 - 1;
        }
        return points;
    }

    public static float[] getPointsTexture(STPoint[] points, int surfaceWidth, int surfaceHeight, int imageWidth, int imageHeight) {
        PointF[] rotatedFace106Points = new PointF[points.length];
        for (int j = 0; j < points.length; j++) {
            rotatedFace106Points[j] = new PointF(points[j].getX(), points[j].getY());
        }
        PointF[] pointF = STUtils.adjustToScreenPointsMin(rotatedFace106Points, surfaceWidth, surfaceHeight, imageWidth, imageHeight);
        float[] pointsResult = new float[pointF.length * 2];
        for (int j = 0; j < pointF.length; j++) {
            pointsResult[2 * j] = pointF[j].x / surfaceWidth * 2 - 1;
            pointsResult[2 * j + 1] = pointF[j].y / surfaceHeight * 2 - 1;
        }

        return pointsResult;
    }

    public static float[] getExtraPoints(STHumanAction humanAction, int faceId, int width, int height) {
        if (humanAction == null || humanAction.getFaceCount() < faceId + 1) {
            return null;
        }

        int count = humanAction.getFaces()[faceId].getExtraFacePointsCount() + humanAction.getFaces()[faceId].getEyeballCenterPointsCount() + humanAction.getFaces()[faceId].getEyeballContourPointsCount();

        float[] points = new float[count * 2];

        for (int i = 0; i < humanAction.getFaces()[faceId].getExtraFacePointsCount(); i++) {
            points[2 * i] = humanAction.getFaces()[faceId].getExtraFacePoints()[i].getX() / width * 2 - 1;
            points[2 * i + 1] = 1 - humanAction.getFaces()[faceId].getExtraFacePoints()[i].getY() / height * 2;
        }

        for (int i = 0; i < humanAction.getFaces()[faceId].getEyeballCenterPointsCount(); i++) {
            points[2 * i + 2 * humanAction.getFaces()[faceId].getExtraFacePointsCount()] = humanAction.getFaces()[faceId].getEyeballCenter()[i].getX() / width * 2 - 1;
            points[2 * i + 1 + 2 * humanAction.getFaces()[faceId].getExtraFacePointsCount()] = 1 - humanAction.getFaces()[faceId].getEyeballCenter()[i].getY() / height * 2;
        }

        for (int i = 0; i < humanAction.getFaces()[faceId].getEyeballContourPointsCount(); i++) {
            points[2 * i + 2 * (humanAction.getFaces()[faceId].getExtraFacePointsCount() + humanAction.getFaces()[faceId].getEyeballCenterPointsCount())] = humanAction.getFaces()[faceId].getEyeballContour()[i].getX() / width * 2 - 1;
            points[2 * i + 1 + 2 * (humanAction.getFaces()[faceId].getExtraFacePointsCount() + humanAction.getFaces()[faceId].getEyeballCenterPointsCount())] = 1 - humanAction.getFaces()[faceId].getEyeballContour()[i].getY() / height * 2;
        }

        return points;
    }

    public static float[] getCatPoints(STAnimalFace animalFace, int width, int height) {
        if (animalFace == null) {
            return null;
        }

        int count = animalFace.getKey_points_count();

        float[] points = new float[count * 2];

        for (int i = 0; i < count; i++) {
            points[2 * i] = animalFace.getP_key_points()[i].getX() / width * 2 - 1;
            points[2 * i + 1] = animalFace.getP_key_points()[i].getY() / height * 2 - 1;
        }

        return points;
    }

    public static float[] getBodyKeyPoints(STHumanAction humanAction, int bodyId, int width, int height) {
        if (humanAction == null || humanAction.getBodyCount() < bodyId + 1) {
            return null;
        }

        int count = humanAction.getBodys()[bodyId].getKeyPointsCount();
        float[] points = new float[count * 2];

        for (int i = 0; i < count; i++) {
            points[2 * i] = humanAction.getBodys()[bodyId].getKeyPoints()[i].getX() / width * 2 - 1;
            points[2 * i + 1] = humanAction.getBodys()[bodyId].getKeyPoints()[i].getY() / height * 2 - 1;
        }

        return points;
    }

    public static Rect rotateRect270AndMirror(Rect rect, int width, int height, boolean needMirror) {
        int left = rect.left;
        rect.left = rect.top;
        rect.top = height - rect.right;
        rect.right = rect.bottom;
        rect.bottom = height - left;

        if (needMirror) {
            Rect rectNew = new Rect();
            rectNew.left = width - rect.right;
            rectNew.right = width - rect.left;
            rectNew.top = rect.top;
            rectNew.bottom = rect.bottom;
            return rectNew;
        }

        return rect;
    }

    public static void rotateRect270AndMirror(float touchX, float touchY, int width, int height, boolean needMirror, float[] touchPositionOut) {
        if (touchPositionOut.length < 2) {
            return;
        }
        touchPositionOut[0] = touchY;
        touchPositionOut[1] = width - touchX;

        if (needMirror) {
            touchPositionOut[0] = height - touchY;
        }
    }

    public static Rect rotateRect90AndMirror(Rect rect, int width, int height, boolean needMirror) {
        int left = rect.left;
        rect.left = height - rect.bottom;
        rect.bottom = rect.right;
        rect.right = height - rect.top;
        rect.top = left;

        if (needMirror) {
            Rect rectNew = new Rect();
            rectNew.left = rect.left;
            rectNew.right = rect.right;
            rectNew.top = width - rect.bottom;
            rectNew.bottom = width - rect.top;
            return rectNew;
        }

        return rect;
    }

    public static void rotateRect90AndMirror(float touchX, float touchY, int width, int height, boolean needMirror, float[] touchPositionOut) {
        if (touchPositionOut.length < 2) {
            return;
        }
        touchPositionOut[0] = height - touchY;
        touchPositionOut[1] = touchX;

        if (needMirror) {
            touchPositionOut[1] = width - touchX;
        }
    }

    /**
     * 计算通用物体跟踪输入Rect，即根据屏幕输入Rect相对于原始buffer计算输入Rect（双输入场景使用）
     *
     * @param inputRect         屏幕输入Rect
     * @param width             图像宽度（buffer中）
     * @param height            图像高度（buffer中）
     * @param cameraId          摄像头ID
     * @param cameraOrientation 摄像头方向
     * @return 输出映射到buffer的Rect
     */
    public static Rect getObjectTrackInputRect(Rect inputRect, int width, int height, int cameraId, int cameraOrientation) {
        if (inputRect == null) {
            return null;
        }
        if (cameraId != Camera.CameraInfo.CAMERA_FACING_FRONT && cameraId != Camera.CameraInfo.CAMERA_FACING_BACK) {
            return inputRect;
        }
        if (cameraOrientation != 90 && cameraOrientation != 270) {
            return inputRect;
        }

        //前置摄像头显示图像相对于buffer为镜像的，需设置needMirrow为true
        if (cameraId == Camera.CameraInfo.CAMERA_FACING_FRONT && cameraOrientation == 90) {
            inputRect = STUtils.rotateRect270AndMirror(inputRect, height, width, true);
        } else if (cameraId == Camera.CameraInfo.CAMERA_FACING_FRONT && cameraOrientation == 270) {
            inputRect = STUtils.rotateRect90AndMirror(inputRect, width, height, true);
        } else if (cameraId == Camera.CameraInfo.CAMERA_FACING_BACK && cameraOrientation == 270) {
            inputRect = STUtils.rotateRect90AndMirror(inputRect, width, height, false);
        } else if (cameraId == Camera.CameraInfo.CAMERA_FACING_BACK && cameraOrientation == 90) {
            inputRect = STUtils.rotateRect270AndMirror(inputRect, height, width, false);
        }

        return inputRect;
    }

    /**
     * 计算通用物体跟踪输出Rect，映射到屏幕画框使用（双输入场景使用）
     *
     * @param outputRect        sdk输入Rect（相对于buffer）
     * @param width             图像宽度（buffer中）
     * @param height            图像高度（buffer中）
     * @param cameraId          摄像头ID
     * @param cameraOrientation 摄像头方向
     * @return 输出映射到屏幕的Rect
     */
    public static Rect getObjectTrackOutputRect(Rect outputRect, int width, int height, int cameraId, int cameraOrientation) {
        if (outputRect == null) {
            return null;
        }
        if (cameraId != Camera.CameraInfo.CAMERA_FACING_FRONT && cameraId != Camera.CameraInfo.CAMERA_FACING_BACK) {
            return outputRect;
        }
        if (cameraOrientation != 90 && cameraOrientation != 270) {
            return outputRect;
        }

        //前置摄像头显示图像相对于buffer为镜像的，需设置needMirrow为true
        if (cameraId == Camera.CameraInfo.CAMERA_FACING_FRONT && cameraOrientation == 90) {
            outputRect = STUtils.rotateRect90AndMirror(outputRect, height, width, true);
        } else if (cameraId == Camera.CameraInfo.CAMERA_FACING_FRONT && cameraOrientation == 270) {
            outputRect = STUtils.rotateRect270AndMirror(outputRect, width, height, true);
        } else if (cameraId == Camera.CameraInfo.CAMERA_FACING_BACK && cameraOrientation == 270) {
            outputRect = STUtils.rotateRect270AndMirror(outputRect, width, height, false);
        } else if (cameraId == Camera.CameraInfo.CAMERA_FACING_BACK && cameraOrientation == 90) {
            outputRect = STUtils.rotateRect90AndMirror(outputRect, height, width, false);
        }

        return outputRect;
    }

    /**
     * 根据手机的分辨率从 dp 的单位 转成为 px(像素)
     */
    public static int dip2px(Context context, float dpValue) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (dpValue * scale + 0.5f);
    }

    /**
     * 根据手机的分辨率从 px(像素) 的单位 转成为 dp
     */
    public static int px2dip(Context context, float pxValue) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (pxValue / scale + 0.5f);
    }

    public static Rect calculateArea(float[] touchPosition, float surfaceWidth, float surfaceHeight, int areaSize) {
        if (touchPosition.length < 2) {
            return null;
        }

        int centerX = (int) (touchPosition[0] / surfaceHeight * 2000 - 1000);
        int centerY = (int) (touchPosition[1] / surfaceWidth * 2000 - 1000);

        int left = clamp(centerX - areaSize / 2, -1000, 1000);
        int right = clamp(left + areaSize, -1000, 1000);
        int top = clamp(centerY - areaSize / 2, -1000, 1000);
        int bottom = clamp(top + areaSize, -1000, 1000);

        return new Rect(left, top, right, bottom);
    }

    public static int clamp(int x, int min, int max) {
        if (x > max) {
            return max;
        }
        if (x < min) {
            return min;
        }
        return x;
    }

    public static void calculateRotatetouchPoint(float touchX, float touchY, int width, int height, int cameraId, int cameraOrientation, float[] touchPositionOut) {
        if (cameraId != Camera.CameraInfo.CAMERA_FACING_FRONT && cameraId != Camera.CameraInfo.CAMERA_FACING_BACK) {
            return;
        }
        if (cameraOrientation != 90 && cameraOrientation != 270) {
            return;
        }

        //前置摄像头显示图像相对于buffer为镜像的，需设置needMirrow为true
        if (cameraId == Camera.CameraInfo.CAMERA_FACING_FRONT && cameraOrientation == 90) {
            STUtils.rotateRect270AndMirror(touchX, touchY, width, height, true, touchPositionOut);
        } else if (cameraId == Camera.CameraInfo.CAMERA_FACING_FRONT && cameraOrientation == 270) {
            STUtils.rotateRect90AndMirror(touchX, touchY, width, height, true, touchPositionOut);
        } else if (cameraId == Camera.CameraInfo.CAMERA_FACING_BACK && cameraOrientation == 270) {
            STUtils.rotateRect90AndMirror(touchX, touchY, width, height, false, touchPositionOut);
        } else if (cameraId == Camera.CameraInfo.CAMERA_FACING_BACK && cameraOrientation == 90) {
            STUtils.rotateRect270AndMirror(touchX, touchY, width, height, false, touchPositionOut);
        }
        Log.d("touchPosition", touchPositionOut[0] + "   " + touchPositionOut[1]);
    }

    public static int bodyBeautyNormalParamConvert(float current) {
        if (current < -1 || current > 1) {
            return 0;
        }
        int ans;
        ans = (int) ((current + 1.0f) * 50f);
        return ans;
    }

    public static float bodyBeautyParamConvert(float low, float high, float current) {
        if (current < 0 || current > 1) {
            return 0;
        }
        float ans;
        if (current >= 0.5f) {
            float a = current - 0.5f;
            float b = high - 1;
            ans = a * b + 0.5f;
            ans *= 2;
        } else {
            float a = 1 - low;
            float b = current * a;
            b += 0.5 * low;
            ans = b * 2;
        }
        return ans;
    }

    public static float bodyBeautyParamTwoSegmentConvert(float low, float high, float current) {
        if (current < 0 || current > 1) {
            return 0;
        }
        float ans;
        if (current >= 0.5f) {
            ans = 2 * high;
            ans *= (current - 0.5);
        } else {
            float tmp = 2 * low * current;
            ans = low - tmp;
        }
        return ans;
    }

    public static int convertToDisplay(int current) {
        if (current < 0 || current > 100) {
            return 0;
        }
        return 2 * current - 100;
    }

    public static int convertToData(int current) {
        if (current < -100 || current > 100) {
            return 0;
        }
        return (current + 100) / 2;
    }

    public static String getDeviceBrand() {
        return android.os.Build.BRAND;
    }

    public static String getTimeFormMs(int time) {
        if (time <= 0) {
            return "00:00";
        }
        int secondnd = ((time / 1000) / 60);
        int million = ((time / 1000) % 60);
        String f = secondnd >= 10 ? String.valueOf(secondnd) : "0" + String.valueOf(secondnd);
        String m = million >= 10 ? String.valueOf(million) : "0" + String.valueOf(million);
        return f + ":" + m;
    }


    public static void saveFloatArrayToFile(File file, float[] vArr) {
        if (null == vArr) {
            return;
        }

        long t = System.currentTimeMillis();

        StringBuffer tBuffer = new StringBuffer();
        //		int len = vArr.length;
        //		System.out.println("len="+len);

        for (float val : vArr) {
            //保留4位小数，这里可以改为其他值
            tBuffer.append(String.format("%.4f", val));
            tBuffer.append(" ");
        }
        tBuffer.append("\r\n");

        try {
            FileWriter out = new FileWriter(file, true);  //文件写入流
            out.write(tBuffer.toString());
            out.close();
        } catch (Exception e) {
            System.out.println("写文件出错:" + e.toString());
        }

        t = System.currentTimeMillis() - t;
        System.out.println("t=" + t);
    }

    public static Bitmap scaleBitmap(Bitmap origin, int newWidth, int newHeight) {
        if (origin == null) {
            return null;
        }
        int height = origin.getHeight();
        int width = origin.getWidth();
        float scaleWidth = ((float) newWidth) / width;
        float scaleHeight = ((float) newHeight) / height;
        Matrix matrix = new Matrix();
        matrix.postScale(scaleWidth, scaleHeight);// 使用后乘
        Bitmap newBM = Bitmap.createBitmap(origin, 0, 0, width, height, matrix, false);
        if (!origin.isRecycled()) {
            origin.recycle();
        }
        return newBM;
    }

    public static int convertMakeupTypeToNewType(int type) {
        int newType = 0;

        if (type == 1) {
            newType = EFFECT_BEAUTY_MAKEUP_EYE_SHADOW;
        } else if (type == 2) {
            newType = EFFECT_BEAUTY_MAKEUP_CHEEK;
        } else if (type == 3) {
            newType = EFFECT_BEAUTY_MAKEUP_LIP;
        } else if (type == 4) {
            newType = EFFECT_BEAUTY_MAKEUP_NOSE;
        } else if (type == 5) {
            newType = EFFECT_BEAUTY_MAKEUP_EYE_BROW;
        } else if (type == 6) {
            newType = EFFECT_BEAUTY_MAKEUP_EYE_LINE;
        } else if (type == 7) {
            newType = EFFECT_BEAUTY_MAKEUP_EYE_LASH;
        } else if (type == 8) {
            newType = EFFECT_BEAUTY_MAKEUP_EYE_BALL;
        } else if (type == 9) {
            newType = EFFECT_BEAUTY_HAIR_DYE;
        } else if (type == 10) {
            newType = EFFECT_BEAUTY_MAKEUP_ALL;
        }

        return newType;
    }

    public static int convertMakeupTypeToNewType2(int type) {
        int newType = 0;

        if (type == EFFECT_BEAUTY_MAKEUP_EYE_SHADOW) {//1
            newType = 1;
        } else if (type == EFFECT_BEAUTY_MAKEUP_CHEEK) {// 2
            newType = 2;
        } else if (type == EFFECT_BEAUTY_MAKEUP_LIP) {
            newType = 3;
        } else if (type == EFFECT_BEAUTY_MAKEUP_NOSE) {
            newType = 4;
        } else if (type == EFFECT_BEAUTY_MAKEUP_EYE_BROW) {
            newType = 5;
        } else if (type == EFFECT_BEAUTY_MAKEUP_EYE_LINE) {
            newType = 6;
        } else if (type == EFFECT_BEAUTY_MAKEUP_EYE_LASH) {
            newType = 7;
        } else if (type == EFFECT_BEAUTY_MAKEUP_EYE_BALL) {
            newType = 8;
        } else if (type == EFFECT_BEAUTY_HAIR_DYE) {
            newType = 9;
        } else if (type == EFFECT_BEAUTY_MAKEUP_ALL) {
            newType = 10;
        }

        return newType;
    }

    public static boolean isValidBeautifyParamsBase(float[] beautifyParamsBase, int index) {
        int[][] arrays = {{0, 1, 2}, {4, 5}};
        int[] array = contains(arrays, index);
        if (array == null) return true;
        if (isZero(beautifyParamsBase, array) && array[0] == index) return true;
        return beautifyParamsBase[index] > 0;
    }

    private static int[] contains(int[][] arrays, int key) {
        for (int[] array : arrays) {
            if (Arrays.binarySearch(array, key) >= 0) return array;
        }
        return null;
    }

    private static boolean isZero(float[] floats, int[] arrays) {
        float i = 0;
        for (int array : arrays) {
            i = i + floats[array];
        }
        return i == 0;
    }

    public static float[] getBeautifyParamsTypeBase(float[] params, STEffectBeautyInfo[] stEffectBeautyInfo) {
        //float[] params = {0f, 0f, 0f, 0f, 0f, 0f};
        for (STEffectBeautyInfo info : stEffectBeautyInfo) {
            if (info.getType() == 101) {
                if (info.getMode() == 0) {
                    params[0] = info.getStrength();
                    params[1] = 0f;
                    params[2] = 0f;
                } else {
                    params[0] = 0f;
                    params[1] = info.getStrength();
                    params[2] = 0f;
                }
            } else if (info.getType() == 102) {
                params[3] = info.getStrength();
            } else if (info.getType() == 103) {// 磨皮
                if (info.getMode() == 0) {
                    params[4] = info.getStrength();// 磨皮1
                    params[5] = 0f;//磨皮2置0
                }
                // 0位磨皮2
                else if (info.getMode() == 1) {
                    params[5] = 0f;
                    params[4] = info.getStrength();
                } else {
                    params[5] = info.getStrength();
                    params[4] = 0f;
                }
            }
        }
        return params;
    }

    public static boolean containsBaseParams(STEffectBeautyInfo[] stEffectBeautyInfo) {
        for (STEffectBeautyInfo info : stEffectBeautyInfo) {
            if (info.getType() / 100 == 1) return true;
        }
        return false;
    }

    public static boolean contains(String[] array, String key) {
        return Arrays.asList(array).contains(key);
    }

    public static Bitmap getImageFromAssetsFile(Context context, String fileName) {
        Bitmap image = null;
        try {
            String[] s = fileName.split("file:///android_asset/");
            fileName = s[1];
            AssetManager am = context.getResources().getAssets();
            try {
                InputStream is = am.open(fileName);
                image = BitmapFactory.decodeStream(is);
                is.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        } catch (ArrayIndexOutOfBoundsException e) {
            e.printStackTrace();
        }
        return image;
    }
}
