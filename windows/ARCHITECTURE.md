# Windows ARCHITECTURE

Windows example project using C++ + MFC (Microsoft Foundation Classes). Demonstrates Agora RTC SDK features through a collection of self-contained dialog-based examples organized by complexity.

## Technology Stack

- Language: C++
- UI Framework: MFC (Microsoft Foundation Classes)
- Architecture: Dialog-based application with example selection
- State: Member variables + message map callbacks

## Directory Structure

```
windows/
├── APIExample/
│   ├── APIExample/
│   │   ├── Basic/
│   │   │   └── <ExampleName>/
│   │   │       ├── C<ExampleName>Dlg.cpp
│   │   │       ├── C<ExampleName>Dlg.h
│   │   │       └── SKILL.md          # Per-example agent guide (present or forthcoming)
│   │   ├── Advanced/
│   │   │   └── <ExampleName>/
│   │   │       ├── C<ExampleName>Dlg.cpp
│   │   │       ├── C<ExampleName>Dlg.h
│   │   │       └── SKILL.md          # Per-example agent guide (present or forthcoming)
│   │   ├── DirectShow/          # DirectShow utilities
│   │   ├── dsound/              # DirectSound utilities
│   │   ├── res/                 # Resources (icons, dialogs, strings)
│   │   ├── APIExample.cpp
│   │   ├── APIExample.h
│   │   ├── APIExampleDlg.cpp    # Main dialog and example registration
│   │   ├── APIExampleDlg.h
│   │   ├── CConfig.cpp          # Configuration management
│   │   ├── CConfig.h
│   │   ├── CSceneDialog.cpp     # Shared dialog helper, not the case registration source
│   │   └── CSceneDialog.h
│   ├── APIExample.sln           # Visual Studio solution
│   └── .vscode/                 # VS Code configuration
├── .agents/skills/query-cases/  # Read-only case and registration discovery
├── .agents/skills/upsert-case/  # Add or modify a case
├── .agents/skills/review-case/  # Review lifecycle, threading, and conventions
├── AGENTS.md                    # Agent guide
└── ARCHITECTURE.md              # This file
```

## Architectural Rules

### Example Structure

Each example lives in its own folder under `APIExample/APIExample/Basic/` or `APIExample/APIExample/Advanced/` and consists of:
- A `.h` + `.cpp` pair for the dialog class
- Optional: Resource definitions in `.rc` file

### Dialog-Based Pattern

Each example is a dialog class that:
- Inherits from `CDialogEx` or `CDialog`
- Implements message handlers via `BEGIN_MESSAGE_MAP` / `END_MESSAGE_MAP`
- Manages its own Agora engine lifecycle
- Implements `IRtcEngineEventHandler` interface
- Owns all UI controls and state for that example

### Naming Convention

- Example folder names: PascalCase (e.g., `JoinChannelVideo`)
- Dialog class: `C<ExampleName>Dlg` (e.g., `CJoinChannelVideoDlg`)
- Header file: `C<ExampleName>Dlg.h`
- Implementation file: `C<ExampleName>Dlg.cpp`

### Menu Registration

All examples are registered in `APIExampleDlg.h` and `APIExampleDlg.cpp`. The localized scene name is wired through `Language.h`, `stdafx.cpp`, `en.ini`, and `zh-cn.ini`, and the example name should still match the folder name.

### Configuration Management

`CConfig` supplies the App ID and localized strings. `GET_APP_ID` expands to
`cs2utf8(CConfig::GetInstance()->GetAPP_ID())`; keep that UTF-8 value alive while initializing
the engine. Media settings belong to the selected case. Token input/generation is described
below and is not a static CConfig API.

### Common Utilities

All examples share utilities:
- `CConfig` — App ID and localized strings
- `VideoExtractor` — Video frame extraction
- `YUVReader` — YUV file reading
- DirectShow and DirectSound wrappers

## Case Index

