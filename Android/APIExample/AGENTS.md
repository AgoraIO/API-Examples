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

4. Update `ARCHITECTURE.md` — add one line to the case list under the correct directory section (`basic/`, `advanced/`, or `audio/`). This keeps the fast-lookup index used by `query-cases` current.

5. Build and run — the case appears automatically via reflection. No other registration needed.

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

Use the `add-new-case` Skill for straightforward cases. Use the Spec workflow (via `complex-case-spec` steering) when the case meets any complexity criteria — see below.

- `.kiro/skills/add-new-case/` — step-by-step guide for adding a new case (template, nav placement, verify checklist)
- `.kiro/skills/query-cases/` — query existing cases by API, group, or sort index; uses `ARCHITECTURE.md` as fast index

### When to use Spec instead of the Skill

Use the Spec workflow if the case meets **any** of these criteria:
1. Involves coordinated calls across two or more Agora API modules
2. Requires a custom UI layout (not one of the standard Layout A/B/C/D templates)
3. Involves multi-channel or multi-engine instance management
4. Requires a foreground Service or background thread coordination
5. Involves developing new shared components (widgets/utils, etc.)
6. Requires optional module integration (`simpleFilter`/`streamEncrypt`)

Activate the `complex-case-spec` steering file (manual inclusion) for the full Spec requirements and design checklist.

## Hooks

- `.kiro/hooks/build-on-task-complete.json` — runs `./gradlew assembleDebug` automatically after each spec task completes to catch compile errors early

## Steering Files

| File | Inclusion | Purpose |
|------|-----------|---------|
| `.kiro/steering/coding-standards.md` | Always | RtcEngine lifecycle rules, Java code standards, naming conventions |
| `.kiro/steering/project-routing.md` | Always | Which sub-project to use for a given scenario |
| `.kiro/steering/complex-case-spec.md` | Manual | Spec requirements/design checklist for complex cases |

## Further Reading

- `ARCHITECTURE.md` — full directory layout, case registration internals, navigation details
