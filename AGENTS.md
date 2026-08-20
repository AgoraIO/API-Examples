# AGENTS.md

Entry point for AI agents working on the Agora RTC Native SDK API-Examples repository.
Read this file first, then navigate to the relevant platform directory.

## Repository Overview

This repository contains sample projects demonstrating Agora RTC Native SDK APIs across four independent platforms. Each platform is self-contained — do not share source files, build scripts, or dependencies across platforms.

| Platform | Language(s) | Directory | SDK |
|----------|-------------|-----------|-----|
| Android | Java / Kotlin | `Android/` | RTC Java SDK (full / voice) |
| iOS | Swift / Objective-C | `iOS/` | RTC Objective-C SDK (full / audio) |
| macOS | Swift | `macOS/` | RTC Objective-C SDK (full) |
| Windows | C++ | `windows/` | RTC C++ SDK (full) |

## Navigation

Each platform directory contains its own `AGENTS.md` with platform-specific rules, project selection guidance, and architecture constraints. Always read the platform-level `AGENTS.md` before making any changes.

| Platform | Entry Point |
|----------|-------------|
| Android | `Android/AGENTS.md` |
| iOS | `iOS/AGENTS.md` |
| macOS | `macOS/AGENTS.md` |
| Windows | `windows/AGENTS.md` |

## AI Engineering Workflow

For AI-assisted version iteration, product-to-implementation routing, multi-agent acceptance, or automatic API example generation, use the repository workflow in `.agents/skills/api-example-release-iteration/SKILL.md`.

Supporting documents:
- `docs/ai-engineering/knowledge-index.md` - repository knowledge map for agents.
- `docs/ai-engineering/release-iteration-gate.md` - product, architecture, review, test, and UX acceptance gates.
- `docs/ai-engineering/role-routing.json` - shared Contract and independent platform role types with logical Codex profiles.
- `docs/ai-engineering/repository-profile.json` - repository-specific SDK package names and version sources used by the shared tools.
- `docs/ai-engineering/release-known-issues.md` - repository release-risk reference.

## Cross-Platform Rules

1. Never share source files, build scripts, or SDK dependencies between platforms.
2. Each platform manages its own SDK version — check the platform-level config file before assuming a version.
3. All examples follow the same structural pattern within their platform: one self-contained class per API feature, managing its own engine lifecycle.
4. Always call the SDK's leave-channel and destroy APIs when an example screen is closed.
5. SDK event/delegate callbacks may arrive on a background thread — always dispatch UI updates to the main thread.

## Repository-Level Files

| File | Purpose |
|------|---------|
| `HOOKS-GUIDE.md` | Git hook installation (sensitive-info detection, commit-message rules) |
| `.pre-commit-config.yaml` | Pre-commit hook configuration |
| `.gitleaks.toml` | Gitleaks allowlist configuration |
| `azure-pipelines.yml` | CI/CD pipeline definition |

## Git Hooks

This repository enforces two rules via Git hooks:
- No sensitive information (API keys, tokens) in committed code.
- Commit messages must be in English only (no Chinese characters).

Run `.git-hooks/install-hooks.sh` once after cloning to activate the hooks.
See `HOOKS-GUIDE.md` for details and troubleshooting.

## Sensitive Configuration

Never commit a real App ID, App Certificate, or token. The mechanism differs by platform, and only Android keeps credentials in a git-ignored file:

| Platform | Where the App ID goes | Tracked by git? |
|----------|----------------------|-----------------|
| Android | `local.properties` at each project root, key `AGORA_APP_ID`, read by `app/build.gradle`(`.kts`) into `BuildConfig.AGORA_APP_ID` | No — `local.properties` is git-ignored |
| iOS | `<Project>/Common/KeyCenter.swift`, or `KeyCenter.h` / `KeyCenter.m` for `APIExample-OC` | **Yes** |
| macOS | `APIExample/Common/KeyCenter.swift` | **Yes** |
| Windows | `APIExample/APIExample/CConfig.h` and `CConfig.cpp` | **Yes** |

The iOS, macOS, and Windows credential files are tracked placeholder files, not git-ignored. Editing one to build locally leaves a real credential in a tracked file, so never stage it. The safety net is the gitleaks pre-commit hook (see Git Hooks above), not `.gitignore` — do not rely on git ignoring these paths.
