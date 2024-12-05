#! /bin/bash
startTime=$(date '+%s')
project_dir="$(cd "$(dirname "$0")" && pwd)" # 获取当前脚本所在路径

WORKSPACE=$project_dir
BUILD_NUMBER=100

## config appid
sed -i -e "s#YOUR APP ID#${APP_ID}#g" entry/src/main/ets/common/KeyCenter.ets
sed -i -e "s#YOUR APP CERTIFICATE##g" entry/src/main/ets/common/KeyCenter.ets
rm -f entry/src/main/ets/common/KeyCenter.ets-e

# COMMANDLINE_TOOL_DIR="/Users/zhangwei03"

## 配置hvigor、ohpm环境变量
# export PATH=${COMMANDLINE_TOOL_DIR}/command-line-tools/bin:$PATH

## 配置hdc环境变量
# export HDC_HOME=${COMMANDLINE_TOOL_DIR}/command-line-tools/sdk/default/openharmony/toolchains # 设置hdc工具的环境变量，hdc工具在toolchains所在路径下
# export PATH=$HDC_HOME:$PATH

## 安装ohpm, 若镜像中已存在ohpm，则无需重新安装
# ohpm -v
## 配置ohpm仓库地址
# ohpm config set registry https://ohpm.openharmony.cn/ohpm/

function ohpm_install {
    cd $1
    ohpm install
}

# 安装依赖
ohpm_install "$project_dir/entry"

## 清除并构建应用
cd $project_dir
hvigorw clean --no-daemon || exit 1
hvigorw assembleHap --mode module -p product=default -p buildMode=release -p debuggable=false --no-daemon || exit 1  #流水线构建命令建议末尾加上--no-daem

endTime=$(date '+%s')
elapsedTime=$(($endTime - $startTime))
echo "build success in ${elapsedTime}s..."

if [ -d "$project_dir" ]; then
  # 查找并删除根目录下的 .hap 文件
  hap_files=$(find "$project_dir" -maxdepth 1 -type f -name "*.hap")
  if [ -n "$hap_files" ]; then
    echo "Deleting .hap files in $project_dir..."
    rm -f $hap_files
    echo "All .hap files in $project_dir have been deleted."
  else
    echo "No .hap files found in $project_dir."
  fi
else
  echo "Directory $project_dir does not exist."
fi

if [ "$WORKSPACE" != "" ]; then
SDK_VERSION=$(echo "$sdk_url" | awk -F'_' '{print $6}' | cut -d'.' -f1-3)
cp $project_dir/entry/build/default/outputs/default/*-signed.hap $WORKSPACE/APIExample_${BUILD_NUMBER}_${SDK_VERSION}_$(date "+%Y%m%d%H%M%S").hap
fi