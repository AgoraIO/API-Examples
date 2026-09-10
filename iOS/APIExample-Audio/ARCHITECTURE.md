# ARCHITECTURE.md — APIExample-Audio

## Case Index

| Case | Path | Key APIs | Description |
|------|------|----------|-------------|
| JoinChannelAudio | `Examples/Basic/JoinChannelAudio/JoinChannelAudio.swift` | `joinChannel()`, `setAudioProfile()`, `setAudioScenario()`, `adjustRecordingSignalVolume()`, `enable(inEarMonitoring:)` | Basic audio call with profile, scenario, volume, and in-ear monitoring controls |
| JoinChannelAudio(Token) | `Examples/Basic/JoinChannelAudio(Token)/JoinChannelAudioToken.swift` | `joinChannel(byToken:)`, `setAudioProfile()`, `setAudioScenario()`, `adjustRecordingSignalVolume()` | Audio call with token authentication |
| VoiceChanger | `Examples/Advanced/VoiceChanger/VoiceChanger.swift` | `setVoiceBeautifierPreset()`, `setAudioEffectPreset()`, `setVoiceConversionPreset()`, `setLocalVoiceEqualizationOf()` | Voice beautifier, effects, conversion presets, and equalizer |
| CustomAudioSource | `Examples/Advanced/CustomAudioSource/CustomAudioSource.swift` | `setExternalAudioSource()` | Push custom audio via external audio source API |
| CustomPcmAudioSource | `Examples/Advanced/CustomPcmAudioSource/CustomPcmAudioSource.swift` | `createCustomAudioTrack()`, `enableCustomAudioLocalPlayback()`, `pushExternalAudioFrameRawData()` | Push custom PCM audio frames as mixable audio track |
| CustomAudioRender | `Examples/Advanced/CustomAudioRender/CustomAudioRender.swift` | `enableExternalAudioSink()`, `pullPlaybackAudioFrameRawData()` | Pull audio frames for custom rendering |
| RawAudioData | `Examples/Advanced/RawAudioData/RawAudioData.swift` | `setAudioFrameDelegate()` | Capture raw audio PCM data via delegate |
| AudioMixing | `Examples/Advanced/AudioMixing/AudioMixing.swift` | `startAudioMixing()`, `stopAudioMixing()`, `adjustAudioMixingVolume()`, `setEffectsVolume()` | Mix local audio file with microphone input |
| RhythmPlayer (hidden) | `Examples/Advanced/RhythmPlayer/RhythmPlayer.swift` | `startRhythmPlayer()`, `stopRhythmPlayer()` | Source retained for reference; hidden because the APIs are deprecated since RTC SDK 4.6.0 |
| PrecallTest | `Examples/Advanced/PrecallTest/PrecallTest.swift` | `startEchoTest()`, `stopEchoTest()`, `startLastmileProbeTest()` | Pre-call echo test and last-mile network probe |
| SpatialAudio | `Examples/Advanced/SpatialAudio/SpatialAudio.swift` | `createMediaPlayer()`, `updateChannel()`, `setEnableSpeakerphone()` | 3D spatial audio with media player integration |

## Directory Layout

