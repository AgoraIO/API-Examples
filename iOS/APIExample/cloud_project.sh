#!/usr/bin/env sh

PROJECT_PATH=$PWD

if [ "$WORKSPACE" = "" ]; then
	WORKSPACE=$PWD
fi
if [ "$BUILD_NUMBER" = "" ]; then
	BUILD_NUMBER=888
fi

#下载美颜资源
echo "start download bytedance resource : $bytedance_lib"
curl -L -O "$bytedance_lib"
unzip -o vender_bytedance_iOS.zip
rm -f vender_bytedance_iOS.zip

echo "start download sense resource : $sense_lib"
curl -L -O "$sense_lib"
unzip -o vender_sense_iOS.zip
rm -f vender_sense_iOS.zip

echo "start download fu resource : $fu_lib"
curl -L -O "$fu_lib"
unzip -o vender_fu_iOS.zip
rm -f vender_fu_iOS.zip

#打开第三方播放器配置
sed -i -e "s#\#  pod 'ijkplayer'#  pod 'ijkplayer'#g" Podfile

#打开第三方美颜
sed -i -e "s#\#  pod 'SenseLib'#  pod 'SenseLib'#g" Podfile
sed -i -e "s#\#  pod 'bytedEffect'#  pod 'bytedEffect'#g" Podfile
sed -i -e "s#\#  pod 'fuLib'#  pod 'fuLib'#g" Podfile

echo "work space: $WORKSPACE"
echo "project path: $PROJECT_PATH"

pod install --repo-update || exit 1

#工程文件路径
APP_PATH="$(ls | grep xcworkspace)"

# 项目target名
TARGET_NAME=${APP_PATH%%.*}

7za a -tzip ${TARGET_NAME}.zip -r ./* > log.txt

echo "pwd path: $PWD"
ls -al
echo "target name: $TARGET_NAME"
OUTPUT_FILE=${WORKSPACE}/${TARGET_NAME}_$(date "+%Y%m%d%H%M%S").zip
mv ${TARGET_NAME}.zip $OUTPUT_FILE
