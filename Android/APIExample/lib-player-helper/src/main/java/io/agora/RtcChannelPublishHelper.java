package io.agora;


import android.util.Log;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Locale;

import io.agora.mediaplayer.AgoraMediaPlayerKit;
import io.agora.mediaplayer.AudioFrameObserver;
import io.agora.mediaplayer.VideoFrameObserver;
import io.agora.mediaplayer.data.AudioFrame;
import io.agora.mediaplayer.data.VideoFrame;
import io.agora.mediaplayer.utils.ToolUtil;
import io.agora.rtc.IRtcEngineEventHandler;
import io.agora.rtc.RtcEngine;
import io.agora.rtc.internal.RtcEngineImpl;
import io.agora.rtc.mediaio.AgoraDefaultSource;
import io.agora.rtc.mediaio.MediaIO;
import io.agora.utils.LogUtil;

/**
 * Created by lixiaochen on 2019/12/25.
 */

public class RtcChannelPublishHelper extends IRtcEngineEventHandler implements AudioFrameObserver, VideoFrameObserver {
    private final static String TAG = "RtcChannelPublishHelper";
    private volatile static RtcChannelPublishHelper rtcChannelPublishHelper = null;
    private RtcEngine mRtcEngine;
    //FIXME
    public boolean enablePushVideoToRtc = false;
    public boolean enablePushAudioToRtc = false;
    private AgoraMediaPlayerKit agoraMediaPlayerKit = null;
    private MediaVideoSource mediaVideoSource;
    private final static int MAX_VOLUME = 400;
    private final static int ERROR_CODE = -1;
    private int rotation = 0;
    private RtcChannelPublishHelper() {

    }

    static {
        Log.i(TAG, "TJY apm-plugin-agora-rtc-player");
        System.loadLibrary("apm-plugin-agora-rtc-player");
    }

    public static RtcChannelPublishHelper getInstance() {
        if (rtcChannelPublishHelper == null) {
            synchronized (RtcChannelPublishHelper.class) {
                rtcChannelPublishHelper = new RtcChannelPublishHelper();
                Log.i(TAG, "getInstance");
            }
        }
        return rtcChannelPublishHelper;
    }

    public int attachPlayerToRtc(AgoraMediaPlayerKit agoraMediaPlayerKit, RtcEngine rtcEngine) {
        this.mRtcEngine = rtcEngine;
        this.agoraMediaPlayerKit = agoraMediaPlayerKit;

        // for debug
//        while (mediaVideoSource.isDetatchFormRtc()) {
//            try {
//                Log.i(TAG,"wait to attach");
//                Thread.sleep(20);
//            } catch (Exception e) {
//                LogUtil.e(e.toString());
//            }
//        }
        enablePushVideoToRtc = false;
        enablePushAudioToRtc = false;
        nativeEnablePushAudioToRtc(false);
        if (mRtcEngine == null || this.agoraMediaPlayerKit == null) {
            return ERROR_CODE;
        }
        nativeEnablePushToRtc();
        Log.i(TAG, "attachPlayerToRtc");
        return 0;
    }

    public int publishVideo() {
        if (mRtcEngine == null || this.agoraMediaPlayerKit == null) {
            return ERROR_CODE;
        }
        if (mediaVideoSource == null) {
            mediaVideoSource = new MediaVideoSource();
            mRtcEngine.setVideoSource(mediaVideoSource);
        }
        enablePushVideoToRtc = true;
        this.agoraMediaPlayerKit.registerVideoFrameObserver(this);
        Log.i(TAG, "publishVideo");
        return 0;
    }

    public int unpublishVideo() {
        if (mRtcEngine == null || this.agoraMediaPlayerKit == null) {
            return ERROR_CODE;
        }
        enablePushVideoToRtc = false;
        mediaVideoSource = null;
        this.agoraMediaPlayerKit.unregisterVideoFrameObserver(this);
        Log.i(TAG, "unpublishVideo");
        return 0;
    }

