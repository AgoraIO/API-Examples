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
import android.content.res.AssetManager;

import androidx.annotation.NonNull;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class AssetsCopyHelper {
    private final Context mContext;
    private final String mAssetsPath;
    private final String mExternalPath;

    private volatile boolean resourceCopyInterupt = false;
    private Thread resourceCopyThread;

    public AssetsCopyHelper(Context context, String assetsPath, String externalPath){
        mContext = context;
        mAssetsPath = assetsPath;
        mExternalPath = externalPath;
    }

    public void start(@NonNull Runnable complete) {
        resourceCopyInterupt = false;
        resourceCopyThread = new Thread() {
            @Override
            public void run() {
                super.run();
                boolean done = false;
                try {
                    done = copyAssets(mContext.getAssets(), mAssetsPath, mExternalPath);
                } catch (IOException e) {
                    e.printStackTrace();
                }
                if(done){
                    complete.run();
                }
            }
        };
        resourceCopyThread.start();
    }

    public void stop(){
        resourceCopyInterupt = true;
        if(resourceCopyThread != null){
            try {
                resourceCopyThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }finally {
                resourceCopyThread = null;
            }
        }
    }

    private boolean copyAssets(AssetManager assets, String path, String rootDir) throws IOException {
        if(resourceCopyInterupt){
            return false;
        }
        if (isAssetsDir(assets, path)) {
            File dir = new File(rootDir + File.separator + path);
            if (!dir.exists() && !dir.mkdirs()) {
                throw new IllegalStateException("mkdir failed");
            }
            for (String s : assets.list(path)) {
                if (!copyAssets(assets, path + "/" + s, rootDir)) {
                    return false;
                }
            }
        } else {
            InputStream input = assets.open(path);
            File dest = new File(rootDir, path);
            copyToFileOrThrow(input, dest);
        }
        return true;
    }

    private static boolean isAssetsDir(@NonNull AssetManager assets, String path) {
        try {
            String[] files = assets.list(path);
            return files != null && files.length > 0;
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

    private static void copyToFileOrThrow(InputStream inputStream, @NonNull File destFile)
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
}
