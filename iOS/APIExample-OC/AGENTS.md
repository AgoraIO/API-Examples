# AGENTS.md — APIExample-OC

Objective-C variant of the API demo. Mirrors cases from `APIExample/` using Objective-C instead of Swift.

## Build Commands

```bash
pod install
# Then open APIExample-OC.xcworkspace in Xcode and build (Cmd+B)
```

## App ID Configuration

Edit `APIExample-OC/Common/KeyCenter.m`:
```objc
+ (NSString *)AppId {
    return @"YOUR_APP_ID";
}

+ (NSString *)Certificate {
    return nil;   // leave nil if App Certificate is not enabled
}
```

## Architecture Red Lines

- Do NOT skip calling `leaveChannel:` + `[AgoraRtcEngineKit destroy]` in `dealloc` or `viewDidDisappear:`
- Do NOT update UI directly inside `AgoraRtcEngineDelegate` callbacks — always dispatch to the main thread via `dispatch_async(dispatch_get_main_queue(), ^{ ... })`
- Do NOT create `AgoraRtcEngineKit` in the Entry VC — engine lifecycle belongs to Main VC only
- Do NOT share `AgoraRtcEngineKit` instances across examples
- Do NOT add cases to `Main.storyboard` — each example must have its own `.storyboard` file
- Do NOT use ARC-unsafe patterns (`__unsafe_unretained`) for delegate references — use `__weak`
- Do NOT request camera/microphone permissions after calling `joinChannelByToken:`

## Skills

| Task | Skill | When to use |
|------|-------|-------------|
| Add or modify a case | `.agent/skills/upsert-case/` | Need to create a new API demo or update an existing one |
| Code review | `.agent/skills/review-case/` | Review case code for lifecycle, thread safety, and OC convention compliance |
| Find an existing case | `.agent/skills/query-cases/` | Locate which file demonstrates a specific API or feature |

## Further Reading

- `ARCHITECTURE.md` — full directory layout, case registration, Entry/Main pattern, engine lifecycle
