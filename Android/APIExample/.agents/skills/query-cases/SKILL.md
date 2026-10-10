---
name: query-cases
description: >
  Query and browse existing API example cases in the APIExample Android demo — lists
  cases by group, finds which case demonstrates a specific Agora API, checks sort
  index availability, and resolves display names from string resources. Use when:
  someone asks what cases exist, which APIs are demonstrated, wants to find a case
  by name or API (e.g. takeSnapshot, setClientRole), needs a free sort index before
  adding a new case, or wants to know if a feature is already implemented.
  Keywords: list cases, find case, query cases, @Example, sort index, BASIC, ADVANCED,
  available cases, existing cases, which case, is there a case.
---

## What this skill is for

Use this skill when the question is about what cases already exist in `APIExample/`, which case demonstrates a specific RTC API, whether a BASIC or ADVANCED slot is free, or whether a case is fully registered. This skill answers inventory and registration questions only; it does not edit files.

## Source of truth

1. `APIExample/ARCHITECTURE.md`
2. `APIExample/app/src/main/java/io/agora/api/example/examples/**`
3. `APIExample/app/src/main/res/navigation/nav_graph.xml`
4. `APIExample/app/src/main/res/values/strings.xml`

## Procedure

1. Read `ARCHITECTURE.md` first for the fast case index.
2. Re-scan live `@Example` registrations from source before claiming index availability or whether a case is fully registered.
3. For any registration answer, cross-check the active `@Example` entry against matching `actionId`, nav action, destination in `nav_graph.xml`, and a valid display-name string resource in `strings.xml`.
4. If `ARCHITECTURE.md` is stale or insufficient for API-to-case lookup, fall back to class Javadoc or direct API calls under `app/src/main/java/io/agora/api/example/examples/**`.
5. Resolve display names from `strings.xml`.
6. Return tables or direct factual conclusions only.

## Verify

- Re-check any free-slot or registration claim against live active `@Example` annotations before answering
- Confirm any registration answer is backed by matching `@Example`, nav action or destination, and display-name string resource
- Confirm any reported case name still matches the `strings.xml` resource used by the annotated entry

## Out of scope

- Editing or registering cases
- Reviewing lifecycle quality
- Approving implementation correctness

## Never

- Never report a free ADVANCED index without scanning both `advanced/` and `audio/`
- Never count a commented-out `//@Example` entry as an active registration
- Never rely on filename alone when a subdirectory case may hide the annotated entry point
