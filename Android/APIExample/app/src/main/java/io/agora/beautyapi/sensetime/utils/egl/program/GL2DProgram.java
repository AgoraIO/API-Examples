package io.agora.beautyapi.sensetime.utils.egl.program;

import android.opengl.GLES20;

public class GL2DProgram extends BaseProgram {

    private static final String FRAGMENT_SHADER =
            "precision mediump float;\n" +
                    "varying vec2 vTextureCoord;\n" +
                    "uniform sampler2D sTexture;\n" +
                    "void main() {\n" +
                    "    gl_FragColor = texture2D(sTexture, vTextureCoord);\n" +
                    "}\n";

    public GL2DProgram() {
        super(FRAGMENT_SHADER);
    }

    @Override
    public void draw(int textureId, float[] texMatrix, float[] mvpMatrix) {
        drawReal(textureId, GLES20.GL_TEXTURE_2D, texMatrix, mvpMatrix);
    }
}
