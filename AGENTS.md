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

For a requirement that may affect more than one project, use
`.agents/skills/cross-platform-api-change/SKILL.md`. It requires one shared behavior contract,
an explicit decision for all nine projects, per-project validation, and a final
cross-platform comparison.

Durable workflow documentation lives in `docs/ai/`. Start with `docs/ai/README.md`; use
`docs/ai/change-contract-template.md` before implementation and record the final scope and
evidence in `.github/pull_request_template.md`.

## Cross-Platform Rules

1. Never share source files, build scripts, or SDK dependencies between platforms.
2. Each platform manages its own SDK version — check the platform-level config file before assuming a version.
3. All examples follow the same structural pattern within their platform: one self-contained class per API feature, managing its own engine lifecycle.
4. Always call the SDK's leave-channel and destroy APIs when an example screen is closed.
5. SDK event/delegate callbacks may arrive on a background thread — follow the selected project's threading rules and keep UI or state updates framework-safe.

## Repository-Level Files

| File | Purpose |
|------|---------|
| `HOOKS-GUIDE.md` | Git hook installation (sensitive-info detection, commit-message rules) |
| `.pre-commit-config.yaml` | Pre-commit hook configuration |
| `.gitleaks.toml` | Gitleaks allowlist configuration |
| `.github/workflows/compile.yml` | Post-release compile-only checks using a placeholder App ID |
| `.github/workflows/repository-policy.yml` | Remote sensitive-information, commit-message, and AI asset checks |
| `.github/ci/build/` | Jenkins release packaging entry points |
| `.github/ci/README.md` | CI ownership, entry points, and local verification |

## Git Hooks

When installed, the repository's local Git hooks enforce two rules:
- No sensitive information (API keys, tokens) in staged changes.
- Commit messages must be in English only (no Chinese characters).

Install `pre-commit`, Gitleaks, and Python 3, then run `.git-hooks/install-hooks.sh` once after cloning. The script validates prerequisites but does not install them.
See `HOOKS-GUIDE.md` for details and troubleshooting.

## Sensitive Configuration

Never commit a real App ID, App Certificate, or token. The mechanism differs by platform, and only Android keeps credentials in a git-ignored file:

| Platform | Where the App ID goes | Tracked by git? |
|----------|----------------------|-----------------|
| Android | `local.properties` at each project root, key `AGORA_APP_ID`, read by `app/build.gradle`(`.kts`) into `BuildConfig.AGORA_APP_ID` | No — `local.properties` is git-ignored |
| iOS | `<Project>/Common/KeyCenter.swift`, or `KeyCenter.h` / `KeyCenter.m` for `APIExample-OC` | **Yes** |
| macOS | `APIExample/Common/KeyCenter.swift` | **Yes** |
| Windows | `windows/APIExample/APIExample/stdafx.h` for the compile-time `APP_ID`; `CConfig.cpp` also reads `AppID.ini` beside the built executable when the placeholder remains | `stdafx.h`: **Yes**; generated `AppID.ini`: **No** |

The iOS, macOS, and Windows credential files are tracked placeholder files, not git-ignored. Editing one to build locally leaves a real credential in a tracked file, so never stage it. The safety net is the gitleaks pre-commit hook (see Git Hooks above), not `.gitignore` — do not rely on git ignoring these paths.

GitHub Actions compilation uses an all-zero 32-character placeholder App ID and does not run RTC sessions. It must not use repository secrets, App Certificates, or tokens. The compile workflow runs automatically only after changes reach `main`, when the referenced SDK version is available from public package repositories.
