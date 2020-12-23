package io.agora.api.example.examples.advanced.customvideo;

import io.agora.rtc.mediaio.IVideoFrameConsumer;
import io.agora.rtc.mediaio.IVideoSource;
import io.agora.rtc.mediaio.MediaIO;

/**
 * Created by wyylling@gmail.com on 03/01/2018.
 */

public class AgoraVideoSource implements IVideoSource {
    private IVideoFrameConsumer mConsumer;

    @Override
    public boolean onInitialize(IVideoFrameConsumer iVideoFrameConsumer) {
        mConsumer = iVideoFrameConsumer;
        return true;
    }

    @Override
    public boolean onStart() {
        return true;
    }

    @Override
    public void onStop() {
    }

    @Override
    public void onDispose() {
    }

    @Override
    public int getBufferType() {
        return MediaIO.BufferType.BYTE_ARRAY.intValue();
    }

    @Override
    public int getCaptureType() {
        return MediaIO.CaptureType.CAMERA.intValue();
    }

    @Override
    public int getContentHint() {
        return MediaIO.ContentHint.NONE.intValue();
    }

    public IVideoFrameConsumer getConsumer() {
        return mConsumer;
    }
}
