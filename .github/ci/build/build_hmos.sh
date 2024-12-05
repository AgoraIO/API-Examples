# 定义下载和目标路径
ci_dir="$(cd "$(dirname "$0")" && pwd)" # 获取当前脚本所在路径
echo ci_dir: $ci_dir
base_dir=$(echo "$ci_dir" | awk -F "/.github" '{print $1}')
echo base_dir: $base_dir
source_root="$base_dir/HarmonyOS_NEXT/ApiExample"
lib_dir="$source_root/entry/libs"

echo Package_Publish: $Package_Publish
echo is_tag_fetch: $is_tag_fetch
echo source_root: %source_root%
echo output: /tmp/jenkins/${project}_out
echo build_date: $build_date
echo build_time: $build_time
echo release_version: $release_version
echo short_version: $short_version
echo pwd: `pwd`
echo sdk_url: $sdk_url

# 获取文件名
file_name=${sdk_url##*/}
echo "文件名: $file_name"

# 去掉 .har 扩展名
name_without_extension=${file_name%.har}
echo "文件名（不含扩展名）: $name_without_extension"

# 安装 curl
apt-get update && apt-get install -y curl zip
# 下载文件
curl -o "$base_dir/$file_name" "$sdk_url" || { echo "File download failed!"; exit 1; }

# 创建目标目录（如果不存在）
mkdir -p "$lib_dir"
# 重命名文件
mv "$base_dir/$file_name" "$lib_dir/AgoraRtcSdk.har" || { echo "File move failed!!"; exit 1; }


# 确保文件已移动后删除下载目录的原始文件
if [ -f "$lib_dir/AgoraRtcSdk.har" ]; then
  echo "File successfully moved to $lib_dir."
  rm -f "$base_dir/$file_name" # 删除原始文件
else
  echo "File move failed!"
  exit 1
fi

# 创建目录 Agora_Native_SDK_for_HarmonyOS_v4.4.2_FULL
sdk_sample_dir="$base_dir/$name_without_extension"
mkdir -p $sdk_sample_dir
# 创建子目录 rtc/samples
samples_dir="$sdk_sample_dir/rtc/samples"
mkdir -p $samples_dir
echo "创建子目录: $samples_dir"

hmos_direction=APIExample

cp -rf $base_dir/HarmonyOS_NEXT/${hmos_direction} $samples_dir/API-Example || exit 1

cd "$(dirname "$sdk_sample_dir")" || { echo "Failed to change directory!"; exit 1; }
zip -r "${sdk_sample_dir}.zip" "./$(basename "$sdk_sample_dir")" || { echo "Compression failed!"; exit 1; }
echo "目录已压缩为: ${sdk_sample_dir}.zip"
rm -rf "./$name_without_extension" #  删除整个目录及其内容
echo "目录已删除: $name_without_extension"

compile_project=true

if [ $compile_project = true ]; then
	which java
	java --version
    # source ~/.bashrc
	cd $source_root || { echo "cd HarmonyOS failed!"; exit 1; }
    echo pwd: `pwd`
    chmod +x ./cloud_build.sh
	./cloud_build.sh || exit 1
fi