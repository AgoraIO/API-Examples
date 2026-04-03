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
│   ├── cicd/                    # CI/CD scripts
│   └── .vscode/                 # VS Code configuration
├── .agent/skills/               # Agent skills
│   ├── create-api-example/
│   ├── find-api-example/
│   └── migrate-api-to-project/
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
- Implements `IAgoraRtcEngineEventHandler` interface
- Owns all UI controls and state for that example

### Naming Convention

- Example folder names: PascalCase (e.g., `JoinChannelVideo`)
- Dialog class: `C<ExampleName>Dlg` (e.g., `CJoinChannelVideoDlg`)
- Header file: `C<ExampleName>Dlg.h`
- Implementation file: `C<ExampleName>Dlg.cpp`

### Menu Registration

All examples are registered in `APIExampleDlg.h` and `APIExampleDlg.cpp`. The localized scene name is wired through `Language.h`, `stdafx.cpp`, `en.ini`, and `zh-cn.ini`, and the example name should still match the folder name.

### Configuration Management

Configuration is centralized in `CConfig` class:
- App ID management
- Token generation
- Global settings (resolution, frame rate, etc.)

### Common Utilities

All examples share utilities:
- `CConfig` — App ID, token, and global settings
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
| Beauty2.0 | `Advanced/Beauty2.0/` | `setBeautyEffectOptions()` with v2.0 API | Enhanced beauty effects with v2.0 API |
| BeautyAudio | `Advanced/BeautyAudio/` | `setBeautyEffectOptions()`, `setAudioEffectPreset()` | Combined audio and video beauty effects |
| CrossChannel | `Advanced/CrossChannel/` | `startChannelMediaRelay()`, `updateChannelMediaRelay()`, `stopChannelMediaRelay()` | Media relay across multiple channels |
| CustomAudioCapture | `Advanced/CustomAudioCapture/` | `setExternalAudioSource()`, `pushAudioFrame()` | Custom audio source capture |
| CustomEncrypt | `Advanced/CustomEncrypt/` | `setEncryptionConfig()`, `enableEncryption()` | Custom stream encryption |
| CustomVideoCapture | `Advanced/CustomVideoCapture/` | `setExternalVideoSource()`, `pushVideoFrame()` | Custom video source capture |
| FaceCapture | `Advanced/FaceCapture/` | `enableFaceDetection()`, `getFaceDetectionResult()` | Face detection and capture |
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

```
1. Create Engine
   createAgoraRtcEngine()
   
2. Initialize Engine
   initialize(RtcEngineContext)
   
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
   
8. Release Engine
   release()
```

## Token Flow

Token is obtained from `CConfig` and passed to `joinChannel()`:

```cpp
const char* token = CConfig::GetToken(channelName);
m_rtcEngine->joinChannel(token, channelName, "", 0);
```

For production, tokens should be generated server-side and refreshed before expiration.
