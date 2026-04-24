# macOS ARCHITECTURE

macOS example project using Swift + Cocoa. Demonstrates Agora RTC SDK features through a collection of self-contained examples organized by complexity.

## Technology Stack

- Language: Swift
- UI Framework: Cocoa (AppKit)
- Architecture: Single-window application with example selection
- State: Instance variables + delegate callbacks

## Directory Structure

```
macOS/
├── APIExample/
│   ├── Examples/
│   │   ├── Basic/
│   │   │   └── <ExampleName>/
│   │   │       ├── <ExampleName>.swift
│   │   │       └── SKILL.md          # Per-example agent guide (present or forthcoming)
│   │   └── Advanced/
│   │       └── <ExampleName>/
│   │           ├── <ExampleName>.swift
│   │           └── SKILL.md          # Per-example agent guide (present or forthcoming)
│   ├── Common/              # Shared utilities (KeyCenter, GlobalSettings, LogUtils, Util)
│   ├── Resources/
│   ├── Base.lproj/          # Storyboard and localization
│   ├── AppDelegate.swift
│   └── ViewController.swift # Main window controller
├── SimpleFilter/            # Specialized filter example
├── APIExample.xcodeproj/    # Xcode project
├── APIExample.xcworkspace/  # Xcode workspace
├── libs/                    # SDK libraries
├── Pods/                    # CocoaPods dependencies
├── .agent/skills/           # Agent skills
│   ├── create-api-example/
│   ├── find-api-example/
│   └── migrate-api-to-project/
├── AGENTS.md                # Agent guide
└── ARCHITECTURE.md          # This file
```

## Architectural Rules

### Example Structure

Each example lives in its own folder under `APIExample/Examples/Basic/` or `APIExample/Examples/Advanced/` and consists of:
- A Swift file containing the example implementation
- A per-example storyboard, typically at `Base.lproj/<ExampleName>.storyboard`

### Example Pattern

Each example is a self-contained class that:
- Manages its own Agora engine lifecycle
- Implements `AgoraRtcEngineDelegate`
- Receives configuration via initialization or property injection
- Owns all UI elements for that example

### Menu Registration

All examples are registered in `APIExample/ViewController.swift` via a menu or list structure. The example name must match the folder name.

### Naming

- Example folder names: PascalCase (e.g., `JoinChannelVideo`)
- Example class: `<ExampleName>` (e.g., `JoinChannelVideo`)

### Common Utilities

All examples share utilities from `APIExample/Common/`:
- `KeyCenter` — App ID and token
- `GlobalSettings` — Shared runtime configuration
- `LogUtils` — SDK log path
- `Util` — Privatization configuration

## Case Index

