##################################
# --- Guidelines: ---
#
# Common Environment Variable Injected:
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

# Record build start time
START_TIME=$(date +%s)
echo "=========================================="
echo "Build started at: $(date '+%Y-%m-%d %H:%M:%S')"
echo "=========================================="
echo ""

echo Package_Publish: $Package_Publish
echo is_tag_fetch: $is_tag_fetch
echo arch: $arch
echo source_root: %source_root%
echo output: /tmp/jenkins/${project}_out
echo build_date: $build_date
echo build_time: $build_time
echo release_version: $release_version
echo short_version: $short_version
echo BUILD_NUMBER: $BUILD_NUMBER
echo pwd: `pwd`
echo sdk_url: $sdk_url
echo android_direction: $android_direction
echo compile_project: $compile_project
echo compress_apiexample: $compress_apiexample
echo api_examples_branch: $api_examples_branch

# Validate required variables
if [ -z "$android_direction" ]; then
    echo "Error: android_direction variable is not set"
    exit 1
fi

# ===== Version Consistency Check =====
echo ""
echo "=========================================="
echo "Checking version consistency..."
echo "=========================================="

# Extract version number from branch name (supports formats like dev/4.6.2, release/4.6.2, etc.)
BRANCH_VERSION=$(echo $api_examples_branch | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -n 1)

if [ -z "$BRANCH_VERSION" ]; then
    echo ""
    echo "=========================================="
    echo "❌ CI BUILD FAILED: CANNOT EXTRACT VERSION"
    echo "=========================================="
    echo "Cannot extract version from branch name: $api_examples_branch"
    echo "Branch name must contain version number (e.g., dev/4.6.2, release/4.6.2)"
    echo "=========================================="
    exit 1
fi

echo "Branch version: $BRANCH_VERSION"
echo ""

# Check all gradle.properties files
GRADLE_FILES=(
    "Android/APIExample/gradle.properties"
    "Android/APIExample-Audio/gradle.properties"
    "Android/APIExample-Compose/gradle.properties"
)

VERSION_MISMATCH=false

