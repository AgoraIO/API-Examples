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

package io.agora.beautyapi.bytedance.utils;

import android.content.Context;
import android.text.TextUtils;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.Executor;
import java.util.concurrent.Executors;

public class AssetsResourcesHelper implements EffectResourceProvider {
    private static final String VERSION_FILE_NAME = "Version";

    private static final Executor sWorkerExecutor = Executors.newSingleThreadExecutor();

    private final Context mContext;
    private final String mExternalResourcePath;
    private final int mResourceVersion;
    private final String mAssetsPath;

    public AssetsResourcesHelper(Context context, String assetsPath){
        this(context, assetsPath, context.getExternalFilesDir("beauty").getAbsolutePath() + File.separator + "bytedance", 1);
    }

    public AssetsResourcesHelper(Context context, String assetsPath, String cachePath, int resourceVersion) {
        mResourceVersion = resourceVersion;
        mContext = context.getApplicationContext();
        mExternalResourcePath = cachePath;
        mAssetsPath = assetsPath;
        init();
    }

    public void init() {
        sWorkerExecutor.execute(() -> {
            try {
                if (getDirVersion(mExternalResourcePath) < mResourceVersion) {
                    deleteDirection(mExternalResourcePath);
                    setDirVersion(mExternalResourcePath, mResourceVersion);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        });
    }


    private String getResourcePathSafely(String path) {
        String targetPath = mExternalResourcePath + File.separator + path;
        if (new File(targetPath).exists()) {
            return targetPath;
        }
        String assetsPath = mAssetsPath + File.separator + path;

        CountDownLatch latch = new CountDownLatch(1);
        sWorkerExecutor.execute(() -> {
            try {
                copyAssets(mContext, assetsPath, targetPath);
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                latch.countDown();
            }
        });
        try {
            latch.await();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        return targetPath;
    }

    public String getLicensePath(String licenseName) {
        return getResourcePathSafely("LicenseBag.bundle" + File.separator + licenseName);
    }

    // Copy util methods

    public static int getDirVersion(String targetDir) throws Exception {
        if (TextUtils.isEmpty(targetDir)) {
            return -1;
        }
        File versionFile = new File(targetDir + File.separator + VERSION_FILE_NAME);
        if (!versionFile.exists()) {
            return -1;
        }
        BufferedReader reader = new BufferedReader(new FileReader(versionFile));
        String line = reader.readLine();
        reader.close();
        try {
            return Integer.parseInt(line);
        } catch (NumberFormatException e) {
            return -1;
        }
    }

    private static void setDirVersion(String targetDir, int version) throws Exception {
        if (TextUtils.isEmpty(targetDir)) {
            return;
        }
        File targetDirFile = new File(targetDir);
        if(!targetDirFile.exists() && !targetDirFile.mkdirs()){
            return;
        }
        File versionFile = new File(targetDirFile,  VERSION_FILE_NAME);
        if(!versionFile.exists() &&  !versionFile.createNewFile()){
           return;
        }
        BufferedWriter writer = new BufferedWriter(new FileWriter(versionFile, false));
        writer.write(String.valueOf(version));
        writer.close();
    }

    public static boolean deleteDirection(String targetDir) {
        if (TextUtils.isEmpty(targetDir)) {
            return false;
        }
        File targetFile = new File(targetDir);
        if (targetFile.isFile()) {
            return targetFile.delete();
        } else {
            File[] files = targetFile.listFiles();
            if (files != null && files.length > 0) {
                for (File file : files) {
                    if (!deleteDirection(file.getAbsolutePath())) {
                        return false;
                    }
                }
                return targetFile.delete();
            }else{
                return targetFile.delete();
            }
        }
    }

    /**
     * 拷贝assets文件下文件到指定路径
     *
     * @param context
     * @param assetDir  源文件/文件夹
     * @param targetDir 目标文件夹
     * @throws Exception
     */
    public static void copyAssets(Context context, String assetDir, String targetDir) throws Exception {
        if (TextUtils.isEmpty(assetDir) || TextUtils.isEmpty(targetDir)) {
            return;
        }
        String separator = File.separator;
        // 获取assets目录assetDir下一级所有文件以及文件夹
        String[] fileNames = context.getResources().getAssets().list(assetDir);
        // 如果是文件夹(目录),则继续递归遍历
        if (fileNames.length > 0) {
            File targetFile = new File(targetDir);
            if (!targetFile.exists() && !targetFile.mkdirs()) {
                return;
            }
            for (String fileName : fileNames) {
                copyAssets(context, assetDir + separator + fileName, targetDir + separator + fileName);
            }
        } else {
            copy(context, assetDir, targetDir);
        }
    }

    /**
     * 复制文件
     *
     * @param context    上下文对象
     * @param zipPath    源文件
     * @param targetPath 目标文件
     * @throws Exception
     */
    public static void copy(Context context, String zipPath, String targetPath) throws Exception {
        if (TextUtils.isEmpty(zipPath) || TextUtils.isEmpty(targetPath)) {
            return;
        }
        Exception exception = null;
        File dest = new File(targetPath);
        dest.getParentFile().mkdirs();
        InputStream in = null;
        OutputStream out = null;
        try {
            in = new BufferedInputStream(context.getAssets().open(zipPath));
            out = new BufferedOutputStream(new FileOutputStream(dest));
            byte[] buffer = new byte[1024];
            int length = 0;
            while ((length = in.read(buffer)) != -1) {
                out.write(buffer, 0, length);
            }
        } catch (FileNotFoundException e) {
            exception = new Exception(e);
        } catch (IOException e) {
            exception = new Exception(e);
        } finally {
            try {
                out.close();
                in.close();
            } catch (IOException e) {
                exception = new Exception(e);
            }
        }
        if (exception != null) {
            throw exception;
        }
    }


    // EffectResourceProvider impletation

    @Override
    public String getModelPath() {
        return getResourcePathSafely("ModelResource.bundle");
    }

    @Override
    public String getComposePath() {
        return getResourcePathSafely("ComposeMakeup.bundle") + File.separator + "ComposeMakeup" + File.separator;
    }

    @Override
    public String getFilterPath() {
        return getResourcePathSafely("FilterResource.bundle");
    }

    @Override
    public String getFilterPath(String filter) {
        return getResourcePathSafely("FilterResource.bundle" + File.separator + "Filter" + File.separator + filter);
    }

    @Override
    public String getStickerPath(String sticker) {
        return getResourcePathSafely("StickerResource.bundle" + File.separator + "stickers" + File.separator + sticker);
    }
}
