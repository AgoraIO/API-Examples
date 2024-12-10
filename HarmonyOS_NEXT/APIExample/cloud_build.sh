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

loadSignAndSigned() {
    echo "[INFO] === Starting loadSignAndSigned ==="
    
    # 使用从 groovy 传来的 SIGN_PATH 环境变量
    local extract_dir="${SIGN_PATH}"
    local cert_file=$(find "${extract_dir}" -name "*.cer")
    local p7b_file=$(find "${extract_dir}" -name "*.p7b")
    local p12_file=$(find "${extract_dir}" -name "*.p12")
    
    echo "[INFO] 使用签名文件路径: ${extract_dir}"
    
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
    java -jar ${COMMANDLINE_TOOL_DIR}/hap-sign-tool.jar sign-app \
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
        echo "签名打包失败"
        exit 1
    fi
    echo "签名打包成功"
    
    # 检查签名后的文件并清理
    if [ -f "$signed_hap" ]; then
        echo "签名后的HAP文件已生成：$signed_hap"
        rm -f "$unsigned_hap"
    else
        echo "错误：签名后的HAP文件未生成"
        exit 1
    fi
}

function main() {
    local startTime=$(date '+%s')
    init_hdc
    init_ohpm
    buildHAP
    # loadSignAndSigned

    local endTime=$(date '+%s')
    local elapsedTime=$(expr $endTime - $startTime)
    echo "构建成功，耗时 ${elapsedTime} 秒"
}

main
