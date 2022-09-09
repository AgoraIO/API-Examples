CURRENT_PATH=$PWD

# 获取项目目录
PROJECT_PATH="$( cd "$1" && pwd  )"

cd ${PROJECT_PATH} && pod install

# 项目target名
TARGET_NAME=${PROJECT_PATH##*/}

KEYCENTER_PATH=${PROJECT_PATH}"/"${TARGET_NAME}"/Common/KeyCenter.swift"

# 打包环境
CONFIGURATION=developer-id 

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

# SimpleFilter
# Debug
/usr/libexec/PlistBuddy -c "Set :objects:8BD4AE7E272518D600E95B87:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:8BD4AE7E272518D600E95B87:buildSettings:DEVELOPMENT_TEAM ''" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:8BD4AE7E272518D600E95B87:buildSettings:PROVISIONING_PROFILE_SPECIFIER ''" $PBXPROJ_PATH
# Release
/usr/libexec/PlistBuddy -c "Set :objects:8BD4AE7F272518D600E95B87:buildSettings:CODE_SIGN_STYLE 'Manual'" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:8BD4AE7F272518D600E95B87:buildSettings:DEVELOPMENT_TEAM ''" $PBXPROJ_PATH
/usr/libexec/PlistBuddy -c "Set :objects:8BD4AE7F272518D600E95B87:buildSettings:PROVISIONING_PROFILE_SPECIFIER ''" $PBXPROJ_PATH


# 读取APPID环境变量
echo AGORA_APP_ID:$APP_ID
echo $AGORA_APP_ID

echo PROJECT_PATH: $PROJECT_PATH
echo TARGET_NAME: $TARGET_NAME
echo KEYCENTER_PATH: $KEYCENTER_PATH
echo APP_PATH: $APP_PATH

#修改Keycenter文件
gsed -i "s|static let AppId: String = <#YOUR APPID#>|static let AppId: String = "\"$AGORA_APP_ID"\"|" $KEYCENTER_PATH
gsed -i 's|static let Certificate: String? = <#YOUR Certificate#>|static let Certificate: String? = nil|' $KEYCENTER_PATH

# Xcode clean
xcodebuild clean -workspace "${APP_PATH}" -configuration "${CONFIGURATION}" -scheme "${TARGET_NAME}"

# 时间戳
CURRENT_TIME=$(date "+%Y-%m-%d %H-%M-%S")

# 归档路径
ARCHIVE_PATH="${PROJECT_PATH}/${TARGET_NAME} ${CURRENT_TIME}/${TARGET_NAME}.xcarchive"
# 编译环境

# 导出路径
EXPORT_PATH="${PROJECT_PATH}/${TARGET_NAME} ${CURRENT_TIME}"

# plist路径
PLIST_PATH="${PROJECT_PATH}/ExportOptions.plist"

echo PLIST_PATH: $PLIST_PATH

# archive 这边使用的工作区间 也可以使用project
xcodebuild archive -workspace "${APP_PATH}" -scheme "${TARGET_NAME}" -configuration "${CONFIGURATION}" -archivePath "${ARCHIVE_PATH}" -destination 'generic/platform=iOS'

# 导出ipa
xcodebuild -exportArchive -archivePath "${ARCHIVE_PATH}" -exportPath "${EXPORT_PATH}" -exportOptionsPlist "${PLIST_PATH}"


#复原Keycenter文件
gsed -i "s|static let AppId: String = "\"$AGORA_APP_ID"\"|static let AppId: String = <#YOUR APPID#>|" $KEYCENTER_PATH
gsed -i 's|static let Certificate: String? = nil|static let Certificate: String? = <#YOUR Certificate#>|' $KEYCENTER_PATH