    public int publishAudio() {
        if (mRtcEngine == null || this.agoraMediaPlayerKit == null) {
            return ERROR_CODE;
        }
        enablePushAudioToRtc = true;
        nativeEnablePushAudioToRtc(true);
        this.agoraMediaPlayerKit.registerAudioFrameObserver(this);
        Log.i(TAG, "publishAudio");
        return 0;
    }

    public int unpublishAudio() {
        if (mRtcEngine == null || this.agoraMediaPlayerKit == null) {
            return ERROR_CODE;
        }
        enablePushAudioToRtc = false;
        nativeEnablePushAudioToRtc(false);
        this.agoraMediaPlayerKit.unregisterAudioFrameObserver(this);
        Log.i(TAG, "unpublishAudio");
        return 0;
    }

    private int adjustPublishSignalVolume(int volume) {
        if (mRtcEngine == null || this.agoraMediaPlayerKit == null) {
            return ERROR_CODE;
        }
        float volumeFloat = (float) (volume) / (float) (MAX_VOLUME);
        if (volumeFloat > 1) {
            volumeFloat = 1;
        }
        adjustPublishSignalVolume(volumeFloat);
        return 0;
    }

    private int adjustPublishVoiceVolume(int volume) {
        if (mRtcEngine == null || this.agoraMediaPlayerKit == null) {
            return ERROR_CODE;
        }
        float volumeFloat = (float) (volume) / (float) (MAX_VOLUME);
        if (volumeFloat > 1) {
            volumeFloat = 1;
        }
        adjustPublishVoiceVolume(volumeFloat);
        return 0;
    }

    public int adjustPublishSignalVolume(int microphoneVolume, int movieVolume) {
        if (mRtcEngine == null || this.agoraMediaPlayerKit == null) {
            return ERROR_CODE;
        }
        adjustPublishSignalVolume(movieVolume);
        adjustPublishVoiceVolume(microphoneVolume);
        Log.i(TAG, "adjustPublishSignalVolume:" + microphoneVolume + "movieVolume:" + movieVolume);
        return 0;
    }

    public void setRotation(int rotation) {
        this.rotation = rotation;
    }

    public int detachPlayerFromRtc() {
        if (mRtcEngine == null || this.agoraMediaPlayerKit == null) {
            return ERROR_CODE;
        }
        //mRtcEngine.setVideoSource(new AgoraDefaultSource());
        //adjustPublishSignalVolume(400,0);
        this.agoraMediaPlayerKit.unregisterAudioFrameObserver(this);
        this.agoraMediaPlayerKit.unregisterVideoFrameObserver(this);
        enablePushVideoToRtc = false;
        enablePushAudioToRtc = false;
        this.rotation = 0;
        nativeEnablePushAudioToRtc(false);
        nativeEnableLocalPlayoutVolume(false);
        nativeUnregisterAudioFrameObserver();
        mediaVideoSource = null;
        nativeRelease();
        Log.i(TAG, "detachPlayerFromRtc");
        return 0;
    }

    public void release() {
        if (mRtcEngine == null || this.agoraMediaPlayerKit == null) {
            return;
        }

        this.agoraMediaPlayerKit.unregisterAudioFrameObserver(this);
        this.agoraMediaPlayerKit.unregisterVideoFrameObserver(this);
        enablePushVideoToRtc = false;
        enablePushAudioToRtc = false;
        nativeEnablePushAudioToRtc(false);
        nativeEnableLocalPlayoutVolume(false);
        nativeUnregisterAudioFrameObserver();
        // mRtcEngine.setVideoSource(new AgoraDefaultSource());
        adjustPublishSignalVolume(400, 0);
        mediaVideoSource = null;
        this.rotation = 0;
        this.agoraMediaPlayerKit = null;
        this.mRtcEngine = null;
        nativeRelease();
        Log.i(TAG, "release");
    }

