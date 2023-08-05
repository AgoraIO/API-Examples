// IRtcEventHandlerInterface.aidl
package io.agora.api.example;

// Declare any non-default types here with import statements

interface IRtcEventHandlerInterface {


    void onUserJoined(int uid);

    void onUserOffline(int uid);

    void onJoinChannelSuccess(String channelName, int uid);
}