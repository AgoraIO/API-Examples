#!/usr/bin/env bash

set -euo pipefail

if ! command -v gitleaks >/dev/null 2>&1; then
    echo "Error: gitleaks is required to scan staged changes." >&2
    echo "Install it from https://github.com/gitleaks/gitleaks#installing" >&2
    exit 1
fi

# Gitleaks renamed "protect" to "git" in v8.19. Support both CLI forms.
if gitleaks git --help >/dev/null 2>&1; then
    exec gitleaks git --pre-commit --staged "$@"
fi

if gitleaks protect --help >/dev/null 2>&1; then
    exec gitleaks protect --staged "$@"
fi

echo "Error: the installed gitleaks version does not support staged scans." >&2
echo "Upgrade gitleaks and try again." >&2
exit 1
