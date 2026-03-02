---
name: create-api-example
description: Step-by-step checklist for adding new examples to Windows APIExample project, including code and API usage guide
compatibility: [Cursor, Kiro, Windsurf, Claude, Copilot]
license: MIT
metadata:
  author: APIExample Team
  version: 1.0.0
  platform: Windows
---

# Create New Example

## Checklist

1. **Determine name and category** (PascalCase, Basic/Advanced)
2. **Create directory and files**
   - `APIExample/APIExample/[Basic|Advanced]/<ExampleName>/`
   - `C<ExampleName>Dlg.h` - Dialog header
   - `C<ExampleName>Dlg.cpp` - Dialog implementation
   - `SKILL.md` - API usage guide
3. **Register scene** - Add example to `CSceneDialog`
4. **Add to Visual Studio project**

## Create SKILL.md

Each example needs a `SKILL.md` to guide users on using the API in business scenarios.

**Use Template:** Copy `.agent/skills/create-api-example/assets/SKILL-template.md` and fill in content

**Core Content:**
- Feature description
- API call flow (5-7 steps with code)
- Key event callbacks
- Common questions (3-6 items)
- Reference documentation links

## Detailed Steps

When you need the complete guide, read `references/creation-guide.md`

## References

- Reference implementation examples in `APIExample/APIExample/Basic/` and `Advanced/`
- `SKILL.md` reference examples
- Read `ARCHITECTURE.md` first to understand conventions
