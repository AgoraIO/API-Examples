---
name: query-cases
description: >
  Find existing API demo cases in the APIExample-OC project by feature name, API name, or keyword.
  Use this before creating a new case to avoid duplication.
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: iOS
---

# query-cases — APIExample-OC

## When to Use

- User asks "where is the screen sharing example?"
- User wants to find code for a specific Agora SDK API
- Before creating a new case, to confirm it does not already exist

## Quick Search (try this first)

Search the `## Case Index` table in `ARCHITECTURE.md` — it lists every case with its path, key APIs, and description. Most queries can be answered without opening any source file.

## Deep Search (for complex queries)

1. Check `APIExample-OC/ViewController.m` — the `+[MenuSection menus]` method lists all registered cases
2. Source files are at:
   - `APIExample-OC/Examples/Basic/<ExampleName>/<ExampleName>.m`
   - `APIExample-OC/Examples/Advanced/<ExampleName>/<ExampleName>.m`
3. Each case folder contains:
   - `<ExampleName>.h` — class declarations
   - `<ExampleName>.m` — Entry and Main implementations

## Common Query Patterns

| Query | Where to look |
|-------|--------------|
| Feature by name | Case Index — search Description column |
| API by method name (OC syntax) | Case Index — search Key APIs column; or grep `.m` files |
| All cases in a category | Case Index — filter by Path prefix `Basic/` or `Advanced/` |
| Cases using a specific pattern | Grep `.m` files under `Examples/` |

## Output Format

Report results as:
- Case name and file path
- Key APIs demonstrated (OC method syntax)
- One-line description