```
APIExample-Audio/
├── Podfile                                  # CocoaPods dependencies (AgoraAudio_iOS, Floaty, AGEVideoLayout)
└── APIExample-Audio/
    ├── AppDelegate.swift
    ├── ViewController.swift                 # Root menu controller — MenuItem registration lives here
    ├── Info.plist
    ├── APIExample.entitlements
    ├── APIExample-Bridging-Header.h
    │
    ├── Common/
    │   ├── KeyCenter.swift                  # App ID and Certificate
    │   ├── GlobalSettings.swift             # Shared runtime config
    │   ├── BaseViewController.swift         # Base class all Main VCs must extend
    │   ├── EntryViewController.swift        # Generic Entry VC for storyboard == "Main" cases
    │   ├── LogViewController.swift          # Log viewer
    │   ├── AlertManager.swift
    │   ├── AgoraExtension.swift
    │   ├── StatisticsInfo.swift
    │   ├── UITypeAlias.swift
    │   ├── VideoView.swift / .xib           # Audio seat view (no video rendering)
    │   ├── Settings/                        # Settings UI components
    │   ├── Utils/                           # LogUtils, Util (privatization config)
    │   ├── NetworkManager/                  # Token request helper
    │   ├── ExternalAudio/                   # External audio source helpers
    │   └── ExternalVideo/                   # (unused in audio project)
    │
    ├── Examples/
    │   ├── Basic/
    │   │   ├── JoinChannelAudio/            # "Join a channel (Audio)"
    │   │   └── JoinChannelAudio(Token)/     # "Join a channel (Token)"
    │   └── Advanced/
    │       ├── VoiceChanger/                # "Voice Effects" — voice beautifier/effects
    │       ├── CustomAudioSource/           # "Custom Audio Source"
    │       ├── CustomPcmAudioSource/        # "Custom Audio Source (PCM)"
    │       ├── CustomAudioRender/           # "Custom Audio Render"
    │       ├── RawAudioData/                # "Raw Audio Data"
    │       ├── AudioMixing/                 # "Audio Mixing"
    │       ├── RhythmPlayer/                # Hidden — APIs deprecated since RTC SDK 4.6.0
    │       ├── PrecallTest/                 # "Pre-call Test"
    │       └── SpatialAudio/                # "Spatial Audio"
    │
    ├── Resources/                           # Audio sample files
    ├── Assets.xcassets/
    ├── Base.lproj/                          # Main.storyboard, LaunchScreen.storyboard
    └── zh-Hans.lproj/                       # Chinese localization
```

## Case Registration Mechanism

Registration is **manual** via the `menus` array in `ViewController.swift`. Identical to `APIExample`.

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

A registered case connects the `MenuItem`, its Swift controller(s), and its storyboard.
New source files must belong to the Xcode target's Sources build phase, and storyboards,
localized files, and media assets must belong to Resources. Follow
[upsert-case](.agents/skills/upsert-case/SKILL.md) for the complete change procedure and
update the Case Index when the case changes.

## Entry/Main ViewController Pattern

Identical to `APIExample`:

**Entry** (`<ExampleName>Entry : UIViewController`)
- Collects user configuration before entering the example
- Passes configuration to Main via a `configs` dictionary

**Main** (`<ExampleName>Main : BaseViewController`)
- Owns the `AgoraRtcEngineKit` lifecycle for the duration of the example
- Implements `AgoraRtcEngineDelegate`
- Receives configuration exclusively through `configs`
- UI contains only audio controls — no video rendering views

## Audio-Only Constraint

This project uses `AgoraAudio_iOS` SDK which has no video module. Main view controllers must NOT include:
- Video rendering views or video canvas setup
- Calls to `enableVideo()`, `setupLocalVideo()`, `setupRemoteVideo()`
- Camera-related APIs

All UI is limited to audio controls, status indicators, and effect parameter inputs.

## AgoraRtcEngineKit Lifecycle

```
viewDidLoad    → AgoraRtcEngineKit.sharedEngine(withAppId:delegate:)
               → engine.setAudioProfile / setAudioScenario
               → engine.joinChannel() (after RECORD_AUDIO permission granted)
                      ↓
                 [AgoraRtcEngineDelegate callbacks — may be on background thread]
                      ↓
willMove(toParent:) when parent == nil
               → engine.leaveChannel()
               → AgoraRtcEngineKit.destroy()
```

For navigation-based cases, cleanup runs when the controller is removed from its parent.
Temporary disappearance alone must not destroy the engine; follow the guarded cleanup in
the upsert and review skills.

## Token Flow

Use the guarded permission → Token → join implementation in
[upsert-case](.agents/skills/upsert-case/SKILL.md). Snapshot channel, UID, request generation
and engine identity before asynchronous work. Recheck them on main before each continuation;
leave/destroy invalidates pending requests and destroy clears engine ownership. Weak capture
alone does not protect a still-alive controller whose RTC session has ended.

A nil/empty Token is allowed only when no App Certificate is configured. Reject a missing
required Token and check the SDK join return code without logging credentials.
