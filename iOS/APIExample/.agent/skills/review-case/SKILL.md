---
name: review-case
description: >
  Structured code review for a case in the APIExample (UIKit + Swift) project.
  Checks engine lifecycle, thread safety, permissions, error handling, API correctness, and code conventions.
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: iOS
---

# review-case — APIExample

## Review Dimensions (in priority order)

### 1. Engine Lifecycle

The most critical dimension. Leaks here cause crashes in subsequent examples.

**Check:**
- `AgoraRtcEngineKit.sharedEngine(with:delegate:)` called in `viewDidLoad` (not in Entry VC)
- `leaveChannel()` + `AgoraRtcEngineKit.destroy()` called in `willMove(toParent:)` when `parent == nil`
- No engine instance stored beyond the Main VC's lifetime

**Correct:**
```swift
override func willMove(toParent parent: UIViewController?) {
    super.willMove(toParent: parent)
    if parent == nil {
        agoraKit?.leaveChannel()
        AgoraRtcEngineKit.destroy()
    }
}
```

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

**Correct:**
```swift
AgoraAudioSession.sharedInstance().requestRecordPermission { [weak self] granted in
    guard granted else { return }
    self?.agoraKit?.joinChannel(...)
}
```

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
- Class names follow `<ExampleName>Entry` / `<ExampleName>Main` pattern
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
