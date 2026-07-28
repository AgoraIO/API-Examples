# Release Dry Run Report Template

Use this template only when a release dry run is intentionally selected as a repository-level evidence snapshot. Routine execution results should stay in the agent response, PR discussion, CI artifact, or task tracker.

Do not fill this template with stale values from an older run. Re-check the live repository and target release version before completing every section.

## Scope

- Repository:
- Branch:
- Commit:
- Target release SDK version:
- Product scenario:
- Target platform roots:
- Verification budget:
- Non-goals:

## Sources Used

- `AGENTS.md`
- `.agent/skills/api-example-release-iteration/SKILL.md`
- `docs/ai-engineering/knowledge-index.md`
- `docs/ai-engineering/repository-profile.json`
- `docs/ai-engineering/release-iteration-gate.md`
- `docs/ai-engineering/release-known-issues.md`
- Platform `AGENTS.md` files:
- Platform SDK version files:
- Platform build scripts:

## Findings

### Product Gate

Status:

Evidence:

### Architecture Gate

Status:

Evidence:

### Repository Release Gate

Status:

| Gate | Result | Evidence |
| --- | --- | --- |
| SDK version consistency |  |  |
| Android Extension include freshness |  |  |
| Third-party beauty license validity |  |  |
| Repository build checks |  |  |

### Static Version Snapshot

| Platform Project | Version Source | Observed Version |
| --- | --- | --- |
| `Android/APIExample` | `gradle.properties` |  |
| `Android/APIExample-Audio` | `gradle.properties` |  |
| `Android/APIExample-Compose` | `gradle.properties` |  |
| `iOS/APIExample` | `Podfile` |  |
| `iOS/APIExample-Audio` | `Podfile` |  |
| `iOS/APIExample-OC` | `Podfile` |  |
| `iOS/APIExample-SwiftUI` | `Podfile` |  |
| `macOS` | `Podfile` |  |
| `windows/APIExample` | SDK package or install script |  |

### Script Checks

Commands:

```bash

```

Result:

Not covered:

- <unchecked area>

### Existing Automation Found

| Area | Evidence |
| --- | --- |
| SDK version validation |  |
| Build script preflight |  |
| Signing visibility |  |
| Windows packaging preflight |  |

## Automation Gaps

1. <gap>

## Acceptance Summary

Product:
- Scenario:
- Target:
- Non-goals:

Architecture:
- Platform/project:
- Key constraints:

Implementation:
- Files changed:
- Skills/docs used:

Review:
- Result:
- Findings:

Testing:
- Commands:
- Result:
- Skipped checks:

Release:
- Required:
- Target SDK versions by platform:
- Checks:
- Skipped checks:
- External handoff: Jenkins packaging and QA

UX:
- Entry point:
- Notes:

Final status: PASS | PASS WITH RISKS | BLOCKED
