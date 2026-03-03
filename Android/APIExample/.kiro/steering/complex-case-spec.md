---
inclusion: manual
---

# Complex Case Spec Guidelines (APIExample)

Follow these rules when creating a Spec for a new case.

## Complexity Criteria

A case requires the Spec workflow if it meets any of the following conditions; otherwise use the `add-new-case` Skill directly:

1. Involves coordinated calls across two or more Agora API modules
2. Requires a custom UI layout (not one of the standard Layout A/B/C/D templates in the Skill)
3. Involves multi-channel or multi-engine instance management
4. Requires a foreground Service or background thread coordination
5. Involves developing new shared components (widgets/utils, etc.)
6. Requires optional module integration (simpleFilter/streamEncrypt)

If none of the above apply → recommend the user use the `add-new-case` Skill directly; no Spec needed.

## Requirements Document Must Include

- List of APIs the case demonstrates
- User interaction flow description
- Expected RtcEngine lifecycle behavior
- Required permissions list

## Design Document Must Include

- Target project identifier: `APIExample`
- Class/file structure design
- API call sequence (Mermaid sequence diagram recommended)
- State management approach
- UI layout plan
- Integration points with existing shared components
- Case registration info: class name, display name, group (BASIC/ADVANCED), sort index
- Risk identification and mitigation

## Pre-generate Registration Info

Registration info must be finalized during the design phase to avoid conflicts during coding:

- Generate `@Example` annotation parameters, `nav_graph.xml` action + destination, `strings.xml` key names (`item_` prefix)
- Read `ARCHITECTURE.md` or use the `query-cases` Skill to check existing indices and avoid conflicts

## Architecture Red Line Checks

The design document must verify compliance with the following red lines:

Global red lines:
- Each case has its own independent RtcEngine instance
- On screen destruction: `leaveChannel()` before `destroy()`
- `IRtcEngineEventHandler` callback thread safety
- Request permissions before `joinChannel()`

Additional checks:
- Use `handler.post(RtcEngine::destroy)` — not a direct call
- Null-check `getPrivateCloudConfig()`
- `setParameters(...)` call must not be omitted

Red line violations → flag in the design document and provide a corrective recommendation.

## Task List and Skill Integration

- Mark which sub-tasks can be executed with the `add-new-case` Skill, and provide Skill input parameters (case name, group, index, API type)
- Mark which sub-tasks require manual coding, and provide target file paths and change summaries
- Tasks for creating new shared components must come before case implementation tasks

## Risk Identification

Identify the following risk types in the design document and provide mitigation strategies:
- API compatibility (SDK version constraints)
- Performance (multiple engine instances, high video stream count)
- Permissions (runtime permission denial)
- Thread safety (callback and UI update interactions)

Risks that cannot be mitigated should be flagged as open issues, with a recommendation to prototype during the implementation phase.
