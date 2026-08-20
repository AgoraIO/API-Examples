---
name: review-case
description: >
  Structured code review for a case in the APIExample-Audio project.
  Checks engine lifecycle, audio-only constraints, thread safety, permissions, and API correctness.
  This project uses AgoraAudio_iOS — video APIs must not appear.
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: iOS
---

# review-case — APIExample-Audio

## Review Dimensions (in priority order)

### 1. Audio-Only Constraint (highest priority for this project)

**Check:**
- No calls to `enableVideo()`, `disableVideo()`, `setupLocalVideo()`, `setupRemoteVideo()`, `startPreview()`, `stopPreview()`
- No `AgoraRtcVideoCanvas` instantiation
- No `VideoView` or video rendering views in storyboard or code
- No camera permission requests

Any video API call in this project is a critical error — the SDK will crash or silently fail.

---

### 2. Engine Lifecycle

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

---

### 3. Thread Safety

All `AgoraRtcEngineDelegate` callbacks may arrive on a background thread.

**Check:**
- Every UI update inside a delegate callback is wrapped in `DispatchQueue.main.async { }`
- No UIKit objects mutated directly in callbacks

---

### 4. Permissions

**Check:**
- Microphone permission requested before `joinChannel()`
- `joinChannel()` called only inside the permission grant callback
- No camera permission requests (audio-only project)

---

### 5. Error Handling

**Check:**
- Return value of `joinChannel()` checked
- `rtcEngine(_:didOccurError:)` implemented and logged
- Token expiry handled if token is used

---

### 6. Code Conventions

**Check:**
- Entry class inherits `UIViewController`, Main class inherits `BaseViewController`
- Class names follow `<ExampleName>Entry` / `<ExampleName>Main` pattern
- `configs` dictionary used to pass data from Entry to Main
- File placed under `Examples/Basic/` or `Examples/Advanced/` matching the MenuItem section
- Storyboard contains only audio controls (labels, sliders, buttons) — no video views

---

### 7. Audio API Usage

**Check:**
- `setAudioProfile(_:)` called before `joinChannel()` if non-default profile needed
- `setAudioScenario(_:)` called before `joinChannel()` if non-default scenario needed
- `enableAudioVolumeIndication(_:smooth:reportVad:)` called if volume callbacks are needed
- Custom audio tracks stopped and released on exit
- External audio sinks disabled on exit if `enableExternalAudioSink` was called

---

### 8. Resource Cleanup

**Check:**
- Audio mixing stopped (`stopAudioMixing()`) if started
- Rhythm player stopped (`stopRhythmPlayer()`) if started
- Echo test stopped (`stopEchoTest()`) if started
- Last-mile probe stopped (`stopLastmileProbeTest()`) if started
- Custom audio tracks destroyed on exit

---

## Review Output Format

```
[SEVERITY] file/line — issue description
Suggestion: how to fix
```

Severity levels:
- `[CRITICAL]` — crash, leak, video API in audio-only project, or incorrect behavior
- `[WARNING]` — convention violation or subtle bug risk
- `[INFO]` — style or minor improvement

---

## Audio-Specific iOS Checks

- `AVAudioSession` category should be `.playAndRecord` with `.defaultToSpeaker` option for most audio cases
- Background audio: verify `UIBackgroundModes` includes `audio` in `Info.plist` if background playback is needed
- In-ear monitoring (`enable(inEarMonitoring:)`) only works with wired headphones — document this limitation in the case if relevant
- `[weak self]` required in all closures capturing `self` to avoid retain cycles