    private void onVideoData(ByteBuffer videoBuffer, byte[] bytes, int format, int stride, int height, int width, int rotation, long timestamp) {
        if (mediaVideoSource == null) {
            mediaVideoSource = new MediaVideoSource();
            mRtcEngine.setVideoSource(mediaVideoSource);
        }
        if (enablePushVideoToRtc && mediaVideoSource != null) {
            if (mediaVideoSource.getFrameConsumer() != null && mediaVideoSource.isEnablePushToRtc()) {
                mediaVideoSource.getFrameConsumer().consumeByteArrayFrame(bytes, MediaIO.PixelFormat.I420.intValue(), stride, height, this.rotation, System.currentTimeMillis());
            }
        }
    }

    private void onAudioData(ByteBuffer audioBuffer, byte[] bytes, int sampleRataHz, int bytesPerSample, int channelNums, int samplesPerChannel, long timestamp) {
        if (enablePushAudioToRtc) {
            nativeOnAudioData(audioBuffer, sampleRataHz, bytesPerSample, channelNums, samplesPerChannel, timestamp);
        }
    }

    public void enableLocalPlayoutVolume(boolean enable) {
        if (mRtcEngine == null || this.agoraMediaPlayerKit == null) {
            return;
        }
        this.agoraMediaPlayerKit.mute(enable);
        if (enable) {
            this.agoraMediaPlayerKit.registerAudioFrameObserver(this);
            mRtcEngine.setParameters("{\"che.audio.enable.aec\":true}");
        } else {
            this.agoraMediaPlayerKit.unregisterAudioFrameObserver(this);
            mRtcEngine.setParameters("{\"che.audio.enable.aec\":false}");
        }
        enablePushAudioToRtc = true;
        nativeEnableLocalPlayoutVolume(enable);
    }

    public int adjustPublishLocalVoiceVolume(int volume) {
        if (mRtcEngine == null || this.agoraMediaPlayerKit == null) {
            return ERROR_CODE;
        }
        float volumeFloat = (float) (volume) / (float) (MAX_VOLUME);
        if (volumeFloat > 1) {
            volumeFloat = 1;
        }
        nativeAdjustPublishLocalVoiceVolume(volumeFloat);
        return 0;
    }

    @Override
    public void onFrame(AudioFrame audioFrame) {
//        try {
//            ToolUtil.saveDataToFile("/sdcard/test_java.pcm",audioFrame.bytes);
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
        this.onAudioData(audioFrame.buffer, audioFrame.bytes, audioFrame.sampleRataHz, audioFrame.bytesPerSample, audioFrame.channelNums, audioFrame.samplesPerChannel, audioFrame.timestamp);
    }

    @Override
    public void onFrame(VideoFrame videoFrame) {
        this.onVideoData(videoFrame.buffer, videoFrame.bytes, videoFrame.format, videoFrame.stride, videoFrame.height, videoFrame.width, videoFrame.rotation, System.currentTimeMillis());
    }


    private native int nativeOnAudioData(ByteBuffer audioBuffer, int sampleRataHz, int bytesPerSample, int channelNums, int samplesPerChannel, long timestamp);

    private native int nativeEnablePushToRtc();

    private native int nativeEnablePushAudioToRtc(boolean enable);

    private native int nativeRelease();

    private native int adjustPublishSignalVolume(float volume);

    private native int nativeAdjustPublishLocalVoiceVolume(float volume);

    private native int nativeEnableLocalPlayoutVolume(boolean enable);

    private native int adjustPublishVoiceVolume(float volume);

    private native int nativeUnregisterAudioFrameObserver();

    public void onJoinChannelSuccess(String channel, int uid, int elapsed) {
        if (!enablePushVideoToRtc) {
            enablePushVideoToRtc = true;
        }
        if (!enablePushAudioToRtc) {
            enablePushAudioToRtc = true;
        }
    }

    public void onRejoinChannelSuccess(String channel, int uid, int elapsed) {
        if (!enablePushVideoToRtc) {
            enablePushVideoToRtc = true;
        }
        if (!enablePushAudioToRtc) {
            enablePushAudioToRtc = true;
        }
    }

    public void onLeaveChannel(IRtcEngineEventHandler.RtcStats stats) {
        enablePushVideoToRtc = false;
        enablePushAudioToRtc = false;
    }

}
