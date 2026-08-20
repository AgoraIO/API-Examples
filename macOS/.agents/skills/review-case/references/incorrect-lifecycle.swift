// ❌ INCORRECT: Creating engine on every join
func joinChannel() {
    agoraKit = AgoraRtcEngineKit(...)  // WRONG: recreates engine
    agoraKit.joinChannel(...)
}

// ❌ INCORRECT: Not calling destroy
override func viewWillClose() {
    agoraKit.leaveChannel(nil)
    // Missing: agoraKit.destroy()
}
