#!/usr/bin/env sh

## config beauty
# sed -i -e "s#io.agora.api.example#io.agora.entfull#g" app/build.gradle
# rm -f app/build.gradle-e
# cd app/src/main || exit 1
# curl -L -H "X-JFrog-Art-Api:${JFROG_API_KEY}" -O "https://artifactory-api.bj2.agoralab.co/artifactory/qa_test_data/beauty/vender_faceunity_resources_apiexample.zip"
# unzip -o vender_faceunity_resources_apiexample.zip
# rm -f vender_faceunity_resources_apiexample.zip

sed -i -e "s#io.agora.api.example#io.agora.entfull#g" app/build.gradle
rm -f app/build.gradle-e
cd app/src/main || exit 1
curl -L -O "https://artifactory.agoralab.co/artifactory/qa_test_data/beauty/vender_bytedance_4.6.0_resources.zip"
unzip -o vender_faceunity_resources_apiexample.zip
rm -f vender_faceunity_resources_apiexample.zip

