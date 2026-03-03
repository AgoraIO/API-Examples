---
inclusion: always
---

# Project Routing Rules

This project is `APIExample-Audio`, using `voice-sdk` — audio APIs only.

## When to Use This Project

- Pure audio scenarios → this project
- Video / screen sharing / beauty / extensions → use `APIExample/`
- Compose UI required → use `APIExample-Compose/`

## Hard Constraints

- Do not share source files with other sub-projects
- Read `AGENTS.md` and `ARCHITECTURE.md` before starting work
- The `.kiro/skills/` directory contains standardized workflows — use them first
- voice-sdk has no video module; any video API call will cause a compile failure
