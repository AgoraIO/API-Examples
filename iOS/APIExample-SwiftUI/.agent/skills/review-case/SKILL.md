---
name: review-case
description: >
  Structured code review for a case in the APIExample-SwiftUI project.
  Checks engine lifecycle, SwiftUI state ownership, thread safety, permissions, and API correctness.
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: iOS
---

# review-case — APIExample-SwiftUI

## Review Dimensions (in priority order)

### 1. Engine Lifecycle

**Check:**
- `AgoraRtcEngineKit` created inside `setupRTC()` of the RTC class, not in the Entry view
- `leaveChannel()` + `AgoraRtcEngineKit.destroy()` called inside `onDestroy()`
- `setupRTC()` called from `.onAppear`, `onDestroy()` called from `.onDisappear`
- No engine instance retained beyond the RTC object's lifetime

**Correct:**
```swift
.onAppear { rtc.setupRTC(configs: configs) }
.onDisappear { rtc.onDestroy() }
```

**Wrong:**
```swift
// Missing onDisappear — engine never destroyed
.onAppear { rtc.setupRTC(configs: configs) }
```

---

### 2. SwiftUI State Ownership

**Check:**
- Main view declares RTC object as `@ObservedObject`, not `@StateObject`
- Entry view does not hold a reference to the RTC object
- `@Published` properties used for state that drives UI updates

**Correct:**
```swift
struct MyCase: View {
    @ObservedObject private var rtc = MyCaseRTC()  // correct
}
```

**Wrong:**
```swift
struct MyCase: View {
    @StateObject private var rtc = MyCaseRTC()  // wrong — SwiftUI owns lifetime, may outlive view
}
```

---

### 3. Thread Safety

All `AgoraRtcEngineDelegate` callbacks may arrive on a background thread.

**Check:**
- Every `@Published` property mutation inside a delegate callback is dispatched to `DispatchQueue.main`
- No direct SwiftUI state mutation on background thread

**Correct:**
```swift
func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
    DispatchQueue.main.async {
        self.remoteUid = uid
    }
}
```

---

### 4. Permissions

**Check:**
- Camera/microphone permissions requested before `joinChannel()`
- `joinChannel()` called only inside the permission grant callback

---

### 5. Error Handling

**Check:**
- Return value of `joinChannel()` checked
- `rtcEngine(_:didOccurError:)` implemented and logged
- Token expiry handled if token is used

---

### 6. Code Conventions

**Check:**
- RTC class inherits `NSObject`, conforms to `ObservableObject` and `AgoraRtcEngineDelegate`
- Entry view named `<ExampleName>Entry`, main view named `<ExampleName>`
- RTC class named `<ExampleName>RTC`
- `configs` dictionary used to pass data from Entry to Main
- No SDK calls inside SwiftUI `body` computed property

---

### 7. Resource Cleanup

**Check:**
- Audio files / custom audio tracks stopped in `onDestroy()`
- External video sources unregistered in `onDestroy()`
- Media player destroyed if created
- Screen capture stopped if started
- Multi-camera capture stopped if started

---

## Review Output Format

```
[SEVERITY] file/line — issue description
Suggestion: how to fix
```

Severity levels:
- `[CRITICAL]` — crash, leak, or incorrect behavior
- `[WARNING]` — convention violation or subtle bug risk
- `[INFO]` — style or minor improvement

---

## SwiftUI-Specific Checks

- `onAppear` can fire multiple times (e.g., sheet dismiss, navigation pop/push) — verify `setupRTC` is idempotent or guarded
- `onDisappear` fires when the view is covered by another view in a `TabView` — verify this does not prematurely destroy the engine
- Video rendering views (`VideoView` / `VideoUIView`) must be created before `setupRTC` is called so the canvas can be set up correctly
- `[weak self]` required in all closures capturing `self` inside the RTC class to avoid retain cycles
