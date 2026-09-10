---
name: review-case
description: >
  Review an existing Compose case implementation against project-specific red
  lines and coding standards. Use after implementing or modifying a case. Use
  when: reviewing a Compose case for correctness, checking red-line compliance,
  verifying lifecycle and state patterns, auditing an existing Composable.
  Keywords: review, audit, check, red lines, lifecycle, state, compliance,
  Compose.
---

## Pending Permission and Token Requests

Review the complete lifecycle against [the creation template](../upsert-case/SKILL.md).
All request/engine ownership transitions must run on main. Freeze channel/UID and request
generation before requesting permission or Token; before joining, check that the generation
and engine identity are still current. Leave invalidates pending work even before joined;
destroy also clears the engine. Repeated cleanup must not destroy another case's engine.
Weak references alone do not invalidate a request when its owner remains alive.

Exercise delayed permission and Token responses after leave/destroy, repeated cleanup,
rapid reopen and out-of-order responses. Only the current request may join. Also check
permission denial, absent required Token and a nonzero join result. Never log credentials.

## What this skill is for

Use this skill after a Compose case has been created or modified in `APIExample-Compose/`. It checks lifecycle, state, registration, and build verification before the case is treated as review-ready.

## Source of truth

1. `APIExample-Compose/AGENTS.md`
2. `APIExample-Compose/ARCHITECTURE.md`
3. The target case source file
4. `APIExample-Compose/app/src/main/java/io/agora/api/example/compose/model/Examples.kt`
5. `APIExample-Compose/app/src/main/res/values/strings.xml`

## Procedure

1. Audit lifecycle, permission, state, and threading-sensitive UI rules in the target Composable source.
2. Check `Examples.kt`, `strings.xml`, and `ARCHITECTURE.md` for registration and documentation closure.
3. Run the minimum build verification command from the Compose project.
4. Report findings first, then verification results, then explicit unverified items if any required verification could not be completed.

## Verify

- Run `./gradlew assembleDebug` from `APIExample-Compose/`
- Confirm `Examples.kt`, `strings.xml`, and `ARCHITECTURE.md` are aligned
- Confirm the effect cleans up the engine it captured, its keys match engine ownership, and live session state resets with engine recreation. A constant `Unit` key also disposes on leaving composition; do not reject it solely for being constant.

## Out of scope

- Rewriting the case during review by default
- Treating snapshot-safe state mutation as permission to show Toast or Dialog off the main thread

## Never

- Never approve effect-key changes that destroy an engine while the case continues to reuse that same remembered instance
- Never approve `RtcEngine` stored in `rememberSaveable`
- Never skip the build command
