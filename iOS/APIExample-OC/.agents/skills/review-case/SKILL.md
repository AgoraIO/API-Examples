---
name: review-case
description: >
  Structured code review for a case in the APIExample-OC (Objective-C + UIKit) project.
  Checks engine lifecycle, thread safety, memory management, permissions, and OC conventions.
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: iOS
---

# review-case — APIExample-OC

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

**Check:**
- `[AgoraRtcEngineKit sharedEngineWithConfig:delegate:]` called in `viewDidLoad` (not in Entry VC)
- `[self.agoraKit leaveChannel:]` + `[AgoraRtcEngineKit destroy]` called when leaving
- Navigation cleanup invalidates requests in `willMoveToParentViewController:` when `parent == nil`; modal presentation needs its own explicit dismissal cleanup

**Correct:** Use `willMoveToParentViewController:` with `parent == nil` and the
idempotent `onDestroy` in the creation template. Temporary disappearance is not teardown.

**Wrong:**
```objc
// Missing destroy — engine leaks
- (void)viewDidDisappear:(BOOL)animated {
    [self.agoraKit leaveChannel:nil];
}
```

---

### 2. Thread Safety

All `AgoraRtcEngineDelegate` callbacks may arrive on a background thread.

**Check:**
- Every UI update inside a delegate callback is wrapped in `dispatch_async(dispatch_get_main_queue(), ^{ })`
- No `UIView` or other UIKit objects mutated directly in callbacks

**Correct:**
```objc
- (void)rtcEngine:(AgoraRtcEngineKit *)engine didJoinedOfUid:(NSUInteger)uid elapsed:(NSInteger)elapsed {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self setupRemoteVideoWithUid:uid];
    });
}
```

**Wrong:**
```objc
- (void)rtcEngine:(AgoraRtcEngineKit *)engine didJoinedOfUid:(NSUInteger)uid elapsed:(NSInteger)elapsed {
    [self setupRemoteVideoWithUid:uid];  // UI update on background thread
}
```

---

### 3. Memory Management

**Check:**
- `__weak typeof(self) weakSelf = self` used in all blocks that capture `self`
- Delegate property on `AgoraRtcEngineKit` is `weak` (it is by SDK design, but verify no strong cycle)
- No `__unsafe_unretained` used for delegate or view references

Import `APIExample_OC-swift.h` for `NetworkManager` and call
`generateTokenWithChannelName:uid:success:` with the snapshotted UID. Swift's `uid = 0`
does not generate an Objective-C selector without `uid:`. Use the complete guarded block
in the creation template; a weak reference without session validation is insufficient.

---

### 4. Permissions

**Check:**
- Camera permission requested before `joinChannelByToken:` for video cases
- Microphone permission requested before `joinChannelByToken:` for all cases
- `joinChannelByToken:` called only inside the permission grant callback

---

### 5. Error Handling

**Check:**
- Return value of `joinChannelByToken:` checked (non-zero = error)
- `rtcEngine:didOccurError:` delegate method implemented and logged
- Token expiry handled via `rtcEngine:tokenPrivilegeWillExpire:` if token is used

---

### 6. Code Conventions

**Check:**
- Entry class inherits `UIViewController`, Main class inherits `BaseViewController`
- Class names follow `<ExampleName>Entry` / `<ExampleName>Main` pattern
- `configs` dictionary (`NSDictionary`) used to pass data from Entry to Main
- File placed under `Examples/Basic/` or `Examples/Advanced/` matching the MenuItem section
- Both `.h` and `.m` files present; public interface minimal in `.h`

---

### 7. API Usage Correctness

**Check:**
- `setVideoEncoderConfiguration:` called before `joinChannelByToken:`
- `setupLocalVideo:` called before `startPreview` and `joinChannelByToken:`
- `enableVideo` called before `setupLocalVideo:` for video cases
- `setClientRole:` called before `joinChannelByToken:` for live streaming cases

---

### 8. Resource Cleanup

**Check:**
- Audio files / custom audio tracks stopped and released on exit
- External video sources unregistered on exit
- Media player destroyed if created (`[self.agoraKit destroyMediaPlayer:player]`)
- Screen capture stopped if started
- Multi-camera capture stopped if started

---

### 9. Xcode Target Membership

For a new or moved case, verify `APIExample-OC.xcodeproj/project.pbxproj` contains file
references for the `.h` and `.m` files, adds the `.m` file to the `APIExample-OC` target's
Sources build phase, and adds every storyboard, localized file, or media asset to its
Resources build phase. A file existing in the folder is not enough. Do not require a
project-file edit for an existing build input that was only modified.

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

## OC-Specific Checks

- Verify `NS_ASSUME_NONNULL_BEGIN/END` wraps the header to reduce nullability warnings
- Verify `IBOutlet` properties are `weak` (Xcode default, but worth confirming)
- For navigation removal, use `willMoveToParentViewController:` with `parent == nil`; distinguish it from temporary disappearance and modal dismissal
- ARC is enabled — no manual `retain`/`release` calls should appear
