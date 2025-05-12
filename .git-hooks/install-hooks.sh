#!/bin/bash

# Functions for colored text output
print_green() {
    echo -e "\033[0;32m$1\033[0m"
}

print_yellow() {
    echo -e "\033[0;33m$1\033[0m"
}

print_red() {
    echo -e "\033[0;31m$1\033[0m"
}

# Function to add executable permissions
ensure_executable() {
    if [ -f "$1" ] && [ ! -x "$1" ]; then
        chmod +x "$1"
        print_green "Added executable permission to $1"
    fi
}

# Ensure script runs from project root directory
if [ ! -d ".git" ]; then
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    cd "$(dirname "$SCRIPT_DIR")" || { print_red "Cannot find project root directory"; exit 1; }
    
    if [ ! -d ".git" ]; then
        print_red "Please run this script from the project root directory"
        exit 1
    fi
fi

# Check if pre-commit is installed
if ! command -v pre-commit &> /dev/null; then
    print_yellow "pre-commit not found, attempting to install..."
    if command -v pip3 &> /dev/null; then
        pip3 install pre-commit
    elif command -v pip &> /dev/null; then
        pip install pre-commit
    else
        print_red "pip not found, please install Python and pip first, then run this script again"
        exit 1
    fi
    
    if [ $? -ne 0 ]; then
        print_red "Failed to install pre-commit, please install manually: pip install pre-commit"
        exit 1
    fi
    print_green "pre-commit installed successfully!"
else
    print_green "pre-commit is already installed!"
fi

# Check if gitleaks is installed
if ! command -v gitleaks &> /dev/null; then
    print_yellow "gitleaks not found, please install it..."
    print_yellow "Installation guide: https://github.com/gitleaks/gitleaks#installing"
    
    # Attempt automatic installation (based on OS)
    if [[ "$OSTYPE" == "darwin"* ]]; then
        print_yellow "Detected macOS, attempting to install gitleaks via Homebrew..."
        if command -v brew &> /dev/null; then
            brew install gitleaks
            if [ $? -eq 0 ]; then
                print_green "gitleaks installed successfully!"
            else
                print_red "Cannot automatically install gitleaks, please install manually"
                exit 1
            fi
        else
            print_red "Homebrew not found, please install Homebrew or install gitleaks manually"
            exit 1
        fi
    else
        print_red "Please install gitleaks manually and try again"
        exit 1
    fi
fi

# Check required files and directories
if [ ! -d ".git-hooks" ]; then
    print_red "Cannot find .git-hooks directory, please ensure you're in the correct project"
    exit 1
fi

if [ ! -f ".gitleaks.toml" ]; then
    print_red "Cannot find .gitleaks.toml configuration file, please ensure it exists"
    exit 1
fi

if [ ! -f ".git-hooks/check-commit-message.sh" ]; then
    print_red "Cannot find .git-hooks/check-commit-message.sh file, please ensure it exists"
    exit 1
fi

# Ensure all scripts have executable permissions
print_yellow "Granting executable permissions to hook scripts..."
ensure_executable ".git-hooks/check-commit-message.sh"
ensure_executable ".git-hooks/post-commit"
ensure_executable ".git-hooks/pre-commit"

# Install pre-commit hook
print_yellow "Installing pre-commit hook..."
pre-commit install
if [ $? -ne 0 ]; then
    print_red "Failed to install pre-commit hook!"
    exit 1
fi
print_green "pre-commit hook installed successfully!"

# Install commit-msg hook
print_yellow "Installing commit-msg hook..."
pre-commit install --hook-type commit-msg
if [ $? -ne 0 ]; then
    print_red "Failed to install commit-msg hook!"
    exit 1
fi
print_green "pre-commit commit-msg hook installed successfully!"

# Copy and set up custom hooks
print_yellow "Setting up custom hooks..."
# Copy commit-msg hook
cp .git-hooks/check-commit-message.sh .git/hooks/commit-msg
chmod +x .git/hooks/commit-msg

# Copy post-commit hook (if exists)
if [ -f ".git-hooks/post-commit" ]; then
    cp .git-hooks/post-commit .git/hooks/post-commit
    chmod +x .git/hooks/post-commit
fi

# Copy pre-commit hook (if exists)
if [ -f ".git-hooks/pre-commit" ]; then
    # Backup pre-commit hook
    if [ -f ".git/hooks/pre-commit" ]; then
        cp .git/hooks/pre-commit .git/hooks/pre-commit.bak
    fi
    
    cp .git-hooks/pre-commit .git/hooks/pre-commit.custom
    chmod +x .git/hooks/pre-commit.custom
    
    # Add custom pre-commit to existing hook chain
    if [ -f ".git/hooks/pre-commit" ]; then
        HOOK_CONTENT=$(cat .git/hooks/pre-commit)
        if ! grep -q "pre-commit.custom" .git/hooks/pre-commit; then
            echo -e "\n# Run custom pre-commit hook\n.git/hooks/pre-commit.custom || exit 1" >> .git/hooks/pre-commit
            chmod +x .git/hooks/pre-commit
        fi
    else
        echo -e "#!/bin/bash\n\n# Run custom pre-commit hook\n.git/hooks/pre-commit.custom" > .git/hooks/pre-commit
        chmod +x .git/hooks/pre-commit
    fi
fi

pre-commit clean && pre-commit install && pre-commit install --hook-type commit-msg

print_green "================================================================"
print_green "🎉 Git hooks setup complete! Your repository now has:"
print_green "  - Sensitive information leak detection using gitleaks"
print_green "  - Chinese character detection in commit messages"
print_green "================================================================" 
