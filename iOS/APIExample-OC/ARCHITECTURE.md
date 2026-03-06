# ARCHITECTURE.md — APIExample-OC

## Case Index

| Case | Path | Key APIs | Description |
|------|------|----------|-------------|
| JoinChannelVideo | `Examples/Basic/JoinChannelVideo/JoinChannelVideo.m` | `joinChannelByToken:`, `setupLocalVideo:`, `setupRemoteVideo:` | Basic video call — join channel and render local/remote video |
| JoinChannelVideo(Token) | `Examples/Basic/JoinChannelVideo(Token)/JoinChannelVideoToken.m` | `joinChannelByToken:`, `setupLocalVideo:`, `setupRemoteVideo:` | Video call with token authentication |
| JoinChannelVideo(Recorder) | `Examples/Basic/JoinChannelVideo(Recorder)/JoinChannelVideoRecorder.m` | `createMediaRecorder:`, `joinChannelByToken:`, `setupLocalVideo:` | Local and remote stream recording |
| JoinChannelAudio | `Examples/Basic/JoinChannelAudio/JoinChannelAudio.m` | `joinChannelByToken:`, `setAudioProfile:`, `enableAudioVolumeIndication:` | Basic audio call |
| LiveStreaming | `Examples/Advanced/LiveStreaming/LiveStreaming.m` | `setClientRole:`, `setVideoScenario:`, `preloadChannelByToken:`, `enableInstantMediaRendering` | Interactive live streaming with role switching |
| RTMPStreaming | `Examples/Advanced/RTMPStreaming/RTMPStreaming.m` | `startRtmpStreamWithoutTranscoding:`, `startRtmpStreamWithTranscoding:`, `updateRtmpTranscoding:`, `stopRtmpStream:` | Push stream to CDN with optional transcoding |
| VideoMetadata | `Examples/Advanced/VideoMetadata/VideoMetadata.m` | `setMediaMetadataDataSource:withType:`, `setMediaMetadataDelegate:withType:` | Send and receive metadata attached to video stream |
| VoiceChanger | `Examples/Advanced/VoiceChanger/VoiceChanger.m` | `setVoiceBeautifierPreset:`, `setAudioEffectPreset:`, `setVoiceConversionPreset:` | Voice beautifier, effects, and conversion presets |
| CustomPcmAudioSource | `Examples/Advanced/CustomPcmAudioSource/CustomPcmAudioSource.m` | `createCustomAudioTrack:config:`, `enableCustomAudioLocalPlayback:enabled:`, `pushExternalAudioFrameRawData:` | Push custom PCM audio frames as external audio source |
| CustomAudioRender | `Examples/Advanced/CustomAudioRender/CustomAudioRender.m` | `enableExternalAudioSink:sampleRate:channels:`, `pullPlaybackAudioFrameRawData:lengthInByte:` | Pull audio frames for custom rendering |
| CustomVideoSourcePush | `Examples/Advanced/CustomVideoSourcePush/CustomVideoSourcePush.m` | `setExternalVideoSource:useTexture:sourceType:`, `pushExternalVideoFrame:videoTrackId:` | Push external video frames as custom video source |
| CustomVideoRender | `Examples/Advanced/CustomVideoRender/CustomVideoRender.m` | `setVideoFrameDelegate:` | Custom rendering of remote video frames via delegate |
| RawAudioData | `Examples/Advanced/RawAudioData/RawAudioData.m` | `setAudioFrameDelegate:` | Capture raw audio PCM data via delegate |
| RawVideoData | `Examples/Advanced/RawVideoData/RawVideoData.m` | `setVideoFrameDelegate:` | Capture raw video frames via delegate |
| SimpleFilter | `Examples/Advanced/SimpleFilter/SimpleFilter.m` | `enableExtensionWithVendor:extension:enabled:`, `setExtensionPropertyWithVendor:extension:key:value:` | Apply audio/video filter via Agora Extension API |
| JoinMultiChannel | `Examples/Advanced/JoinMultiChannel/JoinMultiChannel.m` | `joinChannelExByToken:connection:delegate:mediaOptions:` | Join multiple channels simultaneously via ex connection |
| StreamEncryption | `Examples/Advanced/StreamEncryption/StreamEncryption.m` | `enableEncryption:encryptionConfig:` | Built-in and custom stream encryption |
| AudioMixing | `Examples/Advanced/AudioMixing/AudioMixing.m` | `startAudioMixing:loopback:cycle:`, `adjustAudioMixingVolume:`, `setEffectsVolume:` | Mix local audio file with microphone input |
| MediaPlayer | `Examples/Advanced/MediaPlayer/MediaPlayer.m` | `createMediaPlayerWithDelegate:`, `updateChannelExWithMediaOptions:connection:` | Play media files and publish to channel via media player |
| ScreenShare | `Examples/Advanced/ScreenShare/ScreenShare.m` | `startScreenCapture:`, `updateScreenCapture:`, `stopScreenCapture` | Screen capture and sharing via ReplayKit extension |
| LocalCompositeGraph | `Examples/Advanced/LocalCompositeGraph/LocalCompositeGraph.m` | `startLocalVideoTranscoder:`, `startCameraCapture:config:`, `enableVirtualBackground:backData:segData:` | Composite multiple video sources locally before publishing |
| VideoProcess | `Examples/Advanced/VideoProcess/VideoProcess.m` | `setBeautyEffectOptions:options:`, `enableVirtualBackground:backData:segData:`, `enableExtensionWithVendor:` | Built-in beauty, virtual background, and video enhancement |
| RhythmPlayer | `Examples/Advanced/RhythmPlayer/RhythmPlayer.m` | `startRhythmPlayer:sound2:config:`, `stopRhythmPlayer` | Play metronome-style rhythm audio |
| CreateDataStream | `Examples/Advanced/CreateDataStream/CreateDataStream.m` | `createDataStream:config:`, `sendStreamMessage:data:` | Create and send data stream messages between users |
| MediaChannelRelay | `Examples/Advanced/MediaChannelRelay/MediaChannelRelay.m` | `startOrUpdateChannelMediaRelay:`, `stopChannelMediaRelay`, `pauseAllChannelMediaRelay`, `resumeAllChannelMediaRelay` | Relay media stream to multiple destination channels |
| SpatialAudio | `Examples/Advanced/SpatialAudio/SpatialAudio.m` | `createMediaPlayerWithDelegate:`, `updateChannelWithMediaOptions:` | 3D spatial audio with media player integration |
| ContentInspect | `Examples/Advanced/ContentInspect/ContentInspect.m` | `enableContentInspect:config:`, `switchCamera` | Moderate content in video stream |
| MutliCamera | `Examples/Advanced/MutliCamera/MutliCamera.m` | `enableMultiCamera:config:`, `startCameraCapture:config:`, `stopCameraCapture:` | Capture from front and back cameras simultaneously (iOS 13+) |
| PictureInPicture | `Examples/Advanced/PictureInPicture/PictureInPicture.m` | `setVideoFrameDelegate:`, `AVPictureInPictureController` | Picture-in-Picture using AVKit (iOS 15+) |
| Simulcast | `Examples/Advanced/Simulcast/Simulcast.m` | `setSimulcastConfig:`, `setRemoteVideoStream:type:` | Publish multiple video quality layers simultaneously |
| Multipath | `Examples/Advanced/Multipath/Multipath.m` | `updateChannelWithMediaOptions:` | Multi-path network transmission configuration |

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
