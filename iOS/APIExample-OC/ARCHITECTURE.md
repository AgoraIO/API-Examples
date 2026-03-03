# ARCHITECTURE.md — APIExample-OC

## Directory Layout

```
APIExample-OC/
├── Podfile                                  # CocoaPods dependencies (AgoraRtcEngine_iOS)
├── SimpleFilter/                            # Optional C++ audio/video extension module
├── Agora-ScreenShare-Extension-OC/          # ReplayKit broadcast extension for screen sharing
├── libs/                                    # Local SDK frameworks (when not using CocoaPods)
├── zh-Hans.lproj/                           # Chinese localization (project level)
└── APIExample-OC/
    ├── main.m
    ├── AppDelegate.h / .m
    ├── ViewController.h / .m                # Root menu controller — MenuItem registration lives here
    ├── Info.plist
    ├── APIExample-Bridging-Header.h
    │
    ├── Common/
    │   ├── KeyCenter.h / .m                 # App ID and Certificate
    │   ├── BaseViewController.h / .m        # Base class all Main VCs must extend
    │   ├── VideoView.h / .m / .xib          # Reusable video rendering view
    │   ├── Views/                           # Reusable UI components
    │   ├── Utils/                           # LogUtils, GlobalSettings, Util (privatization config)
    │   ├── NetworkManager/                  # Token request helper
    │   ├── ExternalAudio/                   # External audio source helpers
    │   ├── ExternalVideo/                   # External video source helpers
    │   └── CustomEncryption/                # Custom stream encryption helpers
    │
    ├── Examples/
    │   ├── Basic/
    │   │   ├── JoinChannelVideo/            # "Join a channel (Video)"
    │   │   ├── JoinChannelVideo(Token)/     # "Join a channel (Token)"
    │   │   ├── JoinChannelVideo(Recorder)/  # "Local or remote recording"
    │   │   └── JoinChannelAudio/            # "Join a channel (Audio)"
    │   └── Advanced/
    │       ├── LiveStreaming/               # "Live Streaming"
    │       ├── RTMPStreaming/               # "RTMP Streaming"
    │       ├── VideoMetadata/               # "Video Metadata"
    │       ├── VoiceChanger/                # "Voice Changer"
    │       ├── CustomPcmAudioSource/        # "Custom Audio Source"
    │       ├── CustomAudioRender/           # "Custom Audio Render"
    │       ├── CustomVideoSourcePush/       # "Custom Video Source (Push)"
    │       ├── CustomVideoRender/           # "Custom Video Render"
    │       ├── RawAudioData/                # "Raw Audio Data"
    │       ├── RawVideoData/                # "Raw Video Data"
    │       ├── PictureInPicture/            # "Picture In Picture (iOS15+)"
    │       ├── SimpleFilter/                # "Simple Filter Extension"
    │       ├── JoinMultiChannel/            # "Join Multiple Channels"
    │       ├── StreamEncryption/            # "Stream Encryption"
    │       ├── AudioMixing/                 # "Audio Mixing"
    │       ├── MediaPlayer/                 # "Media Player"
    │       ├── ScreenShare/                 # "Screen Share"
    │       ├── VideoProcess/                # "Video Process"
    │       ├── RhythmPlayer/                # "Rhythm Player"
    │       ├── CreateDataStream/            # "Create Data Stream"
    │       ├── MediaChannelRelay/           # "Media Channel Relay"
    │       ├── SpatialAudio/                # "Spatial Audio"
    │       ├── ContentInspect/              # "Content Inspect"
    │       ├── MutliCamera/                 # "Multi Camera (iOS13+)"
    │       ├── Simulcast/                   # "Simulcast"
    │       ├── Multipath/                   # "Multipath"
    │       └── LocalCompositeGraph/         # "Local Composite Graph"
    │
    ├── Resources/                           # Audio/video sample files
    ├── Assets.xcassets/
    ├── en.lproj/                            # English localization
    └── zh-Hans.lproj/                       # Chinese localization
```

## Case Registration Mechanism

Registration is **manual** via the `+[MenuSection menus]` method in `ViewController.m`. No reflection or annotation scanning.

**`MenuItem` class:**
```objc
@interface MenuItem : NSObject
@property(nonatomic, copy) NSString *name;        // display name in the list
@property(nonatomic, copy) NSString *entry;       // storyboard ID of the entry VC (default: "EntryViewController")
@property(nonatomic, copy) NSString *storyboard;  // storyboard file name
@property(nonatomic, copy) NSString *controller;  // (unused in current implementation)
@property(nonatomic, copy) NSString *note;        // optional description
@end
```

Each example has its own `.storyboard` file. The VC with identifier `entry` (default `"EntryViewController"`) is instantiated directly from that storyboard.

**To add a case, edit exactly two things:**
1. Add a `MenuItem` to the `+[MenuSection menus]` method in `ViewController.m`:
   ```objc
   [[MenuItem alloc] initWithName:@"My New Case".localized storyboard:@"MyNewCase" controller:@""]
   ```
2. Create the example folder under `Examples/Basic/` or `Examples/Advanced/` with the `.h/.m` files and storyboard

## Entry/Main ViewController Pattern

Every example is split into two view controller roles:

**Entry** (`<ExampleName>Entry : UIViewController`)
- Collects user configuration before entering the example
- Passes configuration to Main via a `configs` dictionary (`NSDictionary`)

**Main** (`<ExampleName>Main : BaseViewController`)
- Owns the `AgoraRtcEngineKit` lifecycle for the duration of the example
- Conforms to `AgoraRtcEngineDelegate`
- Receives configuration exclusively through `configs`

## AgoraRtcEngineKit Lifecycle

```
viewDidLoad    → [AgoraRtcEngineKit sharedEngineWithAppId:delegate:]
               → [engine setVideoEncoderConfiguration:] / [engine setChannelProfile:]
               → [engine joinChannelByToken:...] (after permission granted)
                      ↓
                 [AgoraRtcEngineDelegate callbacks — may be on background thread]
                      ↓
viewDidDisappear / dealloc
               → [engine leaveChannel:]
               → [AgoraRtcEngineKit destroy]
```

## Token Flow

```objc
[[NetworkManager shared] generateTokenWithChannelName:channelName success:^(NSString *token) {
    [self.agoraKit joinChannelByToken:token channelId:channelName uid:0 mediaOptions:options];
}];
```
