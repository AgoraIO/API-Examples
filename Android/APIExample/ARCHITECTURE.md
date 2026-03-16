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
        │   ├── basic/                       # group = "BASIC" (index 0–9)
        │   │   ├── JoinChannelVideoByToken.java # [0] "Live Interactive Video Streaming(Token Verify)"
        │   │   ├── JoinChannelVideo.java        # [1] "Live Interactive Video Streaming"
        │   │   └── JoinChannelAudio.java        # [2] "Live Interactive Audio Streaming"
        │   ├── advanced/                        # group = "ADVANCED" (index 10+)
        │   │   ├── LiveStreaming.java           # [10] "RTC Live Streaming" — setClientRole, broadcaster/audience
        │   │   ├── RTMPStreaming.java           # [11] "Push Streams to CDN" — RTMP push streaming
        │   │   ├── MediaMetadata.java           # [12] "Media Metadata" — send/receive metadata in video stream
        │   │   ├── VoiceEffects.java            # [13] "Set the Voice Beautifier and Effects" — setVoiceBeautifierPreset
        │   │   ├── customaudio/CustomAudioSource.java  # [14] "Custom Audio Sources" — push external audio
        │   │   ├── customaudio/CustomAudioRender.java  # [15] "Custom Audio Render" — pull audio for custom rendering
        │   │   ├── PushExternalVideoYUV.java    # [16] "Custom Video Source" — push YUV external video
        │   │   ├── CustomRemoteVideoRender.java # [17] "Custom Video Renderer" — custom remote video rendering
        │   │   ├── ProcessAudioRawData.java     # [18] "Raw Audio Data" — audio raw data processing
        │   │   ├── MultiVideoSourceTracks.java  # [19] "Multi Video Source Tracks" — multiple video sources
        │   │   ├── ProcessRawData.java          # [20] "Raw Video Data" — video raw data processing
        │   │   ├── SimpleExtension.java         # [21] "Simple Extension" — custom video extension
        │   │   ├── PictureInPicture.java        # [22] "Picture In Picture" — PiP mode
        │   │   ├── FaceCapture.java             # [23] "Face Capture" — face detection
        │   │   ├── VideoQuickSwitch.java        # [24] "Quick Switch Channel" — fast channel switching
        │   │   ├── JoinMultipleChannel.java     # [25] "Join Multiple Channel" — multi-channel join
        │   │   ├── ChannelEncryption.java       # [26] "Media Stream Encryption" — built-in encryption
        │   │   ├── PlayAudioFiles.java          # [27] "Play Audio Files" — audio mixing
        │   │   ├── PreCallTest.java             # [28] "Pre-call Tests" — network/device test before joining
        │   │   ├── MediaPlayer.java             # [29] "MediaPlayer" — play media files
        │   │   ├── MediaRecorder.java           # [30] "Local/Remote MediaRecorder" — record media streams
        │   │   ├── ScreenSharing.java           # [31] "Screen Sharing" — screen capture & share
        │   │   ├── VideoProcessExtension.java   # [32] "Video Process Extension" — video filter extension
        │   │   ├── LocalVideoTranscoding.java   # [33] "LocalVideoTranscoding" — local video compositing
        │   │   ├── RhythmPlayer.java            # [34] "Rhythm Player" — metronome/rhythm playback
        │   │   ├── SendDataStream.java          # [35] "Send Data Stream" — data channel messaging
        │   │   ├── HostAcrossChannel.java       # [36] "Relay Streams across Channels" — cross-channel relay
        │   │   ├── SpatialSound.java            # [37] "Spatial Audio" — 3D spatial audio
        │   │   ├── ContentInspect.java          # [38] "Content Inspect" — content moderation
        │   │   ├── ThirdPartyBeauty.java        # [39] "Third-party beauty" — third-party beauty SDK
        │   │   ├── KtvCopyrightMusic.java       # [40] "KTV Copyright Music" — licensed music
        │   │   ├── TransparentRendering.java    # [41] "TransparentRendering" — alpha channel rendering
        │   │   ├── UrlLiveStream.java           # [42] "Ultra Live Streaming with Url" — URL-based live stream
        │   │   ├── AgoraBeauty.java             # [43] "Agora beauty 2.0" — built-in beauty effects
        │   │   ├── Simulcast.java               # [44] "Simulcast" — multi-quality stream publishing
        │   │   ├── Multipath.java               # [45] "Multipath" — multi-path transmission
        │   │   ├── beauty/                      # Third-party beauty integrations
        │   │   └── videoRender/                 # Custom video rendering helpers
        │   └── audio/                           # Audio-specific cases (grouped as BASIC)
        │       ├── AudioWaveform.java           # [5] "Audio Waveform" — audio visualization
        │       ├── AudioRouterPlayer.java       # [6] "AudioRouter(Third Party Player)" — third-party audio routing
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

