---
name: query-cases
description: >
  Query and browse existing API example cases in the APIExample-Compose Android demo —
  lists cases by group, finds which Composables demonstrate a specific Agora API,
  checks append position availability, and resolves display names from string
  resources. Use when: someone asks what Compose cases exist, which APIs are
  demonstrated, wants to find a case by name or API, needs to know the current list
  position before adding a new case, or wants to know if a feature is already
  implemented in Compose. Registration is manual via Examples.kt — no @Example
  annotation. Keywords: list cases, find case, query cases, Examples.kt,
  BasicExampleList, AdvanceExampleList, available cases, existing cases, which
  case, is there a case, Compose case, Jetpack Compose.
---

## What this skill is for

Use this skill when the question is about which Compose cases already exist in `APIExample-Compose/`, which Composables demonstrate a specific RTC API, where a case is registered in `Examples.kt`, or whether a new list position should be appended.

## Source of truth

1. `APIExample-Compose/ARCHITECTURE.md`
2. `APIExample-Compose/app/src/main/java/io/agora/api/example/compose/model/Examples.kt`
3. `APIExample-Compose/app/src/main/java/io/agora/api/example/compose/samples/**`
4. `APIExample-Compose/app/src/main/res/values/strings.xml`

## Procedure

1. Read `ARCHITECTURE.md` first for the fast case index and registration model.
2. Re-scan live registrations from `Examples.kt` before claiming append position, registration presence, or list order.
3. Resolve display names from `strings.xml` for any `R.string.example_*` entry referenced by `Examples.kt`.
4. If the question is API-to-case mapping and `ARCHITECTURE.md` is insufficient, search across `samples/**` and use direct RTC API calls in the Composable source as the primary signal. Return all relevant matching cases, or the best-scoped matches if the API is shared by many samples.
5. Return direct factual conclusions only. For list position questions, report the current list length and the next append position for the target list (`BasicExampleList` or `AdvanceExampleList`). If the target list is not explicit, infer it from the requested feature or call out that the answer depends on which list the case belongs in.

## Verify

- Re-check any append-position or registration answer against the current contents of `Examples.kt` before answering
- Confirm any reported case name still matches the `strings.xml` resource referenced by the registered `Example(...)` entry
- Confirm any API-to-case answer is backed by `ARCHITECTURE.md` or the actual Composable source under `samples/**`

## Out of scope

- Editing or registering cases
- Looking for `@Example` annotations
- Claiming `nav_graph.xml` registration exists for Compose cases

## Never

- Never use `item_` string keys for Compose examples
- Never claim list position is an `@Example` index
