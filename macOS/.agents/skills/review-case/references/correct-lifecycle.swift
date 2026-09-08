// Fragments from the case controller; request tracking is part of the controller state.
// See the upsert-case example-template.swift for a complete skeleton.

override func viewDidLoad() {
    super.viewDidLoad()
    let config = AgoraRtcEngineConfig()
    config.appId = KeyCenter.AppId
    agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
}

override func viewWillBeRemovedFromSplitView() {
    tokenRequestID += 1 // Late Token callbacks must check this ID and the live engine.
    agoraKit?.leaveChannel(nil)
    // Stop case-owned media, timers and observers before destruction.
    if agoraKit != nil {
        AgoraRtcEngineKit.destroy()
        agoraKit = nil
    }
    super.viewWillBeRemovedFromSplitView()
}
