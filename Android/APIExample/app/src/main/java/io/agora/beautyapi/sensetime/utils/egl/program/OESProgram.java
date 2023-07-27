package io.agora.beautyapi.sensetime.utils.egl.program;

import android.opengl.GLES11Ext;

public class OESProgram extends BaseProgram{

    // Simple fragment shader for use with external 2D textures (e.g. what we get from SurfaceTexture).
    private static final String FRAGMENT_SHADER_EXT =
            "#extension GL_OES_EGL_image_external : require\n" +
                    "precision mediump float;\n" +
                    "varying vec2 vTextureCoord;\n" +
                    "uniform samplerExternalOES sTexture;\n" +
                    "void main() {\n" +
                    "    gl_FragColor = texture2D(sTexture, vTextureCoord);\n" +
                    "}\n";


    public OESProgram() {
        super(FRAGMENT_SHADER_EXT);
    }

    @Override
    public void draw(int textureId, float[] texMatrix, float[] mvpMatrix) {
        drawReal(textureId, GLES11Ext.GL_TEXTURE_EXTERNAL_OES, texMatrix, mvpMatrix);
    }
}
