# Pilot Run Reports

This directory stores curated workflow validation examples for the API Examples AI engineering process.

Pilot-run reports are historical evidence. They show how a gate or workflow behaved at a specific branch, commit, and date. They are not the source of truth for current SDK versions, case coverage, CI status, signing state, or release readiness.

Use `docs/ai-engineering/templates/release-dry-run-template.md` when a new release dry-run evidence snapshot is explicitly needed.

## What Belongs Here

- First dry run for a new repository gate or workflow.
- Representative cross-platform rehearsal that future agents can learn from.
- Explicitly requested evidence snapshot for a release or workflow decision.

## What Does Not Belong Here

- Every routine agent execution result.
- Repeated acceptance summaries for normal feature work.
- CI logs or build output that already belongs in CI artifacts.
- Mutable release status that must be checked live.

## Maintenance Rules

- Use dated filenames.
- Keep reviewed reports immutable; create a new report for a new run.
- Include branch, commit, scope, commands, skipped checks, and final status.
- Re-check live files and CI before using an old report as evidence.
