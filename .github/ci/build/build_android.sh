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

ls ~/.gradle || (mkdir -p /tmp/.gradle && ln -s /tmp/.gradle ~/.gradle && touch ~/.gradle/ln_$(date "+%y%m%d%H") && ls ~/.gradle)

zip_name=${sdk_url##*/}
echo zip_name: $zip_name

rm -rf Agora_Native_*
ls -al
# env LC_ALL=en_US.UTF-8 python3 $WORKSPACE/artifactory_utils.py --action=download_file --file=$sdk_url || exit 1
curl -o $zip_name $sdk_url || exit 1
7za x ./$zip_name -y > log.txt

unzip_name=`ls -S -d */ | grep Agora | sed 's/\///g'`
echo unzip_name: $unzip_name
ls -al $unzip_name
ls -al $unzip_name/libs
ls -al $unzip_name/libs/arm64-v8a

samples_dir=$(pwd)/$unzip_name/samples/API-Example
echo sample_dir: $samples_dir
rm -rf $samples_dir
mkdir -p $samples_dir
cp -rf ./Android/APIExample/** $samples_dir
ls -al $unzip_name/libs/arm64-v8a

7za a -tzip result.zip -r $unzip_name > log.txt
mv result.zip $WORKSPACE/withAPIExample_$(date "+%d%H%M")_$zip_name

# install android sdk
which java
java --version
source ~/.bashrc
export ANDROID_HOME=/usr/lib/android_sdk
echo ANDROID_HOME: $ANDROID_HOME

# compile apk
cd $samples_dir
echo compilingDir: $(pwd)

## config appId
sed -i -e "s#YOUR APP ID#${APP_ID}#g" app/src/main/res/values/string_configs.xml
sed -i -e "s#YOUR APP CERTIFICATE##g" app/src/main/res/values/string_configs.xml
sed -i -e "s#YOUR ACCESS TOKEN##g" app/src/main/res/values/string_configs.xml
rm -f app/src/main/res/values/string_configs.xml-e
cat app/src/main/res/values/string_configs.xml

./gradlew clean || exit 101
./gradlew :app:assembleDebug || exit 102
cp app/build/outputs/apk/debug/app-debug.apk ./APIExample_Android_$(date "+%y%m%d%H").apk
7za a -tzip result.zip -r *.apk > log.txt
mv result.zip $WORKSPACE/APIExample_Android$(echo $sdk_url | cut -d "/" -f 6 | grep VOICE | cut -d "_" -f 7 | cut -d "." -f 1 | sed -e 's/V/_V/g')_$(date "+%y%m%d%H%M")_apk.zip
ls $WORKSPACE
cd -

