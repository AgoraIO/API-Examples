#!/usr/bin/env sh

# cache gradle to /tmp/.gradle
ls ~/.gradle || (mkdir -p /tmp/.gradle && ln -s /tmp/.gradle ~/.gradle && touch ~/.gradle/ln_$(date "+%y%m%d%H") && ls ~/.gradle)

#change android maven to china repos
sed -ie "s#google()#maven { url \"https\://maven.aliyun.com/repository/public\" }\n        google()#g" settings.gradle
sed -ie "s#https://services.gradle.org/distributions#https://mirrors.cloud.tencent.com/gradle#g" gradle/wrapper/gradle-wrapper.properties

## config appId
sed -i -e "s#YOUR APP ID#${APP_ID}#g" app/src/main/res/values/string_configs.xml
sed -i -e "s#YOUR APP CERTIFICATE##g" app/src/main/res/values/string_configs.xml
sed -i -e "s#YOUR ACCESS TOKEN##g" app/src/main/res/values/string_configs.xml
rm -f app/src/main/res/values/string_configs.xml-e
echo "First argument: $1"
echo "Second argument: $2"
if [ "$1" = "false" ]; then
     echo "clould build sdk_url is empty"
else
    echo "clould build sdk_url is not empty"
    ## config simple filter
    sed -i -e "s#simpleFilter = false#simpleFilter = true#g" gradle.properties
    rm -f gradle.properties-e
    mkdir -p agora-simple-filter/src/main/agoraLibs
    cp -r ../../sdk/arm64-v8a agora-simple-filter/src/main/agoraLibs/
    cp -r ../../sdk/armeabi-v7a agora-simple-filter/src/main/agoraLibs/
    curl -o opencv4.zip https://agora-adc-artifacts.s3.cn-north-1.amazonaws.com.cn/androidLibs/opencv4.zip
    unzip opencv4.zip
    mkdir -p agora-simple-filter/src/main/libs
    mv arm64-v8a agora-simple-filter/src/main/libs
    mv armeabi-v7a agora-simple-filter/src/main/libs
    sed -i -e "s#jniLibs/#libs/#g" agora-simple-filter/src/main/cpp/CMakeLists.txt
    rm -f agora-simple-filter/src/main/cpp/CMakeLists.txt-e

    ## config agora stream encrypt
    sed -i -e "s#streamEncrypt = false#streamEncrypt = true#g" gradle.properties
    rm -f gradle.properties-e
    mkdir -p agora-stream-encrypt/src/main/agoraLibs
    cp -r ../../sdk/arm64-v8a agora-stream-encrypt/src/main/agoraLibs/
    cp -r ../../sdk/armeabi-v7a agora-stream-encrypt/src/main/agoraLibs/
fi


## config beauty
sed -i -e "s#io.agora.api.example#io.agora.entfull#g" app/build.gradle
rm -f app/build.gradle-e
cd app/src/main || exit 1
curl -L -H "X-JFrog-Art-Api:${JFROG_API_KEY}" -O "https://artifactory-api.bj2.agoralab.co/artifactory/qa_test_data/beauty/vender_faceunity_resources_apiexample.zip"
unzip -o vender_faceunity_resources_apiexample.zip
rm -f vender_faceunity_resources_apiexample.zip
curl -L -H "X-JFrog-Art-Api:${JFROG_API_KEY}" -O "https://artifactory-api.bj2.agoralab.co/artifactory/qa_test_data/beauty/vender_bytedance_resources_apiexample.zip"
unzip -o vender_bytedance_resources_apiexample.zip
rm -f vender_bytedance_resources_apiexample.zip
curl -L -H "X-JFrog-Art-Api:${JFROG_API_KEY}" -O "https://artifactory-api.bj2.agoralab.co/artifactory/qa_test_data/beauty/vender_sensetime_resources_apiexample.zip"
unzip -o vender_sensetime_resources_apiexample.zip
rm -f vender_sensetime_resources_apiexample.zip
cd - || exit 1

./gradlew clean || exit 1
./gradlew :app:assembleRelease || exit 1

SDK_VERSION=""
if [ "$1" = "false" ]; then
    sdk_version_file="./gradle.properties"
    if [[ -f "$sdk_version_file" ]]; then
    rtc_sdk_version=$(grep "rtc_sdk_version" "$sdk_version_file" | cut -d'=' -f2)
    if [[ -n "$rtc_sdk_version" ]]; then
        SDK_VERSION=$(echo "$rtc_sdk_version" | sed 's/^[ \t]*//;s/[ \t]*$//')
    else
        echo "rtc_sdk_version value not found"
    fi
else
    echo "file not found: $sdk_version_file"
fi
else
    SDK_VERSION=$(echo $sdk_url | cut -d "/" -f 5)
fi

if [ "$WORKSPACE" != "" ]; then
cp app/build/outputs/apk/release/*.apk $WORKSPACE/APIExample_${BUILD_NUMBER}_${SDK_VERSION}_$(date "+%Y%m%d%H%M%S").apk
fi