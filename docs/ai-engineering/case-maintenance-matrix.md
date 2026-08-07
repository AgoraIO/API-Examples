# API Examples Case Maintenance Matrix

This matrix is the planning artifact for cross-platform API Example parity work. It does not replace platform `AGENTS.md`, project `ARCHITECTURE.md`, or project `.agent/skills/*` / `.agents/skills/*`; every row must be verified against the target project before implementation.

## How To Use

1. Start from the product request and identify the feature name, SDK family, and key SDK APIs.
2. Find the matching row in this matrix.
3. For each target platform unit, verify the path with the project `query-cases` skill when present.
4. Treat each required platform project as an independent delivery/verification unit inside one requirement-level acceptance manifest.
5. Update this matrix when a case is added, moved, renamed, or confirmed not applicable.

## Status Values

| Status | Meaning |
| --- | --- |
| `DONE(path)` | Inventory says the case is implemented and expected to be registered at the listed path. Verify before editing. For new or changed parity work, use `DONE` only after the required reference, review, and build gates pass. |
| `HIDDEN(path)` | Source is retained at the listed path, but the case is intentionally not registered in the user-visible list. |
| `PARTIAL(note)` | Exists but parity, registration, docs, verification, runtime smoke, or API coverage needs review. |
| `MISSING` | Required for parity but no implementation is currently recorded here. |
| `N/A(reason)` | Not applicable because the SDK family, platform, or project variant does not support it. |
| `UNKNOWN` | Not inventoried yet; run the project case query before making decisions. |

## Active Parity Acceptance Rule

For new or changed parity work, do not move a cell to `DONE` from compilation alone.

The platform execution unit must record:

- Reference Contract: source implementation, UI resources, registration, SDK overloads, enum/option values, defaults, limits, and expected success signals.
- Parity Checklist: final implementation matches the reference contract; similar cases are used only for framework patterns.
- Build Result: the target project compile/build command passes, or the exact blocker is recorded.

If runtime/device behavior is required but unavailable, use `PARTIAL(...)` and note the pending smoke check instead of `DONE(...)`.

## Platform Units

| Unit | Project | Primary Source |
| --- | --- | --- |
| Android full | `Android/APIExample/` | `Android/APIExample/ARCHITECTURE.md` |
| Android audio | `Android/APIExample-Audio/` | `Android/APIExample-Audio/ARCHITECTURE.md` |
| Android Compose | `Android/APIExample-Compose/` | `Android/APIExample-Compose/ARCHITECTURE.md` |
| iOS UIKit | `iOS/APIExample/` | `iOS/APIExample/ARCHITECTURE.md` |
| iOS SwiftUI | `iOS/APIExample-SwiftUI/` | `iOS/APIExample-SwiftUI/ARCHITECTURE.md` |
| iOS Objective-C | `iOS/APIExample-OC/` | `iOS/APIExample-OC/ARCHITECTURE.md` |
| iOS audio | `iOS/APIExample-Audio/` | `iOS/APIExample-Audio/ARCHITECTURE.md` |
| macOS | `macOS/` | `macOS/ARCHITECTURE.md` |
| Windows | `windows/` | `windows/ARCHITECTURE.md` |

## Pilot Matrix

Seeded from the overseas `dev/4.6.4` project `ARCHITECTURE.md` indexes and source tree. The inventory audit resolved 191 `DONE` candidates against their target project roots; paths are relative to the projects listed in Platform Units.

