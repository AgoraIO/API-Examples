// IScreenSharing.aidl
package io.agora.rtc.screencapture.aidl;

import io.agora.rtc.screencapture.aidl.INotification;

// Declare any non-default types here with import statements

interface IScreenSharing {
    void registerCallback(INotification callback);
    void unregisterCallback(INotification callback);
    void startShare();
    void stopShare();
    void renewToken(String token);
}
