#!/bin/bash

# Try to find gitleaks in common locations
GITLEAKS_CMD=""

# Check if gitleaks is in PATH
if command -v gitleaks &> /dev/null; then
    GITLEAKS_CMD="gitleaks"
# Check Homebrew locations (Intel Mac)
elif [ -f "/usr/local/bin/gitleaks" ]; then
    GITLEAKS_CMD="/usr/local/bin/gitleaks"
# Check Homebrew locations (Apple Silicon Mac)
elif [ -f "/opt/homebrew/bin/gitleaks" ]; then
    GITLEAKS_CMD="/opt/homebrew/bin/gitleaks"
# Check if installed via other package managers
elif [ -f "$HOME/.local/bin/gitleaks" ]; then
    GITLEAKS_CMD="$HOME/.local/bin/gitleaks"
fi

# If gitleaks not found, exit with error
if [ -z "$GITLEAKS_CMD" ]; then
    echo "============================================================"
    echo "❌ ERROR: gitleaks not found!"
    echo ""
    echo "Please install gitleaks:"
    echo "  macOS: brew install gitleaks"
    echo "  Linux: https://github.com/gitleaks/gitleaks#installing"
    echo ""
    echo "After installation, run: ./.git-hooks/install-hooks.sh"
    echo "============================================================"
    exit 1
fi

# Run gitleaks with provided arguments
exec "$GITLEAKS_CMD" "$@"
