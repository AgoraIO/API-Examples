# AGENTS.md — APIExample-SwiftUI

SwiftUI version of the API demo. Mirrors cases from `APIExample/` but uses
SwiftUI views instead of UIKit + Storyboards.

## Build Commands

```bash
pod install                      # install CocoaPods dependencies
# Then open APIExample-SwiftUI.xcworkspace in Xcode and build (Cmd+B)
```

## App ID Configuration

Edit `APIExample-SwiftUI/Common/KeyCenter.swift`:
```swift
static let AppId: String = "YOUR_APP_ID"
static let Certificate: String? = nil   // leave nil if App Certificate is not enabled
```

## How to Add a New Case

See `ARCHITECTURE.md` for the full registration mechanism. The required steps are:

1. Create an example folder under `APIExample-SwiftUI/Examples/Basic/` or `APIExample-SwiftUI/Examples/Advanced/`:
   - `<ExampleName>.swift` — Entry view and Main view
   - `<ExampleName>RTC.swift` — RTC wrapper class (`ObservableObject` + `AgoraRtcEngineDelegate`)

2. Add a `MenuItem` to the `menus` array in `APIExample-SwiftUI/ContentView.swift`:
   ```swift
   MenuItem(name: "My New Case".localized, view: AnyView(MyNewCaseEntry()))
   ```

3. Build and run — the case appears in the list.

## Architecture Red Lines

- Do NOT add audio-only cases that require `AgoraAudio_iOS` exclusivity — use `APIExample-Audio/` instead.
- Do NOT use UIKit view controllers or Storyboards — this project is SwiftUI only.
- Do NOT introduce Objective-C files — this project is Swift only.
- Each case must create and destroy its own `AgoraRtcEngineKit` instance.
- Always call `agoraKit.leaveChannel()` before `AgoraRtcEngineKit.destroy()` in `onDestroy()`.
- `AgoraRtcEngineDelegate` callbacks may arrive on a background thread — dispatch UI updates to the main thread.
- Engine setup should happen in `.onAppear`, cleanup in `.onDisappear`.
- Always request camera/microphone permissions before calling `joinChannel()`.

## Further Reading

- `ARCHITECTURE.md` — full directory layout, case registration internals, Entry/RTC pattern details
