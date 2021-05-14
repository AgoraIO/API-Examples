package io.agora;

import android.util.Log;

import io.agora.rtc.mediaio.IVideoFrameConsumer;
import io.agora.rtc.mediaio.IVideoSource;
import io.agora.rtc.mediaio.MediaIO;
import io.agora.rtc.video.AgoraVideoFrame;

/**
 * Created by lixiaochen on 2019/12/26.
 */

public class MediaVideoSource implements IVideoSource {
    private final static String TAG = "RtcChannelPublishHelper";
    private IVideoFrameConsumer mVideoFrameConsumer = null;
    private boolean isEnablePushToRtc = false;

    @Override
    public boolean onInitialize(IVideoFrameConsumer videoFrameConsumer) {
        Log.i(TAG,"MediaVideoSource onInitialize");
        this.mVideoFrameConsumer = videoFrameConsumer;
        isEnablePushToRtc = true;
        return true;
    }

    @Override
    public boolean onStart() {
        Log.i(TAG,"MediaVideoSource onStart");
        isEnablePushToRtc = true;
        return true;
    }

    @Override
    public void onStop() {
        isEnablePushToRtc = false;
        Log.i(TAG,"MediaVideoSource onStop");
    }

    @Override
    public void onDispose() {
        isEnablePushToRtc = false;
        Log.i(TAG,"MediaVideoSource onDispose");
    }

    public boolean isEnablePushToRtc() {
        return isEnablePushToRtc;
    }

    @Override
    public int getBufferType() {
        return MediaIO.BufferType.BYTE_ARRAY.intValue();
    }

    @Override
    public int getCaptureType() {
        return MediaIO.CaptureType.UNKNOWN.intValue();
    }

    @Override
    public int getContentHint() {
        return MediaIO.ContentHint.NONE.intValue();
    }

    public IVideoFrameConsumer getFrameConsumer () {
        return this.mVideoFrameConsumer;
    }
}
