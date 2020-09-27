package io.agora.api.example.common.model;

import io.agora.rtc.IRtcEngineEventHandler.*;

public class StatisticsInfo {
    private LocalVideoStats localVideoStats;
    private LocalAudioStats localAudioStats;
    private RemoteVideoStats remoteVideoStats;
    private RemoteAudioStats remoteAudioStats;
    private RtcStats rtcStats;

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

}
