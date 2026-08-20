# API Examples AI Engineering Knowledge Index

This index is the first file to read when an agent turns a product request into API Example changes. It links the existing repository knowledge into one route so the agent does not rediscover platform rules, case patterns, build commands, and known failure modes from scratch.

## Source Priority

Use sources in this order:

1. Product request or issue text provided by the user.
2. `AGENTS.md` at the repository root for cross-platform boundaries.
3. Platform `AGENTS.md` for platform selection and red lines.
4. Project `AGENTS.md` for concrete commands and project-level skills.
5. Project `ARCHITECTURE.md` for case index, registration rules, and canonical file layout.
6. `docs/ai-engineering/repository-profile.json` for repository-specific SDK package names and version sources.
7. `docs/ai-engineering/case-maintenance-matrix.md` for cross-platform parity planning and known gaps.
8. Existing project `.agents/skills/*/SKILL.md` for case query, creation, and review procedures.
9. `docs/ai-engineering/release-known-issues.md` for release packaging and pipeline risks.
10. Repository hooks and platform build scripts for final verification.

Do not use memory or sibling repositories as the source of truth when the current checkout has conflicting instructions. Treat external knowledge as a hint, then verify it against this repository.

## Repository Boundary

The repository has four independent platform families:

| Platform | Root | Primary Rules |
| --- | --- | --- |
| Android | `Android/` | `Android/AGENTS.md` |
| iOS | `iOS/` | `iOS/AGENTS.md` |
| macOS | `macOS/` | `macOS/AGENTS.md` |
| Windows | `windows/` | `windows/AGENTS.md` |

Never share source files, build scripts, SDK dependencies, or generated project metadata across platform roots unless a platform rule explicitly says to do so.

## Project Selection

| Request Type | Default Target |
| --- | --- |
| Android full RTC, video, screen sharing, beauty, extensions | `Android/APIExample/` |
| Android voice-only or audio-only SDK behavior | `Android/APIExample-Audio/` |
| Android Compose case or Compose parity work | `Android/APIExample-Compose/` |
| iOS UIKit Swift full RTC | `iOS/APIExample/` |
| iOS SwiftUI parity work | `iOS/APIExample-SwiftUI/` |
| iOS Objective-C parity work | `iOS/APIExample-OC/` |
| iOS audio-only SDK behavior | `iOS/APIExample-Audio/` |
| macOS Swift Cocoa sample | `macOS/` |
| Windows C++ MFC sample | `windows/` |

If the product request does not name a platform, default to Android, iOS, macOS, and Windows. Narrow the scope only when the user or Contract records an explicit waiver reason. Treat each required platform as a separate implementation and verification unit.

## Existing Local Skills

| Scope | Skills |
| --- | --- |
| `Android/APIExample/` | `query-cases`, `upsert-case`, `review-case` |
| `Android/APIExample-Audio/` | `query-cases`, `upsert-case`, `review-case` |
| `Android/APIExample-Compose/` | `query-cases`, `upsert-case`, `review-case` |
| `iOS/APIExample/` | `query-cases`, `upsert-case`, `review-case` |
| `iOS/APIExample-Audio/` | `query-cases`, `upsert-case`, `review-case` |
| `iOS/APIExample-OC/` | `query-cases`, `upsert-case`, `review-case` |
| `iOS/APIExample-SwiftUI/` | `query-cases`, `upsert-case`, `review-case` |
| `macOS/` | `upsert-case`, `review-case` |
| `windows/` | `upsert-case`, `review-case` |

The repository-level orchestration skill is `.agents/skills/api-example-release-iteration/SKILL.md`.

## Repository Profile

`docs/ai-engineering/repository-profile.json` contains the SDK version sources that differ between API Examples distributions. Shared Python tools parse source kinds such as Gradle properties, CocoaPods packages, and SDK archive names without embedding distribution package names in code.

The orchestrator stores the profile path and SHA-256 in the execution package and input snapshots. Dispatch and assembly stop if the checked-in profile changes after initialization. Coverage state and historical knowledge do not belong in this profile; keep them in the matrix and durable knowledge documents.

## Codex Role Routing

