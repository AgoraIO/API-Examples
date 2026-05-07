---
name: upsert-case
description: >
  Add a new API example case or modify an existing one in the APIExample Android demo —
  creates or updates Fragment class, XML layout, string resources, nav_graph registration,
  and architecture docs. Use when: adding a new Agora RTC API demo screen, modifying an
  existing case's implementation or registration, implementing a new feature example in
  Java + XML layouts, registering a new case via @Example annotation, subclassing
  BaseFragment for a new demo screen, or updating an existing case's strings, layout, nav
  entry, or architecture docs. Keywords: add case, modify case, update case, new fragment,
  nav_graph, @Example, BaseFragment, APIExample, new screen, demo case, RTC API example.
---

## What this skill is for

Use this skill to add a new case or update an existing case in `APIExample/`. It owns the full change closure: case source, XML layout, strings, `nav_graph.xml`, and `ARCHITECTURE.md`.

## Source of truth

1. `APIExample/AGENTS.md`
2. `APIExample/ARCHITECTURE.md`
3. `APIExample/app/src/main/java/io/agora/api/example/examples/**`
4. `APIExample/app/src/main/res/navigation/nav_graph.xml`
5. `APIExample/app/src/main/res/values/strings.xml`
6. `APIExample/.agents/skills/upsert-case/references/fragment-template.java`

## Procedure

1. Run `query-cases` first when index or placement is unknown.
2. Create or update the Fragment, layout, strings, navigation entries, and `ARCHITECTURE.md`.
3. Use the reference template for lifecycle, reporting, and private-cloud guards.
4. Keep `@Example`, action ID, destination ID, and string resources aligned.
5. Treat the reference template as a pattern source, not a drop-in class.

## Verify

- Run `./gradlew assembleDebug` from `APIExample/`
- Re-open the edited file list and confirm the case closure touched every required file category

## Out of scope

- Auditing the final implementation as a reviewer
- Audio-only cases that belong in `APIExample-Audio/`
- Compose-based cases that belong in `APIExample-Compose/`

## Never

- Never omit `setParameters(...)`
- Never skip the `getPrivateCloudConfig()` null-check
- Never call `RtcEngine.destroy()` directly on the main thread
- Never stop at “it compiles” without updating registration and `ARCHITECTURE.md`
