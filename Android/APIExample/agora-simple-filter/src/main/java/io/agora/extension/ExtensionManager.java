package io.agora.extension;

import android.content.Context;

import androidx.annotation.Keep;

import java.io.File;
import java.io.IOException;

@Keep
public class ExtensionManager {
    public static final String EXTENSION_NAME = "agora-simple-filter"; // Name of target link library used in CMakeLists.txt
    public static final String EXTENSION_VENDOR_NAME = "Agora"; // Provider name used for registering in agora-bytedance.cpp
    public static final String EXTENSION_VIDEO_FILTER_WATERMARK = "Watermark"; // Video filter name defined in ExtensionProvider.h
    public static final String EXTENSION_AUDIO_FILTER_VOLUME = "VolumeChange"; // Audio filter name defined in ExtensionProvider.h

    public static final String KEY_ENABLE_WATER_MARK = "key";
    public static final String ENABLE_WATER_MARK_FLAG = "plugin.watermark.wmEffectEnabled";
    public static final String ENABLE_WATER_MARK_STRING = "plugin.watermark.wmStr";
    public static final String KEY_ADJUST_VOLUME_CHANGE = "volume";

    public static void copyResource(Context context) {
        String path = "resource";
        File dstFile = context.getExternalFilesDir("assets");
        FileUtils.clearDir(new File(dstFile, path));

        try {
            FileUtils.copyAssets(context.getAssets(), path, dstFile.getAbsolutePath());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}