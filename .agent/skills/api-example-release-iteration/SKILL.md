---
name: api-example-release-iteration
description: Use when turning an RTC API Examples product requirement into aligned Android, iOS, macOS, and Windows changes with attributed implementation and independent repository acceptance
---

# API Example Release Iteration

Use this skill for one product requirement across all official platform families. The Lead coordinates a shared Contract, attributed platform delivery, independent platform Verification, and final cross-platform acceptance.

## Required Inputs

Identify before editing:

- Product scenario and key RTC SDK APIs.
- New case, existing case update, docs-only, or SDK-version work.
- Reference case for behavior and parity.
- Verification budget and available platform hosts.
- Target RTC SDK version for each platform. Platforms may remain on different release lines.

Android, iOS, macOS, and Windows are required by default. Contract may choose the appropriate full/audio/Compose/UIKit/Objective-C project for each platform; marking a platform not required needs an explicit waiver reason.

## Source Route

Read root `AGENTS.md`, the repository profile, this knowledge index and gate, then each platform/project `AGENTS.md`, `ARCHITECTURE.md`, and available `query-cases`, `upsert-case`, and `review-case` skills. Project skills may use either `.agent/skills/` or `.agents/skills/`. Read `release-known-issues.md` only for release, SDK version, CI, signing, packaging, license, or third-party asset work.

## Agent Topology

```text
Lead
  Contract
  Android Implementation -> Android Verification
  iOS Implementation     -> iOS Verification
  macOS Implementation   -> macOS Verification
  Windows Implementation -> Windows Verification
  Cross-platform acceptance
```

This is one Lead plus nine independent `codex exec` runs. They are replayable requirement-workflow runs, not parent-managed Codex subagent threads. Contract executes once. In a shared checkout, platform Implementation agents run one at a time so each repository delta belongs to one run; platform Verification agents run concurrently after their corresponding Implementation passes.

| Role Type | Responsibility | Profile |
| --- | --- | --- |
| Contract | Shared behavior, APIs, non-goals, reference, platform targets and allowed files | `standard` |
| Implementation | Query and modify exactly one platform target | `deep` |
| Verification | Findings-first review, parity, UX, and strongest valid platform checks | `review` |

Implementation cannot approve its own platform. Every accepted artifact needs a distinct `agent_id`, Codex run provenance, model/profile, and input snapshot hash.

## Execution

Initialize one requirement:

```bash
python3 docs/ai-engineering/tools/orchestrate_case_execution.py init \
  --matrix docs/ai-engineering/case-maintenance-matrix.md \
  --feature "<feature>" \
  --target-sdk-version "4.6.2" \
  --platform-sdk-version "android=4.6.3" \
  --run-dir /tmp/api-example-requirement
```

`--target-sdk-version` supplies the baseline for all four platforms. Repeat `--platform-sdk-version platform=x.y.z` only for platforms whose target differs. The execution package and manifest store the resolved four-platform mapping, so staggered platform releases never require script or repository-profile changes.

The default `docs/ai-engineering/repository-profile.json` supplies repository-specific SDK package names and version sources. Use `--repository-profile` only for a checked-in alternative profile. The execution package records its repository-relative path and SHA-256.

For a new requirement not yet actionable in the matrix, also pass `--sdk-family "<family>"` and repeat `--key-api "<API>"` as needed.

Dispatch the shared Contract:

```bash
python3 docs/ai-engineering/tools/orchestrate_case_execution.py dispatch \
  --run-dir /tmp/api-example-requirement \
  --phase contract \
  --model "<model>"
```

Dispatch all platform agents by omitting `--platform`. Implementation runs are serialized in the shared checkout for delta attribution; Verification runs fan out concurrently:

```bash
python3 docs/ai-engineering/tools/orchestrate_case_execution.py dispatch \
  --run-dir /tmp/api-example-requirement \
  --phase implementation \
  --model "<model>"

python3 docs/ai-engineering/tools/orchestrate_case_execution.py dispatch \
  --run-dir /tmp/api-example-requirement \
  --phase verification \
  --model "<model>"
```

Use `--platform android|ios|macos|windows` for a single platform and `--retry` to replace a prior `FAIL` or `BLOCKED` artifact. Retrying Implementation invalidates that platform's old Verification and carries all attempts into one cumulative net repository delta. Each run has an independent 900-second default timeout, JSONL command log, host provenance, and target-project working directory so nested `AGENTS.md` files load automatically.

Assemble only after reviewing all required platform artifacts:

```bash
python3 docs/ai-engineering/tools/orchestrate_case_execution.py assemble \
  --run-dir /tmp/api-example-requirement \
  --matrix docs/ai-engineering/case-maintenance-matrix.md \
  --final-status "<PASS|PASS WITH RISKS|BLOCKED>" \
  --cross-platform-result "<PASS|FAIL|BLOCKED>" \
  --cross-platform-evidence "<evidence>"
```

## Platform Rules

- Contract selects the target project and allowed files for each platform.
- The dispatcher runs platform roles from that target project. In a shared checkout it refreshes the package/dependency snapshot, completes and reconciles one Implementation before starting the next, derives that role's cumulative net repository delta, and rejects changes outside Contract paths.
- Each Implementation agent follows only its platform/project instructions.
- Platform source files, dependencies, and build scripts are never shared across roots.
- Verification covers code review, reference parity, discoverability, UX, and valid checks on the current verification host. Executed PASS/FAIL commands must bind to the Codex JSONL command event and exit code, Contract working directory, and recorded host platform.
- Contract and Verification runs may create ignored build output but must not modify tracked or untracked repository content.
- A required platform `FAIL` or `BLOCKED` forces the whole requirement to `BLOCKED`.
- Final non-`BLOCKED` acceptance requires all required platform review, parity, and build results to pass, plus cross-platform acceptance `PASS`.

## Host Mismatch

On macOS, Windows Verification may perform repository-local static review only. It must not download Windows SDK archives, emulate Windows, cross-compile, or use a substitute compiler as build evidence. Keep Windows Verification and the requirement `BLOCKED` until the Verification role is retried on a real Windows host with MSBuild evidence.

## Matrix And Release

Each platform Implementation may propose zero or more matrix updates. `DONE` requires final `PASS`, platform parity/build `PASS`, and no skipped checks. Release data is limited to repository SDK-version consistency. Jenkins packaging, QA validation, artifact distribution, and external website publication are outside this workflow and must not be recorded as manifest gates.

## Final Output

Report the shared Contract, each platform's files and Verification result, declared platform differences, SDK-version checks, manifest validation, and final status. Do not describe the workflow as fully autonomous: the Lead still advances phases, handles retries, and owns final cross-platform judgment.
