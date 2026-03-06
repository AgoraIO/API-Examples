# AGENTS.md — Android

Android platform entry point. Read this first, then go to the relevant project's `AGENTS.md`.

## Projects

| Project | SDK | Language | Purpose |
|---------|-----|----------|---------|
| `APIExample/` | full-sdk | Java / Kotlin + XML | All APIs — default choice |
| `APIExample-Audio/` | voice-sdk | Java + XML | Audio-only — no video APIs |
| `APIExample-Compose/` | full-sdk | Kotlin + Compose | Compose UI, mirrors APIExample cases |

SDK version: each project's `gradle.properties` → `rtc_sdk_version` (currently `4.6.3`)

## Project Selection

- Video / screen sharing / beauty / extensions → `APIExample/`
- Audio-only → `APIExample-Audio/`
- Compose UI or porting an existing case → `APIExample-Compose/`
- No project specified → default to `APIExample/`

Never share source files between projects.

## Navigation

| Project | Entry Point |
|---------|-------------|
| `APIExample/` | `APIExample/AGENTS.md` |
| `APIExample-Audio/` | `APIExample-Audio/AGENTS.md` |
| `APIExample-Compose/` | `APIExample-Compose/AGENTS.md` |
