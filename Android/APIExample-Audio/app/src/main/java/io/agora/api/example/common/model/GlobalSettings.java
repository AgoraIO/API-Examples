package io.agora.api.example.common.model;

import android.text.TextUtils;

import java.util.ArrayList;

import io.agora.rtc2.Constants;
import io.agora.rtc2.RtcEngineConfig;
import io.agora.rtc2.proxy.LocalAccessPointConfiguration;

public class GlobalSettings {
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


    public String getAreaCodeStr() {
        return areaCodeStr;
    }

    public void setAreaCodeStr(String areaCodeStr) {
        this.areaCodeStr = areaCodeStr;
    }

    public int getAreaCode(){
        if("CN".equals(areaCodeStr)){
            return RtcEngineConfig.AreaCode.AREA_CODE_CN;
        }
        else if("NA".equals(areaCodeStr)){
            return RtcEngineConfig.AreaCode.AREA_CODE_NA;
        }
        else if("EU".equals(areaCodeStr)){
            return RtcEngineConfig.AreaCode.AREA_CODE_EU;
        }
        else if("AS".equals(areaCodeStr)){
            return RtcEngineConfig.AreaCode.AREA_CODE_AS;
        }
        else if("JP".equals(areaCodeStr)){
            return RtcEngineConfig.AreaCode.AREA_CODE_JP;
        }
        else if("IN".equals(areaCodeStr)){
            return RtcEngineConfig.AreaCode.AREA_CODE_IN;
        }
        else{
            return RtcEngineConfig.AreaCode.AREA_CODE_GLOB;
        }
    }
}
