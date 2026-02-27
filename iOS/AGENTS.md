# AI Agent Guide

## Folder Overview

This folder contains **Agora RTC iOS API Examples**, with 4 iOS example projects:

| Project | Tech Stack | Description |
|---------|------------|-------------|
| `APIExample/` | UIKit + Storyboard | Standard implementation, most complete |
| `APIExample-SwiftUI/` | SwiftUI | Declarative UI |
| `APIExample-OC/` | Objective-C | Traditional language |
| `APIExample-Audio/` | UIKit | Audio-focused |

## Core Workflow

### Task Type → SKILL Mapping

Based on your task, activate the corresponding SKILL:

| Task | Activate SKILL | Description |
|------|----------------|-------------|
| Find example | `find-api-example` | Locate feature code location |
| Add new example | `api-example-conventions` → `create-api-example` | Understand conventions first, then create |
| Modify example | `api-example-conventions` | Understand structure conventions |
| Migrate code | `migrate-api-to-project` | Cross-project/architecture/language migration |
| Understand API | Check `SKILL.md` in example directory | Specific API usage guide |

### Activation Method

```
discloseContext(name: "skill-name")
```

**Important:** SKILLs are located in `.agent/skills/` (not the default `.kiro/skills/`)

## SKILL Documentation Index

### Project-level SKILLs (`.agent/skills/`)

#### 1. find-api-example
**Quick example search**

- Locate example code for specific features
- Query example file paths
- Use case: When unsure which example contains the functionality

#### 2. api-example-conventions
**iOS example structure conventions**

- Contains conventions for 4 project types (UIKit/SwiftUI/OC/Audio)
- Load detailed conventions for specific projects on demand
- Use case: Must read before adding or modifying examples

#### 3. create-api-example
**Steps to create new example**

- Complete creation workflow checklist
- Directory, code, Storyboard, menu registration
- Dependency: Requires understanding `api-example-conventions` first

#### 4. migrate-api-to-project
**Cross-project migration guide**

- Supports cross-architecture (MVC/MVVM/VIPER)
- Supports cross-language (Swift/Objective-C)
- Supports cross-UI framework (UIKit/SwiftUI)
- Auto-activates: `api-example-conventions`

### Example-level SKILLs (in example directories)

Some examples contain `SKILL.md`, providing:
- API call flow and parameters
- Business scenario integration solutions
- Common Q&A

**Example location:** `APIExample/APIExample/Examples/Basic/JoinChannelVideo/SKILL.md`

## Project Structure

```
iOS/
├── .agent/skills/              # SKILL documentation (custom location)
├── APIExample/                 # UIKit standard project
├── APIExample-SwiftUI/         # SwiftUI project
├── APIExample-OC/              # Objective-C project
├── APIExample-Audio/           # Audio-focused project
└── AGENTS.md                   # This file
```

## Best Practices

1. **Search first, create later** — Use `find-api-example` to confirm if functionality already exists
2. **Conventions first, implementation later** — Activate `api-example-conventions` to understand conventions
3. **Load on demand** — SKILLs support loading detailed documentation on demand to avoid information overload
4. **Auto-activate dependencies** — Some SKILLs automatically activate dependent SKILLs
