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

# Build environment
CONFIGURATION="Debug"

# Project workspace path
APP_PATH="$(ls | grep xcworkspace)"

# Project target name
TARGET_NAME=${APP_PATH%%.*} 

KEYCENTER_PATH=$TARGET_NAME/Common/KeyCenter.m

# Project configuration path
PBXPROJ_PATH=${TARGET_NAME}.xcodeproj/project.pbxproj

# Debug
/usr/libexec/PlistBuddy -c "Set :objects:E70ADE062A5D0050009947CF:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E70ADE062A5D0050009947CF:buildSettings:DEVELOPMENT_TEAM 'GM72UGLGZW'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E70ADE062A5D0050009947CF:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'App'" $PBXPROJ_PATH
# Release
/usr/libexec/PlistBuddy -c "Set :objects:E70ADE072A5D0050009947CF:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E70ADE072A5D0050009947CF:buildSettings:DEVELOPMENT_TEAM 'GM72UGLGZW'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E70ADE072A5D0050009947CF:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'App'" $PBXPROJ_PATH
# Screen Share Extension
# Debug
/usr/libexec/PlistBuddy -c "Set :objects:E72F61D42A7256D500C963D2:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E72F61D42A7256D500C963D2:buildSettings:DEVELOPMENT_TEAM 'GM72UGLGZW'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E72F61D42A7256D500C963D2:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'App'" $PBXPROJ_PATH
# Release
/usr/libexec/PlistBuddy -c "Set :objects:E72F61D52A7256D500C963D2:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E72F61D52A7256D500C963D2:buildSettings:DEVELOPMENT_TEAM 'GM72UGLGZW'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:E72F61D52A7256D500C963D2:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'App'" $PBXPROJ_PATH
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

# Modify Keycenter file
sed -i -e "s#<\#YOUR APPID\#>#@\"$APP_ID\";#g" $KEYCENTER_PATH
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

echo PLIST_PATH: $PLIST_PATH

# Archive using workspace (can also use project)
xcodebuild CODE_SIGN_STYLE="Manual" archive -workspace "${APP_PATH}" -scheme "${TARGET_NAME}" clean CODE_SIGNING_REQUIRED=NO CODE_SIGNING_ALLOWED=NO -configuration "${CONFIGURATION}" -archivePath "${ARCHIVE_PATH}" -destination 'generic/platform=iOS' -quiet || exit 1

cd ${WORKSPACE}

# Compress archive
7za a -tzip "${TARGET_NAME}_${BUILD_NUMBER}.xcarchive.zip" "${ARCHIVE_PATH}"

# Sign
# sh sign "${TARGET_NAME}_${BUILD_NUMBER}.xcarchive.zip" --type xcarchive --plist "${PLIST_PATH}"
sh export "${TARGET_NAME}_${BUILD_NUMBER}.xcarchive.zip" --plist "${PLIST_PATH}"

SDK_VERSION=$(echo $sdk_url | cut -d "/" -f 5)
OUTPUT_FILE=${WORKSPACE}/${TARGET_NAME}_${BUILD_NUMBER}_${SDK_VERSION}_$(date "+%Y%m%d%H%M%S").ipa
mv ${TARGET_NAME}_${BUILD_NUMBER}.ipa $OUTPUT_FILE

rm -rf *.xcarchive
rm -rf *.xcarchive.zip
echo OUTPUT_FILE: $OUTPUT_FILE
