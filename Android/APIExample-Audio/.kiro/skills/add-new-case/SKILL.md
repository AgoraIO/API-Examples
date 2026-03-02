---
name: add-new-case
description: >
  Add a new audio API example case to the APIExample-Audio Android demo — creates
  Fragment class, XML layout, string resources, and nav_graph registration.
  Use when: adding a new Agora audio API demo screen, implementing a new audio
  feature example in Java + XML layouts, registering a new case via @Example
  annotation, or subclassing BaseFragment for a new audio demo screen.
  This project uses voice-sdk — no video APIs available.
  Keywords: add case, new fragment, nav_graph, @Example, BaseFragment, APIExample-Audio,
  audio case, voice-sdk, new screen, audio demo.
---

# Add New Case — APIExample-Audio

Touch exactly 4 files (all paths relative to `app/src/main/`):

| File | What to add |
|---|---|
| `java/.../examples/{basic\|advanced}/YourCaseName.java` | Fragment class |
| `res/layout/fragment_your_case_name.xml` | XML layout |
| `res/values/strings.xml` | 2 strings |
| `res/navigation/nav_graph.xml` | 1 action + 1 destination |

Registration is automatic via reflection — no other files needed.

**voice-sdk constraint**: Do NOT call `enableVideo()`, `setupLocalVideo()`, `VideoCanvas`, or any video API — the module does not exist and will crash at runtime.

---

## Step 1: Clarify before coding

Before writing a single line, ask:
- **What audio API am I demonstrating?** — determines which existing case is the closest reference to copy patterns from
- **BASIC or ADVANCED group?** — BASIC for fundamental join/leave audio patterns; ADVANCED for feature-specific audio APIs
- **What's the sort index?** — run `query-cases` skill first; a collision causes silent ordering bugs at runtime
- **Any special permissions beyond `RECORD_AUDIO`?** — most audio cases only need `RECORD_AUDIO`; check if the API requires anything else

---

## Step 2: Create the Fragment

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

## Step 3: Create the XML layout

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

## Step 4: Add nav entries

File: `res/navigation/nav_graph.xml`

**Action** — inside `<fragment android:id="@+id/mainFragment">`:

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

## Step 5: Verify

```bash
./gradlew assembleDebug
```

- [ ] Case appears in correct group at expected sort position
- [ ] Tap navigates to the case screen (silent failure = nav action in wrong fragment)
- [ ] `onJoinChannelSuccess` fires in Logcat
- [ ] After pressing back, check Logcat for `RtcEngine.destroy` within ~2 seconds — if missing, there is a lifecycle bug in `onDestroy`

---

## NEVER

- **NEVER** call any video API (`enableVideo`, `setupLocalVideo`, `VideoCanvas`) — voice-sdk has no video module; crash is immediate.
- **NEVER** call `RtcEngine.destroy()` directly on the main thread — always `handler.post(RtcEngine::destroy)`. Direct call blocks the UI thread and causes ANR.
- **NEVER** call `setLocalAccessPoint()` without null-checking `getPrivateCloudConfig()` first — it returns null on standard builds, causing NPE.
- **NEVER** update UI directly inside `IRtcEngineEventHandler` callbacks — they run on a background thread. Always wrap with `runOnUIThread()`.
- **NEVER** omit `setParameters(...)` — it's required for Agora backend usage reporting in every case; omitting it causes silent reporting failure even though the app appears to work normally.
