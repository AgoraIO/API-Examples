# AGENTS.md

Entry point for AI agents. Read this first, then go to the relevant project's own `AGENTS.md`.

## Projects

| Project | SDK | Purpose |
|---|---|---|
| `APIExample/` | `full-sdk` | Full demo — all APIs, Java/Kotlin, default choice |
| `APIExample-Audio/` | `voice-sdk` | Audio-only — no video APIs available |
| `APIExample-Compose/` | `full-sdk` | Jetpack Compose UI, mirrors APIExample cases |

SDK version: `gradle.properties` → `rtc_sdk_version` (currently `4.6.3`)

## Which Project to Use

- PRD involves video / screen sharing / beauty / extensions → `APIExample/`
- PRD is audio-only → `APIExample-Audio/`
- PRD asks for Compose UI, or porting an existing case → `APIExample-Compose/`
- No project specified → default to `APIExample/`

Never share source files between projects.

## Architecture Red Lines (all projects)

1. Each case owns its own `RtcEngine` — never share across screens.
2. Always `leaveChannel()` then `RtcEngine.destroy()` when the screen is destroyed.
3. `IRtcEngineEventHandler` callbacks are on a background thread — post UI updates to main thread.
4. Always request permissions before calling `joinChannel()`.

## Further Reading

- `ARCHITECTURE.md` — three-project structure overview
- `APIExample/AGENTS.md` — build commands, config, case registration for the full demo
- `APIExample-Audio/AGENTS.md` — same for the audio demo
- `APIExample-Compose/AGENTS.md` — same for the Compose demo
