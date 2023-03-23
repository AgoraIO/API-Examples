package io.agora.api.example.examples.advanced.beauty;

import android.graphics.Matrix;
import android.util.Size;

import io.agora.base.VideoFrame;
import io.agora.base.internal.video.RendererCommon;

public class VideoCaptureUtils {

    public static Size getCaptureOriginSize(VideoFrame.TextureBuffer texBuffer){
        int width = texBuffer.getWidth();
        int height = texBuffer.getHeight();
        Matrix texMatrix = texBuffer.getTransformMatrix();

        // 根据Matrix反算纹理的真实宽高
        Matrix renderMatrix = new Matrix();
        renderMatrix.preTranslate(0.5F, 0.5F);
        renderMatrix.preScale(1.0F, -1.0F);
        renderMatrix.preTranslate(-0.5F, -0.5F);
        Matrix finalMatrix = new Matrix(texMatrix);
        finalMatrix.preConcat(renderMatrix);
        float[] finalGlMatrix = RendererCommon.convertMatrixFromAndroidGraphicsMatrix(finalMatrix);
        int texWidth = (int) (width * 1.0f / finalGlMatrix[0] + 0.5f);
        int texHeight = (int) (height * 1.0f / finalGlMatrix[5] + 0.5f);
        return new Size(texWidth, texHeight);
    }

}
