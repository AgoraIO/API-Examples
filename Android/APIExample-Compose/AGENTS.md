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

Edit `local.properties` in the project root (not in `app/`):
```
AGORA_APP_ID=YOUR_APP_ID
AGORA_APP_CERT=YOUR_CERT
```
The build will throw a `GradleException` if `AGORA_APP_ID` is missing or empty.
Values are injected as `BuildConfig.AGORA_APP_ID` / `BuildConfig.AGORA_APP_CERT` at compile time.

## How to Add a New Case

Registration is **manual** — there is no reflection in this project.

1. Create `app/src/main/java/io/agora/api/example/compose/samples/MyNewCase.kt`:
   ```kotlin
   @Composable
   fun MyNewCase() {
       // RtcEngine + state here
   }
   ```
   Follow the two-function pattern: a public stateful entry + a private stateless `*View` function.
   See `JoinChannelVideo.kt` as the canonical reference.

2. Register in `model/Examples.kt` — add to `BasicExampleList` or `AdvanceExampleList`:
   ```kotlin
   Example(R.string.example_my_new_case) { MyNewCase() }
   ```

3. Add the display string to `app/src/main/res/values/strings.xml`:
   ```xml
   <string name="example_my_new_case">My New Case</string>
   ```

No `nav_graph.xml` changes needed. Navigation routes by list index automatically.

## Architecture Red Lines

- Do NOT use XML layouts, `Fragment`, or `ViewBinding` — Compose only.
- Do NOT use `View`-based widgets directly in Compose UI — wrap with `AndroidView` if unavoidable.
- `RtcEngine` must be created inside `remember { }` and destroyed inside `DisposableEffect(lifecycleOwner) { onDispose { } }` — key must be `lifecycleOwner`, not `Unit`; wrong key means cleanup never fires on back navigation.
- Always call `rtcEngine.leaveChannel()` before `RtcEngine.destroy()` in `onDispose`.
- Permissions use `rememberLauncherForActivityResult(ActivityResultContracts.RequestMultiplePermissions())`.
- `IRtcEngineEventHandler` callbacks are safe to mutate Compose state directly (snapshot system is thread-safe).

## Skills

- `.kiro/skills/add-new-case/` — step-by-step guide for adding a new Compose case (template, `rememberSaveable` rules, verify checklist)
- `.kiro/skills/query-cases/` — query existing cases by API, group, or list position; uses `ARCHITECTURE.md` as fast index

## Further Reading

- `ARCHITECTURE.md` — full directory layout, Composable case pattern, registration details
