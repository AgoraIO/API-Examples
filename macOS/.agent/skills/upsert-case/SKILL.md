---
name: upsert-case
description: >
  Add a new API example or modify an existing one. Covers both creation and modification scenarios,
  including file structure, per-example storyboard creation, registration, and ARCHITECTURE.md updates.
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: macOS
---

# Upsert Case Skill — macOS

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
3. Create the Swift implementation file
4. Create the example storyboard
5. Register the example in `ViewController.swift`
6. Update `ARCHITECTURE.md` Case Index
7. Verify compilation and functionality

### Scenario 2: Modify an Existing Example

**Trigger:** User requests changes to an existing example (e.g., "Update JoinChannelVideo to support token")

**Steps:**
1. Locate the example in `APIExample/Examples/[Basic|Advanced]/<ExampleName>/`
2. Modify the Swift file
3. Modify the storyboard if outlets, actions, or controller identifiers changed
4. Update `ARCHITECTURE.md` Case Index if APIs changed
5. Verify compilation and functionality

---

## Files to Modify

### New Example

| File | Action | Notes |
|------|--------|-------|
| `APIExample/Examples/[Basic\|Advanced]/<ExampleName>/<ExampleName>.swift` | Create | Main implementation file |
| `APIExample/Examples/[Basic\|Advanced]/<ExampleName>/Base.lproj/<ExampleName>.storyboard` | Create | Example UI and controller identifier |
| `APIExample/ViewController.swift` | Modify | Register example in menu/list |
| `ARCHITECTURE.md` | Modify | Add entry to Case Index |

### Modify Existing Example

| File | Action | Notes |
|------|--------|-------|
| `APIExample/Examples/[Basic\|Advanced]/<ExampleName>/<ExampleName>.swift` | Modify | Update implementation |
| `APIExample/Examples/[Basic\|Advanced]/<ExampleName>/Base.lproj/<ExampleName>.storyboard` | Modify if needed | Keep outlets and controller identifiers aligned |
| `ARCHITECTURE.md` | Modify | Update Case Index if APIs changed |

---

## Step-by-Step Guide

### Step 1: Determine Example Category

- **Basic:** Simple, single-feature examples (JoinChannelVideo, JoinChannelAudio)
- **Advanced:** Complex features, multi-API examples (ScreenShare, CustomVideoSource)

### Step 2: Create Example Folder

```bash
mkdir -p APIExample/Examples/[Basic|Advanced]/<ExampleName>
```

Example folder name must be PascalCase and match the class name.

### Step 3: Create Swift Implementation

Create `APIExample/Examples/[Basic|Advanced]/<ExampleName>/<ExampleName>.swift`

Use the template from `references/example-template.swift` as a starting point. Replace `<ExampleName>` with your example name.

### Step 4: Create the Example Storyboard

Create `APIExample/Examples/[Basic|Advanced]/<ExampleName>/Base.lproj/<ExampleName>.storyboard`.

The storyboard name must match the value passed to `NSStoryboard(name:bundle:)` from `ViewController.swift`, and the main controller identifier in the storyboard must match the `controller` field in `MenuItem`.

### Step 5: Register in ViewController

Edit `APIExample/ViewController.swift` and add the example to the menu/list:

```swift
MenuItem(name: "Example Name".localized,
         identifier: "menuCell",
         controller: "<ExampleName>",
         storyboard: "<ExampleName>")
```

Place it in the correct section (Basic / Advanced).

### Step 6: Update ARCHITECTURE.md

Add a new row to the Case Index table in `ARCHITECTURE.md`:

```markdown
| ExampleName | `Examples/[Basic|Advanced]/ExampleName/` | `api1()`, `api2()`, `api3()` | Brief description of what this example demonstrates |
```

**Key APIs column:** List 2-5 core SDK methods used in this example.

### Step 7: Verify

- [ ] Code compiles without errors
- [ ] Example appears in the menu/list
- [ ] Example can join channel and receive callbacks
- [ ] `leaveChannel()` and `destroy()` are called on close
- [ ] UI updates happen on main thread
- [ ] Storyboard loads with the expected controller identifier
- [ ] ARCHITECTURE.md Case Index is updated

---

## Code Patterns

See `references/` directory for code patterns:
- `lifecycle-pattern.swift` — Proper engine lifecycle
- `thread-safety-pattern.swift` — Thread-safe UI updates

---

## NEVER List

**Do NOT:**
- Forget to call `destroy()` — this causes engine leaks
- Update UI directly from delegate callbacks — always use `DispatchQueue.main.async`
- Create multiple engine instances in one example — use a single shared instance
- Use Objective-C files — Swift only
- Use UIKit or SwiftUI — Cocoa (AppKit) only
- Hardcode App ID or token — use `KeyCenter`
- Forget to implement `AgoraRtcEngineDelegate` for event handling
- Leave the channel without calling `leaveChannel()` first
- Modify examples outside the `APIExample/Examples/[Basic|Advanced]/` structure
- Register a new menu item without also creating the per-example storyboard under the same example folder
- Forget to update `ARCHITECTURE.md` Case Index after adding/modifying an example

---

## Verification Checklist

After completing the upsert, verify:

- [ ] Example folder is in correct location (`APIExample/Examples/[Basic|Advanced]/<ExampleName>/`)
- [ ] Swift file is named `<ExampleName>.swift` (PascalCase)
- [ ] Class name is `<ExampleName>Main` and extends `BaseViewController`
- [ ] Storyboard exists at `APIExample/Examples/[Basic|Advanced]/<ExampleName>/Base.lproj/<ExampleName>.storyboard`
- [ ] Storyboard identifier matches the `controller` value registered in `ViewController.swift`
- [ ] Example is registered in `ViewController.swift`
- [ ] `initializeAgoraEngine()` creates engine with correct config
- [ ] `joinChannel()` uses token from `KeyCenter`
- [ ] `leaveChannel()` and `destroy()` are called in `viewWillClose()`
- [ ] All delegate callbacks dispatch UI updates to main thread
- [ ] `ARCHITECTURE.md` Case Index includes new/updated example
- [ ] Code compiles without warnings or errors
- [ ] Example appears in the application menu/list
- [ ] Example can successfully join channel and receive callbacks
- [ ] Example properly cleans up resources on close

---

## References

- **Template files:** See `references/` directory for Swift code templates
- **Existing examples:** Review `APIExample/Examples/Basic/JoinChannelVideo/` for reference implementation
- **SDK documentation:** Refer to Agora RTC SDK for macOS documentation for API details
