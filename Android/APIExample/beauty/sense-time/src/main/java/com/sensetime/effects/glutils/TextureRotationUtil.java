package com.sensetime.effects.glutils;

public class TextureRotationUtil {
    public static final float TEXTURE_NO_ROTATION[] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
    };

    public static final float TEXTURE_ROTATED_0[] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
    };

    public static final float TEXTURE_ROTATED_90[] = {
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
    };

    public static final float TEXTURE_ROTATED_180[] = {
            1.0f, 0.0f,
            0.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
    };

    public static final float TEXTURE_ROTATED_270[] = {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
    };

    public static final float VERTEX_MATRIX[] = {
            -1.0f, -1.0f,
            1.0f, -1.0f,
            -1.0f, 1.0f,
            1.0f, 1.0f,
    };

    public static final float VERTEX_CUBE[] = {
            -1.0f, -1.0f,
            1.0f, -1.0f,
            -1.0f, 1.0f,
            1.0f, 1.0f,
    };

    public static final float[] TEXTURE_MATRIX = {
            1.0f, 0.0f,
            0.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
    };

    private TextureRotationUtil() {}

    public static float[] getRotation(final int rotation, final boolean flipHorizontal,
                                      final boolean flipVertical) {
        float[] rotatedTex;
        switch (rotation) {
            case 90:
                rotatedTex = TEXTURE_ROTATED_90;
                break;
            case 180:
                rotatedTex = TEXTURE_ROTATED_180;
                break;
            case 270:
                rotatedTex = TEXTURE_ROTATED_270;
                break;
            default:
                rotatedTex = TEXTURE_ROTATED_0;
                break;
        }

        if (flipHorizontal) {
            rotatedTex = new float[] {
                flip(rotatedTex[0]), rotatedTex[1],
                flip(rotatedTex[2]), rotatedTex[3],
                flip(rotatedTex[4]), rotatedTex[5],
                flip(rotatedTex[6]), rotatedTex[7],
            };
        }

        if (flipVertical) {
            rotatedTex = new float[] {
                rotatedTex[0], flip(rotatedTex[1]),
                rotatedTex[2], flip(rotatedTex[3]),
                rotatedTex[4], flip(rotatedTex[5]),
                rotatedTex[6], flip(rotatedTex[7]),
            };
        }

        return rotatedTex;
    }

    public static float[] getPhotoRotation(final int rotation, final boolean flipHorizontal,
                                           final boolean flipVertical) {
        float[] rotatedTex;
        switch (rotation) {
            case 90:
                rotatedTex = TEXTURE_ROTATED_90;
                break;
            case 180:
                rotatedTex = TEXTURE_ROTATED_180;
                break;
            case 270:
                rotatedTex = TEXTURE_ROTATED_270;
                break;
            case 0:
            default:
                rotatedTex = TEXTURE_NO_ROTATION;
                break;
        }

        if (flipHorizontal) {
            rotatedTex = new float[]{
                    flip(rotatedTex[0]), rotatedTex[1],
                    flip(rotatedTex[2]), rotatedTex[3],
                    flip(rotatedTex[4]), rotatedTex[5],
                    flip(rotatedTex[6]), rotatedTex[7],
            };
        }

        if (flipVertical) {
            rotatedTex = new float[]{
                    rotatedTex[0], flip(rotatedTex[1]),
                    rotatedTex[2], flip(rotatedTex[3]),
                    rotatedTex[4], flip(rotatedTex[5]),
                    rotatedTex[6], flip(rotatedTex[7]),
            };
        }

        return rotatedTex;
    }

    public static float[] getVideoRotation(final int rotation, final boolean flipHorizontal,
                                           final boolean flipVertical) {
        float[] rotatedTex;
        switch (rotation) {
            case 90:
                rotatedTex = TEXTURE_ROTATED_90;
                break;
            case 180:
                rotatedTex = TEXTURE_ROTATED_180;
                break;
            case 270:
                rotatedTex = TEXTURE_ROTATED_270;
                break;
            case 0:
            case 360:
            default:
                rotatedTex = TEXTURE_NO_ROTATION;
                break;
        }

        if (flipHorizontal) {
            rotatedTex = new float[]{
                    flip(rotatedTex[0]), rotatedTex[1],
                    flip(rotatedTex[2]), rotatedTex[3],
                    flip(rotatedTex[4]), rotatedTex[5],
                    flip(rotatedTex[6]), rotatedTex[7],
            };
        }

        if (flipVertical) {
            rotatedTex = new float[]{
                    rotatedTex[0], flip(rotatedTex[1]),
                    rotatedTex[2], flip(rotatedTex[3]),
                    rotatedTex[4], flip(rotatedTex[5]),
                    rotatedTex[6], flip(rotatedTex[7]),
            };
        }

        return rotatedTex;
    }

    private static float flip(final float i) {
        return (i == 0.0f) ? 1.0f : 0.0f;
    }
}
