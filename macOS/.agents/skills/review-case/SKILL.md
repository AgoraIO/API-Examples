---
name: review-case
description: >
  Code review for API examples. Ensures examples follow project conventions,
  handle lifecycle correctly, manage threads safely, and use APIs properly.
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: macOS
---

# Review Case Skill — macOS

## When to Use

Use this skill when you need to:
- Review a new or modified example for correctness
- Ensure the example follows project conventions
- Verify lifecycle management and thread safety
- Check API usage and error handling

## Review Dimensions (Priority Order)

### 1. Engine Lifecycle (CRITICAL)

**Check:**
- [ ] Engine is created in `initializeAgoraEngine()` or similar
- [ ] Engine is initialized with `AgoraRtcEngineConfig`
- [ ] `leaveChannel()` is called before `destroy()`
- [ ] `destroy()` is called in `viewWillClose()` or cleanup method
- [ ] No engine leaks (engine not recreated on every join)

**Correct Pattern:**
```swift
override func viewDidLoad() {
    super.viewDidLoad()
    initializeAgoraEngine()  // Create once
}

override func viewWillClose() {
    leaveChannel()
    agoraKit.destroy()
    super.viewWillClose()
}

func joinChannel() {
    agoraKit.joinChannel(byToken: token, channelName: channel, info: nil, uid: 0)
}

func leaveChannel() {
    agoraKit.leaveChannel(nil)
}
```

**Incorrect Pattern:**
See `references/incorrect-lifecycle.swift` for common mistakes.

---

### 2. Thread Safety (CRITICAL)

**Check:**
- [ ] All UI updates in delegate callbacks use `DispatchQueue.main.async`
- [ ] No direct UI updates from background threads
- [ ] Video/audio frame callbacks dispatch to main thread before updating UI

**Correct Pattern:**
```swift
func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
    // Callback may arrive on background thread
    DispatchQueue.main.async {
        self.statusLabel.stringValue = "Joined channel"
    }
}
```

**Incorrect Pattern:**
See `references/incorrect-thread-safety.swift` for common mistakes.

---

### 3. Permission Handling (HIGH)

**Check:**
- [ ] Microphone permission requested before `enableAudio()`
- [ ] Camera permission requested before `enableVideo()`
- [ ] Permissions checked before accessing devices
- [ ] Review guidance stays macOS-specific and does not suggest iOS-only APIs such as `AVAudioSession.sharedInstance().requestRecordPermission`

**Correct Pattern:**
```swift
func initializeAgoraEngine() {
    // Request permissions first
    AVCaptureDevice.requestAccess(for: .video) { granted in
        if granted {
            self.agoraKit.enableVideo()
        }
    }
    
    AVCaptureDevice.requestAccess(for: .audio) { granted in
        if granted {
            self.agoraKit.enableAudio()
        }
    }
}
```

---

### 4. Error Handling (HIGH)

**Check:**
- [ ] `joinChannel()` failures are handled
- [ ] Token expiration is handled
- [ ] Network errors are logged or displayed
- [ ] Invalid parameters are validated

**Correct Pattern:**
```swift
func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
    DispatchQueue.main.async {
        self.showError("Error: \(errorCode.rawValue)")
    }
}

func rtcEngine(_ engine: AgoraRtcEngineKit, tokenPrivilegeWillExpire token: String) {
    // Refresh token before expiration
    let newToken = KeyCenter.Token(channelName: self.channelName)
    self.agoraKit.renewToken(newToken)
}
```

---

### 5. Code Convention (MEDIUM)

**Check:**
- [ ] Class name follows pattern: `<ExampleName>Main`
- [ ] Extends `BaseViewController`
- [ ] File name matches class name (PascalCase)
- [ ] Properties are properly declared with `@IBOutlet` or `var`
- [ ] Methods are organized with `// MARK:` sections
- [ ] Comments explain non-obvious logic

**Correct Pattern:**
```swift
class ScreenShareMain: BaseViewController {
    
    var agoraKit: AgoraRtcEngineKit!
    var remoteUid: UInt = 0
    
    @IBOutlet weak var Container: AGEVideoContainer!
    
    // MARK: - Lifecycle
    override func viewDidLoad() { ... }
    
    // MARK: - Agora Engine Setup
    func initializeAgoraEngine() { ... }
    
    // MARK: - Actions
    @IBAction func joinButtonTapped(_ sender: Any) { ... }
}
```

---

### 6. API Usage Correctness (MEDIUM)

**Check:**
- [ ] SDK methods called in correct order
- [ ] Required parameters are provided
- [ ] Optional parameters are used correctly
- [ ] Return values are checked where necessary
- [ ] Deprecated APIs are not used

**Correct Pattern:**
```swift
// Correct order: enable -> setup -> join
agoraKit.enableVideo()
agoraKit.setupLocalVideo(AgoraRtcVideoCanvas(uid: 0))
agoraKit.joinChannel(byToken: token, channelName: channel, info: nil, uid: 0)
```

**Incorrect Pattern:**
```swift
// ❌ Wrong order
agoraKit.joinChannel(...)  // Join first
agoraKit.enableVideo()     // Enable after join (too late)
```

---

### 7. Resource Cleanup (MEDIUM)