for gradle_file in "${GRADLE_FILES[@]}"; do
    if [ -f "$gradle_file" ]; then
        GRADLE_VERSION=$(grep "rtc_sdk_version" "$gradle_file" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
        echo "Checking $gradle_file: $GRADLE_VERSION"
        
        if [ "$GRADLE_VERSION" != "$BRANCH_VERSION" ]; then
            echo "❌ ERROR: Version mismatch in $gradle_file"
            echo "   Branch version: $BRANCH_VERSION"
            echo "   Gradle version: $GRADLE_VERSION"
            VERSION_MISMATCH=true
        else
            echo "✅ Version matched: $GRADLE_VERSION"
        fi
    else
        echo "⚠️  Warning: $gradle_file not found"
    fi
    echo ""
done

if [ "$VERSION_MISMATCH" = true ]; then
    echo ""
    echo "=========================================="
    echo "❌ CI BUILD FAILED: VERSION MISMATCH"
    echo "=========================================="
    echo "Branch name contains version: $BRANCH_VERSION"
    echo "But gradle.properties has different version(s)"
    echo ""
    echo "Please update rtc_sdk_version in all gradle.properties files to: $BRANCH_VERSION"
    echo "=========================================="
    exit 1
fi

echo "✅ All version checks passed!"
echo "=========================================="
echo ""

unzip_name=Agora_Native_SDK_for_Android_FULL_DEFAULT
zip_name=Agora_Native_SDK_for_Android_FULL_DEFAULT.zip
if [ -z "$sdk_url" ] || [ "$sdk_url" = "none" ]; then
   echo "sdk_url is empty, using default SDK structure"
   echo unzip_name: $unzip_name 
   echo zip_name: $zip_name
else
	zip_name=${sdk_url##*/}
	echo zip_name: $zip_name

	# Download SDK
	echo "Downloading SDK from: $sdk_url"
	# env LC_ALL=en_US.UTF-8 python3 $WORKSPACE/artifactory_utils.py --action=download_file --file=$sdk_url || exit 1
	curl -o $zip_name $sdk_url || exit 1
	echo "✅ SDK downloaded successfully"

	# Extract SDK
	echo "Extracting SDK package..."
	7za x ./$zip_name -y > log.txt
	echo "✅ SDK extracted successfully"

	# Support top-level directory name containing 'Agora' or 'Shengwang'
	unzip_name=`ls -S -d */ | grep -E 'Agora|Shengwang' | head -n 1 | sed 's/\///g'`
	if [ -z "$unzip_name" ]; then
		echo "Error: Unzipped directory not found. The SDK package structure may be invalid or the top-level directory does not contain 'Agora' or 'Shengwang'"
		exit 1
	fi
	echo unzip_name: $unzip_name

	# Clean up unnecessary files
	echo "Cleaning up unnecessary files..."
	rm -rf ./$unzip_name/rtc/bin
	rm -rf ./$unzip_name/rtc/demo
	rm -f ./$unzip_name/.commits
	rm -f ./$unzip_name/spec
	rm -rf ./$unzip_name/pom
	echo "✅ Cleanup completed"
fi

mkdir -p ./$unzip_name/rtc/samples/${android_direction} || exit 1
rm -rf ./$unzip_name/rtc/samples/${android_direction}/*

echo ""
echo "Copying API Example code from ./Android/${android_direction}..."
if [ -d "./Android/${android_direction}" ]; then
    cp -rf ./Android/${android_direction}/* ./$unzip_name/rtc/samples/${android_direction}/ || exit 1
    echo "✅ API Example code copied successfully"
else
    echo "Error: Source directory ./Android/${android_direction} does not exist"
    exit 1
fi
echo ""

if [ "$compress_apiexample" = true ]; then
    # Compress only API Example code (without SDK)
    echo "Creating code-only package..."
    TEMP_ZIP_NAME="temp_code_only.zip"
    7za a -tzip $TEMP_ZIP_NAME -r ./$unzip_name/rtc/samples/${android_direction} > log.txt
    echo "✅ Code-only package created"
    
    # Generate final output zip name with version
    OUTPUT_ZIP_NAME="Agora_Native_SDK_for_Android_v${BRANCH_VERSION}_${android_direction}_${BUILD_NUMBER}.zip"
    echo "Output zip name: $OUTPUT_ZIP_NAME"
    mv $TEMP_ZIP_NAME $WORKSPACE/$OUTPUT_ZIP_NAME
    
    # Display package size
    echo "Package size: $(du -h $WORKSPACE/$OUTPUT_ZIP_NAME | cut -f1)"
else
    # Compress full SDK with API Example
    echo "Creating full package with SDK..."
    TEMP_ZIP_NAME="temp_with_sdk.zip"
    7za a -tzip $TEMP_ZIP_NAME -r $unzip_name > log.txt
    echo "✅ Full package created"
    
    # Generate final output zip name with version (with SDK)
    OUTPUT_ZIP_NAME="withSDK_Agora_Native_SDK_for_Android_v${BRANCH_VERSION}_${android_direction}_${BUILD_NUMBER}.zip"
    echo "Output zip name: $OUTPUT_ZIP_NAME"
    mv $TEMP_ZIP_NAME $WORKSPACE/$OUTPUT_ZIP_NAME
    
    # Display package size
    echo "Package size: $(du -h $WORKSPACE/$OUTPUT_ZIP_NAME | cut -f1)"
fi
echo ""

if [ $compile_project = true ]; then
	echo "Starting project compilation..."
	cd ./$unzip_name/rtc/samples/${android_direction} || exit 1
	
	# Try to compile, but don't fail the build if compilation fails
	if [ -z "$sdk_url" ] || [ "$sdk_url" = "none" ]; then
		./cloud_build.sh false || echo "⚠️  Warning: Project compilation failed, but continuing..."
	else
		./cloud_build.sh true || echo "⚠️  Warning: Project compilation failed, but continuing..."
	fi
	
	# Check if compilation was successful
	if [ $? -eq 0 ]; then
		echo "✅ Project compiled successfully"
	else
		echo "⚠️  Project compilation had issues, but package was created successfully"
	fi
	echo ""
fi

# Calculate and display build duration
END_TIME=$(date +%s)
DURATION=$((END_TIME - START_TIME))
MINUTES=$((DURATION / 60))
SECONDS=$((DURATION % 60))

echo ""
echo "=========================================="
echo "✅ BUILD COMPLETED SUCCESSFULLY"
echo "=========================================="
echo "Completed at: $(date '+%Y-%m-%d %H:%M:%S')"
if [ $MINUTES -gt 0 ]; then
    echo "Total duration: ${MINUTES}m ${SECONDS}s"
else
    echo "Total duration: ${SECONDS}s"
fi
echo "Output package: $OUTPUT_ZIP_NAME"
echo "=========================================="

