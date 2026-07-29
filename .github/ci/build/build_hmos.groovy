// -*- mode: groovy -*-
// vim: set filetype=groovy :
@Library('agora-build-pipeline-library') _

buildUtils = new agora.build.BuildUtils()

compileConfig = [
    "sourceDir": "api-examples",
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
    extraArgs = compileConfig.get(type).extraArgs
    extraArgs += " " + params.getOrDefault("extra_args", "")
    commandConfig = [
        "command": command,
        "sourceRoot": "${compileConfig.sourceDir}",
        "extraArgs": extraArgs
    ]
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
    def archiveUrls = (archive.archiveFiles(archiveInfos) ?: []) as Set
    if (archiveUrls) {
        writeFile(file: "package_urls", text: archiveUrls.join("\n"), encoding: "utf-8")
    }
    archiveArtifacts(artifacts: "package_urls", allowEmptyArchive: true)
    sh "rm -f -- *.zip *.hap"
}

pipelineLoad(this, "ApiExample", "build", "harmonyos", "RTC-Sample")
