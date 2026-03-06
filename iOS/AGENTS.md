# AGENTS.md

Entry point for AI agents working on iOS examples. Read this first, then go to the relevant project's own `AGENTS.md`.

## Projects

| Project | SDK | Purpose |
|---------|-----|---------|
| `APIExample/` | `AgoraRtcEngine_iOS` | Full demo — all APIs, UIKit + Swift, default choice |
| `APIExample-SwiftUI/` | `AgoraRtcEngine_iOS` | SwiftUI variant, mirrors APIExample cases |
| `APIExample-OC/` | `AgoraRtcEngine_iOS` | Objective-C variant, mirrors APIExample cases |
| `APIExample-Audio/` | `AgoraAudio_iOS` | Audio-only — no video APIs available |

SDK version: each project's `Podfile` specifies the version.

## Which Project to Use

- Need video call, screen sharing, beauty filters, or extensions → `APIExample/`
- Audio-only features (voice call, audio effects, spatial audio) → `APIExample-Audio/`
- Building with SwiftUI, or porting an existing case to SwiftUI → `APIExample-SwiftUI/`
- Need Objective-C implementation → `APIExample-OC/`
- Not sure → default to `APIExample/`

## Architecture Red Lines

- Do NOT share source files, storyboards, or SDK dependencies between projects
- Do NOT add video rendering APIs (`enableVideo`, `setupLocalVideo`) to `APIExample-Audio/`
- Do NOT call SDK APIs on a background thread without dispatching UI updates to the main thread
- Do NOT commit `KeyCenter.swift` / `KeyCenter.m` with real App IDs or certificates
- Always call `leaveChannel()` and `AgoraRtcEngineKit.destroy()` when an example screen is closed

## Further Reading

- `ARCHITECTURE.md` — four-project structure overview
- `APIExample/AGENTS.md` — build commands, config, Skills for the full demo
- `APIExample-SwiftUI/AGENTS.md` — same for the SwiftUI demo
- `APIExample-OC/AGENTS.md` — same for the Objective-C demo
- `APIExample-Audio/AGENTS.md` — same for the audio demo
