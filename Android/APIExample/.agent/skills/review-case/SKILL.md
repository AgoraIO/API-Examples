---
name: review-case
description: >
  Review an existing case implementation against project-specific red lines
  and coding standards. Use after implementing or modifying a case.
  Use when: reviewing a case for correctness, checking red-line compliance,
  verifying lifecycle and threading patterns, auditing an existing Fragment.
  Keywords: review, audit, check, red lines, lifecycle, threading, compliance.
---

# Review Case — APIExample

Run through every item below before considering a case implementation complete.
Open the case's Fragment source file and verify each point against the actual code.

## Checklist

### Teardown & Lifecycle

- [ ] **leaveChannel before destroy** — `engine.leaveChannel()` is called before `RtcEngine.destroy()` in the teardown path (typically `onDestroy()`). Destroying without leaving first leaks the channel session on the server side.

- [ ] **handler.post for destroy** — `RtcEngine.destroy()` is invoked via `handler.post(RtcEngine::destroy)` and **not** called directly on the main thread. A direct call blocks the UI thread and causes ANR.

### Threading

- [ ] **runOnUIThread for callbacks** — All `IRtcEngineEventHandler` callbacks that update UI are wrapped with `runOnUIThread()`. SDK callbacks arrive on a background thread; touching Views without dispatching to the main thread causes crashes or silent rendering corruption.

### Permissions

- [ ] **Permission check before join** — `checkOrRequestPermission()` is called before `joinChannel()`. Joining without the required permissions (RECORD_AUDIO, and CAMERA for video cases) causes a silent failure — no error callback, just no audio/video.

### Backend Reporting

- [ ] **setParameters present** — `setParameters(...)` is called during engine initialisation. This is required for Agora backend usage reporting in every case; omitting it causes silent reporting failure even though the app appears to work normally.

### Private Cloud

- [ ] **getPrivateCloudConfig null-check** — `getPrivateCloudConfig()` is null-checked before `setLocalAccessPoint()` is called. The method returns `null` on standard (non-private-cloud) builds, so calling `setLocalAccessPoint()` without the guard causes a NullPointerException.

## If a Check Fails

- Teardown order wrong (`destroy` before `leaveChannel`) — fix teardown to `leaveChannel()` first, then `handler.post(RtcEngine::destroy)`, and re-test back navigation.
- UI touched in SDK callback without main-thread dispatch — wrap UI updates in `runOnUIThread()` and re-run the case to verify no thread exceptions.
- Permission flow missing before `joinChannel()` — add `checkOrRequestPermission()` gate and verify join succeeds only after permission is granted.
- Missing `setParameters(...)` or private-cloud null-check — add both safeguards in engine init and re-run the init path once.

## NEVER

- **NEVER** approve a case review with direct `RtcEngine.destroy()` on main thread.
- **NEVER** approve a case review when `leaveChannel()` is missing before destroy.
- **NEVER** ignore background-thread UI updates inside `IRtcEngineEventHandler` callbacks.
- **NEVER** assume runtime behavior is correct without at least one back-navigation teardown check in Logcat.
