#!/usr/bin/env sh
export LANG=en_US.UTF-8
export PATH=$PATH:/opt/homebrew/bin

PROJECT_PATH=$PWD

if [ "$WORKSPACE" = "" ]; then
	WORKSPACE=$PWD
fi
if [ "$BUILD_NUMBER" = "" ]; then
	BUILD_NUMBER=888
fi


cd ${PROJECT_PATH}

echo "start download fu resource : $fu_lib"
curl -L -O "$fu_lib"
unzip -o vender_fu_iOS.zip
rm -f vender_fu_iOS.zip

# Enable third-party player configuration
sed -i -e "s#\#  pod 'ijkplayer'#  pod 'ijkplayer'#g" Podfile

# Enable third-party beauty filters
sed -i -e "s#\#  pod 'fuLib'#  pod 'fuLib'#g" Podfile

echo "work space: $WORKSPACE"
echo "project path: $PROJECT_PATH"


pod install --repo-update || exit 1

# Build configuration
CONFIGURATION="Debug"

# Project file path
APP_PATH="$(ls | grep xcworkspace)"

# Project target name
TARGET_NAME=${APP_PATH%%.*} 

KEYCENTER_PATH=$TARGET_NAME/Common/KeyCenter.swift

# Project configuration path
PBXPROJ_PATH=${TARGET_NAME}.xcodeproj/project.pbxproj

# Debug
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF72448758C00B599B3:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF72448758C00B599B3:buildSettings:CODE_SIGN_IDENTITY 'iPhone Distribution'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF72448758C00B599B3:buildSettings:DEVELOPMENT_TEAM 'YS397FG5PA'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF72448758C00B599B3:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'apiexample_wildcard_adhoc'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF72448758C00B599B3:buildSettings:PRODUCT_BUNDLE_IDENTIFIER io.agora.entfull" $PBXPROJ_PATH

# Release
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF82448758C00B599B3:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF82448758C00B599B3:buildSettings:CODE_SIGN_IDENTITY 'iPhone Distribution'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF82448758C00B599B3:buildSettings:DEVELOPMENT_TEAM 'YS397FG5PA'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF82448758C00B599B3:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'apiexample_wildcard_adhoc'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF82448758C00B599B3:buildSettings:PRODUCT_BUNDLE_IDENTIFIER io.agora.entfull" $PBXPROJ_PATH

# Screen Share Extension
# Debug
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB825205B80007D4FDD:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB825205B80007D4FDD:buildSettings:CODE_SIGN_IDENTITY 'iPhone Distribution'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB825205B80007D4FDD:buildSettings:DEVELOPMENT_TEAM 'YS397FG5PA'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB825205B80007D4FDD:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'apiexample_wildcard_adhoc'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB825205B80007D4FDD:buildSettings:PRODUCT_BUNDLE_IDENTIFIER io.agora.entfull.Agora-ScreenShare-Extension" $PBXPROJ_PATH

# Release
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB925205B80007D4FDD:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB925205B80007D4FDD:buildSettings:CODE_SIGN_IDENTITY 'iPhone Distribution'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB925205B80007D4FDD:buildSettings:DEVELOPMENT_TEAM 'YS397FG5PA'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB925205B80007D4FDD:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'apiexample_wildcard_adhoc'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB925205B80007D4FDD:buildSettings:PRODUCT_BUNDLE_IDENTIFIER io.agora.entfull.Agora-ScreenShare-Extension" $PBXPROJ_PATH

# SimpleFilter
# Debug
/usr/libexec/PlistBuddy -c "Set :objects:8B10BE1726AFFFA6002E1373:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:8B10BE1726AFFFA6002E1373:buildSettings:DEVELOPMENT_TEAM ''" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:8B10BE1726AFFFA6002E1373:buildSettings:PROVISIONING_PROFILE_SPECIFIER ''" $PBXPROJ_PATH
# Release
/usr/libexec/PlistBuddy -c "Set :objects:8B10BE1826AFFFA6002E1373:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:8B10BE1826AFFFA6002E1373:buildSettings:DEVELOPMENT_TEAM ''" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:8B10BE1826AFFFA6002E1373:buildSettings:PROVISIONING_PROFILE_SPECIFIER ''" $PBXPROJ_PATH

# Modify build number
# Debug
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF72448758C00B599B3:buildSettings:CURRENT_PROJECT_VERSION ${BUILD_NUMBER}" $PBXPROJ_PATH

# Release
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF82448758C00B599B3:buildSettings:CURRENT_PROJECT_VERSION ${BUILD_NUMBER}" $PBXPROJ_PATH

MODIFIED_BUNDLE_ID=$(/usr/libexec/PlistBuddy -c "Print :objects:03D13BF72448758C00B599B3:buildSettings:PRODUCT_BUNDLE_IDENTIFIER" "$PBXPROJ_PATH")
echo "Modified Bundle Identifier: $MODIFIED_BUNDLE_ID"

