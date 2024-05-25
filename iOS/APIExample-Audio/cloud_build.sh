CURRENT_PATH=$PWD

PROJECT_PATH=$PWD

cd ${PROJECT_PATH} && pod install || exit 1

# 打包环境
CONFIGURATION="Debug"

#工程文件路径
APP_PATH="$(ls | grep xcworkspace)"

# 项目target名
TARGET_NAME=${APP_PATH%%.*} 

KEYCENTER_PATH=$TARGET_NAME/Common/KeyCenter.swift

#工程配置路径
PBXPROJ_PATH=${TARGET_NAME}.xcodeproj/project.pbxproj

# Debug
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF72448758C00B599B3:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF72448758C00B599B3:buildSettings:DEVELOPMENT_TEAM 'GM72UGLGZW'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF72448758C00B599B3:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'App'" $PBXPROJ_PATH
# Release
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF82448758C00B599B3:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF82448758C00B599B3:buildSettings:DEVELOPMENT_TEAM 'GM72UGLGZW'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF82448758C00B599B3:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'App'" $PBXPROJ_PATH

#修改build number
# Debug
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF72448758C00B599B3:buildSettings:CURRENT_PROJECT_VERSION ${BUILD_NUMBER}" $PBXPROJ_PATH
# Release
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF82448758C00B599B3:buildSettings:CURRENT_PROJECT_VERSION ${BUILD_NUMBER}" $PBXPROJ_PATH

# 读取APPID环境变量
echo AGORA_APP_ID: $APP_ID

echo PROJECT_PATH: $PROJECT_PATH
echo TARGET_NAME: $TARGET_NAME
echo KEYCENTER_PATH: $KEYCENTER_PATH
echo APP_PATH: $APP_PATH

#修改Keycenter文件
sed -i -e "s#<\#YOUR AppId\#>#\"$APP_ID\"#g" $KEYCENTER_PATH
sed -i -e "s#<\#YOUR Certificate\#>#nil#g" $KEYCENTER_PATH
rm -f ${KEYCENTER_PATH}-e

# Xcode clean
xcodebuild clean -workspace "${APP_PATH}" -configuration "${CONFIGURATION}" -scheme "${TARGET_NAME}"

# 时间戳
CURRENT_TIME=$(date "+%Y-%m-%d %H-%M-%S")

# 归档路径
ARCHIVE_PATH="${WORKSPACE}/${TARGET_NAME}_${BUILD_NUMBER}.xcarchive"

# 编译环境

# plist路径
PLIST_PATH="${PROJECT_PATH}/ExportOptions.plist"

echo PLIST_PATH: $PLIST_PATH

# archive 这边使用的工作区间 也可以使用project
xcodebuild CODE_SIGN_STYLE="Manual" archive -workspace "${APP_PATH}" -scheme "${TARGET_NAME}" clean CODE_SIGNING_REQUIRED=NO CODE_SIGNING_ALLOWED=NO -configuration "${CONFIGURATION}" -archivePath "${ARCHIVE_PATH}" -destination 'generic/platform=iOS' -quiet || exit 1


cd ${WORKSPACE}

# 压缩archive
7za a -tzip "${TARGET_NAME}_${BUILD_NUMBER}.xcarchive.zip" "${ARCHIVE_PATH}"


# 签名
# sh sign "${TARGET_NAME}_${BUILD_NUMBER}.xcarchive.zip" --type xcarchive --plist "${PLIST_PATH}"
sh export "${TARGET_NAME}_${BUILD_NUMBER}.xcarchive.zip" --plist "${PLIST_PATH}"

SDK_VERSION=$(echo $sdk_url | cut -d "/" -f 5)
OUTPUT_FILE=${WORKSPACE}/${TARGET_NAME}_${BUILD_NUMBER}_$SDK_VERSION_$(date "+%Y%m%d%H%M%S").ipa
mv ${TARGET_NAME}_${BUILD_NUMBER}.ipa $OUTPUT_FILE

rm -rf *.xcarchive
echo OUTPUT_FILE: $OUTPUT_FILE


