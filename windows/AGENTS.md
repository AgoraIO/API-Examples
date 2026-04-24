# AGENTS.md — Windows

## Project Context

This is the C++ + MFC implementation of Agora RTC SDK examples for Windows. Before making any changes, read `ARCHITECTURE.md` to understand the structural rules.

## Build Commands

```bash
# Build using Visual Studio (from command line)
cd windows/APIExample
msbuild APIExample.sln /p:Configuration=Release /p:Platform=x64

# Or open in Visual Studio and build manually
start APIExample.sln
```

## App ID Configuration

Configure your Agora App ID in `APIExample/APIExample/CConfig.h` and `CConfig.cpp`:

```cpp
// CConfig.h
class CConfig {
public:
    static const char* GetAppId() { return "<#YOUR_APP_ID#>"; }
    static const char* GetToken(const char* channelName) { return "<#YOUR_TOKEN#>"; }
};
```

## Architecture Red Lines

**Do NOT:**
- Introduce C# or other languages — use C++ only
- Use WinForms or WPF — use MFC only
- Deviate from MFC naming conventions (`C` prefix for classes, `m_` prefix for members)
- Use modern C++ patterns (lambdas, smart pointers) unless already present in the file being modified
- Forget to call `leaveChannel()` and `release()` when closing an example
- Update UI from background threads — always post messages to the main thread
- Share engine instances between examples — each example manages its own lifecycle
- Forget to implement `IAgoraRtcEngineEventHandler` for event handling

## Rules

### Follow the Architecture

All work must conform to the rules defined in `ARCHITECTURE.md`:
- Every example is a dialog class inheriting from `CDialogEx` or `CDialog`
- Each example implements `IAgoraRtcEngineEventHandler` interface
- Each example manages its own Agora engine lifecycle
- Message handlers are defined via `BEGIN_MESSAGE_MAP` / `END_MESSAGE_MAP`
- All examples are registered in `APIExampleDlg.h` and `APIExampleDlg.cpp`
- Configuration is managed centrally via `CConfig` class

### Follow the Existing Language and Framework

- Language is C++ — do not introduce C# or other languages
- UI framework is MFC — do not introduce WinForms or WPF
- Use MFC conventions: `C` prefix for classes, `m_` prefix for member variables
- Use message map pattern for event handling — do not introduce modern C++ patterns unless they already exist in the file being modified
- Match the code style, naming, and patterns of existing examples

### Use Project-Level SKILLs

For broader tasks, use the skills in `.agent/skills/`:

| Task | Skill | When to use |
|------|-------|-------------|
| Add or modify an example | `.agent/skills/upsert-case/` | Need to create a new API demo or update an existing one |
| Code review | `.agent/skills/review-case/` | Review example code for lifecycle, thread safety, and convention compliance |
