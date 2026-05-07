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
- Confirm engine lifecycle and cleanup are owned by `DisposableEffect(lifecycleOwner)`, and `rememberSaveable` / `remember` boundaries still match current project practice

## Out of scope

- Rewriting the case during review by default
- Treating snapshot-safe state mutation as permission to show Toast or Dialog off the main thread

## Never

- Never approve `DisposableEffect(Unit)` for case cleanup
- Never approve `RtcEngine` stored in `rememberSaveable`
- Never skip the build command
