---
name: query-cases
description: >
  Find existing API demo cases in the APIExample project by feature name, API name, or keyword.
  Use this before creating a new case to avoid duplication.
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: iOS
---

# query-cases — APIExample

## When to Use

- User asks "where is the screen sharing example?"
- User wants to find code for a specific Agora SDK API
- Before creating a new case, to confirm it does not already exist

## Quick Search (try this first)

Search the `## Case Index` table in `ARCHITECTURE.md` — it lists every case with its path, key APIs, and description. Most queries can be answered without opening any source file.

Example: searching `screenCapture` in the Case Index immediately returns the `ScreenShare` row.

## Deep Search (for complex queries)

For queries like "which cases use multi-channel" or "which cases call joinChannelEx", scan source files:

1. Check `APIExample/ViewController.swift` — the `menus` array lists all registered cases
2. Source files are at:
   - `APIExample/Examples/Basic/<ExampleName>/<ExampleName>.swift`
   - `APIExample/Examples/Advanced/<ExampleName>/<ExampleName>.swift`

## Common Query Patterns

| Query | Where to look |
|-------|--------------|
| Feature by name (e.g. "screen share") | Case Index — search Description column |
| API by method name (e.g. `startScreenCapture`) | Case Index — search Key APIs column |
| All cases in a category | Case Index — filter by Path prefix `Basic/` or `Advanced/` |
| Cases using a specific pattern (e.g. `joinChannelEx`) | Grep source files under `Examples/` |

## Output Format

Report results as:
- Case name and file path
- Key APIs demonstrated
- One-line description
