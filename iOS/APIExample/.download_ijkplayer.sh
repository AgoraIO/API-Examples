#!/bin/bash

if [ -d "IJKMediaFramework.framework" ]; then
    echo "Folder exists"
    exit;
fi

git clone https://gitee.com/shengwang-dependencies/ijkplayer.git

unzip ijkplayer/IJKMediaFramework.framework.zip

rm -rf ijkplayer