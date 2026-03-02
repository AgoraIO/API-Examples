# Agent Guide — APIExample

## Project Context

This is the UIKit + Swift implementation of Agora RTC SDK examples. Before making any changes, read `ARCHITECTURE.md` to understand the structural rules of this project.

## Rules

### Follow the Architecture

All work in this project must conform to the rules defined in `ARCHITECTURE.md`:
- Every example uses the Entry/Main ViewController pattern
- Entry class inherits `UIViewController`; Main class inherits `BaseViewController`
- Each example has exactly one storyboard with two scenes (`EntryViewController` and `<ExampleName>`)
- Configuration flows from Entry to Main exclusively via the `configs` dictionary
- All examples are registered in `ViewController.swift` via `MenuItem`

### Follow the Existing Language and Framework

- Language is Swift — do not introduce Objective-C files
- UI is UIKit + Storyboards — do not introduce SwiftUI views
- State management uses instance variables and delegate callbacks — do not introduce Combine or async/await patterns unless they already exist in the file being modified
- Match the code style, naming, and patterns of existing examples in this project

### Use Example-Level SKILLs

Each example may contain a `SKILL.md` file in its folder. When working on or referencing a specific example:
1. Check whether a `SKILL.md` exists in that example's directory
2. If it exists, read it before making changes — it describes the API usage, call flow, and known constraints for that example
3. If it does not exist, one will be created in the future; proceed using the source code as the reference

**SKILL.md location pattern:** `APIExample/Examples/[Basic|Advanced]/<ExampleName>/SKILL.md`

### Use Project-Level SKILLs

For broader tasks, use the skills in `../.agent/skills/`:

| Task | SKILL |
|------|-------|
| Find an existing example | `find-api-example` |
| Create a new example | `create-api-example` |
| Migrate an example to another project | `migrate-api-to-project` |
