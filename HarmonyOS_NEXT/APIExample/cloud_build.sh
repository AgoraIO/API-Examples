#!/usr/bin/env bash
set -euo pipefail
umask 077

project_dir="$(cd "$(dirname "$0")" && pwd)"
deveco_home="${DEVECO_HOME:-/Applications/DevEco-Studio.app/Contents}"
ohpm_bin="${OHPM_BIN:-${deveco_home}/tools/ohpm/bin/ohpm}"
hvigor_bin="${HVIGOR_BIN:-${deveco_home}/tools/hvigor/bin/hvigorw}"
java_bin="${JAVA_BIN:-${deveco_home}/jbr/Contents/Home/bin/java}"
node_home="${HMOS_NODE_HOME:-${deveco_home}/tools/node}"
node_bin="${node_home}/bin/node"
deveco_sdk_home="${deveco_home}/sdk"
sign_tool="${HMOS_SIGN_TOOL_JAR:-${deveco_home}/sdk/default/openharmony/toolchains/lib/hap-sign-tool.jar}"

signing_dir="${HMOS_SIGN_CONFIG_DIR:-/Users/admin/.ohos/config/apiexample-config}"
profile_file="${HMOS_PROFILE_FILE:-${signing_dir}/debugDebug.p7b}"
keystore_file="${HMOS_KEYSTORE_FILE:-${signing_dir}/wayang.p12}"
certificate_file="${HMOS_CERTIFICATE_FILE:-${signing_dir}/wayangAgora.cer}"

require_file() {
    if [[ ! -f "$1" ]]; then
        echo "[ERROR] Required file not found: $1"
        exit 1
    fi
}

require_executable() {
    if [[ ! -x "$1" ]]; then
        echo "[ERROR] Required executable not found: $1"
        exit 1
    fi
}

require_directory() {
    if [[ ! -d "$1" ]]; then
        echo "[ERROR] Required directory not found: $1"
        exit 1
    fi
}

if [[ -z "${APP_ID:-}" ]]; then
    echo "[ERROR] APP_ID is required"
    exit 1
fi
if [[ -z "${HMOS_KEY_PWD:-}" ]]; then
    echo "[ERROR] HMOS_KEY_PWD is required"
    exit 1
fi

require_executable "${ohpm_bin}"
require_executable "${hvigor_bin}"
require_executable "${java_bin}"
require_executable "${node_bin}"
require_directory "${deveco_sdk_home}"
require_file "${sign_tool}"
require_file "${profile_file}"
require_file "${keystore_file}"
require_file "${certificate_file}"

export NODE_HOME="${node_home}"
export PATH="${node_home}/bin:${PATH}"
export DEVECO_SDK_HOME="${deveco_sdk_home}"

app_id="${APP_ID}"
app_id="${app_id#\'}"
app_id="${app_id%\'}"
app_id="${app_id#\"}"
app_id="${app_id%\"}"
if [[ ! "${app_id}" =~ ^[[:xdigit:]]{32}$ ]]; then
    echo "[ERROR] APP_ID must be a 32-character hexadecimal value"
    exit 1
fi
escaped_app_id="${app_id//&/\\&}"

key_center="${project_dir}/entry/src/main/ets/common/KeyCenter.ets"
sed -E -i '' \
    "s#^export const AppID: string[[:space:]]*=.*\$#export const AppID: string = '${escaped_app_id}'#" \
    "${key_center}"
sed -E -i '' \
    "s#^export const AppCertificate: string[[:space:]]*=.*\$#export const AppCertificate: string = ''#" \
    "${key_center}"

"${java_bin}" -version
(
    cd "${project_dir}"
    "${ohpm_bin}" install
    cd "${project_dir}/entry"
    "${ohpm_bin}" install
    cd "${project_dir}"
    "${hvigor_bin}" clean --no-daemon
    "${hvigor_bin}" assembleHap --mode module -p product=default -p buildMode=debug --no-daemon
)

unsigned_hap="${project_dir}/entry/build/default/outputs/default/entry-default-unsigned.hap"
require_file "${unsigned_hap}"

build_number="${BUILD_NUMBER:-local}"
sdk_version="${SDK_VERSION:-unknown}"
artifact_timestamp="${ARTIFACT_TIMESTAMP:-$(date '+%Y%m%d%H%M%S')}"
signed_hap="${project_dir}/APIExample_${build_number}_${sdk_version}_${artifact_timestamp}.hap"

"${java_bin}" -jar "${sign_tool}" sign-app \
    -keyAlias "${HMOS_KEY_PWD}" \
    -signAlg "SHA256withECDSA" \
    -mode "localSign" \
    -appCertFile "${certificate_file}" \
    -profileFile "${profile_file}" \
    -keystoreFile "${keystore_file}" \
    -inFile "${unsigned_hap}" \
    -outFile "${signed_hap}" \
    -keyPwd "${HMOS_KEY_PWD}" \
    -keystorePwd "${HMOS_KEY_PWD}" \
    -signCode "1"

if [[ ! -s "${signed_hap}" ]]; then
    echo "[ERROR] Signed HAP was not produced"
    exit 1
fi

echo "[INFO] Signed HAP: ${signed_hap}"
