# macOS ARCHITECTURE

macOS example project using Swift + Cocoa. Demonstrates Agora RTC SDK features through a collection of self-contained examples organized by complexity.

## Technology Stack

- Language: Swift
- UI Framework: Cocoa (AppKit)
- Architecture: Single-window application with example selection
- State: Instance variables + delegate callbacks

## Directory Structure

```
macOS/
├── APIExample/
│   ├── Examples/
│   │   ├── Basic/
│   │   │   └── <ExampleName>/
│   │   │       ├── <ExampleName>.swift
│   │   │       └── SKILL.md          # Per-example agent guide (present or forthcoming)
│   │   └── Advanced/
│   │       └── <ExampleName>/
│   │           ├── <ExampleName>.swift
│   │           └── SKILL.md          # Per-example agent guide (present or forthcoming)
│   ├── Common/              # Shared utilities (KeyCenter, GlobalSettings, LogUtils, Util)
│   ├── Resources/
│   ├── Base.lproj/          # Storyboard and localization
│   ├── AppDelegate.swift
│   └── ViewController.swift # Main window controller
├── SimpleFilter/            # Specialized filter example
├── APIExample.xcodeproj/    # Xcode project
├── APIExample.xcworkspace/  # Xcode workspace
├── libs/                    # SDK libraries
├── Pods/                    # CocoaPods dependencies
├── .agent/skills/           # Agent skills
│   ├── create-api-example/
│   ├── find-api-example/
│   └── migrate-api-to-project/
├── AGENTS.md                # Agent guide
└── ARCHITECTURE.md          # This file
```

## Architectural Rules

### Example Structure

Each example lives in its own folder under `APIExample/Examples/Basic/` or `APIExample/Examples/Advanced/` and consists of:
- A Swift file containing the example implementation
- Optional: A storyboard or XIB file for UI layout

### Example Pattern

Each example is a self-contained class that:
- Manages its own Agora engine lifecycle
- Implements `AgoraRtcEngineDelegate`
- Receives configuration via initialization or property injection
- Owns all UI elements for that example

### Menu Registration

All examples are registered in `APIExample/ViewController.swift` via a menu or list structure. The example name must match the folder name.

### Naming

- Example folder names: PascalCase (e.g., `JoinChannelVideo`)
- Example class: `<ExampleName>` (e.g., `JoinChannelVideo`)

### Common Utilities

All examples share utilities from `APIExample/Common/`:
- `KeyCenter` — App ID and token
- `GlobalSettings` — Shared runtime configuration
- `LogUtils` — SDK log path
- `Util` — Privatization configuration
