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
    
    // 下载签名文件
    if (params.Package_Publish) {
        dir(compileConfig.sourceDir) {
            try {
                echo "[INFO] Downloading sign file..."
                loadResources(["apiexample-hmos-sign.zip"], "publish")
                
                def signFile = sh(script: 'readlink -f apiexample-hmos-sign.zip', returnStdout: true).trim()
                echo "[INFO] Sign file path: ${signFile}"
                
                if [ ! -f "${signFile}" ]; then
                    echo "[ERROR] Sign file not found after download: ${signFile}"
                    exit 1
                fi
                
                echo "[INFO] Sign file details:"
                ls -lh "${signFile}"
                
                # 确保目标目录存在
                mkdir -p /tmp/jenkins/api-examples/HarmonyOS_NEXT/APIExample/
                
                # 复制文件到目标位置
                cp -v "${signFile}" /tmp/jenkins/api-examples/HarmonyOS_NEXT/APIExample/apiexample-hmos-sign.zip
                
                # 验证复制后的文件
                if [ -f "/tmp/jenkins/api-examples/HarmonyOS_NEXT/APIExample/apiexample-hmos-sign.zip" ]; then
                    echo "[INFO] File successfully copied to target location"
                    ls -lh /tmp/jenkins/api-examples/HarmonyOS_NEXT/APIExample/apiexample-hmos-sign.zip
                else
                    echo "[ERROR] Failed to copy file to target location"
                    exit 1
                fi
                
                echo "[INFO] Adding sign file to extraArgs: SIGN_FILE=${signFile}"
                extraArgs += " SIGN_FILE=${signFile}"
            } catch (Exception e) {
                echo "[ERROR] Failed to download sign file: ${e.message}"
                echo "[ERROR] Stack trace:"
                e.printStackTrace()
                throw e
            }
        }
    }
    
    def commandConfig = [
        "command": command,
        "sourceRoot": "${compileConfig.sourceDir}",
        "extraArgs": extraArgs,
        "docker": docker,
        "volumes": [
            "/home/jenkins/workspace/ApiExample/build_harmonyos:/home/jenkins/workspace/ApiExample/build_harmonyos",
            "/tmp/jenkins:/tmp/jenkins"
        ]
    ]
    
    echo "[INFO] Build config: ${commandConfig}"
    
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