---
name: query-cases
description: >
  Query and browse existing API example cases in the APIExample-Audio Android demo.
  Use when: someone asks what audio cases exist, which case demonstrates a specific
  Agora voice API, whether a BASIC or ADVANCED slot is free, or whether a case is
  fully registered. This project uses voice-sdk only and does not support video APIs.
  Keywords: list cases, find case, query cases, @Example, sort index, BASIC, ADVANCED,
  existing cases, available cases, audio case, voice API.
---

# Query Cases — APIExample-Audio

## What this skill is for

Use this skill when the question is about which audio cases already exist in `APIExample-Audio/`, which case demonstrates a specific voice API, whether a BASIC or ADVANCED slot is free, or whether an audio case is fully registered.

## Source of truth

1. `APIExample-Audio/ARCHITECTURE.md`
2. `APIExample-Audio/app/src/main/java/io/agora/api/example/examples/**`
3. `APIExample-Audio/app/src/main/res/navigation/nav_graph.xml`
4. `APIExample-Audio/app/src/main/res/values/strings.xml`

## Procedure

1. Start with `ARCHITECTURE.md` to answer ordinary "what exists?" or "which case uses API X?" questions quickly.
2. Read the relevant case source under `examples/basic/`, `examples/advanced/`, or `examples/audio/` when the answer depends on live `@Example` values, method usage, or whether the architecture doc is stale.
3. For API-to-case mapping, prefer the class Javadoc and then confirm by scanning for the API call in the case implementation.
4. For registration checks, confirm the case has a live `@Example` annotation, a matching destination and action in `nav_graph.xml`, and corresponding string resources in `strings.xml`.
5. For free-index or collision checks, scan active `@Example` annotations from source immediately before answering; treat `examples/audio/` as part of the `ADVANCED` index namespace.
6. Present results with explicit group, index, case name, file path, and key API when applicable, and call out any stale or inconsistent registration artifacts.

## Verify

- Re-scan source before reporting any free BASIC or ADVANCED index
- Confirm commented-out annotations such as `//@Example` are excluded from active-case results
- Confirm any claimed registration status matches `@Example`, `nav_graph.xml`, and `strings.xml`

## Out of scope

- Editing or registering cases
- Recommending video APIs
- Approving implementation quality

## Never

- Never suggest `enableVideo()`, `setupLocalVideo()`, or `VideoCanvas`
- Never report a free ADVANCED index without scanning both `advanced/` and `audio/`
