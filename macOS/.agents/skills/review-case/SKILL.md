---
name: review-case
description: >
  Code review for API examples. Ensures examples follow project conventions,
  handle lifecycle correctly, manage threads safely, and use APIs properly.
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: macOS
---

# Review Case — macOS

Review the selected AppKit case against its behavior contract, current SDK headers, and
live source. Use `APIExample/Examples/Basic/JoinChannelVideo/JoinChannelVideo.swift`,
`APIExample/Common/NetworkManager/NetworkManager.swift`, and the project `AGENTS.md` as
references. Report findings first; do not silently rewrite the case during review.

## Lifecycle and Ownership

- Create the case engine once with `AgoraRtcEngineKit.sharedEngine(with:delegate:)` and an
  `AgoraRtcEngineConfig` containing `KeyCenter.AppId`.
- The main controller inherits `BaseViewController`; the menu host calls
  `viewWillBeRemovedFromSplitView()` when replacing its content.
- On removal, invalidate pending Token/join work, leave the channel, stop case-owned media,
  call `AgoraRtcEngineKit.destroy()`, and clear the engine reference. Repeated cleanup must
  be safe. A late asynchronous response must not use the destroyed engine.
- Complete destruction before another case creates an engine. Never destroy from an SDK
  delegate callback.

```swift
override func viewWillBeRemovedFromSplitView() {
    // Invalidate outstanding requests using the case's request ID or cancellation state.
    agoraKit?.leaveChannel(nil)
    if agoraKit != nil {
        AgoraRtcEngineKit.destroy()
        agoraKit = nil
    }
    super.viewWillBeRemovedFromSplitView()
}
```

## Threading and Permissions

- Dispatch AppKit mutations to the main queue; SDK callbacks may arrive on another thread.
  Avoid routing expensive frame processing through the UI queue merely to update a label.
- Request camera/microphone access with `AVCaptureDevice.requestAccess(for:)` before using
  the corresponding device; handle denial without starting capture or joining anyway.
- Do not suggest iOS-only permission APIs such as AVAudioSession on macOS.
- Check object/request validity again inside asynchronous permission and Token callbacks.

## Token and Error Handling

`KeyCenter` contains AppId and Certificate; it has no Token method. Use the existing
`NetworkManager.shared.generateToken(channelName:uid:success:)` flow or the case's explicit
Token input. Store the active channel and UID in the case, and use the same values when
refreshing. The helper can return nil; do not treat a failed Token request as successful
authentication when the project requires a certificate.

- Check join/configuration return codes and implement `rtcEngine(_:didOccurError:)`.
- Handle `rtcEngine(_:tokenPrivilegeWillExpire:)` when using expiring tokens. After fetching
  a replacement, validate that the case/request is still active before calling
  `renewToken(_:)`; do not rejoin the channel solely to renew a token.
- Keep logs and errors useful without exposing App Certificates or tokens.

## API and Media Correctness

- Compare exact method names, argument labels, enums, defaults and return values with the
  selected SDK. The channel join overload used by the current basic case is
  `joinChannel(byToken:channelId:uid:mediaOptions:)`.
- Configure channel profile, client role, capture, canvases and encoder settings as needed
  by the contract before joining. Audio-only behavior must not enable video capture.
- Construct a canvas with `AgoraRtcVideoCanvas()`, then assign uid/view/renderMode. Do not
  invent convenience initializers.
- On exit stop any audio mixing, preview, screen/camera capture, custom media, player, timer
  or observer owned by this case before destroying the engine.

## Structure and Registration

- Keep the implementation under `APIExample/Examples/Basic/` or `Advanced/`, using Swift
  and AppKit. Follow existing threading and language conventions in the edited code.
- Verify `APIExample/ViewController.swift` supplies the right MenuItem, storyboard name
  and controller identifier; confirm the storyboard actually loads that controller.
- New source files must belong to the APIExample Xcode target's Sources build phase;
  storyboards, localized files and media assets must belong to Resources. Existing-file
  edits need no project-file change unless a build input was added or moved.
- Update the Case Index when paths, behavior or key APIs change. Check source before relying
  on an index entry to establish implementation or registration completeness.

## Verification and Output

Run the project's documented build or the strongest applicable static check for the change.
Use `PASS`, `FAIL`, `BLOCKED` or `NOT_RUN`, recording the command and evidence. Compilation
alone does not establish device permissions, RTC connectivity or lifecycle behavior.

For each actionable finding provide severity, file/line, trigger, impact and a concrete
correction. Include unresolved validation and intentional platform differences. Do not
approve leaked engines, UI mutations off the main thread, missing cleanup, invalid target
membership or hardcoded credentials.

## Reference Snippets

These are illustrative fragments to adapt to the selected case and its real ownership:

- `correct-lifecycle`: [snippet](references/correct-lifecycle.swift)
- `incorrect-lifecycle`: [snippet](references/incorrect-lifecycle.swift)
- `correct-thread-safety`: [snippet](references/correct-thread-safety.swift)
- `incorrect-thread-safety`: [snippet](references/incorrect-thread-safety.swift)
