---
name: review-case
description: >
  Code review for API examples. Ensures examples follow project conventions,
  handle lifecycle correctly, manage threads safely, and use APIs properly.
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: Windows
---

# Review Case — Windows

Review the selected MFC case against its behavior contract, current SDK headers and live
source. Read `APIExample/APIExample/APIExampleDlg.cpp` for scene ownership and
`APIExample/APIExample/Basic/JoinChannelVideoByToken/` for a nearby implementation.
Report findings first; do not silently rewrite the case during review.

## Lifecycle and Ownership

- The host precreates dialogs in `InitSceneDialog()`. `OnInitDialog()` initializes UI only.
  A scene creates its engine in public `InitAgora()`, called from `CreateScene()`.
- `ReleaseScene()` calls `UnInitAgora()` before hiding the dialog. Cleanup must leave the
  channel, stop case-owned media, release the engine and clear the pointer even if joining
  failed or is still pending. Make cleanup idempotent and finish it before another scene
  creates an engine.
- For the current SDK, `release(nullptr)` is synchronous; a non-null callback selects
  asynchronous release. Never destroy from an SDK callback. If using async release, keep
  the event handler alive until completion and serialize the next engine creation.
- Keep the callback receiver stable until synchronous release completes, then detach it
  and discard queued messages belonging to the old engine. Prevent late Token requests
  from joining after a scene has exited.
- A destructor is a fallback. `PostNcDestroy()` is sufficient only for a standalone window
  whose actual exit path destroys it; it cannot replace `UnInitAgora()` for hidden scenes.
  Do not add `delete this` to a precreated dialog without verifying host ownership.

## Threading and Event Messages

- Implement `IRtcEngineEventHandler`; post SDK events to the UI thread using `PostMessage`.
  Never mutate MFC controls or block on a UI-thread response inside SDK callbacks.
- Use event constants from `stdafx.h`, including `EID_JOINCHANNEL_SUCCESS` and `EID_ERROR`.
  Each message-map handler must interpret the payload actually posted by its callback.
  An error posted in wParam is an error code, not an event ID; the message ID selects the
  handler. The SDK error callback signature is `onError(int err, const char* msg)`.
- Copy callback data before asynchronous use; SDK string/struct pointers may expire on
  callback return. Free owned payloads on post failure and when discarding queued events.
- Put `DECLARE_MESSAGE_MAP()` inside the class declaration, and `BEGIN_MESSAGE_MAP` /
  `END_MESSAGE_MAP` at file scope in the .cpp implementation.

## Configuration, Tokens and Errors

- Obtain App ID through `GET_APP_ID`, which wraps
  `cs2utf8(CConfig::GetInstance()->GetAPP_ID())`. Keep the UTF-8 string alive through
  `initialize()`. CConfig does not provide GetAppId or GetToken static methods.
- Use the case's Token input or an existing asynchronous Token helper with the same
  channel/UID as the join. Never embed real credentials in source, examples or logs.
- Verify current join overloads, such as `joinChannel(token, channelId, info, uid)` or
  `joinChannel(token, channelId, uid, options)`, against the SDK in this project.
- Check initialization, configuration and join return codes. Release a partially
  initialized engine on failure. A zero join return value means the request was accepted;
  wait for the success callback before marking the UI joined.
- For expiring tokens, handle the SDK Token callbacks, fetch a replacement for the active
  request and call `renewToken()` only while that scene/engine is still active.
- Log/display asynchronous errors without confusing every SDK error with a join failure.
  Provide a recoverable UI state for denied permissions, invalid input and network failure.

## Devices, Media and Cleanup

- Check the devices needed by the case and handle Windows privacy/permission failures.
  A successful `enableAudio()` or `enableVideo()` call alone does not prove a device exists
  or capture is authorized. Avoid applying mobile permission APIs to an MFC desktop app.
- Configure role/profile, tracks, capture and video canvases for the contract. Do not
  enable camera capture for an audio-only case.
- Stop and release case-owned capture, mixing, media players, custom sources, device
  collections, observers, timers and worker threads before releasing their dependencies.

## Structure and Registration

- Use C++/MFC, the `C<ExampleName>Dlg` naming pattern and `m_` member prefixes; keep files
  under `APIExample/APIExample/Basic/` or `Advanced/` and follow adjacent conventions.
- Verify dialog resources/control IDs, message maps, header includes and host pointers.
- Check all three host paths: `InitSceneDialog()`, `CreateScene()` and `ReleaseScene()`.
  Register the label through `Language.h`, `stdafx.cpp`, `en.ini` and `zh-cn.ini`.
- Confirm new source/header files belong to `APIExample.vcxproj` and `.filters`, and that
  the dialog resource is included in `APIExample.rc`. Update the Case Index when needed;
  use source to verify API names and registration rather than trusting the index alone.

## Verification and Output

Run the project's documented build or the strongest applicable static check for the
change. Record commands and `PASS`, `FAIL`, `BLOCKED` or `NOT_RUN`. A header/API check on
another OS does not establish a successful Windows MFC build or RTC session.

For each actionable finding provide severity, file/line, trigger, impact and a concrete
correction. Include unresolved validation and intentional platform differences. Do not
approve leaked engines, UI mutations from SDK callbacks, stale callback access, invalid
project/resource membership or hardcoded credentials.

## Reference Snippets

These are illustrative fragments to adapt to the selected case and its real ownership:

- `correct-lifecycle`: [snippet](references/correct-lifecycle.cpp)
- `incorrect-lifecycle`: [snippet](references/incorrect-lifecycle.cpp)
- `correct-thread-safety`: [snippet](references/correct-thread-safety.cpp)
- `incorrect-thread-safety`: [snippet](references/incorrect-thread-safety.cpp)
