# ARCHITECTURE.md — APIExample-SwiftUI

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
