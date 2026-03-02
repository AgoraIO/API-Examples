# APIExample-Audio Architecture

## Overview

iOS example project using UIKit + Swift, focused exclusively on audio features. Shares the same Entry/Main pattern as `APIExample` but omits all video rendering concerns.

## Technology Stack

- Language: Swift
- UI Framework: UIKit + Storyboards
- Architecture: Entry/Main ViewController pattern
- State: Instance variables + delegate callbacks
- Focus: Audio capture, playback, effects, mixing, and custom sources

## Directory Structure

```
APIExample-Audio/
├── APIExample-Audio/
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
│   ├── Resources/           # Audio assets and images
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
- Collects user configuration (channel name, audio settings)
- Owns the storyboard loading and Main VC instantiation
- Passes configuration to Main via a `configs` dictionary

**Main** (`<ExampleName>Main : BaseViewController`)
- Owns the Agora engine lifecycle for the duration of the example
- Implements `AgoraRtcEngineDelegate`
- Receives configuration exclusively through `configs`
- UI contains only audio controls — no video rendering views

### Storyboard Convention

Each example storyboard contains exactly two scenes:
- Entry scene — storyboard ID: `EntryViewController`
- Main scene — storyboard ID: `<ExampleName>` (matches folder name)

### Audio-Only Constraint

Main view controllers in this project must not include video rendering views or video canvas setup. All UI is limited to audio controls, status indicators, and effect parameter inputs.

### Menu Registration

All examples are registered in `ViewController.swift` via a `MenuItem` in the `menus` array. The `storyboard` field must match the example folder name.

### Naming

- Example folder names: PascalCase (e.g., `JoinChannelAudio`)
- Entry class: `<ExampleName>Entry`
- Main class: `<ExampleName>Main`

### Common Utilities

All examples share utilities from `Common/`:
- `KeyCenter` — App ID and token
- `GlobalSettings` — Shared runtime configuration
- `LogUtils` — SDK log path
- `Util` — Privatization configuration
