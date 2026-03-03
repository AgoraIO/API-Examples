# AGENTS.md — APIExample-Audio

Audio-only demo project. Uses `voice-sdk` — the video module is not available.
Use this project only when the PRD is explicitly audio-only.

## Build Commands

```bash
./gradlew assembleDebug          # build debug APK
./gradlew installDebug           # build + install to connected device
./gradlew test                   # unit tests
./gradlew connectedAndroidTest   # instrumented tests (device required)
```

## App ID Configuration

Edit `app/src/main/res/values/string-config.xml`:
```xml
<string name="agora_app_id" translatable="false">YOUR_APP_ID</string>
<string name="agora_app_certificate" translatable="false">YOUR_CERT</string>
<string name="agora_access_token" translatable="false">YOUR_ACCESS_TOKEN</string>
```
Leave certificate and token blank if your project has not enabled App Certificate.

## How to Add a New Case

The registration mechanism is identical to `APIExample`. Required steps:

1. Create a Fragment class under `app/src/main/java/io/agora/api/example/examples/{basic|advanced|audio}/`
   - Extend `BaseFragment`
   - Add `@Example(index, group, name, actionId, tipsId)` annotation

2. Add string resources to `app/src/main/res/values/strings.xml`:
   - `name` string (display name in the list)
   - `tips` string (description shown on the case screen)

3. Add navigation entries to `app/src/main/res/navigation/nav_graph.xml`:
   - `<fragment>` destination for the new Fragment
   - `<action>` from `mainFragment` with the `actionId` used in the annotation

4. Build and run — the case appears automatically via reflection.

## Architecture Red Lines

- Do NOT call `enableVideo()`, `setupLocalVideo()`, or `VideoCanvas` — `voice-sdk` has no video module and will crash at runtime.
- Do NOT add video, screen sharing, or beauty cases — use `APIExample/` instead.
- Each case Fragment must create and destroy its own `RtcEngine` instance.
- Always call `engine.leaveChannel()` before `RtcEngine.destroy()` in `onDestroy()`.
- All `IRtcEngineEventHandler` callbacks run on a background thread — use `handler.post {}` for UI updates.
- Always call `checkOrRequestPermission()` before `joinChannel()`. Audio cases only need `RECORD_AUDIO`.

## Skills

- `.kiro/skills/add-new-case/` — step-by-step guide for adding a new audio case (voice-sdk template, nav placement, verify checklist)
- `.kiro/skills/query-cases/` — query existing cases by API, group, or sort index; uses `ARCHITECTURE.md` as fast index

## Hooks

- `.kiro/hooks/build-on-task-complete.json` — automatically runs `./gradlew assembleDebug` after each Spec task completes to verify the build

## Steering
| File | Inclusion | Purpose |
|------|-----------|---------|
| `.kiro/steering/project-routing.md` | Always | routes work to the correct sub-project; enforces audio-only constraint
| `.kiro/steering/coding-standards.md` | Always | RtcEngine lifecycle rules, Java standards, voice-sdk constraints, naming conventions
| `.kiro/steering/complex-case-spec.md`| Manual | Spec workflow for complex cases; include via `#complex-case-spec` when creating a Spec

## Further Reading

- `ARCHITECTURE.md` — full directory layout and case registration details
