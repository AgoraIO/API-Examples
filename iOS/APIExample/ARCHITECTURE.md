# APIExample Architecture

## Overview

Standard iOS example project using UIKit + Swift. Demonstrates Agora RTC SDK features through a collection of self-contained examples organized by complexity.

## Technology Stack

- Language: Swift
- UI Framework: UIKit + Storyboards
- Architecture: Entry/Main ViewController pattern
- State: Instance variables + delegate callbacks

## Directory Structure

```
APIExample/
├── APIExample/
│   ├── Examples/
│   │   ├── Basic/
│   │   │   └── <ExampleName>/
│   │   │       ├── <ExampleName>.swift
│   │   │       ├── Base.lproj/<ExampleName>.storyboard
│   │   │       └── SKILL.md          # Per-example agent guide (present or forthcoming)
│   │   └── Advanced/
│   │       └── <ExampleName>/
│   │           ├── <ExampleName>.swift
│   │           ├── Base.lproj/<ExampleName>.storyboard
│   │           └── SKILL.md          # Per-example agent guide (present or forthcoming)
│   ├── Common/              # Shared utilities (KeyCenter, GlobalSettings, LogUtils, Util)
│   ├── Resources/
│   ├── AppDelegate.swift
│   └── ViewController.swift # Root menu controller
├── AGENTS.md                # Agent guide for this project
└── ARCHITECTURE.md          # This file
```

## Architectural Rules

### Example Structure

Each example lives in its own folder under `Examples/Basic/` or `Examples/Advanced/` and consists of:
- A Swift file containing both `Entry` and `Main` classes
- A storyboard with two scenes: Entry and Main

### Entry/Main Pattern

Every example is split into two view controller roles:

**Entry** (`<ExampleName>Entry : UIViewController`)
- Collects user configuration before entering the example
- Owns the storyboard loading and Main VC instantiation
- Passes configuration to Main via a `configs` dictionary

**Main** (`<ExampleName>Main : BaseViewController`)
- Owns the Agora engine lifecycle for the duration of the example
- Implements `AgoraRtcEngineDelegate`
- Receives configuration exclusively through `configs`

### Storyboard Convention

Each example storyboard contains exactly two scenes:
- Entry scene — storyboard ID: `EntryViewController`
- Main scene — storyboard ID: `<ExampleName>` (matches folder name)

### Menu Registration

All examples are registered in `ViewController.swift` via a `MenuItem` in the `menus` array. The `storyboard` field must match the example folder name.

### Naming

- Example folder names: PascalCase (e.g., `JoinChannelVideo`)
- Entry class: `<ExampleName>Entry`
- Main class: `<ExampleName>Main`

### Common Utilities

All examples share utilities from `Common/`:
- `KeyCenter` — App ID and token
- `GlobalSettings` — Shared runtime configuration (region, resolution, fps)
- `LogUtils` — SDK log path
- `Util` — Privatization configuration
- `AGEVideoLayout` — Multi-user video layout management
