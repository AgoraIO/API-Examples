---
name: upsert-case
description: >
  Add a new Compose API example case or modify an existing one in the
  APIExample-Compose Android demo — updates Composable source, Examples.kt
  registration, string resources, and architecture docs. Use when: adding a new
  Agora RTC API demo screen in Jetpack Compose, modifying an existing case's
  implementation or registration, or porting an APIExample case to Compose while
  preserving Compose-only lifecycle and state rules.
---

## What this skill is for

Use this skill to add or update a case in `APIExample-Compose/`. It owns the full change closure: Composable source, `Examples.kt` registration, localized `strings.xml` updates, and `ARCHITECTURE.md`.

## Source of truth

1. `APIExample-Compose/AGENTS.md`
2. `APIExample-Compose/ARCHITECTURE.md`
3. `APIExample-Compose/app/src/main/java/io/agora/api/example/compose/model/Examples.kt`
4. `APIExample-Compose/app/src/main/java/io/agora/api/example/compose/samples/**`
5. `APIExample-Compose/app/src/main/res/values/strings.xml`
6. [composable-template.kt](references/composable-template.kt)

## Procedure

1. Run `query-cases` first when the target list placement or the closest nearby examples are unclear.
2. Create or update the Composable source, `Examples.kt`, `strings.xml`, and `ARCHITECTURE.md` together as one change set.
3. When the case title is user-facing, update both `res/values/strings.xml` and `res/values-zh/strings.xml` to keep locale coverage aligned with current project practice.
4. Use the reference template above for Compose state, lifecycle, permission, and registration patterns.
5. Keep the public stateful Composable, the private preview/view split, and the `Examples.kt` plus string registration alignment consistent.
6. Treat this skill as the current source of truth for case-creation closure even if older sections in `ARCHITECTURE.md` still describe a smaller file set; update `ARCHITECTURE.md` as part of the same change.

## Verify

- Run `./gradlew assembleDebug` from `APIExample-Compose/`
- Confirm the edited case updates `Examples.kt`, `strings.xml`, `values-zh/strings.xml` when needed, and `ARCHITECTURE.md`
- Use `rememberSaveable` for user inputs, and keep joined status/assigned UID in `remember` with the current engine; a recreated engine starts without an RTC session. Keep `RtcEngine` and other non-serializable objects out of saved state.
- Confirm the public Composable stays stateful and the preview stays on the private view function

## Out of scope

- Adding XML, Fragment, or ViewBinding code
- Skipping docs or registration because the Composable compiles

## Never

- Never use `rememberSaveable` for `RtcEngine`
- Never key teardown to a changing object unless engine creation follows the same ownership changes; `DisposableEffect(rtcEngine)` pairs cleanup with a remembered engine
- Never hardcode `mAreaCode`
