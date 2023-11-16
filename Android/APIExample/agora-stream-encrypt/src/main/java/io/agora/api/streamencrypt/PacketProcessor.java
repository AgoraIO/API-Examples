package io.agora.api.streamencrypt;

public class PacketProcessor {
    static {
        System.loadLibrary("agora-stream-encrypt");
    }

    public static void registerProcessing(long rtcEngineHandler) {
        doRegisterProcessing(rtcEngineHandler);
    }

    public static void unregisterProcessing(long rtcEngineHandler) {
        doUnregisterProcessing(rtcEngineHandler);
    }

    private native static void doRegisterProcessing(long rtcEngineHandler);
    private native static void doUnregisterProcessing(long rtcEngineHandler);
}
