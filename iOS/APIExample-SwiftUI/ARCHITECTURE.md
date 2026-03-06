# ARCHITECTURE.md — APIExample-SwiftUI

## Case Index

| Case | Path | Key APIs | Description |
|------|------|----------|-------------|
| JoinChannelVideo | `Examples/Basic/JoinChannelVideo/` | `joinChannel()`, `setupLocalVideo()`, `setupRemoteVideo()` | Basic video call — join channel and render local/remote video |
| JoinChannelVideo(Token) | `Examples/Basic/JoinChannelVideo(Token)/` | `joinChannel(byToken:)`, `setupLocalVideo()`, `setupRemoteVideo()` | Video call with token authentication |
| JoinChannelVideo(Recorder) | `Examples/Basic/JoinChannelVideo(Recorder)/` | `createMediaRecorder()`, `joinChannel()`, `setupLocalVideo()` | Local and remote stream recording |
| JoinChannelAudio | `Examples/Basic/JoinChannelAudio/` | `joinChannel()`, `setAudioProfile()`, `setAudioScenario()`, `adjustRecordingSignalVolume()`, `enable(inEarMonitoring:)` | Basic audio call with profile, scenario, and volume controls |
| LiveStreaming | `Examples/Advanced/LiveStreaming/` | `setClientRole()`, `setVideoScenario()`, `preloadChannel()`, `enableInstantMediaRendering()` | Interactive live streaming with role switching |
| RTMPStream | `Examples/Advanced/RTMPStream/` | `startRtmpStreamWithoutTranscoding()`, `startRtmpStream(withTranscoding:)`, `updateRtmpTranscoding()`, `stopRtmpStream()` | Push stream to CDN with optional transcoding |
| VideoMetadata | `Examples/Advanced/VideoMetadata/` | `setMediaMetadataDataSource()`, `setMediaMetadataDelegate()` | Send and receive metadata attached to video stream |
| VoiceChanger | `Examples/Advanced/VoiceChanger/` | `setVoiceBeautifierPreset()`, `setAudioEffectPreset()`, `setVoiceConversionPreset()`, `setLocalVoiceFormant()` | Voice beautifier, effects, and conversion presets |
| CustomPCMAudioSource | `Examples/Advanced/CustomPCMAudioSource/` | `createCustomAudioTrack()`, `enableCustomAudioLocalPlayback()`, `pushExternalAudioFrameRawData()` | Push custom PCM audio frames as external audio source |
| CustomAudioRender | `Examples/Advanced/CustomAudioRender/` | `enableExternalAudioSink()`, `pullPlaybackAudioFrameRawData()` | Pull audio frames for custom rendering |
| RawAudioData | `Examples/Advanced/RawAudioData/` | `setAudioFrameDelegate()`, `sendAudioMetadata()` | Capture raw audio PCM data via delegate |
| RawVideoData | `Examples/Advanced/RawVideoData/` | `setVideoFrameDelegate()` | Capture raw video frames via delegate |
| PictureInPicture | `Examples/Advanced/PictureInPicture/` | `AVPictureInPictureController`, `joinChannel()`, `setVideoFrameDelegate()` | Picture-in-Picture using AVKit (iOS 15+) |
| QuickSwitchChannel | `Examples/Advanced/QuickSwitchChannel/` | `joinChannel()`, `leaveChannel()` | Quickly switch between channels as audience |
| JoinMultiChannel | `Examples/Advanced/JoinMultiChannel/` | `joinChannelEx()`, `takeSnapshotEx()` | Join multiple channels simultaneously via ex connection |
| StreamEncryption | `Examples/Advanced/StreamEncryption/` | `enableEncryption()` | Built-in and custom stream encryption |
| AudioMixing | `Examples/Advanced/AudioMixing/` | `startAudioMixing()`, `stopAudioMixing()`, `adjustAudioMixingVolume()`, `setEffectsVolume()` | Mix local audio file with microphone input |
| PrecallTest | `Examples/Advanced/PrecallTest/` | `startEchoTest()`, `stopEchoTest()`, `startLastmileProbeTest()` | Pre-call echo test and last-mile network probe |
| MediaPlayer | `Examples/Advanced/MediaPlayer/` | `createMediaPlayer()`, `updateChannelEx()` | Play media files and publish to channel via media player |
| ScreenShare | `Examples/Advanced/ScreenShare/` | `startScreenCapture()`, `updateScreenCapture()`, `stopScreenCapture()` | Screen capture and sharing via ReplayKit extension |
| LocalVideoTranscoding | `Examples/Advanced/LocalVideoTranscoding/` | `startLocalVideoTranscoder()`, `startCameraCapture()`, `createMediaPlayer()` | Transcode multiple video sources locally before publishing |
| LocalVideoComposition | `Examples/Advanced/LocalVideoComposition/` | `startLocalVideoTranscoder()`, `startCameraCapture()`, `startScreenCapture()` | Composite camera and screen capture into one stream |
| VideoProcess | `Examples/Advanced/VideoProcess/` | `setBeautyEffectOptions()`, `enableVirtualBackground()`, `enableExtension()` | Built-in beauty, virtual background, and video enhancement |
| AgoraBeauty | `Examples/Advanced/AgoraBeauty/` | `enableExtension()`, `createVideoEffectObject()`, `setFilterEffectOptions()` | Agora beauty extension with makeup and virtual background |
| RhythmPlayer | `Examples/Advanced/RhythmPlayer/` | `startRhythmPlayer()`, `stopRhythmPlayer()` | Play metronome-style rhythm audio |
| CreateDataStream | `Examples/Advanced/CreateDataStream/` | `createDataStream()`, `sendStreamMessage()` | Create and send data stream messages between users |
| MediaChannelRelay | `Examples/Advanced/MediaChannelRelay/` | `startOrUpdateChannelMediaRelay()`, `stopChannelMediaRelay()`, `pauseAllChannelMediaRelay()` | Relay media stream to multiple destination channels |
| SpatialAudio | `Examples/Advanced/SpatialAudio/` | `createMediaPlayer()`, `updateChannel()` | 3D spatial audio with media player integration |
| ContentInspect | `Examples/Advanced/ContentInspect/` | `enableContentInspect()`, `switchCamera()` | Moderate content in video stream |
| MutliCamera | `Examples/Advanced/MutliCamera/` | `enableMultiCamera()`, `startCameraCapture()`, `stopCameraCapture()` | Capture from front and back cameras simultaneously (iOS 13+) |
| KtvCopyrightMusic | `Examples/Advanced/KtvCopyrightMusic/` | — | Links to KTV copyright music documentation |
| ARKit | `Examples/Advanced/ARKit/` | `setVideoFrameDelegate()`, `enableInstantMediaRendering()`, `startMediaRenderingTracing()` | Push ARKit face tracking frames as custom video source |
| AudioWaveform | `Examples/Advanced/AudioWaveform/` | `setAudioProfile()`, `enableAudioVolumeIndication()` | Visualize audio waveform from volume callbacks |
| FaceCapture | `Examples/Advanced/FaceCapture/` | `enableExtension()`, `setExtensionPropertyWithVendor()`, `setFaceInfoDelegate()` | Face capture and lip sync via Agora extension |
| Simulcast | `Examples/Advanced/Simulcast/` | `setSimulcastConfig()`, `setRemoteVideoStream()` | Publish multiple video quality layers simultaneously |
| Multipath | `Examples/Advanced/Multipath/` | `updateChannel()` | Multi-path network transmission configuration |

