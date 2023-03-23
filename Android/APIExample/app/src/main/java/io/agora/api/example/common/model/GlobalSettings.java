package io.agora.api.example.common.model;

import static io.agora.rtc2.video.VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15;
import static io.agora.rtc2.video.VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_ADAPTIVE;
import static io.agora.rtc2.video.VideoEncoderConfiguration.VD_640x480;

import android.text.TextUtils;
import android.util.Log;

import java.lang.reflect.Field;
import java.util.ArrayList;

import io.agora.rtc2.Constants;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;
import io.agora.rtc2.video.VideoEncoderConfiguration;

public class GlobalSettings {
    private String videoEncodingDimension;
    private String videoEncodingFrameRate;
    private String videoEncodingOrientation;
    private String areaCodeStr = "GLOBAL";

    // private cloud config
    public String privateCloudIp = "";
    public boolean privateCloudLogReportEnable = false;
    public String privateCloudLogServerDomain = "";
    public int privateCloudLogServerPort = 80;
    public String privateCloudLogServerPath = "";
    public boolean privateCloudUseHttps = false;
    // public String privateCloudIp = "10.62.0.85";
    // public boolean privateCloudLogReportEnable = true;
    // public String privateCloudLogServerDomain = "10.72.0.29";
    // public int privateCloudLogServerPort = 442;
    // public String privateCloudLogServerPath = "/kafka/log/upload/v1";
    // public boolean privateCloudUseHttps = true;

    public String getVideoEncodingDimension() {
        if (videoEncodingDimension == null)
            return "VD_640x480";
        else
            return videoEncodingDimension;
    }

    public LocalAccessPointConfiguration getPrivateCloudConfig() {
        LocalAccessPointConfiguration config = new LocalAccessPointConfiguration();
        if(TextUtils.isEmpty(privateCloudIp)){
            return config;
        }
        config.ipList = new ArrayList<>();
        config.ipList.add(privateCloudIp);
        config.domainList = new ArrayList<>();
        config.mode = Constants.LOCAL_RPOXY_LOCAL_ONLY;
        if (privateCloudLogReportEnable) {
            LocalAccessPointConfiguration.AdvancedConfigInfo advancedConfig = new LocalAccessPointConfiguration.AdvancedConfigInfo();
            LocalAccessPointConfiguration.LogUploadServerInfo logUploadServer = new LocalAccessPointConfiguration.LogUploadServerInfo();
            logUploadServer.serverDomain = privateCloudLogServerDomain;
            logUploadServer.serverPort = privateCloudLogServerPort;
            logUploadServer.serverPath = privateCloudLogServerPath;
            logUploadServer.serverHttps = privateCloudUseHttps;

            advancedConfig.logUploadServer = logUploadServer;
            config.advancedConfig = advancedConfig;
        }
        return config;
    }

    public VideoEncoderConfiguration.VideoDimensions getVideoEncodingDimensionObject() {
        VideoEncoderConfiguration.VideoDimensions value = VD_640x480;
        try {
            Field tmp = VideoEncoderConfiguration.class.getDeclaredField(getVideoEncodingDimension());
            tmp.setAccessible(true);
            value = (VideoEncoderConfiguration.VideoDimensions) tmp.get(null);
        } catch (NoSuchFieldException e) {
            Log.e("Field", "Can not find field " + getVideoEncodingDimension());
        } catch (IllegalAccessException e) {
            Log.e("Field", "Could not access field " + getVideoEncodingDimension());
        }
        return value;
    }

    public void setVideoEncodingDimension(String videoEncodingDimension) {
        this.videoEncodingDimension = videoEncodingDimension;
    }

    public String getVideoEncodingFrameRate() {
        if (videoEncodingFrameRate == null)
            return FRAME_RATE_FPS_15.name();
        else
            return videoEncodingFrameRate;
    }

    public void setVideoEncodingFrameRate(String videoEncodingFrameRate) {
        this.videoEncodingFrameRate = videoEncodingFrameRate;
    }

    public String getVideoEncodingOrientation() {
        if (videoEncodingOrientation == null)
            return ORIENTATION_MODE_ADAPTIVE.name();
        else
            return videoEncodingOrientation;
    }

    public void setVideoEncodingOrientation(String videoEncodingOrientation) {
        this.videoEncodingOrientation = videoEncodingOrientation;
    }

    public String getAreaCodeStr() {
        return areaCodeStr;
    }

    public void setAreaCodeStr(String areaCodeStr) {
        this.areaCodeStr = areaCodeStr;
    }

    public int getAreaCode() {
        if ("CN".equals(areaCodeStr)) {
            return RtcEngineConfig.AreaCode.AREA_CODE_CN;
        } else if ("NA".equals(areaCodeStr)) {
            return RtcEngineConfig.AreaCode.AREA_CODE_NA;
        } else if ("EU".equals(areaCodeStr)) {
            return RtcEngineConfig.AreaCode.AREA_CODE_EU;
        } else if ("AS".equals(areaCodeStr)) {
            return RtcEngineConfig.AreaCode.AREA_CODE_AS;
        } else if ("JP".equals(areaCodeStr)) {
            return RtcEngineConfig.AreaCode.AREA_CODE_JP;
        } else if ("IN".equals(areaCodeStr)) {
            return RtcEngineConfig.AreaCode.AREA_CODE_IN;
        } else {
            return RtcEngineConfig.AreaCode.AREA_CODE_GLOB;
        }
    }
}
