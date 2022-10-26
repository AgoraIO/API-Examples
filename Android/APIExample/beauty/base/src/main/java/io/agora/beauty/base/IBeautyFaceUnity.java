package io.agora.beauty.base;

import android.content.Context;

import java.lang.reflect.Constructor;

public interface IBeautyFaceUnity {

    int process(int oesTexId, int width, int height);

    int process(byte[] nv21, int oesTexId, int width, int height);

    void release();

    void setFaceBeautifyEnable(boolean enable);

    void setMakeUpEnable(boolean enable);

    void setStickerEnable(boolean enable);

    void setBodyBeautifyEnable(boolean enable);

    static boolean hasIntegrated(){
        try {
            Class.forName("io.agora.beauty.faceunity.BeautyFaceUnityImpl");
            return true;
        } catch (ClassNotFoundException e) {
            return false;
        }
    }

    static IBeautyFaceUnity create(Context context) {
        try {
            Class<?> implClazz = Class.forName("io.agora.beauty.faceunity.BeautyFaceUnityImpl");
            Constructor<?> constructor = implClazz.getConstructor(Context.class);
            return (IBeautyFaceUnity) constructor.newInstance(context);
        } catch (Exception e) {
            return null;
        }
    }

}
