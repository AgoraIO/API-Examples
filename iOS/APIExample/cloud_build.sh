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

echo "start download bytedance resource : $bytedance_lib"
curl -L -O "$bytedance_lib"
unzip -o vender_bytedance_iOS.zip
rm -f vender_bytedance_iOS.zip

echo "start download sense resource : $sense_lib"
curl -L -O "$sense_lib"
unzip -o vender_sense_iOS.zip
rm -f vender_sense_iOS.zip

echo "start download fu resource : $fu_lib"
curl -L -O "$fu_lib"
unzip -o vender_fu_iOS.zip
rm -f vender_fu_iOS.zip

# Enable third-party player configuration
sed -i -e "s#\#  pod 'ijkplayer'#  pod 'ijkplayer'#g" Podfile

# Enable third-party beauty filters
sed -i -e "s#\#  pod 'SenseLib'#  pod 'SenseLib'#g" Podfile
sed -i -e "s#\#  pod 'bytedEffect'#  pod 'bytedEffect'#g" Podfile
sed -i -e "s#\#  pod 'fuLib'#  pod 'fuLib'#g" Podfile

echo "work space: $WORKSPACE"
echo "project path: $PROJECT_PATH"


pod install --repo-update || exit 1

# Build environment
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
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF72448758C00B599B3:buildSettings:DEVELOPMENT_TEAM 'JDPG69R49Z'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF72448758C00B599B3:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'App'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF72448758C00B599B3:buildSettings:PRODUCT_BUNDLE_IDENTIFIER io.agora.entfull" $PBXPROJ_PATH

# Release
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF82448758C00B599B3:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF82448758C00B599B3:buildSettings:DEVELOPMENT_TEAM 'JDPG69R49Z'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF82448758C00B599B3:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'App'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF82448758C00B599B3:buildSettings:PRODUCT_BUNDLE_IDENTIFIER io.agora.entfull" $PBXPROJ_PATH

# Screen Share Extension
# Debug
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB825205B80007D4FDD:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB825205B80007D4FDD:buildSettings:DEVELOPMENT_TEAM 'JDPG69R49Z'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB825205B80007D4FDD:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'App'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB825205B80007D4FDD:buildSettings:PRODUCT_BUNDLE_IDENTIFIER io.agora.entfull.Agora-ScreenShare-Extension" $PBXPROJ_PATH

# Release
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB925205B80007D4FDD:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB925205B80007D4FDD:buildSettings:DEVELOPMENT_TEAM 'JDPG69R49Z'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB925205B80007D4FDD:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'App'" $PBXPROJ_PATH
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

# Modify Keycenter file
sed -i -e "s#<\#YOUR AppId\#>#\"$APP_ID\"#g" $KEYCENTER_PATH
rm -f ${KEYCENTER_PATH}-e

# Xcode clean
xcodebuild clean -workspace "${APP_PATH}" -configuration "${CONFIGURATION}" -scheme "${TARGET_NAME}"

# Timestamp
CURRENT_TIME=$(date "+%Y-%m-%d %H-%M-%S")

# Archive path
ARCHIVE_PATH="${WORKSPACE}/${TARGET_NAME}_${BUILD_NUMBER}.xcarchive"

# Build environment

# Plist path
PLIST_PATH="${PROJECT_PATH}/ExportOptions.plist"

# Modify ExportOptions.plist
# Modify io.agora.api.examples value
echo "start modify ExportOption.plist"
# Get original values
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

# archive using workspace (can also use project)
xcodebuild CODE_SIGN_STYLE="Manual" archive -workspace "${APP_PATH}" -scheme "${TARGET_NAME}" clean CODE_SIGNING_REQUIRED=NO CODE_SIGNING_ALLOWED=NO -configuration "${CONFIGURATION}" -archivePath "${ARCHIVE_PATH}" -destination 'generic/platform=iOS' -quiet || exit 1

echo "xcode build finished"

cd ${WORKSPACE}

# Compress archive
7za a -tzip "${TARGET_NAME}_${BUILD_NUMBER}.xcarchive.zip" "${ARCHIVE_PATH}"

# Sign
echo "start export, targetName: $TARGET_NAME, bundleNumber: $BUILD_NUMBER, plistPath: $PLIST_PATH"
# sh sign "${TARGET_NAME}_${BUILD_NUMBER}.xcarchive.zip" --type xcarchive --plist "${PLIST_PATH}"
sh export "${TARGET_NAME}_${BUILD_NUMBER}.xcarchive.zip" --plist "${PLIST_PATH}"

echo "export finished"

SDK_VERSION=$(echo $sdk_url | cut -d "/" -f 5)
OUTPUT_FILE=${WORKSPACE}/${TARGET_NAME}_${BUILD_NUMBER}_${SDK_VERSION}_$(date "+%Y%m%d%H%M%S").ipa
mv ${TARGET_NAME}_${BUILD_NUMBER}.ipa $OUTPUT_FILE

rm -rf *.xcarchive
rm -rf *.xcarchive.zip
echo OUTPUT_FILE: $OUTPUT_FILE
