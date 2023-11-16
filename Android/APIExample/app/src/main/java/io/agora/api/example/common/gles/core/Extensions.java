package io.agora.api.example.common.gles.core;

import android.content.Context;
import android.content.res.AssetManager;

import java.io.IOException;
import java.io.InputStream;

/**
 * The type Extensions.
 */
public abstract class Extensions {

    /**
     * Get bytes byte [ ].
     *
     * @param inputStream the input stream
     * @return the byte [ ]
     */
    public static byte[] getBytes(InputStream inputStream) {
        try {
            byte[] bytes = new byte[inputStream.available()];
            inputStream.read(bytes);
            inputStream.close();
            return bytes;
        } catch (IOException e) {
            e.printStackTrace();
        }

        return new byte[0];
    }

    /**
     * Get bytes byte [ ].
     *
     * @param assetManager the asset manager
     * @param fileName     the file name
     * @return the byte [ ]
     */
    public static byte[] getBytes(AssetManager assetManager, String fileName) {
        try {
            return getBytes(assetManager.open(fileName));
        } catch (IOException e) {
            e.printStackTrace();
        }

        return new byte[0];
    }

    /**
     * Read text file from resource string.
     *
     * @param context    the context
     * @param resourceId the resource id
     * @return the string
     */
    public static String readTextFileFromResource(Context context, int resourceId) {
        return new String(Extensions.getBytes(context.getResources().openRawResource(resourceId)));
    }

}
