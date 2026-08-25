# API Examples Case Maintenance Matrix

This matrix is the planning artifact for parity work across the four primary APIExample projects. It does not replace platform `AGENTS.md`, project `ARCHITECTURE.md`, or project `.agents/skills/*`; every row must be verified against the target project before implementation.

## How To Use

1. Start from the product request and identify the feature name, SDK family, and key SDK APIs.
2. Find the matching row in this matrix.
3. For each target platform unit, verify the path with the project `query-cases` skill when present.
4. Treat each of the four primary projects as an independent delivery/verification unit inside one requirement-level acceptance manifest.
5. Update this matrix when a case is added, moved, renamed, or confirmed not applicable in a primary project.

## Status Values

| Status | Meaning |
| --- | --- |
| `DONE(path)` | Inventory says the case is implemented and expected to be registered at the listed path. Verify before editing. For new or changed parity work, use `DONE` only after the required reference, review, and build gates pass. |
| `HIDDEN(path)` | Source is retained at the listed path, but the case is intentionally not registered in the user-visible list. |
| `PARTIAL(note)` | Exists but parity, registration, docs, verification, runtime smoke, or API coverage needs review. |
| `MISSING` | Required for parity but no implementation is currently recorded here. |
| `N/A(reason)` | Not applicable because the SDK family or platform does not support it. |
| `UNKNOWN` | Not inventoried yet; run the project case query before making decisions. |

## Active Parity Acceptance Rule

For new or changed parity work, do not move a cell to `DONE` from compilation alone.

The platform execution unit must record:

- Reference Contract: source implementation, UI resources, registration, SDK overloads, enum/option values, defaults, limits, and expected success signals.
- Parity Checklist: final implementation matches the reference contract; similar cases are used only for framework patterns.
- Build Result: the target project compile/build command passes, or the exact blocker is recorded.

If runtime/device behavior is required but unavailable, use `PARTIAL(...)` and note the pending smoke check instead of `DONE(...)`.

## Platform Units

Only these primary projects participate in repository-level parity planning and execution:

| Unit | Project | Primary Source |
| --- | --- | --- |
| Android full | `Android/APIExample/` | `Android/APIExample/ARCHITECTURE.md` |
| iOS UIKit | `iOS/APIExample/` | `iOS/APIExample/ARCHITECTURE.md` |
| macOS | `macOS/` | `macOS/ARCHITECTURE.md` |
| Windows | `windows/` | `windows/ARCHITECTURE.md` |

Audio, Compose, SwiftUI, and Objective-C variants are outside this matrix and do not create backlog or acceptance units.

## Pilot Matrix

Seeded from the primary project `ARCHITECTURE.md` indexes and source trees, then re-verified against `dev/4.7.0` (`ee77cc85`). Paths are relative to the projects listed in Platform Units.

Inventory scope: the initial rows were derived from the `Android/APIExample/` case index and its counterparts in the other three primary projects. Features that exist only outside the Android primary project are not inventoried yet, including `ARKit`, `FaceCapture`, `MutliCamera`/`MultiCameraSourece`, `RawMediaData`, `CustomVideoSourceMediaIO`, `VideoChat`, and Windows-only dialogs such as `AudioProfile`, `AudioVolume`, `RegionConn`, `ReportInCall`, `RTMPinject`, and `MultiVideoSource`. Their absence from this matrix is an inventory gap, not a coverage claim.