# Read APPID environment variable
echo AGORA_APP_ID: $APP_ID

echo PROJECT_PATH: $PROJECT_PATH
echo TARGET_NAME: $TARGET_NAME
echo KEYCENTER_PATH: $KEYCENTER_PATH
echo APP_PATH: $APP_PATH

# Modify KeyCenter file
sed -i -e "s#<\#YOUR AppId\#>#\"$APP_ID\"#g" $KEYCENTER_PATH
rm -f ${KEYCENTER_PATH}-e

# Archive path
ARCHIVE_PATH="${WORKSPACE}/${TARGET_NAME}_${BUILD_NUMBER}.xcarchive"

# plist path
PLIST_PATH="${PROJECT_PATH}/ExportOptions.plist"

# Modify ExportOptions.plist
# Modify the value of io.agora.api.examples
echo "start modify ExportOption.plist"
# Get original values first
value1=$(/usr/libexec/PlistBuddy -c "Print :provisioningProfiles:io.agora.api.examples" "$PLIST_PATH")
value2=$(/usr/libexec/PlistBuddy -c "Print :provisioningProfiles:io.agora.api.examples.Agora-ScreenShare-Extension" "$PLIST_PATH")

# Delete original keys
/usr/libexec/PlistBuddy -c "Delete :provisioningProfiles:io.agora.api.examples" "$PLIST_PATH"
/usr/libexec/PlistBuddy -c "Delete :provisioningProfiles:io.agora.api.examples.Agora-ScreenShare-Extension" "$PLIST_PATH"

# Add new keys and values
/usr/libexec/PlistBuddy -c "Add :provisioningProfiles:io.agora.entfull string $value1" "$PLIST_PATH"
/usr/libexec/PlistBuddy -c "Add :provisioningProfiles:io.agora.entfull.Agora-ScreenShare-Extension string $value2" "$PLIST_PATH"

# Print modified provisioningProfiles values
echo "Modified provisioningProfiles values:"
/usr/libexec/PlistBuddy -c "Print :provisioningProfiles" "$PLIST_PATH"

echo "start xcode build, appPath: $APP_PATH, target: $TARGET_NAME, config: $CONFIGURATION, archivePath: $ARCHIVE_PATH"

# Archive using workspace, can also use project
# Disable code signing during archive phase to avoid Pods Framework signing issues
# Code signing will be performed during export phase according to ExportOptions.plist
xcodebuild CODE_SIGN_STYLE="Manual" \
  -workspace "${APP_PATH}" \
  -scheme "${TARGET_NAME}" \
  clean \
  CODE_SIGNING_REQUIRED=NO \
  CODE_SIGNING_ALLOWED=NO \
  -configuration "${CONFIGURATION}" \
  archive \
  -archivePath "${ARCHIVE_PATH}" \
  -destination 'generic/platform=iOS' \
  -quiet || exit 1

echo "xcode build finished"

cd ${WORKSPACE}

# Print certificates installed on current device (for debugging)
echo "=========================================="
echo "Code signing certificates installed on current device:"
echo "=========================================="
security find-identity -v -p codesigning | grep -E "(iPhone Distribution|Apple Distribution|iOS Distribution)" || security find-identity -v -p codesigning
echo "=========================================="
echo ""

# Print ExportOptions.plist content (for debugging)
echo "=========================================="
echo "ExportOptions.plist configuration content:"
echo "=========================================="
cat "${PLIST_PATH}"
echo "=========================================="
echo ""

# Export IPA (using xcodebuild directly, consistent with Xcode manual export)
EXPORT_PATH="${WORKSPACE}/export"
rm -rf "${EXPORT_PATH}"
mkdir -p "${EXPORT_PATH}"

security unlock-keychain -p "123456" ~/Library/Keychains/login.keychain

echo "Starting IPA export..."
xcodebuild -exportArchive \
  -archivePath "${ARCHIVE_PATH}" \
  -exportPath "${EXPORT_PATH}" \
  -exportOptionsPlist "${PLIST_PATH}" \
  -allowProvisioningUpdates || exit 1

# Rename and move IPA file
SDK_VERSION=$(echo $sdk_url | cut -d "/" -f 5)
OUTPUT_FILE=${WORKSPACE}/${TARGET_NAME}_${BUILD_NUMBER}_${SDK_VERSION}_$(date "+%Y%m%d%H%M%S").ipa
mv ${EXPORT_PATH}/${TARGET_NAME}.ipa $OUTPUT_FILE

# Clean up temporary files
rm -rf "${EXPORT_PATH}"
rm -rf "${ARCHIVE_PATH}"
echo OUTPUT_FILE: $OUTPUT_FILE


