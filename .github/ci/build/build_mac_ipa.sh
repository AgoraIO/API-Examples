CURRENT_PATH=$PWD

# 获取项目目录
PROJECT_PATH="$( cd "$1" && pwd  )"

cd ${PROJECT_PATH} && pod install
if [ $? -eq 0 ]; then
    echo " pod install success"
else
    echo " pod install failed"
    exit 1
fi

# 项目target名
TARGET_NAME=${PROJECT_PATH##*/}

KEYCENTER_PATH=${PROJECT_PATH}"/"${TARGET_NAME}"/Common/KeyCenter.swift"

# 打包环境
CONFIGURATION=Release

#工程文件路径
APP_PATH="${PROJECT_PATH}/${TARGET_NAME}.xcworkspace"

#工程配置路径
PBXPROJ_PATH="${PROJECT_PATH}/${TARGET_NAME}.xcodeproj/project.pbxproj"
echo PBXPROJ_PATH: $PBXPROJ_PATH

# 主项目工程配置
# Debug
/usr/libexec/PlistBuddy -c "Set :objects:03896D5324F8A011008593CD:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03896D5324F8A011008593CD:buildSettings:CODE_SIGN_IDENTITY 'Developer ID Application'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03896D5324F8A011008593CD:buildSettings:DEVELOPMENT_TEAM 'YS397FG5PA'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03896D5324F8A011008593CD:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'apiexamplemac'" $PBXPROJ_PATH
# Release
/usr/libexec/PlistBuddy -c "Set :objects:03896D5424F8A011008593CD:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03896D5424F8A011008593CD:buildSettings:CODE_SIGN_IDENTITY 'Developer ID Application'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03896D5424F8A011008593CD:buildSettings:DEVELOPMENT_TEAM 'YS397FG5PA'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:03896D5424F8A011008593CD:buildSettings:PROVISIONING_PROFILE_SPECIFIER 'apiexamplemac'" $PBXPROJ_PATH

#修改build number
# Debug
/usr/libexec/PlistBuddy -c "Set :objects:03896D5324F8A011008593CD:buildSettings:CURRENT_PROJECT_VERSION ${BUILD_NUMBER}" $PBXPROJ_PATH
# Release
/usr/libexec/PlistBuddy -c "Set :objects:03896D5424F8A011008593CD:buildSettings:CURRENT_PROJECT_VERSION ${BUILD_NUMBER}" $PBXPROJ_PATH

# 读取APPID环境变量
echo AGORA_APP_ID:$APP_ID
echo $AGORA_APP_ID

echo PROJECT_PATH: $PROJECT_PATH
echo TARGET_NAME: $TARGET_NAME
echo KEYCENTER_PATH: $KEYCENTER_PATH
echo APP_PATH: $APP_PATH

#修改Keycenter文件

python3 /tmp/jenkins/api-examples/.github/ci/build/modify_ios_keycenter.py $KEYCENTER_PATH 0
if [ $? -eq 0 ]; then
    echo "修改Keycenter文件 success"
else
    echo "修改Keycenter文件 failed"
    exit 1
fi
# Xcode clean
xcodebuild clean -workspace "${APP_PATH}" -configuration "${CONFIGURATION}" -scheme "${TARGET_NAME}"

# 时间戳
CURRENT_TIME=$(date "+%Y-%m-%d %H-%M-%S")

SDK_VERSION=$(echo $sdk_url | cut -d "/" -f 5)

# 归档路径
ARCHIVE_PATH="${WORKSPACE}/${TARGET_NAME}_${BUILD_NUMBER}.xcarchive"
# 编译环境

# plist路径
PLIST_PATH="${PROJECT_PATH}/ExportOptions.plist"

echo PLIST_PATH: $PLIST_PATH

# archive 这边使用的工作区间 也可以使用project
xcodebuild archive -workspace "${APP_PATH}" -scheme "${TARGET_NAME}" -configuration "${CONFIGURATION}" -archivePath "${ARCHIVE_PATH}"

cd ${WORKSPACE}

# 压缩archive
7za a -slp "${TARGET_NAME}_${BUILD_NUMBER}.xcarchive.zip" "${ARCHIVE_PATH}"

# 签名
sh sign "${WORKSPACE}/${TARGET_NAME}_${BUILD_NUMBER}.xcarchive.zip" --type xcarchive --plist "${PLIST_PATH}" --application macApp

# 重命名
cp "${TARGET_NAME}_${BUILD_NUMBER}.app.zip" "${TARGET_NAME}_SDK_${SDK_VERSION}_CI_${BUILD_NUMBER}.app.zip"

# 上传IPA
python3 artifactory_utils.py --action=upload_file --file="${TARGET_NAME}_SDK_${SDK_VERSION}_CI_${BUILD_NUMBER}.app.zip" --project

# 删除archive文件
rm -rf ${TARGET_NAME}_${BUILD_NUMBER}.xcarchive
rm -rf *.zip

#复原Keycenter文件
python3 /tmp/jenkins/api-examples/.github/ci/build/modify_ios_keycenter.py $KEYCENTER_PATH 1




