# ARCHITECTURE.md — APIExample-Compose

## Directory Layout

```
APIExample-Compose/
├── gradle.properties                        # rtc_sdk_version
├── AGENTS.md                                # Agent entry point — build commands, red lines, skill index
├── ARCHITECTURE.md                          # This file — directory layout, patterns, registration
├── .agents/
│   └── skills/
│       ├── upsert-case/
│       │   ├── SKILL.md                     # Add or update a Compose case
│       │   └── references/
│       │       └── composable-template.kt   # Compose lifecycle/state reference skeleton
│       ├── query-cases/
│       │   └── SKILL.md                     # Query existing cases by API, group, or list position
│       └── review-case/
│           └── SKILL.md                     # Review a Compose case against project red lines
└── app/src/main/
    ├── AndroidManifest.xml
    ├── assets/                              # Audio/video sample files
    ├── res/
    │   └── values/strings.xml               # Display name strings (prefix: example_*)
    └── java/io/agora/api/example/compose/
        ├── APIExampleApp.kt                 # Application class
        ├── MainActivity.kt                  # Single-Activity, sets content to NavGraph()
        ├── NavGraph.kt                      # Compose Navigation host — home / settings / example
        │
        ├── model/
        │   ├── Example.kt                   # data class: name: Int, content: @Composable
        │   ├── Examples.kt                  # Hardcoded lists: BasicExampleList, AdvanceExampleList
        │   └── Components.kt                # Groups the two lists into Components for the home screen
        │
        ├── samples/                         # One .kt file per case — all @Composable
        │   ├── JoinChannelVideoToken.kt     # Basic: "Join Video Channel (With Token)"
        │   ├── JoinChannelVideo.kt          # Basic: "Join Video Channel" — canonical reference
        │   ├── JoinChannelAudio.kt          # Basic: "Join Audio Channel"
        │   ├── LiveStreaming.kt             # Advanced: "Live Streaming" — setClientRole
        │   ├── RTMPStreaming.kt             # Advanced: "RTMP Streaming" — push to CDN
        │   ├── MediaMetadata.kt             # Advanced: "Media Metadata" — send/receive metadata
        │   ├── VoiceEffects.kt              # Advanced: "Voice Effects" — voice beautifier/effects
        │   ├── OriginAudioData.kt           # Advanced: "Origin Audio Data" — raw audio processing
        │   ├── CustomAudioSource.kt         # Advanced: "Custom Audio Source" — push external audio
        │   ├── CustomAudioRender.kt         # Advanced: "Custom Audio Render" — pull audio rendering
        │   ├── OriginVideoData.kt           # Advanced: "Origin Video Data" — raw video processing
        │   ├── CustomVideoSource.kt         # Advanced: "Custom Video Source" — push external video
        │   ├── CustomVideoRender.kt         # Advanced: "Custom Video Render" — custom video rendering
        │   ├── PictureInPicture.kt          # Advanced: "Picture In Picture" — PiP mode
        │   ├── JoinMultiChannel.kt          # Advanced: "Join Multi Channel" — multi-channel join
        │   ├── ChannelEncryption.kt         # Advanced: "Channel Encryption" — built-in encryption
        │   ├── PlayAudioFiles.kt            # Advanced: "Play Audio Files" — audio mixing
        │   ├── PreCallTest.kt               # Advanced: "Pre Call Test" — network/device test
        │   ├── MediaRecorder.kt             # Advanced: "Media Recorder" — record media streams
        │   ├── MediaPlayer.kt               # Advanced: "Media Player" — play media files
        │   ├── ScreenSharing.kt             # Advanced: "Screen Sharing" — screen capture & share
        │   ├── VideoProcessExtension.kt     # Advanced: "Video Process Extension" — video filter
        │   ├── RhythmPlayer.kt              # Advanced: "Rhythm Player" — metronome playback
        │   ├── LocalVideoTranscoding.kt     # Advanced: "Local Video Transcoding" — local compositing
        │   ├── SendDataStream.kt            # Advanced: "Send Data Stream" — data channel messaging
        │   ├── HostAcrossChannel.kt         # Advanced: "Host Across Channel" — cross-channel relay
        │   ├── SpatialSound.kt              # Advanced: "Spatial Sound" — 3D spatial audio
        │
        ├── ui/
        │   ├── home/
        │   │   └── Home.kt                  # Home screen — renders grouped example list
        │   ├── example/
        │   │   ├── Example.kt               # Wrapper screen: calls example.content(back)
        │   │   └── ExampleItem.kt           # Single row in the example list
        │   ├── settings/
        │   │   └── Settings.kt              # Settings screen (area, resolution, frame rate)
        │   ├── common/
        │   │   ├── APIExampleScaffold.kt    # Shared scaffold with top bar
        │   │   ├── APIExampleTopAppBar.kt
        │   │   └── Widgets.kt               # ChannelNameInput, VideoGrid, VideoStatsInfo, etc.
        │   └── theme/
        │       └── Theme.kt
        │
        ├── data/
        │   └── SettingPreferences.kt        # DataStore-backed settings (area, resolution, frame rate)
        │
        └── utils/
            ├── TokenUtils.java              # Fetches RTC tokens from Agora token server
            ├── AudioFileReader.java
            ├── AudioPlayer.java
            ├── VideoFileReader.java
            ├── FileUtils.java
            ├── YUVUtils.java
            ├── YuvFboProgram.java
            ├── YuvUploader.java
            └── GLTextureView.java
```

