package io.agora.rtc.ss.impl;

import io.agora.rtc.mediaio.IVideoFrameConsumer;
import io.agora.rtc.mediaio.IVideoSource;
import io.agora.rtc.mediaio.MediaIO;
import io.agora.rtc.video.AgoraVideoFrame;

public class ScreenCaptureSource implements IVideoSource {

    private IVideoFrameConsumer mConsumer;

    @Override
    public boolean onInitialize(IVideoFrameConsumer observer) {
        mConsumer = observer;
        return true;
    }

    @Override
    public int getBufferType() {
        return AgoraVideoFrame.BUFFER_TYPE_TEXTURE;
    }

    @Override
    public int getCaptureType() {
        return MediaIO.CaptureType.SCREEN.intValue();
    }

    @Override
    public int getContentHint() {
        return MediaIO.ContentHint.NONE.intValue();
    }

    @Override
    public void onDispose() {
        mConsumer = null;
    }

    @Override
    public void onStop() {
    }

    @Override
    public boolean onStart() {
        return true;
    }

    public IVideoFrameConsumer getConsumer() {
        return mConsumer;
    }

}
