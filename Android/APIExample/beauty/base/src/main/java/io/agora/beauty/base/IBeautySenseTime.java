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
     * @param mirror      camera preview mirror
     * @param nv21        camera preview pixel data
     * @param textureId   camera preview texture id
     * @param texFormat   {@link GLES11Ext#GL_TEXTURE_EXTERNAL_OES} or {@link GLES20#GL_TEXTURE_2D}
     * @param texMatrix   float[16]
     * @return new Texture ID to render
     */
    public int process(
            int cameraId,
            int width, int height, int orientation, boolean mirror,
            byte[] nv21,
            int textureId, int texFormat, float[] texMatrix);


    /**
     * @param width       camera preview width
     * @param height      camera preview height
     * @param orientation camera preview orientation
     * @param mirror      camera preview mirror
     * @param nv21        camera preview pixel data
     * @return new Texture ID to render
     */
    int process(
            int cameraId,
            int width, int height, int orientation, boolean mirror,
            byte[] nv21
    );


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