## Case Index

| Case | Path | Key APIs | Description |
|------|------|----------|-------------|
| Join Video Channel (With Token) | `JoinChannelVideoToken.kt` | `joinChannel()`, `enableVideo()`, `setupLocalVideo()`, `setupRemoteVideo()` | Joins a video channel using a manually provided token instead of fetching one automatically |
| Join Video Channel | `JoinChannelVideo.kt` | `joinChannel()`, `enableVideo()`, `setupLocalVideo()`, `setupRemoteVideo()`, `setVideoEncoderConfiguration()` | Canonical reference for joining a video channel with token generation and basic video rendering |
| Join Audio Channel | `JoinChannelAudio.kt` | `joinChannel()`, `enableAudio()`, `setChannelProfile()`, `setAudioScenario()`, `setAudioProfile()`, `enableInEarMonitoring()` | Joins an audio-only channel with audio route, in-ear monitoring, and volume controls |
| Live Streaming | `LiveStreaming.kt` | `joinChannel()`, `enableVideo()`, `setClientRole()`, `setDualStreamMode()`, `setVideoScenario()`, `addVideoWatermark()`, `setVideoEncoderConfiguration()` | Demonstrates live streaming with client role switching, dual stream, watermark, and encoder options |
| RTMP Streaming | `RTMPStreaming.kt` | `joinChannel()`, `enableVideo()`, `startRtmpStreamWithTranscoding()`, `startRtmpStreamWithoutTranscoding()`, `stopRtmpStream()`, `updateRtmpTranscoding()` | Pushes a live stream to a CDN via RTMP with optional transcoding |
| Media Metadata | `MediaMetadata.kt` | `joinChannel()`, `enableVideo()`, `registerMediaMetadataObserver()` | Sends and receives video metadata through the IMetadataObserver interface |
| Voice Effects | `VoiceEffects.kt` | `joinChannel()`, `enableAudio()`, `setVoiceBeautifierPreset()`, `setVoiceConversionPreset()`, `setAudioEffectPreset()`, `setAudioEffectParameters()`, `setAINSMode()` | Applies voice beautifier, voice changer, style transformation, and noise suppression presets |
| Origin Audio Data | `OriginAudioData.kt` | `joinChannel()`, `enableAudio()`, `registerAudioFrameObserver()`, `setRecordingAudioFrameParameters()`, `setPlaybackAudioFrameParameters()` | Accesses and rewrites raw audio frames via the IAudioFrameObserver interface |
| Custom Audio Source | `CustomAudioSource.kt` | `joinChannel()`, `enableAudio()`, `createCustomAudioTrack()`, `pushExternalAudioFrame()`, `destroyCustomAudioTrack()`, `enableCustomAudioLocalPlayback()` | Pushes external audio from a file into a custom audio track |
| Custom Audio Render | `CustomAudioRender.kt` | `joinChannel()`, `enableAudio()`, `setExternalAudioSink()`, `pullPlaybackAudioFrame()` | Pulls remote audio frames and renders them through a custom AudioTrack player |
| Origin Video Data | `OriginVideoData.kt` | `joinChannel()`, `enableVideo()`, `registerVideoFrameObserver()` | Captures raw video frames via IVideoFrameObserver for screenshot functionality |
| Custom Video Source | `CustomVideoSource.kt` | `joinChannel()`, `enableVideo()`, `pushExternalVideoFrameById()` | Pushes external video frames in I420, NV21, NV12, or Texture2D format |
| Custom Video Render | `CustomVideoRender.kt` | `joinChannel()`, `enableVideo()`, `registerVideoFrameObserver()` | Renders remote video frames using a custom OpenGL renderer via IVideoFrameObserver |
| Picture In Picture | `PictureInPicture.kt` | `joinChannel()`, `enableVideo()`, `setupLocalVideo()`, `setupRemoteVideo()`, `enterPictureInPictureMode()` | Demonstrates Android Picture-in-Picture mode during a video call |
| Join Multi Channel | `JoinMultiChannel.kt` | `joinChannel()`, `joinChannelEx()`, `leaveChannelEx()`, `enableVideo()`, `setupRemoteVideoEx()`, `takeSnapshotEx()` | Joins two channels simultaneously using RtcEngineEx multi-channel APIs |
| Channel Encryption | `ChannelEncryption.kt` | `joinChannel()`, `enableVideo()`, `enableEncryption()` | Enables built-in media encryption before joining a channel |
| Play Audio Files | `PlayAudioFiles.kt` | `joinChannel()`, `enableAudio()`, `startAudioMixing()`, `stopAudioMixing()`, `playEffect()`, `preloadEffect()`, `setAudioProfile()` | Plays audio mixing and sound effect files with volume controls |
| Pre Call Test | `PreCallTest.kt` | `startLastmileProbeTest()`, `stopLastmileProbeTest()`, `startEchoTest()`, `stopEchoTest()`, `enableVideo()` | Runs network quality probe and audio/video echo tests before joining a channel |
| Media Recorder | `MediaRecorder.kt` | `joinChannel()`, `enableVideo()`, `createMediaRecorder()`, `startRecording()`, `stopRecording()` | Records local or remote media streams to MP4 files using AgoraMediaRecorder |
| Media Player | `MediaPlayer.kt` | `joinChannel()`, `enableVideo()`, `createMediaPlayer()`, `open()`, `play()`, `stop()`, `updateChannelMediaOptions()` | Plays media files and publishes the player track to the channel |
| Screen Sharing | `ScreenSharing.kt` | `joinChannel()`, `enableVideo()`, `startScreenCapture()`, `stopScreenCapture()`, `updateScreenCaptureParameters()`, `setScreenCaptureScenario()` | Captures and shares the device screen with scenario and audio options |
| Video Process Extension | `VideoProcessExtension.kt` | `joinChannel()`, `enableVideo()`, `setBeautyEffectOptions()`, `setLowlightEnhanceOptions()`, `setColorEnhanceOptions()`, `setVideoDenoiserOptions()`, `enableVirtualBackground()`, `enableExtension()` | Applies beauty filters, low-light enhancement, color enhancement, denoiser, and virtual background |
| Rhythm Player | `RhythmPlayer.kt` | `joinChannel()`, `startRhythmPlayer()`, `stopRhythmPlayer()`, `updateChannelMediaOptions()` | Plays a metronome beat track and publishes it to the channel |
| Local Video Transcoding | `LocalVideoTranscoding.kt` | `joinChannel()`, `enableVideo()`, `startLocalVideoTranscoder()`, `stopLocalVideoTranscoder()`, `startCameraCapture()`, `stopCameraCapture()` | Composites camera and media player streams into a single transcoded video |
| Send Data Stream | `SendDataStream.kt` | `joinChannel()`, `enableVideo()`, `createDataStream()`, `sendStreamMessage()` | Sends and receives real-time data messages through a data channel |
| Host Across Channel | `HostAcrossChannel.kt` | `joinChannel()`, `enableVideo()`, `startOrUpdateChannelMediaRelay()`, `stopChannelMediaRelay()`, `pauseAllChannelMediaRelay()`, `resumeAllChannelMediaRelay()` | Relays media streams from one channel to another for cross-channel hosting |
| Spatial Sound | `SpatialSound.kt` | `joinChannel()`, `enableAudio()`, `ILocalSpatialAudioEngine.initialize()`, `updateSelfPosition()`, `updateRemotePosition()`, `updatePlayerPositionInfo()` | Demonstrates 3D spatial audio with draggable sound source positioning |

