# Agent Guide — APIExample-SwiftUI

## Project Context

This is the SwiftUI implementation of Agora RTC SDK examples. Before making any changes, read `ARCHITECTURE.md` to understand the structural rules of this project.

## Rules

### Follow the Architecture

All work in this project must conform to the rules defined in `ARCHITECTURE.md`:
- Every example follows the MVVM pattern with a `View` and a `ViewModel`
- ViewModel inherits `NSObject`, conforms to `ObservableObject` and `AgoraRtcEngineDelegate`
- View holds the ViewModel as `@StateObject`
- State is exposed exclusively via `@Published` properties on the ViewModel
- Delegate callbacks must dispatch UI updates to the main thread
- UIKit video views are bridged into SwiftUI via `UIViewRepresentable`
- Examples are registered in `ContentView.swift` as navigation destinations

### Follow the Existing Language and Framework

- Language is Swift — do not introduce Objective-C files
- UI framework is SwiftUI — do not introduce UIKit view controllers or storyboards
- State management uses `@ObservableObject` / `@Published` / `@StateObject` — do not introduce UIKit delegate-based state patterns
- Match the code style, naming, and patterns of existing examples in this project

### Use Example-Level SKILLs

Each example may contain a `SKILL.md` file in its folder. When working on or referencing a specific example:
1. Check whether a `SKILL.md` exists in that example's directory
2. If it exists, read it before making changes — it describes the API usage, call flow, and known constraints for that example
3. If it does not exist, one will be created in the future; proceed using the source code as the reference

**SKILL.md location pattern:** `APIExample-SwiftUI/Examples/[Basic|Advanced]/<ExampleName>/SKILL.md`

### Use Project-Level SKILLs

For broader tasks, use the skills in `../.agent/skills/`:

| Task | SKILL |
|------|-------|
| Find an existing example | `find-api-example` |
| Create a new example | `create-api-example` |
| Migrate an example to another project | `migrate-api-to-project` |
