---
name: find-api-example
description: Quickly locate example locations and file paths for specific features in Windows APIExample project
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: Windows
---

# Find API Examples

## Task Objective

Quickly locate example code locations for specific features on Windows platform.

## Scope

This SKILL covers the Windows project:

| Project | Tech Stack | Menu File |
|---------|------------|-----------|
| `APIExample/` | C++ + MFC | `APIExample/APIExample/CSceneDialog.cpp` |

## When to Use

- User asks "How to implement XXX", "Is there an XXX example", "Where is XXX"
- Need to find reference code for specific features
- Unsure which example implements a certain feature

## Search Process

### Step 1: Keyword Matching

Extract keywords from user's question and find corresponding example names.

**Examples:**
- "How to implement screen sharing?" → Keyword: screen share → Example name: `ScreenShare`
- "How to join a channel?" → Keyword: join channel → Example name: `JoinChannelVideo`

### Step 2: Dynamic Verification

Read the scene dialog file `APIExample/APIExample/CSceneDialog.cpp`, search for the example name in the scene list.

### Step 3: Infer Path

Infer complete path based on example name:

**Path Format:**
```
APIExample/APIExample/[Basic|Advanced]/[ExampleName]/
```

**Examples:**
- `JoinChannelVideo` → `APIExample/APIExample/Basic/JoinChannelVideo/`
- `ScreenShare` → `APIExample/APIExample/Advanced/ScreenShare/`

**Determine Basic or Advanced:**
- Basic features (join channel, audio/video call) → Basic
- Advanced features (screen sharing, custom source, effects) → Advanced
- When unsure, read scene dialog to check which section it's in

### Step 4: Verify Files

Check if key files exist in the directory:

- `C<ExampleName>Dlg.h` — Header file
- `C<ExampleName>Dlg.cpp` — Implementation file
- `SKILL.md` — API usage guide (optional)

## Output Format

```
**Example:** [Feature Name]
**Project:** Windows APIExample (C++ + MFC)
**Path:** [Complete Path]
**Files:** [Main File List]
**Dialog Class:** C<ExampleName>Dlg
```

## Common Query Examples

| User Question | Keyword | Example Name | Dialog Class |
|---------------|---------|--------------|--------------|
| How to join a channel? | join channel | `JoinChannelVideo` | `CJoinChannelVideoDlg` |
| How to do screen sharing? | screen share | `ScreenShare` | `CScreenShareDlg` |
| Custom video source | custom video | `CustomVideoSourcePush` | `CCustomVideoSourcePushDlg` |
| Voice changer effect | voice changer | `VoiceChanger` | `CVoiceChangerDlg` |

## Notes

1. **Prioritize dynamic search** — Read scene dialog directly to get latest example list
2. **Path structure** — Examples are in `APIExample/APIExample/[Basic|Advanced]/`
3. **Naming convention** — Dialog classes use `C` prefix and `Dlg` suffix
4. **SKILL documentation** — Some examples include `SKILL.md` providing detailed API usage guide

## Troubleshooting

**If example not found:**
1. Check if keywords are accurate
2. Read scene dialog to see complete list
3. Check example name spelling (case-sensitive)

**If path doesn't exist:**
1. Verify example name is correct
2. Check Basic/Advanced category
3. Confirm dialog class name matches folder name
