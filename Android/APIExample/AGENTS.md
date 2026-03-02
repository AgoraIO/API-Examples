# AGENTS.md — APIExample

Full demo project. Covers all Agora RTC APIs using Java/Kotlin + XML layouts.
Default project when the PRD does not specify a platform variant.

## Build Commands

```bash
./gradlew assembleDebug          # build debug APK
./gradlew installDebug           # build + install to connected device
./gradlew test                   # unit tests
./gradlew connectedAndroidTest   # instrumented tests (device required)
```

## App ID Configuration

Edit `app/src/main/res/values/string-configs.xml`:
```xml
<string name="agora_app_id" translatable="false">YOUR_APP_ID</string>
<string name="agora_app_certificate" translatable="false">YOUR_CERT</string>
```
Leave certificate blank if your project has not enabled App Certificate.

## Optional Modules

Controlled via `gradle.properties`:
- `simpleFilter = true` — enables the C++ video extension module (`agora-simple-filter`). Requires OpenCV and Agora C++ SDK headers. See README for setup.
- `streamEncrypt = true` — enables the custom stream encryption module (`agora-stream-encrypt`). Requires Agora C++ SDK headers. See README for setup.

Both are `false` by default. Do not enable them unless the PRD explicitly requires it.

## How to Add a New Case

See `ARCHITECTURE.md` for the full registration mechanism. The required steps are:

1. Create a Fragment class under `app/src/main/java/io/agora/api/example/examples/{basic|advanced|audio}/`
   - Extend `BaseFragment` (or `BaseVbFragment` for ViewBinding)
   - Add `@Example(index, group, name, actionId, tipsId)` annotation

2. Add string resources to `app/src/main/res/values/strings.xml`:
   - `name` string (display name in the list)
   - `tips` string (description shown on the case screen)

3. Add navigation entries to `app/src/main/res/navigation/nav_graph.xml`:
   - `<fragment>` destination (under root `<navigation>`) for the new Fragment
   - `<action>` inside `<fragment id="Ready">` — **not** `mainFragment`; `mainFragment` only routes to `Ready`

4. Build and run — the case appears automatically via reflection. No other registration needed.

## Architecture Red Lines

- Do NOT add audio-only cases that require `voice-sdk` exclusivity — use `APIExample-Audio/` instead.
- Do NOT use Jetpack Compose — this project is XML + ViewBinding only.
- Each case Fragment must create and destroy its own `RtcEngine` instance.
- Always call `engine.leaveChannel()` before `RtcEngine.destroy()` in `onDestroy()`.
- All `IRtcEngineEventHandler` callbacks run on a background thread — use `handler.post {}` for UI updates.
- Always call `checkOrRequestPermission()` before `joinChannel()`.

## Skills

- `.kiro/skills/add-new-case/` — step-by-step guide for adding a new case (template, nav placement, verify checklist)
- `.kiro/skills/query-cases/` — query existing cases by API, group, or sort index; uses `ARCHITECTURE.md` as fast index

## Further Reading

- `ARCHITECTURE.md` — full directory layout, case registration internals, navigation details
