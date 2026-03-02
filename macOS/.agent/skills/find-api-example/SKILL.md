---
name: find-api-example
description: Quickly locate example locations and file paths for specific features in macOS APIExample project
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: macOS
---

# Find API Examples

## Task Objective

Quickly locate example code locations for specific features on macOS platform.

## Scope

This SKILL covers the macOS project:

| Project | Tech Stack | Menu File |
|---------|------------|-----------|
| `APIExample/` | Swift + Cocoa | `APIExample/ViewController.swift` |

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

Read the menu file `APIExample/ViewController.swift`, search for the example name in the menu structure.

### Step 3: Infer Path

Infer complete path based on example name:

**Path Format:**
```
APIExample/Examples/[Basic|Advanced]/[ExampleName]/
```

**Examples:**
- `JoinChannelVideo` → `APIExample/Examples/Basic/JoinChannelVideo/`
- `ScreenShare` → `APIExample/Examples/Advanced/ScreenShare/`

**Determine Basic or Advanced:**
- Basic features (join channel, audio/video call) → Basic
- Advanced features (screen sharing, custom source, effects) → Advanced
- When unsure, read menu file to check which section it's in

### Step 4: Verify Files

Check if key files exist in the directory:

- `[ExampleName].swift` — Swift code
- `Base.lproj/[ExampleName].storyboard` — Storyboard file (if applicable)
- `SKILL.md` — API usage guide (optional)

## Output Format

```
**Example:** [Feature Name]
**Project:** macOS APIExample (Swift + Cocoa)
**Path:** [Complete Path]
**Files:** [Main File List]
```

## Common Query Examples

| User Question | Keyword | Example Name |
|---------------|---------|--------------|
| How to join a channel? | join channel | `JoinChannelVideo` |
| How to do screen sharing? | screen share | `ScreenShare` |
| Custom video source | custom video | `CustomVideoSourcePush` |
| Voice changer effect | voice changer | `VoiceChanger` |

## Notes

1. **Prioritize dynamic search** — Read menu files directly to get latest example list
2. **Path structure** — Examples are in `APIExample/Examples/[Basic|Advanced]/`
3. **SKILL documentation** — Some examples include `SKILL.md` providing detailed API usage guide

## Troubleshooting

**If example not found:**
1. Check if keywords are accurate
2. Read menu file to see complete list
3. Check example name spelling (case-sensitive)

**If path doesn't exist:**
1. Verify example name is correct
2. Check Basic/Advanced category
3. Confirm example name matches folder name
