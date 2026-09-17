# ARCHITECTURE.md

Four independent iOS example projects, each with its own Xcode workspace and CocoaPods dependencies.
For internal details of each project, see the project-level `ARCHITECTURE.md`.

---

## Deployment Targets and Lifecycle

| Project | Minimum iOS | App lifecycle |
| --- | --- | --- |
| `APIExample` | 12.0 | AppDelegate window on iOS 12; SceneDelegate on iOS 13+ |
| `APIExample-Audio` | 12.0 | AppDelegate window on iOS 12; SceneDelegate on iOS 13+ |
| `APIExample-OC` | 12.0 | AppDelegate window on iOS 12; SceneDelegate on iOS 13+ |
| `APIExample-SwiftUI` | 14.0 | SwiftUI `App` and `WindowGroup` |

The app and extension deployment targets remain unchanged. The Audio project has no app
extension. Choose an Xcode version that supports the project's deployment target; see
[Apple's Xcode support table](https://developer.apple.com/support/xcode/). Adopting the
scene lifecycle does not require increasing the minimum iOS version.

The three UIKit apps configure a single application scene and retain the iOS 12 startup
path. SwiftUI keeps its existing scene configuration. See
[Apple TN3187](https://developer.apple.com/documentation/technotes/tn3187-migrating-to-the-uikit-scene-based-life-cycle)
and each project's architecture document for window ownership and UI presentation rules.

---

## APIExample — Full Demo

- Language: Swift
- UI Framework: UIKit + Storyboards
- SDK: AgoraRtcEngine_iOS (full-featured)
- Architecture: Entry/Main ViewController pattern
- Case registration: `MenuItem` array in `ViewController.swift`
- Details: [APIExample/ARCHITECTURE.md](APIExample/ARCHITECTURE.md)

---

## APIExample-SwiftUI — SwiftUI Demo

- Language: Swift
- UI Framework: SwiftUI
- SDK: AgoraRtcEngine_iOS (full-featured)
- Architecture: MVVM (View + ViewModel)
- Case registration: navigation destinations in `ContentView.swift`
- Details: [APIExample-SwiftUI/ARCHITECTURE.md](APIExample-SwiftUI/ARCHITECTURE.md)

---

## APIExample-OC — Objective-C Demo

- Language: Objective-C
- UI Framework: UIKit + Storyboards
- SDK: AgoraRtcEngine_iOS (full-featured)
- Architecture: Entry/Main ViewController pattern (same as APIExample)
- Case registration: `MenuItem` array in `ViewController.m`
- Details: [APIExample-OC/ARCHITECTURE.md](APIExample-OC/ARCHITECTURE.md)

---

## APIExample-Audio — Audio-Only Demo

- Language: Swift
- UI Framework: UIKit + Storyboards
- SDK: AgoraAudio_iOS (no video module)
- Architecture: Entry/Main ViewController pattern
- Case registration: `MenuItem` array in `ViewController.swift`
- Constraint: no video rendering views
- Details: [APIExample-Audio/ARCHITECTURE.md](APIExample-Audio/ARCHITECTURE.md)
