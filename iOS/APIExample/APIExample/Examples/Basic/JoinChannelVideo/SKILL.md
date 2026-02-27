---
name: join-channel-video-guide
description: Guide for implementing video call functionality in business scenarios, including SDK initialization, joining channels, video encoding configuration, and event handling
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  example: JoinChannelVideo
  category: Basic
  apis: [AgoraRtcEngineKit, joinChannel, enableVideo, setupLocalVideo, setupRemoteVideo]
---

# Video Call Implementation Guide

## Feature Description

This example demonstrates how to use Agora RTC SDK to implement basic video call functionality, including:
- Initialize SDK engine
- Configure video encoding parameters (resolution, frame rate, orientation)
- Join channel
- Display local and remote video
- Handle user join/leave events

## Core API Call Flow

### 1. Initialize SDK

```swift
let config = AgoraRtcEngineConfig()
config.appId = KeyCenter.AppId
config.areaCode = GlobalSettings.shared.area
config.channelProfile = .liveBroadcasting
agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
```

**Key Parameters:**
- `appId`: App ID obtained from Agora Console
- `areaCode`: Region code for specifying connection region
- `channelProfile`: Channel profile, `.liveBroadcasting` supports host and audience roles

### 2. Set User Role and Enable Audio/Video

```swift
agoraKit.setClientRole(.broadcaster)  // or .audience
agoraKit.enableVideo()
agoraKit.enableAudio()
```

### 3. Configure Video Encoding Parameters

```swift
agoraKit.setVideoEncoderConfiguration(
    AgoraVideoEncoderConfiguration(
        size: CGSize(width: 960, height: 540),
        frameRate: .fps15,
        bitrate: AgoraVideoBitrateStandard,
        orientationMode: .adaptative,
        mirrorMode: .auto
    )
)
```

**Configurable Parameters:**
- `size`: Video resolution (90x90 ~ 1280x720)
- `frameRate`: Frame rate (10/15/24/30/60 fps)
- `orientationMode`: Video orientation mode
  - `.adaptative`: Adaptive
  - `.fixedLandscape`: Fixed landscape
  - `.fixedPortrait`: Fixed portrait

### 4. Set Local Video Preview

```swift
let videoCanvas = AgoraRtcVideoCanvas()
videoCanvas.uid = 0  // Local user uid is 0
videoCanvas.view = localVideoView
videoCanvas.renderMode = .hidden
agoraKit.setupLocalVideo(videoCanvas)
agoraKit.startPreview()
```

### 5. Join Channel

```swift
let option = AgoraRtcChannelMediaOptions()
option.publishCameraTrack = true
option.publishMicrophoneTrack = true
option.clientRoleType = .broadcaster

agoraKit.joinChannel(
    byToken: token,
    channelId: channelName,
    uid: 0,  // 0 means SDK automatically assigns uid
    mediaOptions: option
)
```

**Important Notes:**
- If App Certificate is enabled in console, Token must be used
- Token must be generated with the same channel name and uid
- Return value of 0 indicates success

### 6. Set Remote Video

Set in `didJoinedOfUid` callback:

```swift
func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
    let videoCanvas = AgoraRtcVideoCanvas()
    videoCanvas.uid = uid
    videoCanvas.view = remoteVideoView
    videoCanvas.renderMode = .hidden
    agoraKit.setupRemoteVideo(videoCanvas)
}
```

### 7. Leave Channel and Clean Up Resources

```swift
agoraKit.disableAudio()
agoraKit.disableVideo()
agoraKit.stopPreview()
agoraKit.leaveChannel { stats in
    print("left channel, duration: \(stats.duration)")
}
```

## Key Event Callbacks

### didJoinChannel
```swift
func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int)
```
Triggered when local user successfully joins channel.

### didJoinedOfUid
```swift
func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int)
```
Triggered when remote user joins channel (not triggered for audience role).

### didOfflineOfUid
```swift
func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason)
```
Triggered when remote user leaves channel.

### didOccurError
```swift
func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode)
```
Triggered when SDK encounters an error, recommend displaying error message to user.

## Common Questions

### Q: Can't see local video?
A: Check if `startPreview()` and `setupLocalVideo()` have been called

### Q: Can't see remote video?
A: 
1. Confirm remote user role is broadcaster
2. Check if `setupRemoteVideo()` is correctly set in `didJoinedOfUid` callback
3. Confirm `option.publishCameraTrack = true`

### Q: joinChannel returns non-zero value?
A: 
- Check if App ID is correct
- If App Certificate is enabled, check if Token is valid
- Check if channel name and uid match those used when generating Token

### Q: How to switch camera?
A: Call `agoraKit.switchCamera()`

### Q: How to mute/unmute?
A: 
- Local mute: `agoraKit.muteLocalAudioStream(true/false)`
- Remote mute: `agoraKit.muteRemoteAudioStream(uid, mute: true/false)`

### Q: How to disable/enable video?
A:
- Local video: `agoraKit.muteLocalVideoStream(true/false)`
- Remote video: `agoraKit.muteRemoteVideoStream(uid, mute: true/false)`

## Reference Documentation

- [iOS API Reference (English)](https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit)
- [iOS API Reference (Chinese)](https://doc.shengwang.cn/api-ref/rtc/ios/API/toc_video_call)
- [Error Code Description](https://doc.shengwang.cn/api-ref/rtc/ios/error-code)

## Related Examples

- `JoinChannelAudio` - Audio-only call
- `JoinChannelVideoToken` - Join channel with Token
- `VideoProcess` - Video processing (filters, watermarks)
- `CustomVideoSourcePush` - Custom video source
