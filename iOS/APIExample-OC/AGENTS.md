# AGENTS.md — APIExample-OC

Objective-C version of the API demo. Mirrors cases from `APIExample/` but uses
Objective-C instead of Swift.

## Build Commands

```bash
pod install                      # install CocoaPods dependencies
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

## How to Add a New Case

See `ARCHITECTURE.md` for the full registration mechanism. The required steps are:

1. Create an example folder under `APIExample-OC/Examples/Basic/` or `APIExample-OC/Examples/Advanced/`:
   - `<ExampleName>.h` / `<ExampleName>.m` containing both Entry and Main classes
   - `<ExampleName>.storyboard` with two scenes: Entry (`EntryViewController`) and Main

2. Add a `MenuItem` to the `+[MenuSection menus]` method in `APIExample-OC/ViewController.m`:
   ```objc
   [[MenuItem alloc] initWithName:@"My New Case".localized storyboard:@"MyNewCase" controller:@""]
   ```

3. Build and run — the case appears in the list.

## Architecture Red Lines

- Do NOT add audio-only cases that require `AgoraAudio_iOS` exclusivity — use `APIExample-Audio/` instead.
- Do NOT use SwiftUI or UIKit Storyboards with Swift — this project is Objective-C + Storyboards only.
- Do NOT introduce Swift files — this project is Objective-C only.
- Each case must create and destroy its own `AgoraRtcEngineKit` instance.
- Always call `[agoraKit leaveChannel:]` before `[AgoraRtcEngineKit destroy]` when the screen is dismissed.
- `AgoraRtcEngineDelegate` callbacks may arrive on a background thread — dispatch UI updates to the main thread.
- Always request camera/microphone permissions before calling `joinChannelByToken:`.

## Further Reading

- `ARCHITECTURE.md` — full directory layout, case registration internals, Entry/Main pattern details
