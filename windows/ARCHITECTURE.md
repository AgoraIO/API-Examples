# Windows ARCHITECTURE

Windows example project using C++ + MFC (Microsoft Foundation Classes). Demonstrates Agora RTC SDK features through a collection of self-contained dialog-based examples organized by complexity.

## Technology Stack

- Language: C++
- UI Framework: MFC (Microsoft Foundation Classes)
- Architecture: Dialog-based application with example selection
- State: Member variables + message map callbacks

## Directory Structure

```
windows/
├── APIExample/
│   ├── APIExample/
│   │   ├── Basic/
│   │   │   └── <ExampleName>/
│   │   │       ├── C<ExampleName>Dlg.cpp
│   │   │       ├── C<ExampleName>Dlg.h
│   │   │       └── SKILL.md          # Per-example agent guide (present or forthcoming)
│   │   ├── Advanced/
│   │   │   └── <ExampleName>/
│   │   │       ├── C<ExampleName>Dlg.cpp
│   │   │       ├── C<ExampleName>Dlg.h
│   │   │       └── SKILL.md          # Per-example agent guide (present or forthcoming)
│   │   ├── DirectShow/          # DirectShow utilities
│   │   ├── dsound/              # DirectSound utilities
│   │   ├── res/                 # Resources (icons, dialogs, strings)
│   │   ├── APIExample.cpp
│   │   ├── APIExample.h
│   │   ├── APIExampleDlg.cpp    # Main dialog
│   │   ├── APIExampleDlg.h
│   │   ├── CConfig.cpp          # Configuration management
│   │   ├── CConfig.h
│   │   ├── CSceneDialog.cpp     # Scene selection dialog
│   │   └── CSceneDialog.h
│   ├── APIExample.sln           # Visual Studio solution
│   ├── cicd/                    # CI/CD scripts
│   └── .vscode/                 # VS Code configuration
├── .agent/skills/               # Agent skills
│   ├── create-api-example/
│   ├── find-api-example/
│   └── migrate-api-to-project/
├── AGENTS.md                    # Agent guide
└── ARCHITECTURE.md              # This file
```

## Architectural Rules

### Example Structure

Each example lives in its own folder under `APIExample/APIExample/Basic/` or `APIExample/APIExample/Advanced/` and consists of:
- A `.h` + `.cpp` pair for the dialog class
- Optional: Resource definitions in `.rc` file

### Dialog-Based Pattern

Each example is a dialog class that:
- Inherits from `CDialogEx` or `CDialog`
- Implements message handlers via `BEGIN_MESSAGE_MAP` / `END_MESSAGE_MAP`
- Manages its own Agora engine lifecycle
- Implements `IAgoraRtcEngineEventHandler` interface
- Owns all UI controls and state for that example

### Naming Convention

- Example folder names: PascalCase (e.g., `JoinChannelVideo`)
- Dialog class: `C<ExampleName>Dlg` (e.g., `CJoinChannelVideoDlg`)
- Header file: `C<ExampleName>Dlg.h`
- Implementation file: `C<ExampleName>Dlg.cpp`

### Menu Registration

All examples are registered in `CSceneDialog` via a scene list. The example name must match the folder name.

### Configuration Management

Configuration is centralized in `CConfig` class:
- App ID management
- Token generation
- Global settings (resolution, frame rate, etc.)

### Common Utilities

All examples share utilities:
- `CConfig` — App ID, token, and global settings
- `VideoExtractor` — Video frame extraction
- `YUVReader` — YUV file reading
- DirectShow and DirectSound wrappers
