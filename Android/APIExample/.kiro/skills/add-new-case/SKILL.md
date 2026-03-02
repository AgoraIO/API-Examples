---
name: add-new-case
description: >
  Add a new API example case to the APIExample Android demo — creates Fragment class,
  XML layout, string resources, and nav_graph registration. Use when: adding a new
  Agora RTC API demo screen, implementing a new feature example in Java + XML layouts,
  registering a new case via @Example annotation, or subclassing BaseFragment for a
  new demo screen. Keywords: add case, new fragment, nav_graph, @Example, BaseFragment,
  APIExample, new screen, demo case, RTC API example.
---

# Add New Case — APIExample

Touch exactly 4 files (all paths relative to `app/src/main/`):

| File | What to add |
|---|---|
| `java/.../examples/{basic\|advanced}/YourCaseName.java` | Fragment class |
| `res/layout/fragment_your_case_name.xml` | XML layout |
| `res/values/strings.xml` | 2 strings |
| `res/navigation/nav_graph.xml` | 1 action + 1 destination |

Registration is automatic via reflection — no other files needed.

---

## Step 1: Clarify before coding

Before writing a single line, ask:
- **What API am I demonstrating?** — determines which existing case is the closest reference to copy patterns from
- **Video or audio-only?** — determines permissions (`CAMERA` + `RECORD_AUDIO` vs `RECORD_AUDIO` only), layout complexity, and whether `VideoReportLayout` is needed
- **BASIC or ADVANCED group?** — BASIC for fundamental channel join/leave patterns; ADVANCED for feature-specific APIs
- **What's the sort index?** — run `query-cases` skill first; a collision causes silent ordering bugs at runtime

---

## Step 2: Create the Fragment

**MANDATORY — READ ENTIRE FILE before writing any code**:
[`references/fragment-template.java`](references/fragment-template.java)

Do NOT skip — the `setParameters`, `handler.post`, and `getPrivateCloudConfig()` null-check patterns are only fully shown there and are required in every case.

**Do NOT load** any other reference files for this task.

Non-obvious points the template highlights:

- `setParameters(...)` for app scenario reporting — **required in every case**, do not remove
- `handler.post(RtcEngine::destroy)` — NOT `RtcEngine.destroy()` directly; direct call blocks UI thread (ANR)
- `getPrivateCloudConfig()` null-check before `setLocalAccessPoint()` — returns null on non-private-cloud builds (NPE)
- All `IRtcEngineEventHandler` callbacks run on a **background thread** — always `runOnUIThread()` for UI
- `onActivityCreated` → create engine; `onDestroy` → `leaveChannel()` then `handler.post(RtcEngine::destroy)`

For video cases, add `VideoReportLayout` fields and wire `setupRemoteVideo` in `onUserJoined`/`onUserOffline`.

---

## Step 3: Create the XML layout

Minimum structure — channel input + join button at bottom:

```xml
<?xml version="1.0" encoding="utf-8"?>
<RelativeLayout xmlns:android="http://schemas.android.com/apk/res/android"
    android:layout_width="match_parent"
    android:layout_height="match_parent"
    android:fitsSystemWindows="true">

    <!-- video containers go here for video cases -->

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

For video cases, copy the `VideoReportLayout` grid pattern from `fragment_joinchannel_video.xml`.

---

## Step 4: Add nav entries

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

## Step 5: Update ARCHITECTURE.md

Add one line to the case list in `ARCHITECTURE.md` under the correct directory section (`basic/`, `advanced/`, or `audio/`):

```
├── YourCaseName.java    # [index] "Display Name" — key API description
```

Keep the format consistent with existing entries. This file is the fast-lookup index used by `query-cases` — keeping it current avoids full directory scans.

---

## Step 6: Verify

```bash
./gradlew assembleDebug
```

- [ ] Case appears in correct group at expected sort position
- [ ] Tap navigates to the case screen (silent failure = nav action in wrong fragment)
- [ ] `onJoinChannelSuccess` fires in Logcat
- [ ] After pressing back, check Logcat for `RtcEngine.destroy` within ~2 seconds — if missing, there is a lifecycle bug in `onDestroy`

---

## NEVER

- **NEVER** put the nav action inside `<fragment id="mainFragment">` — it belongs in `<fragment id="Ready">`. mainFragment only routes to Ready; all case actions live in Ready. Wrong placement causes silent navigation failure at runtime.
- **NEVER** call `RtcEngine.destroy()` directly on the main thread — always `handler.post(RtcEngine::destroy)`. Direct call blocks the UI thread and causes ANR.
- **NEVER** call `setLocalAccessPoint()` without null-checking `getPrivateCloudConfig()` first — it returns null on standard builds, causing NPE.
- **NEVER** update UI directly inside `IRtcEngineEventHandler` callbacks — they run on a background thread. Always wrap with `runOnUIThread()`.
- **NEVER** omit `setParameters(...)` — it's required for Agora backend usage reporting in every case; omitting it causes silent reporting failure even though the app appears to work normally.
