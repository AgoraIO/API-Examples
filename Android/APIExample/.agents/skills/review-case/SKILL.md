---
name: review-case
description: >
  Review an existing case implementation against project-specific red lines
  and coding standards. Use after implementing or modifying a case.
  Use when: reviewing a case for correctness, checking red-line compliance,
  verifying lifecycle and threading patterns, auditing an existing Fragment.
  Keywords: review, audit, check, red lines, lifecycle, threading, compliance.
---

## What this skill is for

Use this skill after a case has been created or modified in `APIExample/`. It checks project red lines, registration closure, and minimum executable verification before the case is treated as review-ready.

## Source of truth

1. `APIExample/AGENTS.md`
2. `APIExample/ARCHITECTURE.md`
3. The target case source file
4. `APIExample/app/src/main/res/navigation/nav_graph.xml`
5. `APIExample/app/src/main/res/values/strings.xml`

## Procedure

1. Audit lifecycle, permission, threading, and registration rules.
2. Check `@Example`, `nav_graph.xml`, `strings.xml`, and `ARCHITECTURE.md` for closure.
3. Run the minimum build verification command.
4. Report findings first, then verification results, then explicit unverified items.

## Verify

- Run `./gradlew assembleDebug` from `APIExample/`
- Confirm the target case still appears to satisfy `@Example` + action + destination + string alignment

## Out of scope

- Re-implementing the case from scratch
- Making silent code fixes during review
- Claiming runtime behavior was validated if only compile checks ran

## Never

- Never approve direct `RtcEngine.destroy()` on the main thread
- Never approve missing `leaveChannel()` before destroy
- Never skip the build command
