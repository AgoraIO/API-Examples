#!/usr/bin/env bash
set -euo pipefail

ci_dir="$(cd "$(dirname "$0")" && pwd)"
repo_root="$(cd "${ci_dir}/../../.." && pwd)"
project_dir="${repo_root}/HarmonyOS_NEXT/APIExample"
libs_dir="${project_dir}/entry/libs"
output_dir="${WORKSPACE:-${repo_root}}"

if [[ -z "${sdk_url:-}" ]]; then
    echo "[ERROR] sdk_url is required"
    exit 1
fi

sdk_file_name="$(basename "${sdk_url%%\?*}")"
if [[ "${sdk_file_name}" != *.zip ]]; then
    echo "[ERROR] sdk_url must point to a FULL SDK .zip file: ${sdk_file_name}"
    exit 1
fi

stage_dir="$(mktemp -d "${TMPDIR:-/tmp}/apiexample-hmos.XXXXXX")"
trap 'rm -rf "${stage_dir}"' EXIT

sdk_archive="${stage_dir}/${sdk_file_name}"
sdk_extract_dir="${stage_dir}/sdk"
mkdir -p "${sdk_extract_dir}"
curl -fL --retry 3 --output "${sdk_archive}" "${sdk_url}"
unzip -q "${sdk_archive}" -d "${sdk_extract_dir}"

sdk_hars=()
while IFS= read -r sdk_har; do
    sdk_hars+=("${sdk_har}")
done < <(find "${sdk_extract_dir}" -type f -path '*/rtc/sdk/AgoraRtcSdk.har')

if [[ "${#sdk_hars[@]}" -ne 1 ]]; then
    echo "[ERROR] Expected exactly one rtc/sdk/AgoraRtcSdk.har in ${sdk_file_name}, found ${#sdk_hars[@]}"
    exit 1
fi

sdk_har="${sdk_hars[0]}"
sdk_root="${sdk_har%/rtc/sdk/AgoraRtcSdk.har}"
sdk_name="$(basename "${sdk_root}")"

mkdir -p "${libs_dir}"
cp -f "${sdk_har}" "${libs_dir}/AgoraRtcSdk.har"

package_metadata="${stage_dir}/oh-package.json5"
tar -xOzf "${sdk_har}" package/oh-package.json5 > "${package_metadata}"
detected_version="$(python3 -c 'import json, sys; print(json.load(open(sys.argv[1]))["version"])' "${package_metadata}")"
export SDK_VERSION="${SDK_VERSION:-${detected_version}}"

sample_dir="${sdk_root}/rtc/samples/API-Example"
rm -rf "${sample_dir}"
mkdir -p "${sample_dir}"
rsync -a \
    --exclude '.hvigor/' \
    --exclude 'build/' \
    --exclude 'oh_modules/' \
    "${project_dir}/" "${sample_dir}/"

build_number="${BUILD_NUMBER:-local}"
timestamp="$(date '+%Y%m%d%H%M%S')"
export ARTIFACT_TIMESTAMP="${timestamp}"
zip_file="${repo_root}/APIExample_${build_number}_${SDK_VERSION}_${timestamp}.zip"
(
    cd "$(dirname "${sdk_root}")"
    zip -qry "${zip_file}" "${sdk_name}"
)

if [[ "${compile_project:-true}" == "true" ]]; then
    "${project_dir}/cloud_build.sh"

    hap_file="${project_dir}/APIExample_${build_number}_${SDK_VERSION}_${timestamp}.hap"
    if [[ ! -s "${hap_file}" ]]; then
        echo "[ERROR] No signed HAP was produced"
        exit 1
    fi
    cp -f "${hap_file}" "${output_dir}/"
fi

if [[ "${output_dir}" != "${repo_root}" ]]; then
    cp -f "${zip_file}" "${output_dir}/"
fi

echo "[INFO] HarmonyOS artifacts are ready in ${output_dir}"
