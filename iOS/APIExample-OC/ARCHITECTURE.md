# APIExample-OC Architecture

## Overview

iOS example project using Objective-C + UIKit. Mirrors the architectural patterns of `APIExample` (Swift/UIKit) but implemented in Objective-C.

## Technology Stack

- Language: Objective-C
- UI Framework: UIKit + Storyboards
- Architecture: Entry/Main ViewController pattern
- State: Instance variables + delegate callbacks

## Directory Structure

```
APIExample-OC/
├── APIExample-OC/
│   ├── Examples/
│   │   ├── Basic/
│   │   │   └── <ExampleName>/
│   │   │       ├── <ExampleName>.h / .m
│   │   │       ├── Base.lproj/<ExampleName>.storyboard
│   │   │       └── SKILL.md          # Per-example agent guide (present or forthcoming)
│   │   └── Advanced/
│   │       └── <ExampleName>/
│   │           ├── <ExampleName>.h / .m
│   │           ├── Base.lproj/<ExampleName>.storyboard
│   │           └── SKILL.md          # Per-example agent guide (present or forthcoming)
│   ├── Common/              # Shared utilities (KeyCenter, GlobalSettings, LogUtils, Util)
│   ├── Resources/
│   ├── AppDelegate.h / .m
│   └── ViewController.h / .m  # Root menu controller
├── AGENTS.md                # Agent guide for this project
└── ARCHITECTURE.md          # This file
```

## Architectural Rules

### Example Structure

Each example lives in its own folder under `Examples/Basic/` or `Examples/Advanced/` and consists of:
- A `.h` + `.m` pair for the Entry class
- A `.h` + `.m` pair for the Main class
- A storyboard with two scenes: Entry and Main

### Entry/Main Pattern

Every example is split into two view controller roles:

**Entry** (`<ExampleName>Entry : UIViewController`)
- Collects user configuration before entering the example
- Owns the storyboard loading and Main VC instantiation
- Passes configuration to Main via a `configs` dictionary (`NSDictionary`)

**Main** (`<ExampleName>Main : BaseViewController`)
- Owns the Agora engine lifecycle for the duration of the example
- Conforms to `AgoraRtcEngineDelegate`
- Receives configuration exclusively through `configs`

### Storyboard Convention

Each example storyboard contains exactly two scenes:
- Entry scene — storyboard ID: `EntryViewController`
- Main scene — storyboard ID: `<ExampleName>` (matches folder name)

### Menu Registration

All examples are registered in `ViewController.m` via a `MenuItem` in the menus array. The `storyboard` field must match the example folder name.

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
