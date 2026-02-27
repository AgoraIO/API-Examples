---
name: create-api-example
description: Step-by-step checklist for adding new examples to APIExample project, including code, Storyboard, and API usage guide
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
---

# Create New Example

## Checklist

1. **Determine name and category** (PascalCase, Basic/Advanced)
2. **Create directory and files**
   - `APIExample/Examples/[Basic|Advanced]/<ExampleName>/`
   - `<ExampleName>.swift` - Entry and Main classes
   - `Base.lproj/<ExampleName>.storyboard`
   - `SKILL.md` - API usage guide
3. **Register menu** - Add `MenuItem` in `ViewController.swift`
4. **Add to Xcode target**

## Create SKILL.md

Each example needs a `SKILL.md` to guide users on using the API in business scenarios.

**Use Template:** Copy `.agent/skills/create-api-example/assets/SKILL-template.md` and fill in content  
**Reference Example:** `JoinChannelVideo/SKILL.md`

**Core Content:**
- Feature description
- API call flow (5-7 steps with code)
- Key event callbacks
- Common questions (3-6 items)
- Reference documentation links

## Detailed Steps

When you need the complete guide, read `references/creation-guide.md`

## References

- `JoinChannelVideo.swift` - Entry/Main reference implementation
- `JoinChannelVideo/SKILL.md` - SKILL file reference
- Activate `api-example-conventions` first to understand conventions
