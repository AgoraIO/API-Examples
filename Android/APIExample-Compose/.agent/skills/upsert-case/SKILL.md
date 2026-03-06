---
name: upsert-case
description: >
  Add a new API example case or modify an existing one in the APIExample-Compose Android demo —
  creates or updates a Kotlin Composable file, registers or updates it in Examples.kt, and manages
  string resources. Use when: adding a new Agora RTC API demo screen in Jetpack Compose, modifying
  an existing case's implementation or registration, porting an existing APIExample case to Compose,
  implementing a new feature example in Kotlin + Compose UI, registering a new entry in
  BasicExampleList or AdvanceExampleList, or updating an existing case's strings or Examples.kt entry.
  Kotlin only — no XML layouts, no Fragments. Keywords: add case, modify case, update case,
  new composable, Examples.kt, BasicExampleList, AdvanceExampleList, APIExample-Compose, Compose case,
  new screen, Jetpack Compose, RTC API example, upsert case.
---

# Upsert Case — APIExample-Compose

## Adding a New Case

Touch exactly 3 files (all paths relative to `app/src/main/`):

| File | What to add |
|---|---|
| `java/.../compose/samples/YourCaseName.kt` | Composable file |
| `java/.../compose/model/Examples.kt` | 1 list entry |
| `res/values/strings.xml` | 1 string |

No `nav_graph.xml` changes — navigation routes by list position automatically.

---

### Step 1: Clarify before coding

Before writing a single line, ask:
- **What API am I demonstrating?** — determines which existing case is the closest reference (`JoinChannelVideo.kt` for video, `JoinChannelAudio.kt` for audio)
- **Video or audio-only?** — determines permissions (`CAMERA` + `RECORD_AUDIO` vs `RECORD_AUDIO` only), whether `enableVideo()` and `VideoGrid` are needed
- **BasicExampleList or AdvanceExampleList?** — Basic for fundamental join/leave patterns; Advance for feature-specific APIs
- **List position?** — run `query-cases` skill to see current entries; list order is display order

---

### Step 2: Create the Composable file

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

### Step 3: Register in Examples.kt

File: `app/src/main/java/io/agora/api/example/compose/model/Examples.kt`

```kotlin
val AdvanceExampleList = listOf(
    // … existing entries …
    Example(R.string.example_your_case_name) { YourCaseName() }
)
```

List order is display order — position determines where the case appears in the UI.

---

### Step 4: Add string resource

File: `app/src/main/res/values/strings.xml`

```xml
<string name="example_your_case_name">Your Case Name</string>
```

String key must use the `example_` prefix. No separate tips string needed (unlike APIExample).

---

### Step 5: Update ARCHITECTURE.md

Add one line to the case list in `ARCHITECTURE.md` under the correct directory section:

```
├── YourCaseName.kt    # "Display Name" — key API description
```

Keep the format consistent with existing entries. This file is the fast-lookup index used by `query-cases` — keeping it current avoids full directory scans.

---

## Modifying an Existing Case

When modifying an existing case rather than creating a new one, identify which files need changes based on what you are updating:

| What changed | Files to touch |
|---|---|
| Implementation logic (API calls, event handling, Compose state) | `java/.../compose/samples/CaseName.kt` |
| Display name | `res/values/strings.xml` |
| List group (Basic ↔ Advance) or position | `java/.../compose/model/Examples.kt` (move entry between lists or reorder) |
| Composable function rename | `CaseName.kt` (file + function name), `Examples.kt` (lambda reference), `ARCHITECTURE.md` |

After making changes:

1. **Verify `Examples.kt` entry consistency** — ensure the string resource reference, composable lambda, and list placement (`BasicExampleList` or `AdvanceExampleList`) still match the actual case. A mismatch causes the case to silently disappear from the list or render the wrong screen.
2. **Update `res/values/strings.xml`** if the display name changed.
3. **Update `ARCHITECTURE.md`** — update the Directory Layout entry and the Case Index table row to reflect any changes to the case name, path, Key APIs, or description.

---

## Verify

```bash
./gradlew assembleDebug
```

- [ ] Case appears in the correct group at the expected list position
- [ ] Tap navigates to the case screen
- [ ] Channel join succeeds and `isJoined` flips to `true`
- [ ] Press back — check Logcat for `RtcEngine.destroy` within ~2 seconds; if missing, `DisposableEffect` key is wrong or `onDispose` is incomplete
- [ ] Rotate screen — `channelName` and `isJoined` survive (`rememberSaveable` working)
- [ ] `ARCHITECTURE.md` Case Index table is updated — row added (new case) or row updated (modified case) with correct Case, Path, Key APIs, and Description
- [ ] `Examples.kt` entry is consistent — string resource, composable lambda, and list placement match the actual case

---

## When to Use a Spec Instead

If the case meets any of the following criteria, create a Spec rather than using this skill directly:

1. Involves coordinated calls across two or more Agora API modules
2. Requires a custom Composable layout not covered by the standard template above
3. Manages multiple channels or multiple engine instances
4. Requires a foreground Service or background coroutine coordination
5. Involves developing new shared components (shared Composables / utils)
6. Requires optional module integration (simpleFilter / streamEncrypt)

If none apply → use this skill directly; no Spec needed.

### Spec Requirements Document Must Include

- List of APIs the case demonstrates
- User interaction flow description
- Expected RtcEngine lifecycle behavior
- Required permissions list

### Spec Design Document Must Include

- Target project identifier: `APIExample-Compose`
- Composable function structure design
- API call sequence (Mermaid sequence diagram recommended)
- State management plan (`remember` vs `rememberSaveable` boundaries)
- UI layout plan
- Integration points with existing shared components
- Case registration info: `Examples.kt` list entry, `strings.xml` key (`example_` prefix) — finalize during design to avoid conflicts
- Read `ARCHITECTURE.md` or use the `query-cases` skill to check existing entries
- Compose-specific checks: `DisposableEffect(lifecycleOwner)`, `rememberSaveable` vs `remember`, main-thread dispatch for Toast/Dialog
- Risk identification and mitigation (API compatibility, performance, permissions, thread safety, rotation/config changes)

### Spec Task List Integration

- Mark which sub-tasks can be executed with this `upsert-case` skill, and provide skill input parameters
- Mark which sub-tasks require manual coding, and provide target file paths and change summaries
- Tasks for creating new shared Composables must come before case implementation tasks

---

## NEVER

- **NEVER** use XML layouts, `Fragment`, or `ViewBinding` — Compose only.
- **NEVER** use `remember` for channelName, isJoined, or uid — they must be `rememberSaveable` to survive rotation.
- **NEVER** use `rememberSaveable` for `RtcEngine` — it is not serializable and will crash on rotation.
- **NEVER** use `Unit` as the `DisposableEffect` key — it fires only once and won't clean up on back navigation. Always use `lifecycleOwner`.
- **NEVER** put `@Preview` on the public stateful function — it will crash because `LocalContext` and `LocalLifecycleOwner` are unavailable in preview. Only preview the private `*View` function.
- **NEVER** call `Toast`/`Dialog`/`AlertDialog` directly inside `IRtcEngineEventHandler` callbacks — they require the main thread. Use `coroutineScope.launch(Dispatchers.Main) { }`.
- **NEVER** hardcode `mAreaCode` — always use `SettingPreferences.getArea()`.
