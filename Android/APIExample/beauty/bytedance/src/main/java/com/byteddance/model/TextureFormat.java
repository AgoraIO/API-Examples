package com.byteddance.model;

import android.opengl.GLES11Ext;
import android.opengl.GLES20;

public enum TextureFormat {
    Texture2D(GLES20.GL_TEXTURE_2D),
    Texture_Oes(GLES11Ext.GL_TEXTURE_EXTERNAL_OES);
    private int value;

    TextureFormat(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }
}
