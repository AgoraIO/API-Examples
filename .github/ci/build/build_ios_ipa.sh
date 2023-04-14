CURRENT_PATH=$PWD

# 获取项目目录
PROJECT_PATH="$( cd "$1" && pwd  )"

cd ${PROJECT_PATH} && pod install

if [ $? -eq 0 ]; then
    echo "success"
else
    echo "failed"
    exit 1
fi

# 项目target名
TARGET_NAME=${PROJECT_PATH##*/}

KEYCENTER_PATH=${PROJECT_PATH}"/"${TARGET_NAME}"/Common/KeyCenter.swift"

# 打包环境
CONFIGURATION="Debug"

#工程文件路径
APP_PATH="${PROJECT_PATH}/${TARGET_NAME}.xcworkspace"

#工程配置路径
PBXPROJ_PATH="${PROJECT_PATH}/${TARGET_NAME}.xcodeproj/project.pbxproj"
echo PBXPROJ_PATH: $PBXPROJ_PATH

# 主项目工程配置
# Debug
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF72448758C00B599B3:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF72448758C00B599B3:buildSettings:DEVELOPMENT_TEAM 'GM72UGLGZW'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF72448758C00B599B3:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'App'" $PBXPROJ_PATH
# Release
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF82448758C00B599B3:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF82448758C00B599B3:buildSettings:DEVELOPMENT_TEAM 'GM72UGLGZW'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF82448758C00B599B3:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'App'" $PBXPROJ_PATH

# 屏幕共享Extension
# Debug
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB825205B80007D4FDD:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB825205B80007D4FDD:buildSettings:DEVELOPMENT_TEAM 'GM72UGLGZW'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB825205B80007D4FDD:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'App'" $PBXPROJ_PATH
# Release
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB925205B80007D4FDD:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB925205B80007D4FDD:buildSettings:DEVELOPMENT_TEAM 'GM72UGLGZW'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:0339BEB925205B80007D4FDD:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'App'" $PBXPROJ_PATH

# SimpleFilter
# Debug
/usr/libexec/PlistBuddy -c "Set :objects:8B10BE1726AFFFA6002E1373:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:8B10BE1726AFFFA6002E1373:buildSettings:DEVELOPMENT_TEAM ''" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:8B10BE1726AFFFA6002E1373:buildSettings:PROVISIONING_PROFILE_SPECIFIER ''" $PBXPROJ_PATH
# Release
/usr/libexec/PlistBuddy -c "Set :objects:8B10BE1826AFFFA6002E1373:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:8B10BE1826AFFFA6002E1373:buildSettings:DEVELOPMENT_TEAM ''" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:8B10BE1826AFFFA6002E1373:buildSettings:PROVISIONING_PROFILE_SPECIFIER ''" $PBXPROJ_PATH

#修改build number
# Debug
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF72448758C00B599B3:buildSettings:CURRENT_PROJECT_VERSION ${BUILD_NUMBER}" $PBXPROJ_PATH
# Release
/usr/libexec/PlistBuddy -c "Set :objects:03D13BF82448758C00B599B3:buildSettings:CURRENT_PROJECT_VERSION ${BUILD_NUMBER}" $PBXPROJ_PATH


# 读取APPID环境变量
echo AGORA_APP_ID:$APP_ID
echo $AGORA_APP_ID

echo PROJECT_PATH: $PROJECT_PATH
echo TARGET_NAME: $TARGET_NAME
echo KEYCENTER_PATH: $KEYCENTER_PATH
echo APP_PATH: $APP_PATH

#修改Keycenter文件
python3 /tmp/jenkins/api-examples/.github/ci/build/modify_ios_keycenter.py $KEYCENTER_PATH 0

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
xcodebuild CODE_SIGN_STYLE="Manual" archive -workspace "${APP_PATH}" -scheme "${TARGET_NAME}" clean CODE_SIGNING_REQUIRED=NO CODE_SIGNING_ALLOWED=NO -configuration "${CONFIGURATION}" -archivePath "${ARCHIVE_PATH}" -destination 'generic/platform=iOS' -quiet || exit

# 压缩archive
7za a -tzip "${TARGET_NAME}_${BUILD_NUMBER}.xcarchive.zip" "${ARCHIVE_PATH}"

# 签名
pushd ${WORKSPACE}
# sh sign "${PROJECT_PATH}/${TARGET_NAME}_${BUILD_NUMBER}.xcarchive.zip" --type xcarchive --plist "${PLIST_PATH}"
sh export "${PROJECT_PATH}/${TARGET_NAME}_${BUILD_NUMBER}.xcarchive.zip" --plist "${PLIST_PATH}"
popd

cd ${WORKSPACE}

# 上传IPA
PAYLOAD_PATH="${TARGET_NAME}_${BUILD_NUMBER}_Payload"
mkdir "${PAYLOAD_PATH}"
# mv "${TARGET_NAME}_${BUILD_NUMBER}_iOS.ipa" "${PAYLOAD_PATH}"
mv "${TARGET_NAME}_${BUILD_NUMBER}.ipa" "${PAYLOAD_PATH}"

7za a "${TARGET_NAME}_${BUILD_NUMBER}_IPA.zip" -r "${PAYLOAD_PATH}"

# 删除IPA文件夹
rm -rf ${TARGET_NAME}_${BUILD_NUMBER}.xcarchive
rm -rf *.zip
rm -rf ${PAYLOAD_PATH}

#复原Keycenter文件
python3 /tmp/jenkins/api-examples/.github/ci/build/modify_ios_keycenter.py $KEYCENTER_PATH 1




