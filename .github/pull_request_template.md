<!-- For multi-project changes, follow docs/ai/README.md and establish the shared contract before implementation. -->

## Change Contract

- Requirement or issue:
- Shared behavior:
- SDK/API changes:
- Non-goals:
- Reference implementation or documentation:

## Project Scope

Use `REQUIRED`, `REVIEW_ONLY`, or `NOT_APPLICABLE` and explain every non-required decision.
For documentation or repository-only work, one shared reason may cover all unaffected rows.

| Project | Decision | Change or reason |
| --- | --- | --- |
| Android Views full | | |
| Android Views audio | | |
| Android Compose | | |
| iOS UIKit full | | |
| iOS UIKit audio | | |
| iOS SwiftUI | | |
| iOS Objective-C | | |
| macOS AppKit | | |
| Windows MFC | | |

## Cross-Platform Review

- Intentional platform differences and reasons:
- Lifecycle, threading, registration, resource, or permission considerations:
- Remaining blockers or risks:

## Validation

Use `PASS`, `FAIL`, `BLOCKED`, or `NOT_RUN`. Scope and validation are separate: a documented
blocker is useful handoff information, but it does not make a required check pass.

| Project | Command or behavioral check | Status | Evidence, blocker, or reason | Remaining command and unblock condition |
| --- | --- | --- | --- | --- |
| | | | | |

## Repository Checks

- [ ] The implementation matches the shared contract across all required projects.
- [ ] Every required validation for each `REQUIRED` project is `PASS`; otherwise this change remains incomplete.
- [ ] SDK versions and dependency sources match `docs/ai/project-matrix.md`.
- [ ] Gitleaks passed and no real App ID, certificate, token, private key, signing asset, or private URL is included.
- [ ] Commit messages are in English.
- [ ] Release packaging changes were checked separately from compile-only CI when applicable.