| Case | Path | Key APIs | Description |
|------|------|----------|-------------|
| JoinChannelVideoByToken | `Basic/JoinChannelVideoByToken/` | `createAgoraRtcEngine()`, `joinChannel()` with token, `setupLocalVideo()`, `setupRemoteVideo()` | Basic video call with token authentication |
| LiveBroadcasting | `Basic/LiveBroadcasting/` | `setClientRole()`, `joinChannel()`, `startRtmpStreamWithTranscoding()` | Live broadcasting with RTMP streaming |
| AudioEffect | `Advanced/AudioEffect/` | `setAudioEffectPreset()`, `setVoiceBeautifierPreset()` | Audio effects and voice beautification |
| AudioMixing | `Advanced/AudioMixing/` | `startAudioMixing()`, `stopAudioMixing()`, `pauseAudioMixing()`, `resumeAudioMixing()` | Audio file mixing and playback control |
| AudioProfile | `Advanced/AudioProfile/` | `setAudioProfile()`, `setAudioScenario()` | Audio profile and scenario configuration |
| AudioVolume | `Advanced/AudioVolume/` | `adjustRecordingSignalVolume()`, `adjustPlaybackSignalVolume()`, `adjustUserPlaybackSignalVolume()` | Audio volume adjustment and control |
| Beauty | `Advanced/Beauty/` | `setBeautyEffectOptions()`, `setVideoEncoderConfiguration()` | Beauty filter and enhancement effects |
| Beauty2.0 | `Advanced/Beauty2.0/` | `createVideoEffectObject()`, `setVideoEffectStringParam()`, `setVideoEffectFloatParam()` | Enhanced beauty effects with v2.0 API |
| BeautyAudio | `Advanced/BeautyAudio/` | `setBeautyEffectOptions()`, `setAudioEffectPreset()` | Combined audio and video beauty effects |
| CrossChannel | `Advanced/CrossChannel/` | `startChannelMediaRelay()`, `updateChannelMediaRelay()`, `stopChannelMediaRelay()` | Media relay across multiple channels |
| CustomAudioCapture | `Advanced/CustomAudioCapture/` | `createCustomAudioTrack()`, `pushAudioFrame()`, `destroyCustomAudioTrack()` | Custom audio source capture |
| CustomEncrypt | `Advanced/CustomEncrypt/` | `setEncryptionConfig()`, `enableEncryption()` | Custom stream encryption |
| CustomVideoCapture | `Advanced/CustomVideoCapture/` | `setExternalVideoSource()`, `pushVideoFrame()` | Custom video source capture |
| LocalVideoTranscoding | `Advanced/LocalVideoTranscoding/` | `startLocalVideoTranscoding()`, `updateLocalTranscodingConfig()`, `stopLocalVideoTranscoding()` | Local video transcoding and composition |
| MediaEncrypt | `Advanced/MediaEncrypt/` | `setEncryptionConfig()`, `enableEncryption()` | Media stream encryption |
| MediaPlayer | `Advanced/MediaPlayer/` | `createMediaPlayer()`, `open()`, `play()`, `pause()`, `stop()` | Media file playback and control |
| MediaRecorder | `Advanced/MediaRecorder/` | `startRecording()`, `stopRecording()`, `setRecordingAudioFrameParameters()` | Media recording with custom parameters |
| Metadata | `Advanced/Metadata/` | `registerMediaMetadataObserver()`, `onMetadataReceived()` | Metadata transmission and reception |
| MultiCamera | `Advanced/MultiCamera/` | `enumerateDevices()`, `setDevice()` with multiple cameras | Multiple camera source selection |
| MultiChannel | `Advanced/MultiChannel/` | `createRtcChannel()`, `joinChannel()` on multiple channels | Join and manage multiple channels simultaneously |
| Multipath | `Advanced/Multipath/` | `enableMultipath()`, `setMultipathConfig()` | Multipath redundancy for reliability |
| MultiVideoSource | `Advanced/MultiVideoSource/` | `setExternalVideoSource()`, `pushVideoFrame()` with multiple sources | Multiple video sources |
| MultiVideoSourceTracks | `Advanced/MultiVideoSourceTracks/` | `createCustomVideoTrack()`, `pushVideoFrame()` on custom tracks | Multiple video tracks with custom sources |
| OriginalAudio | `Advanced/OriginalAudio/` | `setAudioFrameDelegate()`, `onPlaybackAudioFrame()` | Raw audio frame access |
| OriginalVideo | `Advanced/OriginalVideo/` | `setVideoFrameDelegate()`, `onCapturedVideoFrame()`, `onRemoteVideoFrame()` | Raw video frame access |
| PreCallTest | `Advanced/PreCallTest/` | `startEchoTest()`, `stopEchoTest()`, `startNetworkTest()`, `stopNetworkTest()` | Pre-call network and device testing |
| PushExternalVideoYUV | `Advanced/PushExternalVideoYUV/` | `setExternalVideoSource()`, `pushVideoFrame()` with YUV format | Push external YUV video frames |
| RegionConn | `Advanced/RegionConn/` | `setCloudProxy()`, `setRegion()` | Region connection and cloud proxy |
| ReportInCall | `Advanced/ReportInCall/` | `startRtcStats()`, `getRtcStats()` | In-call statistics and reporting |
| RtePlayer | `Advanced/RtePlayer/` | `createMediaPlayer()`, `open()` with RTE protocol | RTE protocol media playback |
| RTMPinject | `Advanced/RTMPinject/` | `addInjectStreamUrl()`, `removeInjectStreamUrl()` | RTMP stream injection |
| RTMPStream | `Advanced/RTMPStream/` | `startRtmpStreamWithTranscoding()`, `updateRtmpTranscodingConfig()`, `stopRtmpStream()` | RTMP streaming with live transcoding |
| ScreenShare | `Advanced/ScreenShare/` | `startScreenCapture()`, `updateScreenCaptureParameters()`, `stopScreenCapture()` | Screen sharing and capture |
| Simulcast | `Advanced/Simulcast/` | `setSimulcastConfig()`, `enableSimulcast()` | Simulcast streaming with multiple bitrates |
| SpatialAudio | `Advanced/SpatialAudio/` | `getLocalSpatialAudioEngine()`, `updateSelfPosition()`, `updateRemotePosition()` | 3D spatial audio positioning |
| TransparentBg | `Advanced/TransparentBg/` | `setVideoEncoderConfiguration()`, `setBeautyEffectOptions()` | Transparent background effects |

