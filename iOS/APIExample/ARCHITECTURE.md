# ARCHITECTURE.md — APIExample

## Case Index

| Case | Path | Key APIs | Description |
|------|------|----------|-------------|
| JoinChannelVideo | `Examples/Basic/JoinChannelVideo/JoinChannelVideo.swift` | `joinChannel()`, `setupLocalVideo()`, `setupRemoteVideo()` | Basic video call — join channel and render local/remote video |
| JoinChannelVideo(Token) | `Examples/Basic/JoinChannelVideo(Token)/JoinChannelVideoToken.swift` | `joinChannel(byToken:)`, `setupLocalVideo()`, `setupRemoteVideo()` | Video call with token authentication |
| JoinChannelVideo(Recorder) | `Examples/Basic/JoinChannelVideo(Recorder)/JoinChannelVideoRecorder.swift` | `createMediaRecorder()`, `joinChannel()`, `setupLocalVideo()` | Local and remote stream recording |
| JoinChannelAudio | `Examples/Basic/JoinChannelAudio/JoinChannelAudio.swift` | `joinChannel()`, `setAudioProfile()`, `enableAudioVolumeIndication()`, `adjustRecordingSignalVolume()` | Basic audio call with volume and in-ear monitoring controls |
| LiveStreaming | `Examples/Advanced/LiveStreaming/LiveStreaming.swift` | `setClientRole()`, `setVideoScenario()`, `preloadChannel()`, `enableCameraCenterStage()` | Interactive live streaming with role switching and camera features |
| RTMPStreaming | `Examples/Advanced/RTMPStreaming/RTMPStreaming.swift` | `startRtmpStreamWithoutTranscoding()`, `startRtmpStream(withTranscoding:)`, `updateRtmpTranscoding()`, `stopRtmpStream()` | Push stream to CDN with optional transcoding |
| VideoMetadata | `Examples/Advanced/VideoMetadata/VideoMetadata.swift` | `setMediaMetadataDataSource()`, `setMediaMetadataDelegate()` | Send and receive metadata attached to video stream |
| VoiceChanger | `Examples/Advanced/VoiceChanger/VoiceChanger.swift` | `setVoiceBeautifierPreset()`, `setAudioEffectPreset()`, `setVoiceConversionPreset()`, `setAINSMode()` | Voice beautifier, effects, conversion presets, and AI noise suppression |
| CustomPcmAudioSource | `Examples/Advanced/CustomPcmAudioSource/CustomPcmAudioSource.swift` | `createCustomAudioTrack()`, `enableCustomAudioLocalPlayback()`, `pushExternalAudioFrameRawData()` | Push custom PCM audio frames as external audio source |
| CustomAudioRender | `Examples/Advanced/CustomAudioRender/CustomAudioRender.swift` | `enableExternalAudioSink()`, `pullPlaybackAudioFrameRawData()` | Pull audio frames for custom rendering |
| CustomAudioSource | `Examples/Advanced/CustomAudioSource/CustomAudioSource.swift` | `createCustomAudioTrack()` | Push custom audio via mixable audio track |
| CustomVideoSourcePush | `Examples/Advanced/CustomVideoSourcePush/CustomVideoSourcePush.swift` | `setExternalVideoSource()`, `pushExternalVideoFrame()` | Push external video frames as custom video source |
| CustomVideoSourcePushMulti | `Examples/Advanced/CustomVideoSourcePushMulti/CustomVideoSourcePushMulti.swift` | `createCustomVideoTrack()`, `createCustomEncodedVideoTrack()`, `pushExternalEncodedVideoFrame()` | Multi-track custom video source with encoded frame push |
| CustomVideoRender | `Examples/Advanced/CustomVideoRender/CustomVideoRender.swift` | `setVideoFrameDelegate()` | Custom rendering of remote video frames via delegate |
| RawAudioData | `Examples/Advanced/RawAudioData/RawAudioData.swift` | `setAudioFrameDelegate()`, `sendAudioMetadata()` | Capture raw audio PCM data via delegate |
| RawVideoData | `Examples/Advanced/RawVideoData/RawVideoData.swift` | `setVideoFrameDelegate()` | Capture raw video frames via delegate |
| RawMediaData | `Examples/Advanced/RawMediaData/RawMediaData.swift` | `setVideoFrameDelegate()`, `setAudioFrameDelegate()`, `setRecordingAudioFrameParametersWithSampleRate()`, `startAudioRecording()` | Capture both raw audio and video data simultaneously |
| PictureInPicture | `Examples/Advanced/PictureInPicture/` | `AVPictureInPictureController`, `joinChannel()`, `setVideoFrameDelegate()` | Picture-in-Picture using AVKit (iOS 15+) |
| SimpleFilter | `Examples/Advanced/SimpleFilter/SimpleFilter.swift` | `enableExtension()`, `setExtensionPropertyWithVendor()` | Apply audio/video filter via Agora Extension API |
| QuickSwitchChannel | `Examples/Advanced/QuickSwitchChannel/QuickSwitchChannel.swift` | `joinChannel()`, `leaveChannel()` | Quickly switch between channels as audience |
| JoinMultiChannel | `Examples/Advanced/JoinMultiChannel/JoinMultiChannel.swift` | `joinChannelEx()`, `takeSnapshotEx()` | Join multiple channels simultaneously via ex connection |
| StreamEncryption | `Examples/Advanced/StreamEncryption/StreamEncryption.swift` | `enableEncryption()` | Built-in and custom stream encryption |
| AudioMixing | `Examples/Advanced/AudioMixing/AudioMixing.swift` | `startAudioMixing()`, `stopAudioMixing()`, `adjustAudioMixingVolume()`, `setEffectsVolume()` | Mix local audio file with microphone input |
| PrecallTest | `Examples/Advanced/PrecallTest/PrecallTest.swift` | `startEchoTest()`, `stopEchoTest()`, `startLastmileProbeTest()`, `stopLastmileProbeTest()` | Pre-call echo test and last-mile network probe |
| MediaPlayer | `Examples/Advanced/MediaPlayer/MediaPlayer.swift` | `createMediaPlayer()`, `updateChannelEx()` | Play media files and publish to channel via media player |
| ScreenShare | `Examples/Advanced/ScreenShare/ScreenShare.swift` | `startScreenCapture()`, `updateScreenCapture()`, `stopScreenCapture()`, `setScreenCaptureScenario()` | Screen capture and sharing via ReplayKit extension |
| LocalCompositeGraph | `Examples/Advanced/LocalCompositeGraph/LocalCompositeGraph.swift` | `startLocalVideoTranscoder()`, `startCameraCapture()`, `startScreenCapture()`, `enableVirtualBackground()` | Composite multiple video sources locally before publishing |
| VideoProcess | `Examples/Advanced/VideoProcess/VideoProcess.swift` | `setBeautyEffectOptions()`, `enableVirtualBackground()`, `enableExtension()` | Built-in beauty, virtual background, and video enhancement |
| AgoraBeauty | `Examples/Advanced/AgoraBeauty/AgoraBeauty.swift` | `enableExtension()`, `enableVirtualBackground()` | Agora beauty extension with virtual background |
| RhythmPlayer | `Examples/Advanced/RhythmPlayer/RhythmPlayer.swift` | `startRhythmPlayer()`, `stopRhythmPlayer()` | Play metronome-style rhythm audio |
| CreateDataStream | `Examples/Advanced/CreateDataStream/CreateDataStream.swift` | `createDataStream()`, `sendStreamMessage()` | Create and send data stream messages between users |
| MediaChannelRelay | `Examples/Advanced/MediaChannelRelay/MediaChannelRelay.swift` | `startOrUpdateChannelMediaRelay()`, `stopChannelMediaRelay()`, `pauseAllChannelMediaRelay()`, `resumeAllChannelMediaRelay()` | Relay media stream to multiple destination channels |
| SpatialAudio | `Examples/Advanced/SpatialAudio/SpatialAudio.swift` | `createMediaPlayer()`, `updateChannel()` | 3D spatial audio with media player integration |
| ContentInspect | `Examples/Advanced/ContentInspect/ContentInspect.swift` | `enableContentInspect()`, `switchCamera()` | Moderate content in video stream |
| MutliCamera | `Examples/Advanced/MutliCamera/MutliCamera.swift` | `enableMultiCamera()`, `startCameraCapture()`, `stopCameraCapture()` | Capture from front and back cameras simultaneously (iOS 13+) |
| KtvCopyrightMusic | `Examples/Advanced/KtvCopyrightMusic/KtvCopyrightMusic.swift` | — | Links to KTV copyright music documentation |
| ThirdBeautify | `Examples/Advanced/ThirdBeautify/ThirdBeautify.swift` | `enableExtension()` | Third-party beauty SDK integration (ByteDance / FaceUnity / SenseTime) |
| ARKit | `Examples/Advanced/ARKit/ARKit.swift` | `setVideoFrameDelegate()`, `enableInstantMediaRendering()`, `startMediaRenderingTracing()` | Push ARKit face tracking frames as custom video source |
| AudioRouterPlayer | `Examples/Advanced/AudioRouterPlayer/AudioRouterPlayer.swift` | `setEnableSpeakerphone()` | Control audio output routing with third-party player |
| AudioWaveform | `Examples/Advanced/AudioWaveform/AudioWaveform.swift` | `setAudioProfile()`, `enableAudioVolumeIndication()` | Visualize audio waveform from volume callbacks |
| FaceCapture | `Examples/Advanced/FaceCapture/FaceCapture.swift` | `enableExtension()`, `setExtensionPropertyWithVendor()`, `setFaceInfoDelegate()` | Face capture and lip sync via Agora extension |
| TransparentRender | `Examples/Advanced/TransparentRender/TransparentRender.swift` | `createMediaPlayer()`, `setExternalVideoSource()`, `pushExternalVideoFrame()` | Render video with transparent background |
| RtePlayer | `Examples/Advanced/RtePlayer/RtePlayer.swift` | `AgoraRte`, `AgoraRtePlayer`, `AgoraRteCanvas` | URL-based stream playback via RTE Player API |
| Simulcast | `Examples/Advanced/Simulcast/Simulcast.swift` | `setSimulcastConfig()`, `setRemoteVideoStream()` | Publish multiple video quality layers simultaneously |
| Multipath | `Examples/Advanced/Multipath/Multipath.swift` | `updateChannel()` | Multi-path network transmission configuration |

