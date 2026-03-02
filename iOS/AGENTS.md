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
| Add new example | `create-api-example` | Create a new example following project conventions |
| Modify example | Read project `ARCHITECTURE.md` | Understand structure rules |
| Migrate code | `migrate-api-to-project` | Cross-project/architecture/language migration |
| Understand API | Check `SKILL.md` in example directory | Specific API usage guide |

### How to Use SKILLs

SKILLs are located in `.agent/skills/` and can be accessed by agents using their available tools and methods. Each SKILL contains a `SKILL.md` file with detailed documentation.

**SKILL Location Pattern:** `.agent/skills/{skill-name}/SKILL.md`

## SKILL Documentation Index

### Project-level SKILLs (`.agent/skills/`)

#### 1. find-api-example
**Quick example search**

- Locate example code for specific features
- Query example file paths
- Use case: When unsure which example contains the functionality

#### 2. create-api-example
**Steps to create new example**

- Complete creation workflow checklist
- Directory, code, Storyboard, menu registration
- Dependency: Requires understanding `api-example-conventions` first

#### 4. migrate-api-to-project
**Cross-project migration guide**

- Supports cross-architecture (MVC/MVVM/VIPER)
- Supports cross-language (Swift/Objective-C)
- Supports cross-UI framework (UIKit/SwiftUI)

### Example-level SKILLs (in example directories)

Some examples contain `SKILL.md`, providing:
- API call flow and parameters
- Business scenario integration solutions
- Common Q&A

**Example location:** `APIExample/APIExample/Examples/Basic/JoinChannelVideo/SKILL.md`

## Project Structure

See `ARCHITECTURE.md` for the full directory structure of this folder.

## Best Practices

1. **Search first, create later** — Use `find-api-example` to confirm if functionality already exists
2. **Conventions first, implementation later** — Activate `api-example-conventions` to understand conventions
3. **Load on demand** — SKILLs support loading detailed documentation on demand to avoid information overload
4. **Auto-activate dependencies** — Some SKILLs automatically activate dependent SKILLs
