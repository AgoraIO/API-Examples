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

zip_name=${sdk_url##*/}
echo zip_name: $zip_name

# env LC_ALL=en_US.UTF-8 python3 $WORKSPACE/artifactory_utils.py --action=download_file --file=$sdk_url || exit 1
curl -o $zip_name $sdk_url || exit 1
7za x ./$zip_name -y > log.txt

unzip_name=`ls -S -d */ | grep Agora | sed 's/\///g'`
echo unzip_name: $unzip_name

rm -rf ./$unzip_name/rtc/bin
rm -rf ./$unzip_name/rtc/demo
rm ./$unzip_name/rtc/commits
rm ./$unzip_name/rtc/package_size_report.txt
mkdir ./$unzip_name/rtc/samples
mkdir ./$unzip_name/rtc/samples/API-example
cp -rf ./Android/APIExample$(echo $sdk_url | cut -d "/" -f 9 | grep audio_only | cut -d "_" -f 1 | sed -e 's/a/-A/g')/** ./$unzip_name/rtc/samples/API-example

7za a -tzip result.zip -r $unzip_name > log.txt
mv result.zip $WORKSPACE/withAPIExample_$(date "+%d%H%M")_$zip_name

# install android sdk
which java
java --version
source ~/.bashrc
export ANDROID_HOME=/usr/lib/android_sdk
echo ANDROID_HOME: $ANDROID_HOME

# compile apk
cd ./$unzip_name/rtc/samples/API-example
pwd

## config appId
sed -i -e "s#YOUR APP ID#${APP_ID}#g" app/src/main/res/values/string_configs.xml
sed -i -e "s#YOUR APP CERTIFICATE##g" app/src/main/res/values/string_configs.xml
sed -i -e "s#YOUR ACCESS TOKEN##g" app/src/main/res/values/string_configs.xml
rm -f app/src/main/res/values/string_configs.xml-e
cat app/src/main/res/values/string_configs.xml

## config simple filter
sed -i -e "s#simpleFilter = false#simpleFilter = true#g" gradle.properties
mkdir -p agora-simple-filter/src/main/agoraLibs
cp -r ../../sdk/arm64-v8a agora-simple-filter/src/main/agoraLibs/
cp -r ../../sdk/armeabi-v7a agora-simple-filter/src/main/agoraLibs/
curl -o opencv4.zip https://agora-adc-artifacts.s3.cn-north-1.amazonaws.com.cn/androidLibs/opencv4.zip
unzip opencv4.zip
mkdir -p agora-simple-filter/src/main/jniLibs2
mv arm64-v8a agora-simple-filter/src/main/jniLibs2
mv armeabi-v7a agora-simple-filter/src/main/jniLibs2
sed -i -e "s#jniLibs/#jniLibs2/#g" agora-simple-filter/src/main/cpp/CMakeLists.txt

./gradlew clean || exit 1
./gradlew :app:assembleDebug || exit 1
cp app/build/outputs/apk/debug/app-debug.apk ./APIExample_Android_$(date "+%y%m%d%H").apk
7za a -tzip result.zip -r *.apk > log.txt
mv result.zip $WORKSPACE/APIExample_Android$(echo $sdk_url | cut -d "/" -f 9 | grep audio_only | cut -d "_" -f 1 | sed -e 's/a/_A/g')_$(date "+%y%m%d%H%M")_apk.zip
ls $WORKSPACE
cd -

