import json
import sys
import tempfile
import textwrap
import unittest
from pathlib import Path


TOOLS_DIR = Path(__file__).resolve().parent
if str(TOOLS_DIR) not in sys.path:
    sys.path.insert(0, str(TOOLS_DIR))

from prepare_case_execution import (
    collect_sdk_version_checks,
    load_repository_profile,
    prepare_case_execution,
)
from validate_acceptance_manifest import validate_manifest


PLATFORMS = ["android", "ios", "macos", "windows"]


class PrepareCaseExecutionTest(unittest.TestCase):
    TARGET_SDK_VERSION = "4.6.4"

    def write_matrix(self):
        handle = tempfile.NamedTemporaryFile("w", suffix=".md", delete=False)
        handle.write(
            textwrap.dedent(
                """
                | Feature | SDK Family | Key APIs | Android full | iOS UIKit | macOS | Windows | Notes |
                | --- | --- | --- | --- | --- | --- | --- | --- |
                | Join channel audio | Full RTC | `joinChannel`, `setAudioProfile` | `DONE(app/JoinChannelAudio.java)` | `DONE/APIExample/JoinChannelAudio.swift` | `PARTIAL(APIExample/JoinChannelAudio.swift)` | `MISSING` | Keep all official platforms aligned. |

                ## Confirmed Gaps

                | Gap | Affected Units | Severity |
                | --- | --- | --- |
                | Basic audio-only join channel | Windows | High - missing foundational case |
                """
            )
        )
        handle.close()
        self.addCleanup(lambda: Path(handle.name).unlink(missing_ok=True))
        return Path(handle.name)

    def test_prepares_one_requirement_with_four_platform_delivery_units(self):
        package = prepare_case_execution(
            self.write_matrix(),
            feature="Join channel audio",
            target_sdk_version=self.TARGET_SDK_VERSION,
        )

        manifest = package["acceptance_manifest_seed"]
        self.assertEqual(manifest["version"], 4)
        self.assertEqual(sorted(manifest["platforms"]), PLATFORMS)
        self.assertIn("contract", manifest)
        self.assertNotIn("roles", manifest)
        self.assertEqual(sorted(package["role_contracts"]), ["contract", "implementation", "verification"])
        self.assertEqual(sorted(manifest["contract"]["output"]["platform_targets"]), PLATFORMS)
        self.assertEqual(manifest["requirement"]["target_sdk_version"], self.TARGET_SDK_VERSION)
        self.assertTrue(manifest["release"]["required"])
        self.assertEqual(manifest["release"]["target_sdk_version"], self.TARGET_SDK_VERSION)
        self.assertNotIn("qa_acceptance", manifest["release"])
        self.assertNotIn("publication_channel", manifest["requirement"])
        self.assertNotIn("publication", manifest["release"])
        for platform in PLATFORMS:
            unit = manifest["platforms"][platform]
            self.assertEqual(sorted(unit), ["implementation", "verification"])
            self.assertEqual(unit["implementation"]["dispatch"]["mode"], "pending")
            self.assertEqual(unit["verification"]["status"], "BLOCKED")
        self.assertEqual(validate_manifest(manifest), [])

    def test_platform_defaults_select_official_full_sdk_projects(self):
        package = prepare_case_execution(
            self.write_matrix(),
            feature="Join channel audio",
            target_sdk_version=self.TARGET_SDK_VERSION,
        )
        targets = package["acceptance_manifest_seed"]["contract"]["output"]["platform_targets"]

        self.assertEqual(targets["android"]["target_project"], "Android/APIExample/")
        self.assertEqual(targets["ios"]["target_project"], "iOS/APIExample/")
        self.assertEqual(targets["macos"]["target_project"], "macOS/")
        self.assertEqual(targets["windows"]["target_project"], "windows/")
        self.assertTrue(all(target["required"] for target in targets.values()))

    def test_selects_highest_priority_feature_when_omitted(self):
        package = prepare_case_execution(
            self.write_matrix(), target_sdk_version=self.TARGET_SDK_VERSION
        )

        self.assertEqual(package["requirement"]["feature"], "Join channel audio")
        self.assertEqual(package["requirement"]["key_apis"], ["joinChannel", "setAudioProfile"])

    def test_package_is_json_serializable(self):
        serialized = json.dumps(
            prepare_case_execution(
                self.write_matrix(), target_sdk_version=self.TARGET_SDK_VERSION
            )
        )
        self.assertIn('"version": 4', serialized)

    def test_prepares_new_requirement_not_yet_present_in_matrix(self):
        package = prepare_case_execution(
            self.write_matrix(),
            feature="Spatial audio",
            sdk_family="Full RTC",
            key_apis=["enableSpatialAudio"],
            target_sdk_version=self.TARGET_SDK_VERSION,
        )

        self.assertEqual(package["requirement"]["feature"], "Spatial audio")
        self.assertEqual(package["requirement"]["key_apis"], ["enableSpatialAudio"])
        self.assertEqual(sorted(package["acceptance_manifest_seed"]["platforms"]), PLATFORMS)

    def test_requires_target_sdk_version(self):
        with self.assertRaisesRegex(ValueError, "target_sdk_version is required"):
            prepare_case_execution(self.write_matrix(), feature="Join channel audio")

    def test_collects_live_sdk_version_evidence(self):
        for distribution in ["Shengwang", "Agora"]:
            with self.subTest(distribution=distribution), tempfile.TemporaryDirectory() as tmpdir:
                root = Path(tmpdir)
                profile_path = self.write_repository_profile(root, distribution)

                profile = load_repository_profile(profile_path)
                checks = collect_sdk_version_checks(
                    self.TARGET_SDK_VERSION,
                    repo_root=root,
                    sources=profile["sdk_version_sources"],
                )

                self.assertTrue(all(check["result"] == "PASS" for check in checks))
                self.assertTrue(
                    all(
                        set(check["actual_versions"].values()) == {self.TARGET_SDK_VERSION}
                        for check in checks
                    )
                )

                archive = root / "windows/install.ps1"
                archive.write_text(
                    f"{distribution}_Native_SDK_for_Windows_v4.6.2_FULL.zip\n",
                    encoding="utf-8",
                )
                checks = collect_sdk_version_checks(
                    self.TARGET_SDK_VERSION,
                    repo_root=root,
                    profile_path=profile_path,
                )
                windows = next(
                    check for check in checks if check["name"] == "sdk-version-windows"
                )
                self.assertEqual(windows["result"], "BLOCKED")
                self.assertIn("4.6.2", windows["reason"])

    def write_repository_profile(self, root, distribution):
        sources = {
            "android": [
                {
                    "path": "Android/gradle.properties",
                    "kind": "gradle-property",
                    "key": "rtc_sdk_version",
                }
            ],
            "ios": [
                {
                    "path": "iOS/Podfile",
                    "kind": "cocoapods",
                    "package": f"{distribution}RtcEngine_iOS",
                }
            ],
            "macos": [
                {
                    "path": "macOS/Podfile",
                    "kind": "cocoapods",
                    "package": f"{distribution}RtcEngine_macOS",
                }
            ],
            "windows": [
                {
                    "path": "windows/install.ps1",
                    "kind": "archive-name",
                    "prefix": f"{distribution}_Native_SDK_for_Windows_v",
                    "suffix": "_FULL.zip",
                }
            ],
        }
        contents = {
            "Android/gradle.properties": "rtc_sdk_version = 4.6.4\n",
            "iOS/Podfile": f"pod '{distribution}RtcEngine_iOS', '4.6.4'\n",
            "macOS/Podfile": f"pod '{distribution}RtcEngine_macOS', '4.6.4'\n",
            "windows/install.ps1": (
                f"{distribution}_Native_SDK_for_Windows_v4.6.4_FULL.zip\n"
            ),
        }
        for path_text, content in contents.items():
            path = root / path_text
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(content, encoding="utf-8")
        profile_path = root / "repository-profile.json"
        profile_path.write_text(
            json.dumps({"version": 1, "sdk_version_sources": sources}),
            encoding="utf-8",
        )
        return profile_path


if __name__ == "__main__":
    unittest.main()
