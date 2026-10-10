// Lifecycle fragments inside a BaseViewController + AgoraRtcEngineDelegate case.
// See example-template.swift for the complete skeleton and asynchronous Token guards.

// Create once when the case controller loads.
let config = AgoraRtcEngineConfig()
config.appId = KeyCenter.AppId
agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)

// After permission/Token resolution, verify the case and request are still active.
// token, channelName, uid and options come from the case's current request.
let result = agoraKit?.joinChannel(byToken: token, channelId: channelName,
                                   uid: uid, mediaOptions: options)
// Handle a nonzero result before updating UI to a joined state.

// In viewWillBeRemovedFromSplitView(), on the main queue:
tokenRequestID += 1
agoraKit?.leaveChannel(nil)
// Stop case-owned media, timers and observers before destruction.
if agoraKit != nil {
    AgoraRtcEngineKit.destroy()
    agoraKit = nil
}
// Call super.viewWillBeRemovedFromSplitView() last.
