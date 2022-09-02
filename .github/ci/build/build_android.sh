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

python3 $WORKSPACE/artifactory_utils.py --action=download_file --file=$sdk_url
7za x ./$zip_name -y

unzip_name=`ls -S -d */ | grep Agora`
echo unzip_name: $unzip_name

mv $unzip_name/rtc/** $unzip_name
rm -rf $unzip_name/rtc
rm -rf ./$unzip_name/bin
rm -rf ./$unzip_name/demo
rm ./$unzip_name/commits
rm ./$unzip_name/package_size_report.txt
mkdir ./$unzip_name/samples
mkdir ./$unzip_name/samples/API-example

cp -rf ./Android/APIExample$(echo $sdk_url | cut -d "/" -f 9 | grep audio_only | cut -d "_" -f 1 | sed -e 's/a/-A/g')/** ./$unzip_name/samples/API-example

7za a -tzip result.zip -r $unzip_name
cp result.zip $WORKSPACE/withAPIExample_$zip_name

# install android sdk
which java
java --version
echo ${ANDROID_HOME}
ls -al ${ANDROID_HOME}

#cd ./$unzip_name/samples/
#mkdir AndroidSDK
#export ANDROID_HOME=$(pwd)/AndroidSDK
#cd -
#cd ${ANDROID_HOME}
#wget https://dl.google.com/android/repository/commandlinetools-linux-8512546_latest.zip
#unzip commandlinetools-linux-8512546_latest.zip
#export PATH=$(pwd)/cmdline-tools/bin:$PATH
#yes | sdkmanager --licenses --sdk_root=${ANDROID_HOME}
#yes | sdkmanager "platform-tools" "platforms;android-31" --sdk_root=${ANDROID_HOME}
#cd -

# compile apk
cd ./$unzip_name/samples/API-example
pwd
ls -al
sed -i -e "s#YOUR APP ID#${APP_ID}#g" app/src/main/res/values/string_configs.xml
sed -i -e "s#YOUR APP CERTIFICATE##g" app/src/main/res/values/string_configs.xml
sed -i -e "s#YOUR ACCESS TOKEN##g" app/src/main/res/values/string_configs.xml
rm -f app/src/main/res/values/string_configs.xml-e
cat app/src/main/res/values/string_configs.xml
./gradlew clean
./gradlew :app:assembleDebug
cp app/build/outputs/apk/debug/app-debug.apk $WORKSPACE/APIExample_Android_$(date "+%y%m%d%H").apk
ls $WORKSPACE
cd -