| Feature | SDK Family | Key APIs | Android full | Android audio | Android Compose | iOS UIKit | iOS SwiftUI | iOS Objective-C | iOS audio | macOS | Windows | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Join channel video | Full RTC | `joinChannel`, `setupLocalVideo`, `setupRemoteVideo` | `DONE(basic/JoinChannelVideo.java)` | `N/A(audio SDK)` | `DONE(samples/JoinChannelVideo.kt)` | `DONE(Basic/JoinChannelVideo/)` | `DONE(Basic/JoinChannelVideo/)` | `DONE(Basic/JoinChannelVideo/)` | `N/A(audio SDK)` | `DONE(Basic/JoinChannelVideo/)` | `PARTIAL(Basic/JoinChannelVideoByToken)` | Windows records token variant only; non-token parity gap. |
| Join channel video with token | Full RTC | `joinChannel` with token, local and remote video setup | `DONE(basic/JoinChannelVideoByToken.java)` | `N/A(audio SDK)` | `DONE(samples/JoinChannelVideoToken.kt)` | `DONE(Basic/JoinChannelVideo(Token)/)` | `DONE(Basic/JoinChannelVideo(Token)/)` | `DONE(Basic/JoinChannelVideo(Token)/)` | `N/A(audio SDK)` | `DONE(Basic/JoinChannelVideo(Token)/)` | `DONE(Basic/JoinChannelVideoByToken/)` | Verify token-generation behavior per platform before changing auth flow. |
| Join channel audio | Full RTC or audio SDK | `joinChannel`, `setAudioProfile`, audio routing or volume APIs | `DONE(basic/JoinChannelAudio.java)` | `DONE(basic/JoinChannelAudio.java)` | `DONE(samples/JoinChannelAudio.kt)` | `DONE(Basic/JoinChannelAudio/)` | `DONE(Basic/JoinChannelAudio/)` | `DONE(Basic/JoinChannelAudio/)` | `DONE(Basic/JoinChannelAudio/)` | `DONE(Basic/JoinChannelAudio/)` | `MISSING` | Windows has no basic audio-only join case. |
| Live streaming role switch | Full RTC | `setClientRole`, `joinChannel`, streaming role configuration | `DONE(advanced/LiveStreaming.java)` | `N/A(video/live)` | `DONE(samples/LiveStreaming.kt)` | `DONE(Advanced/LiveStreaming/)` | `DONE(Advanced/LiveStreaming/)` | `DONE(Advanced/LiveStreaming/)` | `N/A(video/live)` | `DONE(Advanced/LiveStreaming/)` | `DONE(Basic/LiveBroadcasting/)` | Names differ by platform; keep product scenario wording stable. |
| RTMP or CDN streaming | Full RTC | `startRtmpStreamWithTranscoding`, `startRtmpStreamWithoutTranscoding`, `stopRtmpStream` | `DONE(advanced/RTMPStreaming.java)` | `N/A(video/live)` | `DONE(samples/RTMPStreaming.kt)` | `DONE(Advanced/RTMPStreaming/)` | `DONE(Advanced/RTMPStream/)` | `DONE(Advanced/RTMPStreaming/)` | `N/A(video/live)` | `DONE(Advanced/RTMPStreaming/)` | `DONE(Advanced/RTMPStream/)` | Windows also has `RTMPinject`; verify requested API before routing. |
| Media metadata | Full RTC | `registerMediaMetadataObserver` or platform metadata delegate APIs | `DONE(advanced/MediaMetadata.java)` | `N/A(audio SDK)` | `DONE(samples/MediaMetadata.kt)` | `DONE(Advanced/VideoMetadata/)` | `DONE(Advanced/VideoMetadata/)` | `DONE(Advanced/VideoMetadata/)` | `N/A(audio SDK)` | `MISSING` | `DONE(Advanced/Metadata/)` | macOS has no metadata case. iOS names this VideoMetadata. |
| Data stream messaging | Full RTC | `createDataStream`, `sendStreamMessage` | `DONE(advanced/SendDataStream.java)` | `MISSING` | `DONE(samples/SendDataStream.kt)` | `DONE(Advanced/CreateDataStream/)` | `DONE(Advanced/CreateDataStream/)` | `DONE(Advanced/CreateDataStream/)` | `MISSING` | `DONE(Advanced/CreateDataStream/)` | `MISSING` | Windows, Android audio, and iOS audio have no data stream case. |
| Custom audio source | Full RTC or audio SDK | `createCustomAudioTrack`, `setExternalAudioSource`, `pushExternalAudioFrame` | `DONE(advanced/customaudio/CustomAudioSource.java)` | `DONE(advanced/customaudio/CustomAudioSource.java)` | `DONE(samples/CustomAudioSource.kt)` | `DONE(Advanced/CustomPcmAudioSource/)` | `DONE(Advanced/CustomPCMAudioSource/)` | `DONE(Advanced/CustomPcmAudioSource/)` | `DONE(Advanced/CustomAudioSource/)` | `DONE(Advanced/CustomAudioSource/)` | `DONE(Advanced/CustomAudioCapture/)` | Platform APIs differ; compare demonstrated behavior, not only method names. |
| Custom audio render | Full RTC or audio SDK | `setExternalAudioSink`, pull playback audio frame APIs | `DONE(advanced/customaudio/CustomAudioRender.java)` | `DONE(advanced/customaudio/CustomAudioRender.java)` | `DONE(samples/CustomAudioRender.kt)` | `DONE(Advanced/CustomAudioRender/)` | `DONE(Advanced/CustomAudioRender/)` | `DONE(Advanced/CustomAudioRender/)` | `DONE(Advanced/CustomAudioRender/)` | `DONE(Advanced/CustomAudioRender/)` | `MISSING` | Windows has no custom audio render case. |
| Custom video source | Full RTC | `setExternalVideoSource`, `pushExternalVideoFrame`, custom video track APIs | `DONE(advanced/PushExternalVideoYUV.java)` | `N/A(audio SDK)` | `DONE(samples/CustomVideoSource.kt)` | `DONE(Advanced/CustomVideoSourcePush/)` | `MISSING` | `DONE(Advanced/CustomVideoSourcePush/)` | `N/A(audio SDK)` | `DONE(Advanced/CustomVideoSourcePush/)` | `DONE(Advanced/CustomVideoCapture/)` | SwiftUI has CustomVideoRender but no source-push case. Multi-source variants are separate rows. |
| Raw audio data | Full RTC or audio SDK | `registerAudioFrameObserver`, audio frame delegate APIs | `DONE(advanced/ProcessAudioRawData.java)` | `DONE(advanced/ProcessAudioRawData.java)` | `DONE(samples/OriginAudioData.kt)` | `DONE(Advanced/RawAudioData/)` | `DONE(Advanced/RawAudioData/)` | `DONE(Advanced/RawAudioData/)` | `DONE(Advanced/RawAudioData/)` | `DONE(Advanced/RawAudioData/)` | `DONE(Advanced/OriginalAudio/)` | Check callback threading in every platform review. |
| Raw video data | Full RTC | `registerVideoFrameObserver`, video frame delegate APIs | `DONE(advanced/ProcessRawData.java)` | `N/A(audio SDK)` | `DONE(samples/OriginVideoData.kt)` | `DONE(Advanced/RawVideoData/)` | `DONE(Advanced/RawVideoData/)` | `DONE(Advanced/RawVideoData/)` | `N/A(audio SDK)` | `DONE(Advanced/RawVideoData/)` | `DONE(Advanced/OriginalVideo/)` | Raw media combined samples are tracked separately. |
| Screen sharing | Full RTC | `startScreenCapture`, `updateScreenCaptureParameters`, `stopScreenCapture` | `DONE(advanced/ScreenSharing.java)` | `N/A(audio SDK)` | `DONE(samples/ScreenSharing.kt)` | `DONE(Advanced/ScreenShare/)` | `DONE(Advanced/ScreenShare/)` | `DONE(Advanced/ScreenShare/)` | `N/A(audio SDK)` | `DONE(Advanced/ScreenShare/)` | `DONE(Advanced/ScreenShare/)` | iOS requires ReplayKit extension; Android requires foreground service. |
| Media player | Full RTC | `createMediaPlayer`, `open`, `play`, publish/update channel options | `DONE(advanced/MediaPlayer.java)` | `MISSING` | `DONE(samples/MediaPlayer.kt)` | `DONE(Advanced/MediaPlayer/)` | `DONE(Advanced/MediaPlayer/)` | `DONE(Advanced/MediaPlayer/)` | `MISSING` | `DONE(Advanced/MediaPlayer/)` | `DONE(Advanced/MediaPlayer/)` | Android audio and iOS audio have no standalone media player case. |
| Spatial audio | Full RTC or audio SDK | spatial audio engine, remote position, media player spatialization APIs | `DONE(advanced/SpatialSound.java)` | `DONE(advanced/SpatialSound.java)` | `DONE(samples/SpatialSound.kt)` | `DONE(Advanced/SpatialAudio/)` | `DONE(Advanced/SpatialAudio/)` | `DONE(Advanced/SpatialAudio/)` | `DONE(Advanced/SpatialAudio/)` | `DONE(Advanced/SpatialAudio/)` | `DONE(Advanced/SpatialAudio/)` | iOS and Android architecture docs record different key APIs; verify scenario first. |
| Stream encryption | Full RTC | `enableEncryption`, encryption configuration APIs | `DONE(advanced/ChannelEncryption.java)` | `MISSING` | `DONE(samples/ChannelEncryption.kt)` | `DONE(Advanced/StreamEncryption/)` | `DONE(Advanced/StreamEncryption/)` | `DONE(Advanced/StreamEncryption/)` | `MISSING` | `DONE(Advanced/StreamEncryption/)` | `DONE(Advanced/CustomEncrypt/ + MediaEncrypt/)` | Windows has multiple encryption rows; route by requested API. Audio projects lack encryption cases. |
| Pre-call test | Full RTC or audio SDK | `startEchoTest`, `stopEchoTest`, last-mile probe or network test APIs | `DONE(advanced/PreCallTest.java)` | `DONE(advanced/PreCallTest.java)` | `DONE(samples/PreCallTest.kt)` | `DONE(Advanced/PrecallTest/)` | `DONE(Advanced/PrecallTest/)` | `MISSING` | `DONE(Advanced/PrecallTest/)` | `DONE(Advanced/PrecallTest/)` | `DONE(Advanced/PreCallTest/)` | iOS Objective-C has no pre-call test case. |
| Third-party beauty | Full RTC | extension enablement, third-party SDK initialization, video frame processing | `DONE(advanced/ThirdPartyBeauty.java)` | `N/A(audio SDK)` | `MISSING` | `DONE(Advanced/ThirdBeautify/)` | `MISSING` | `MISSING` | `N/A(audio SDK)` | `MISSING` | `MISSING` | Only Android full and iOS UIKit have third-party beauty. Release work must check license expiration. |
| Extension sample | Full RTC extension | `enableExtension`, extension property APIs, matching native headers | `DONE(advanced/SimpleExtension.java)` | `N/A(audio SDK)` | `PARTIAL(VideoProcessExtension covers enableExtension)` | `DONE(Advanced/SimpleFilter/)` | `MISSING` | `DONE(Advanced/SimpleFilter/)` | `N/A(audio SDK)` | `DONE(Advanced/SimpleFilter/)` | `MISSING` | During SDK bumps, verify Android extension include files and iOS/macOS native assets. |
| Audio mixing | Full RTC or audio SDK | `startAudioMixing`, `stopAudioMixing`, `adjustAudioMixingVolume`, effect APIs | `DONE(advanced/PlayAudioFiles.java)` | `DONE(advanced/PlayAudioFiles.java)` | `DONE(samples/PlayAudioFiles.kt)` | `DONE(Advanced/AudioMixing/)` | `DONE(Advanced/AudioMixing/)` | `DONE(Advanced/AudioMixing/)` | `DONE(Advanced/AudioMixing/)` | `DONE(Advanced/AudioMixing/)` | `DONE(Advanced/AudioMixing/)` | Full coverage across all projects. |
| Rhythm player | Full RTC or audio SDK | `startRhythmPlayer`, `stopRhythmPlayer` | `HIDDEN(advanced/RhythmPlayer.java)` | `HIDDEN(advanced/RhythmPlayer.java)` | `HIDDEN(samples/RhythmPlayer.kt)` | `HIDDEN(Advanced/RhythmPlayer/)` | `HIDDEN(Advanced/RhythmPlayer/)` | `HIDDEN(Advanced/RhythmPlayer/)` | `HIDDEN(Advanced/RhythmPlayer/)` | `N/A(no case)` | `N/A(no case)` | All implemented Rhythm Player sources are retained for reference but hidden because the APIs are deprecated since RTC SDK 4.6.0. |
| Media recorder | Full RTC | `createMediaRecorder`, `startRecording`, `stopRecording` | `DONE(advanced/MediaRecorder.java)` | `N/A(audio SDK)` | `DONE(samples/MediaRecorder.kt)` | `DONE(Basic/JoinChannelVideo(Recorder)/)` | `DONE(Basic/JoinChannelVideo(Recorder)/)` | `DONE(Basic/JoinChannelVideo(Recorder)/)` | `N/A(audio SDK)` | `DONE(Basic/JoinChannelVideo(Recorder)/)` | `DONE(Advanced/MediaRecorder/)` | iOS/macOS embed recorder in a join-channel variant; Android/Compose/Windows have standalone cases. |
| Local video transcoding | Full RTC | `startLocalVideoTranscoder`, `startCameraCapture`, compositing APIs | `DONE(advanced/LocalVideoTranscoding.java)` | `N/A(audio SDK)` | `DONE(samples/LocalVideoTranscoding.kt)` | `DONE(Advanced/LocalCompositeGraph/)` | `DONE(Advanced/LocalVideoTranscoding/)` | `DONE(Advanced/LocalCompositeGraph/)` | `N/A(audio SDK)` | `DONE(Advanced/LocalVideoTranscoding/)` | `DONE(Advanced/LocalVideoTranscoding/)` | iOS UIKit and OC name this LocalCompositeGraph; SwiftUI splits transcoding and composition. |
| Multi-channel join | Full RTC | `joinChannelEx`, `leaveChannelEx`, multi-channel connection APIs | `DONE(advanced/JoinMultipleChannel.java)` | `N/A(audio SDK)` | `DONE(samples/JoinMultiChannel.kt)` | `DONE(Advanced/JoinMultiChannel/)` | `DONE(Advanced/JoinMultiChannel/)` | `DONE(Advanced/JoinMultiChannel/)` | `N/A(audio SDK)` | `DONE(Advanced/JoinMultiChannel/)` | `DONE(Advanced/MultiChannel/)` | Full coverage across full-RTC projects. |
| Multipath | Full RTC | `enableMultipath`, `setMultipathConfig`, or `updateChannel` with multipath options | `DONE(advanced/Multipath.java)` | `N/A(audio SDK)` | `MISSING` | `DONE(Advanced/Multipath/)` | `DONE(Advanced/Multipath/)` | `DONE(Advanced/Multipath/)` | `N/A(audio SDK)` | `DONE(Advanced/Multipath/)` | `DONE(Advanced/Multipath/)` | Android Compose has no multipath case. |
| Content inspect | Full RTC | `enableContentInspect` | `DONE(advanced/ContentInspect.java)` | `N/A(audio SDK)` | `MISSING` | `DONE(Advanced/ContentInspect/)` | `DONE(Advanced/ContentInspect/)` | `DONE(Advanced/ContentInspect/)` | `N/A(audio SDK)` | `DONE(Advanced/ContentInspect/)` | `MISSING` | Android Compose and Windows have no content inspect case. |
| Channel media relay | Full RTC | `startOrUpdateChannelMediaRelay`, `stopChannelMediaRelay`, pause/resume APIs | `DONE(advanced/HostAcrossChannel.java)` | `N/A(audio SDK)` | `DONE(samples/HostAcrossChannel.kt)` | `DONE(Advanced/MediaChannelRelay/)` | `DONE(Advanced/MediaChannelRelay/)` | `DONE(Advanced/MediaChannelRelay/)` | `N/A(audio SDK)` | `DONE(Advanced/ChannelMediaRelay/)` | `DONE(Advanced/CrossChannel/)` | Full coverage across full-RTC projects. |
| Picture in picture | Full RTC | `enterPictureInPictureMode` or `AVPictureInPictureController` | `DONE(advanced/PictureInPicture.java)` | `N/A(audio SDK)` | `DONE(samples/PictureInPicture.kt)` | `DONE(Advanced/PictureInPicture/)` | `DONE(Advanced/PictureInPicture/)` | `DONE(Advanced/PictureInPicture/)` | `N/A(audio SDK)` | `MISSING` | `MISSING` | macOS and Windows have no PiP case. |

