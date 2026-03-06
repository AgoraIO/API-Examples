# AGENTS.md — APIExample-Compose

Jetpack Compose version of the API demo. Mirrors cases from `APIExample/` but uses
`@Composable` functions instead of Fragments + XML layouts. Kotlin only.

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

- Do NOT use XML layouts, `Fragment`, or `ViewBinding` — Compose only.
- Do NOT use `View`-based widgets directly in Compose UI — wrap with `AndroidView` if unavoidable.
- `RtcEngine` must be created inside `remember { }` and destroyed inside `DisposableEffect(lifecycleOwner) { onDispose { } }` — key must be `lifecycleOwner`, not `Unit`; wrong key means cleanup never fires on back navigation.
- Always call `rtcEngine.leaveChannel()` before `RtcEngine.destroy()` in `onDispose`.
- Permissions use `rememberLauncherForActivityResult(ActivityResultContracts.RequestMultiplePermissions())`.
- `IRtcEngineEventHandler` callbacks are safe to mutate Compose state directly (snapshot system is thread-safe).

## Skills

| Skill | Path | Description |
|-------|------|-------------|
| upsert-case | `.agent/skills/upsert-case/` | Add a new Compose case or modify an existing one |
| query-cases | `.agent/skills/query-cases/` | Query and browse existing Compose cases |
| review-case | `.agent/skills/review-case/` | Review a case against project red lines |

## Further Reading

- `ARCHITECTURE.md` — full directory layout, Composable case pattern, registration details
