# AGENTS.md — APIExample-Audio

Audio-only demo project. Uses `AgoraAudio_iOS` — the video module is not available.
Use this project only when the PRD is explicitly audio-only.

## Build Commands

```bash
pod install                      # install CocoaPods dependencies
# Then open APIExample-Audio.xcworkspace in Xcode and build (Cmd+B)
```

## App ID Configuration

Edit `APIExample-Audio/Common/KeyCenter.swift`:
```swift
static let AppId: String = "YOUR_APP_ID"
static let Certificate: String? = nil   // leave nil if App Certificate is not enabled
```

## How to Add a New Case

See `ARCHITECTURE.md` for the full registration mechanism. The required steps are:

1. Create an example folder under `APIExample-Audio/Examples/Basic/` or `APIExample-Audio/Examples/Advanced/`:
   - `<ExampleName>.swift` containing both `<ExampleName>Entry` and `<ExampleName>Main` classes
   - `Base.lproj/<ExampleName>.storyboard` with two scenes: Entry and Main

2. Add a `MenuItem` to the `menus` array in `APIExample-Audio/ViewController.swift`:
   ```swift
   MenuItem(name: "My New Case".localized, storyboard: "MyNewCase", controller: "MyNewCase")
   ```

3. Build and run — the case appears in the list.

## Architecture Red Lines

- Do NOT call `enableVideo()`, `setupLocalVideo()`, `setupRemoteVideo()`, or any camera-related APIs — `AgoraAudio_iOS` has no video module and will crash at runtime.
- Do NOT add video, screen sharing, or beauty cases — use `APIExample/` instead.
- Do NOT use SwiftUI — this project is UIKit + Storyboards only.
- Do NOT introduce Objective-C files — this project is Swift only.
- Each case must create and destroy its own `AgoraRtcEngineKit` instance.
- Always call `agoraKit.leaveChannel()` before `AgoraRtcEngineKit.destroy()` when the screen is dismissed.
- `AgoraRtcEngineDelegate` callbacks may arrive on a background thread — dispatch UI updates to the main thread.
- Always request microphone permission before calling `joinChannel()`. Audio cases only need `RECORD_AUDIO`.

## Further Reading

- `ARCHITECTURE.md` — full directory layout, case registration internals, Entry/Main pattern details
