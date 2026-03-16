# ARCHITECTURE.md — APIExample-Audio

## Directory Layout

```
APIExample-Audio/
├── gradle.properties                        # rtc_sdk_version
└── app/src/main/
    ├── AndroidManifest.xml
    ├── assets/                              # Audio sample files
    ├── res/
    │   ├── navigation/nav_graph.xml         # Single nav graph — all case destinations live here
    │   ├── values/strings.xml               # All display names and tips strings
    │   └── layout/                          # XML layouts for each case Fragment
    └── java/io/agora/api/example/
        ├── MainApplication.java             # Scans DEX and registers all @Example cases at startup
        ├── MainActivity.java                # Single-Activity host, owns NavController
        ├── MainFragment.java                # Home screen — renders BASIC / ADVANCED section list
        ├── ReadyFragment.java               # Splash / config check screen
        ├── SettingActivity.java             # Global settings (area code, audio profile)
        │
        ├── annotation/
        │   └── Example.java                 # @Example annotation — identical to APIExample
        │
        ├── common/
        │   ├── BaseFragment.java            # Base class ALL case Fragments must extend
        │   ├── Constant.java                # App-wide constants
        │   ├── adapter/
        │   │   └── SectionAdapter.java      # RecyclerView adapter for the grouped case list
        │   ├── model/
        │   │   ├── Examples.java            # Static registry: ITEM_MAP keyed by group name
        │   │   ├── GlobalSettings.java      # Audio config shared across cases
        │   │   ├── ExampleBean.java
        │   │   ├── Peer.java
        │   │   └── StatisticsInfo.java
        │   ├── widget/
        │   │   ├── AudioOnlyLayout.java     # Audio seat layout (no video surface)
        │   │   ├── AudioSeatManager.java
        │   │   └── WaveformView.java
        │   └── gles/                        # OpenGL ES helpers (for waveform visualization)
        │
        ├── examples/                        # All cases live here — ClassUtils scans this package
        │   ├── basic/                       # group = "BASIC" (index 0–9)
        │   │   ├── JoinChannelAudioByToken.java # [0] "Live Interactive Audio Streaming(Token Verify)"
        │   │   └── JoinChannelAudio.java        # [1] "Live Interactive Audio Streaming"
        │   ├── advanced/                        # group = "ADVANCED" (index 10+)
        │   │   ├── VoiceEffects.java            # [10] "Set the Voice Beautifier and Effects" — setVoiceBeautifierPreset
        │   │   ├── customaudio/CustomAudioSource.java  # [11] "Custom Audio Sources" — push external audio
        │   │   ├── customaudio/CustomAudioRender.java  # [12] "Custom Audio Render" — pull audio for custom rendering
        │   │   ├── customaudio/AudioPlayer.java        # helper for CustomAudioRender
        │   │   ├── ProcessAudioRawData.java     # [13] "Raw Audio Data" — audio raw data processing
        │   │   ├── PlayAudioFiles.java          # [14] "Play Audio Files" — audio mixing
        │   │   ├── PreCallTest.java             # [15] "Pre-call Tests" — network/device test before joining
        │   │   ├── RhythmPlayer.java            # [16] "Rhythm Player" — metronome/rhythm playback
        │   │   └── SpatialSound.java            # [17] "Spatial Audio" — 3D spatial audio
        │   └── audio/                           # Audio-specific cases (grouped as ADVANCED)
        │       └── AudioWaveform.java           # [18] "Audio Waveform" — audio visualization
        │
        └── utils/
            ├── ClassUtils.java              # DEX scanner — auto-discovers @Example classes
            ├── TokenUtils.java              # Fetches RTC tokens from Agora token server
            ├── PermissonUtils.java          # Permission check/request helpers
            ├── CommonUtil.java
            ├── ErrorUtil.java
            ├── FileUtils.java
            ├── AudioFileReader.java
            └── YUVUtils.java
```

## Case Index

