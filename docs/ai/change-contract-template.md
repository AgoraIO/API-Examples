# Cross-Platform Change Contract

Copy this template into the issue, pull request, or task notes before implementation. Use
concise, testable statements and keep it current when the requirement changes.

## Requirement

- Work item or issue:
- Contract owner:
- Target SDK version by platform:
- User-visible scenario:
- Inputs and defaults:
- Success signals:
- Error and recovery behavior:
- Lifecycle and cleanup:
- Permissions, assets, or external dependencies:
- Non-goals:
- Reference documentation or implementation:

## SDK Contract

| Concern | Shared intent | Android API | Apple API | Windows API |
| --- | --- | --- | --- | --- |
| Initialization | | | | |
| Main operation | | | | |
| Callback or result | | | | |
| Error handling | | | | |
| Cleanup | | | | |

## Project Scope

Use `REQUIRED`, `REVIEW_ONLY`, or `NOT_APPLICABLE`. Every non-required decision needs a
reason.

| Project | Decision | Required behavior or reason | Expected files or entry point | Planned validation |
| --- | --- | --- | --- | --- |
| Android Views full | | | | |
| Android Views audio | | | | |
| Android Compose | | | | |
| iOS UIKit full | | | | |
| iOS UIKit audio | | | | |
| iOS SwiftUI | | | | |
| iOS Objective-C | | | | |
| macOS AppKit | | | | |
| Windows MFC | | | | |

## Validation Evidence

Validation status is independent of the project scope decision. Add one row for every
planned check, especially every check required by a `REQUIRED` project.

- `PASS`: the check completed and met its acceptance criteria.
- `FAIL`: the check ran and failed.
- `BLOCKED`: the check cannot run yet; record the remaining command and unblock condition.
- `NOT_RUN`: the check was not attempted; record why.

| Project | Command or behavioral check | Status | Evidence, blocker, or reason | Remaining command and unblock condition |
| --- | --- | --- | --- | --- |
| | | | | |

## Shared Acceptance

- [ ] SDK arguments, defaults, and expected callbacks match the contract.
- [ ] Join, leave, resource ownership, and engine destruction are complete.
- [ ] UI updates follow each platform's threading model.
- [ ] Errors and unsupported states are handled consistently.
- [ ] Case registration, resources, permissions, and localization are updated where needed.
- [ ] Every required validation for each `REQUIRED` project is `PASS`.
- [ ] SDK versions and dependency sources match `docs/ai/project-matrix.md`.
- [ ] No credentials, tokens, signing assets, private keys, or private URLs are committed.

## Intentional Differences

| Project | Difference | Product, SDK, UI, or OS reason |
| --- | --- | --- |
| | | |

## Contract Changes

| Date | Change | Affected projects | Revalidation required |
| --- | --- | --- | --- |
| | | | |

A contract with `FAIL`, `BLOCKED`, or `NOT_RUN` validation can be handed off, but remains
incomplete until every required validation is `PASS`.
