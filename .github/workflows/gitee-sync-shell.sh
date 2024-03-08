#!/bin/bash
git config --global user.email "sync2gitee@example.com"
git config --global user.name "sync2gitee"

pwd
git remote -v
git checkout dev/4.3.1

# change android maven to china repos
sed -ie "s#google()#maven { url \"https\://maven.aliyun.com/repository/public\" }\n        google()#g" Android/APIExample/settings.gradle
sed -ie "s#https://services.gradle.org/distributions#https://mirrors.cloud.tencent.com/gradle#g" Android/APIExample/gradle/wrapper/gradle-wrapper.properties
sed -ie "s#google()#maven { url \"https\://maven.aliyun.com/repository/public\" }\n        google()#g" Android/APIExample-Audio/settings.gradle
sed -ie "s#https://services.gradle.org/distributions#https://mirrors.cloud.tencent.com/gradle#g" Android/APIExample-Audio/gradle/wrapper/gradle-wrapper.properties
git add Android/APIExample/settings.gradle Android/APIExample/gradle/wrapper/gradle-wrapper.properties Android/APIExample-Audio/settings.gradle Android/APIExample-Audio/gradle/wrapper/gradle-wrapper.properties
git commit -m '[Android] gitee sync >> use china repos.'

# change iOS Podfile to china repos
sed -ie '1s/^/source "https://mirrors.tuna.tsinghua.edu.cn/git/CocoaPods/Specs.git"\n' iOS/APIExample/Podfile
sed -ie '1s/^/source "https://mirrors.tuna.tsinghua.edu.cn/git/CocoaPods/Specs.git"\n' iOS/APIExample-Audio/Podfile
sed -ie '1s/^/source "https://mirrors.tuna.tsinghua.edu.cn/git/CocoaPods/Specs.git"\n' iOS/APIExample-OC/Podfile
sed -ie '1s/^/source "https://mirrors.tuna.tsinghua.edu.cn/git/CocoaPods/Specs.git"\n' macOS/Podfile
git add iOS/APIExample/Podfile iOS/APIExample-Audio/Podfile iOS/APIExample-OC/Podfile macOS/Podfile
git commit -m '[iOS] gitee sync >> use china repos.'


git branch
git status
git push gitee


