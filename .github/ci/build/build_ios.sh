#!/bin/bash -ilex
##################################
# --- Guidelines: ---
#
# Common Environment Variable:
# 'Package_Publish:boolean:true',
# 'Clean_Clone:boolean:false',
# 'is_tag_fetch:boolean:false',
# 'is_offical_build:boolean:false',
# 'repo:string',
# 'base:string',
# 'arch:string'
# 'output:string'
# 'short_version:string'
# 'release_version:string'
# 'build_date:string(yyyyMMdd)',
# 'build_timestamp:string (yyyyMMdd_hhmm)',
# 'platform: string',
# 'BUILD_NUMBER: string',
# 'WORKSPACE: string'
#
# --- Test Related: ---
#   PR build, zip test related to test.zip
#   Package build, zip package related to package.zip
# --- Artifactory Related: ---
# download artifactory:
# python3 ${WORKSPACE}/artifactory_utils.py --action=download_file --file=ARTIFACTORY_URL
# upload file to artifactory:
# python3 ${WORKSPACE}/artifactory_utils.py --action=upload_file --file=FILEPATTERN --server_path=SERVERPATH --server_repo=SERVER_REPO --with_pattern
# for example: python3 ${WORKSPACE}/artifactory_utils.py --action=upload_file --file=*.zip --server_path=windows/ --server_repo=ACCS_repo --with_pattern
# upload folder to artifactory
# python3 ${WORKSPACE}/artifactory_utils.py --action=upload_file --file=FILEPATTERN --server_path=SERVERPATH --server_repo=SERVER_REPO --with_folder
# for example: python3 ${WORKSPACE}/artifactory_utils.py --action=upload_file --file=*.zip --server_path=windows/ --server_repo=ACCS_repo --with_folder
# --- Input: ----
# sourcePath: see jenkins console for details.
# WORKSPACE: ${WORKSPACE}
# --- Output: ----
# pr: output test.zip to workspace dir
# others: Rename the zip package name yourself, But need copy it to workspace dir
##################################
xcode_version=$(xcodebuild -version | grep Xcode | awk '{print $2}')
echo "Xcode Version: $xcode_version"
echo ios_direction: $ios_direction
echo Package_Publish: $Package_Publish
echo is_tag_fetch: $is_tag_fetch
echo arch: $arch
echo source_root: %source_root%
echo output: /tmp/jenkins/${project}_out
echo build_date: $build_date
echo build_time: $build_time
echo release_version: $release_version
echo short_version: $short_version
echo pwd: `pwd`
echo sdk_url: $sdk_url

export https_proxy=10.10.114.55:1080
export http_proxy=10.10.114.55:1080
export all_proxy=10.10.114.55:1080
export LANG=en_US.UTF-8

unzip_name=Agora_Native_SDK_for_iOS_FULL
zip_name=output.zip
sdk_url_flag=false
if [ -z "$sdk_url" ]; then
   sdk_url_flag=false
   echo "sdk_url is empty"
   echo unzip_name: $unzip_name 
   mkdir -p ./$unzip_name/samples
   cp -rf ./iOS/${ios_direction} ./$unzip_name/samples/${ios_direction} || exit 1
   ls -al ./$unzip_name/samples/${ios_direction}/
else
   sdk_url_flag=true
   zip_name=${sdk_url##*/}
   echo zip_name: $zip_name
   curl -o $zip_name $sdk_url || exit 1
   7za x ./$zip_name -y > log.txt
   unzip_name=`ls -S -d */ | grep Agora | sed 's/\///g'`
   echo unzip_name: $unzip_name
   rm -rf ./$unzip_name/bin
   rm -f ./$unzip_name/commits
   rm -f ./$unzip_name/package_size_report.txt

   rm -f ./$unzip_name/.commits
   rm -f ./$unzip_name/AgoraInfra_iOS.swift
   rm -f ./$unzip_name/AgoraRtcEngine_iOS.podspec
   rm -f ./$unzip_name/AgoraAudio_iOS.podspec
   rm -f ./$unzip_name/Package.swift
   mkdir -p ./$unzip_name/samples
   cp -rf ./iOS/${ios_direction} ./$unzip_name/samples/${ios_direction} || exit 1
   ls -al ./$unzip_name/samples/${ios_direction}/
   mv ./$unzip_name/samples/${ios_direction}/sdk.podspec ./$unzip_name/ || exit 1
fi

python3 ./.github/ci/build/modify_podfile.py ./$unzip_name/samples/${ios_direction}/Podfile $sdk_url_flag || exit 1

echo "start compress"
7za a -tzip result.zip -r $unzip_name > log.txt
echo "start move to"
echo $WORKSPACE/with${ios_direction}_${BUILD_NUMBER}_$zip_name
mv result.zip $WORKSPACE/with${ios_direction}_${BUILD_NUMBER}_$zip_name

if [ $compile_project = true ]; then
	cd ./$unzip_name/samples/${ios_direction}
	./cloud_build.sh || exit 1
	cd -
fi