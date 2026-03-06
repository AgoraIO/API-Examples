---
name: upsert-case
description: >
  Add a new audio API example case or modify an existing one in the APIExample-Audio Android demo —
  creates or updates Fragment class, XML layout, string resources, and nav_graph registration.
  Use when: adding a new Agora audio API demo screen, modifying an existing case's implementation
  or registration, implementing a new audio feature example in Java + XML layouts, registering a new
  case via @Example annotation, subclassing BaseFragment for a new audio demo screen, or updating
  an existing case's strings, layout, or nav entry.
  This project uses voice-sdk — no video APIs available.
  Keywords: add case, modify case, update case, new fragment, nav_graph, @Example, BaseFragment,
  APIExample-Audio, audio case, voice-sdk, new screen, audio demo, upsert case.
---

# Upsert Case — APIExample-Audio

## Adding a New Case

Touch exactly 4 files (all paths relative to `app/src/main/`):

| File | What to add |
|---|---|
| `java/.../examples/{basic\|advanced\|audio}/YourCaseName.java` | Fragment class |
| `res/layout/fragment_your_case_name.xml` | XML layout |
| `res/values/strings.xml` | 2 strings |
| `res/navigation/nav_graph.xml` | 1 action + 1 destination |

Registration is automatic via reflection — no other files needed.

**voice-sdk constraint**: Do NOT call `enableVideo()`, `setupLocalVideo()`, `VideoCanvas`, or any video API — the module does not exist and will crash at runtime.

---

### Step 1: Clarify before coding

Before writing a single line, ask:
- **What audio API am I demonstrating?** — determines which existing case is the closest reference to copy patterns from
- **BASIC or ADVANCED group?** — BASIC for fundamental join/leave audio patterns; ADVANCED for feature-specific audio APIs
- **What's the sort index?** — index must be unique within the group. BASIC uses 0–9, ADVANCED starts from 10. Run `query-cases` skill first; a collision causes silent ordering bugs at runtime
- **Any special permissions beyond `RECORD_AUDIO`?** — most audio cases only need `RECORD_AUDIO`; check if the API requires anything else

---

### Step 2: Create the Fragment

**MANDATORY — READ ENTIRE FILE before writing any code**:
[`references/fragment-template.java`](references/fragment-template.java)

Do NOT skip — the `setParameters`, `handler.post`, `getPrivateCloudConfig()` null-check, `AudioSeatManager` wiring, and voice-sdk constraints are only fully shown there and are required in every case.

**Do NOT load** any other reference files for this task.

Non-obvious points the template highlights:

- `setParameters(...)` for app scenario reporting — **required in every case**, do not remove
- `handler.post(RtcEngine::destroy)` — NOT `RtcEngine.destroy()` directly; direct call blocks UI thread (ANR)
- `getPrivateCloudConfig()` null-check before `setLocalAccessPoint()` — returns null on non-private-cloud builds (NPE)
- All `IRtcEngineEventHandler` callbacks run on a **background thread** — always `runOnUIThread()` for UI
- `onActivityCreated` → create engine; `onDestroy` → `leaveChannel()` then `handler.post(RtcEngine::destroy)`
- `ChannelMediaOptions` must NOT set `publishCameraTrack` or `autoSubscribeVideo` — voice-sdk has no video module
- Use `AudioSeatManager` (not `VideoReportLayout`) to visualize remote participants

---

### Step 3: Create the XML layout

Typical audio layout — channel input + join button + audio controls:

```xml
<?xml version="1.0" encoding="utf-8"?>
<RelativeLayout xmlns:android="http://schemas.android.com/apk/res/android"
    android:layout_width="match_parent"
    android:layout_height="match_parent"
    android:fitsSystemWindows="true">

    <!-- audio status / waveform view goes here -->

    <LinearLayout
        android:id="@+id/ll_join"
        android:layout_width="match_parent"
        android:layout_height="wrap_content"
        android:layout_alignParentBottom="true"
        android:gravity="center_vertical"
        android:orientation="horizontal">

        <androidx.appcompat.widget.AppCompatEditText
            android:id="@+id/et_channel"
            android:layout_width="0dp"
            android:layout_height="wrap_content"
            android:layout_weight="1"
            android:digits="@string/chanel_support_char"
            android:hint="@string/channel_id" />

        <androidx.appcompat.widget.AppCompatButton
            android:id="@+id/btn_join"
            android:layout_width="wrap_content"
            android:layout_height="wrap_content"
            android:text="@string/join" />
    </LinearLayout>
</RelativeLayout>
```

For waveform visualization, copy the `WaveformView` pattern from `fragment_join_channel_audio.xml`.

---

### Step 4: Add nav entries

File: `res/navigation/nav_graph.xml`

**Action** — inside `<fragment android:id="@+id/Ready">` (NOT mainFragment — mainFragment only has one action, to Ready):

```xml
<action
    android:id="@+id/action_mainFragment_to_yourCaseName"
    app:destination="@id/yourCaseName" />
```

**Destination** — at root `<navigation>` level:

```xml
<fragment
    android:id="@+id/yourCaseName"
    android:name="io.agora.api.example.examples.advanced.YourCaseName"
    android:label="@string/item_your_case_name"
    tools:layout="@layout/fragment_your_case_name" />
```

