---
name: upsert-case
description: >
  Add a new API example or modify an existing one. Covers both creation and modification scenarios,
  including dialog class structure, registration in APIExampleDlg, localization wiring, and
  ARCHITECTURE.md updates.
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: Windows
---

# Upsert Case Skill — Windows

## When to Use

Use this skill when you need to:
- Create a new API example (case)
- Modify an existing example
- Ensure the example is properly registered and documented

## Applicable Scenarios

### Scenario 1: Create a New Example

**Trigger:** User requests a new API demo (e.g., "Add a screen sharing example")

**Steps:**
1. Determine if the example belongs in `Basic/` or `Advanced/`
2. Create the example folder with PascalCase name
3. Create `.h` and `.cpp` files for the dialog class
4. Register the example in `APIExampleDlg.h` and `APIExampleDlg.cpp`
5. Add scene label wiring in `Language.h`, `stdafx.cpp`, and language `.ini` files
6. Update `ARCHITECTURE.md` Case Index
7. Verify compilation and functionality

### Scenario 2: Modify an Existing Example

**Trigger:** User requests changes to an existing example (e.g., "Update JoinChannelVideo to support token")

**Steps:**
1. Locate the example in `APIExample/APIExample/[Basic|Advanced]/<ExampleName>/`
2. Modify the `.h` and `.cpp` files
3. Update `APIExampleDlg.cpp` if routing or lifecycle hooks changed
4. Update `ARCHITECTURE.md` Case Index if APIs changed
5. Verify compilation and functionality

---

## Files to Modify

### New Example

| File | Action | Notes |
|------|--------|-------|
| `APIExample/APIExample/[Basic\|Advanced]/<ExampleName>/C<ExampleName>Dlg.h` | Create | Dialog class header |
| `APIExample/APIExample/[Basic\|Advanced]/<ExampleName>/C<ExampleName>Dlg.cpp` | Create | Dialog class implementation |
| `APIExample/APIExample/APIExampleDlg.h` | Modify | Add include and dialog member pointer |
| `APIExample/APIExample/APIExampleDlg.cpp` | Modify | Register, create, show, and release the dialog |
| `APIExample/APIExample/Language.h` | Modify | Declare the localized scene label |
| `APIExample/APIExample/stdafx.cpp` | Modify | Initialize the localized scene label in `InitKeyInfomation()` |
| `APIExample/APIExample/en.ini` | Modify | Add English display text |
| `APIExample/APIExample/zh-cn.ini` | Modify | Add Chinese display text |
| `APIExample/APIExample/APIExample.vcxproj` | Modify if needed | Add new source/header files when not using the Visual Studio UI |
| `APIExample/APIExample/APIExample.vcxproj.filters` | Modify if needed | Keep Solution Explorer grouping correct |
| `ARCHITECTURE.md` | Modify | Add entry to Case Index |

### Modify Existing Example

| File | Action | Notes |
|------|--------|-------|
| `APIExample/APIExample/[Basic\|Advanced]/<ExampleName>/C<ExampleName>Dlg.h` | Modify | Update dialog class |
| `APIExample/APIExample/[Basic\|Advanced]/<ExampleName>/C<ExampleName>Dlg.cpp` | Modify | Update implementation |
| `APIExample/APIExample/APIExampleDlg.cpp` | Modify if routing changes | Update show/hide or scene selection behavior if needed |
| `ARCHITECTURE.md` | Modify | Update Case Index if APIs changed |

---

## Step-by-Step Guide

### Step 1: Determine Example Category

- **Basic:** Simple, single-feature examples (JoinChannelVideo, LiveBroadcasting)
- **Advanced:** Complex features, multi-API examples (ScreenShare, CustomVideoCapture)

### Step 2: Create Example Folder

```bash
mkdir APIExample\APIExample\[Basic|Advanced]\<ExampleName>
```

Example folder name must be PascalCase.

### Step 3: Create Dialog Header File

Create `APIExample/APIExample/[Basic|Advanced]/<ExampleName>/C<ExampleName>Dlg.h`

Use the template from `references/example-template.h` as a starting point. Replace `<ExampleName>` with your example name.

### Step 4: Create Dialog Implementation File

Create `APIExample/APIExample/[Basic|Advanced]/<ExampleName>/C<ExampleName>Dlg.cpp`

Use the template from `references/example-template.cpp` as a starting point. Replace `<ExampleName>` with your example name.

### Step 5: Register in APIExampleDlg

Do not edit `CSceneDialog.cpp` for case registration. In this project, scene ownership lives in the main dialog:

- `APIExample/APIExample/APIExampleDlg.h`
  Add the example header include and a member pointer such as `C<ExampleName>Dlg* m_p<ExampleName>Dlg = nullptr;`
