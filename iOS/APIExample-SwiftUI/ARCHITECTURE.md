# APIExample-SwiftUI Architecture

## Overview

iOS example project using SwiftUI. Demonstrates Agora RTC SDK features with a declarative UI and reactive state management.

## Technology Stack

- Language: Swift
- UI Framework: SwiftUI
- Architecture: MVVM
- State: `@ObservableObject` / `@Published` / `@StateObject`
- Video Rendering: `UIViewRepresentable` wrapping UIKit views

## Directory Structure

```
APIExample-SwiftUI/
├── APIExample-SwiftUI/
│   ├── Examples/
│   │   ├── Basic/
│   │   │   └── <ExampleName>/
│   │   │       ├── <ExampleName>View.swift
│   │   │       ├── <ExampleName>ViewModel.swift
│   │   │       └── SKILL.md          # Per-example agent guide (present or forthcoming)
│   │   └── Advanced/
│   │       └── <ExampleName>/
│   │           ├── <ExampleName>View.swift
│   │           ├── <ExampleName>ViewModel.swift
│   │           └── SKILL.md          # Per-example agent guide (present or forthcoming)
│   ├── Common/              # Shared utilities (KeyCenter, GlobalSettings, LogUtils, Util)
│   ├── Resources/
│   ├── App.swift            # App entry point
│   └── ContentView.swift    # Root navigation view
├── AGENTS.md                # Agent guide for this project
└── ARCHITECTURE.md          # This file
```

## Architectural Rules

### Example Structure

Each example lives in its own folder under `Examples/Basic/` or `Examples/Advanced/` and consists of:
- A `View` file (`<ExampleName>View.swift`)
- A `ViewModel` file (`<ExampleName>ViewModel.swift`)
- A `UIViewRepresentable` wrapper for video rendering when needed

### MVVM Pattern

**ViewModel** (`<ExampleName>ViewModel : NSObject, ObservableObject, AgoraRtcEngineDelegate`)
- Owns the Agora engine lifecycle
- Exposes state to the View exclusively via `@Published` properties
- Implements all delegate callbacks; dispatches UI updates to the main thread

**View** (`<ExampleName>View : View`)
- Holds the ViewModel as `@StateObject`
- Binds to `@Published` properties for rendering
- Triggers ViewModel methods in response to user actions
- Manages engine lifecycle via `.onAppear` / `.onDisappear`

### Video Rendering

UIKit video views are bridged into SwiftUI via `UIViewRepresentable`. The ViewModel owns the `UIView` instances; the View wraps them for display.

### Navigation

Examples are registered in `ContentView.swift` as navigation destinations.

### Naming

- Example folder names: PascalCase (e.g., `JoinChannelVideo`)
- View: `<ExampleName>View`
- ViewModel: `<ExampleName>ViewModel`

### Common Utilities

All examples share utilities from `Common/`:
- `KeyCenter` — App ID and token
- `GlobalSettings` — Shared runtime configuration
- `LogUtils` — SDK log path
- `Util` — Privatization configuration