| Case | Path | Key APIs | Description |
|------|------|----------|-------------|
| JoinChannelAudio | `Examples/Basic/JoinChannelAudio/` | `createAgoraRtcEngine()`, `joinChannel()`, `leaveChannel()`, `destroy()` | Basic audio call — join channel and manage audio stream |
| JoinChannelVideo | `Examples/Basic/JoinChannelVideo/` | `createAgoraRtcEngine()`, `joinChannel()`, `setupLocalVideo()`, `setupRemoteVideo()`, `leaveChannel()`, `destroy()` | Basic video call — join channel and render local/remote video |
| JoinChannelVideo(Token) | `Examples/Basic/JoinChannelVideo(Token)/` | `createAgoraRtcEngine()`, `joinChannel()` with token, `setupLocalVideo()`, `setupRemoteVideo()` | Video call with token authentication |
| JoinChannelVideo(Recorder) | `Examples/Basic/JoinChannelVideo(Recorder)/` | `createAgoraRtcEngine()`, `joinChannel()`, `startAudioRecording()`, `stopAudioRecording()` | Video call with local audio recording |
| AgoraBeauty | `Examples/Advanced/AgoraBeauty/` | `setBeautyEffectOptions()`, `setVideoEncoderConfiguration()` | Beauty filter and enhancement effects |
| AudioMixing | `Examples/Advanced/AudioMixing/` | `startAudioMixing()`, `stopAudioMixing()`, `pauseAudioMixing()`, `resumeAudioMixing()` | Audio file mixing and playback control |
| ChannelMediaRelay | `Examples/Advanced/ChannelMediaRelay/` | `startChannelMediaRelay()`, `updateChannelMediaRelay()`, `stopChannelMediaRelay()` | Relay media streams across multiple channels |
| ContentInspect | `Examples/Advanced/ContentInspect/` | `enableContentInspect()`, `disableContentInspect()` | Content inspection and moderation |
| CreateDataStream | `Examples/Advanced/CreateDataStream/` | `createDataStream()`, `sendStreamMessage()` | Custom data stream creation and messaging |
| CustomAudioRender | `Examples/Advanced/CustomAudioRender/` | `setExternalAudioSink()`, `pullAudioFrame()` | Custom audio rendering pipeline |
| CustomAudioSource | `Examples/Advanced/CustomAudioSource/` | `setExternalAudioSource()`, `pushAudioFrame()` | Custom audio source capture |
| CustomVideoRender | `Examples/Advanced/CustomVideoRender/` | `setExternalVideoSink()`, `pullVideoFrame()` | Custom video rendering pipeline |
| CustomVideoSourceMediaIO | `Examples/Advanced/CustomVideoSourceMediaIO/` | `setExternalVideoSource()`, `pushVideoFrame()` with MediaIO | Custom video source with media I/O |
| CustomVideoSourcePush | `Examples/Advanced/CustomVideoSourcePush/` | `setExternalVideoSource()`, `pushVideoFrame()` | Custom video source push |
| CustomVideoSourcePushMulti | `Examples/Advanced/CustomVideoSourcePushMulti/` | `setExternalVideoSource()`, `pushVideoFrame()` with multiple sources | Multiple custom video sources |
| FaceCapture | `Examples/Advanced/FaceCapture/` | `enableFaceDetection()`, `getFaceDetectionResult()` | Face detection and capture |
| JoinMultiChannel | `Examples/Advanced/JoinMultiChannel/` | `createRtcChannel()`, `joinChannel()` on multiple channels | Join and manage multiple channels simultaneously |
| LiveStreaming | `Examples/Advanced/LiveStreaming/` | `setClientRole()`, `startRtmpStreamWithTranscoding()`, `stopRtmpStream()` | RTMP live streaming with transcoding |
| LocalVideoTranscoding | `Examples/Advanced/LocalVideoTranscoding/` | `startLocalVideoTranscoding()`, `updateLocalTranscodingConfig()`, `stopLocalVideoTranscoding()` | Local video transcoding and composition |
| MediaPlayer | `Examples/Advanced/MediaPlayer/` | `createMediaPlayer()`, `open()`, `play()`, `pause()`, `stop()` | Media file playback and control |
| MultiCameraSourece | `Examples/Advanced/MultiCameraSourece/` | `enumerateDevices()`, `setDevice()` with multiple cameras | Multiple camera source selection |
| Multipath | `Examples/Advanced/Multipath/` | `enableMultipath()`, `setMultipathConfig()` | Multipath redundancy for reliability |
| PrecallTest | `Examples/Advanced/PrecallTest/` | `startEchoTest()`, `stopEchoTest()`, `startNetworkTest()`, `stopNetworkTest()` | Pre-call network and device testing |
| QuickSwitchChannel | `Examples/Advanced/QuickSwitchChannel/` | `switchChannel()` | Quick channel switching without reconnection |
| RawAudioData | `Examples/Advanced/RawAudioData/` | `setAudioFrameDelegate()`, `onMixedAudioFrame()` | Raw audio frame access and processing |
| RawMediaData | `Examples/Advanced/RawMediaData/` | `setVideoFrameDelegate()`, `setAudioFrameDelegate()` | Raw audio and video frame access |
| RawVideoData | `Examples/Advanced/RawVideoData/` | `setVideoFrameDelegate()`, `onCapturedVideoFrame()`, `onRemoteVideoFrame()` | Raw video frame capture and processing |
| RtePlayer | `Examples/Advanced/RtePlayer/` | `createMediaPlayer()`, `open()` with RTE protocol | RTE protocol media playback |
| RTMPStreaming | `Examples/Advanced/RTMPStreaming/` | `startRtmpStreamWithTranscoding()`, `updateRtmpTranscodingConfig()`, `stopRtmpStream()` | RTMP streaming with live transcoding |
| ScreenShare | `Examples/Advanced/ScreenShare/` | `startScreenCapture()`, `updateScreenCaptureParameters()`, `stopScreenCapture()` | Screen sharing and capture |
| SimpleFilter | `Examples/Advanced/SimpleFilter/` | `setVideoEncoderConfiguration()`, `setBeautyEffectOptions()` | Simple video filter effects |
| Simulcast | `Examples/Advanced/Simulcast/` | `setSimulcastConfig()`, `enableSimulcast()` | Simulcast streaming with multiple bitrates |
| SpatialAudio | `Examples/Advanced/SpatialAudio/` | `getLocalSpatialAudioEngine()`, `updateSelfPosition()`, `updateRemotePosition()` | 3D spatial audio positioning |
| StreamEncryption | `Examples/Advanced/StreamEncryption/` | `enableEncryption()`, `setEncryptionConfig()` | Stream encryption and security |
| VideoProcess | `Examples/Advanced/VideoProcess/` | `setVideoEncoderConfiguration()`, `setBeautyEffectOptions()` | Video processing and enhancement |
| VoiceChanger | `Examples/Advanced/VoiceChanger/` | `setVoiceBeautifierPreset()`, `setAudioEffectPreset()` | Voice effects and voice changer |

## Engine Lifecycle

```
1. Create Engine
   createAgoraRtcEngine()
   
2. Initialize Engine
   initialize(AgoraRtcEngineConfig)
   
3. Enable Features (optional)
   enableVideo(), enableAudio()
   
4. Setup Local Media (optional)
   setupLocalVideo(), startAudioMixing()
   
5. Join Channel
   joinChannel(token, channelName, uid)
   
6. Handle Callbacks
   onJoinChannelSuccess(), onUserJoined(), onUserOffline()
   
7. Leave Channel
   leaveChannel()
   
8. Destroy Engine
   destroy()
```

## Token Flow

Token is obtained from `KeyCenter.swift` and passed to `joinChannel()`:

```swift
let token = KeyCenter.Token(channelName: channelName)
agoraKit.joinChannel(byToken: token, channelName: channelName, info: nil, uid: 0)
```

For production, tokens should be generated server-side and refreshed before expiration.
