package io.agora.api.example.common.model;

import io.agora.rtc.IRtcEngineEventHandler.LastmileProbeResult;
import io.agora.rtc.IRtcEngineEventHandler.LocalAudioStats;
import io.agora.rtc.IRtcEngineEventHandler.LocalVideoStats;
import io.agora.rtc.IRtcEngineEventHandler.RemoteAudioStats;
import io.agora.rtc.IRtcEngineEventHandler.RemoteVideoStats;
import io.agora.rtc.IRtcEngineEventHandler.RtcStats;

public class StatisticsInfo {
    private LocalVideoStats localVideoStats;
    private LocalAudioStats localAudioStats;
    private RemoteVideoStats remoteVideoStats;
    private RemoteAudioStats remoteAudioStats;
    private RtcStats rtcStats;
    private int quality;
    private LastmileProbeResult lastMileProbeResult;

    public void setLocalVideoStats(LocalVideoStats localVideoStats) {
        this.localVideoStats = localVideoStats;
    }

    public void setLocalAudioStats(LocalAudioStats localAudioStats) {
        this.localAudioStats = localAudioStats;
    }

    public void setRemoteVideoStats(RemoteVideoStats remoteVideoStats) {
        this.remoteVideoStats = remoteVideoStats;
    }

    public void setRemoteAudioStats(RemoteAudioStats remoteAudioStats) {
        this.remoteAudioStats = remoteAudioStats;
    }

    public void setRtcStats(RtcStats rtcStats) {
        this.rtcStats = rtcStats;
    }

    public String getLocalVideoStats() {
        if(localVideoStats == null || localAudioStats == null){
            return "";
        }
        StringBuilder builder = new StringBuilder();
        return builder
                .append(localVideoStats.encodedFrameWidth)
                .append("×")
                .append(localVideoStats.encodedFrameHeight)
                .append(",")
                .append(localVideoStats.encoderOutputFrameRate)
                .append("fps")
                .append("\n")
                .append("LM Delay: ")
                .append(rtcStats.lastmileDelay)
                .append("ms")
                .append("\n")
                .append("VSend: ")
                .append(localVideoStats.sentBitrate)
                .append("kbps")
                .append("\n")
                .append("ASend: ")
                .append(localAudioStats.sentBitrate)
                .append("kbps")
                .append("\n")
                .append("CPU: ")
                .append(rtcStats.cpuAppUsage)
                .append("%/")
                .append(rtcStats.cpuTotalUsage)
                .append("%/")
                .append("\n")
                .append("VSend Loss: ")
                .append(localVideoStats.txPacketLossRate)
                .append("%")
                .append("\n")
                .append("ASend Loss: ")
                .append(localAudioStats.txPacketLossRate)
                .append("%")
                .toString();
    }

    public String getRemoteVideoStats() {
        if(remoteAudioStats == null || remoteVideoStats == null){
            return "";
        }
        StringBuilder builder = new StringBuilder();
        return builder
                .append(remoteVideoStats.width)
                .append("×")
                .append(remoteVideoStats.height)
                .append(",")
                .append(remoteVideoStats.rendererOutputFrameRate)
                .append("fps")
                .append("\n")
                .append("VRecv: ")
                .append(remoteVideoStats.receivedBitrate)
                .append("kbps")
                .append("\n")
                .append("ARecv: ")
                .append(remoteAudioStats.receivedBitrate)
                .append("kbps")
                .append("\n")
                .append("VLoss: ")
                .append(remoteVideoStats.packetLossRate)
                .append("%")
                .append("\n")
                .append("ALoss: ")
                .append(remoteAudioStats.audioLossRate)
                .append("%")
                .append("\n")
                .append("AQuality: ")
                .append(remoteAudioStats.quality)
                .toString();
    }

    public void setLastMileQuality(int quality) {
        this.quality = quality;
    }

    public String getLastMileQuality(){
        switch (quality){
            case 1:
                return "EXCELLENT";
            case 2:
                return "GOOD";
            case 3:
                return "POOR";
            case 4:
                return "BAD";
            case 5:
                return "VERY BAD";
            case 6:
                return "DOWN";
            case 7:
                return "UNSUPPORTED";
            case 8:
                return "DETECTING";
            default:
                return "UNKNOWN";
        }
    }

    public String getLastMileResult() {
        if(lastMileProbeResult == null)
            return null;
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append("Rtt: ")
                .append(lastMileProbeResult.rtt)
                .append("ms")
                .append("\n")
                .append("DownlinkAvailableBandwidth: ")
                .append(lastMileProbeResult.downlinkReport.availableBandwidth)
                .append("Kbps")
                .append("\n")
                .append("DownlinkJitter: ")
                .append(lastMileProbeResult.downlinkReport.jitter)
                .append("ms")
                .append("\n")
                .append("DownlinkLoss: ")
                .append(lastMileProbeResult.downlinkReport.packetLossRate)
                .append("%")
                .append("\n")
                .append("UplinkAvailableBandwidth: ")
                .append(lastMileProbeResult.uplinkReport.availableBandwidth)
                .append("Kbps")
                .append("\n")
                .append("UplinkJitter: ")
                .append(lastMileProbeResult.uplinkReport.jitter)
                .append("ms")
                .append("\n")
                .append("UplinkLoss: ")
                .append(lastMileProbeResult.uplinkReport.packetLossRate)
                .append("%");
        return stringBuilder.toString();
    }

    public void setLastMileProbeResult(LastmileProbeResult lastmileProbeResult) {
        this.lastMileProbeResult = lastmileProbeResult;
    }

}
