# Release Risks

This file records verified repository-specific conditions that can change implementation or
validation decisions. Jenkins execution state, QA status, artifact URLs, and temporary
incidents belong in the release ticket rather than this document.

| Area | Verified risk | Required handling |
| --- | --- | --- |
| Public SDK availability | Release branches can reference an SDK version before Maven, CocoaPods, or the Windows download endpoint publishes it. | Use an approved local/internal SDK source for pre-release validation or record the public-source build as blocked. Run `.github/workflows/compile.yml` after the version reaches `main`. |
| iOS UIKit full SDK source | `iOS/APIExample/Podfile` uses a local `sdk.podspec`; the binary is injected outside Git. Compile CI rewrites this dependency to the published `AgoraRtcEngine_iOS` version. | Verify the injected SDK version during release packaging and verify the published pod separately in compile CI. Do not treat one as evidence for the other. |
| Android native extensions | Optional extension projects use SDK headers and native assets outside the normal Maven dependency. | Review headers and matching native assets whenever the RTC SDK version changes. |
| Apple signing | Repository compilation disables signing and cannot prove that Jenkins signing certificates or profiles are valid. | Keep signing verification in the release pipeline and record its evidence in the release ticket. |
| Windows validation | Windows uses architecture-specific files from either `../../sdk` or the public full archive and requires MSBuild. | Run dependency installation and compilation on a Windows worker; a non-Windows source review is not build evidence. |
| Packaging versus compilation | GitHub Actions checks source compilation; `.github/ci/build/` owns distributable artifacts. | Do not report compile CI success as proof that release packages were produced correctly. |

Update this file only when the condition is reproducible from tracked code or current release
automation. Remove entries when the underlying condition no longer exists.
