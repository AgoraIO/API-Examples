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
cp app/build/outputs/apk/release/*.apk $WORKSPACE/APIExample-Audio_${BUILD_NUMBER}_${SDK_VERSION}_$(date "+%Y%m%d%H%M%S").apk
fi