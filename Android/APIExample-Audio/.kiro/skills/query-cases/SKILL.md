---
name: query-cases
description: Query and browse existing API example cases in APIExample-Audio (audio-only demo). Use when someone asks what audio cases exist, which audio APIs are demonstrated, or wants to find a specific case. This project uses voice-sdk — no video APIs available.
---

# Query Cases — APIExample-Audio

Use this skill to answer questions like:
- "What audio cases are available?"
- "Which case demonstrates setVoiceBeautifierPreset?"
- "List all BASIC cases"
- "Is there a spatial audio example?"

## How cases are registered

Identical to APIExample — automatic via reflection. Every case is a Fragment class under:

```
app/src/main/java/io/agora/api/example/examples/{basic|advanced|audio}/
```

Each case class has an `@Example` annotation:

```java
@Example(
    index    = 10,          // sort order within the group
    group    = ADVANCED,    // "BASIC" or "ADVANCED"
    name     = R.string.item_xxx,
    actionId = R.id.action_mainFragment_to_xxx,
    tipsId   = R.string.xxx_tips
)
```

A commented-out `@Example` (prefixed with `//`) means the case is disabled.

This project uses `voice-sdk`. All cases are audio-only — no video APIs exist.

## How to query

### Step 1: Scan the case directories

List files under these three directories:

```
app/src/main/java/io/agora/api/example/examples/basic/
app/src/main/java/io/agora/api/example/examples/advanced/
app/src/main/java/io/agora/api/example/examples/audio/
```

Subdirectories (e.g. `customaudio/`) may contain additional cases.

### Step 2: Read the `@Example` annotation

For each case file, look for the `@Example(...)` annotation to extract:
- `group` — BASIC or ADVANCED
- `index` — sort position within the group
- `name` — string resource ID (resolve from `res/values/strings.xml` for display name)
- `tipsId` — string resource ID (resolve for description)

If the annotation is commented out (`//@Example`), the case is disabled.

### Step 3: Read the class Javadoc

The Javadoc comment above the class describes what the case demonstrates and lists key Agora audio APIs used.

### Step 4: Present results

Format the output as a table:

```
| Group    | Index | Case Name              | File                        | Key Audio APIs              |
|----------|-------|------------------------|-----------------------------|-----------------------------|
| BASIC    | 0     | Join Channel (Token)   | JoinChannelAudioByToken.java| joinChannel()               |
| BASIC    | 1     | Join Channel Audio     | JoinChannelAudio.java       | joinChannel()               |
| ADVANCED | 4     | Voice Effects          | VoiceEffects.java           | setVoiceBeautifierPreset()  |
```

When answering a specific query, filter the table to matching cases only.

## String resource lookup

Display names and descriptions are in:

```
app/src/main/res/values/strings.xml
```

## Quick reference: case directories

| Directory | Group | Description |
|-----------|-------|-------------|
| `examples/basic/` | BASIC | Core audio — join channel |
| `examples/advanced/` | ADVANCED | Advanced audio features — voice effects, spatial sound, etc. |
| `examples/audio/` | ADVANCED | Audio visualization (waveform) — grouped as ADVANCED |
