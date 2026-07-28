# API Examples Release Iteration Gate

One RTC product requirement is accepted across Android, iOS, macOS, and Windows. The Lead coordinates shared scope and cross-platform closure; platform implementation and verification remain independently owned.

## Topology

| Agent | Count | Pass Evidence |
| --- | --- | --- |
| Contract | 1 | Shared behavior, APIs, non-goals, reference, platform targets and constraints |
| Platform Implementation | 4 | Query/upsert result, scoped files, matrix proposals |
| Platform Verification | 4 | Findings, parity, entry/UX, commands, build result, skipped reasons |
| Lead | 1 | Cross-platform differences, final status, manifest assembly |

Contract runs once. In a shared checkout, Android/iOS/macOS/Windows Implementation runs are serialized so every repository delta is attributable to one agent. Each Verification depends on Contract and its matching Implementation, runs concurrently with other platform Verification roles, and cannot reuse the Implementation agent identity.

## Intake Gate

Before Contract starts, identify feature behavior, SDK family, key APIs, target SDK version for each platform, reference case, expected user flow, repository scope, and available target verification hosts. Platform targets may differ when SDK release lines are staggered. Confirm that the checked-in repository profile describes the current SDK package names and version sources. All four top-level platforms are required by default. Contract may choose the appropriate project variant per platform.

## Contract Gate

Contract passes only when:

- Scenario, inputs, success signals, APIs, and non-goals are observable.
- A reference source and result are recorded when parity is required.
- Cross-platform invariant behavior is explicit.
- Android, iOS, macOS, and Windows each have `required`, `target_project`, constraints, and allowed files.
- Lifecycle, callback threading, registration, SDK family, and credential boundaries are explicit.

Nearby cases guide framework patterns; product behavior comes from the shared Contract/reference.

## Platform Implementation Gate

Each platform Implementation passes only when it:

- Reads that platform and target-project instructions.
- Runs from the Contract-selected target project so nested `AGENTS.md` instructions are loaded by Codex.
- Queries existing cases before editing.
- Uses the selected project skill and stays inside Contract's allowed paths.
- Completes registration, localization, lifecycle cleanup, and architecture index updates.
- Lets the orchestrator derive its real platform file delta and records matrix proposals.

Platform source roots are independent, but Implementation runs sharing one checkout are serialized so cross-root edits cannot be credited to the wrong agent. Each run refreshes an input snapshot bound to the execution package and dependency artifacts; retries retain attempt provenance and produce one cumulative net delta. Separate workspaces may execute them concurrently only when each produces an independently attributable patch.

## Platform Verification Gate

Each Verification independently checks:

- Contract/reference parity, SDK types/defaults, lifecycle, threading, errors, and cleanup.
- Registration, display name, inputs, feedback, layout, and audio/video affordances.
- The strongest valid build or static command on the current verification host.
- Each executed PASS/FAIL command is bound to the hashed Codex JSONL command event and exit code; build results use recognized platform build actions from the Contract working directory.
- Dispatch provenance records the host platform. Windows build `PASS` requires a Windows host.
- The Verification run does not modify tracked or untracked repository content.
- Concrete reasons for every skipped check.

For non-`BLOCKED` acceptance, every required platform needs role status, review result, parity result, and build result `PASS`. Final `PASS` permits no skipped checks.

## Host Mismatch

Host mismatch never authorizes SDK downloads, emulation, cross-compilation, or substitute compilers as evidence. On macOS, Windows Verification may report static findings but must keep `build_result=BLOCKED` and role status `BLOCKED`. A retry on a real Windows host can replace that artifact using `--platform windows --retry`.

## Cross-Platform Gate

After platform Verification, the Lead records:

- `result`: `PASS`, `FAIL`, or `BLOCKED`.
- Evidence that all required platform contracts and behavior were compared.
- Intentional platform differences and their product/API justification.

Any required platform blocker or cross-platform result other than `PASS` forces final `BLOCKED`.

## Release Checklist

Release is mandatory manifest data, not an agent. `requirement.target_sdk_versions` and `release.target_sdk_versions` must contain the same Android, iOS, macOS, and Windows mapping. The orchestrator compares each platform only with its own target while refreshing dependency versions from live repository files during assembly. Non-`BLOCKED` acceptance requires all SDK-version checks to pass and permits no skipped repository release checks. A platform remaining on an older SDK is valid and does not require changing the script or repository profile.

Jenkins packaging, QA validation, artifact URLs, and external website publication are downstream processes outside this repository workflow. A final `PASS` means the API Example source and repository checks are ready for that external handoff; the manifest does not represent packaging or QA completion.

## Manifest V4

The single state tree is:

```text
requirement
contract
platforms.android.implementation / verification
platforms.ios.implementation / verification
platforms.macos.implementation / verification
platforms.windows.implementation / verification
cross_platform_acceptance
release
knowledge_updates
final_status
```

Validate it with:

```bash
python3 docs/ai-engineering/tools/validate_acceptance_manifest.py <manifest.json>
```

The validator enforces platform completeness, unique agent/run provenance, Contract target boundaries, platform result closure, cross-platform closure, matrix rules, SDK-version checks, repository paths, evidence-file hashes, and knowledge writeback.

## Orchestration

```bash
python3 docs/ai-engineering/tools/orchestrate_case_execution.py init \
  --matrix docs/ai-engineering/case-maintenance-matrix.md \
  --feature "<feature>" \
  --target-sdk-version "4.6.2" \
  --platform-sdk-version "android=4.6.3" \
  --run-dir /tmp/api-example-requirement

python3 docs/ai-engineering/tools/orchestrate_case_execution.py dispatch \
  --run-dir /tmp/api-example-requirement --phase contract --model "<model>"

python3 docs/ai-engineering/tools/orchestrate_case_execution.py dispatch \
  --run-dir /tmp/api-example-requirement --phase implementation --model "<model>"

python3 docs/ai-engineering/tools/orchestrate_case_execution.py dispatch \
  --run-dir /tmp/api-example-requirement --phase verification --model "<model>"
```

`--target-sdk-version` is the baseline for all four platforms. Repeat `--platform-sdk-version platform=x.y.z` for each differing platform; omit overrides when all four use one version. If the product requirement is not yet an actionable matrix row, initialize it directly with `--sdk-family` and one or more `--key-api` arguments.

Omitting `--platform` dispatches all four platform roles. Implementation runs are serialized in the shared checkout and reconciled after each run; Verification runs execute concurrently. Each independent `codex exec` run has a timeout, host provenance, a target-project working directory, and a hashed JSONL log. The execution package and input snapshots bind the checked-in repository profile by path and SHA-256. A prior `FAIL`/`BLOCKED` platform artifact can be replaced with `--retry`.

## Automation Boundary

The tools prepare platform agents, serialize shared-checkout implementation, parallelize immutable verification, enforce dependencies/timeouts, validate v4, and gate matrix changes. The Lead still approves phase advancement, retries, intentional differences, and final status.

## Final Summary

```markdown
## Acceptance Summary

Requirement:
- Scenario/APIs:
- Contract/reference:

Platforms:
- Android: files, verification, build
- iOS: files, verification, build
- macOS: files, verification, build
- Windows: files, verification, build

Cross-platform:
- Result/evidence:
- Intentional differences:

Release:
- SDK-version checks:
- External handoff required: Jenkins packaging and QA

Manifest:
- Path/validation:

Final status: PASS | PASS WITH RISKS | BLOCKED
```

Routine manifests remain execution artifacts unless explicitly selected as repository evidence.
