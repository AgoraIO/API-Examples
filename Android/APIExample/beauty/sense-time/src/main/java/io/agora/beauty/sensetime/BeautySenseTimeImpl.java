package io.agora.beauty.sensetime;

import android.content.Context;

import com.sensetime.effects.STRenderer;
import com.sensetime.stmobile.STCommonNative;

import io.agora.beauty.base.IBeautySenseTime;

public class BeautySenseTimeImpl implements IBeautySenseTime {
    private final STRenderer stRenderer;

    public BeautySenseTimeImpl(Context context) {
        stRenderer = new STRenderer(context);
    }

    @Override
    public int process(int cameraId, int width, int height, int orientation, boolean mirror, byte[] nv21, int textureId, int texFormat, float[] texMatrix) {
        return stRenderer.preProcess(
                cameraId,
                width, height, orientation, mirror,
                nv21, STCommonNative.ST_PIX_FMT_NV21,
                textureId, texFormat, texMatrix
        );
    }

    @Override
    public int process(int cameraId, int width, int height, int orientation, boolean mirror, byte[] nv21) {
        return stRenderer.preProcess(
                cameraId,
                width, height, orientation, mirror,
                nv21, STCommonNative.ST_PIX_FMT_NV21
        );
    }
}
