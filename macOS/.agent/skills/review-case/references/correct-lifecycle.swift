// Correct Engine Lifecycle Pattern

override func viewDidLoad() {
    super.viewDidLoad()
    initializeAgoraEngine()  // Create once
}

override func viewWillClose() {
    leaveChannel()
    super.viewWillClose()
}

func joinChannel() {
    agoraKit.joinChannel(byToken: token, channelName: channel, info: nil, uid: 0)
}

func leaveChannel() {
    agoraKit.leaveChannel(nil)
    agoraKit.destroy()
}
