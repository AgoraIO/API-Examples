# Agent Guide — Windows

## Project Context

This is the C++ + MFC implementation of Agora RTC SDK examples for Windows. Before making any changes, read `ARCHITECTURE.md` to understand the structural rules.

## Rules

### Follow the Architecture

All work must conform to the rules defined in `ARCHITECTURE.md`:
- Every example is a dialog class inheriting from `CDialogEx` or `CDialog`
- Each example implements `IAgoraRtcEngineEventHandler` interface
- Each example manages its own Agora engine lifecycle
- Message handlers are defined via `BEGIN_MESSAGE_MAP` / `END_MESSAGE_MAP`
- All examples are registered in `CSceneDialog`
- Configuration is managed centrally via `CConfig` class

### Follow the Existing Language and Framework

- Language is C++ — do not introduce C# or other languages
- UI framework is MFC — do not introduce WinForms or WPF
- Use MFC conventions: `C` prefix for classes, `m_` prefix for member variables
- Use message map pattern for event handling — do not introduce modern C++ patterns unless they already exist in the file being modified
- Match the code style, naming, and patterns of existing examples

### Use Example-Level SKILLs

Each example may contain a `SKILL.md` file in its folder. When working on or referencing a specific example:
1. Check whether a `SKILL.md` exists in that example's directory
2. If it exists, read it before making changes — it describes the API usage, call flow, and known constraints
3. If it does not exist, one will be created in the future; proceed using the source code as the reference

**SKILL.md location pattern:** `APIExample/APIExample/[Basic|Advanced]/<ExampleName>/SKILL.md`

### Use Project-Level SKILLs

For broader tasks, use the skills in `.agent/skills/`:

| Task | SKILL |
|------|-------|
| Find an existing example | `find-api-example` |
| Create a new example | `create-api-example` |
