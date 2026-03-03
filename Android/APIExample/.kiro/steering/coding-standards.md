---
inclusion: always
---

# Coding Standards (APIExample)

## RtcEngine Lifecycle Rules

1. Each case owns its own `RtcEngine` instance — never share across screens
2. On screen destruction, always call `leaveChannel()` before `RtcEngine.destroy()`
3. `IRtcEngineEventHandler` callbacks run on a background thread — always switch to the main thread for UI updates
4. Always request permissions before calling `joinChannel()`

## Java Code Standards

- `RtcEngine.destroy()` must be called via `handler.post(RtcEngine::destroy)` — calling it directly blocks the main thread and causes ANR
- Always null-check the return value of `getPrivateCloudConfig()` — it returns null in non-private-cloud builds
- `setParameters(...)` is a required call in every case for backend reporting — do not omit it
- Use `runOnUIThread()` for UI updates

## Naming Conventions

- Case class names: PascalCase, placed under `examples/{basic|advanced}/`
- String resources: `item_` prefix
- Layout files: `fragment_` + snake_case
