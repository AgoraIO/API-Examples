---
name: query-cases
description: Query and browse existing API example cases in APIExample-Compose (Jetpack Compose demo). Use when someone asks what Compose cases exist, which APIs are demonstrated, or wants to find a specific case. Kotlin only, no XML layouts.
---

# Query Cases — APIExample-Compose

Use this skill to answer questions like:
- "What Compose cases are available?"
- "Which case demonstrates takeSnapshot?"
- "List all BasicExampleList cases"
- "Is there a screen sharing example in Compose?"

## How cases are registered

Unlike APIExample, this project does NOT use reflection. Cases are manually registered in:

```
app/src/main/java/io/agora/api/example/compose/model/Examples.kt
```

Two lists define the groups:
- `BasicExampleList` — basic cases
- `AdvanceExampleList` — advanced cases

Each entry is an `Example` data class:

```kotlin
data class Example(
    @StringRes val name: Int,
    val description: String = "",
    val content: @Composable (back: () -> Unit) -> Unit
)
```

List order is display order (no index field — position in the list determines order).

## How to query

### Step 1: Read Examples.kt

Read the file:

```
app/src/main/java/io/agora/api/example/compose/model/Examples.kt
```

Parse `BasicExampleList` and `AdvanceExampleList` to get all registered cases.

### Step 2: Resolve string resources

The `name` field references a string resource like `R.string.example_video_snapshot`. Look up the corresponding entry in:

```
app/src/main/res/values/strings.xml
```

### Step 3: Read the Composable file

Case implementations are in:

```
app/src/main/java/io/agora/api/example/compose/samples/
```

Each `.kt` file contains a top-level `@Composable` function. The KDoc comment above it describes what the case demonstrates and lists key Agora APIs.

### Step 4: Present results

Format the output as a table:

```
| Group    | Position | Case Name              | File                     | Key APIs                    |
|----------|----------|------------------------|--------------------------|-----------------------------|
| Basic    | 1        | Join Channel (Token)   | JoinChannelVideoToken.kt | joinChannel()               |
| Basic    | 2        | Join Channel Video     | JoinChannelVideo.kt      | joinChannel(), setupLocalVideo() |
| Advanced | 1        | Live Streaming         | LiveStreaming.kt          | setClientRole()             |
```

Position is the 1-based index in the list. When answering a specific query, filter to matching cases only.

## String resource lookup

Display names are in:

```
app/src/main/res/values/strings.xml
```

String keys use the prefix `example_` (e.g. `example_video_snapshot`).

## Key differences from APIExample

| Aspect | APIExample | APIExample-Compose |
|--------|------------|-------------------|
| Registration | `@Example` annotation + reflection | Manual list in `Examples.kt` |
| Ordering | `index` field in annotation | Position in list |
| Groups | `BASIC` / `ADVANCED` strings | `BasicExampleList` / `AdvanceExampleList` variables |
| Language | Java | Kotlin |
| UI | XML layouts | Jetpack Compose |
| Tips string | Separate `tipsId` field | No tips — only `name` and optional `description` |