| Case | Path | Key APIs | Description |
|------|------|----------|-------------|
| Live Interactive Audio Streaming(Token Verify) | `basic/JoinChannelAudioByToken.java` | `RtcEngine.create()`, `joinChannel()`, `setClientRole()` | Demonstrates audio-only calling with manual App ID and token input |
| Live Interactive Audio Streaming | `basic/JoinChannelAudio.java` | `RtcEngine.create()`, `joinChannel()`, `setAudioProfile()`, `setAudioScenario()`, `muteLocalAudioStream()`, `enableInEarMonitoring()`, `adjustRecordingSignalVolume()`, `adjustPlaybackSignalVolume()` | Demonstrates audio-only calling with volume controls, in-ear monitoring, and audio routing |
| Set the Voice Beautifier and Effects | `advanced/VoiceEffects.java` | `setVoiceBeautifierPreset()`, `setAudioEffectPreset()`, `setVoiceConversionPreset()`, `setAudioEffectParameters()`, `setLocalVoicePitch()`, `setLocalVoiceEqualization()`, `setLocalVoiceReverb()`, `setLocalVoiceFormant()`, `setAINSMode()`, `enableVoiceAITuner()` | Demonstrates voice beautifier presets, audio effects, voice conversion, and AI noise suppression |
| Custom Audio Sources | `advanced/customaudio/CustomAudioSource.java` | `createCustomAudioTrack()`, `pushExternalAudioFrame()`, `enableCustomAudioLocalPlayback()`, `destroyCustomAudioTrack()` | Demonstrates pushing external audio frames via a custom audio track |
| Custom Audio Render | `advanced/customaudio/CustomAudioRender.java` | `setExternalAudioSink()`, `pullPlaybackAudioFrame()` | Demonstrates pulling audio frames for custom audio rendering |
| Raw Audio Data | `advanced/ProcessAudioRawData.java` | `registerAudioFrameObserver()`, `setRecordingAudioFrameParameters()`, `setPlaybackAudioFrameParameters()` | Demonstrates processing raw audio data through the audio frame observer |
| Play Audio Files | `advanced/PlayAudioFiles.java` | `startAudioMixing()`, `stopAudioMixing()`, `pauseAudioMixing()`, `resumeAudioMixing()`, `getAudioEffectManager()`, `adjustAudioMixingVolume()` | Demonstrates audio mixing and sound effect playback |
| Pre-call Tests | `advanced/PreCallTest.java` | `startLastmileProbeTest()`, `stopLastmileProbeTest()`, `startEchoTest()`, `stopEchoTest()` | Demonstrates network quality probing and echo testing before joining a channel |
| Rhythm Player | `advanced/RhythmPlayer.java` | `startRhythmPlayer()`, `stopRhythmPlayer()`, `enableAudioVolumeIndication()` | Demonstrates metronome/rhythm playback synchronized with audio streaming |
| Spatial Audio | `advanced/SpatialSound.java` | `ILocalSpatialAudioEngine.create()`, `updateSelfPosition()`, `updateRemotePosition()`, `updatePlayerPositionInfo()`, `setZones()`, `createMediaPlayer()` | Demonstrates 3D spatial audio positioning for remote users and media players |
| Audio Waveform | `audio/AudioWaveform.java` | `enableAudio()`, `enableAudioVolumeIndication()` | Demonstrates real-time audio waveform visualization |

## Case Registration Mechanism

Identical to `APIExample` — automatic via reflection, no manual list.

**Startup flow:**
1. `MainApplication.onCreate()` calls `ClassUtils.getFileNameByPackageName(context, "io.agora.api.example.examples")`.
2. `ClassUtils` scans all DEX entries whose class name starts with that prefix.
3. For each class, it checks for `@Example` annotation and calls `Examples.addItem(annotation)`.
4. `Examples.sortItem()` sorts each group by `index`.
5. `MainFragment` reads `Examples.ITEM_MAP` and renders the list.

**`@Example` annotation — all four fields are required:**
```java
@Example(
    index    = 2,                               // sort order within the group; BASIC: 0–9, ADVANCED: 10+
    group    = BASIC,                           // "BASIC" or "ADVANCED"
    name     = R.string.item_my_case,           // display name string resource
    actionId = R.id.action_mainFragment_to_myCase, // nav action ID in nav_graph.xml
    tipsId   = R.string.my_case_tips            // description string resource
)
public class MyCase extends BaseFragment { … }
```

## Navigation

Identical to `APIExample` — single `nav_graph.xml` with Jetpack Navigation Component.

Every case needs:
- A `<fragment>` destination entry in `nav_graph.xml`
- An `<action android:id="@+id/action_mainFragment_to_myCase">` inside `<fragment id="Ready">`
- The action `id` must exactly match `actionId` in `@Example`

## RtcEngine Lifecycle

```
onActivityCreated → RtcEngine.create()  (voice-sdk — no video APIs)
                  → engine.setAudioProfile / setAudioScenario
                  → joinChannel() (after RECORD_AUDIO permission granted)
                         ↓
                    [IRtcEngineEventHandler callbacks — background thread]
                         ↓
onDestroy         → engine.leaveChannel()
                  → RtcEngine.destroy()
                  → engine = null
```

## Token Flow

```java
TokenUtils.gen(requireContext(), channelId, uid, token -> {
    engine.joinChannel(token, channelId, uid, options);
});
```

`TokenUtils` reads `AGORA_APP_ID` and `AGORA_APP_CERT` from `local.properties` via `BuildConfig`. If `AGORA_APP_CERT` is empty, token generation is skipped — valid for projects without certificate.
