#!/usr/bin/env bash
set -euo pipefail

WORKING_PATH="$1"
APP_PROJECT="$2"
APP_TARGET="$3"
MODE=Release

echo "WORKING_PATH: ${WORKING_PATH}"
echo "APP_TARGET: ${APP_TARGET}"

cd "${WORKING_PATH}"
pwd

if [[ -f Podfile ]]; then
    pod install
    project_args=(-workspace "${APP_PROJECT}.xcworkspace")
else
    project_args=(-project "${APP_PROJECT}.xcodeproj")
fi

xcodebuild \
    "${project_args[@]}" \
    -scheme "${APP_TARGET}" \
    -configuration "${MODE}" \
    -destination 'generic/platform=iOS' \
    CODE_SIGNING_ALLOWED=NO \
    CODE_SIGNING_REQUIRED=NO \
    CODE_SIGN_IDENTITY= \
    DEVELOPMENT_TEAM= \
    PROVISIONING_PROFILE_SPECIFIER= \
    clean build
