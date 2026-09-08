#!/usr/bin/env bash

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if ! repo_root="$(git -C "$script_dir" rev-parse --show-toplevel 2>/dev/null)"; then
    echo "Error: .git-hooks must be inside a Git repository." >&2
    exit 1
fi

cd "$repo_root"

has_error=false

hooks_path="$(git config --path --get core.hooksPath || true)"

if ! command -v pre-commit >/dev/null 2>&1; then
    echo "Error: pre-commit is required. Install it from https://pre-commit.com/#install" >&2
    has_error=true
fi

if ! command -v gitleaks >/dev/null 2>&1; then
    echo "Error: gitleaks is required. Install it from https://github.com/gitleaks/gitleaks#installing" >&2
    has_error=true
fi

if ! command -v python3 >/dev/null 2>&1; then
    echo "Error: python3 is required to validate commit messages." >&2
    has_error=true
fi

required_files=(
    ".pre-commit-config.yaml"
    ".gitleaks.toml"
    ".git-hooks/run-gitleaks.sh"
    ".git-hooks/check-commit-message.sh"
    ".git-hooks/pre-commit-dispatcher.sh"
)

for required_file in "${required_files[@]}"; do
    if [[ ! -f "$required_file" ]]; then
        echo "Error: required file not found: $required_file" >&2
        has_error=true
    fi
done

if [[ "$has_error" == true ]]; then
    exit 1
fi

if [[ -z "$hooks_path" ]]; then
    pre-commit install --hook-type pre-commit --hook-type commit-msg
else
    if [[ "$hooks_path" == /* ]]; then
        hooks_dir="$hooks_path"
    else
        hooks_dir="$repo_root/$hooks_path"
    fi

    dispatcher_source=".git-hooks/pre-commit-dispatcher.sh"
    dispatcher_marker="api-examples-pre-commit-dispatcher"

    for hook_type in pre-commit commit-msg; do
        hook_target="$hooks_dir/$hook_type"
        if [[ -e "$hook_target" || -L "$hook_target" ]]; then
            if ! grep -Fq "$dispatcher_marker" "$hook_target" 2>/dev/null; then
                echo "Error: refusing to replace existing hook: $hook_target" >&2
                echo "Make that hook invoke pre-commit, then run this script again." >&2
                has_error=true
            fi
        fi
    done

    if [[ "$has_error" == true ]]; then
        exit 1
    fi

    mkdir -p "$hooks_dir"
    install -m 0755 "$dispatcher_source" "$hooks_dir/pre-commit"
    install -m 0755 "$dispatcher_source" "$hooks_dir/commit-msg"
    echo "Installed pre-commit dispatchers in configured hook path: $hooks_dir"
fi

echo "Git hooks installed successfully:"
echo "  - staged changes are scanned for sensitive information"
echo "  - commit messages containing Chinese characters are rejected"
