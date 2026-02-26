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
        │   ├── basic/                       # group = "BASIC"
        │   │   ├── JoinChannelAudio.java
        │   │   └── JoinChannelAudioByToken.java
        │   ├── advanced/                    # group = "ADVANCED"
        │   │   ├── VoiceEffects.java
        │   │   ├── SpatialSound.java
        │   │   ├── PlayAudioFiles.java
        │   │   ├── ProcessAudioRawData.java
        │   │   ├── RhythmPlayer.java
        │   │   ├── PreCallTest.java
        │   │   └── customaudio/
        │   │       ├── CustomAudioSource.java
        │   │       ├── CustomAudioRender.java
        │   │       └── AudioPlayer.java
        │   └── audio/
        │       └── AudioWaveform.java
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
    index    = 2,                               // sort order within the group
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
- An `<action android:id="@+id/action_mainFragment_to_myCase">` inside `<fragment id="mainFragment">`
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
