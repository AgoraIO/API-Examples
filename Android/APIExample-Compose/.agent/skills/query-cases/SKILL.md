---
name: query-cases
description: >
  Query and browse existing API example cases in the APIExample-Compose Android demo —
  lists cases by group, finds which case demonstrates a specific Agora API, checks list
  position availability, and resolves display names from string resources. Use when:
  someone asks what Compose cases exist, which APIs are demonstrated, wants to find a
  case by name or API (e.g. takeSnapshot, setClientRole), needs to know the current
  list position before adding a new case, or wants to know if a feature is already
  implemented in Compose. Registration is manual via Examples.kt — no @Example annotation.
  Keywords: list cases, find case, query cases, Examples.kt, BasicExampleList,
  AdvanceExampleList, available cases, existing cases, which case, is there a case,
  Compose case, Jetpack Compose.
---

# Query Cases — APIExample-Compose

## How cases are registered

Unlike APIExample, this project does NOT use reflection. Cases are manually registered in:

`app/src/main/java/io/agora/api/example/compose/model/Examples.kt`

Two lists define the groups:

```kotlin
val BasicExampleList = listOf(
    Example(R.string.example_join_channel_video) { JoinChannelVideo() },
    // …
)

val AdvanceExampleList = listOf(
    Example(R.string.example_live_streaming) { LiveStreaming() },
    // …
)
```

List position is display order — there is no `index` field. String keys use the `example_` prefix.

---

## Query procedure

### Step 1: Decide scope before scanning

Before reading files, ask:
- **Looking for a specific API?** — read Composable KDoc comments for the API name; no need to read all files
- **Need to know current list positions?** — read Examples.kt only; positions are 1-based list indices
- **Listing all cases?** — read Examples.kt for the full registry, then resolve names from strings.xml

### Step 2: Read ARCHITECTURE.md first

Read `ARCHITECTURE.md` (the `samples/` section of the Directory Layout). It contains a pre-built index of all cases with group, position, display name, and key API — no file scanning needed for most queries.

Use ARCHITECTURE.md as the primary source. Fall back to reading Examples.kt only when:
- The query requires data not in ARCHITECTURE.md (e.g. exact list position, `description` field)
- ARCHITECTURE.md appears stale (a case exists in Examples.kt but not in the doc)
- The output involves list position availability, duplicate registration checks, or "is this case already registered?" decisions — these must be validated from `Examples.kt` immediately before final output

### Step 3: Read Examples.kt (fallback / position queries)

File: `app/src/main/java/io/agora/api/example/compose/model/Examples.kt`

Parse `BasicExampleList` and `AdvanceExampleList`. Each entry is:

```kotlin
Example(R.string.example_your_case_name) { YourCaseName() }
```

Position in the list (1-based) is the display order. There is no `index` field and no disabled/commented-out mechanism equivalent to `//@Example`.

### Step 4: Resolve display names

Resolve `R.string.example_*` from `app/src/main/res/values/strings.xml`:
`R.string.example_video_snapshot` → `<string name="example_video_snapshot">Video Snapshot</string>`

### Step 5: Read Composable KDoc for API mapping

Case implementations are in `app/src/main/java/io/agora/api/example/compose/samples/`. The KDoc above each public Composable lists key APIs:

```kotlin
/**
 * Demonstrates how to take a snapshot of the local video stream.
 *
 * Key APIs used:
 * - RtcEngine.takeSnapshot()
 */
@Composable
fun VideoSnapshot() { … }
```

Use this to answer "which case uses X?" without reading the full implementation. If no KDoc, scan the function body for the API name.

### Step 6: Present results

Full listing — table format:

| Group | Position | Case Name | File | Key APIs |
|-------|----------|-----------|------|----------|
| Basic | 1 | Join Channel Video | JoinChannelVideo.kt | joinChannel(), setupLocalVideo() |
| Advanced | 3 | Video Snapshot | VideoSnapshot.kt | takeSnapshot() |

For a specific query, return only matching rows.

For a position query, list current entries in the target list and identify the next available slot:
> AdvanceExampleList has 12 entries → next position: 13 (append at end)

Before returning any position/registration-conflict result, re-read `Examples.kt` and recompute from the current list entries.

---

## NEVER

- **NEVER** look for `@Example` annotations — this project uses manual registration in Examples.kt, not reflection.
- **NEVER** treat list position as a unique ID that must be gap-free — position is just list order; new cases always append at the end of the appropriate list.
- **NEVER** use the `item_` string prefix — Compose cases use `example_` prefix; `item_` belongs to APIExample.
- **NEVER** scan `nav_graph.xml` for case registration — Compose navigation is position-based and requires no nav graph changes.
