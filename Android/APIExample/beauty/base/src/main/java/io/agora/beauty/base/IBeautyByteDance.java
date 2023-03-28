package io.agora.beauty.base;

import android.content.Context;

import java.lang.reflect.Constructor;

public interface IBeautyByteDance {

    int process(int oesTexId, int width, int height, int rotation);

    int process(byte[] nv21, int width, int height, int rotation);

    void release();

    void setFaceBeautifyEnable(boolean enable);

    void setMakeUpEnable(boolean enable);

    void setStickerEnable(boolean enable);

    void setBodyBeautifyEnable(boolean enable);

    static boolean hasIntegrated(){
        try {
            Class.forName("io.agora.beauty.bytedance.BeautyByteDanceImpl");
            return true;
        } catch (ClassNotFoundException e) {
            return false;
        }
    }

    static IBeautyByteDance create(Context context) {
        try {
            Class<?> implClazz = Class.forName("io.agora.beauty.bytedance.BeautyByteDanceImpl");
            Constructor<?> constructor = implClazz.getConstructor(Context.class);
            return (IBeautyByteDance) constructor.newInstance(context);
        } catch (Exception e) {
            return null;
        }
    }
}