## Case Index

| Case | Path | Key APIs | Description |
|------|------|----------|-------------|
| Live Interactive Video Streaming(Token Verify) | `basic/JoinChannelVideoByToken.java` | `RtcEngine.create()`, `joinChannel()`, `setupLocalVideo()`, `enableVideo()`, `setVideoEncoderConfiguration()` | Demonstrates one-to-one video calling with manual App ID and token input |
| Live Interactive Video Streaming | `basic/JoinChannelVideo.java` | `RtcEngine.create()`, `joinChannel()`, `setupLocalVideo()`, `enableVideo()`, `setVideoEncoderConfiguration()` | Demonstrates basic one-to-one video calling with auto-generated token |
| Live Interactive Audio Streaming | `basic/JoinChannelAudio.java` | `RtcEngine.create()`, `joinChannel()`, `setAudioProfile()`, `setAudioScenario()`, `muteLocalAudioStream()`, `enableInEarMonitoring()`, `adjustRecordingSignalVolume()`, `adjustPlaybackSignalVolume()` | Demonstrates audio-only calling with volume controls, in-ear monitoring, and audio routing |
| RTC Live Streaming | `advanced/LiveStreaming.java` | `setClientRole()`, `enableDualStreamMode()`, `startPreview()`, `preloadChannel()`, `enableInstantMediaRendering()`, `startMediaRenderingTracing()`, `addVideoWatermark()`, `setRemoteDefaultVideoStreamType()`, `takeSnapshot()`, `enableVideoImageSource()` | Demonstrates broadcaster/audience role switching with dual-stream, watermark, and snapshot features |
| Streaming from RTC to CDN | `advanced/RTMPStreaming.java` | `startRtmpStreamWithTranscoding()`, `startRtmpStreamWithoutTranscoding()`, `stopRtmpStream()`, `updateRtmpTranscoding()` | Demonstrates pushing media streams from RTC to a CDN via RTMP |
| Media Metadata | `advanced/MediaMetadata.java` | `registerMediaMetadataObserver()`, `sendAudioMetadata()` | Demonstrates sending and receiving metadata alongside video streams |
| Set the Voice Beautifier and Effects | `advanced/VoiceEffects.java` | `setVoiceBeautifierPreset()`, `setAudioEffectPreset()`, `setVoiceConversionPreset()`, `setAudioEffectParameters()`, `setLocalVoicePitch()`, `setLocalVoiceEqualization()`, `setLocalVoiceReverb()`, `setLocalVoiceFormant()`, `setAINSMode()`, `enableVoiceAITuner()` | Demonstrates voice beautifier presets, audio effects, voice conversion, and AI noise suppression |
| Custom Audio Sources | `advanced/customaudio/CustomAudioSource.java` | `createCustomAudioTrack()`, `pushExternalAudioFrame()`, `enableCustomAudioLocalPlayback()`, `destroyCustomAudioTrack()` | Demonstrates pushing external audio frames via a custom audio track |
| Custom Audio Render | `advanced/customaudio/CustomAudioRender.java` | `setExternalAudioSink()`, `pullPlaybackAudioFrame()` | Demonstrates pulling audio frames for custom audio rendering |
| Custom Video Source | `advanced/PushExternalVideoYUV.java` | `setExternalVideoSource()`, `pushExternalVideoFrame()`, `setExternalRemoteEglContext()` | Demonstrates pushing external YUV video frames as a custom video source |
| Custom Video Renderer | `advanced/CustomRemoteVideoRender.java` | `registerVideoFrameObserver()`, `setExternalRemoteEglContext()` | Demonstrates custom rendering of remote video streams via video frame observer |
| Raw Audio Data | `advanced/ProcessAudioRawData.java` | `registerAudioFrameObserver()`, `setRecordingAudioFrameParameters()`, `setPlaybackAudioFrameParameters()` | Demonstrates processing raw audio data through the audio frame observer |
| Multi Video Source Tracks | `advanced/MultiVideoSourceTracks.java` | `createCustomVideoTrack()`, `pushExternalVideoFrameById()`, `joinChannelEx()`, `destroyCustomVideoTrack()`, `createCustomEncodedVideoTrack()`, `pushExternalEncodedVideoFrameById()` | Demonstrates publishing multiple custom video tracks simultaneously |
| Raw Video Data | `advanced/ProcessRawData.java` | `registerVideoFrameObserver()`, `startPreview()` | Demonstrates processing raw video data through the video frame observer |
| Simple Extension | `advanced/SimpleExtension.java` | `enableExtension()`, `setExtensionProperty()`, `enableAudioVolumeIndication()` | Demonstrates loading and configuring a custom audio/video extension |
| Picture In Picture | `advanced/PictureInPicture.java` | `joinChannel()`, `setupLocalVideo()`, `enableVideo()` | Demonstrates Android Picture-in-Picture mode during a video call |
| Face Capture | `advanced/FaceCapture.java` | `enableExtension()`, `setExtensionProperty()`, `registerVideoFrameObserver()`, `registerFaceInfoObserver()` | Demonstrates face capture and lip-sync driven video using extensions |
| Quick Switch Channel | `advanced/VideoQuickSwitch.java` | `joinChannel()`, `leaveChannel()`, `startPreview()`, `setClientRole()` | Demonstrates fast channel switching for audience members |
| Join Multiple Channel | `advanced/JoinMultipleChannel.java` | `joinChannel()`, `joinChannelEx()`, `leaveChannelEx()`, `startPreview()`, `takeSnapshotEx()` | Demonstrates joining two channels simultaneously using RtcEngineEx |
| Media Stream Encryption | `advanced/ChannelEncryption.java` | `enableEncryption()` | Demonstrates built-in media stream encryption |
| Play Audio Files | `advanced/PlayAudioFiles.java` | `startAudioMixing()`, `stopAudioMixing()`, `pauseAudioMixing()`, `resumeAudioMixing()`, `getAudioEffectManager()`, `adjustAudioMixingVolume()` | Demonstrates audio mixing and sound effect playback |
| Pre-call Tests | `advanced/PreCallTest.java` | `startLastmileProbeTest()`, `stopLastmileProbeTest()`, `startEchoTest()`, `stopEchoTest()` | Demonstrates network quality probing and echo testing before joining a channel |
| MediaPlayer | `advanced/MediaPlayer.java` | `createMediaPlayer()`, `mediaPlayer.open()`, `mediaPlayer.play()`, `mediaPlayer.stop()`, `mediaPlayer.pause()`, `mediaPlayer.seek()`, `updateChannelMediaOptions()` | Demonstrates playing media files with the built-in media player |
| Local/Remote MediaRecorder | `advanced/MediaRecorder.java` | `createMediaRecorder()`, `destroyMediaRecorder()`, `startRecordingDeviceTest()` | Demonstrates recording local and remote media streams |
| Scree Sharing | `advanced/ScreenSharing.java` | `startScreenCapture()`, `stopScreenCapture()`, `updateScreenCaptureParameters()`, `setScreenCaptureScenario()` | Demonstrates screen capture and sharing during a video call |
| Video Enhancement | `advanced/VideoProcessExtension.java` | `setBeautyEffectOptions()`, `setFilterEffectOptions()`, `setLowlightEnhanceOptions()`, `setVideoDenoiserOptions()`, `setColorEnhanceOptions()`, `enableVirtualBackground()`, `setFaceShapeBeautyOptions()`, `setFaceShapeAreaOptions()` | Demonstrates built-in video enhancement including beauty, filter, denoising, and virtual background |
| LocalVideoTranscoding | `advanced/LocalVideoTranscoding.java` | `startLocalVideoTranscoder()`, `startCameraCapture()`, `startScreenCapture()`, `stopScreenCapture()`, `enableVirtualBackground()` | Demonstrates compositing multiple local video sources into a single stream |
| Rhythm Player | `advanced/RhythmPlayer.java` | `startRhythmPlayer()`, `stopRhythmPlayer()`, `enableAudioVolumeIndication()` | Demonstrates metronome/rhythm playback synchronized with audio streaming |
| Send Data Stream | `advanced/SendDataStream.java` | `createDataStream()`, `sendStreamMessage()` | Demonstrates sending and receiving data channel messages |
| Relay Streams across Channels | `advanced/HostAcrossChannel.java` | `startOrUpdateChannelMediaRelay()`, `stopChannelMediaRelay()`, `pauseAllChannelMediaRelay()`, `resumeAllChannelMediaRelay()` | Demonstrates relaying media streams from one channel to another |
| Spatial Audio | `advanced/SpatialSound.java` | `enableAudio()`, `setRemoteUserSpatialAudioParams()`, `createMediaPlayer()` | Demonstrates 3D spatial audio positioning for remote users |
| Content Inspect | `advanced/ContentInspect.java` | `enableContentInspect()` | Demonstrates real-time content moderation on video streams |
| Third-party beauty | `advanced/ThirdPartyBeauty.java` | `registerVideoFrameObserver()` | Demonstrates integration with third-party beauty SDKs (e.g. FaceUnity) |
| KTV Copyright Music | `advanced/KtvCopyrightMusic.java` | N/A (browser-based documentation link) | Demonstrates the KTV copyright music feature via documentation reference |
| TransparentRendering | `advanced/TransparentRendering.java` | `setExternalVideoSource()`, `pushExternalVideoFrame()`, `createMediaPlayer()`, `startPreview()` | Demonstrates alpha-channel transparent video rendering |
| Ultra Live Streaming with Url | `advanced/UrlLiveStream.java` | `Rte()`, `Player()`, `Canvas()`, `player.openWithUrl()`, `player.stop()` | Demonstrates ultra-low-latency live streaming playback via URL using the RTE SDK |
| Agora beauty 2.0 | `advanced/AgoraBeauty.java` | `enableVirtualBackground()`, `setFaceShapeAreaOptions()` | Demonstrates built-in Agora beauty effects with face shaping and virtual background |
| Simulcast | `advanced/Simulcast.java` | `setSimulcastConfig()`, `setRemoteVideoStreamType()` | Demonstrates publishing multiple quality streams with simulcast |
| Multipath | `advanced/Multipath.java` | `joinChannel()`, `updateChannelMediaOptions()` | Demonstrates multi-path transmission for improved network reliability |
| Audio Waveform | `audio/AudioWaveform.java` | `enableAudio()`, `enableAudioVolumeIndication()` | Demonstrates real-time audio waveform visualization |
| AudioRouter(Third Party Player) | `audio/AudioRouterPlayer.java` | `setEnableSpeakerphone()`, `joinChannel()` | Demonstrates audio routing with third-party media players (ExoPlayer, IjkPlayer, Native) |

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
    index    = 2,                               // sort order within the group; BASIC: 0–9, ADVANCED: 10+
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
- A `<fragment>` destination entry under the root `<navigation>` in `nav_graph.xml`
- An `<action android:id="@+id/action_mainFragment_to_myCase">` inside `<fragment id="Ready">` — **not** `mainFragment`; `mainFragment` has only one action pointing to `Ready`, all case actions live in `Ready`
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

`TokenUtils` reads `AGORA_APP_ID` and `AGORA_APP_CERT` from `local.properties` via `BuildConfig`. If `AGORA_APP_CERT` is empty, token generation is skipped — valid for projects without certificate.
