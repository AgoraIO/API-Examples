//package io.agora;
//
//import android.util.Log;
//import io.agora.mediaplayer.AgoraMediaPlayerKit;
//import io.agora.mediaplayer.AudioFrameObserver;
//import io.agora.mediaplayer.VideoFrameObserver;
//import io.agora.mediaplayer.data.AudioFrame;
//import io.agora.mediaplayer.data.VideoFrame;
//import io.agora.rtc.IRtcEngineEventHandler;
//import io.agora.rtc.RtcEngine;
//import io.agora.rtc.IRtcEngineEventHandler.RtcStats;
//import io.agora.rtc.mediaio.MediaIO.PixelFormat;
//import java.nio.ByteBuffer;
//
//public class RtcChannelPublishHelper extends IRtcEngineEventHandler implements AudioFrameObserver, VideoFrameObserver {
//    private static final String TAG = "RtcChannelPublishHelper";
//    private static volatile RtcChannelPublishHelper rtcChannelPublishHelper = null;
//    private RtcEngine mRtcEngine;
//    public boolean enablePushVideoToRtc = false;
//    public boolean enablePushAudioToRtc = false;
//    private AgoraMediaPlayerKit agoraMediaPlayerKit = null;
//    private MediaVideoSource mediaVideoSource;
//    private static final int MAX_VOLUME = 400;
//    private static final int ERROR_CODE = -1;
//    private int rotation = 0;
//
//    private RtcChannelPublishHelper() {
//    }
//
//    public static RtcChannelPublishHelper getInstance() {
//        if (rtcChannelPublishHelper == null) {
//            Class var0 = RtcChannelPublishHelper.class;
//            synchronized(RtcChannelPublishHelper.class) {
//                rtcChannelPublishHelper = new RtcChannelPublishHelper();
//                Log.i("RtcChannelPublishHelper", "getInstance");
//            }
//        }
//
//        return rtcChannelPublishHelper;
//    }
//
//    public int attachPlayerToRtc(AgoraMediaPlayerKit agoraMediaPlayerKit, RtcEngine rtcEngine) {
//        this.mRtcEngine = rtcEngine;
//        this.agoraMediaPlayerKit = agoraMediaPlayerKit;
//        this.enablePushVideoToRtc = false;
//        this.enablePushAudioToRtc = false;
//        this.nativeEnablePushAudioToRtc(false);
//        if (this.mRtcEngine != null && this.agoraMediaPlayerKit != null) {
//            this.nativeEnablePushToRtc();
//            Log.i("RtcChannelPublishHelper", "attachPlayerToRtc");
//            return 0;
//        } else {
//            return -1;
//        }
//    }
//
//    public int publishVideo() {
//        if (this.mRtcEngine != null && this.agoraMediaPlayerKit != null) {
//            if (this.mediaVideoSource == null) {
//                this.mediaVideoSource = new MediaVideoSource();
//                this.mRtcEngine.setVideoSource(this.mediaVideoSource);
//            }
//
//            this.enablePushVideoToRtc = true;
//            this.agoraMediaPlayerKit.registerVideoFrameObserver(this);
//            Log.i("RtcChannelPublishHelper", "publishVideo");
//            return 0;
//        } else {
//            return -1;
//        }
//    }
//
//    public int unpublishVideo() {
//        if (this.mRtcEngine != null && this.agoraMediaPlayerKit != null) {
//            this.enablePushVideoToRtc = false;
//            this.mediaVideoSource = null;
//            this.agoraMediaPlayerKit.unregisterVideoFrameObserver(this);
//            Log.i("RtcChannelPublishHelper", "unpublishVideo");
//            return 0;
//        } else {
//            return -1;
//        }
//    }
//
//    public int publishAudio() {
//        if (this.mRtcEngine != null && this.agoraMediaPlayerKit != null) {
//            this.enablePushAudioToRtc = true;
//            this.nativeEnablePushAudioToRtc(true);
//            this.agoraMediaPlayerKit.registerAudioFrameObserver(this);
//            Log.i("RtcChannelPublishHelper", "publishAudio");
//            return 0;
//        } else {
//            return -1;
//        }
//    }
//
//    public int unpublishAudio() {
//        if (this.mRtcEngine != null && this.agoraMediaPlayerKit != null) {
//            this.enablePushAudioToRtc = false;
//            this.nativeEnablePushAudioToRtc(false);
//            this.agoraMediaPlayerKit.unregisterAudioFrameObserver(this);
//            Log.i("RtcChannelPublishHelper", "unpublishAudio");
//            return 0;
//        } else {
//            return -1;
//        }
//    }
//
//    private int adjustPublishSignalVolume(int volume) {
//        if (this.mRtcEngine != null && this.agoraMediaPlayerKit != null) {
//            float volumeFloat = (float)volume / 400.0F;
//            if (volumeFloat > 1.0F) {
//                volumeFloat = 1.0F;
//            }
//
//            this.adjustPublishSignalVolume(volumeFloat);
//            return 0;
//        } else {
//            return -1;
//        }
//    }
//
//    private int adjustPublishVoiceVolume(int volume) {
//        if (this.mRtcEngine != null && this.agoraMediaPlayerKit != null) {
//            float volumeFloat = (float)volume / 400.0F;
//            if (volumeFloat > 1.0F) {
//                volumeFloat = 1.0F;
//            }
//
//            this.adjustPublishVoiceVolume(volumeFloat);
//            return 0;
//        } else {
//            return -1;
//        }
//    }
//
//    public int adjustPublishSignalVolume(int microphoneVolume, int movieVolume) {
//        if (this.mRtcEngine != null && this.agoraMediaPlayerKit != null) {
//            this.adjustPublishSignalVolume(movieVolume);
//            this.adjustPublishVoiceVolume(microphoneVolume);
//            Log.i("RtcChannelPublishHelper", "adjustPublishSignalVolume:" + microphoneVolume + "movieVolume:" + movieVolume);
//            return 0;
//        } else {
//            return -1;
//        }
//    }
//
//    public void setRotation(int rotation) {
//        this.rotation = rotation;
//    }
//
//    public int detachPlayerFromRtc() {
//        if (this.mRtcEngine != null && this.agoraMediaPlayerKit != null) {
//            this.agoraMediaPlayerKit.unregisterAudioFrameObserver(this);
//            this.agoraMediaPlayerKit.unregisterVideoFrameObserver(this);
//            this.enablePushVideoToRtc = false;
//            this.enablePushAudioToRtc = false;
//            this.rotation = 0;
//            this.nativeEnablePushAudioToRtc(false);
//            this.nativeEnableLocalPlayoutVolume(false);
//            this.mediaVideoSource = null;
//            Log.i("RtcChannelPublishHelper", "detachPlayerFromRtc");
//            return 0;
//        } else {
//            return -1;
//        }
//    }
//
//    public void release() {
//        if (this.mRtcEngine != null && this.agoraMediaPlayerKit != null) {
//            this.agoraMediaPlayerKit.unregisterAudioFrameObserver(this);
//            this.agoraMediaPlayerKit.unregisterVideoFrameObserver(this);
//            this.enablePushVideoToRtc = false;
//            this.enablePushAudioToRtc = false;
//            this.nativeEnablePushAudioToRtc(false);
//            this.nativeEnableLocalPlayoutVolume(false);
//            this.adjustPublishSignalVolume(400, 0);
//            this.mediaVideoSource = null;
//            this.rotation = 0;
//            this.agoraMediaPlayerKit = null;
//            this.mRtcEngine = null;
//            this.nativeRelease();
//            Log.i("RtcChannelPublishHelper", "release");
//        }
//    }
//
//    private void onVideoData(ByteBuffer videoBuffer, byte[] bytes, int format, int stride, int height, int width, int rotation, long timestamp) {
//        if (this.mediaVideoSource == null) {
//            this.mediaVideoSource = new MediaVideoSource();
//            this.mRtcEngine.setVideoSource(this.mediaVideoSource);
//        }
//
//        if (this.enablePushVideoToRtc && this.mediaVideoSource != null && this.mediaVideoSource.getFrameConsumer() != null && this.mediaVideoSource.isEnablePushToRtc()) {
//            this.mediaVideoSource.getFrameConsumer().consumeByteArrayFrame(bytes, PixelFormat.I420.intValue(), stride, height, this.rotation, timestamp);
//        }
//
//    }
//
//    private void onAudioData(ByteBuffer audioBuffer, byte[] bytes, int sampleRataHz, int bytesPerSample, int channelNums, int samplesPerChannel, long timestamp) {
//        if (this.enablePushAudioToRtc) {
//            this.nativeOnAudioData(audioBuffer, sampleRataHz, bytesPerSample, channelNums, samplesPerChannel, timestamp);
//        }
//
//    }
//
//    public void enableLocalPlayoutVolume(boolean enable) {
//        if (this.mRtcEngine != null && this.agoraMediaPlayerKit != null) {
//            this.agoraMediaPlayerKit.mute(enable);
//            if (enable) {
//                this.agoraMediaPlayerKit.registerAudioFrameObserver(this);
//                this.mRtcEngine.setParameters("{\"che.audio.enable.aec\":true}");
//            } else {
//                this.agoraMediaPlayerKit.unregisterAudioFrameObserver(this);
//                this.mRtcEngine.setParameters("{\"che.audio.enable.aec\":false}");
//            }
//
//            this.nativeEnableLocalPlayoutVolume(enable);
//        }
//    }
//
//    public void onFrame(AudioFrame audioFrame) {
//        this.onAudioData(audioFrame.buffer, audioFrame.bytes, audioFrame.sampleRataHz, audioFrame.bytesPerSample, audioFrame.channelNums, audioFrame.samplesPerChannel, audioFrame.timestamp);
//    }
//
//    public void onFrame(VideoFrame videoFrame) {
//        this.onVideoData(videoFrame.buffer, videoFrame.bytes, videoFrame.format, videoFrame.stride, videoFrame.height, videoFrame.width, videoFrame.rotation, videoFrame.timestamp);
//    }
//
//    private native int nativeOnAudioData(ByteBuffer var1, int var2, int var3, int var4, int var5, long var6);
//
//    private native int nativeEnablePushToRtc();
//
//    private native int nativeEnablePushAudioToRtc(boolean var1);
//
//    private native int nativeRelease();
//
//    private native int adjustPublishSignalVolume(float var1);
//
//    private native int nativeEnableLocalPlayoutVolume(boolean var1);
//
//    private native int adjustPublishVoiceVolume(float var1);
//
//    public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
//        if (!this.enablePushVideoToRtc) {
//            this.enablePushVideoToRtc = true;
//        }
//
//        if (!this.enablePushAudioToRtc) {
//            this.enablePushAudioToRtc = true;
//        }
//
//    }
//
//    public void onRejoinChannelSuccess(String channel, int uid, int elapsed) {
//        if (!this.enablePushVideoToRtc) {
//            this.enablePushVideoToRtc = true;
//        }
//
//        if (!this.enablePushAudioToRtc) {
//            this.enablePushAudioToRtc = true;
//        }
//
//    }
//
//    public void onLeaveChannel(RtcStats stats) {
//        this.enablePushVideoToRtc = false;
//        this.enablePushAudioToRtc = false;
//    }
//
//    static {
//        Log.i("RtcChannelPublishHelper", "TJY apm-plugin-agora-rtc-player");
//        System.loadLibrary("apm-plugin-agora-rtc-player");
//    }
//}
