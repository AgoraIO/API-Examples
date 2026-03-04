# AGENTS.md

Entry point for AI agents. Read this first, then go to the relevant project's own `AGENTS.md`.

## Projects

| Project | SDK | Purpose |
|---|---|---|
| `APIExample/` | `AgoraRtcEngine_iOS` | Full demo — all APIs, UIKit + Swift, default choice |
| `APIExample-SwiftUI/` | `AgoraRtcEngine_iOS` | SwiftUI, mirrors APIExample cases |
| `APIExample-OC/` | `AgoraRtcEngine_iOS` | Objective-C + UIKit, mirrors APIExample cases |
| `APIExample-Audio/` | `AgoraAudio_iOS` | Audio-only — no video APIs available |

SDK version: each project's `Podfile` specifies the SDK version.

## Which Project to Use

- PRD involves video / screen sharing / beauty / extensions → `APIExample/`
- PRD is audio-only → `APIExample-Audio/`
- PRD asks for SwiftUI, or porting an existing case → `APIExample-SwiftUI/`
- PRD asks for Objective-C → `APIExample-OC/`
- No project specified → default to `APIExample/`

Never share source files between projects.

## Further Reading

- `ARCHITECTURE.md` — four-project structure overview
- `APIExample/AGENTS.md` — config, case registration for the full demo
- `APIExample-SwiftUI/AGENTS.md` — same for the SwiftUI demo
- `APIExample-OC/AGENTS.md` — same for the Objective-C demo
- `APIExample-Audio/AGENTS.md` — same for the audio demo
