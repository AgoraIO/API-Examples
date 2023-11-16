package io.agora.api.example.common.model;

import static io.agora.rtc2.video.VideoEncoderConfiguration.FRAME_RATE.FRAME_RATE_FPS_15;
import static io.agora.rtc2.video.VideoEncoderConfiguration.ORIENTATION_MODE.ORIENTATION_MODE_ADAPTIVE;
import static io.agora.rtc2.video.VideoEncoderConfiguration.VD_960x540;

import android.text.TextUtils;
import android.util.Log;

import java.lang.reflect.Field;
import java.util.ArrayList;

import io.agora.rtc2.Constants;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;
import io.agora.rtc2.video.VideoEncoderConfiguration;

/**
 * The type Global settings.
 */
public class GlobalSettings {
    private String videoEncodingDimension;
    private String videoEncodingFrameRate;
    private String videoEncodingOrientation;
    private String areaCodeStr = "GLOBAL";

    /**
     * The Private cloud ip.
     */
// private cloud config
    public String privateCloudIp = "";
    /**
     * The Private cloud log report enable.
     */
    public boolean privateCloudLogReportEnable = false;
    /**
     * The Private cloud log server domain.
     */
    public String privateCloudLogServerDomain = "";
    /**
     * The Private cloud log server port.
     */
    public int privateCloudLogServerPort = 80;
    /**
     * The Private cloud log server path.
     */
    public String privateCloudLogServerPath = "";
    /**
     * The Private cloud use https.
     */
    public boolean privateCloudUseHttps = false;
    // public String privateCloudIp = "10.62.0.85";
    // public boolean privateCloudLogReportEnable = true;
    // public String privateCloudLogServerDomain = "10.72.0.29";
    // public int privateCloudLogServerPort = 442;
    // public String privateCloudLogServerPath = "/kafka/log/upload/v1";
    // public boolean privateCloudUseHttps = true;

    /**
     * Gets video encoding dimension.
     *
     * @return the video encoding dimension
     */
    public String getVideoEncodingDimension() {
        if (videoEncodingDimension == null) {
            return "VD_960x540";
        } else {
            return videoEncodingDimension;
        }
    }

    /**
     * Gets private cloud config.
     *
     * @return the private cloud config
     */
    public LocalAccessPointConfiguration getPrivateCloudConfig() {
        LocalAccessPointConfiguration config = new LocalAccessPointConfiguration();
        if (TextUtils.isEmpty(privateCloudIp)) {
            return null;
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

    /**
     * Gets video encoding dimension object.
     *
     * @return the video encoding dimension object
     */
    public VideoEncoderConfiguration.VideoDimensions getVideoEncodingDimensionObject() {
        VideoEncoderConfiguration.VideoDimensions value = VD_960x540;
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

    /**
     * Sets video encoding dimension.
     *
     * @param videoEncodingDimension the video encoding dimension
     */
    public void setVideoEncodingDimension(String videoEncodingDimension) {
        this.videoEncodingDimension = videoEncodingDimension;
    }

    /**
     * Gets video encoding frame rate.
     *
     * @return the video encoding frame rate
     */
    public String getVideoEncodingFrameRate() {
        if (videoEncodingFrameRate == null) {
            return FRAME_RATE_FPS_15.name();
        } else {
            return videoEncodingFrameRate;
        }
    }

    /**
     * Sets video encoding frame rate.
     *
     * @param videoEncodingFrameRate the video encoding frame rate
     */
    public void setVideoEncodingFrameRate(String videoEncodingFrameRate) {
        this.videoEncodingFrameRate = videoEncodingFrameRate;
    }

    /**
     * Gets video encoding orientation.
     *
     * @return the video encoding orientation
     */
    public String getVideoEncodingOrientation() {
        if (videoEncodingOrientation == null) {
            return ORIENTATION_MODE_ADAPTIVE.name();
        } else {
            return videoEncodingOrientation;
        }

    }

    /**
     * Sets video encoding orientation.
     *
     * @param videoEncodingOrientation the video encoding orientation
     */
    public void setVideoEncodingOrientation(String videoEncodingOrientation) {
        this.videoEncodingOrientation = videoEncodingOrientation;
    }

    /**
     * Gets area code str.
     *
     * @return the area code str
     */
    public String getAreaCodeStr() {
        return areaCodeStr;
    }

    /**
     * Sets area code str.
     *
     * @param areaCodeStr the area code str
     */
    public void setAreaCodeStr(String areaCodeStr) {
        this.areaCodeStr = areaCodeStr;
    }

    /**
     * Gets area code.
     *
     * @return the area code
     */
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