`docs/ai-engineering/role-routing.json` defines three role types and logical profiles. Provider model names remain runtime inputs through `--model` or `CODEX_MODEL_STANDARD`, `CODEX_MODEL_DEEP`, and `CODEX_MODEL_REVIEW`. The orchestrator starts replayable independent `codex exec` sessions; do not describe them as parent-managed Codex subagents.

The phase order is:

1. `contract`: one shared Contract for product behavior, reference, cross-platform invariants, and platform targets.
2. `implementation`: Android, iOS, macOS, and Windows agents run independently after Contract passes. A shared checkout serializes them and reconciles each delta before the next run.
3. `verification`: independent Android, iOS, macOS, and Windows agents run concurrently after their matching implementation passes.

Release checks are mandatory manifest data, not an agent phase. Every platform dispatch has its own timeout, hashed JSONL command log, run identity, host platform, input snapshot, and Contract-selected working directory. Use `--platform` for a focused retry; otherwise the workflow covers all four official roots.

## Agent Acceptance Artifacts

Use `docs/ai-engineering/templates/acceptance-manifest-template.json` when an iteration changes source, workflow gates, matrix state, SDK-version logic, or docs that affect agent behavior.

Validate the filled manifest with:

```bash
python3 docs/ai-engineering/tools/validate_acceptance_manifest.py <manifest.json>
```

The filled manifest is normally an execution artifact, not a repository document. Commit it only when the user explicitly asks for an evidence snapshot or when it is selected as a curated pilot-run example.

For case backfill work, generate candidate platform execution units from the matrix:

```bash
python3 docs/ai-engineering/tools/generate_case_backlog.py
```

Use the generated priority to select the next product requirement, then deliver that requirement across all four official platform families. Contract selects the applicable project variant inside each platform.

## Case Implementation Knowledge

For cross-platform parity requests:

- Start with `docs/ai-engineering/case-maintenance-matrix.md`.
- Verify every relevant matrix cell against the target project before editing.
- Treat `UNKNOWN` as "not checked yet", not as a confirmed gap.
- Treat each required platform as an independent delivery/verification unit inside one requirement-level acceptance manifest.

Before adding a case:

- Use the target project's `query-cases` skill when available.
- Read the target project's `ARCHITECTURE.md` case index.
- Confirm whether the case already exists by feature name and SDK API name.
- Confirm the target group, registration mechanism, display name, and index rules.
- Extract the reference contract for parity work before implementation. The closest existing target-project case may guide lifecycle, permissions, UI framework, and registration patterns, but product semantics must come from the source reference contract.

While adding or modifying a case:

- Follow the target project's `upsert-case` skill.
- Keep edits inside the selected project unless the project-level instructions say otherwise.
- Update `ARCHITECTURE.md` when the case list, path, or key APIs change.
- Keep sensitive configuration placeholders intact. Do not commit real App IDs, certificates, tokens, or credentials.

After implementation:

- Run the target project's `review-case` skill.
- Run the applicable build or static check from the project `AGENTS.md`.
- Record which checks passed, failed, or were skipped with a reason. PASS/FAIL commands must match a JSONL `command_execution` event and exit code.
- Fill and validate an acceptance manifest before claiming final acceptance.

## Knowledge Maintenance Protocol

When an iteration uncovers a reusable failure pattern, record it in the right durable document instead of leaving it only in a chat summary.

Use this structure:

- Source: review finding, CI failure, release blocker, customer report, or manual smoke.
- Impact platform/project.
- Symptom.
- Root cause.
- Guardrail or rule.
- Verification command or evidence.
- Updated date.

Where to write it:

- Cross-platform routing or implementation traps: `docs/ai-engineering/knowledge-index.md`.
- Release, signing, CI, packaging, license, or SDK-version risks: `docs/ai-engineering/release-known-issues.md`.
- Case coverage state: `docs/ai-engineering/case-maintenance-matrix.md`.
- Project-specific implementation traps: the target project `ARCHITECTURE.md` or `.agents/skills/*/SKILL.md`.

Do not duplicate the same rule in every file. Put the durable rule at the lowest scope that future agents must read.

