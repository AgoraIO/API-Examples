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

echo compile_project:$compile_project
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
apiexample_cn_name=Shengwang_Native_SDK_for_Mac
apiexample_global_name=Agora_Native_SDK_for_Mac
cn_dir=CN
global_dir=Global

# ===================================
# Version validation logic
# ===================================
echo "=========================================="
echo "Starting branch version validation..."
echo "=========================================="

# Get current branch name (try multiple methods for CI environments)
BRANCH_NAME=""

# Method 1: Try environment variable (Jenkins/GitLab CI)
if [ ! -z "$GIT_BRANCH" ]; then
    BRANCH_NAME="$GIT_BRANCH"
    echo "Branch from GIT_BRANCH: $BRANCH_NAME"
elif [ ! -z "$BRANCH_NAME" ]; then
    echo "Branch from BRANCH_NAME: $BRANCH_NAME"
elif [ ! -z "$CI_COMMIT_REF_NAME" ]; then
    BRANCH_NAME="$CI_COMMIT_REF_NAME"
    echo "Branch from CI_COMMIT_REF_NAME: $BRANCH_NAME"
# Method 2: Try git command
elif [ -z "$BRANCH_NAME" ]; then
    BRANCH_NAME=$(git rev-parse --abbrev-ref HEAD 2>/dev/null)
    if [ "$BRANCH_NAME" = "HEAD" ]; then
        # In detached HEAD state, try to get branch from remote
        BRANCH_NAME=$(git branch -r --contains HEAD | grep -v HEAD | head -1 | sed 's/^[[:space:]]*origin\///')
        echo "Branch from git branch -r: $BRANCH_NAME"
    else
        echo "Branch from git rev-parse: $BRANCH_NAME"
    fi
fi

# Remove origin/ prefix if present (but keep the rest of the path)
BRANCH_NAME=$(echo "$BRANCH_NAME" | sed 's/^origin\///')

if [ -z "$BRANCH_NAME" ] || [ "$BRANCH_NAME" = "HEAD" ]; then
    echo "Warning: Unable to get Git branch name, skipping version validation"
else
    echo "Current branch: $BRANCH_NAME"
    
    # Extract version from branch name (format: dev/x.x.x)
    if [[ $BRANCH_NAME =~ ^dev/([0-9]+\.[0-9]+\.[0-9]+)$ ]]; then
        BRANCH_VERSION="${BASH_REMATCH[1]}"
        echo "Branch version: $BRANCH_VERSION"
        echo "Current building project: macOS/APIExample"
        echo ""
        
        # Validate the macOS project
        PROJECT_PATH="macOS"
        PROJECT_NAME="APIExample"
        
        echo "-----------------------------------"
        echo "Validating: $PROJECT_PATH"
        
        PBXPROJ_FILE="${PROJECT_PATH}/${PROJECT_NAME}.xcodeproj/project.pbxproj"
        
        if [ ! -f "$PBXPROJ_FILE" ]; then
            echo "Error: project.pbxproj file not found: $PBXPROJ_FILE"
            exit 1
        fi
        
        # Extract MARKETING_VERSION for main target
        PLIST_VERSION=$(grep "MARKETING_VERSION" "$PBXPROJ_FILE" | grep -v "//" | head -1 | sed 's/.*MARKETING_VERSION = \([^;]*\);/\1/' | tr -d ' ')
        
        if [ -z "$PLIST_VERSION" ]; then
            echo "Error: Unable to read MARKETING_VERSION from project.pbxproj"
            exit 1
        fi
        
        echo "Project version: $PLIST_VERSION"
        echo "-----------------------------------"
        
        # Compare versions
        if [ "$BRANCH_VERSION" != "$PLIST_VERSION" ]; then
            echo ""
            echo "=========================================="
            echo "Error: Version mismatch!"
            echo "=========================================="
            echo "  Branch version:  $BRANCH_VERSION"
            echo "  Project version: $PLIST_VERSION"
            echo "  Project path:    $PROJECT_PATH"
            echo ""
            echo "Please ensure the version in branch name matches MARKETING_VERSION in Info.plist"
            echo ""
            exit 1
        fi
        
        echo "✓ Version validation passed: $BRANCH_VERSION"
    else
        echo "Warning: Branch name does not match dev/x.x.x format!"
        echo "Current branch: $BRANCH_NAME"
        echo "Expected format: dev/x.x.x (e.g., dev/4.5.3)"
        echo "Skipping version validation for non-version branches..."
    fi
fi

echo "Version validation completed"
echo "=========================================="
echo ""

echo zip_name: $zip_name
if [ -z "$sdk_url" -o "$sdk_url" = "none" ]; then
   sdk_url_flag=false
   echo "sdk_url is empty"
   echo unzip_name: $unzip_name 
   mkdir -p ./$unzip_name/samples
   cp -rf ./macOS ./$unzip_name/samples/APIExample || exit 1
   ls -al ./$unzip_name/samples/API-Example/
else 
   sdk_url_flag=true
   zip_name=${sdk_url##*/}
   echo unzip_name: $unzip_name
   curl -o $zip_name $sdk_url || exit 1
   7za x ./$zip_name -y > log.txt
   unzip_name=`ls -S -d */ | egrep 'Agora|Shengwang' | sed 's/\///g'`
   echo unzip_name: $unzip_name

   rm -rf ./$unzip_name/bin
   rm -f ./$unzip_name/commits
   rm -f ./$unzip_name/package_size_report.txt
   rm -f ./$unzip_name/.commits
   rm -f ./$unzip_name/AgoraInfra_macOS.swift
   rm -f ./$unzip_name/AgoraRtcEngine_macOS.podspec
   rm -f ./$unzip_name/Package.swift
   
   mkdir ./$unzip_name/samples
   cp -rf ./macOS ./$unzip_name/samples/APIExample || exit 1
   ls -al ./$unzip_name/samples/API-Example/
   mv ./$unzip_name/samples/APIExample/sdk.podspec ./$unzip_name/
fi

python3 ./.github/ci/build/modify_podfile.py ./$unzip_name/samples/APIExample/Podfile $sdk_url_flag

echo "start compress"
7za a -tzip result.zip -r $unzip_name > log.txt
echo "start move to"
echo $WORKSPACE/with${BUILD_NUMBER}_$zip_name
mv result.zip $WORKSPACE/with_${BUILD_NUMBER}_$zip_name

if [ $compress_apiexample = true ]; then
    sdk_version=$(grep "pod 'AgoraRtcEngine_macOS'" ./macOS/Podfile | sed -n "s/.*'\([0-9.]*\)'.*/\1/p")
    echo "sdk_version: $sdk_version"
    
    cp -rf ./macOS $global_dir/
   
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



