---
name: query-cases
description: >
  Query and browse existing API example cases in the APIExample-Audio Android demo —
  lists cases by group, finds which case demonstrates a specific Agora audio API,
  checks sort index availability, and resolves display names from string resources.
  Use when: someone asks what cases exist, which audio APIs are demonstrated, wants
  to find a case by name or API (e.g. setVoiceBeautifierPreset, enableSpatialAudio),
  needs a free sort index before adding a new case, or wants to know if an audio
  feature is already implemented. This project uses voice-sdk — no video APIs.
  Keywords: list cases, find case, query cases, @Example, sort index, BASIC, ADVANCED,
  available cases, existing cases, which case, is there a case, audio case.
---

# Query Cases — APIExample-Audio

## How cases are registered

Every case is a Fragment under `app/src/main/java/io/agora/api/example/examples/{basic|advanced|audio}/` with an `@Example` annotation:

```java
@Example(
    index    = 10,       // unique within the group; BASIC: 0–9, ADVANCED: 10+
    group    = ADVANCED,
    name     = R.string.item_xxx,
    actionId = R.id.action_mainFragment_to_xxx,
    tipsId   = R.string.xxx_tips
)
```

A commented-out `@Example` (`//@Example`) means the case is disabled and won't appear in the app.

This project uses `voice-sdk` — all cases are audio-only, no video APIs exist.

---

## Query procedure

### Step 1: Decide scope before scanning

Before listing files, ask:
- **Looking for a specific API?** — scan Javadoc comments for the API name; no need to read all files
- **Need a free sort index?** — collect all `index` values for the target group, then find the gap
- **Listing all cases?** — scan all three directories and collect annotations

### Step 2: Read ARCHITECTURE.md first

Read `ARCHITECTURE.md` (the `examples/` section of the Directory Layout). It contains a pre-built index of all cases with group, index, display name, and key API — no file scanning needed for most queries.

Use ARCHITECTURE.md as the primary source. Fall back to scanning the source directories only when:
- The query requires data not in ARCHITECTURE.md (e.g. full `@Example` field values, `tipsId`)
- ARCHITECTURE.md appears stale (a case exists in source but not in the doc)
- The output involves free-index claims, index collisions, or "is index X available?" decisions — these must be validated from source immediately before final output

### Step 3: Scan case directories (fallback only)

| Directory | Group | Contents |
|-----------|-------|----------|
| `examples/basic/` | BASIC | Core audio join/leave patterns |
| `examples/advanced/` | ADVANCED | Feature-specific audio APIs |
| `examples/audio/` | ADVANCED | Audio visualization (still grouped ADVANCED) |

Each `.java` file is a case. Subdirectories (e.g. `customaudio/`) contain multi-file cases — the main class is the file whose name matches the directory name; if no name match, look for the file containing `@Example`.

### Step 4: Extract `@Example` fields

For each file, read the annotation for `group`, `index`, `name` (string resource ID), and `tipsId`. If the annotation is commented out, the case is disabled.

Resolve display names from `app/src/main/res/values/strings.xml`:
`R.string.item_voice_effects` → `<string name="item_voice_effects">Voice Effects</string>`

### Step 5: Read class Javadoc for API mapping

The Javadoc above each class lists the key APIs demonstrated:

```java
/**
 * This demo demonstrates how to apply voice beautifier effects.
 *
 * Key APIs used:
 * - RtcEngine.setVoiceBeautifierPreset()
 */
```

Use this to answer "which case uses X?" queries without reading the full implementation.

If no Javadoc is present, scan the method body for the API name as a method call. If still not found, note "API mapping unavailable" in the results table.

### Step 6: Present results

Full listing — table format:

| Group | Index | Case Name | File | Key APIs |
|-------|-------|-----------|------|----------|
| BASIC | 0 | Join Channel Audio | JoinChannelAudio.java | joinChannel() |
| ADVANCED | 4 | Voice Effects | VoiceEffects.java | setVoiceBeautifierPreset() |

For a specific query (e.g. "which case uses enableSpatialAudio?"), return only matching rows.

For a free-index query, list all used indices in the target group and identify the next available slot:
> BASIC range: 0–9. ADVANCED range: 10+.
> ADVANCED indices in use: 10, 11, 12, 15, 20 → next free: 13

Before returning any free-index/collision result, re-scan source registration points (`@Example` across `basic/`, `advanced/`, `audio/`) and recompute once from source-of-truth data.

---

## NEVER

- **NEVER** count a commented-out `@Example` (`//@Example`) as an active case — it is disabled and won't appear in the app.
- **NEVER** mix index spaces across groups — `audio/` cases use `group=ADVANCED` but share the same index namespace as `advanced/`; always scan both directories together when finding a free index.
- **NEVER** use filename alone to identify a subdirectory case — the main class is the file whose name matches the directory name; if no match, look for the file with `@Example`.
- **NEVER** report a free index without scanning all three directories (`basic/`, `advanced/`, `audio/`) for the target group — missing one causes index collisions.
- **NEVER** suggest video APIs — this project uses voice-sdk only; video APIs do not exist.