`action android:id` must exactly match `actionId` in `@Example`.

---

### Step 5: Update ARCHITECTURE.md

Add one line to the case list in `ARCHITECTURE.md` under the correct directory section (`basic/`, `advanced/`, or `audio/`):

```
├── YourCaseName.java    # [index] "Display Name" — key API description
```

Keep the format consistent with existing entries. This file is the fast-lookup index used by `query-cases` — keeping it current avoids full directory scans.

---

## Modifying an Existing Case

When modifying an existing case rather than creating a new one, identify which files need changes based on what you are updating:

| What changed | Files to touch |
|---|---|
| Implementation logic (API calls, event handling) | `java/.../examples/{basic\|advanced\|audio}/CaseName.java` |
| UI layout (views, controls) | `res/layout/fragment_case_name.xml` |
| Display name or tips text | `res/values/strings.xml` |
| Sort index or group (BASIC ↔ ADVANCED) | `@Example` annotation in the Fragment class |
| Navigation label | `res/navigation/nav_graph.xml` (fragment label attribute) |
| Class rename or package move | Fragment class, `nav_graph.xml` (android:name + destination id), `@Example` annotation (actionId), layout file name, `ARCHITECTURE.md` |

After making changes:

1. **Verify `@Example` annotation consistency** — ensure `index`, `group`, `name`, `actionId`, and `tipsId` still match the actual string resources, nav action ID, and intended group/position. A mismatch causes the case to silently disappear from the list or navigate to the wrong screen.
2. **Update `res/values/strings.xml`** if the display name or tips text changed.
3. **Update `res/navigation/nav_graph.xml`** if the class name, package, or label changed.
4. **Update `ARCHITECTURE.md`** — update the Directory Layout entry and the Case Index table row to reflect any changes to the case name, path, Key APIs, or description.

---

## Verify

```bash
./gradlew assembleDebug
```

- [ ] Case appears in correct group at expected sort position
- [ ] Tap navigates to the case screen (silent failure = nav action in wrong fragment)
- [ ] `onJoinChannelSuccess` fires in Logcat
- [ ] After pressing back, check Logcat for `RtcEngine.destroy` within ~2 seconds — if missing, there is a lifecycle bug in `onDestroy`
- [ ] `ARCHITECTURE.md` Case Index table is updated — row added (new case) or row updated (modified case) with correct Case, Path, Key APIs, and Description
- [ ] `@Example` annotation fields (`index`, `group`, `name`, `actionId`, `tipsId`) are consistent with string resources and nav_graph entries

---

## When to Use a Spec Instead

If the case meets any of the following criteria, create a Spec rather than using this skill directly:

1. Involves coordinated calls across two or more Agora API modules
2. Requires a custom UI layout (not one of the standard templates above)
3. Manages multiple channels or multiple engine instances
4. Requires a foreground Service or background thread coordination
5. Involves developing new shared components (widget/utils, etc.)
6. Requires optional module integration (e.g. streamEncrypt)

If none apply → use this skill directly; no Spec needed.

### Spec Requirements Document Must Include

- List of APIs the case demonstrates (audio APIs only)
- User interaction flow description
- Expected RtcEngine lifecycle behavior
- Required permissions (typically only `RECORD_AUDIO`)

### Spec Design Document Must Include

- Target project identifier: `APIExample-Audio`
- Class/file structure design
- API call sequence (Mermaid sequence diagram recommended)
- State management approach
- UI layout plan
- Integration points with existing shared components
- Case registration info: class name, display name, group (BASIC/ADVANCED), sort index — finalize during design to avoid conflicts
- Generate `@Example` annotation parameters, `nav_graph.xml` action + destination, `strings.xml` key names (`item_` prefix)
- Read `ARCHITECTURE.md` or use the `query-cases` skill to check existing indices
- voice-sdk checks: no video APIs (`enableVideo`, `setupLocalVideo`, `setupRemoteVideo`, `VideoCanvas`, `startScreenCapture`) — violations must be eliminated at design time
- Risk identification and mitigation (API availability, permissions, thread safety, performance)

### Spec Task List Integration

- Mark which sub-tasks can be executed with this `upsert-case` skill, and provide skill input parameters
- Mark which sub-tasks require manual coding, and provide target file paths and change summaries
- New shared component creation tasks must come before case implementation tasks

---

## NEVER

- **NEVER** call any video API (`enableVideo`, `setupLocalVideo`, `VideoCanvas`) — voice-sdk has no video module; crash is immediate.
- **NEVER** put the nav action inside `<fragment id="mainFragment">` — it belongs in `<fragment id="Ready">`. mainFragment only routes to Ready; all case actions live in Ready. Wrong placement causes silent navigation failure at runtime.
- **NEVER** call `RtcEngine.destroy()` directly on the main thread — always `handler.post(RtcEngine::destroy)`. Direct call blocks the UI thread and causes ANR.
- **NEVER** call `setLocalAccessPoint()` without null-checking `getPrivateCloudConfig()` first — it returns null on standard builds, causing NPE.
- **NEVER** update UI directly inside `IRtcEngineEventHandler` callbacks — they run on a background thread. Always wrap with `runOnUIThread()`.
- **NEVER** omit `setParameters(...)` — it's required for Agora backend usage reporting in every case; omitting it causes silent reporting failure even though the app appears to work normally.
