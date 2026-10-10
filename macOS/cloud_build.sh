#!/usr/bin/env sh
set -eu

export LANG=en_US.UTF-8
export PATH=$PATH:/opt/homebrew/bin

PROJECT_PATH=$PWD
WORKSPACE=${WORKSPACE:-$PWD}
SIGNING_TEAM="YS397FG5PA"

: "${BUILD_NUMBER:?BUILD_NUMBER is required}"
: "${APP_ID:?APP_ID is required}"
: "${JFROG_API_KEY:?JFROG_API_KEY is required}"

cd "${PROJECT_PATH}"

curl --fail --location -H "X-JFrog-Art-Api:${JFROG_API_KEY}" -o AgoraBeautyMaterial.bundle.zip "https://artifactory-api.bj2.agoralab.co/artifactory/qa_test_data/beauty/AgoraBeautyMaterial.bundle.zip"
rm -rf APIExample/Resources/AgoraBeautyMaterial.bundle
unzip -q AgoraBeautyMaterial.bundle.zip -d APIExample/Resources
rm -f AgoraBeautyMaterial.bundle.zip
test -f APIExample/Resources/AgoraBeautyMaterial.bundle/beauty_material_functional/config.json

pod install

# Build environment
CONFIGURATION="Debug"

# Project file path
APP_PATH=$(find . -maxdepth 1 -type d -name '*.xcworkspace' -print -quit)
if [ -z "${APP_PATH}" ]; then
	echo "Error: No Xcode workspace found in ${PROJECT_PATH}"
	exit 1
fi
APP_PATH=${APP_PATH#./}

# Project target name
TARGET_NAME=${APP_PATH%%.*}

KEYCENTER_PATH=$TARGET_NAME/Common/KeyCenter.swift

# Read APPID environment variable
echo "AGORA_APP_ID is configured"

echo PROJECT_PATH: "$PROJECT_PATH"
echo TARGET_NAME: "$TARGET_NAME"
echo KEYCENTER_PATH: "$KEYCENTER_PATH"
echo APP_PATH: "$APP_PATH"

# Modify Keycenter file
sed -i -e "s#<\#YOUR AppId\#>#\"$APP_ID\"#g" "${KEYCENTER_PATH}"
rm -f "${KEYCENTER_PATH}-e"

# Archive path
ARCHIVE_PATH="${WORKSPACE}/${TARGET_NAME}_${BUILD_NUMBER}.xcarchive"

# Build environment

# Plist path
PLIST_PATH="${PROJECT_PATH}/ExportOptions.plist"
EXPORT_PATH="${WORKSPACE}/${TARGET_NAME}_${BUILD_NUMBER}_export"

echo PLIST_PATH: "$PLIST_PATH"

# Archive with Xcode-managed development signing for the configured team.
rm -rf "${ARCHIVE_PATH}" "${EXPORT_PATH}"
xcodebuild \
	-workspace "${APP_PATH}" \
	-scheme "${TARGET_NAME}" \
	-configuration "${CONFIGURATION}" \
	-destination 'generic/platform=macOS' \
	-archivePath "${ARCHIVE_PATH}" \
	-allowProvisioningUpdates \
	CODE_SIGN_STYLE=Automatic \
	CODE_SIGN_IDENTITY="Apple Development" \
	DEVELOPMENT_TEAM="${SIGNING_TEAM}" \
	PROVISIONING_PROFILE_SPECIFIER= \
	CURRENT_PROJECT_VERSION="${BUILD_NUMBER}" \
	clean archive

if [ ! -d "${ARCHIVE_PATH}/Products/Applications/${TARGET_NAME}.app" ]; then
	echo "Error: Archive does not contain ${TARGET_NAME}.app"
	exit 1
fi

mkdir -p "${EXPORT_PATH}"
xcodebuild -exportArchive \
	-archivePath "${ARCHIVE_PATH}" \
	-exportPath "${EXPORT_PATH}" \
	-exportOptionsPlist "${PLIST_PATH}" \
	-allowProvisioningUpdates

EXPORTED_APP="${EXPORT_PATH}/${TARGET_NAME}.app"
if [ ! -d "${EXPORTED_APP}" ]; then
	echo "Error: Xcode export did not produce ${TARGET_NAME}.app"
	exit 1
fi
codesign --verify --deep --strict --verbose=2 "${EXPORTED_APP}"

SDK_VERSION=$(echo "${sdk_url:-unknown}" | cut -d "/" -f 5)
OUTPUT_FILE=${WORKSPACE}/${TARGET_NAME}_${BUILD_NUMBER}_${SDK_VERSION}_$(date "+%Y%m%d%H%M%S").app.zip
rm -f "${OUTPUT_FILE}"
ditto -c -k --sequesterRsrc --keepParent "${EXPORTED_APP}" "${OUTPUT_FILE}"
if [ ! -s "${OUTPUT_FILE}" ]; then
	echo "Error: App package was not created"
	exit 1
fi

rm -rf "${ARCHIVE_PATH}" "${EXPORT_PATH}"
echo OUTPUT_FILE: "$OUTPUT_FILE"
