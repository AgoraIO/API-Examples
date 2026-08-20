# API Examples Release Known Issues

This file records repository-specific risks that can affect API Example source acceptance. Keep Jenkins packaging, QA validation, artifact distribution, and website publication outside the acceptance manifest.

## Repository Acceptance Risks

| Area | Risk | Repository Gate |
| --- | --- | --- |
| SDK versions | Android, iOS, macOS, and Windows dependency declarations can drift across release branches. | Validate every source declared in `repository-profile.json` against the target SDK version. |
| iOS full demo SDK source | `iOS/APIExample/Podfile` keeps `pod 'AgoraRtcEngine_iOS'` commented out and builds from the local `sdk.podspec`, which vendors `libs/*.xcframework` and declares an unrelated `spec.version`. No RTC semver is readable from the repository, so `sdk-version-ios` reports `BLOCKED` with `no active SDK version declaration`. A commented-out pod line is never accepted as evidence. | Before release, restore the pinned `pod 'AgoraRtcEngine_iOS', '<target>'` line, or record an explicit waiver stating the SDK was supplied out-of-band and state how its version was confirmed. |
| Android extensions | Extension samples can retain headers or native assets from an older SDK. | Review extension `include` files and matching assets whenever the SDK dependency changes. |
| Third-party beauty | Sample licenses and bundled assets can expire or become incompatible. | Record the current license or asset limitation when the related example changes. |
| iOS/macOS signing | Local verification may not cover external package-signing configuration. | Treat signing and packaging as an external handoff; do not report repository acceptance as package completion. |
| Windows verification | macOS cannot provide MSBuild evidence. | Keep Windows build verification blocked until the role runs on a real Windows host. |

## Maintenance Rules

- Keep target SDK package names and version sources in `repository-profile.json`, not Python code.
- Add a reusable failure pattern to this file only when it is verified against this repository.
- Keep case coverage in `case-maintenance-matrix.md`; do not use this file as a second coverage tracker.
- Do not add Jenkins URLs, package artifact URLs, QA ownership, or website publication state to the acceptance manifest.
