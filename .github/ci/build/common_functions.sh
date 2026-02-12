#!/usr/bin/env bash

# Common functions for iOS/macOS build scripts
# This file contains reusable functions for version validation

# Function: Get current git branch name
# Tries multiple methods to determine the branch name in CI environments
# Returns: Branch name (without origin/ prefix)
get_branch_name() {
    local branch_name=""
    
    # Method 1: Try environment variables (Jenkins/GitLab CI)
    if [ ! -z "$GIT_BRANCH" ]; then
        branch_name="$GIT_BRANCH"
        echo "Branch from GIT_BRANCH: $branch_name" >&2
    elif [ ! -z "$BRANCH_NAME" ]; then
        branch_name="$BRANCH_NAME"
        echo "Branch from BRANCH_NAME: $branch_name" >&2
    elif [ ! -z "$CI_COMMIT_REF_NAME" ]; then
        branch_name="$CI_COMMIT_REF_NAME"
        echo "Branch from CI_COMMIT_REF_NAME: $branch_name" >&2
    # Method 2: Try git command
    else
        branch_name=$(git rev-parse --abbrev-ref HEAD 2>/dev/null)
        if [ "$branch_name" = "HEAD" ]; then
            # In detached HEAD state, try to get branch from remote
            branch_name=$(git branch -r --contains HEAD | grep -v HEAD | head -1 | sed 's/^[[:space:]]*origin\///')
            echo "Branch from git branch -r: $branch_name" >&2
        else
            echo "Branch from git rev-parse: $branch_name" >&2
        fi
    fi
    
    # Remove origin/ prefix if present (but keep the rest of the path)
    branch_name=$(echo "$branch_name" | sed 's/^origin\///')
    
    echo "$branch_name"
}

# Function: Extract version from branch name
# Args:
#   $1 - Branch name
# Returns: Version string (e.g., "4.6.2") or empty if not in dev/x.x.x format
extract_branch_version() {
    local branch_name="$1"
    
    if [[ $branch_name =~ ^dev/([0-9]+\.[0-9]+\.[0-9]+)$ ]]; then
        echo "${BASH_REMATCH[1]}"
    else
        echo ""
    fi
}

# Function: Validate project MARKETING_VERSION against branch version
# Args:
#   $1 - Project path (e.g., "iOS/APIExample" or "macOS")
#   $2 - Project name (e.g., "APIExample")
#   $3 - Branch version (e.g., "4.6.2")
#   $4 - Platform (optional: "ios" or "macos", defaults to "ios")
# Returns: 0 on success, 1 on failure
validate_project_version() {
    local project_path="$1"
    local project_name="$2"
    local branch_version="$3"
    local platform="${4:-ios}"
    
    echo "-----------------------------------"
    echo "Validating project version: $project_path"
    
    local pbxproj_file="${project_path}/${project_name}.xcodeproj/project.pbxproj"
    
    if [ ! -f "$pbxproj_file" ]; then
        echo "Error: project.pbxproj file not found: $pbxproj_file"
        return 1
    fi
    
    # Extract MARKETING_VERSION for main target (skip Extension targets)
    local plist_version=""
    if [ "$platform" = "macos" ]; then
        # macOS uses @executable_path/../Frameworks or just MARKETING_VERSION
        plist_version=$(grep "MARKETING_VERSION" "$pbxproj_file" | grep -v "//" | head -1 | sed 's/.*MARKETING_VERSION = \([^;]*\);/\1/' | tr -d ' ')
    else
        # iOS uses @executable_path/Frameworks
        plist_version=$(grep -A 2 "@executable_path/Frameworks" "$pbxproj_file" | grep "MARKETING_VERSION" | head -1 | sed 's/.*MARKETING_VERSION = \([^;]*\);/\1/' | tr -d ' ')
    fi
    
    if [ -z "$plist_version" ]; then
        echo "Error: Unable to read MARKETING_VERSION from project.pbxproj"
        return 1
    fi
    
    echo "Project version: $plist_version"
    
    # Compare versions
    if [ "$branch_version" != "$plist_version" ]; then
        echo ""
        echo "=========================================="
        echo "Error: Version mismatch!"
        echo "=========================================="
        echo "  Branch version:  $branch_version"
        echo "  Project version: $plist_version"
        echo "  Project path:    $project_path"
        echo ""
        echo "Please ensure the version in branch name matches MARKETING_VERSION in Info.plist"
        echo ""
        return 1
    fi
    
    echo "✓ Project version matches: $plist_version"
    return 0
}