- `APIExample/APIExample/APIExampleDlg.cpp`
  Mirror an existing example across:
  - `InitSceneDialog()` to push the localized label into `m_vecBasic` or `m_vecAdvanced`, create the dialog, and position it
  - `CreateScene()` to call the dialog's init/show path when the tree item is selected
  - `ReleaseScene()` to call the dialog's cleanup/hide path when the tree item is left

`InitSceneList()` reads from `m_vecBasic` and `m_vecAdvanced`, so the tree updates automatically once the vectors are populated in `InitSceneDialog()`.

### Step 6: Add localized scene labels

Register the example name used by the tree view:

- Add an `extern wchar_t ...[INFO_LEN];` declaration to `APIExample/APIExample/Language.h`
- Initialize it in `APIExample/APIExample/stdafx.cpp` inside `InitKeyInfomation()`
- Add matching keys to `APIExample/APIExample/en.ini` and `APIExample/APIExample/zh-cn.ini`

Follow the existing naming pattern such as `Basic.JoinChannelVideoByToken` or `Advanced.ScreenCap`.

### Step 7: Update ARCHITECTURE.md

Add a new row to the Case Index table in `ARCHITECTURE.md`:

```markdown
| ExampleName | `[Basic|Advanced]/ExampleName/` | `api1()`, `api2()`, `api3()` | Brief description of what this example demonstrates |
```

**Key APIs column:** List 2-5 core SDK methods used in this example.

### Step 8: Verify

- [ ] Code compiles without errors
- [ ] Example appears in the scene list
- [ ] Example can join channel and receive callbacks
- [ ] `leaveChannel()` and `release()` are called on close
- [ ] UI updates happen on main thread (via message map)
- [ ] Localized labels resolve correctly in both `en.ini` and `zh-cn.ini`
- [ ] ARCHITECTURE.md Case Index is updated

---

## Code Patterns

See `references/` directory for code patterns:
- `lifecycle-pattern.cpp` — Proper engine lifecycle
- `message-map-pattern.cpp` — Message map pattern for thread-safe UI updates
- `event-handler-pattern.cpp` — Event handler pattern

---

## NEVER List

**Do NOT:**
- Forget to call `release()` — this causes engine leaks
- Update UI directly from event handler callbacks — use message map pattern
- Create multiple engine instances in one example — use a single shared instance
- Use C# or other languages — C++ only
- Use WinForms or WPF — MFC only
- Deviate from MFC naming conventions (`C` prefix, `m_` prefix)
- Hardcode App ID or token — use `CConfig`
- Forget to implement `IRtcEngineEventHandler` for event handling
- Leave the channel without calling `leaveChannel()` first
- Register a new case in `CSceneDialog.cpp` — registration lives in `APIExampleDlg.h` and `APIExampleDlg.cpp`
- Modify examples outside the `APIExample/APIExample/[Basic|Advanced]/` structure
- Forget to update `ARCHITECTURE.md` Case Index after adding/modifying an example
- Use modern C++ patterns (lambdas, smart pointers) unless already in the file

---

## Verification Checklist

After completing the upsert, verify:

- [ ] Example folder is in correct location (`APIExample/APIExample/[Basic|Advanced]/<ExampleName>/`)
- [ ] Header file is named `C<ExampleName>Dlg.h` (with C prefix)
- [ ] Implementation file is named `C<ExampleName>Dlg.cpp`
- [ ] Dialog class inherits from `CDialogEx` or `CDialog`
- [ ] Event handler implements `IRtcEngineEventHandler`
- [ ] Message map properly defined with `BEGIN_MESSAGE_MAP` / `END_MESSAGE_MAP`
- [ ] Example is registered in `APIExampleDlg.h` and `APIExampleDlg.cpp`
- [ ] Scene label is declared in `Language.h` and initialized in `stdafx.cpp`
- [ ] Scene label has entries in both `en.ini` and `zh-cn.ini`
- [ ] `InitializeAgoraEngine()` creates engine with correct config
- [ ] `JoinChannel()` uses token from `CConfig`
- [ ] `LeaveChannel()` and `release()` are called in `PostNcDestroy()`
- [ ] All engine events posted to main thread via `PostMessage()`
- [ ] `APIExample.vcxproj` and `.filters` include the new files when they were added outside the IDE
- [ ] `ARCHITECTURE.md` Case Index includes new/updated example
- [ ] Code compiles without warnings or errors
- [ ] Example appears in the scene list
- [ ] Example can successfully join channel and receive callbacks
- [ ] Example properly cleans up resources on close

---

## References

- **Template files:** See `references/` directory for C++ code templates
- **Existing examples:** Review `APIExample/APIExample/Basic/JoinChannelVideoByToken/` for reference implementation
- **SDK documentation:** Refer to Agora RTC SDK for Windows documentation for API details
- **MFC Documentation:** [Microsoft Foundation Classes](https://docs.microsoft.com/en-us/cpp/mfc/mfc-desktop-applications)
