// -*- mode: groovy -*-
// vim: set filetype=groovy :
@Library('agora-build-pipeline-library') _
import groovy.transform.Field

buildUtils = new agora.build.BuildUtils()

compileConfig = [
    "sourceDir": "api-examples",
    "docker": "hub.agoralab.co/server/ep/linux/ohos:0.0.1-5.0.5.200",
    "non-publish": [
        "command": "./.github/ci/build/build_hmos.sh",
        "extraArgs": "",
    ],
    "publish": [
        "command": "./.github/ci/build/build_hmos.sh",
        "extraArgs": "",
    ]
]

def doBuild(buildVariables) {
    type = params.Package_Publish ? "publish" : "non-publish"
    command = compileConfig.get(type).command
    preCommand = compileConfig.get(type).get("preCommand", "")
    postCommand = compileConfig.get(type).get("postCommand", "")
    docker = compileConfig.docker
    extraArgs = compileConfig.get(type).extraArgs
    extraArgs += " " + params.getOrDefault("extra_args", "")
    
    // 下载并解压签名文件
    if (params.Package_Publish) {
        dir(compileConfig.sourceDir) {
            def signFile = "${env.WORKSPACE}/apiexample-hmos-sign.zip"
            def extractDir = "${env.WORKSPACE}/signing"
            
            // 只在文件/目录存在时才清理
            echo "[INFO] 检查并清理本地签名文件和解压目录..."
            if (fileExists(signFile)) {
                sh "rm -f '${signFile}'"
            }
            if (fileExists(extractDir)) {
                sh "rm -rf '${extractDir}'"
            }
            
            echo "[INFO] 下载签名文件..."
            loadResources(["apiexample-hmos-sign.zip"], "publish")
            
            echo "[INFO] 签名文件路径: ${signFile}"
            
            // 使用 fileExists 步骤来检查文件是否存在
            if (!fileExists(signFile)) {
                echo "[ERROR] 下载后未找到签名文件: ${signFile}"
            }
            
            // 解压签名文件到自定义目录
            echo "[INFO] 解压签名文件到 ${extractDir}..."
            def result = sh(script: "7za x -y '${signFile}' -o'${extractDir}'", returnStatus: true)
            if (result != 0) {
                echo "[ERROR] 解压签名文件失败"
            }
            
            echo "[INFO] 将签名文件路径添加到 extraArgs: SIGN_PATH=${extractDir}"
            extraArgs += " SIGN_PATH=${extractDir}"
        }
    }
    
    def commandConfig = [
        "command": command,
        "sourceRoot": "${compileConfig.sourceDir}",
        "extraArgs": extraArgs,
        "docker": docker,
        "volumes": [
            "${env.WORKSPACE}:/workspace",
            "/tmp/jenkins:/tmp/jenkins"
        ]
    ]
    
    echo "[INFO] 构建配置: ${commandConfig}"
    
    loadResources(["config.json", "artifactory_utils.py"])
    buildUtils.customBuild(commandConfig, preCommand, postCommand)
}

def doPublish(buildVariables) {
    if (!params.Package_Publish) {
        return
    }
    (shortVersion, releaseVersion) = buildUtils.getBranchVersion()
    def archiveInfos = [
        [
          "type": "ARTIFACTORY",
          "archivePattern": "*.zip",
          "serverPath": "ApiExample/${shortVersion}/${buildVariables.buildDate}/${env.platform}",
          "serverRepo": "SDK_repo"
        ],
        [
          "type": "ARTIFACTORY",
          "archivePattern": "*.hap",
          "serverPath": "ApiExample/${shortVersion}/${buildVariables.buildDate}/${env.platform}",
          "serverRepo": "SDK_repo"
        ]
    ]
    archiveUrls = archive.archiveFiles(archiveInfos) ?: []
    archiveUrls = archiveUrls as Set
    if (archiveUrls) {
        def content = archiveUrls.join("\n")
        writeFile(file: 'package_urls', text: content, encoding: "utf-8")
    }
    archiveArtifacts(artifacts: "package_urls", allowEmptyArchive:true)
    sh "rm -rf *.zip *.hap || true"
}

pipelineLoad(this, "ApiExample", "build", "harmonyos", "apiexample_linux")