## Directory Layout

```
APIExample-SwiftUI/
├── Podfile                                  # CocoaPods dependencies (AgoraRtcEngine_iOS)
├── Agora-ScreenShare-Extension/             # ReplayKit broadcast extension for screen sharing
├── libs/                                    # Local SDK frameworks (when not using CocoaPods)
└── APIExample-SwiftUI/
    ├── APIExample_SwiftUIApp.swift          # App entry point (@main)
    ├── ContentView.swift                    # Root navigation — MenuItem registration lives here
    ├── Info.plist
    ├── APIExample-Bridging-Header.h
    │
    ├── Common/
    │   ├── KeyCenter.swift                  # App ID and Certificate
    │   ├── AgoraExtension.swift
    │   ├── PickerView.swift
    │   ├── StatisticsInfo.swift
    │   ├── VideoView.swift                  # SwiftUI wrapper for video rendering
    │   ├── VideoUIView.swift                # UIKit video view
    │   ├── ViewExtensions.swift
    │   ├── View/                            # Reusable SwiftUI components
    │   ├── Settings/                        # GlobalSettings
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
    │       ├── LiveStreaming/               # "Live Streaming"
    │       ├── RTMPStream/                  # "RTMP Streaming"
    │       ├── VideoMetadata/               # "Video Metadata"
    │       ├── VoiceChanger/                # "Voice Changer"
    │       ├── CustomPCMAudioSource/        # "Custom Audio Source (PCM)"
    │       ├── CustomAudioRender/           # "Custom Audio Render"
    │       ├── RawAudioData/                # "Raw Audio Data"
    │       ├── RawVideoData/                # "Raw Video Data"
    │       ├── PictureInPicture/            # "Picture In Picture"
    │       ├── QuickSwitchChannel/          # "Quick Switch Channel"
    │       ├── JoinMultiChannel/            # "Join Multiple Channels"
    │       ├── StreamEncryption/            # "Stream Encryption"
    │       ├── AudioMixing/                 # "Audio Mixing"
    │       ├── PrecallTest/                 # "Precall Test"
    │       ├── MediaPlayer/                 # "Media Player"
    │       ├── ScreenShare/                 # "Screen Share"
    │       ├── LocalVideoTranscoding/       # "Local Video Transcoding"
    │       ├── LocalVideoComposition/       # "Local Composite Graph"
    │       ├── VideoProcess/                # "Video Process"
    │       ├── AgoraBeauty/                 # "Agora Beauty"
    │       ├── RhythmPlayer/                # "Rhythm Player"
    │       ├── CreateDataStream/            # "Create Data Stream"
    │       ├── MediaChannelRelay/           # "Media Channel Relay"
    │       ├── SpatialAudio/                # "Spatial Audio"
    │       ├── ContentInspect/              # "Content Inspect"
    │       ├── MutliCamera/                 # "Multi Camera (iOS13+)"
    │       ├── KtvCopyrightMusic/           # "KTV Copyright Music"
    │       ├── ARKit/                       # "ARKit"
    │       ├── AudioWaveform/               # "Audio Waveform"
    │       ├── FaceCapture/                 # "Face Capture"
    │       ├── Simulcast/                   # "Simulcast"
    │       └── Multipath/                   # "Multipath"
    │
    ├── Resources/                           # Audio/video sample files
    ├── Assets.xcassets/
    └── Preview Content/
```

