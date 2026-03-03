---
inclusion: always
---

# Coding Standards (APIExample-Compose)

## RtcEngine Lifecycle Rules

1. Each case owns its own `RtcEngine` instance — never share across screens
2. On screen destruction, always call `leaveChannel()` before `RtcEngine.destroy()`
3. `IRtcEngineEventHandler` callbacks run on a background thread — UI updates must switch to the main thread
4. Always request permissions before calling `joinChannel()`

## Kotlin/Compose Standards

- `DisposableEffect` key must be `lifecycleOwner`, not `Unit` — wrong key means cleanup never fires on back navigation
- `rememberSaveable` for values that must survive rotation (channelName, isJoined, uid)
- `remember` for non-serializable objects like RtcEngine
- `mAreaCode` must use `SettingPreferences.getArea()` — never hardcode
- `@Preview` only on private `*View` functions
- `Toast`/`Dialog` inside callbacks require `coroutineScope.launch(Dispatchers.Main)`

## Naming Conventions

- Case file names: PascalCase.kt, placed under `samples/`
- String resources: `example_` prefix
