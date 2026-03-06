# AGENTS.md

Entry point for AI agents working on the Agora RTC Native SDK API-Examples repository.
Read this file first, then navigate to the relevant platform directory.

## Repository Overview

This repository contains sample projects demonstrating Agora RTC Native SDK APIs across five platforms. Each platform is fully independent — do not share source files or dependencies across platforms.

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

API keys and App IDs are never committed. Each platform stores them in a `KeyCenter` file (Swift/OC) or `KeyCenter.java` / `KeyCenter.kt` (Android) or `CConfig` (Windows). These files are git-ignored and must be populated locally before building.
