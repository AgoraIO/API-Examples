---
inclusion: manual
---

# Complex Case Spec Guidelines (APIExample-Compose)

Follow these rules when creating a Spec for a new case.

## Complexity Criteria

A case requires the Spec workflow if it meets any of the following conditions; otherwise use the `add-new-case` skill directly:

1. Involves coordinated calls across two or more Agora API modules
2. Requires a custom Composable layout not covered by the skill's standard template
3. Manages multiple channels or multiple engine instances
4. Requires a foreground Service or background coroutine coordination
5. Involves developing new shared components (shared Composables / utils)
6. Requires optional module integration (simpleFilter / streamEncrypt)

If none of the above apply → recommend the user use the `add-new-case` skill directly, no Spec needed.

## Requirements Document Must Include

- List of APIs the case demonstrates
- User interaction flow description
- Expected RtcEngine lifecycle behavior
- Required permissions list

## Design Document Must Include

- Target project identifier: `APIExample-Compose`
- Composable function structure design
- API call sequence (Mermaid sequence diagram recommended)
- State management plan (`remember` vs `rememberSaveable` boundaries)
- UI layout plan
- Integration points with existing shared components
- Case registration info: `Examples.kt` list entry, `strings.xml` key (`example_` prefix)
- Risk identification and mitigation

## Pre-generate Registration Info

Registration info must be finalized during the design phase to avoid conflicts during coding:

- Generate the `Examples.kt` list entry and `strings.xml` key (`example_` prefix)
- Read `ARCHITECTURE.md` or use the `query-cases` skill to check existing entries and avoid conflicts

## Architecture Red Line Checks

The design document must verify compliance with the following red lines:

Global red lines:
- Each case has an independent RtcEngine instance
- On screen destruction: `leaveChannel()` before `destroy()` (via `DisposableEffect` `onDispose`)
- `IRtcEngineEventHandler` callbacks are thread-safe
- Request permissions before `joinChannel()`

Compose-specific checks:
- `DisposableEffect(lifecycleOwner)` not `DisposableEffect(Unit)`
- `rememberSaveable` for rotation-surviving state (channelName, isJoined, uid)
- `remember` for non-serializable objects like RtcEngine
- UI updates in callbacks via `coroutineScope.launch(Dispatchers.Main)`

Violations → flag in the design document and provide a correction recommendation.

## Task List and Skill Integration

- Mark which sub-tasks can be executed with the `add-new-case` skill, and provide skill input parameters (case name, group, index)
- Mark which sub-tasks require manual coding, and provide target file paths and change summaries
- Tasks for creating new shared Composables must come before case implementation tasks

## Risk Identification

Identify the following risk types in the design document and provide mitigation:
- API compatibility (SDK version constraints)
- Performance (multiple engine instances, many video streams, Compose recomposition frequency)
- Permissions (runtime permission denial)
- Thread safety (callback and Compose State update interaction)
- Rotation / configuration changes (state preservation strategy)

Risks that cannot be mitigated should be flagged as open issues and recommended for prototype validation during implementation.
