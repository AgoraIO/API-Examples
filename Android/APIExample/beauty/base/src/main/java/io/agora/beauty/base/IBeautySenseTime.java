package io.agora.beauty.base;

import android.content.Context;

import java.lang.reflect.Constructor;

import io.agora.rtc2.video.IVideoFrameObserver;

public interface IBeautySenseTime {


    IVideoFrameObserver getVideoFrameObserver();

    void release();

    void setFaceBeautifyEnable(boolean enable);

    void setMakeUpEnable(boolean enable);

    void setStickerEnable(boolean enable);

    void setFilterEnable(boolean enable);

    static boolean hasIntegrated(){
        try {
            Class.forName("io.agora.beauty.sensetime.BeautySenseTimeImpl");
            return true;
        } catch (ClassNotFoundException e) {
            return false;
        }
    }

    static IBeautySenseTime create(Context context) {
        try {
            Class<?> implClazz = Class.forName("io.agora.beauty.sensetime.BeautySenseTimeImpl");
            Constructor<?> constructor = implClazz.getConstructor(Context.class);
            return (IBeautySenseTime) constructor.newInstance(context);
        } catch (Exception e) {
            return null;
        }
    }

}
