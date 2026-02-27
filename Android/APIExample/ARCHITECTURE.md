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
        │   │   ├── JoinChannelVideoByToken.java # [0] "Live Interactive Video Streaming(Token Verify)"
        │   │   ├── JoinChannelVideo.java        # [1] "Live Interactive Video Streaming"
        │   │   └── JoinChannelAudio.java        # [2] "Live Interactive Audio Streaming"
        │   ├── advanced/                        # group = "ADVANCED"
        │   │   ├── LiveStreaming.java           # [0]  "RTC Live Streaming" — setClientRole, broadcaster/audience
        │   │   ├── RTMPStreaming.java           # [1]  "Push Streams to CDN" — RTMP push streaming
        │   │   ├── MediaMetadata.java           # [3]  "Media Metadata" — send/receive metadata in video stream
        │   │   ├── VoiceEffects.java            # [4]  "Set the Voice Beautifier and Effects" — setVoiceBeautifierPreset
        │   │   ├── customaudio/CustomAudioSource.java  # [5] "Custom Audio Sources" — push external audio
        │   │   ├── customaudio/CustomAudioRender.java  # [6] "Custom Audio Render" — pull audio for custom rendering
        │   │   ├── PushExternalVideoYUV.java    # [7]  "Custom Video Source" — push YUV external video
        │   │   ├── CustomRemoteVideoRender.java # [8]  "Custom Video Renderer" — custom remote video rendering
        │   │   ├── ProcessAudioRawData.java     # [9]  "Raw Audio Data" — audio raw data processing
        │   │   ├── MultiVideoSourceTracks.java  # [10] "Multi Video Source Tracks" — multiple video sources
        │   │   ├── ProcessRawData.java          # [11] "Raw Video Data" — video raw data processing
        │   │   ├── SimpleExtension.java         # [11] "Simple Extension" — custom video extension
        │   │   ├── PictureInPicture.java        # [11] "Picture In Picture" — PiP mode
        │   │   ├── FaceCapture.java             # [12] "Face Capture" — face detection
        │   │   ├── VideoQuickSwitch.java        # [12] "Quick Switch Channel" — fast channel switching
        │   │   ├── JoinMultipleChannel.java     # [13] "Join Multiple Channel" — multi-channel join
        │   │   ├── ChannelEncryption.java       # [14] "Media Stream Encryption" — built-in encryption
        │   │   ├── PlayAudioFiles.java          # [15] "Play Audio Files" — audio mixing
        │   │   ├── MediaPlayer.java             # [17] "MediaPlayer" — play media files
        │   │   ├── ScreenSharing.java           # [18] "Screen Sharing" — screen capture & share
        │   │   ├── VideoProcessExtension.java   # [19] "Video Process Extension" — video filter extension
        │   │   ├── LocalVideoTranscoding.java   # [19] "LocalVideoTranscoding" — local video compositing
        │   │   ├── RhythmPlayer.java            # [19] "Rhythm Player" — metronome/rhythm playback
        │   │   ├── SendDataStream.java          # [20] "Send Data Stream" — data channel messaging
        │   │   ├── HostAcrossChannel.java       # [21] "Relay Streams across Channels" — cross-channel relay
        │   │   ├── SpatialSound.java            # [22] "Spatial Audio" — 3D spatial audio
        │   │   ├── ContentInspect.java          # [23] "Content Inspect" — content moderation
        │   │   ├── ThirdPartyBeauty.java        # [24] "Third-party beauty" — third-party beauty SDK
        │   │   ├── KtvCopyrightMusic.java       # [24] "KTV Copyright Music" — licensed music
        │   │   ├── TransparentRendering.java    # [25] "TransparentRendering" — alpha channel rendering
        │   │   ├── UrlLiveStream.java           # [26] "Ultra Live Streaming with Url" — URL-based live stream
        │   │   ├── AgoraBeauty.java             # [27] "Agora beauty 2.0" — built-in beauty effects
        │   │   ├── Simulcast.java               # [28] "Simulcast" — multi-quality stream publishing
        │   │   ├── Multipath.java               # [29] "Multipath" — multi-path transmission
        │   │   ├── VideoSnapshot.java           # [30] "Video Snapshot" — takeSnapshot API
        │   │   ├── MediaRecorder.java           # "Local/Remote MediaRecorder" — record media streams
        │   │   ├── beauty/                      # Third-party beauty integrations
        │   │   ├── CDNStreaming/                 # (disabled) CDN streaming entry
        │   │   └── videoRender/                 # Custom video rendering helpers
        │   └── audio/                           # Audio-specific cases (grouped as ADVANCED)
        │       ├── AudioWaveform.java           # [17] "Audio Waveform" — audio visualization
        │       ├── AudioRouterPlayer.java       # [17] "AudioRouter(Third Party Player)" — third-party audio routing
        │       └── AudioRouterPlayer*.java      # Exo / Ijk / Native variants
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