| Feature | SDK Family | Key APIs | Android full | iOS UIKit | macOS | Windows | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Join channel video | Full RTC | `joinChannel`, `setupLocalVideo`, `setupRemoteVideo` | `DONE(basic/JoinChannelVideo.java)` | `DONE(Basic/JoinChannelVideo/)` | `DONE(Basic/JoinChannelVideo/)` | `PARTIAL(Basic/JoinChannelVideoByToken)` | Windows records the token variant only; non-token parity remains unverified. |
| Join channel video with token | Full RTC | `joinChannel` with token, local and remote video setup | `DONE(basic/JoinChannelVideoByToken.java)` | `DONE(Basic/JoinChannelVideo(Token)/)` | `DONE(Basic/JoinChannelVideo(Token)/)` | `DONE(Basic/JoinChannelVideoByToken/)` | Verify token-generation behavior per platform before changing auth flow. |
| Join channel audio | Full RTC | `joinChannel`, `setAudioProfile`, audio routing or volume APIs | `DONE(basic/JoinChannelAudio.java)` | `DONE(Basic/JoinChannelAudio/)` | `DONE(Basic/JoinChannelAudio/)` | `MISSING` | Windows has no basic audio-only join case. |
| Live streaming role switch | Full RTC | `setClientRole`, `joinChannel`, streaming role configuration | `DONE(advanced/LiveStreaming.java)` | `DONE(Advanced/LiveStreaming/)` | `DONE(Advanced/LiveStreaming/)` | `DONE(Basic/LiveBroadcasting/)` | Names differ by platform; keep product scenario wording stable. |
| RTMP or CDN streaming | Full RTC | `startRtmpStreamWithTranscoding`, `startRtmpStreamWithoutTranscoding`, `stopRtmpStream` | `DONE(advanced/RTMPStreaming.java)` | `DONE(Advanced/RTMPStreaming/)` | `DONE(Advanced/RTMPStreaming/)` | `DONE(Advanced/RTMPStream/)` | Windows also has `RTMPinject`; verify the requested API before routing. |
| Media metadata | Full RTC | `registerMediaMetadataObserver` or platform metadata delegate APIs | `DONE(advanced/MediaMetadata.java)` | `DONE(Advanced/VideoMetadata/)` | `MISSING` | `DONE(Advanced/Metadata/)` | macOS has no metadata case. iOS names this VideoMetadata. |
| Data stream messaging | Full RTC | `createDataStream`, `sendStreamMessage` | `DONE(advanced/SendDataStream.java)` | `DONE(Advanced/CreateDataStream/)` | `DONE(Advanced/CreateDataStream/)` | `MISSING` | Windows has no data stream case. |
| Custom audio source | Full RTC | `createCustomAudioTrack`, `setExternalAudioSource`, `pushExternalAudioFrame` | `DONE(advanced/customaudio/CustomAudioSource.java)` | `DONE(Advanced/CustomPcmAudioSource/)` | `DONE(Advanced/CustomAudioSource/)` | `DONE(Advanced/CustomAudioCapture/)` | Platform APIs differ; compare demonstrated behavior, not only method names. |
| Custom audio render | Full RTC | `setExternalAudioSink`, pull playback audio frame APIs | `DONE(advanced/customaudio/CustomAudioRender.java)` | `DONE(Advanced/CustomAudioRender/)` | `DONE(Advanced/CustomAudioRender/)` | `MISSING` | Windows has no custom audio render case. |
| Custom video source | Full RTC | `setExternalVideoSource`, `pushExternalVideoFrame`, custom video track APIs | `DONE(advanced/PushExternalVideoYUV.java)` | `DONE(Advanced/CustomVideoSourcePush/)` | `DONE(Advanced/CustomVideoSourcePush/)` | `DONE(Advanced/CustomVideoCapture/)` | Multi-source variants are tracked separately. |
| Raw audio data | Full RTC | `registerAudioFrameObserver`, audio frame delegate APIs | `DONE(advanced/ProcessAudioRawData.java)` | `DONE(Advanced/RawAudioData/)` | `DONE(Advanced/RawAudioData/)` | `DONE(Advanced/OriginalAudio/)` | Check callback threading in every platform review. |
| Raw video data | Full RTC | `registerVideoFrameObserver`, video frame delegate APIs | `DONE(advanced/ProcessRawData.java)` | `DONE(Advanced/RawVideoData/)` | `DONE(Advanced/RawVideoData/)` | `DONE(Advanced/OriginalVideo/)` | Raw media combined samples are tracked separately. |
| Screen sharing | Full RTC | `startScreenCapture`, `updateScreenCaptureParameters`, `stopScreenCapture` | `DONE(advanced/ScreenSharing.java)` | `DONE(Advanced/ScreenShare/)` | `DONE(Advanced/ScreenShare/)` | `DONE(Advanced/ScreenShare/)` | iOS requires a ReplayKit extension; Android requires a foreground service. |
| Media player | Full RTC | `createMediaPlayer`, `open`, `play`, publish/update channel options | `DONE(advanced/MediaPlayer.java)` | `DONE(Advanced/MediaPlayer/)` | `DONE(Advanced/MediaPlayer/)` | `DONE(Advanced/MediaPlayer/)` | Standalone media player coverage exists in all four primary projects. |
| Spatial audio | Full RTC | spatial audio engine, remote position, media player spatialization APIs | `DONE(advanced/SpatialSound.java)` | `DONE(Advanced/SpatialAudio/)` | `DONE(Advanced/SpatialAudio/)` | `DONE(Advanced/SpatialAudio/)` | iOS and Android architecture docs record different key APIs; verify the scenario first. |
| Stream encryption | Full RTC | `enableEncryption`, encryption configuration APIs | `DONE(advanced/ChannelEncryption.java)` | `DONE(Advanced/StreamEncryption/)` | `DONE(Advanced/StreamEncryption/)` | `DONE(Advanced/CustomEncrypt/ + MediaEncrypt/)` | Windows has multiple encryption cases; route by requested API. |
| Pre-call test | Full RTC | `startEchoTest`, `stopEchoTest`, last-mile probe or network test APIs | `DONE(advanced/PreCallTest.java)` | `DONE(Advanced/PrecallTest/)` | `DONE(Advanced/PrecallTest/)` | `DONE(Advanced/PreCallTest/)` | Primary-project coverage exists on all four platforms. |
| Third-party beauty | Full RTC | extension enablement, third-party SDK initialization, video frame processing | `DONE(advanced/ThirdPartyBeauty.java)` | `DONE(Advanced/ThirdBeautify/)` | `MISSING` | `MISSING` | Release work must check third-party license expiration. |
| Extension sample | Full RTC extension | `enableExtension`, extension property APIs, matching native headers | `DONE(advanced/SimpleExtension.java)` | `DONE(Advanced/SimpleFilter/)` | `DONE(Advanced/SimpleFilter/)` | `MISSING` | During SDK bumps, verify Android extension include files and Apple native assets. |
| Audio mixing | Full RTC | `startAudioMixing`, `stopAudioMixing`, `adjustAudioMixingVolume`, effect APIs | `DONE(advanced/PlayAudioFiles.java)` | `DONE(Advanced/AudioMixing/)` | `DONE(Advanced/AudioMixing/)` | `DONE(Advanced/AudioMixing/)` | Full coverage across the four primary projects. |
| Rhythm player | Full RTC | `startRhythmPlayer`, `stopRhythmPlayer` | `HIDDEN(advanced/RhythmPlayer.java)` | `HIDDEN(Advanced/RhythmPlayer/)` | `N/A(no case)` | `N/A(no case)` | Android and iOS sources are retained but hidden because the APIs are deprecated since RTC SDK 4.6.0. |
| Media recorder | Full RTC | `createMediaRecorder`, `startRecording`, `stopRecording` | `DONE(advanced/MediaRecorder.java)` | `DONE(Basic/JoinChannelVideo(Recorder)/)` | `DONE(Basic/JoinChannelVideo(Recorder)/)` | `DONE(Advanced/MediaRecorder/)` | iOS and macOS embed recorder behavior in a join-channel variant; Android and Windows use standalone cases. |
| Local video transcoding | Full RTC | `startLocalVideoTranscoder`, `startCameraCapture`, compositing APIs | `DONE(advanced/LocalVideoTranscoding.java)` | `DONE(Advanced/LocalCompositeGraph/)` | `DONE(Advanced/LocalVideoTranscoding/)` | `DONE(Advanced/LocalVideoTranscoding/)` | iOS names the equivalent case LocalCompositeGraph. |
| Multi-channel join | Full RTC | `joinChannelEx`, `leaveChannelEx`, multi-channel connection APIs | `DONE(advanced/JoinMultipleChannel.java)` | `DONE(Advanced/JoinMultiChannel/)` | `DONE(Advanced/JoinMultiChannel/)` | `DONE(Advanced/MultiChannel/)` | Full coverage across the four primary projects. |
| Multipath | Full RTC | `enableMultipath`, `setMultipathConfig`, or `updateChannel` with multipath options | `DONE(advanced/Multipath.java)` | `DONE(Advanced/Multipath/)` | `DONE(Advanced/Multipath/)` | `DONE(Advanced/Multipath/)` | Full coverage across the four primary projects. |
| Content inspect | Full RTC | `enableContentInspect` | `DONE(advanced/ContentInspect.java)` | `DONE(Advanced/ContentInspect/)` | `DONE(Advanced/ContentInspect/)` | `MISSING` | Windows has no content inspect case. |
| Channel media relay | Full RTC | `startOrUpdateChannelMediaRelay`, `stopChannelMediaRelay`, pause/resume APIs | `DONE(advanced/HostAcrossChannel.java)` | `DONE(Advanced/MediaChannelRelay/)` | `DONE(Advanced/ChannelMediaRelay/)` | `DONE(Advanced/CrossChannel/)` | Full coverage across the four primary projects. |
| Picture in picture | Full RTC | `enterPictureInPictureMode` or `AVPictureInPictureController` | `DONE(advanced/PictureInPicture.java)` | `DONE(Advanced/PictureInPicture/)` | `MISSING` | `MISSING` | macOS and Windows have no picture-in-picture case. |
| Voice beautifier and effects | Full RTC | `setVoiceBeautifierPreset`, `setAudioEffectPreset`, `setVoiceConversionPreset` | `DONE(advanced/VoiceEffects.java)` | `DONE(Advanced/VoiceChanger/)` | `DONE(Advanced/VoiceChanger/)` | `DONE(Advanced/BeautyAudio/)` | Windows bundles voice effects with video beauty in `BeautyAudio`. |
| Audio waveform | Full RTC | `enableAudioVolumeIndication`, audio frame observer waveform rendering | `DONE(audio/AudioWaveform.java)` | `DONE(Advanced/AudioWaveform/)` | `MISSING` | `MISSING` | Windows `AudioVolume` adjusts signal volume and is not a waveform case. |
| Third-party player audio routing | Full RTC | third-party player playback combined with the SDK audio route | `DONE(audio/AudioRouterPlayer.java)` | `DONE(Advanced/AudioRouterPlayer/)` | `MISSING` | `MISSING` | Confirm the third-party player dependency is available before porting. |
| Agora beauty (video effect object) | Full RTC | `createVideoEffectObject`, `setVideoEffectStringParam`, `setVideoEffectFloatParam` | `DONE(advanced/AgoraBeauty.java)` | `DONE(Advanced/AgoraBeauty/)` | `DONE(Advanced/AgoraBeauty/)` | `DONE(Advanced/Beauty2.0/)` | Distinct from the built-in `setBeautyEffectOptions` video-enhancement case. |
| Simulcast | Full RTC | `setSimulcastConfig` | `DONE(advanced/Simulcast.java)` | `DONE(Advanced/Simulcast/)` | `DONE(Advanced/Simulcast/)` | `DONE(Advanced/Simulcast/)` | Verified by `setSimulcastConfig`. |
| Multiple custom video tracks | Full RTC | `createCustomVideoTrack`, `joinChannelEx`, `pushExternalVideoFrameById` | `DONE(advanced/MultiVideoSourceTracks.java)` | `DONE(Advanced/CustomVideoSourcePushMulti/)` | `DONE(Advanced/CustomVideoSourcePushMulti/)` | `DONE(Advanced/MultiVideoSourceTracks/)` | Case names differ per platform; correspondence is verified by `createCustomVideoTrack`. |
| Custom remote video render | Full RTC | remote render path via `registerVideoFrameObserver` or `setVideoFrameDelegate` | `DONE(advanced/CustomRemoteVideoRender.java)` | `DONE(Advanced/CustomVideoRender/)` | `DONE(Advanced/CustomVideoRender/)` | `MISSING` | Windows `OriginalVideo` is raw video data and is tracked in its own row. |
| Transparent background rendering | Full RTC | alpha mask and alpha stitch mode with encoded alpha | `DONE(advanced/TransparentRendering.java)` | `DONE(Advanced/TransparentRender/)` | `MISSING` | `DONE(Advanced/TransparentBg/)` | macOS uses alpha options only inside `VideoProcess`/`AgoraBeauty`, not in a dedicated case. |
| Quick channel switch | Full RTC | rapid `joinChannel` / `leaveChannel` switching across channels | `DONE(advanced/VideoQuickSwitch.java)` | `DONE(Advanced/QuickSwitchChannel/)` | `DONE(Advanced/QuickSwitchChannel/)` | `MISSING` | Correspondence is by scenario and case name; this feature has no single distinctive API. |
| URL live stream player | Full RTC | RTE player `openWithUrl`, `startWithUrl` | `DONE(advanced/UrlLiveStream.java)` | `UNKNOWN` | `UNKNOWN` | `UNKNOWN` | Apple and Windows ship an `RtePlayer` case that was not confirmed to demonstrate the same scenario. Run `query-cases` before treating these as gaps. |
| KTV copyright music | None (docs link) | none - the case only opens the Agora documentation page | `DONE(advanced/KtvCopyrightMusic.java)` | `DONE(Advanced/KtvCopyrightMusic/)` | `N/A(docs-link case)` | `N/A(docs-link case)` | The Android and iOS implementations only open a docs URL; absence elsewhere is not a parity gap. |

