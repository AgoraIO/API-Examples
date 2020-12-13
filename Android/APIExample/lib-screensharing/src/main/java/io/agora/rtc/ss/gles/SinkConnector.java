package io.agora.rtc.ss.gles;

public abstract class SinkConnector<T> {
    private volatile boolean mConnected = false;

    public SinkConnector() {
    }

    protected void onConnected() {
        this.mConnected = true;
    }

    protected synchronized void onDisconnect() {
        this.mConnected = false;
    }

    public boolean isConnected() {
        return this.mConnected;
    }

    public abstract void onFormatChanged(Object format);

    public abstract void onFrameAvailable(T frame);
}
