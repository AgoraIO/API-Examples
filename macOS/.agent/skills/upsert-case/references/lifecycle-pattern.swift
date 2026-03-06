// Proper Engine Lifecycle Pattern

// Create
let config = AgoraRtcEngineConfig()
config.appId = KeyCenter.AppId
agoraKit = AgoraRtcEngineKit(config: config, delegate: self)

// Use
agoraKit.joinChannel(byToken: token, channelName: channel, info: nil, uid: 0)

// Cleanup
agoraKit.leaveChannel(nil)
agoraKit.destroy()