## Confirmed Gaps

These gaps are derived from the primary-project matrix above and should be prioritized when parity work is requested.

| Gap | Affected Units | Severity |
| --- | --- | --- |
| Join channel video | Windows | High - foundational non-token flow remains partial |
| Basic audio-only join channel | Windows | High - missing foundational case |
| Media metadata | macOS | Medium - full-RTC platform gap |
| Data stream messaging | Windows | Medium - full-RTC platform gap |
| Custom audio render | Windows | Medium - full-RTC platform gap |
| Third-party beauty | macOS, Windows | Low - third-party SDK and license overhead |
| Extension sample | Windows | Medium - extension enablement gap |
| Content inspect | Windows | Low - moderation feature gap |
| Picture in picture | macOS, Windows | Low - platform feature gap |
| Audio waveform | macOS, Windows | Medium - audio visualization gap |
| Third-party player audio routing | macOS, Windows | Low - depends on third-party player dependencies |
| Custom remote video render | Windows | Medium - full-RTC platform gap |
| Transparent background rendering | macOS | Low - niche rendering feature |
| Quick channel switch | Windows | Low - switching-performance demo gap |

## Maintenance Rules

- Keep the matrix header fixed to `Android full`, `iOS UIKit`, `macOS`, and `Windows`; project variants are outside repository-level parity scope.
- Do not change an `UNKNOWN` cell to `MISSING` until the primary target project's `query-cases` skill or architecture file has been checked.
- Do not change a `PARTIAL` cell to `DONE` unless registration, discoverability, reference parity, review, and build evidence are confirmed for the active work.
- Prefer feature names based on user-visible scenario rather than platform-specific class names.
- Keep API names in the matrix broad enough to route work, then validate exact signatures in the target SDK version.
- Keep release-risk notes in this matrix short; detailed release checks belong in `docs/ai-engineering/release-known-issues.md`.
- When a new case is added to a primary project, update the matching row in the same edit or follow-up.
