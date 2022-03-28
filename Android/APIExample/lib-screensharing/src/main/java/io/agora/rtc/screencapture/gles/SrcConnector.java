package io.agora.rtc.screencapture.gles;

import java.util.Iterator;
import java.util.LinkedList;

public class SrcConnector<T> {
    private LinkedList<io.agora.rtc.screencapture.gles.SinkConnector<T>> plugList = new LinkedList<>();
    private Object mFormat;

    public SrcConnector() {
    }

    public synchronized boolean isConnected() {
        return !this.plugList.isEmpty();
    }

    public synchronized void connect(io.agora.rtc.screencapture.gles.SinkConnector<T> sink) {
        if (!this.plugList.contains(sink)) {
            this.plugList.add(sink);
            sink.onConnected();
            if (mFormat != null) {
                sink.onFormatChanged(mFormat);
            }
        }
    }

    public synchronized void onFormatChanged(Object format) {
        mFormat = format;
        Iterator<io.agora.rtc.screencapture.gles.SinkConnector<T>> it = this.plugList.iterator();
        while (it.hasNext()) {
            io.agora.rtc.screencapture.gles.SinkConnector<T> pin = it.next();
            pin.onFormatChanged(format);
        }
    }

    public synchronized void onFrameAvailable(T frame) {
        Iterator<io.agora.rtc.screencapture.gles.SinkConnector<T>> it = this.plugList.iterator();
        while (it.hasNext()) {
            io.agora.rtc.screencapture.gles.SinkConnector<T> sink = it.next();
            sink.onFrameAvailable(frame);
        }
    }

    public synchronized void disconnect() {
        this.disconnect(null);
    }

    public synchronized void disconnect(io.agora.rtc.screencapture.gles.SinkConnector<T> sink) {
        if (sink != null) {
            sink.onDisconnect();
            this.plugList.remove(sink);
        } else {
            Iterator it = this.plugList.iterator();
            while (it.hasNext()) {
                io.agora.rtc.screencapture.gles.SinkConnector pin = (SinkConnector) it.next();
                pin.onDisconnect();
            }
            this.plugList.clear();
        }

    }
}
