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
if [[ "${sdk_file_name}" != *.har ]]; then
    echo "[ERROR] sdk_url must point to a .har file: ${sdk_file_name}"
    exit 1
fi

mkdir -p "${libs_dir}"
sdk_temp_file="${libs_dir}/AgoraRtcSdk.har.download"
curl -fL --retry 3 --output "${sdk_temp_file}" "${sdk_url}"
mv "${sdk_temp_file}" "${libs_dir}/AgoraRtcSdk.har"

detected_version="$(printf '%s' "${sdk_file_name}" | grep -Eo '[0-9]+\.[0-9]+\.[0-9]+' | head -n 1 || true)"
export SDK_VERSION="${SDK_VERSION:-${detected_version:-unknown}}"

stage_dir="$(mktemp -d "${TMPDIR:-/tmp}/apiexample-hmos.XXXXXX")"
trap 'rm -rf "${stage_dir}"' EXIT

sdk_name="${sdk_file_name%.har}"
sample_dir="${stage_dir}/${sdk_name}/rtc/samples/API-Example"
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
    cd "${stage_dir}"
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