## Confirmed Gaps

These gaps are derived from the pilot matrix above and should be prioritized when parity work is requested.

| Gap | Affected Units | Severity |
| --- | --- | --- |
| Basic audio-only join channel | Windows | High — missing foundational case |
| Media metadata | macOS | Medium — full-RTC platform gap |
| Data stream messaging | Android audio, iOS audio, Windows | Medium — audio and Windows gap |
| Custom audio render | Windows | Medium — full-RTC platform gap |
| Media player (standalone) | Android audio, iOS audio | Low — audio projects may not need standalone player |
| Stream encryption | Android audio, iOS audio | Low — audio projects may not need encryption |
| Pre-call test | iOS Objective-C | Medium — full-RTC variant gap |
| Third-party beauty | Android Compose, iOS SwiftUI, iOS OC, macOS, Windows | Low — third-party SDK and license overhead |
| Extension sample | iOS SwiftUI, Windows | Medium — extension enablement gap |
| Multipath | Android Compose | Low — Compose variant gap |
| Content inspect | Android Compose, Windows | Low — moderation feature gap |
| Picture in picture | macOS, Windows | Low — platform feature gap |
| Custom video source (push) | iOS SwiftUI | Medium — SwiftUI variant gap |

## Maintenance Rules

- Do not change a `UNKNOWN` cell to `MISSING` until the target project `query-cases` skill or architecture file has been checked.
- Do not change a `PARTIAL` cell to `DONE` unless registration, discoverability, reference parity, review, and build evidence are confirmed for the active work.
- Prefer feature names based on user-visible scenario rather than platform-specific class names.
- Keep API names in the matrix broad enough to route work, then validate exact signatures in the target SDK version.
- Keep release-risk notes in this matrix short; detailed release checks belong in `docs/ai-engineering/release-known-issues.md`.
- When a new case is added to any project, update the matching row in the same edit or follow-up.
