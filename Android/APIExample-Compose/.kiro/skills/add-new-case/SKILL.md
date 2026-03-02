---
name: add-new-case
description: >
  Add a new API example case to the APIExample-Compose Android demo — creates a
  Kotlin Composable file, registers it in Examples.kt, and adds a string resource.
  Use when: adding a new Agora RTC API demo screen in Jetpack Compose, porting an
  existing APIExample case to Compose, implementing a new feature example in Kotlin
  + Compose UI, or registering a new entry in BasicExampleList or AdvanceExampleList.
  Kotlin only — no XML layouts, no Fragments. Keywords: add case, new composable,
  Examples.kt, BasicExampleList, AdvanceExampleList, APIExample-Compose, Compose case,
  new screen, Jetpack Compose, RTC API example.
---

# Add New Case — APIExample-Compose

Touch exactly 3 files (all paths relative to `app/src/main/`):

| File | What to add |
|---|---|
| `java/.../compose/samples/YourCaseName.kt` | Composable file |
| `java/.../compose/model/Examples.kt` | 1 list entry |
| `res/values/strings.xml` | 1 string |

No `nav_graph.xml` changes — navigation routes by list position automatically.

---

## Step 1: Clarify before coding

Before writing a single line, ask:
- **What API am I demonstrating?** — determines which existing case is the closest reference (`JoinChannelVideo.kt` for video, `JoinChannelAudio.kt` for audio)
- **Video or audio-only?** — determines permissions (`CAMERA` + `RECORD_AUDIO` vs `RECORD_AUDIO` only), whether `enableVideo()` and `VideoGrid` are needed
- **BasicExampleList or AdvanceExampleList?** — Basic for fundamental join/leave patterns; Advance for feature-specific APIs
- **List position?** — run `query-cases` skill to see current entries; list order is display order

---

## Step 2: Create the Composable file

**MANDATORY — READ ENTIRE FILE before writing any code**:
[`references/composable-template.kt`](references/composable-template.kt)

Do NOT skip — the `SettingPreferences.getArea()`, `DisposableEffect` key, `rememberSaveable` vs `remember` rules, and `@Preview` placement are only fully shown there and are required in every case.

**Do NOT load** any other reference files for this task.

Non-obvious points the template highlights:

- `mAreaCode = SettingPreferences.getArea()` — **required**, do not hardcode or omit
- `DisposableEffect(lifecycleOwner)` — key must be `lifecycleOwner`, not `Unit`; wrong key means cleanup never fires on back navigation
- `rememberSaveable` for channelName, isJoined, uid, videoIdList — survives rotation
- `remember` for RtcEngine — must NOT be `rememberSaveable` (engine is not serializable)
- `IRtcEngineEventHandler` callbacks can mutate Compose state directly — snapshot system is thread-safe, no `runOnUIThread()` needed
- `Toast`/`Dialog`/`AlertDialog` inside callbacks still need main thread — use `coroutineScope.launch(Dispatchers.Main) { }`
- `@Preview` goes on the **private** `*View` function only — never on the public stateful entry

---

## Step 3: Register in Examples.kt

File: `app/src/main/java/io/agora/api/example/compose/model/Examples.kt`

```kotlin
val AdvanceExampleList = listOf(
    // … existing entries …
    Example(R.string.example_your_case_name) { YourCaseName() }
)
```

List order is display order — position determines where the case appears in the UI.

---

## Step 4: Add string resource

File: `app/src/main/res/values/strings.xml`

```xml
<string name="example_your_case_name">Your Case Name</string>
```

String key must use the `example_` prefix. No separate tips string needed (unlike APIExample).

---

## Step 5: Verify

```bash
./gradlew assembleDebug
```

- [ ] Case appears in the correct group at the expected list position
- [ ] Tap navigates to the case screen
- [ ] Channel join succeeds and `isJoined` flips to `true`
- [ ] Press back — check Logcat for `RtcEngine.destroy` within ~2 seconds; if missing, `DisposableEffect` key is wrong or `onDispose` is incomplete
- [ ] Rotate screen — `channelName` and `isJoined` survive (`rememberSaveable` working)

---

## NEVER

- **NEVER** use XML layouts, `Fragment`, or `ViewBinding` — Compose only.
- **NEVER** use `remember` for channelName, isJoined, or uid — they must be `rememberSaveable` to survive rotation.
- **NEVER** use `rememberSaveable` for `RtcEngine` — it is not serializable and will crash on rotation.
- **NEVER** use `Unit` as the `DisposableEffect` key — it fires only once and won't clean up on back navigation. Always use `lifecycleOwner`.
- **NEVER** put `@Preview` on the public stateful function — it will crash because `LocalContext` and `LocalLifecycleOwner` are unavailable in preview. Only preview the private `*View` function.
- **NEVER** call `Toast`/`Dialog`/`AlertDialog` directly inside `IRtcEngineEventHandler` callbacks — they require the main thread. Use `coroutineScope.launch(Dispatchers.Main) { }`.
- **NEVER** hardcode `mAreaCode` — always use `SettingPreferences.getArea()`.