# Function: Validate SDK version in Podfile against branch version
# Args:
#   $1 - Podfile path
#   $2 - Branch version
#   $3 - SDK type (optional: "ios" or "macos", defaults to "ios")
# Returns: 0 on success, 1 on failure
validate_sdk_version() {
    local podfile_path="$1"
    local branch_version="$2"
    local sdk_type="${3:-ios}"
    
    echo "-----------------------------------"
    echo "Validating SDK version: $podfile_path"
    
    # Extract SDK version from Podfile
    # Support commented lines (lines starting with #)
    local sdk_version=""
    if [ "$sdk_type" = "macos" ]; then
        sdk_version=$(grep -E "^[[:space:]]*#?[[:space:]]*pod[[:space:]]+'AgoraRtcEngine_macOS'" "$podfile_path" | sed -n "s/.*'\([0-9.]*\)'.*/\1/p" | head -1)
    else
        # iOS: support both AgoraRtcEngine_iOS and AgoraAudio_iOS
        sdk_version=$(grep -E "^[[:space:]]*#?[[:space:]]*pod[[:space:]]+'AgoraRtcEngine_iOS'" "$podfile_path" | sed -n "s/.*'\([0-9.]*\)'.*/\1/p" | head -1)
        if [ -z "$sdk_version" ]; then
            sdk_version=$(grep -E "^[[:space:]]*#?[[:space:]]*pod[[:space:]]+'AgoraAudio_iOS'" "$podfile_path" | sed -n "s/.*'\([0-9.]*\)'.*/\1/p" | head -1)
        fi
    fi
    
    if [ -z "$sdk_version" ]; then
        echo "Error: Unable to extract SDK version from Podfile"
        echo "Podfile path: $podfile_path"
        return 1
    fi
    
    echo "SDK version: $sdk_version"
    
    # Compare versions
    if [ "$branch_version" != "$sdk_version" ]; then
        echo ""
        echo "=========================================="
        echo "Error: SDK version mismatch!"
        echo "=========================================="
        echo "  Branch version: $branch_version"
        echo "  SDK version:    $sdk_version"
        echo "  Podfile path:   $podfile_path"
        echo ""
        echo "Please ensure the SDK version in Podfile matches the branch version."
        echo ""
        return 1
    fi
    
    echo "✓ SDK version matches: $sdk_version"
    return 0
}

# Function: Main version validation entry point
# Args:
#   $1 - Project path
#   $2 - Project name
#   $3 - Platform ("ios" or "macos")
# Returns: 0 on success, 1 on failure
# Sets global variable: BRANCH_VERSION
run_version_validation() {
    local project_path="$1"
    local project_name="$2"
    local platform="$3"
    
    echo "=========================================="
    echo "Starting branch version validation..."
    echo "=========================================="
    
    BRANCH_NAME=$(get_branch_name)
    
    if [ -z "$BRANCH_NAME" ] || [ "$BRANCH_NAME" = "HEAD" ]; then
        echo "Error: Unable to get Git branch name"
        echo "Please ensure you are on a valid branch"
        return 1
    fi
    
    echo "Current branch: $BRANCH_NAME"
    
    # Special case: main branch skips all validation
    if [ "$BRANCH_NAME" = "main" ]; then
        echo "Branch is 'main', skipping version validation (main branch is trusted)"
        BRANCH_VERSION=""
        echo "Version validation completed"
        echo "=========================================="
        echo ""
        return 0
    fi
    
    # Extract version from branch name (format: dev/x.x.x)
    BRANCH_VERSION=$(extract_branch_version "$BRANCH_NAME")
    
    if [ -z "$BRANCH_VERSION" ]; then
        echo ""
        echo "=========================================="
        echo "Error: Branch naming is not compliant!"
        echo "=========================================="
        echo "Current branch: $BRANCH_NAME"
        echo "Required format: dev/x.x.x (e.g., dev/4.6.2)"
        echo ""
        echo "Branch naming rules:"
        echo "  - Version branches must follow: dev/x.x.x"
        echo "  - Main branch: main (skips validation)"
        echo ""
        return 1
    fi
    
    echo "Branch version: $BRANCH_VERSION"
    echo "Current building project: $project_path"
    echo ""
    
    # Validate project version
    if ! validate_project_version "$project_path" "$project_name" "$BRANCH_VERSION" "$platform"; then
        return 1
    fi
    
    echo "-----------------------------------"
    echo "✓ All version validations passed: $BRANCH_VERSION"
    echo "Version validation completed"
    echo "=========================================="
    echo ""
    
    return 0
}
