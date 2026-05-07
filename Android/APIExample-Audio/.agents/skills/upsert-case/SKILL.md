## What this skill is for

Use this skill to add or update a case in `APIExample-Audio/`. It owns the full change closure: case source, XML layout, strings, `nav_graph.xml`, and `ARCHITECTURE.md`, while preserving the voice-SDK-only constraint.

## Source of truth

1. `APIExample-Audio/AGENTS.md`
2. `APIExample-Audio/ARCHITECTURE.md`
3. `APIExample-Audio/app/src/main/java/io/agora/api/example/examples/**`
4. `APIExample-Audio/app/src/main/res/navigation/nav_graph.xml`
5. `APIExample-Audio/app/src/main/res/values/strings.xml`
6. `APIExample-Audio/.agents/skills/upsert-case/references/fragment-template.java`

## Procedure

1. Run `query-cases` first when the case index, group placement, or nearby examples are unknown.
2. Create or update the Fragment, layout, strings, nav entries, and `ARCHITECTURE.md` together so the case stays fully registered.
3. Use `APIExample-Audio/.agents/skills/upsert-case/references/fragment-template.java` as the reference for engine lifecycle, usage reporting, private-cloud setup, and voice-only guardrails.
4. Keep `@Example`, action ID, destination ID, string resources, and audio-only constraints aligned across the implementation.

## Verify

- Run `./gradlew assembleDebug` from `APIExample-Audio/`
- Confirm the edited case avoids video APIs and keeps `@Example`, layout, strings, nav, and docs aligned

## Out of scope

- Reviewing the finished implementation as an independent reviewer
- Omitting docs or registration updates because the fragment compiles

## Never

- Never add `publishCameraTrack`
- Never call `enableVideo()`
- Never skip `ARCHITECTURE.md`
