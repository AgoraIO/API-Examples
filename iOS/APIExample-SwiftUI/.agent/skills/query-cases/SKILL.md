---
name: query-cases
description: >
  Find existing API demo cases in the APIExample-SwiftUI project by feature name, API name, or keyword.
  Use this before creating a new case to avoid duplication.
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: iOS
---

# query-cases — APIExample-SwiftUI

## When to Use

- User asks "where is the screen sharing example?"
- User wants to find code for a specific Agora SDK API
- Before creating a new case, to confirm it does not already exist

## Quick Search (try this first)

Search the `## Case Index` table in `ARCHITECTURE.md` — it lists every case with its path, key APIs, and description. Most queries can be answered without opening any source file.

## Deep Search (for complex queries)

1. Check `APIExample-SwiftUI/ContentView.swift` — the `menus` array lists all registered cases
2. Source files are at:
   - `APIExample-SwiftUI/Examples/Basic/<ExampleName>/`
   - `APIExample-SwiftUI/Examples/Advanced/<ExampleName>/`
3. Each case folder contains:
   - `<ExampleName>.swift` — Entry and Main SwiftUI views
   - `<ExampleName>RTC.swift` — engine lifecycle and delegate callbacks

## Common Query Patterns

| Query | Where to look |
|-------|--------------|
| Feature by name | Case Index — search Description column |
| API by method name | Case Index — search Key APIs column; or grep `*RTC.swift` files |
| All cases in a category | Case Index — filter by Path prefix `Basic/` or `Advanced/` |
| Cases using a specific pattern | Grep `*RTC.swift` files under `Examples/` |

## Output Format

Report results as:
- Case name and folder path
- Key APIs demonstrated
- One-line description