When an acceptance manifest includes `knowledge_updates`, at least one platform Implementation must list the durable document or skill change in `files_changed`. The validator enforces this so new failure patterns cannot remain only in the execution summary.

## Cross-Cutting Red Lines

- Every case owns its RTC engine lifecycle.
- Leave the channel before destroying or releasing the engine.
- SDK callbacks can arrive on background threads; dispatch UI updates to the platform main thread.
- Do not hardcode real credentials.
- Do not mix full SDK and audio-only SDK APIs.
- Do not move common sample code across platform roots to reduce duplication.
- Do not edit packaging or CI scripts unless the product request or failure requires it.

## Verification Entrypoints

| Area | Entrypoint |
| --- | --- |
| Sensitive info and commit-message hooks | `HOOKS-GUIDE.md`, `.git-hooks/`, `.pre-commit-config.yaml`, `.gitleaks.toml` |
| Android build/test | project `AGENTS.md`, project Gradle files |
| iOS build | project `AGENTS.md`, project `Podfile`, `.github/ci/build/build_ios*.sh` |
| macOS build | `macOS/AGENTS.md`, `macOS/Podfile`, `.github/ci/build/build_mac*.sh` |
| Windows build | `windows/AGENTS.md`, `.github/ci/build/build_windows.*` |
| CI/CD | `azure-pipelines.yml`, `.github/ci/`, `.github/workflows/`, `cicd/` |
| Release known issues | `docs/ai-engineering/release-known-issues.md` |

Prefer the smallest check that validates the changed surface. For docs-only changes, use static validation and link checks instead of full platform builds.

## Pilot Run Reports

`docs/ai-engineering/pilot-runs/` contains curated workflow validation examples, not a log of every agent execution.

Use these reports to understand how a gate was exercised at a point in time. Do not use them as the source of truth for current SDK versions, case coverage, CI status, or release readiness. Re-check the live repository and current CI before reusing any conclusion.

Only add a new pilot-run report when it is intentionally selected as a representative workflow example or when the user explicitly asks for a repository evidence snapshot. Routine run results should stay in the response, PR discussion, CI artifact, or task tracker.

Use `docs/ai-engineering/templates/release-dry-run-template.md` as the starting point when such a repository evidence snapshot is needed.

## Known Failure Patterns To Guard Against

| Pattern | Guardrail |
| --- | --- |
| Wrong platform touched during a focused fix | Re-read root and platform `AGENTS.md`; list intended files before editing. |
| Case added but not visible in app | Verify project registration mechanism and case index. |
| Android case ordering collision | Use `query-cases` and scan source annotations before choosing a sort index. |
| SDK callback updates UI directly | Run `review-case`; check every callback that touches UI. |
| Engine leak after leaving screen | Check leave-channel before destroy/release in the real screen-close path. |
| Audio-only project receives video API | Re-check selected project SDK type before implementation. |
| CI diagnosis starts in the wrong layer | Identify the first failing log line before changing scripts or signing settings. |
| Third-party beauty license expires silently | Track expiration and confirm renewal before release branches are packaged. |
| Android Extension SDK headers drift from SDK version | Check extension `include` files during SDK version bumps. |
| iOS/macOS certificates expire on build machines | Inspect or print certificate expiration during release preparation. |
| SDK dependency version is not bumped on release branch | Verify platform SDK version files against the release target. |
| Model-declared build result is not backed by command telemetry | Bind each executed verification command to the hashed Codex JSONL log and exit code, and accept build status only for recognized platform build tools. |
| Platform run misses nested instructions | Start `codex exec` in the Contract-selected target so root/platform/project `AGENTS.md` files load automatically. |
| External packaging metadata leaks into repository acceptance | Keep Jenkins jobs, artifact URLs, QA evidence, and website publication outside the acceptance manifest. |
| Windows packaging fails from path length or permissions | Run Windows script preflight after path or packaging changes. |

## Output Contract

Every completed AI-assisted iteration should end with an acceptance summary:

- Product request interpreted.
- Target platform/project.
- Files changed.
- Skills or docs used.
- Verification run and results.
- Open risks or skipped checks.

Use `docs/ai-engineering/release-iteration-gate.md` for the full gate.