## Case Registration Mechanism

Registration is **manual** via the `menus` array in `ContentView.swift`. No reflection or annotation scanning.

**`MenuItem` struct:**
```swift
struct MenuItem: Identifiable {
    let id = UUID()
    var name: String
    var view: AnyView   // the Entry view wrapped in AnyView
}
```

Navigation uses SwiftUI `NavigationLink`. Each `MenuItem` holds an `AnyView` wrapping the Entry view.

**To add a case, edit exactly two things:**
1. Add a `MenuItem` to the `menus` array in `ContentView.swift`:
   ```swift
   MenuItem(name: "My New Case".localized, view: AnyView(MyNewCaseEntry()))
   ```
2. Create the example folder under `Examples/Basic/` or `Examples/Advanced/` with the Swift files

## Entry/RTC Pattern

Every example is split into two parts:

**Entry** (`<ExampleName>Entry : View`)
- A SwiftUI View that collects user configuration (channel name, etc.)
- Uses `NavigationLink` to navigate to the main view
- Passes configuration via a `configs` dictionary

**RTC** (`<ExampleName>RTC : NSObject, ObservableObject, AgoraRtcEngineDelegate`)
- Owns the `AgoraRtcEngineKit` lifecycle
- Exposes state to the View via `@Published` properties
- Implements all delegate callbacks

**Main View** (`<ExampleName> : View`)
- Holds the RTC object as `@ObservedObject`
- Calls `setupRTC()` in `.onAppear`
- Calls `onDestroy()` in `.onDisappear`

## Video Rendering

UIKit video views (`VideoUIView`) are bridged into SwiftUI via `UIViewRepresentable` (`VideoView`). The RTC class owns the `UIView` instances; the SwiftUI View wraps them for display.

## AgoraRtcEngineKit Lifecycle

```
.onAppear      → setupRTC()
               → AgoraRtcEngineKit.sharedEngine(with:delegate:)
               → engine.setVideoEncoderConfiguration / setClientRole
               → engine.joinChannel() (after token generation)
                      ↓
                 [AgoraRtcEngineDelegate callbacks]
                      ↓
.onDisappear   → onDestroy()
               → engine.leaveChannel()
               → AgoraRtcEngineKit.destroy()
```

## Token Flow

```swift
NetworkManager.shared.generateToken(channelName: channelName) { token in
    self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: 0, mediaOptions: option)
}
```
