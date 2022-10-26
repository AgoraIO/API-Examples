// Copyright (C) 2018 Beijing Bytedance Network Technology Co., Ltd.
package com.byteddance.effect;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.AssetManager;

import androidx.annotation.NonNull;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.concurrent.CountDownLatch;

public class ResourceHelper {

    public static final String RESOURCE = "resource";
    public static final String FILTER_RESOURCE = "FilterResource.bundle/Filter";
    private static final String LICENSE_NAME = "labcv_test_20220210_20230210_com.bytedance.labcv.demo_4.2.1.licbag";

    public static void initialize(Context context) {
        CountDownLatch latch = new CountDownLatch(1);
        new Thread() {
            @Override
            public void run() {
                super.run();
                String resourcePath = getResourcePath(context);
                File rootDir = new File(resourcePath);
                if (rootDir.exists()) {
                    latch.countDown();
                    return;
                } else if (!rootDir.mkdirs()) {
                    latch.countDown();
                    return;
                }
                try {
                    copyAssets(context.getAssets(), RESOURCE, getAssetsPath(context));
                } catch (IOException e) {
                    e.printStackTrace();
                }
                latch.countDown();
            }
        }.start();
        try {
            latch.await();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public static void copyAssets(AssetManager assets, String path, String rootDir) throws IOException {
        if (isAssetsDir(assets, path)) {
            File dir = new File(rootDir + File.separator + path);
            if (!dir.exists() && !dir.mkdirs()) {
                throw new IllegalStateException("mkdir failed");
            }
            for (String s : assets.list(path)) {
                copyAssets(assets, path + "/" + s, rootDir);
            }
        } else {
            InputStream input = assets.open(path);
            File dest = new File(rootDir, path);
            copyToFileOrThrow(input, dest);
        }

    }

    private static boolean isAssetsDir(AssetManager assets, String path) {
        try {

            String[] files = assets.list(path);
            return files != null && files.length > 0;
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

    private static void copyToFileOrThrow(InputStream inputStream, File destFile)
            throws IOException {
        if (destFile.exists()) {
            return;

        }
        File file = destFile.getParentFile();
        if (file != null && !file.exists()) {
            file.mkdirs();
        }
        FileOutputStream out = new FileOutputStream(destFile);
        try {
            byte[] buffer = new byte[4096];
            int bytesRead;
            while ((bytesRead = inputStream.read(buffer)) >= 0) {
                out.write(buffer, 0, bytesRead);
            }
        } finally {
            out.flush();
            try {
                out.getFD().sync();
            } catch (IOException e) {
            }
            out.close();
        }
    }


    private static String getResourcePath(Context context) {
        return getAssetsPath(context) + File.separator + RESOURCE;
    }

    @NonNull
    private static String getAssetsPath(Context context) {
        return context.getExternalFilesDir("assets").getAbsolutePath();
    }

    public static String getModelDir(@NonNull final Context context) {
        File file = new File(new File(getResourcePath(context), "ModelResource.bundle"), "");
        return file.getAbsolutePath();
    }

    public static String getLicensePath(@NonNull final Context context) {
        File file = new File(new File(getResourcePath(context), "LicenseBag.bundle"), LICENSE_NAME);
        return file.getAbsolutePath();
    }

    public static String getStickersPath(@NonNull final Context context) {
        File file = new File(new File(getResourcePath(context), "StickerResource.bundle"), "stickers");
        return file.getAbsolutePath();
    }

    public static String getAnimojiPath(final Context context) {
        File file = new File(new File(getResourcePath(context), "StickerResource.bundle"), "animoji");
        return file.getAbsolutePath();
    }

    public static String getGamePath(Context context) {
        File file = new File(new File(getResourcePath(context), "StickerResource.bundle"), "game");
        return file.getAbsolutePath();
    }

    public static String getComposeMakeupComposerPath(@NonNull final Context context) {
        return getResourcePath(context) + File.separator + "ComposeMakeup.bundle" + File.separator + "ComposeMakeup/composer";
    }

    public static String getComposePath(@NonNull final Context context) {
        return getResourcePath(context) + File.separator + "ComposeMakeup.bundle"  + File.separator + "ComposeMakeup/";
    }


    public static File[] getFilterResources(@NonNull final Context context) {
        return getResources(context, FILTER_RESOURCE);
    }


    public static File[] getResources(@NonNull final Context context, String type) {
        File file = new File(new File(getResourcePath(context), type), "");
        if (file.exists() && file.isDirectory())
            return file.listFiles();
        return new File[0];
    }

    public static String getStickerPath(@NonNull final Context context,String name) {
        return getStickersPath(context) + File.separator + name;

    }

    public static  String getAnimojiPath(final Context context, String name) {
        return getAnimojiPath(context) + File.separator + name;
    }

    public static String getGamePath(Context context, String name) {
        return getGamePath(context) + File.separator + name;
    }

    public static boolean isResourceReady(@NonNull final Context context, int versionCode) {

        SharedPreferences preferences=context.getSharedPreferences("user", Context.MODE_PRIVATE);
       boolean resourceReady = preferences.getBoolean("resource", false);
        int preVersioncode = preferences.getInt("versionCode", 0);

        // 如果上次已经拷贝过 继续检查版本号
        // Continue to check the version number if it was copied last time
       if (resourceReady && versionCode == preVersioncode){
           return true;
       }
       return false;
    }

    public static void setResourceReady(@NonNull final Context context, boolean isReady, int versionCode){
        SharedPreferences preferences=context.getSharedPreferences("user", Context.MODE_PRIVATE);

        SharedPreferences.Editor editor = preferences.edit();
        editor.putBoolean("resource", isReady);
        editor.putInt("versionCode", versionCode);
        editor.commit();
    }

    public static String getDownloadedStickerDir(@NonNull final Context context){
        File file = new File(new File(getResourcePath(context), "download"), "sticker");
        if (!file.exists()) {
            file.mkdirs();

        }
        return file.getAbsolutePath();


    }

    public static String getFilterResourcePathByName(@NonNull final Context context, String filterName){
        File file = new File(new File(getResourcePath(context), FILTER_RESOURCE), "");

        return file.getAbsolutePath() + File.separator + filterName;
    }




}
