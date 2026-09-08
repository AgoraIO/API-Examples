---
name: cross-platform-api-change
description: Coordinate an RTC API Examples change across Android, iOS, macOS, and Windows when behavior, SDK usage, dependencies, CI, or packaging may affect more than one project.
---

# Cross-Platform API Change

Use one written contract to keep every affected implementation aligned. The contract is
the source of truth for behavior; an existing platform implementation is only a reference.

## Required Context

1. Read the root `AGENTS.md`.
2. Read `docs/ai/README.md`, `docs/ai/change-contract-template.md`, and
   `docs/ai/project-matrix.md`.
3. For SDK, dependency, CI, packaging, signing, or external-asset changes, also read
   `docs/ai/release-risks.md`.
4. Read the platform and project `AGENTS.md` and `ARCHITECTURE.md` files for every project
   that may be affected.

Treat the current source tree as the case inventory. Do not rely on a separately maintained
case catalog.

## Workflow

### 1. Establish The Contract

Before editing, copy `docs/ai/change-contract-template.md` into the issue, pull request, or
task notes and complete it. Define observable behavior, SDK operations, inputs and defaults,
callbacks, success and failure signals, lifecycle cleanup, non-goals, and the reference
implementation or documentation.

Separate shared behavior from platform-specific API and UI details. If the requirement
changes, update the contract first and then re-evaluate scope and existing work.

### 2. Decide Every Project

Review every row in `docs/ai/project-matrix.md`. Mark each project `REQUIRED`,
`REVIEW_ONLY`, or `NOT_APPLICABLE`. Include a reason for every project that is not required.
Do not assume that Android Views covers Compose, iOS UIKit covers SwiftUI or Objective-C,
or a full-SDK project covers its audio-only counterpart.

### 3. Implement From The Same Contract

One coordinator owns the contract and cross-platform decisions. Split implementation by
top-level platform only when parallel work is explicitly available, and give every owner
the same contract, its project scope, allowed paths, reference, and validation requirements.
Platform owners must not redefine shared behavior.

Implementations may differ because of SDK types, UI frameworks, permissions, or operating
system behavior. Record each intentional difference and its reason in the contract.

### 4. Verify Each Required Project

For every required project:

- compare the diff with the shared contract;
- verify SDK arguments, defaults, callbacks, lifecycle, threading, errors, registration,
  resources, localization, and cleanup as applicable;
- run the strongest valid build or static check documented by that project;
- record changed files, commands, results, blockers, and intentional differences.

Keep scope decisions and validation status separate. Record each planned validation as one
of:

- `PASS`: the command or behavioral check completed and met its acceptance criteria;
- `FAIL`: the check ran and failed;
- `BLOCKED`: the check cannot run yet; record the blocker, remaining command, and the
  condition that will unblock it;
- `NOT_RUN`: the check was not attempted; record why.

Do not convert `BLOCKED` or `NOT_RUN` into a pass. They are valid handoff states, but they
leave the overall change incomplete.

Compilation is necessary but does not prove behavioral parity. Use an independent review
pass when available.

### 5. Close Cross-Platform Acceptance

The change is complete only when every project has an explicit scope decision, every
required implementation satisfies the contract, and every required validation is `PASS`.
A change with `FAIL`, `BLOCKED`, or `NOT_RUN` validation may be handed off with clear next
steps, but must not be reported as complete. Copy the final scope, differences, and
validation evidence into the pull request using `.github/pull_request_template.md`.

Before handoff, confirm that SDK sources match `docs/ai/project-matrix.md` and that no real
App ID, certificate, token, signing asset, private key, or private download URL is staged.

Do not commit task transcripts, agent identities, generated case catalogs, prompt snapshots,
or run logs. Preserve durable product behavior and verification evidence in the issue or
pull request.
