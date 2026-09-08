#!/usr/bin/env bash
# api-examples-pre-commit-dispatcher

set -euo pipefail

if ! repo_root="$(git rev-parse --show-toplevel 2>/dev/null)"; then
    exit 0
fi

config_file="$repo_root/.pre-commit-config.yaml"
if [[ ! -f "$config_file" ]]; then
    exit 0
fi

if ! command -v pre-commit >/dev/null 2>&1; then
    echo "Error: pre-commit is required by $config_file" >&2
    echo "Install it from https://pre-commit.com/#install" >&2
    exit 1
fi

cd "$repo_root"

case "$(basename "$0")" in
    pre-commit)
        exec pre-commit run --config "$config_file" --hook-stage pre-commit
        ;;
    commit-msg)
        if [[ $# -ne 1 ]]; then
            echo "Error: commit-msg hook expected a commit message file." >&2
            exit 1
        fi
        exec pre-commit run --config "$config_file" --hook-stage commit-msg --commit-msg-filename "$1"
        ;;
    *)
        echo "Error: unsupported hook name: $(basename "$0")" >&2
        exit 1
        ;;
esac
