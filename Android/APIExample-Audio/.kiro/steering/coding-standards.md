---
inclusion: always
---

# Coding Standards (APIExample-Audio)

## RtcEngine Lifecycle Red Lines

1. Each case owns its own `RtcEngine` instance — never share across screens
2. On screen destruction, always call `leaveChannel()` before `RtcEngine.destroy()`
3. `IRtcEngineEventHandler` callbacks run on a background thread — UI updates must switch to the main thread
4. Always request permissions before calling `joinChannel()`

## Java Code Standards

- `RtcEngine.destroy()` must be called via `handler.post(RtcEngine::destroy)` — calling it directly blocks the main thread and causes ANR
- Always null-check the return value of `getPrivateCloudConfig()` — it returns null on non-private-cloud builds
- `setParameters(...)` is required in every case for backend reporting — do not omit
- Use `runOnUIThread()` for all UI updates

## voice-sdk Constraints

- This project uses `voice-sdk` — **any video API call is forbidden**
- Banned: `enableVideo()`, `setupLocalVideo()`, `setupRemoteVideo()`, `VideoCanvas`, `startScreenCapture()`
- Only audio APIs are allowed: `enableAudio()`, `adjustRecordingSignalVolume()`, etc.

## Naming Conventions

- Case class names: PascalCase, placed under `examples/{basic|advanced|audio}/`
- String resources: `item_` prefix
- Layout files: `fragment_` + snake_case