## Directory Layout

```
APIExample/
├── Podfile                                  # CocoaPods dependencies (AgoraRtcEngine_iOS, Floaty, AGEVideoLayout, etc.)
├── SimpleFilter/                            # Optional C++ audio/video extension module
├── Agora-ScreenShare-Extension/             # ReplayKit broadcast extension for screen sharing
├── ByteEffectLib/                           # Optional ByteDance beauty SDK resources
├── FULib/                                   # Optional FaceUnity beauty SDK resources
├── SenseLib/                                # Optional SenseTime beauty SDK resources
├── libs/                                    # Local SDK frameworks (when not using CocoaPods)
└── APIExample/
    ├── AppDelegate.swift
    ├── ViewController.swift                 # Root menu controller — MenuItem registration lives here
    ├── Info.plist
    ├── APIExample.entitlements
    ├── APIExample-Bridging-Header.h
    │
    ├── Common/
    │   ├── KeyCenter.swift                  # App ID and Certificate
    │   ├── GlobalSettings.swift             # Shared runtime config (resolution, fps, orientation, role)
    │   ├── BaseViewController.swift         # Base class all Main VCs must extend
    │   ├── EntryViewController.swift        # Generic Entry VC for storyboard == "Main" cases
    │   ├── LogViewController.swift          # Log viewer
    │   ├── AlertManager.swift
    │   ├── AgoraExtension.swift
    │   ├── PickerView.swift
    │   ├── StatisticsInfo.swift
    │   ├── UITypeAlias.swift
    │   ├── VideoView.swift / .xib           # Reusable video rendering view
    │   ├── Settings/                        # Settings UI components
    │   ├── Utils/                           # LogUtils, Util (privatization config)
    │   ├── NetworkManager/                  # Token request helper
    │   ├── ExternalAudio/                   # External audio source helpers
    │   ├── ExternalVideo/                   # External video source helpers
    │   ├── CustomEncryption/                # Custom stream encryption helpers
    │   └── ARKit/                           # ARKit integration helpers
    │
    ├── Examples/
    │   ├── Basic/
    │   │   ├── JoinChannelVideo/            # "Join a channel (Video)"
    │   │   ├── JoinChannelVideo(Token)/     # "Join a channel (Token)"
    │   │   ├── JoinChannelVideo(Recorder)/  # "Local or remote recording"
    │   │   └── JoinChannelAudio/            # "Join a channel (Audio)"
    │   └── Advanced/
    │       ├── LiveStreaming/               # "Live Streaming" — setClientRole
    │       ├── RTMPStreaming/               # "RTMP Streaming" — push to CDN
    │       ├── VideoMetadata/               # "Video Metadata" — send/receive metadata
    │       ├── VoiceChanger/                # "Voice Changer" — voice beautifier/effects
    │       ├── CustomPcmAudioSource/        # "Custom Audio Source" — push PCM audio
    │       ├── CustomAudioRender/           # "Custom Audio Render" — pull audio rendering
    │       ├── CustomAudioSource/           # (legacy custom audio source)
    │       ├── CustomVideoSourcePush/       # "Custom Video Source(Push)" — push external video
    │       ├── CustomVideoSourcePushMulti/  # "Custom Video Source(Multi)" — multi-track push
    │       ├── CustomVideoRender/           # "Custom Video Render"
    │       ├── RawAudioData/                # "Raw Audio Data"
    │       ├── RawVideoData/                # "Raw Video Data"
    │       ├── RawMediaData/                # (legacy raw media data)
    │       ├── PictureInPicture/            # "Picture In Picture (iOS15+)"
    │       ├── SimpleFilter/                # "Simple Filter Extension"
    │       ├── QuickSwitchChannel/          # "Quick Switch Channel"
    │       ├── JoinMultiChannel/            # "Join Multiple Channels"
    │       ├── StreamEncryption/            # "Stream Encryption"
    │       ├── AudioMixing/                 # "Audio Mixing"
    │       ├── PrecallTest/                 # "Precall Test"
    │       ├── MediaPlayer/                 # "Media Player"
    │       ├── ScreenShare/                 # "Screen Share"
    │       ├── LocalCompositeGraph/         # "Local Composite Graph"
    │       ├── VideoProcess/                # "Video Process"
    │       ├── AgoraBeauty/                 # "Agora Beauty"
    │       ├── RhythmPlayer/                # "Rhythm Player"
    │       ├── CreateDataStream/            # "Create Data Stream"
    │       ├── MediaChannelRelay/           # "Media Channel Relay"
    │       ├── SpatialAudio/                # "Spatial Audio"
    │       ├── ContentInspect/              # "Content Inspect"
    │       ├── MutliCamera/                 # "Multi Camera (iOS13+)"
    │       ├── KtvCopyrightMusic/           # "KTV Copyright Music"
    │       ├── ThirdBeautify/               # "Third Beautify" — third-party beauty SDK
    │       ├── ARKit/                       # "ARKit"
    │       ├── AudioRouterPlayer/           # "Audio Router (Third Party Player)"
    │       ├── AudioWaveform/               # "Audio Waveform"
    │       ├── FaceCapture/                 # "Face Capture"
    │       ├── TransparentRender/           # "Transparent Render"
    │       ├── RtePlayer/                   # "URL Streaming (RTE Player)"
    │       ├── Simulcast/                   # "Simulcast"
    │       ├── Multipath/                   # "Multipath"
    │       └── VideoChat/                   # (disabled) Group Video Chat
    │
    ├── Resources/                           # Audio/video sample files, beauty resources
    ├── Assets.xcassets/
    ├── Base.lproj/                          # Main.storyboard, LaunchScreen.storyboard
    └── zh-Hans.lproj/                       # Chinese localization
```


