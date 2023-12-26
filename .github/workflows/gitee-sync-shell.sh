#!/bin/bash
git config --global user.email "sync2gitee@example.com"
git config --global user.name "sync2gitee"

pwd
git remote -v

# change android maven to china repos
sed -ie "s#google()#maven { url \"https\://maven.aliyun.com/repository/public\" }\n        google()#g" Android/APIExample/settings.gradle
sed -ie "s#https://services.gradle.org/distributions#https://mirrors.cloud.tencent.com/gradle#g" Android/APIExample/gradle/wrapper/gradle-wrapper.properties
sed -ie "s#google()#maven { url \"https\://maven.aliyun.com/repository/public\" }\n        google()#g" Android/APIExample-Audio/settings.gradle
sed -ie "s#https://services.gradle.org/distributions#https://mirrors.cloud.tencent.com/gradle#g" Android/APIExample-Audio/gradle/wrapper/gradle-wrapper.properties
git add Android/APIExample/settings.gradle Android/APIExample/gradle/wrapper/gradle-wrapper.properties Android/APIExample-Audio/settings.gradle Android/APIExample-Audio/gradle/wrapper/gradle-wrapper.properties
git commit -m '[Android] gitee sync >> use china repos.'

git branch
git status
git push gitee


