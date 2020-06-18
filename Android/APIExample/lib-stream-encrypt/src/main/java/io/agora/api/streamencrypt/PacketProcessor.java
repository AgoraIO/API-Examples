package io.agora.api.streamencrypt;

public class PacketProcessor {
    static {
        System.loadLibrary("apm-plugin-packet-processing");
    }

    public final void registerProcessing() {
        doRegisterProcessing();
    }

    public final void unregisterProcessing() {
        doUnregisterProcessing();
    }

    private native void doRegisterProcessing();
    private native void doUnregisterProcessing();
}
