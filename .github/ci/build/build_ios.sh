#!/bin/bash -ilex
##################################
# --- Guidelines: ---
#
# Common Environment Variable:
# 'Package_Publish:boolean:true',
# 'Clean_Clone:boolean:false',
# 'is_tag_fetch:boolean:false',
# 'is_offical_build:boolean:false',
# 'repo:string',
# 'base:string',
# 'arch:string'
# 'output:string'
# 'short_version:string'
# 'release_version:string'
# 'build_date:string(yyyyMMdd)',
# 'build_timestamp:string (yyyyMMdd_hhmm)',
# 'platform: string',
# 'BUILD_NUMBER: string',
# 'WORKSPACE: string'
#
# --- Test Related: ---
#   PR build, zip test related to test.zip
#   Package build, zip package related to package.zip
# --- Artifactory Related: ---
# download artifactory:
# python3 ${WORKSPACE}/artifactory_utils.py --action=download_file --file=ARTIFACTORY_URL
# upload file to artifactory:
# python3 ${WORKSPACE}/artifactory_utils.py --action=upload_file --file=FILEPATTERN --server_path=SERVERPATH --server_repo=SERVER_REPO --with_pattern
# for example: python3 ${WORKSPACE}/artifactory_utils.py --action=upload_file --file=*.zip --server_path=windows/ --server_repo=ACCS_repo --with_pattern
# upload folder to artifactory
# python3 ${WORKSPACE}/artifactory_utils.py --action=upload_file --file=FILEPATTERN --server_path=SERVERPATH --server_repo=SERVER_REPO --with_folder
# for example: python3 ${WORKSPACE}/artifactory_utils.py --action=upload_file --file=*.zip --server_path=windows/ --server_repo=ACCS_repo --with_folder
# --- Input: ----
# sourcePath: see jenkins console for details.
# WORKSPACE: ${WORKSPACE}
# --- Output: ----
# pr: output test.zip to workspace dir
# others: Rename the zip package name yourself, But need copy it to workspace dir
##################################
export PATH=$PATH:/opt/homebrew/bin

xcode_version=$(xcodebuild -version | grep Xcode | awk '{print $2}')
echo "Xcode Version: $xcode_version"
echo ios_direction: $ios_direction
echo Package_Publish: $Package_Publish
echo is_tag_fetch: $is_tag_fetch
echo arch: $arch
echo source_root: %source_root%
echo output: /tmp/jenkins/${project}_out
echo build_date: $build_date
echo build_time: $build_time
echo release_version: $release_version
echo short_version: $short_version
echo pwd: `pwd`
echo sdk_url: $sdk_url

export https_proxy=10.10.114.55:1080
export http_proxy=10.10.114.55:1080
export all_proxy=10.10.114.55:1080
export LANG=en_US.UTF-8

unzip_name=Agora_Native_SDK_for_iOS_FULL
zip_name=output.zip
sdk_url_flag=false
apiexample_cn_name=Shengwang_Native_SDK_for_iOS
apiexample_global_name=Agora_Native_SDK_for_iOS
global_dir=Global

# ===================================
# Version Validation Functions
# ===================================

