# ARCHITECTURE.md — APIExample-Compose

## Directory Layout

```
APIExample-Compose/
├── gradle.properties                        # rtc_sdk_version
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
        │   └── VideoSnapshot.kt             # Advanced: "Video Snapshot" — takeSnapshot API
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

## Case Registration Mechanism

Registration is **manual** — no reflection, no annotation scanning.

**To add a case, edit exactly two files:**

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

No `nav_graph.xml`, no `@Example` annotation, no action ID. `NavGraph.kt` routes to cases by their
index in the list — the order in `Examples.kt` is the display order.

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
        mAppId = BuildConfig.AGORA_APP_ID
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
