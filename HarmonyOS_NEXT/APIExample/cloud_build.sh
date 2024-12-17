#! /bin/bash
# 获取当前脚本所在路径
PROJECT_PATH="$(cd "$(dirname "$0")" && pwd)"

## 配置 APP ID
# 替换 KeyCenter.ets 中的 APP ID 和证书
sed -i -e "s#YOUR APP ID#${APP_ID}#g" entry/src/main/ets/common/KeyCenter.ets
sed -i -e "s#YOUR APP CERTIFICATE##g" entry/src/main/ets/common/KeyCenter.ets
rm -f entry/src/main/ets/common/KeyCenter.ets-e

# 从 SDK URL 中提取版本号
SDK_VERSION=$(echo "$sdk_url" | awk -F'_' '{print $6}' | cut -d'.' -f1-3)

# 配置 Java 环境
JAVA_HOME="/home/jdk-17.0.6"
export PATH=$PATH:$JAVA_HOME/bin
java -version

COMMANDLINE_TOOL_DIR="/home/ohos-sdk/command-line-tools"

#配置hvigor、ohpm环境变量
export PATH=${COMMANDLINE_TOOL_DIR}/bin:$PATH

#配置hdc环境变量
init_hdc() {
    export HDC_HOME=${COMMANDLINE_TOOL_DIR}/sdk/default/openharmony/toolchains
    export PATH=$HDC_HOME:$PATH
}

# 安装ohpm
init_ohpm() {
    ohpm -v
    ohpm config set registry https://ohpm.openharmony.cn/ohpm/
}

# 进入package目录安装依赖
ohpm_install() {
    cd "$1"
    ohpm install
}

# 环境适配
buildHAP() {
    ohpm_install "${PROJECT_PATH}"
    ohpm_install "${PROJECT_PATH}/entry"
    cd ${PROJECT_PATH}
    hvigorw clean --no-daemon
    hvigorw assembleHap --mode module -p product=default -p buildMode=debug --no-daemon
}

signedHAP() {
    echo "[INFO] === Starting loadSignAndSigned ==="
    
    # 创建证书目录
    local config_dir=".ohos/config"
    mkdir -p "${config_dir}"
    
    # 下载证书
    local sign_url="http://10.80.1.174:8070/hmos/apiexample-hmos-sign.zip"
    local sign_file="${config_dir}/apiexample-hmos-sign.zip"
    
    echo "[INFO] 下载签名文件..."
    curl -o "${sign_file}" "${sign_url}"
    
    # 解压证书
    cd "${config_dir}"
    7za x -y apiexample-hmos-sign.zip
    cd - > /dev/null
    
    # 使用解压后的证书文件
    local cert_file=$(find "${config_dir}/sign" -name "*.cer")
    local p7b_file=$(find "${config_dir}/sign" -name "*.p7b")
    local p12_file=$(find "${config_dir}/sign" -name "*.p12")
    
    # 检查证书文件是否存在
    if [ ! -f "$cert_file" ] || [ ! -f "$p7b_file" ] || [ ! -f "$p12_file" ]; then
        echo "错误：未找到所需的证书文件"
        exit 1
    fi
    
    # 获取未签名的 HAP 文件
    local unsigned_hap="${PROJECT_PATH}/entry/build/default/outputs/default/entry-default-unsigned.hap"
    # 生成签名后的 HAP 文件名
    local signed_hap="${PROJECT_PATH}/APIExample_${BUILD_NUMBER}_${SDK_VERSION}_$(date "+%Y%m%d%H%M%S").hap"
    
    # 签名打包
    echo "开始签名打包..."
    java -jar ${COMMANDLINE_TOOL_DIR}/sdk/default/openharmony/toolchains/lib/hap-sign-tool.jar sign-app \
        -keyAlias "${HMOS_KEY_PWD}" \
        -signAlg "SHA256withECDSA" \
        -mode "localSign" \
        -appCertFile "$cert_file" \
        -profileFile "$p7b_file" \
        -keystoreFile "$p12_file" \
        -inFile "$unsigned_hap" \
        -outFile "$signed_hap" \
        -keyPwd "${HMOS_KEY_PWD}" \
        -keystorePwd "${HMOS_KEY_PWD}" \
        -signCode "1"
    
    if [ $? -ne 0 ]; then
        echo "[ERROR] HAP 签名失败"
        exit 1
    fi
    
    # 检查签名后的文件
    if [ ! -f "$signed_hap" ]; then
        echo "[ERROR] 签名后的 HAP 文件未生成"
        exit 1
    fi
    
    echo "[INFO] HAP 签名成功: $signed_hap"
}

function main() {
    local startTime=$(date '+%s')
    init_hdc
    init_ohpm
    buildHAP
    signedHAP

    local endTime=$(date '+%s')
    local elapsedTime=$(expr $endTime - $startTime)
    echo "构建成功，耗时 ${elapsedTime} 秒"
}

main
