// IMultiProcessVideoRenderlInterface.aidl
package io.agora.api.example;

// Declare any non-default types here with import statements
import io.agora.api.example.IRtcEventHandlerInterface;

interface IMultiProcessVideoRenderlInterface {

    void initRtcEngine();

    void destroyRtcEngine();

    void joinChannel(String channelName, int uid, int roleType, in IRtcEventHandlerInterface rtcEventHandler);

    void leaveChannel();

    void setupLocalVideo(in Surface surface, int width, int height, int mirrorMode, int renderMode);

    void setupRemoteVideo(in Surface surface, int width, int height, int uid, int mirrorMode, int renderMode);

    void switchCamera();
}