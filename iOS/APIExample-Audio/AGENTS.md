# AGENTS.md — APIExample-Audio

Audio-only demo project. Uses `AgoraAudio_iOS` SDK — the video module is not included.

## Build Commands

```bash
pod install
# Then open APIExample-Audio.xcworkspace in Xcode and build (Cmd+B)
```

## App ID Configuration

Edit `APIExample-Audio/Common/KeyCenter.swift`:
```swift
static let AppId: String = "YOUR_APP_ID"
static let Certificate: String? = nil   // leave nil if App Certificate is not enabled
```

## Architecture Red Lines

- Do NOT call any video API: `enableVideo()`, `setupLocalVideo()`, `setupRemoteVideo()`, `startPreview()` — the SDK has no video module
- Do NOT add video rendering views (`AgoraRtcVideoCanvas`, `VideoView`) to any case in this project
- Do NOT skip calling `leaveChannel()` + `AgoraRtcEngineKit.destroy()` in `willMove(toParent:)` when `parent == nil`
- Do NOT update UI directly inside `AgoraRtcEngineDelegate` callbacks — always dispatch to `DispatchQueue.main`
- Do NOT create `AgoraRtcEngineKit` in the Entry VC — engine lifecycle belongs to Main VC only
- Do NOT share `AgoraRtcEngineKit` instances across examples
- Do NOT request microphone permission after calling `joinChannel()`

## Skills

| Task | Skill | When to use |
|------|-------|-------------|
| Add or modify a case | `.agent/skills/upsert-case/` | Need to create a new audio API demo or update an existing one |
| Code review | `.agent/skills/review-case/` | Review case code for lifecycle, thread safety, and audio-only convention compliance |
| Find an existing case | `.agent/skills/query-cases/` | Locate which file demonstrates a specific API or feature |

## Further Reading

- `ARCHITECTURE.md` — full directory layout, case registration, Entry/Main pattern, engine lifecycle
