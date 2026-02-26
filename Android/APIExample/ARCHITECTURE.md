# ARCHITECTURE.md — APIExample

## Directory Layout

```
APIExample/
├── gradle.properties                        # rtc_sdk_version, simpleFilter, streamEncrypt flags
├── agora-simple-filter/                     # Optional C++ video extension module
├── agora-stream-encrypt/                    # Optional custom stream encryption module
└── app/src/main/
    ├── AndroidManifest.xml
    ├── assets/                              # Audio/video sample files, beauty resources
    ├── res/
    │   ├── navigation/nav_graph.xml         # Single nav graph — all case destinations live here
    │   ├── values/strings.xml               # All display names and tips strings
    │   └── layout/                          # XML layouts for each case Fragment
    └── java/io/agora/api/example/
        ├── MainApplication.java             # Scans DEX and registers all @Example cases at startup
        ├── MainActivity.java                # Single-Activity host, owns NavController
        ├── MainFragment.java                # Home screen — renders BASIC / ADVANCED section list
        ├── ReadyFragment.java               # Splash / config check screen
        ├── SettingActivity.java             # Global settings (resolution, frame rate, area code)
        │
        ├── annotation/
        │   └── Example.java                 # @Example annotation — the case registration contract
        │
        ├── common/
        │   ├── BaseFragment.java            # Base class ALL case Fragments must extend
        │   ├── BaseVbFragment.java          # ViewBinding variant of BaseFragment
        │   ├── Constant.java                # App-wide constants
        │   ├── adapter/
        │   │   └── SectionAdapter.java      # RecyclerView adapter for the grouped case list
        │   ├── model/
        │   │   ├── Examples.java            # Static registry: ITEM_MAP keyed by group name
        │   │   ├── GlobalSettings.java      # Video/audio config shared across cases
        │   │   ├── ExampleBean.java
        │   │   └── StatisticsInfo.java
        │   ├── widget/
        │   │   ├── VideoReportLayout.java   # Video container with stats overlay
        │   │   ├── AudioOnlyLayout.java     # Audio-only seat layout
        │   │   ├── AudioSeatManager.java
        │   │   └── WaveformView.java
        │   ├── floatwindow/                 # Floating window helper for in-call overlay
        │   └── gles/                        # OpenGL ES helpers for custom video rendering
        │
        ├── examples/                        # All cases live here — ClassUtils scans this package
        │   ├── basic/                       # group = "BASIC"
        │   │   ├── JoinChannelVideo.java
        │   │   ├── JoinChannelAudio.java
        │   │   └── JoinChannelVideoByToken.java
        │   ├── advanced/                    # group = "ADVANCED"
        │   │   ├── LiveStreaming.java
        │   │   ├── ScreenSharing.java
        │   │   ├── MediaPlayer.java
        │   │   ├── MediaRecorder.java
        │   │   ├── VoiceEffects.java
        │   │   ├── SpatialSound.java
        │   │   ├── ChannelEncryption.java
        │   │   ├── JoinMultipleChannel.java
        │   │   ├── SendDataStream.java
        │   │   ├── ProcessRawData.java
        │   │   ├── ProcessAudioRawData.java
        │   │   ├── PushExternalVideo.java
        │   │   ├── PushExternalVideoYUV.java
        │   │   ├── CustomRemoteVideoRender.java
        │   │   ├── LocalVideoTranscoding.java
        │   │   ├── MultiVideoSourceTracks.java
        │   │   ├── VideoProcessExtension.java
        │   │   ├── SimpleExtension.java
        │   │   ├── ContentInspect.java
        │   │   ├── FaceCapture.java
        │   │   ├── AgoraBeauty.java
        │   │   ├── ThirdPartyBeauty.java
        │   │   ├── HostAcrossChannel.java
        │   │   ├── RTMPStreaming.java
        │   │   ├── MediaMetadata.java
        │   │   ├── PlayAudioFiles.java
        │   │   ├── RhythmPlayer.java
        │   │   ├── PreCallTest.java
        │   │   ├── InCallReport.java
        │   │   ├── KtvCopyrightMusic.java
        │   │   ├── PictureInPicture.java
        │   │   ├── Simulcast.java
        │   │   ├── SwitchCameraScreenShare.java
        │   │   ├── TransparentRendering.java
        │   │   ├── UrlLiveStream.java
        │   │   ├── VideoQuickSwitch.java
        │   │   ├── Multipath.java
        │   │   ├── beauty/                  # Third-party beauty integrations
        │   │   ├── CDNStreaming/
        │   │   ├── customaudio/
        │   │   └── videoRender/
        │   └── audio/                       # Audio-specific cases in the full demo
        │       ├── AudioWaveform.java
        │       ├── AudioRouterPlayer.java
        │       └── AudioRouterPlayer*.java  # Exo / Ijk / Native variants
        │
        ├── service/
        │   └── MediaProjectionService.java  # Foreground service required for screen sharing
        │
        └── utils/
            ├── ClassUtils.java              # DEX scanner — auto-discovers @Example classes
            ├── TokenUtils.java              # Fetches RTC tokens from Agora token server
            ├── PermissonUtils.java          # Permission check/request helpers
            ├── CommonUtil.java
            ├── ErrorUtil.java
            ├── FileUtils.java
            ├── FileKtUtils.kt
            ├── AudioFileReader.java
            ├── VideoFileReader.java
            └── YUVUtils.java
```

## Case Registration Mechanism

Registration is **automatic via reflection**. No manual list to maintain.

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

A missing or malformed annotation causes the case to silently not appear — no crash.

## Navigation

Single `nav_graph.xml` with Jetpack Navigation Component.

Every case needs:
- A `<fragment>` destination entry in `nav_graph.xml`
- An `<action android:id="@+id/action_mainFragment_to_myCase">` inside `<fragment id="mainFragment">`
- The action `id` must exactly match `actionId` in `@Example`

`MainActivity` calls `Navigation.findNavController(...).navigate(example.actionId())` on list item tap.

## RtcEngine Lifecycle

```
onActivityCreated → RtcEngine.create()
                  → engine.setParameters / setVideoEncoderConfiguration
                  → joinChannel() (after permission granted)
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

If `agora_app_certificate` is empty, `TokenUtils` passes `null` — valid for projects without certificate.
