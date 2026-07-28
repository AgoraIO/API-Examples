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
    def type = params.Package_Publish ? "publish" : "non-publish"
    def commandConfig = [
        "command": compileConfig.get(type).command,
        "sourceRoot": "${compileConfig.sourceDir}",
        "extraArgs": compileConfig.get(type).extraArgs + " " + params.getOrDefault("extra_args", "")
    ]

    loadResources(["config.json", "artifactory_utils.py"])
    withCredentials([
        string(
            credentialsId: "hmos-apiexample-signing-secret",
            variable: "HMOS_SIGNING_SECRET"
        )
    ]) {
        buildUtils.customBuild(commandConfig, "", "")
    }
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
