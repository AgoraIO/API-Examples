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

cd ${PROJECT_PATH} && pod install || exit 1

# Build configuration
CONFIGURATION="Debug"

# Project file path
APP_PATH="$(ls | grep xcworkspace)"

# Project target name
TARGET_NAME=${APP_PATH%%.*} 

KEYCENTER_PATH=$TARGET_NAME/Common/KeyCenter.m

# Project configuration path
PBXPROJ_PATH=${TARGET_NAME}.xcodeproj/project.pbxproj

# Debug
/usr/libexec/PlistBuddy -c "Set :objects:E70ADE062A5D0050009947CF:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E70ADE062A5D0050009947CF:buildSettings:CODE_SIGN_IDENTITY 'iPhone Distribution'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E70ADE062A5D0050009947CF:buildSettings:DEVELOPMENT_TEAM 'YS397FG5PA'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E70ADE062A5D0050009947CF:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'apiexample_wildcard_adhoc'" $PBXPROJ_PATH
# Release
/usr/libexec/PlistBuddy -c "Set :objects:E70ADE072A5D0050009947CF:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E70ADE072A5D0050009947CF:buildSettings:CODE_SIGN_IDENTITY 'iPhone Distribution'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E70ADE072A5D0050009947CF:buildSettings:DEVELOPMENT_TEAM 'YS397FG5PA'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E70ADE072A5D0050009947CF:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'apiexample_wildcard_adhoc'" $PBXPROJ_PATH
# Screen Share Extension
# Debug
/usr/libexec/PlistBuddy -c "Set :objects:E72F623E2A7B8AFB00C963D2:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E72F623E2A7B8AFB00C963D2:buildSettings:CODE_SIGN_IDENTITY 'iPhone Distribution'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E72F623E2A7B8AFB00C963D2:buildSettings:DEVELOPMENT_TEAM 'YS397FG5PA'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E72F623E2A7B8AFB00C963D2:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'apiexample_wildcard_adhoc'" $PBXPROJ_PATH
# Release
/usr/libexec/PlistBuddy -c "Set :objects:E72F623F2A7B8AFB00C963D2:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E72F623F2A7B8AFB00C963D2:buildSettings:CODE_SIGN_IDENTITY 'iPhone Distribution'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E72F623F2A7B8AFB00C963D2:buildSettings:DEVELOPMENT_TEAM 'YS397FG5PA'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E72F623F2A7B8AFB00C963D2:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'apiexample_wildcard_adhoc'" $PBXPROJ_PATH
# SimpleFilter
# Debug
/usr/libexec/PlistBuddy -c "Set :objects:E7361F932A6E6E7100925BD6:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E7361F932A6E6E7100925BD6:buildSettings:DEVELOPMENT_TEAM ''" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E7361F932A6E6E7100925BD6:buildSettings:PROVISIONING_PROFILE_SPECIFIER ''" $PBXPROJ_PATH
# Release
/usr/libexec/PlistBuddy -c "Set :objects:E7361F942A6E6E7100925BD6:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E7361F942A6E6E7100925BD6:buildSettings:DEVELOPMENT_TEAM ''" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E7361F942A6E6E7100925BD6:buildSettings:PROVISIONING_PROFILE_SPECIFIER ''" $PBXPROJ_PATH
# Modify build number
# Debug
/usr/libexec/PlistBuddy -c "Set :objects:E70ADE062A5D0050009947CF:buildSettings:CURRENT_PROJECT_VERSION ${BUILD_NUMBER}" $PBXPROJ_PATH
# Release
/usr/libexec/PlistBuddy -c "Set :objects:E70ADE072A5D0050009947CF:buildSettings:CURRENT_PROJECT_VERSION ${BUILD_NUMBER}" $PBXPROJ_PATH

# Read APPID environment variable
echo AGORA_APP_ID: $APP_ID

echo PROJECT_PATH: $PROJECT_PATH
echo TARGET_NAME: $TARGET_NAME
echo KEYCENTER_PATH: $KEYCENTER_PATH
echo APP_PATH: $APP_PATH

# Modify KeyCenter file
sed -i -e "s#<\#YOUR APPID\#>#@\"$APP_ID\";#g" $KEYCENTER_PATH
rm -f ${KEYCENTER_PATH}-e

# Xcode clean
xcodebuild clean -workspace "${APP_PATH}" -configuration "${CONFIGURATION}" -scheme "${TARGET_NAME}"

# Timestamp
CURRENT_TIME=$(date "+%Y-%m-%d %H-%M-%S")

# Archive path
ARCHIVE_PATH="${WORKSPACE}/${TARGET_NAME}_${BUILD_NUMBER}.xcarchive"

# Build environment

# plist path
PLIST_PATH="${PROJECT_PATH}/ExportOptions.plist"

echo PLIST_PATH: $PLIST_PATH

# Archive using workspace, can also use project
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

cd ${WORKSPACE}

echo "Verify Extension signature configuration..."
/usr/libexec/PlistBuddy -c "Print :objects:E72F623E2A7B8AFB00C963D2:buildSettings:CODE_SIGN_IDENTITY" $PBXPROJ_PATH || echo "Debug Extension CODE_SIGN_IDENTITY not found"
/usr/libexec/PlistBuddy -c "Print :objects:E72F623E2A7B8AFB00C963D2:buildSettings:PROVISIONING_PROFILE_SPECIFIER" $PBXPROJ_PATH || echo "Debug Extension PROVISIONING_PROFILE_SPECIFIER not found"
/usr/libexec/PlistBuddy -c "Print :objects:E72F623F2A7B8AFB00C963D2:buildSettings:CODE_SIGN_IDENTITY" $PBXPROJ_PATH || echo "Release Extension CODE_SIGN_IDENTITY not found"
/usr/libexec/PlistBuddy -c "Print :objects:E72F623F2A7B8AFB00C963D2:buildSettings:PROVISIONING_PROFILE_SPECIFIER" $PBXPROJ_PATH || echo "Release Extension PROVISIONING_PROFILE_SPECIFIER not found"
echo ""

echo "=========================================="
echo "List of code signing certificates installed on the current device："
echo "=========================================="
security find-identity -v -p codesigning | grep -E "(iPhone Distribution|Apple Distribution|iOS Distribution)" || security find-identity -v -p codesigning
echo "=========================================="
echo ""

echo "=========================================="
echo "ExportOptions.plist configuration content:"
echo "=========================================="
cat "${PLIST_PATH}"
echo "=========================================="
echo ""

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

SDK_VERSION=$(echo $sdk_url | cut -d "/" -f 5)
OUTPUT_FILE=${WORKSPACE}/${TARGET_NAME}_${BUILD_NUMBER}_${SDK_VERSION}_$(date "+%Y%m%d%H%M%S").ipa
mv ${EXPORT_PATH}/${TARGET_NAME}.ipa $OUTPUT_FILE

rm -rf "${EXPORT_PATH}"
rm -rf "${ARCHIVE_PATH}"
echo OUTPUT_FILE: $OUTPUT_FILE


