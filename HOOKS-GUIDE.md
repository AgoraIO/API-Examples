# Git Hook Installation Guide

This repository uses pre-commit to run two local checks:

- Gitleaks scans staged changes for sensitive information.
- Commit messages containing Chinese characters are rejected.

Chinese content in source files and documentation is allowed. It is not part of the commit-message rule.

## Prerequisites

Install these tools before installing the hooks:

- [pre-commit](https://pre-commit.com/#install)
- [Gitleaks](https://github.com/gitleaks/gitleaks#installing)
- Python 3

For example, on macOS with Homebrew:

```bash
brew install pre-commit gitleaks python
```

The repository installation script checks these dependencies but does not install or update them.

## Install The Hooks

Run this command once after cloning the repository:

```bash
./.git-hooks/install-hooks.sh
```

The script installs both the `pre-commit` and `commit-msg` hook types from `.pre-commit-config.yaml`. It supports normal clones, Git worktrees, and custom hook paths.

## Custom Hook Paths

Git ignores its default hook directory when `core.hooksPath` is configured. The installation script detects this setting and installs repository-aware dispatchers into that directory. Existing hooks with other names, such as `post-commit` and `pre-push`, are preserved.

Inspect the active value and its source with:

```bash
git config --show-origin --get core.hooksPath
```

The dispatchers run pre-commit only when the current repository contains `.pre-commit-config.yaml`. This allows a shared hook directory to serve multiple repositories without hardcoded repository paths.

The installer never changes `core.hooksPath`. It also refuses to overwrite an existing `pre-commit` or `commit-msg` hook that it does not manage. If either name is already used, update that hook to invoke pre-commit before rerunning the installer.

## Usage

The hooks run automatically during `git commit`:

- The pre-commit hook scans only the content staged for the commit.
- The commit-msg hook checks the proposed commit message.

To run the staged sensitive-information check manually:

```bash
pre-commit run gitleaks
```

Git allows hooks to be bypassed with `git commit --no-verify`. Use that option only when explicitly approved; bypassing the hook also bypasses the sensitive-information scan.

## Troubleshooting

If a valid sample value triggers Gitleaks, add the narrowest possible rule-specific allowlist entry to `.gitleaks.toml`. Do not allowlist an entire documentation or source directory, because doing so would hide real credentials in that directory.

If a commit message is rejected, rewrite it in English. The commit-message check requires UTF-8 input and Python 3.
