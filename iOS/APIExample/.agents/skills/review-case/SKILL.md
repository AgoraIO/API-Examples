---
name: review-case
description: >
  Structured code review for a case in the APIExample (UIKit + Swift) project.
  Checks engine lifecycle, thread safety, permissions, error handling, API correctness, and code conventions.
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: iOS
---

# review-case — APIExample

## Pending Permission and Token Requests

Review the complete lifecycle against [the creation template](../upsert-case/SKILL.md).
All request/engine ownership transitions must run on main. Freeze channel/UID and request
generation before requesting permission or Token; before joining, check that the generation
and engine identity are still current. Leave invalidates pending work even before joined;
destroy also clears the engine. Repeated cleanup must not destroy another case's engine.
Weak references alone do not invalidate a request when its owner remains alive.

Exercise delayed permission and Token responses after leave/destroy, repeated cleanup,
rapid reopen and out-of-order responses. Only the current request may join. Also check
permission denial, absent required Token and a nonzero join result. Never log credentials.

## Review Dimensions (in priority order)

### 1. Engine Lifecycle

The most critical dimension. Leaks here cause crashes in subsequent examples.

**Check:**
- `AgoraRtcEngineKit.sharedEngine(with:delegate:)` called in `viewDidLoad` (not in Entry VC)
- `leaveChannel()` + `AgoraRtcEngineKit.destroy()` called in `willMove(toParent:)` when `parent == nil`
- No engine instance stored beyond the Main VC's lifetime

**Correct:** Call the creation template’s idempotent `onDestroy()` from
`willMove(toParent:)` when `parent == nil`. It invalidates pending permission/Token work
before leaving, destroying and clearing the owned engine.

**Wrong:**
```swift
// Missing destroy — engine leaks
override func viewDidDisappear(_ animated: Bool) {
    agoraKit?.leaveChannel()
}
```

---

### 2. Thread Safety

All `AgoraRtcEngineDelegate` callbacks may arrive on a background thread.

**Check:**
- Every UI update inside a delegate callback is wrapped in `DispatchQueue.main.async { }`
- No `UIView`, `UILabel`, or other UIKit objects mutated directly in callbacks

**Correct:**
```swift
func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
    DispatchQueue.main.async {
        self.remoteView.isHidden = false
        self.setupRemoteVideo(uid: uid)
    }
}
```

**Wrong:**
```swift
func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
    remoteView.isHidden = false  // UI update on background thread
}
```

---

### 3. Permissions

**Check:**
- Camera permission requested before `joinChannel()` for video cases
- Microphone permission requested before `joinChannel()` for all cases
- `joinChannel()` called only inside the permission grant callback, not before

**Correct:** Pass the case’s actual camera/microphone permission request to
`requestJoin(channelName:requestPermission:)` in the creation template. Its guarded main-queue
continuation checks the request and engine lifetime before requesting a Token or joining.

---

### 4. Error Handling

**Check:**
- Return value of `joinChannel()` checked (non-zero = error)
- `rtcEngine(_:didOccurError:)` delegate method implemented and logged
- Token expiry handled via `rtcEngine(_:tokenPrivilegeWillExpire:)` if token is used

---

### 5. Code Conventions

**Check:**
- Entry class inherits `UIViewController`, Main class inherits `BaseViewController`
- Entry class usually follows `<ExampleName>Entry`; the main controller may be `<ExampleName>Main` or an existing project-specific `*ViewController` name as long as storyboard wiring is correct
- `configs` dictionary used to pass data from Entry to Main (no direct property injection)
- File placed under `Examples/Basic/` or `Examples/Advanced/` matching the MenuItem section
- Storyboard ID of Main scene matches the `controller` field in `MenuItem`

---

### 6. API Usage Correctness

**Check:**
- `setVideoEncoderConfiguration` called before `joinChannel`, not after
- `setupLocalVideo` called before `startPreview` and `joinChannel`
- `enableVideo()` called before `setupLocalVideo` for video cases
- `setClientRole` called before `joinChannel` for live streaming cases
- No deprecated API variants used (check SDK release notes if unsure)

---

### 7. Resource Cleanup

**Check:**
- Audio files / custom audio tracks stopped and released on exit
- External video sources unregistered (`setExternalVideoSource(false, ...)`)
- Media player destroyed if created (`agoraKit.destroy(mediaPlayer)`)
- Screen capture stopped if started (`stopScreenCapture()`)
- Multi-camera capture stopped if started (`stopCameraCapture(.cameraSecondary)`)

---

### 8. Xcode Target Membership

For a new or moved case, verify `APIExample.xcodeproj/project.pbxproj` explicitly adds every
Swift file to the `APIExample` target's Sources build phase and every storyboard, localized
file, or media asset to its Resources build phase. A file existing in the folder is not
enough. Do not require a project-file edit for an existing build input that was only modified.

---

## Review Output Format

For each issue found, report:

```
[SEVERITY] file/line — issue description
Suggestion: how to fix
```

Severity levels:
- `[CRITICAL]` — will cause crash, leak, or incorrect behavior
- `[WARNING]` — violates convention or may cause subtle bugs
- `[INFO]` — style or minor improvement suggestion

---

## iOS-Specific Checks

- Background audio: if the case uses audio, verify `AVAudioSession` category is set appropriately and `UIBackgroundModes` includes `audio` if background playback is needed
- `willMove(toParent:)` is the correct hook — do NOT use `viewWillDisappear` or `deinit` for engine cleanup in navigation-based flows
- `[weak self]` must be used in all closures that capture `self` to avoid retain cycles with the engine delegate
