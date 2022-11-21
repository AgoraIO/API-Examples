package com.bytedance.labcv.common.model;

import com.bytedance.labcv.effectsdk.BytedEffectConstants;

/**
 * Created  on 2021/5/14 4:08 PM
 */
public class ProcessInput {
    private int texture;
    private int width;
    private int height;
    private byte[] buffer;
    private int format;
    public BytedEffectConstants.Rotation sensorRotation;


    public int getTexture() {
        return texture;
    }

    public void setTexture(int texture) {
        this.texture = texture;
    }

    public int getWidth() {
        return width;
    }

    public void setWidth(int width) {
        this.width = width;
    }

    public int getHeight() {
        return height;
    }

    public void setHeight(int height) {
        this.height = height;
    }

    public byte[] getBuffer() { return buffer; }

    public void setBuffer(byte[] buffer) { this.buffer = buffer; }

    public int getFormat() { return format; }

    public void setFormat(int format) { this.format = format; }

    public BytedEffectConstants.Rotation getSensorRotation() {
        return sensorRotation;
    }

    public void setSensorRotation(BytedEffectConstants.Rotation sensorRotation) {
        this.sensorRotation = sensorRotation;
    }
}
