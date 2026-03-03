---
inclusion: manual
---

# Complex Case Spec Guidelines (APIExample-Audio)

Follow these rules when creating a Spec for a new case.

## Complexity Criteria

A case requires the Spec workflow if it meets any of the following conditions; otherwise use the `add-new-case` skill directly:

1. Involves coordinated calls across two or more Agora API modules
2. Requires a custom UI layout (not one of the standard Layout A/B/C/D templates in the skill)
3. Manages multiple channels or multiple engine instances
4. Requires a foreground Service or background thread coordination
5. Involves developing new shared components (widget/utils, etc.)
6. Requires optional module integration (e.g. streamEncrypt)

If none of the above apply → recommend the user use the `add-new-case` skill directly; no Spec needed.

## Requirements Document Must Include

- List of APIs the case demonstrates (audio APIs only)
- User interaction flow description
- Expected RtcEngine lifecycle behavior
- Required permissions (typically only `RECORD_AUDIO`)

## Design Document Must Include

- Target project identifier: `APIExample-Audio`
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
- Read `ARCHITECTURE.md` or use the `query-cases` skill to check existing indices and avoid collisions

## Architecture Red Line Checks

The design document must verify compliance with the following red lines:

Global red lines:
- Each case has its own independent RtcEngine instance
- On screen destruction: `leaveChannel()` before `destroy()`
- `IRtcEngineEventHandler` callback thread safety
- Request permissions before `joinChannel()`

Java-specific checks:
- `handler.post(RtcEngine::destroy)` — not a direct call
- Null-check `getPrivateCloudConfig()`
- `setParameters(...)` call must not be omitted

voice-sdk checks (must pass):
- No video APIs: `enableVideo`, `setupLocalVideo`, `setupRemoteVideo`, `VideoCanvas`, `startScreenCapture`
- Violation → compile failure; must be eliminated at design time

Flag any red line violation in the design document and provide a correction recommendation.

## Task List and Skill Integration

- Mark which sub-tasks can be executed with the `add-new-case` skill and provide skill input parameters (case name, group, index, API type)
- Mark which sub-tasks require manual coding and provide target file paths and change summaries
- New shared component creation tasks must come before case implementation tasks

## Risk Identification

Identify the following risk types in the design document and provide mitigation:
- API availability (voice-sdk has no video module — confirm API existence upfront)
- Permissions (`RECORD_AUDIO` runtime denial)
- Thread safety (callback and UI update interaction)
- Performance (multiple engine instances, audio processing)

Risks that cannot be mitigated should be flagged as open issues and recommended for prototype validation during implementation.
