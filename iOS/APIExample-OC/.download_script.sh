#!/bin/bash
version=$1
url="https://download.agora.io/sdk/release/Agora_Native_SDK_for_iOS_v${version}_FULL.zip"
# https://download.agora.io/sdk/release/Agora_Native_SDK_for_iOS_v4.3.0_VOICE.zip
zip_filename=$(basename "$url")

if [ -d "libs" ] && [ "$2" = "false" ]; then
    echo "Folder exists"
    exit;
fi

rm -rf libs

# 使用curl命令下载文件
echo 'download Agora RTC SDK...'
curl -o "$zip_filename" "$url"

unzip $zip_filename

folder_name=$(unzip -qql $zip_filename | head -n1 | tr -s ' ' | cut -d' ' -f5-)  # 获取文件夹名称
mv "${folder_name}libs" .

rm -rf $zip_filename
rm -rf $folder_name

echo "download sdk finished！"