---
name: find-api-example
description: Quickly locate example locations and file paths for specific features in iOS APIExample projects
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 2.0.0
  platform: iOS
---

# Find API Examples

## Task Objective

Quickly locate example code locations for specific features on iOS platform.

## Scope

This SKILL covers the following iOS projects:

| Project | Tech Stack | Menu File |
|---------|------------|-----------|
| `APIExample/` | Swift + UIKit | `APIExample/APIExample/ViewController.swift` |
| `APIExample-Audio/` | Swift + UIKit | `APIExample-Audio/APIExample-Audio/ViewController.swift` |
| `APIExample-SwiftUI/` | Swift + SwiftUI | `APIExample-SwiftUI/APIExample-SwiftUI/ContentView.swift` |
| `APIExample-OC/` | Objective-C + UIKit | `APIExample-OC/APIExample-OC/ViewController.m` |

## When to Use

- User asks "How to implement XXX", "Is there an XXX example", "Where is XXX"
- Need to find reference code for specific features
- Unsure which example implements a certain feature

## Search Process

### Step 1: Keyword Matching

Extract keywords from user's question and find corresponding example names in the mapping table in `references/example-index.md`.

**Examples:**
- "How to implement beauty effects?" → Keyword: beauty → Example name: `AgoraBeauty`
- "How to do screen sharing?" → Keyword: screen share → Example name: `ScreenShare`

### Step 2: Select Project

Choose appropriate project based on user needs:

| Need | Recommended Project |
|------|---------------------|
| Default/Most complete | `APIExample/` |
| Pure audio features | `APIExample-Audio/` |
| SwiftUI implementation | `APIExample-SwiftUI/` |
| Objective-C implementation | `APIExample-OC/` |

### Step 3: Dynamic Verification

Read the menu file of the corresponding project, search for the `MenuItem` corresponding to the example name, and get the `storyboard` field.

**UIKit Project Example:**
```swift
// Search in ViewController.swift
MenuItem(name: "Agora Beauty", storyboard: "AgoraBeauty", controller: "")
```

**SwiftUI Project Example:**
```swift
// Search in ContentView.swift
NavigationLink("Agora Beauty", destination: AgoraBeautyView())
```

### Step 4: Infer Path

Infer complete path based on project type and example name:

**Path Format:**
```
[ProjectName]/[ProjectName]/Examples/[Basic|Advanced]/[ExampleName]/
```

**Examples:**
- `AgoraBeauty` → `APIExample/APIExample/Examples/Advanced/AgoraBeauty/`
- `JoinChannelVideo` → `APIExample/APIExample/Examples/Basic/JoinChannelVideo/`

**Determine Basic or Advanced:**
- Basic features (join channel, audio/video call) → Basic
- Advanced features (beauty, custom source, effects) → Advanced
- When unsure, read menu file to check which section it's in

### Step 5: Verify Files

Check if key files exist in the directory:

**UIKit Project:**
- `[ExampleName].swift` — Swift code (Entry and Main classes)
- `Base.lproj/[ExampleName].storyboard` — Storyboard file
- `SKILL.md` — API usage guide (optional)

**SwiftUI Project:**
- `[ExampleName]View.swift` — SwiftUI View
- `[ExampleName]ViewModel.swift` — ViewModel (optional)

**Objective-C Project:**
- `[ExampleName].h` — Header file
- `[ExampleName].m` — Implementation file
- `Base.lproj/[ExampleName].storyboard` — Storyboard file

## Output Format

```
**Example:** [Feature Name]
**Project:** [Project Name] ([Tech Stack])
**Path:** [Complete Path]
**Files:** [Main File List]
**Storyboard:** [Storyboard Name] (if applicable)
```

**If other projects also have this example, add note:**
```
**Other Implementations:**
- SwiftUI: [Path]
- Objective-C: [Path]
- Audio: [Path]
```

## Common Query Examples

| User Question | Keyword | Example Name | Recommended Project |
|---------------|---------|--------------|---------------------|
| How to implement beauty effects? | beauty | `AgoraBeauty` | APIExample |
| How to do screen sharing? | screen share | `ScreenShare` | APIExample, SwiftUI, OC |
| Custom video source | custom video | `CustomVideoSourcePush` | APIExample, OC |
| Voice changer effect | voice changer | `VoiceChanger` | APIExample-Audio |
| Join channel | join channel | `JoinChannelVideo` | All projects |
| SwiftUI implementation | swiftui | Any feature | APIExample-SwiftUI |

## Notes

1. **Prioritize dynamic search** — Read menu files directly to get latest example list, index file is only for keyword matching
2. **Path updated** — Examples are in `[Project]/[Project]/Examples/` not `[Project]/Examples/`
3. **Project differences** — Different projects contain different numbers of examples and implementations
4. **SKILL documentation** — Some examples include `SKILL.md` providing detailed API usage guide
5. **Audio features** — Pure audio features prioritize `APIExample-Audio/`

## Troubleshooting

**If example not found:**
1. Check if keywords are accurate
2. Try searching in other projects
3. Read menu file to see complete list
4. Check example name spelling (case-sensitive)

**If path doesn't exist:**
1. Verify project name is correct
2. Check Basic/Advanced category
3. Confirm example name matches Storyboard name
