---
name: query-cases
description: Query and browse existing API example cases in APIExample (full demo). Use when someone asks what cases exist, which APIs are demonstrated, or wants to find a specific case by name, group, or Agora API.
---

# Query Cases — APIExample

Use this skill to answer questions like:
- "What cases are available?"
- "Which case demonstrates takeSnapshot?"
- "List all BASIC cases"
- "Is there a screen sharing example?"

## How cases are registered

Cases are auto-discovered at startup via reflection. Every case is a Fragment class under:

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

A commented-out `@Example` (prefixed with `//`) means the case is disabled and will not appear in the app.

## How to query

### Step 1: Scan the case directories

List files under these three directories:

```
app/src/main/java/io/agora/api/example/examples/basic/
app/src/main/java/io/agora/api/example/examples/advanced/
app/src/main/java/io/agora/api/example/examples/audio/
```

Each `.java` file is a case (subdirectories may contain additional cases).

### Step 2: Read the `@Example` annotation

For each case file, look for the `@Example(...)` annotation to extract:
- `group` — BASIC or ADVANCED
- `index` — sort position within the group
- `name` — string resource ID (resolve from `res/values/strings.xml` for display name)
- `tipsId` — string resource ID (resolve for description)

If the annotation is commented out (`//@Example`), the case is disabled.

### Step 3: Read the class Javadoc

The Javadoc comment above the class describes what the case demonstrates and lists key Agora APIs used. Example:

```java
/**
 * This demo demonstrates how to take a snapshot
 * of the local video stream and save it to the device gallery.
 *
 * Key APIs used:
 * - RtcEngine.takeSnapshot()
 */
```

### Step 4: Present results

Format the output as a table:

```
| Group    | Index | Case Name            | File                        | Key APIs                    |
|----------|-------|----------------------|-----------------------------|-----------------------------|
| BASIC    | 0     | Join Channel (Token) | JoinChannelVideoByToken.java| joinChannel()               |
| BASIC    | 1     | Join Channel Video   | JoinChannelVideo.java       | joinChannel(), setupLocalVideo() |
| ADVANCED | 0     | Live Streaming       | LiveStreaming.java           | setClientRole()             |
```

When answering a specific query (e.g. "which case uses takeSnapshot?"), filter the table to matching cases only.

## String resource lookup

Display names and descriptions are in:

```
app/src/main/res/values/strings.xml
```

The `name` field in `@Example` references a string resource like `R.string.item_video_snapshot`. Look up the corresponding `<string name="item_video_snapshot">` entry to get the human-readable name.

## Quick reference: case directories

| Directory | Group | Description |
|-----------|-------|-------------|
| `examples/basic/` | BASIC | Core functionality — join channel, basic audio/video |
| `examples/advanced/` | ADVANCED | Advanced features — beauty, encryption, streaming, etc. |
| `examples/audio/` | ADVANCED | Audio-specific cases (waveform, audio routing) — still grouped as ADVANCED |
