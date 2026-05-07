# AGENTS.md — APIExample

Full demo project. Covers all Agora RTC APIs using Java/Kotlin + XML layouts.
Default project for video, screen sharing, beauty, or extension demos.

## Build Commands

```bash
./gradlew assembleDebug          # build debug APK
./gradlew installDebug           # build + install to connected device
./gradlew test                   # unit tests
./gradlew connectedAndroidTest   # instrumented tests (device required)
```

## App ID Configuration

See [README.md — Obtain an App Id](README.md#obtain-an-app-id).

## Optional Modules

Controlled via `gradle.properties`:
- `simpleFilter = true` — enables the C++ video extension module (`agora-simple-filter`). Requires OpenCV and Agora C++ SDK headers. See README for setup.
- `streamEncrypt = true` — enables the custom stream encryption module (`agora-stream-encrypt`). Requires Agora C++ SDK headers. See README for setup.

Both are `false` by default. Do not enable unless the feature explicitly requires it.

## Architecture Red Lines

- Do NOT add audio-only cases that require `voice-sdk` exclusivity — use `APIExample-Audio/` instead.
- Do NOT use Jetpack Compose — this project is XML + ViewBinding only.
- Each case Fragment must create and destroy its own `RtcEngine` instance.
- Always call `engine.leaveChannel()` before `RtcEngine.destroy()` in `onDestroy()`.
- Call `RtcEngine.destroy()` via `handler.post(RtcEngine::destroy)` — direct call blocks the main thread (ANR).
- All `IRtcEngineEventHandler` callbacks run on a background thread — use `runOnUIThread()` for UI updates.
- Always call `checkOrRequestPermission()` before `joinChannel()`.
- `setParameters(...)` is required in every case for backend reporting — do not omit it.
- Always null-check `getPrivateCloudConfig()` before calling `setLocalAccessPoint()` — returns null on non-private-cloud builds.

## Skills

| Skill | Path | Description |
|-------|------|-------------|
| upsert-case | `.agents/skills/upsert-case/` | Add a new case or modify an existing one |
| query-cases | `.agents/skills/query-cases/` | Query and browse existing cases |
| review-case | `.agents/skills/review-case/` | Review a case against project red lines |

## Further Reading

- `ARCHITECTURE.md` — full directory layout, case registration internals, navigation details
