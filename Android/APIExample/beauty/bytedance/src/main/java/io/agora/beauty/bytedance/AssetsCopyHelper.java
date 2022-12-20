package io.agora.beauty.bytedance;

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
