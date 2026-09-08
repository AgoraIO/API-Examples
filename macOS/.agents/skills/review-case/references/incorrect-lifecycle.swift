// ❌ INCORRECT: Creating engine on every join
func joinChannel() {
    agoraKit = AgoraRtcEngineKit(...)  // WRONG: recreates engine
    agoraKit.joinChannel(...)
}

// ❌ INCORRECT: Not calling destroy
override func viewWillBeRemovedFromSplitView() {
    agoraKit.leaveChannel(nil)
    // Missing: AgoraRtcEngineKit.destroy()
    super.viewWillBeRemovedFromSplitView()
}
