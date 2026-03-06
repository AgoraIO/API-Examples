---
name: review-case
description: >
  Review an existing case implementation against project-specific red lines
  and coding standards. Use after implementing or modifying a case.
  Use when: reviewing a Compose case for correctness, checking red-line compliance,
  verifying lifecycle and state patterns, auditing an existing Composable.
  Keywords: review, audit, check, red lines, lifecycle, state, compliance, Compose.
---

# Review Case — APIExample-Compose

Run through every item below before considering a case implementation complete.
Open the case's Composable source file and verify each point against the actual code.

## Checklist

### Teardown & Lifecycle

- [ ] **leaveChannel before destroy in onDispose** — `engine.leaveChannel()` is called before `RtcEngine.destroy()` in the `onDispose` block. Destroying without leaving first leaks the channel session on the server side.

- [ ] **DisposableEffect key is lifecycleOwner not Unit** — `DisposableEffect(lifecycleOwner)` not `DisposableEffect(Unit)`. Using `Unit` fires only once and won't clean up on back navigation; the `onDispose` block never re-executes when the lifecycle owner changes.

### State Management

- [ ] **rememberSaveable for channelName/isJoined/uid and remember for RtcEngine** — `channelName`, `isJoined`, `uid` use `rememberSaveable`; `RtcEngine` uses `remember`. `rememberSaveable` survives configuration changes (rotation); `RtcEngine` is not serializable and will crash if placed in `rememberSaveable`.

### Threading

- [ ] **Callbacks dispatch to main thread via coroutineScope.launch(Dispatchers.Main)** — `IRtcEngineEventHandler` callbacks that show `Toast`, `Dialog`, or `AlertDialog` dispatch to the main thread via `coroutineScope.launch(Dispatchers.Main)`. SDK callbacks arrive on a background thread; `Toast` and dialog APIs require the main thread or they throw `CalledFromWrongThreadException`. Note: simple Compose state mutations (e.g. `isJoined = true`) are thread-safe via the snapshot system and do **not** need main-thread dispatch.

### Permissions

- [ ] **Permission check before joinChannel** — Permission launcher (`rememberLauncherForActivityResult`) is called before `joinChannel()`. Joining without the required permissions (`RECORD_AUDIO`, and `CAMERA` for video cases) causes a silent failure — no error callback, just no audio/video.

## If a Check Fails

- `DisposableEffect(Unit)` is used — change key to `lifecycleOwner`, then verify back navigation triggers cleanup.
- `RtcEngine` stored in `rememberSaveable` or state fields in `remember` only — fix to `RtcEngine -> remember`, UI/session state -> `rememberSaveable`, then verify rotation.
- Toast/Dialog shown directly in callback — move UI-thread-only calls into `coroutineScope.launch(Dispatchers.Main)`.
- Permission launcher bypassed before `joinChannel()` — gate join flow behind permission callback and re-test denied/granted paths.

## NEVER

- **NEVER** approve a review when `DisposableEffect` key is `Unit` for case teardown logic.
- **NEVER** approve a review when `RtcEngine` uses `rememberSaveable`.
- **NEVER** treat Compose callback state safety as permission to call Toast/Dialog off main thread.
- **NEVER** skip rotation and back-navigation checks for lifecycle-sensitive Compose cases.
