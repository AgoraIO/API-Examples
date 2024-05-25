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

zip_name=${sdk_url##*/}
echo zip_name: $zip_name

python3 $WORKSPACE/artifactory_utils.py --action=download_file --file=$sdk_url
7za x ./$zip_name -y

unzip_name=`ls -S -d */ | grep Agora | sed 's/\///g'`
echo unzip_name: $unzip_name

rm -rf ./$unzip_name/bin
rm ./$unzip_name/commits
rm ./$unzip_name/package_size_report.txt
rm -rf ./$unzip_name/samples
mkdir -p ./$unzip_name/samples/API-Example

git clean -xfd
git checkout .
ls -al ./$unzip_name/samples/API-Example/
ls -al ./iOS/$ios_direction/

cp -rf ./iOS/$ios_direction/** ./$unzip_name/samples/API-Example/ || exit 1
cp -rf ./iOS/$ios_direction/.** ./$unzip_name/samples/API-Example/
mv ./$unzip_name/samples/API-Example/sdk.podspec ./$unzip_name/ || exit 1
python3 ./.github/ci/build/modify_podfile.py ./$unzip_name/samples/API-Example/Podfile || exit 1

ls -al ./$unzip_name/samples/API-Example/

7za a -tzip result.zip -r $unzip_name > log.txt
mv result.zip $WORKSPACE/withAPIExample_$(date "+%d%H%M")_$zip_name

if [ $compile_project = true ]; then
	cd ./$unzip_name/samples/API-Example
	./cloud_build.sh || exit 1
	cd -
fi

rm -rf ./$unzip_name