## Case Registration Mechanism

Registration is **manual** — no reflection, no annotation scanning.

**To add a case, update at least four project-local artifacts:**

**1. `model/Examples.kt`** — append to `BasicExampleList` or `AdvanceExampleList`:
```kotlin
val AdvanceExampleList = listOf(
    // … existing entries …
    Example(R.string.example_my_new_case) { MyNewCase() }
)
```

**2. `samples/MyNewCase.kt`** — create the Composable:
```kotlin
@Composable
fun MyNewCase() { … }
```

**3. `res/values/strings.xml`** — add the user-facing example title.

**4. `ARCHITECTURE.md`** — update the case index so discovery tooling stays current.

Update `res/values-zh/strings.xml` too when the case title should remain localized alongside the existing examples.

No `nav_graph.xml`, no `@Example` annotation, and no action ID. `NavGraph.kt` routes to cases by their
position in the list — the order in `Examples.kt` is the display order inside the target list.

## Composable Case Pattern

Every case follows a two-function structure. `JoinChannelVideo.kt` is the canonical reference.

```
MyNewCase()                     ← public, stateful: owns RtcEngine, state, permissions
    └── MyNewCaseView(...)      ← private, stateless: receives data + lambdas, pure UI
```

**Engine creation and cleanup:**
```kotlin
val rtcEngine = remember {
    RtcEngine.create(RtcEngineConfig().apply {
        mContext = context
        mAppId = AgoraConfig.getAppId()
        mEventHandler = object : IRtcEngineEventHandler() { … }
    })
}
DisposableEffect(lifecycleOwner) {  // key must be lifecycleOwner, not Unit
    onDispose {
        if (isJoined) rtcEngine.leaveChannel()
        RtcEngine.destroy()
    }
}
```

**Permissions:**
```kotlin
val permissionLauncher = rememberLauncherForActivityResult(
    ActivityResultContracts.RequestMultiplePermissions()
) { grantedMap ->
    if (grantedMap.values.all { it }) { /* join channel */ }
}
// trigger:
permissionLauncher.launch(arrayOf(Manifest.permission.RECORD_AUDIO, Manifest.permission.CAMERA))
```

**State rules:**
- `rememberSaveable` — values that must survive rotation (channelName, isJoined, uid)
- `remember` — objects that must not be recreated (RtcEngine, collections)
- `IRtcEngineEventHandler` callbacks can mutate Compose state directly — the snapshot system is thread-safe

## Token Flow

```kotlin
TokenUtils.gen(channelName, uid) { token ->
    rtcEngine.joinChannel(token, channelName, uid, options)
}
```
