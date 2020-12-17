package io.agora.api.example.common.model;

import android.util.Log;

import java.lang.reflect.Field;

import io.agora.rtc.video.VideoEncoderConfiguration;

import static io.agora.rtc.video.VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15;
import static io.agora.rtc.video.VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_ADAPTIVE;
import static io.agora.rtc.video.VideoEncoderConfiguration.VD_640x360;

public class GlobalSettings {
    private String videoEncodingDimension;
    private String videoEncodingFrameRate;
    private String videoEncodingOrientation;

    public String getVideoEncodingDimension() {
        if(videoEncodingDimension == null)
            return "VD_640x360";
        else
            return videoEncodingDimension;
    }

    public VideoEncoderConfiguration.VideoDimensions getVideoEncodingDimensionObject() {
        if(videoEncodingDimension == null)
            return VD_640x360;
        VideoEncoderConfiguration.VideoDimensions value = VD_640x360;
        try {
            Field tmp = VideoEncoderConfiguration.class.getDeclaredField(videoEncodingDimension);
            tmp.setAccessible(true);
            value = (VideoEncoderConfiguration.VideoDimensions) tmp.get(null);
        } catch (NoSuchFieldException e) {
            Log.e("Field", "Can not find field " + videoEncodingDimension);
        } catch (IllegalAccessException e) {
            Log.e("Field", "Could not access field " + videoEncodingDimension);
        }
        return value;
    }

    public void setVideoEncodingDimension(String videoEncodingDimension) {
        this.videoEncodingDimension = videoEncodingDimension;
    }

    public String getVideoEncodingFrameRate() {
        if(videoEncodingFrameRate == null)
            return FRAME_RATE_FPS_15.name();
        else
            return videoEncodingFrameRate;
    }

    public void setVideoEncodingFrameRate(String videoEncodingFrameRate) {
        this.videoEncodingFrameRate = videoEncodingFrameRate;
    }

    public String getVideoEncodingOrientation() {
        if(videoEncodingOrientation == null)
            return ORIENTATION_MODE_ADAPTIVE.name();
        else
            return videoEncodingOrientation;
    }

    public void setVideoEncodingOrientation(String videoEncodingOrientation) {
        this.videoEncodingOrientation = videoEncodingOrientation;
    }
}
