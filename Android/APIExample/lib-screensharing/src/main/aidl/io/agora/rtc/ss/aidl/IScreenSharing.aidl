// IScreenSharing.aidl
package io.agora.rtc.ss.aidl;

import io.agora.rtc.ss.aidl.INotification;

// Declare any non-default types here with import statements

interface IScreenSharing {
    void registerCallback(INotification callback);
    void unregisterCallback(INotification callback);
    void startShare();
    void stopShare();
    void renewToken(String token);
}