## Engine Lifecycle

`APIExampleDlg.cpp` owns the scene lifecycle:

1. `InitSceneDialog()` precreates each dialog. `OnInitDialog()` sets up UI only.
2. `CreateScene()` calls the selected dialog's `InitAgora()` and shows it. Create the engine
   with `createAgoraRtcEngine()`, initialize it with `RtcEngineContext`, and configure the
   media required by the case before joining.
3. SDK callbacks post messages to the dialog's UI thread through `IRtcEngineEventHandler`.
4. `ReleaseScene()` calls `UnInitAgora()` and hides the dialog. Invalidate pending requests,
   leave the channel, stop owned media, release the engine and clear its pointer here.

The current SDK's `release(nullptr)` completes destruction synchronously. Keep the callback
receiver alive until release finishes, then detach it and discard stale queued messages.
Do not release inside an SDK callback. Complete cleanup before the next scene initializes.
Hidden scene dialogs are not destroyed, so `PostNcDestroy()` or a destructor alone cannot
provide scene-exit cleanup. Standalone windows may use destruction hooks only when their
actual exit path destroys the window and matches host ownership.

See [upsert-case](.agents/skills/upsert-case/SKILL.md) and
[review-case](.agents/skills/review-case/SKILL.md) for registration and validation details.

## Token Flow

Use the case's explicit Token input (as in `Basic/JoinChannelVideoByToken/`) or its existing
asynchronous helper. CConfig has no GetToken method. Pass the same channel and UID used to
obtain the Token to one of the SDK's actual join overloads:

```cpp
// token, channelName and uid are values owned by the active case.
int result = m_rtcEngine->joinChannel(token.c_str(), channelName.c_str(), "", uid);
// Check result, then wait for onJoinChannelSuccess before showing a joined state.
```

Handle request failure and expiring tokens explicitly; renew only while the original
scene/engine is active. Production tokens should be generated server-side. Never stage
real App IDs, App Certificates or tokens.
