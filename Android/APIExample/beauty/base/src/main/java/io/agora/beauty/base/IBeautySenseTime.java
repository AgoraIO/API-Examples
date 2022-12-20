package io.agora.beauty.base;

import android.content.Context;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;

import java.lang.reflect.Constructor;

public interface IBeautySenseTime {


    /**
     * @param width       camera preview width
     * @param height      camera preview height
     * @param orientation camera preview orientation
     * @param nv21        camera preview pixel data
     * @param textureId   camera preview texture id
     * @param texFormat   {@link GLES11Ext#GL_TEXTURE_EXTERNAL_OES} or {@link GLES20#GL_TEXTURE_2D}
     * @return new Texture ID to render
     */
    public int process(
            byte[] nv21, int textureId, int texFormat, int width, int height, int orientation);


    int process(
            byte[] nv21,
            int width, int height, int orientation
    );

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