## Case Registration Mechanism

Registration is **manual** via the `menus` array in `ViewController.swift`. No reflection or annotation scanning.

**`MenuItem` struct:**
```swift
struct MenuItem {
    var name: String        // display name in the list
    var entry: String       // storyboard ID of the entry VC (default: "EntryViewController")
    var storyboard: String  // storyboard file name (default: "Main")
    var controller: String  // storyboard ID of the main VC
    var note: String        // optional description
}
```

**Two navigation paths exist depending on `storyboard`:**

1. `storyboard == "Main"` — uses the shared `Main.storyboard`. The generic `EntryViewController` is instantiated, and `nextVCIdentifier` is set to `controller` to load the Main VC.
2. `storyboard != "Main"` — each example has its own `.storyboard` file. The VC with identifier `entry` (default `"EntryViewController"`) is instantiated directly from that storyboard.

Most examples use path 2 (their own storyboard).

**To add a case, edit exactly two things:**
1. Add a `MenuItem` to the `menus` array in `ViewController.swift`
2. Create the example folder under `Examples/Basic/` or `Examples/Advanced/` with the Swift file(s) and storyboard

## Entry/Main ViewController Pattern

Every example is split into two view controller roles:

**Entry** (`<ExampleName>Entry : UIViewController`)
- Collects user configuration before entering the example
- Passes configuration to Main via a `configs` dictionary

**Main** (`<ExampleName>Main : BaseViewController`)
- Owns the `AgoraRtcEngineKit` lifecycle for the duration of the example
- Implements `AgoraRtcEngineDelegate`
- Receives configuration exclusively through `configs`

## AgoraRtcEngineKit Lifecycle

```
viewDidLoad    → AgoraRtcEngineKit.sharedEngine(withAppId:delegate:)
               → engine.setVideoEncoderConfiguration / setChannelProfile
               → engine.joinChannel() (after permission granted)
                      ↓
                 [AgoraRtcEngineDelegate callbacks — may be on background thread]
                      ↓
viewDidDisappear / willMove(toParent:)
               → engine.leaveChannel()
               → AgoraRtcEngineKit.destroy()
```

## Token Flow

```swift
NetworkManager.shared.generateToken(channelName: channelId, uid: uid) { token in
    self.agoraKit?.joinChannel(byToken: token, channelId: channelId, uid: uid, mediaOptions: options)
}
```

If `KeyCenter.Certificate` is nil, token generation is skipped and a nil token is used — valid for projects without App Certificate.
