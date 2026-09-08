#!/usr/bin/env bash

set -euo pipefail

if [[ $# -ne 1 || ! -f "$1" ]]; then
    echo "Error: expected a commit message file." >&2
    exit 1
fi

if ! command -v python3 >/dev/null 2>&1; then
    echo "Error: python3 is required to validate commit messages." >&2
    exit 1
fi

python3 - "$1" <<'PY'
import pathlib
import sys


han_ranges = (
    (0x3400, 0x4DBF),
    (0x4E00, 0x9FFF),
    (0xF900, 0xFAFF),
    (0x20000, 0x2EBEF),
    (0x30000, 0x323AF),
)

try:
    message = pathlib.Path(sys.argv[1]).read_text(encoding="utf-8")
except (OSError, UnicodeError) as error:
    print(f"Error: cannot read commit message as UTF-8: {error}", file=sys.stderr)
    raise SystemExit(1)

if any(start <= ord(character) <= end for character in message for start, end in han_ranges):
    print("Error: commit message contains Chinese characters.", file=sys.stderr)
    print("Please use English only in commit messages.", file=sys.stderr)
    raise SystemExit(1)
PY
