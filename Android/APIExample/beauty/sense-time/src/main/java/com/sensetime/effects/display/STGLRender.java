package com.sensetime.effects.display;

import android.opengl.Matrix;

public class STGLRender {
    private final static String TAG = "STGLRender";

    public static final float[] IDENTITY_MATRIX = new float[16];
    static {
        Matrix.setIdentityM(IDENTITY_MATRIX, 0);
    }

    private float[] renderMVPMatrix = new float[16];


    private RenderProgram mRenderProgram;
    private int mRotation;
    private boolean mFlipH, mFlipV;

    public STGLRender(int textureType) {
        mRenderProgram = new RenderProgram(textureType);
    }

    public void adjustRenderSize(int width, int height, int rotation, boolean flipH, boolean flipV) {
        boolean change = mRenderProgram.resize(width, height) || mRotation != rotation || mFlipH != flipH || mFlipV != flipV;
        if (change) {
            mRotation = rotation;
            mFlipH = flipH;
            mFlipV = flipV;
            float[] tmp = new float[16];
            Matrix.setIdentityM(tmp, 0);

            boolean _flipH = flipH;
            boolean _flipV = flipV;
            if(rotation % 180 != 0){
                _flipH = flipV;
                _flipV = flipH;
            }

            if (_flipH) {
                Matrix.rotateM(tmp, 0, tmp, 0, 180, 0, 1f, 0);
            }
            if (_flipV) {
                Matrix.rotateM(tmp, 0, tmp, 0, 180, 1f, 0f, 0);
            }

            float _rotation = rotation;
            if (_rotation != 0) {
                if(_flipH != _flipV){
                    _rotation *= -1;
                }
                Matrix.rotateM(tmp, 0, tmp, 0, _rotation, 0, 0, 1);
            }

            Matrix.setIdentityM(renderMVPMatrix, 0);
            Matrix.multiplyMM(renderMVPMatrix, 0, tmp, 0, renderMVPMatrix, 0);
        }
    }

    public int process(int textureId, float[] texMatrix) {
        return mRenderProgram.process(textureId, texMatrix, renderMVPMatrix);
    }


    public void destroyPrograms() {
        mRenderProgram.destroyProgram();
    }
}
