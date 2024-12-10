# 定义下载和目标路径
ci_dir="$(cd "$(dirname "$0")" && pwd)" # 获取当前脚本所在路径
echo ci_dir: $ci_dir

base_dir=$(echo "$ci_dir" | awk -F "/.github" '{print $1}')
echo base_dir: $base_dir

source_root="$base_dir/HarmonyOS_NEXT/APIExample"
lib_dir="$source_root/entry/libs"

echo WORKSPACE: $WORKSPACE
echo Package_Publish: $Package_Publish
echo is_tag_fetch: $is_tag_fetch
echo output: /tmp/jenkins/${project}_out
echo build_date: $build_date
echo build_time: $build_time
echo release_version: $release_version
echo short_version: $short_version
echo pwd: `pwd`
echo sdk_url: $sdk_url

# 获取文件名
file_name=${sdk_url##*/}
name_without_extension=${file_name%.har}
echo "文件名: $file_name"
echo "文件名（不含扩展名）: $name_without_extension"

# 函数：检查并安装 curl
install_curl_if_needed() {
    if command -v curl >/dev/null 2>&1; then
        echo "curl 已安装，版本：$(curl --version | head -n 1)"
    else
        echo "curl 未安装，正在尝试安装..."
        
        # 备份原始源文件
        cp /etc/apt/sources.list /etc/apt/sources.list.backup
        # 使用阿里云镜像源
        echo "deb http://mirrors.aliyun.com/ubuntu/ focal main restricted universe multiverse" > /etc/apt/sources.list
        echo "deb http://mirrors.aliyun.com/ubuntu/ focal-updates main restricted universe multiverse" >> /etc/apt/sources.list
        echo "deb http://mirrors.aliyun.com/ubuntu/ focal-security main restricted universe multiverse" >> /etc/apt/sources.list
        
        # 更新并安装，忽略 GPG 错误
        apt-get update --allow-unauthenticated || true
        apt-get install -y --allow-unauthenticated curl
        
        # 恢复原始源文件
        mv /etc/apt/sources.list.backup /etc/apt/sources.list

        # 验证安装是否成功
        if command -v curl >/dev/null 2>&1; then
            echo "curl 安装成功，版本：$(curl --version | head -n 1)"
        else
            echo "curl 安装失败，请检查日志"
            return 1
        fi
    fi
}

# 安装 curl
install_curl_if_needed || exit 1;

# 下载文件
curl -o "$base_dir/$file_name" "$sdk_url" || { echo "文件下载失败！"; exit 1; }

# 创建目标目录（如果不存在）
mkdir -p "$lib_dir" || { echo "无法创建目录 $lib_dir"; exit 1; }

# 移动文件并重命名
mv "$base_dir/$file_name" "$lib_dir/AgoraRtcSdk.har" || { echo "文件移动失败！"; exit 1; }


# 确认文件已移动
if [ ! -f "$lib_dir/AgoraRtcSdk.har" ]; then
    echo "文件移动失败！"
    exit 1
fi

# 创建 SDK 样例目录
sdk_sample_dir="$base_dir/$name_without_extension"
mkdir -p "$sdk_sample_dir" || { echo "无法创建目录 $sdk_sample_dir"; exit 1; }

# 创建子目录
samples_dir="$sdk_sample_dir/rtc/samples"
mkdir -p "$samples_dir" || { echo "无法创建目录 $samples_dir"; exit 1; }
echo "创建子目录: $samples_dir"

# 复制文件
hmos_direction=APIExample
cp -rf "$base_dir/HarmonyOS_NEXT/${hmos_direction}" "$samples_dir/API-Example" || exit 1

# 复制签名文件到目标目录
if [ -n "${SIGN_FILE}" ] && [ -f "${SIGN_FILE}" ]; then
    echo "[INFO] Found sign file: ${SIGN_FILE}"
    echo "[INFO] Copying sign file to target directory: ${source_root}/HarmonyOS_NEXT/APIExample/"
    
    # 确保目标目录存在
    mkdir -p "${source_root}/HarmonyOS_NEXT/APIExample/"
    
    # 复制文件并验证
    if cp -v "${SIGN_FILE}" "${source_root}/HarmonyOS_NEXT/APIExample/apiexample-hmos-sign.zip"; then
        echo "[INFO] Sign file copy completed"
        # 验证复制后的文件
        if [ -f "${source_root}/HarmonyOS_NEXT/APIExample/apiexample-hmos-sign.zip" ]; then
            echo "[INFO] Verified: Sign file exists in target location"
            ls -l "${source_root}/HarmonyOS_NEXT/APIExample/apiexample-hmos-sign.zip"
        else
            echo "[ERROR] Failed to verify copied sign file"
            exit 1
        fi
    else
        echo "[ERROR] Failed to copy sign file"
        exit 1
    fi
else
    echo "[WARN] Sign file not found or not specified: ${SIGN_FILE}"
    echo "[INFO] Current directory: $(pwd)"
    echo "[INFO] Directory contents:"
    ls -la
fi

# 压缩并清理
cd "$(dirname "$sdk_sample_dir")" || { echo "切换目录失败！"; exit 1; }
7za a "${sdk_sample_dir}.zip" "./$(basename "$sdk_sample_dir")" || { echo "压缩失败！"; exit 1; }
rm -rf "./$name_without_extension"
echo "目录已删除: $name_without_extension"

if [ "$compile_project" = true ]; then
    cd "$source_root" || { echo "切换到 HarmonyOS 源代码目录失败！"; exit 1; }
    chmod +x ./cloud_build.sh
    ./cloud_build.sh || exit 1
fi