#!/usr/bin/env sh

# cache gradle to /tmp/.gradle
ls ~/.gradle || (mkdir -p /tmp/.gradle && ln -s /tmp/.gradle ~/.gradle && touch ~/.gradle/ln_$(date "+%y%m%d%H") && ls ~/.gradle)

## use open jdk 17
if [ ! -d "/tmp/jdk-17" ];then
  mkdir -p /tmp/jdk-17
  wget https://download.java.net/java/GA/jdk17.0.2/dfd4a8d0985749f896bed50d7138ee7f/8/GPL/openjdk-17.0.2_linux-x64_bin.tar.gz
  tar xvf openjdk-17.0.2_linux-x64_bin.tar.gz
  mv jdk-17.0.2/ /tmp/jdk-17/
fi
export JAVA_HOME=/tmp/jdk-17
export PATH=$JAVA_HOME/bin:$PATH
java --version

## config appId
if [ ! -f "local.properties" ];then
  touch local.properties
  echo "AGORA_APP_ID=${APP_ID}" >> local.properties
fi

./gradlew clean || exit 1
./gradlew :app:assembleRelease || exit 1
cp app/build/outputs/apk/release/*.apk $WORKSPACE