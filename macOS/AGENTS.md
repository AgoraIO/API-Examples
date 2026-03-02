# Agent Guide — macOS

## Project Context

This is the Swift + Cocoa implementation of Agora RTC SDK examples for macOS. Before making any changes, read `ARCHITECTURE.md` to understand the structural rules.

## Rules

### Follow the Architecture

All work must conform to the rules defined in `ARCHITECTURE.md`:
- Every example is a self-contained class implementing `AgoraRtcEngineDelegate`
- Each example manages its own Agora engine lifecycle
- Configuration is passed via initialization or property injection
- All examples are registered in `APIExample/ViewController.swift`

### Follow the Existing Language and Framework

- Language is Swift — do not introduce Objective-C files
- UI framework is Cocoa (AppKit) — do not introduce UIKit or SwiftUI
- State management uses instance variables and delegate callbacks — do not introduce Combine or async/await patterns unless they already exist in the file being modified
- Match the code style, naming, and patterns of existing examples

### Use Example-Level SKILLs

Each example may contain a `SKILL.md` file in its folder. When working on or referencing a specific example:
1. Check whether a `SKILL.md` exists in that example's directory
2. If it exists, read it before making changes — it describes the API usage, call flow, and known constraints
3. If it does not exist, one will be created in the future; proceed using the source code as the reference

**SKILL.md location pattern:** `APIExample/Examples/[Basic|Advanced]/<ExampleName>/SKILL.md`

### Use Project-Level SKILLs

For broader tasks, use the skills in `.agent/skills/`:

| Task | SKILL |
|------|-------|
| Find an existing example | `find-api-example` |
| Create a new example | `create-api-example` |