# Function: Get current branch name from various sources
get_branch_name() {
    local branch_name=""
    
    # Method 1: Try environment variable (Jenkins/GitLab CI)
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

# Function: Validate project MARKETING_VERSION against branch version
validate_project_version() {
    local project_path="$1"
    local project_name="$2"
    local branch_version="$3"
    
    echo "-----------------------------------"
    echo "Validating project version: $project_path"
    
    local pbxproj_file="${project_path}/${project_name}.xcodeproj/project.pbxproj"
    
    if [ ! -f "$pbxproj_file" ]; then
        echo "Error: project.pbxproj file not found: $pbxproj_file"
        return 1
    fi
    
    # Extract MARKETING_VERSION for main target (skip Extension targets)
    local plist_version=$(grep -A 2 "@executable_path/Frameworks" "$pbxproj_file" | grep "MARKETING_VERSION" | head -1 | sed 's/.*MARKETING_VERSION = \([^;]*\);/\1/' | tr -d ' ')
    
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
validate_sdk_version() {
    local podfile_path="$1"
    local branch_version="$2"
    
    echo "-----------------------------------"
    echo "Validating SDK version: $podfile_path"
    
    # Extract SDK version from Podfile (support both AgoraRtcEngine_iOS and AgoraAudio_iOS)
    # Also support commented lines (lines starting with #)
    local sdk_version=$(grep -E "^[[:space:]]*#?[[:space:]]*pod[[:space:]]+'AgoraRtcEngine_iOS'" "$podfile_path" | sed -n "s/.*'\([0-9.]*\)'.*/\1/p" | head -1)
    if [ -z "$sdk_version" ]; then
        sdk_version=$(grep -E "^[[:space:]]*#?[[:space:]]*pod[[:space:]]+'AgoraAudio_iOS'" "$podfile_path" | sed -n "s/.*'\([0-9.]*\)'.*/\1/p" | head -1)
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

# Main version validation logic
echo "=========================================="
echo "Starting branch version validation..."
echo "=========================================="

BRANCH_NAME=$(get_branch_name)

if [ -z "$BRANCH_NAME" ] || [ "$BRANCH_NAME" = "HEAD" ]; then
    echo "Warning: Unable to get Git branch name, skipping version validation"
    BRANCH_VERSION=""
else
    echo "Current branch: $BRANCH_NAME"
    
    # Extract version from branch name (format: dev/x.x.x)
    if [[ $BRANCH_NAME =~ ^dev/([0-9]+\.[0-9]+\.[0-9]+)$ ]]; then
        BRANCH_VERSION="${BASH_REMATCH[1]}"
        echo "Branch version: $BRANCH_VERSION"
        echo "Current building project: $ios_direction"
        echo ""
        
        # Validate project version
        PROJECT_PATH="iOS/${ios_direction}"
        PROJECT_NAME="${ios_direction}"
        
        if ! validate_project_version "$PROJECT_PATH" "$PROJECT_NAME" "$BRANCH_VERSION"; then
            exit 1
        fi
        
        echo "-----------------------------------"
        echo "✓ All version validations passed: $BRANCH_VERSION"
    else
        echo "Warning: Branch name does not match dev/x.x.x format!"
        echo "Current branch: $BRANCH_NAME"
        echo "Expected format: dev/x.x.x (e.g., dev/4.5.3)"
        echo "Skipping version validation for non-version branches..."
        BRANCH_VERSION=""
    fi
fi

echo "Version validation completed"
echo "=========================================="
echo ""



if [ -z "$sdk_url" -o "$sdk_url" = "none" ]; then
   sdk_url_flag=false
   echo "sdk_url is empty"
   echo unzip_name: $unzip_name 
   mkdir -p ./$unzip_name/samples
   cp -rf ./iOS/${ios_direction} ./$unzip_name/samples/${ios_direction} || exit 1
   ls -al ./$unzip_name/samples/${ios_direction}/
else
   sdk_url_flag=true
   zip_name=${sdk_url##*/}
   echo zip_name: $zip_name
   curl -o $zip_name $sdk_url || exit 1
   7za x ./$zip_name -y > log.txt
   unzip_name=`ls -S -d */ | egrep 'Agora|Shengwang' | sed 's/\///g'`
   echo unzip_name: $unzip_name
   rm -rf ./$unzip_name/bin
   rm -f ./$unzip_name/commits
   rm -f ./$unzip_name/package_size_report.txt

   rm -f ./$unzip_name/.commits
   rm -f ./$unzip_name/AgoraInfra_iOS.swift
   rm -f ./$unzip_name/AgoraRtcEngine_iOS.podspec
   rm -f ./$unzip_name/AgoraAudio_iOS.podspec
   rm -f ./$unzip_name/Package.swift
   mkdir -p ./$unzip_name/samples
   cp -rf ./iOS/${ios_direction} ./$unzip_name/samples/${ios_direction} || exit 1
   ls -al ./$unzip_name/samples/${ios_direction}/
   mv ./$unzip_name/samples/${ios_direction}/sdk.podspec ./$unzip_name/ || exit 1
fi

python3 ./.github/ci/build/modify_podfile.py ./$unzip_name/samples/${ios_direction}/Podfile $sdk_url_flag || exit 1

echo "start compress"
7za a -tzip result.zip -r $unzip_name > log.txt
echo "start move to"
echo $WORKSPACE/with${ios_direction}_${BUILD_NUMBER}_$zip_name
mv result.zip $WORKSPACE/with${ios_direction}_${BUILD_NUMBER}_$zip_name

if [ $compress_apiexample = true ]; then
    # Extract SDK version from Podfile (support both AgoraRtcEngine_iOS and AgoraAudio_iOS)
    # Also support commented lines
    sdk_version=$(grep -E "^[[:space:]]*#?[[:space:]]*pod[[:space:]]+'AgoraRtcEngine_iOS'" ./iOS/${ios_direction}/Podfile | sed -n "s/.*'\([0-9.]*\)'.*/\1/p" | head -1)
    if [ -z "$sdk_version" ]; then
        sdk_version=$(grep -E "^[[:space:]]*#?[[:space:]]*pod[[:space:]]+'AgoraAudio_iOS'" ./iOS/${ios_direction}/Podfile | sed -n "s/.*'\([0-9.]*\)'.*/\1/p" | head -1)
    fi
    echo "sdk_version: $sdk_version"
    
    # Validate SDK version matches branch version (if on dev/x.x.x branch)
    if [ ! -z "$BRANCH_VERSION" ]; then
        if ! validate_sdk_version "./iOS/${ios_direction}/Podfile" "$BRANCH_VERSION"; then
            exit 1
        fi
    else
        echo "Skipping SDK version validation (not on dev/x.x.x branch)"
    fi
    
    cp -rf ./iOS/${ios_direction} $global_dir/
   
    echo "start compress api example"
    7za a -tzip global_result.zip $global_dir    
    echo "complete compress api example"
    echo "current path: `pwd`"
    ls -al
    global_des_path=$WORKSPACE/${apiexample_global_name}_v${sdk_version}_APIExample_${BUILD_NUMBER}.zip
    echo "global_des_path: $global_des_path"
    echo "Moving global_result.zip to $global_des_path"
    mv global_result.zip $global_des_path
    
    ls -al $WORKSPACE/
fi 

if [ $compile_project = true ]; then
	cd ./$unzip_name/samples/${ios_direction}
	./cloud_build.sh || exit 1
	cd -
fi

