// INotification.aidl
package io.agora.rtc.ss.aidl;

// Declare any non-default types here with import statements

interface INotification {
    void onError(int error);
    void onTokenWillExpire();
}
