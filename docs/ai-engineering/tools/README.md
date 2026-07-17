# AI Engineering Tools

`orchestrate_case_execution.py` turns one RTC requirement into a shared Contract plus attributed Android, iOS, macOS, and Windows delivery. It launches independent, replayable `codex exec` sessions rather than parent-managed Codex subagent threads.

## Role Types

| Phase | Agents | Profile |
| --- | --- | --- |
| `contract` | One shared Contract | `standard` |
| `implementation` | Four platform Implementation agents | `deep` |
| `verification` | Four independent platform Verification agents | `review` |

Provider model names remain runtime inputs through `--model` or `CODEX_MODEL_STANDARD`, `CODEX_MODEL_DEEP`, and `CODEX_MODEL_REVIEW`.

## Run A Requirement

```bash
python3 docs/ai-engineering/tools/orchestrate_case_execution.py init \
  --matrix docs/ai-engineering/case-maintenance-matrix.md \
  --feature "Join channel audio" \
  --target-sdk-version "4.6.2" \
  --platform-sdk-version "android=4.6.3" \
  --run-dir /tmp/api-example-requirement
```

For a new feature outside the matrix backlog, add `--sdk-family "Full RTC" --key-api "<API>"`; repeat `--key-api` for multiple APIs.

`--target-sdk-version` is a baseline expanded to Android, iOS, macOS, and Windows. Repeat `--platform-sdk-version platform=x.y.z` for any platform on a different SDK line. For example, the command above resolves to Android 4.6.3 and iOS/macOS/Windows 4.6.2. The final mapping is stored in the execution package and manifest; platform release cadence changes do not require edits to Python or the repository profile.

The default `docs/ai-engineering/repository-profile.json` defines repository-specific SDK version sources. Shared tools support Gradle property, CocoaPods package, and SDK archive-name sources without hard-coding a distribution in Python. `init` accepts `--repository-profile` for a checked-in alternative and binds its path and SHA-256 into the execution package.

Dispatch Contract, then the two platform phases:

```bash
python3 docs/ai-engineering/tools/orchestrate_case_execution.py dispatch \
  --run-dir /tmp/api-example-requirement --phase contract --model "<model>"

python3 docs/ai-engineering/tools/orchestrate_case_execution.py dispatch \
  --run-dir /tmp/api-example-requirement --phase implementation --model "<model>"

python3 docs/ai-engineering/tools/orchestrate_case_execution.py dispatch \
  --run-dir /tmp/api-example-requirement --phase verification --model "<model>"
```

Omitting `--platform` covers Android, iOS, macOS, and Windows. Implementation runs are serialized in a shared checkout and reconciled before the next agent starts; Verification runs execute concurrently. Use `--platform windows` for one platform. Use `--retry` only to replace a prior `FAIL` or `BLOCKED` artifact; an Implementation retry automatically invalidates that platform's old Verification.

Each run starts in the Contract-selected target project, writes independent stdout/stderr logs, records its host platform, and has a 900-second default timeout. Input snapshots bind the execution package, current repository state, routing config, repository profile, and dependency artifact hashes. This lets Codex load nested platform/project `AGENTS.md` files automatically. `--dry-run` resolves prompts, content-aware snapshots, models, working directories, and commands without starting Codex.

Verification PASS/FAIL commands are accepted only when their exact command string and exit code exist in the hashed Codex JSONL log. Build commands must execute a real target-platform build action from the Contract working directory; Windows build `PASS` additionally requires a Windows host. Contract and Verification may create ignored build output but fail if they modify tracked or untracked repository content. Implementation `files_changed` is derived from the real per-run repository delta rather than trusted from model output.

Assemble with an explicit cross-platform conclusion:

```bash
python3 docs/ai-engineering/tools/orchestrate_case_execution.py assemble \
  --run-dir /tmp/api-example-requirement \
  --matrix docs/ai-engineering/case-maintenance-matrix.md \
  --final-status "BLOCKED" \
  --cross-platform-result "BLOCKED" \
  --cross-platform-evidence "Windows-host verification pending"
```

Manifest v4 stores one shared Contract and one Implementation/Verification pair per official platform. Implementation retries preserve attempt history and expose one cumulative net delta whose file list must match the manifest. Matrix updates are applied only after structural and evidence-file validation and never for final `BLOCKED`. Assembly also refreshes all live platform SDK dependency versions.

Assembly does not accept CI job URLs, package artifact URLs, or QA metadata. Jenkins packaging, QA validation, artifact distribution, and website publication are external handoff processes rather than repository acceptance gates.

## macOS And Windows

Windows Verification on macOS is static review only. It must not download Windows SDKs, emulate, cross-compile, or treat another compiler as MSBuild evidence. Keep the Windows artifact `BLOCKED`, then replace it from a real Windows host with:

```bash
python3 docs/ai-engineering/tools/orchestrate_case_execution.py dispatch \
  --run-dir /tmp/api-example-requirement \
  --phase verification \
  --platform windows \
  --retry \
  --model "<model>"
```

## Supporting Tools

- `generate_case_backlog.py`: reads `MISSING`/`PARTIAL` matrix cells and prioritizes requirement candidates.
- `prepare_case_execution.py`: creates a v4 requirement package without starting Codex.
- `validate_acceptance_manifest.py`: validates a filled v4 manifest.

```bash
python3 docs/ai-engineering/tools/prepare_case_execution.py \
  --feature "Join channel audio" \
  --target-sdk-version "4.6.2" \
  --platform-sdk-version "android=4.6.3" \
  --repository-profile docs/ai-engineering/repository-profile.json
python3 docs/ai-engineering/tools/validate_acceptance_manifest.py <manifest.json>
```

The standalone validator resolves evidence paths relative to the manifest and recomputes prompt, role-artifact, input-snapshot, command-log, and repository-delta hashes.

Routine manifests and dispatch logs stay outside the repository unless the user requests an evidence snapshot.

## Tests

```bash
PYTHONDONTWRITEBYTECODE=1 python3 -m unittest \
  docs/ai-engineering/tools/validate_acceptance_manifest_test.py \
  docs/ai-engineering/tools/generate_case_backlog_test.py \
  docs/ai-engineering/tools/prepare_case_execution_test.py \
  docs/ai-engineering/tools/orchestrate_case_execution_test.py
```
