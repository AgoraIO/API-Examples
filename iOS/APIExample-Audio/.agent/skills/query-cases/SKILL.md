---
name: query-cases
description: >
  Find existing audio API demo cases in the APIExample-Audio project by feature name, API name, or keyword.
  Use this before creating a new case to avoid duplication.
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: iOS
---

# query-cases — APIExample-Audio

## When to Use

- User asks "where is the voice changer example?"
- User wants to find code for a specific Agora audio API
- Before creating a new case, to confirm it does not already exist

## Quick Search (try this first)

Search the `## Case Index` table in `ARCHITECTURE.md` — it lists every case with its path, key APIs, and description. This project has 11 cases total, so the Case Index is the fastest lookup.

## Deep Search (for complex queries)

1. Check `APIExample-Audio/ViewController.swift` — the `menus` array lists all registered cases
2. Source files are at:
   - `APIExample-Audio/Examples/Basic/<ExampleName>/<ExampleName>.swift`
   - `APIExample-Audio/Examples/Advanced/<ExampleName>/<ExampleName>.swift`

## Common Query Patterns

| Query | Where to look |
|-------|--------------|
| Feature by name (e.g. "spatial audio") | Case Index — search Description column |
| API by method name (e.g. `setAudioProfile`) | Case Index — search Key APIs column |
| All basic cases | Case Index — filter by Path prefix `Basic/` |
| Cases using custom audio | Case Index — search for `CustomAudio` or `pushExternal` |

## Output Format

Report results as:
- Case name and file path
- Key APIs demonstrated
- One-line description
