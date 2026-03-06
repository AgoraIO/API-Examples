---
name: review-case
description: >
  Review an existing case implementation against project-specific red lines
  and coding standards. Use after implementing or modifying a case.
  Use when: reviewing a case for correctness, checking red-line compliance,
  verifying lifecycle and threading patterns, auditing an existing Fragment.
  Keywords: review, audit, check, red lines, lifecycle, threading, compliance.
---

# Review Case — APIExample-Audio

Run through every item below before considering a case implementation complete.
Open the case's Fragment source file and verify each point against the actual code.

## Checklist

### Teardown & Lifecycle

- [ ] **leaveChannel before destroy** — `engine.leaveChannel()` is called before `RtcEngine.destroy()` in the teardown path (typically `onDestroy()`). Destroying without leaving first leaks the channel session on the server side.

- [ ] **handler.post for destroy** — `RtcEngine.destroy()` is invoked via `handler.post(RtcEngine::destroy)` and **not** called directly on the main thread. A direct call blocks the UI thread and causes ANR.

### Threading

- [ ] **runOnUIThread for callbacks** — All `IRtcEngineEventHandler` callbacks that update UI are wrapped with `runOnUIThread()`. SDK callbacks arrive on a background thread; touching Views without dispatching to the main thread causes crashes or silent rendering corruption.

### Permissions

- [ ] **Permission check before join** — `checkOrRequestPermission()` is called before `joinChannel()`. Joining without the required permissions (RECORD_AUDIO) causes a silent failure — no error callback, just no audio.

### Backend Reporting

- [ ] **setParameters present** — `setParameters(...)` is called during engine initialisation. This is required for Agora backend usage reporting in every case; omitting it causes silent reporting failure even though the app appears to work normally.

### Private Cloud

- [ ] **getPrivateCloudConfig null-check** — `getPrivateCloudConfig()` is null-checked before `setLocalAccessPoint()` is called. The method returns `null` on standard (non-private-cloud) builds, so calling `setLocalAccessPoint()` without the guard causes a NullPointerException.

### Audio-Only Constraint

- [ ] **No video APIs** — The case must not call `enableVideo()`, `setupLocalVideo()`, or reference `VideoCanvas`. APIExample-Audio uses the voice-SDK which has no video module; calling video APIs causes a compile error or runtime crash.
