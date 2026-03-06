# AGENTS.md — APIExample-Audio

Audio-only demo project. Uses `voice-sdk` — the video module is not available.
Use this project for audio-only features.

## Build Commands

```bash
./gradlew assembleDebug          # build debug APK
./gradlew installDebug           # build + install to connected device
./gradlew test                   # unit tests
./gradlew connectedAndroidTest   # instrumented tests (device required)
```

## App ID Configuration

See [README.md — Obtain an App Id](README.md#obtain-an-app-id).

## Architecture Red Lines

- Do NOT call `enableVideo()`, `setupLocalVideo()`, or `VideoCanvas` — `voice-sdk` has no video module and will crash at runtime.
- Do NOT add video, screen sharing, or beauty cases — use `APIExample/` instead.
- Each case Fragment must create and destroy its own `RtcEngine` instance.
- Always call `engine.leaveChannel()` before `RtcEngine.destroy()` in `onDestroy()`.
- All `IRtcEngineEventHandler` callbacks run on a background thread — use `handler.post {}` for UI updates.
- Always call `checkOrRequestPermission()` before `joinChannel()`. Audio cases only need `RECORD_AUDIO`.

## Skills

| Skill | Path | Description |
|-------|------|-------------|
| upsert-case | `.agent/skills/upsert-case/` | Add a new audio case or modify an existing one |
| query-cases | `.agent/skills/query-cases/` | Query and browse existing audio cases |
| review-case | `.agent/skills/review-case/` | Review a case against project red lines |

## Further Reading

- `ARCHITECTURE.md` — full directory layout and case registration details
