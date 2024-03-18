#!/bin/bash

if [ -d "ijkplayer/IJKMediaFramework.framework" ]; then
    echo "Folder exists"
    exit;
fi

rm -rf ijkplayer

git clone https://gitee.com/shengwang-dependencies/ijkplayer.git

rm -rf ijkplayer/IJKMediaFramework.framework
unzip ijkplayer/IJKMediaFramework.framework.zip -d ijkplayer/
rm -rf ijkplayer/ijkplayer
rm -rf ijkplayer/ijkplayer-Swift
rm -rf ijkplayer/*.zip
rm -rf ijkplayer/screenshot


