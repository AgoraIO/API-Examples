# API Examples Release Known Issues

This file records repository-specific risks that can affect API Example source acceptance. Keep Jenkins packaging, QA validation, artifact distribution, and website publication outside the acceptance manifest.

## Repository Acceptance Risks

| Area | Risk | Repository Gate |
| --- | --- | --- |
| SDK versions | The Android, iOS, macOS, and Windows main APIExample version declarations or evidence can drift across release branches. | Validate every main-project source declared in `repository-profile.json` against the platform target SDK version; keep unverifiable external injection blocked. |
| iOS full demo SDK source | `iOS/APIExample/Podfile` uses a local `sdk.podspec`, while the ignored `libs/*.xcframework` binaries are injected outside the repository. The podspec package version and commented remote pod example do not prove the injected RTC SDK version. | Keep the repository SDK check `BLOCKED` until a trustworthy project-owned version source exists; a successful build alone is not version evidence. |
| Android extensions | Extension samples can retain headers or native assets from an older SDK. | Review extension `include` files and matching assets whenever the SDK dependency changes. |
| Third-party beauty | Sample licenses and bundled assets can expire or become incompatible. | Record the current license or asset limitation when the related example changes. |
| iOS/macOS signing | Local verification may not cover external package-signing configuration. | Treat signing and packaging as an external handoff; do not report repository acceptance as package completion. |
| Windows verification | macOS cannot provide MSBuild evidence. | Keep Windows build verification blocked until the role runs on a real Windows host. |

## Maintenance Rules

- Keep target SDK package names and version sources in `repository-profile.json`, not Python code.
- Add a reusable failure pattern to this file only when it is verified against this repository.
- Keep case coverage in `case-maintenance-matrix.md`; do not use this file as a second coverage tracker.
- Do not add Jenkins URLs, package artifact URLs, QA ownership, or website publication state to the acceptance manifest.
