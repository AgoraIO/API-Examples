# ARCHITECTURE.md

Four independent iOS example projects sharing one Xcode workspace, each managing dependencies via CocoaPods.
For internal details of each project, see the project-level `ARCHITECTURE.md`.

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
