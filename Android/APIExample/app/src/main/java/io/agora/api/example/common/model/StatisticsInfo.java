package io.agora.api.example.common.model;


import io.agora.rtc2.IRtcEngineEventHandler;

/**
 * The type Statistics info.
 */
public class StatisticsInfo {
    private IRtcEngineEventHandler.LocalVideoStats localVideoStats = new IRtcEngineEventHandler.LocalVideoStats();
    private IRtcEngineEventHandler.LocalAudioStats localAudioStats = new IRtcEngineEventHandler.LocalAudioStats();
    private IRtcEngineEventHandler.RemoteVideoStats remoteVideoStats = new IRtcEngineEventHandler.RemoteVideoStats();
    private IRtcEngineEventHandler.RemoteAudioStats remoteAudioStats = new IRtcEngineEventHandler.RemoteAudioStats();
    private IRtcEngineEventHandler.RtcStats rtcStats = new IRtcEngineEventHandler.RtcStats();
    private int quality;
    private IRtcEngineEventHandler.LastmileProbeResult lastMileProbeResult;

    /**
     * Sets local video stats.
     *
     * @param localVideoStats the local video stats
     */
    public void setLocalVideoStats(IRtcEngineEventHandler.LocalVideoStats localVideoStats) {
        this.localVideoStats = localVideoStats;
    }

    /**
     * Sets local audio stats.
     *
     * @param localAudioStats the local audio stats
     */
    public void setLocalAudioStats(IRtcEngineEventHandler.LocalAudioStats localAudioStats) {
        this.localAudioStats = localAudioStats;
    }

    /**
     * Sets remote video stats.
     *
     * @param remoteVideoStats the remote video stats
     */
    public void setRemoteVideoStats(IRtcEngineEventHandler.RemoteVideoStats remoteVideoStats) {
        this.remoteVideoStats = remoteVideoStats;
    }

    /**
     * Sets remote audio stats.
     *
     * @param remoteAudioStats the remote audio stats
     */
    public void setRemoteAudioStats(IRtcEngineEventHandler.RemoteAudioStats remoteAudioStats) {
        this.remoteAudioStats = remoteAudioStats;
    }

    /**
     * Sets rtc stats.
     *
     * @param rtcStats the rtc stats
     */
    public void setRtcStats(IRtcEngineEventHandler.RtcStats rtcStats) {
        this.rtcStats = rtcStats;
    }

    /**
     * Gets local video stats.
     *
     * @return the local video stats
     */
    public String getLocalVideoStats() {
        StringBuilder builder = new StringBuilder();
        return builder
                .append("" + localVideoStats.encodedFrameWidth)
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
                .append(rtcStats.txPacketLossRate)
                .append("%")
                .toString();
    }

    /**
     * Gets remote video stats.
     *
     * @return the remote video stats
     */
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

    /**
     * Sets last mile quality.
     *
     * @param quality the quality
     */
    public void setLastMileQuality(int quality) {
        this.quality = quality;
    }

    /**
     * Get last mile quality string.
     *
     * @return the string
     */
    public String getLastMileQuality() {
        switch (quality) {
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

    /**
     * Gets last mile result.
     *
     * @return the last mile result
     */
    public String getLastMileResult() {
        if (lastMileProbeResult == null) {
            return null;
        }
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

    /**
     * Sets last mile probe result.
     *
     * @param lastmileProbeResult the lastmile probe result
     */
    public void setLastMileProbeResult(IRtcEngineEventHandler.LastmileProbeResult lastmileProbeResult) {
        this.lastMileProbeResult = lastmileProbeResult;
    }

}
