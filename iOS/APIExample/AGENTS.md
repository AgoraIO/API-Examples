# AGENTS.md — APIExample

Full demo project. Covers all Agora RTC APIs using UIKit + Swift.
Default project when the PRD does not specify a platform variant.

## Build Commands

```bash
pod install                      # install CocoaPods dependencies
# Then open APIExample.xcworkspace in Xcode and build (Cmd+B)
```

## App ID Configuration

Edit `APIExample/Common/KeyCenter.swift`:
```swift
static let AppId: String = "YOUR_APP_ID"
static let Certificate: String? = nil   // leave nil if App Certificate is not enabled
```

## How to Add a New Case

See `ARCHITECTURE.md` for the full registration mechanism. The required steps are:

1. Create an example folder under `APIExample/Examples/Basic/` or `APIExample/Examples/Advanced/`:
   - `<ExampleName>.swift` containing both `<ExampleName>Entry` and `<ExampleName>Main` classes
   - `Base.lproj/<ExampleName>.storyboard` with two scenes: Entry (`EntryViewController`) and Main (`<ExampleName>`)

2. Add a `MenuItem` to the `menus` array in `APIExample/ViewController.swift`:
   ```swift
   MenuItem(name: "My New Case".localized, storyboard: "MyNewCase", controller: "MyNewCase")
   ```

3. Build and run — the case appears in the list.

## Architecture Red Lines

- Do NOT add audio-only cases that require `AgoraAudio_iOS` exclusivity — use `APIExample-Audio/` instead.
- Do NOT use SwiftUI — this project is UIKit + Storyboards only.
- Do NOT introduce Objective-C files — this project is Swift only.
- Each case must create and destroy its own `AgoraRtcEngineKit` instance.
- Always call `agoraKit.leaveChannel()` before `AgoraRtcEngineKit.destroy()` when the screen is dismissed.
- `AgoraRtcEngineDelegate` callbacks may arrive on a background thread — dispatch UI updates to the main thread.
- Always request camera/microphone permissions before calling `joinChannel()`.

## Further Reading

- `ARCHITECTURE.md` — full directory layout, case registration internals, Entry/Main pattern details
