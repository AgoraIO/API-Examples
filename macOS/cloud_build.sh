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

# Project file path
APP_PATH="$(ls | grep xcworkspace)"

# Project target name
TARGET_NAME=${APP_PATH%%.*} 

KEYCENTER_PATH=$TARGET_NAME/Common/KeyCenter.swift

# Project configuration path
PBXPROJ_PATH=${TARGET_NAME}.xcodeproj/project.pbxproj

# Main project configuration
# Debug
/usr/libexec/PlistBuddy -c "Set :objects:03896D5324F8A011008593CD:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03896D5324F8A011008593CD:buildSettings:CODE_SIGN_IDENTITY 'Developer ID Application'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03896D5324F8A011008593CD:buildSettings:DEVELOPMENT_TEAM 'GM72UGLGZW'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03896D5324F8A011008593CD:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'App'" $PBXPROJ_PATH
# Release
/usr/libexec/PlistBuddy -c "Set :objects:03896D5424F8A011008593CD:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03896D5424F8A011008593CD:buildSettings:CODE_SIGN_IDENTITY 'Developer ID Application'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03896D5424F8A011008593CD:buildSettings:DEVELOPMENT_TEAM 'GM72UGLGZW'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03896D5424F8A011008593CD:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'App'" $PBXPROJ_PATH

# SimpleFilter
# Debug
/usr/libexec/PlistBuddy -c "Set :objects:8BD4AE7E272518D600E95B87:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:8BD4AE7E272518D600E95B87:buildSettings:DEVELOPMENT_TEAM ''" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:8BD4AE7E272518D600E95B87:buildSettings:PROVISIONING_PROFILE_SPECIFIER ''" $PBXPROJ_PATH
# Release
/usr/libexec/PlistBuddy -c "Set :objects:8BD4AE7F272518D600E95B87:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:8BD4AE7F272518D600E95B87:buildSettings:DEVELOPMENT_TEAM ''" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:8BD4AE7F272518D600E95B87:buildSettings:PROVISIONING_PROFILE_SPECIFIER ''" $PBXPROJ_PATH

# Modify build number
# Debug
/usr/libexec/PlistBuddy -c "Set :objects:03896D5324F8A011008593CD:buildSettings:CURRENT_PROJECT_VERSION ${BUILD_NUMBER}" $PBXPROJ_PATH
# Release
/usr/libexec/PlistBuddy -c "Set :objects:03896D5424F8A011008593CD:buildSettings:CURRENT_PROJECT_VERSION ${BUILD_NUMBER}" $PBXPROJ_PATH


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

echo PLIST_PATH: $PLIST_PATH

# Archive using workspace (can also use project)
xcodebuild archive -workspace "${APP_PATH}" -scheme "${TARGET_NAME}" -configuration "${CONFIGURATION}" -archivePath "${ARCHIVE_PATH}"

cd ${WORKSPACE}

# Compress archive
7za a -tzip "${TARGET_NAME}_${BUILD_NUMBER}.xcarchive.zip" "${ARCHIVE_PATH}"

echo "start sign..."

# Sign
sh sign "${WORKSPACE}/${TARGET_NAME}_${BUILD_NUMBER}.xcarchive.zip" --type xcarchive --plist "${PLIST_PATH}" --application macApp


SDK_VERSION=$(echo $sdk_url | cut -d "/" -f 5)
OUTPUT_FILE=${WORKSPACE}/${TARGET_NAME}_${BUILD_NUMBER}_${SDK_VERSION}_$(date "+%Y%m%d%H%M%S").app.zip
mv ${TARGET_NAME}_${BUILD_NUMBER}.app.zip $OUTPUT_FILE

rm -rf *.xcarchive
rm -rf *.xcarchive.zip
echo OUTPUT_FILE: $OUTPUT_FILE