**Check:**
- [ ] Audio files are stopped and released
- [ ] Video captures are stopped
- [ ] Custom audio/video sources are cleaned up
- [ ] Observers are unregistered
- [ ] Timers are invalidated

**Correct Pattern:**
```swift
func leaveChannel() {
    agoraKit.stopAudioMixing()  // Stop audio
    agoraKit.stopScreenCapture()  // Stop screen share
    agoraKit.leaveChannel(nil)
}

override func viewWillClose() {
    leaveChannel()
    agoraKit.destroy()
    super.viewWillClose()
}
```

---

## Review Output Format

When reviewing, provide feedback in this format:

```
## Review Results

### ✅ Passed
- Engine lifecycle correctly managed
- Thread safety ensured with DispatchQueue.main.async
- Permissions requested before device access

### ⚠️ Issues Found

**[HIGH] Thread Safety Issue**
- File: `ScreenShare.swift`
- Line: 45
- Issue: UI update in delegate callback without DispatchQueue.main.async
- Suggestion: Wrap UI update with `DispatchQueue.main.async { ... }`

**[MEDIUM] Missing Error Handling**
- File: `ScreenShare.swift`
- Line: 78
- Issue: joinChannel() result not checked
- Suggestion: Implement `rtcEngine(_:didOccurError:)` delegate method

### 🔧 Recommendations
- Add logging for debugging
- Consider adding retry logic for network failures
```

---

## Platform-Specific Checks

### macOS-Specific

**Check:**
- [ ] Using Cocoa (AppKit) — not UIKit or SwiftUI
- [ ] Window/view lifecycle properly handled
- [ ] No Combine or async/await unless already in codebase
- [ ] Storyboard/XIB files properly configured if used

**Correct Pattern:**
```swift
// macOS: Use Cocoa
import Cocoa
import AgoraRtcKit

class ExampleMain: BaseViewController {
    @IBOutlet weak var Container: AGEVideoContainer!
    // Cocoa-based UI
}
```

**Incorrect Pattern:**
```swift
// ❌ iOS patterns in macOS
import UIKit  // Wrong framework
class ExampleMain: UIViewController { }  // Wrong base class
```

---

## NEVER List

**Do NOT accept:**
- Engine not destroyed (memory leak)
- UI updates from background threads without DispatchQueue.main.async
- Multiple engine instances in one example
- Hardcoded App ID or token (must use KeyCenter)
- Missing `leaveChannel()` before `destroy()`
- Objective-C files (Swift only)
- UIKit or SwiftUI (Cocoa only)
- Examples outside `APIExample/Examples/[Basic|Advanced]/` structure
- Missing delegate implementation for event handling
- No error handling for joinChannel failures

---

## Review Checklist

Use this checklist when reviewing an example:

**Lifecycle:**
- [ ] Engine created once in initialization
- [ ] `leaveChannel()` called before `destroy()`
- [ ] `destroy()` called in cleanup
- [ ] No engine leaks

**Thread Safety:**
- [ ] All UI updates use `DispatchQueue.main.async`
- [ ] No direct UI updates from callbacks
- [ ] Frame callbacks dispatch to main thread

**Permissions:**
- [ ] Microphone permission requested
- [ ] Camera permission requested
- [ ] Permissions checked before use

**Error Handling:**
- [ ] joinChannel failures handled
- [ ] Token expiration handled
- [ ] Network errors logged

**Code Quality:**
- [ ] Follows naming conventions
- [ ] Properly organized with MARK sections
- [ ] Comments explain non-obvious logic
- [ ] No hardcoded credentials

**API Usage:**
- [ ] Methods called in correct order
- [ ] Required parameters provided
- [ ] Return values checked
- [ ] No deprecated APIs

**Resources:**
- [ ] Audio/video properly stopped
- [ ] Observers unregistered
- [ ] Timers invalidated
- [ ] No resource leaks

**Platform:**
- [ ] Using Cocoa (AppKit)
- [ ] No UIKit or SwiftUI
- [ ] Window lifecycle handled
- [ ] No modern C++ patterns unless existing

---

## Common Issues and Fixes

### Issue: "Engine not initialized"
**Cause:** `destroy()` called without `leaveChannel()` first
**Fix:** Always call `leaveChannel()` before `destroy()`

### Issue: "UI updates crash the app"
**Cause:** Direct UI update from background thread
**Fix:** Wrap with `DispatchQueue.main.async { ... }`

### Issue: "Memory leak detected"
**Cause:** `destroy()` not called or engine recreated
**Fix:** Ensure `destroy()` in `viewWillClose()` and create engine once

### Issue: "Token expired error"
**Cause:** No token refresh handling
**Fix:** Implement `tokenPrivilegeWillExpire()` delegate method

### Issue: "No audio/video"
**Cause:** Permissions not requested
**Fix:** Request permissions before `enableAudio()` / `enableVideo()`

---

## References

- **Agora RTC SDK for macOS:** [Documentation](https://docs.agora.io/en/video-calling/reference/macos-sdk)
- **Existing examples:** Review `APIExample/Examples/Basic/JoinChannelVideo/` for reference
- **BaseViewController:** Check `APIExample/Common/` for base class implementation
