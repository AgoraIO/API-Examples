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
        │   ├── JoinChannelVideo.kt          # Canonical reference — read this before writing a new case
        │   ├── JoinChannelAudio.kt
        │   ├── JoinChannelVideoToken.kt
        │   ├── LiveStreaming.kt
        │   ├── RTMPStreaming.kt
        │   ├── ScreenSharing.kt
        │   ├── MediaPlayer.kt
        │   ├── MediaRecorder.kt
        │   ├── MediaMetadata.kt
        │   ├── VoiceEffects.kt
        │   ├── SpatialSound.kt
        │   ├── ChannelEncryption.kt
        │   ├── JoinMultiChannel.kt
        │   ├── HostAcrossChannel.kt
        │   ├── SendDataStream.kt
        │   ├── OriginAudioData.kt
        │   ├── OriginVideoData.kt
        │   ├── CustomAudioSource.kt
        │   ├── CustomAudioRender.kt
        │   ├── CustomVideoSource.kt
        │   ├── CustomVideoRender.kt
        │   ├── PlayAudioFiles.kt
        │   ├── RhythmPlayer.kt
        │   ├── PreCallTest.kt
        │   ├── PictureInPicture.kt
        │   ├── LocalVideoTranscoding.kt
        │   ├── VideoProcessExtension.kt
        │   └── SpatialSound.kt
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
DisposableEffect(lifecycleOwner) {
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
