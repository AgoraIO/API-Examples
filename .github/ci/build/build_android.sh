##################################
# --- Guidelines: ---
#
# Common Environment Variable Injected:
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
export PATH=$PATH:/opt/homebrew/bin

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
echo android_direction: $android_direction

unzip_name=Agora_Native_SDK_for_Android_FULL_DEFAULT
zip_name=Agora_Native_SDK_for_Android_FULL_DEFAULT.zip
if [ -z "$sdk_url" ] || [ "$sdk_url" = "none" ]; then
   echo "sdk_url is empty"
   echo unzip_name: $unzip_name 
   echo zip_name: $zip_name
else
	zip_name=${sdk_url##*/}
	echo zip_name: $zip_name

	# env LC_ALL=en_US.UTF-8 python3 $WORKSPACE/artifactory_utils.py --action=download_file --file=$sdk_url || exit 1
	curl -o $zip_name $sdk_url || exit 1
	7za x ./$zip_name -y > log.txt

	# Support top-level directory name containing 'Agora' or 'Shengwang'
	unzip_name=`ls -S -d */ | grep -E 'Agora|Shengwang' | head -n 1 | sed 's/\///g'`
	if [ -z "$unzip_name" ]; then
		echo "Error: Unzipped directory not found. The SDK package structure may be invalid or the top-level directory does not contain 'Agora' or 'Shengwang'"
		exit 1
	fi
	echo unzip_name: $unzip_name

	rm -rf ./$unzip_name/rtc/bin
	rm -rf ./$unzip_name/rtc/demo
	rm -f ./$unzip_name/.commits
	rm -f ./$unzip_name/spec
	rm -rf ./$unzip_name/pom
fi

mkdir -p ./$unzip_name/rtc/samples/${android_direction} || exit 1
rm -rf ./$unzip_name/rtc/samples/${android_direction}/*

if [ -d "./Android/${android_direction}" ]; then
    cp -rf ./Android/${android_direction}/* ./$unzip_name/rtc/samples/${android_direction}/ || exit 1
else
    echo "Error: Source directory ./Android/${android_direction} does not exist"
    exit 1
fi

7za a -tzip result.zip -r $unzip_name > log.txt
mv result.zip $WORKSPACE/withAPIExample_${BUILD_NUMBER}_$zip_name

if [ $compress_apiexample = true ]; then
	onlyCodeZipName=${android_direction}_onlyCode.zip
	7za a -tzip $onlyCodeZipName -r ./$unzip_name/rtc/samples/${android_direction} >> log.txt
	mv $onlyCodeZipName $WORKSPACE/APIExample_onlyCode_${BUILD_NUMBER}_$zip_name
fi

if [ $compile_project = true ]; then
	cd ./$unzip_name/rtc/samples/${android_direction} || exit 1
	if [ -z "$sdk_url" ] || [ "$sdk_url" = "none" ]; then
		./cloud_build.sh false || exit 1
	else
		./cloud_build.sh true || exit 1
	fi
fi

