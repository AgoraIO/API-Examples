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
- Keep one remembered `RtcEngine` per case, directly or in a remembered case session holder. Pair ownership with `DisposableEffect(rtcEngine)` or `DisposableEffect(session) { onDispose { } }`; a session holder creates its engine in that effect. The effect must clean up the exact engine it owns. `onDispose` runs when the effect leaves composition or a key changes; `Unit` is valid when ownership is constant. Use a lifecycle-owner key only when engine creation and teardown both follow that owner's lifetime.
- Save user inputs with `rememberSaveable`; keep live session state such as joined status and assigned UID with the current engine so it resets when that engine is recreated.
- Always call `rtcEngine.leaveChannel()` before `RtcEngine.destroy()` in `onDispose`.
- Permissions use `rememberLauncherForActivityResult(ActivityResultContracts.RequestMultiplePermissions())`.
- `IRtcEngineEventHandler` callbacks are safe to mutate Compose state directly (snapshot system is thread-safe).

## Skills

| Skill | Path | Description |
|-------|------|-------------|
| upsert-case | `.agents/skills/upsert-case/` | Add a new Compose case or modify an existing one |
| query-cases | `.agents/skills/query-cases/` | Query and browse existing Compose cases |
| review-case | `.agents/skills/review-case/` | Review a case against project red lines |

## Further Reading

- `ARCHITECTURE.md` — full directory layout, Composable case pattern, registration details
