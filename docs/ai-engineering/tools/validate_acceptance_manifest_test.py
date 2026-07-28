import copy
import hashlib
import json
import sys
import tempfile
import unittest
from pathlib import Path


TOOLS_DIR = Path(__file__).resolve().parent
if str(TOOLS_DIR) not in sys.path:
    sys.path.insert(0, str(TOOLS_DIR))

from validate_acceptance_manifest import (
    is_durable_knowledge_path,
    validate_evidence_files,
    validate_manifest,
)


PLATFORMS = ["android", "ios", "macos", "windows"]
TARGETS = {
    "android": "Android/APIExample/",
    "ios": "iOS/APIExample/",
    "macos": "macOS/",
    "windows": "windows/",
}
SNAPSHOT_SHA256 = "c" * 64
BUILD_COMMANDS = {
    "android": "./gradlew assembleDebug",
    "ios": "xcodebuild -scheme APIExample build",
    "macos": "xcodebuild -scheme APIExample build",
    "windows": "msbuild APIExample.sln /t:Build",
}


def dispatch_for(name, profile):
    platform = name.split("-")[0]
    dispatch = {
        "mode": "codex-exec",
        "run_id": f"run-{name}",
        "prompt": f"role-prompts/{name}.md",
        "prompt_sha256": "a" * 64,
        "artifact": f"role-artifacts/{name}.json",
        "artifact_sha256": "b" * 64,
        "evidence": f"Independent {name} run.",
        "model_profile": profile,
        "model": "test-model",
        "reasoning_effort": "high" if profile in {"deep", "review"} else "medium",
        "codex_version": "codex-cli test",
        "input_snapshot": f"input-snapshots/{name}.json",
        "input_snapshot_sha256": SNAPSHOT_SHA256,
        "command_log": f"dispatch-logs/{name}.jsonl",
        "command_log_sha256": "d" * 64,
        "working_directory": TARGETS.get(name.split("-")[0], "."),
        "host_platform": "win32" if platform == "windows" else "darwin",
    }
    if name.endswith("-implementation"):
        dispatch["repository_delta"] = f"repository-deltas/{name}.json"
        dispatch["repository_delta_sha256"] = "e" * 64
    return dispatch


def artifact(name, profile, output):
    return {
        "agent_id": f"{name}-agent-1",
        "dispatch": dispatch_for(name, profile),
        "status": "PASS",
        "evidence": f"{name} evidence",
        "summary": f"{name} completed",
        "output": output,
    }


def base_manifest():
    platform_targets = {
        platform: {
            "required": True,
            "target_project": target,
            "key_constraints": [f"Keep changes in {target}"],
            "files_allowed": [target],
            "waiver_reason": "",
        }
        for platform, target in TARGETS.items()
    }
    platforms = {}
    for platform, target in TARGETS.items():
        platforms[platform] = {
            "implementation": artifact(
                f"{platform}-implementation",
                "deep",
                {
                    "target_project": target,
                    "query_cases": "No duplicate case found.",
                    "upsert_case": "Platform case updated.",
                    "files_changed": [f"{target}AGENTS.md"],
                    "matrix_updates": [],
                },
            ),
            "verification": artifact(
                f"{platform}-verification",
                "review",
                {
                    "result": "PASS",
                    "findings": ["No blocking findings."],
                    "parity_result": "PASS",
                    "entry_point": "Basic > Join channel audio",
                    "ux_notes": "Matches adjacent examples.",
                    "commands": [
                        {
                            "kind": "build",
                            "command": BUILD_COMMANDS[platform],
                            "result": "PASS",
                            "evidence": (
                                f"dispatch-logs/{platform}-verification.jsonl#build; exit_code=0"
                            ),
                        }
                    ],
                    "build_result": "PASS",
                    "skipped_checks": [],
                },
            ),
        }
    return {
        "version": 4,
        "final_status": "PASS",
        "requirement": {
            "feature": "Join channel audio",
            "sdk_family": "Full RTC",
            "key_apis": ["joinChannel", "setAudioProfile"],
            "target_sdk_versions": {platform: "4.6.4" for platform in PLATFORMS},
        },
        "contract": artifact(
            "contract",
            "standard",
            {
                "scenario": "Keep the official audio-only join sample aligned on every platform.",
                "key_apis": ["joinChannel", "setAudioProfile"],
                "non_goals": ["Release packaging"],
                "reference": {
                    "required": True,
                    "source_case": "Android/APIExample/app/src/main/java/io/agora/api/example/examples/basic/JoinChannelAudio.java",
                    "contract_result": "PASS",
                },
                "cross_platform_requirements": ["Equivalent join, leave, mute, and status behavior"],
                "platform_targets": platform_targets,
            },
        ),
        "platforms": platforms,
        "cross_platform_acceptance": {
            "result": "PASS",
            "evidence": "All four required platform verification artifacts passed.",
            "differences": [],
        },
        "release": {
            "required": True,
            "target_sdk_versions": {platform: "4.6.4" for platform in PLATFORMS},
            "checks": [
                {
                    "name": f"sdk-version-{platform}",
                    "result": "PASS",
                    "expected_version": "4.6.4",
                    "actual_versions": {f"{platform}/version-source": "4.6.4"},
                    "evidence": f"{platform} SDK sources match 4.6.4.",
                    "reason": "",
                }
                for platform in PLATFORMS
            ],
            "skipped_checks": [],
        },
        "knowledge_updates": [],
    }


class AcceptanceManifestValidatorTest(unittest.TestCase):
    def assert_error_contains(self, manifest, expected):
        errors = validate_manifest(manifest)
        self.assertTrue(any(expected in error for error in errors), errors)

    def test_accepts_multi_platform_v4_manifest(self):
        self.assertEqual(validate_manifest(base_manifest()), [])

    def test_rejects_legacy_single_role_tree(self):
        manifest = base_manifest()
        manifest["roles"] = {}

        self.assert_error_contains(manifest, "unsupported top-level field: roles")

    def test_requires_all_official_platform_units(self):
        manifest = base_manifest()
        del manifest["platforms"]["windows"]

        self.assert_error_contains(manifest, "platforms missing required platform: windows")

    def test_required_platform_blocker_forces_global_blocked(self):
        manifest = base_manifest()
        manifest["platforms"]["windows"]["verification"]["status"] = "BLOCKED"

        self.assert_error_contains(
            manifest, "platforms.windows.verification.status=BLOCKED requires final_status=BLOCKED"
        )

    def test_required_role_cannot_be_waived(self):
        manifest = base_manifest()
        artifact = manifest["platforms"]["android"]["verification"]
        artifact["status"] = "WAIVED"
        artifact["waiver_reason"] = "No Android worker."

        self.assert_error_contains(
            manifest,
            "platforms.android.verification.status=WAIVED is only valid when the platform is not required",
        )

    def test_blocked_manifest_accepts_pending_windows_verification(self):
        manifest = base_manifest()
        manifest["final_status"] = "BLOCKED"
        manifest["cross_platform_acceptance"] = {
            "result": "BLOCKED",
            "evidence": "Windows-host verification is pending.",
            "differences": [],
        }
        verification = manifest["platforms"]["windows"]["verification"]
        verification.update(
            {
                "agent_id": "windows-verification-agent-pending",
                "dispatch": {
                    "mode": "pending",
                    "prompt": "role-prompts/windows-verification.md",
                    "artifact": "role-artifacts/windows-verification.json",
                },
                "status": "BLOCKED",
                "evidence": "Windows target build requires a Windows host.",
                "summary": "Windows verification pending.",
                "output": {
                    "result": "BLOCKED",
                    "findings": [],
                    "parity_result": "BLOCKED",
                    "entry_point": "Pending Windows verification.",
                    "ux_notes": "Pending.",
                    "commands": [],
                    "build_result": "BLOCKED",
                    "skipped_checks": [
                        {"name": "Windows MSBuild", "reason": "Current host is macOS."}
                    ],
                },
            }
        )

        self.assertEqual(validate_manifest(manifest), [])

    def test_non_required_platform_accepts_explicit_waiver(self):
        manifest = base_manifest()
        target = manifest["contract"]["output"]["platform_targets"]["windows"]
        target["required"] = False
        target["waiver_reason"] = "The requested SDK family is not available on Windows."
        for role in ["implementation", "verification"]:
            artifact = manifest["platforms"]["windows"][role]
            artifact["status"] = "WAIVED"
            artifact["waiver_reason"] = target["waiver_reason"]
            artifact["dispatch"] = {
                "mode": "pending",
                "prompt": f"role-prompts/windows-{role}.md",
                "artifact": f"role-artifacts/windows-{role}.json",
            }
        implementation = manifest["platforms"]["windows"]["implementation"]["output"]
        implementation["files_changed"] = []
        implementation["matrix_updates"] = []
        verification = manifest["platforms"]["windows"]["verification"]["output"]
        verification["result"] = "SKIPPED"
        verification["parity_result"] = "SKIPPED"
        verification["build_result"] = "SKIPPED"
        verification["commands"] = []
        verification["skipped_checks"] = []

        self.assertEqual(validate_manifest(manifest), [])

    def test_cross_platform_acceptance_must_pass_for_non_blocked_result(self):
        manifest = base_manifest()
        manifest["cross_platform_acceptance"]["result"] = "BLOCKED"

        self.assert_error_contains(manifest, "non-BLOCKED acceptance requires cross_platform_acceptance.result=PASS")

    def test_contract_and_implementation_target_must_match(self):
        manifest = base_manifest()
        manifest["platforms"]["ios"]["implementation"]["output"]["target_project"] = "iOS/APIExample-OC/"

        self.assert_error_contains(
            manifest, "platforms.ios.implementation.output.target_project must match contract target"
        )

    def test_contract_must_cover_requirement_key_apis(self):
        manifest = base_manifest()
        manifest["contract"]["output"]["key_apis"] = ["joinChannel"]

        self.assert_error_contains(
            manifest,
            "contract.output.key_apis must include every requirement.key_apis value: setAudioProfile",
        )

    def test_passing_contract_requires_cross_platform_constraints(self):
        manifest = base_manifest()
        manifest["contract"]["output"]["cross_platform_requirements"] = []

        self.assert_error_contains(
            manifest,
            "contract.output.cross_platform_requirements is required when Contract passes",
        )

    def test_implementation_requires_query_and_upsert_results(self):
        manifest = base_manifest()
        manifest["platforms"]["android"]["implementation"]["output"]["query_cases"] = ""

        self.assert_error_contains(
            manifest,
            "platforms.android.implementation.output.query_cases is required",
        )

    def test_required_verification_requires_entry_point_and_ux_notes(self):
        manifest = base_manifest()
        manifest["platforms"]["ios"]["verification"]["output"]["entry_point"] = ""

        self.assert_error_contains(
            manifest,
            "platforms.ios.verification.output.entry_point is required",
        )

    def test_duplicate_agent_ids_across_platforms_are_rejected(self):
        manifest = base_manifest()
        manifest["platforms"]["windows"]["verification"]["agent_id"] = manifest["platforms"]["ios"][
            "verification"
        ]["agent_id"]

        self.assert_error_contains(manifest, "duplicates platforms.ios.verification.agent_id")

    def test_duplicate_codex_run_ids_across_roles_are_rejected(self):
        manifest = base_manifest()
        implementation_run_id = manifest["platforms"]["ios"]["implementation"]["dispatch"][
            "run_id"
        ]
        manifest["platforms"]["ios"]["verification"]["dispatch"][
            "run_id"
        ] = implementation_run_id

        self.assert_error_contains(
            manifest,
            "platforms.ios.verification.dispatch.run_id duplicates platforms.ios.implementation.dispatch.run_id",
        )

    def test_pass_rejects_skipped_platform_checks(self):
        manifest = base_manifest()
        manifest["platforms"]["windows"]["verification"]["output"]["skipped_checks"] = [
            {"name": "runtime", "reason": "No device"}
        ]

        self.assert_error_contains(manifest, "final_status=PASS cannot include skipped checks")

    def test_done_matrix_update_requires_platform_build_pass(self):
        manifest = base_manifest()
        manifest["platforms"]["windows"]["implementation"]["output"]["matrix_updates"] = [
            {
                "feature": "Join channel audio",
                "platform_unit": "Windows",
                "from": "MISSING",
                "to": "DONE",
                "to_cell": "DONE(Basic/JoinChannelAudio/)",
                "evidence": "Implemented.",
            }
        ]
        manifest["platforms"]["windows"]["verification"]["output"]["build_result"] = "SKIPPED"

        self.assert_error_contains(manifest, "Windows matrix update to DONE requires build_result=PASS")

    def test_matrix_update_must_match_current_requirement(self):
        manifest = base_manifest()
        manifest["platforms"]["windows"]["implementation"]["output"]["matrix_updates"] = [
            {
                "feature": "Media metadata",
                "platform_unit": "Windows",
                "from": "MISSING",
                "to": "PARTIAL",
                "to_cell": "PARTIAL(pending)",
                "evidence": "Unrelated update.",
            }
        ]

        self.assert_error_contains(
            manifest,
            "platforms.windows.implementation.output.matrix_updates[0].feature must match requirement.feature",
        )

    def test_matrix_update_must_match_contract_target_project(self):
        manifest = base_manifest()
        manifest["platforms"]["windows"]["implementation"]["output"]["matrix_updates"] = [
            {
                "feature": "Join channel audio",
                "platform_unit": "Android full",
                "from": "MISSING",
                "to": "PARTIAL",
                "to_cell": "PARTIAL(wrong platform)",
                "evidence": "Wrong platform update.",
            }
        ]

        self.assert_error_contains(
            manifest,
            "platforms.windows.implementation.output.matrix_updates[0].platform_unit must match Contract target",
        )

    def test_matrix_update_cell_status_must_match_structured_status(self):
        manifest = base_manifest()
        manifest["final_status"] = "PASS WITH RISKS"
        manifest["platforms"]["android"]["verification"]["output"]["skipped_checks"] = [
            {"name": "device-smoke", "reason": "Device unavailable."}
        ]
        manifest["platforms"]["android"]["implementation"]["output"]["matrix_updates"] = [
            {
                "feature": "Join channel audio",
                "platform_unit": "Android full",
                "from": "MISSING",
                "to": "PARTIAL",
                "to_cell": "DONE(fake/path)",
                "evidence": "Runtime smoke is pending.",
            }
        ]

        self.assert_error_contains(
            manifest,
            "platforms.android.implementation.output.matrix_updates[0].to_cell status must match to=PARTIAL",
        )

    def test_release_cannot_be_disabled(self):
        manifest = base_manifest()
        manifest["release"]["required"] = False

        self.assert_error_contains(manifest, "release.required must be true")

    def test_rejects_removed_publication_fields(self):
        manifest = base_manifest()
        manifest["requirement"]["publication_channel"] = "legacy"
        manifest["release"]["publication"] = {}

        self.assert_error_contains(manifest, "unsupported requirement field: publication_channel")
        self.assert_error_contains(manifest, "unsupported release field: publication")

    def test_release_target_versions_must_match_requirement(self):
        manifest = base_manifest()
        manifest["release"]["target_sdk_versions"]["android"] = "4.6.3"

        self.assert_error_contains(
            manifest, "release.target_sdk_versions must match requirement.target_sdk_versions"
        )

    def test_target_sdk_versions_require_all_platform_semver_triplets(self):
        manifest = base_manifest()
        manifest["requirement"]["target_sdk_versions"]["android"] = "next"
        manifest["release"]["target_sdk_versions"]["android"] = "next"

        self.assert_error_contains(
            manifest,
            "requirement.target_sdk_versions.android must use x.y.z format",
        )

        manifest = base_manifest()
        manifest["requirement"]["target_sdk_versions"]["android"] = ""
        manifest["release"]["target_sdk_versions"]["android"] = ""
        self.assert_error_contains(
            manifest,
            "requirement.target_sdk_versions.android must use x.y.z format",
        )

        manifest = base_manifest()
        del manifest["requirement"]["target_sdk_versions"]["windows"]
        del manifest["release"]["target_sdk_versions"]["windows"]
        self.assert_error_contains(
            manifest,
            "requirement.target_sdk_versions must define exactly android, ios, macos, windows",
        )

        manifest = base_manifest()
        manifest["requirement"]["target_sdk_versions"]["linux"] = "4.6.4"
        manifest["release"]["target_sdk_versions"]["linux"] = "4.6.4"
        self.assert_error_contains(
            manifest,
            "requirement.target_sdk_versions must define exactly android, ios, macos, windows",
        )

    def test_release_sdk_check_requires_all_sources_to_match_target(self):
        manifest = base_manifest()
        check = manifest["release"]["checks"][0]
        check["actual_versions"]["android/version-source"] = "4.6.3"

        self.assert_error_contains(manifest, "sdk-version-android actual versions must all match 4.6.4")

    def test_rejects_external_ci_and_qa_metadata(self):
        manifest = base_manifest()
        manifest["release"]["qa_acceptance"] = {}

        self.assert_error_contains(manifest, "unsupported release field: qa_acceptance")

    def test_non_blocked_release_rejects_skipped_checks(self):
        manifest = base_manifest()
        manifest["final_status"] = "PASS WITH RISKS"
        manifest["release"]["skipped_checks"] = [
            {"name": "qa-device-smoke", "reason": "Device unavailable"}
        ]

        self.assert_error_contains(
            manifest,
            "non-BLOCKED acceptance cannot include skipped release checks",
        )

    def test_build_pass_requires_bound_command_evidence(self):
        manifest = base_manifest()
        del manifest["platforms"]["android"]["verification"]["output"]["commands"][0][
            "evidence"
        ]

        self.assert_error_contains(
            manifest,
            "platforms.android.verification.output.commands[0].evidence is required",
        )

    def test_successful_non_build_command_cannot_satisfy_build_result(self):
        manifest = base_manifest()
        command = manifest["platforms"]["android"]["verification"]["output"]["commands"][0]
        command["command"] = "true"

        self.assert_error_contains(
            manifest,
            "platforms.android.verification.output.commands[0] is not a recognized android build command",
        )

    def test_build_tool_name_in_echo_cannot_satisfy_build_result(self):
        manifest = base_manifest()
        command = manifest["platforms"]["android"]["verification"]["output"]["commands"][0]
        command["command"] = "echo ./gradlew assembleDebug"

        self.assert_error_contains(
            manifest,
            "platforms.android.verification.output.commands[0] is not a recognized android build command",
        )

    def test_non_build_tool_modes_cannot_satisfy_build_result(self):
        for platform, command_text in [
            ("android", "./gradlew --dry-run assembleDebug"),
            ("android", "./gradlew --dry-run=true assembleDebug"),
            ("android", "./gradlew help --task assembleDebug"),
            ("ios", "xcodebuild -version"),
            ("ios", "xcodebuild -scheme APIExample -showTestPlans"),
            ("ios", "xcodebuild -scheme APIExample -resolvePackageDependencies"),
            ("ios", "xcodebuild -scheme APIExample -checkFirstLaunchStatus"),
            ("macos", "xcodebuild -showsdks"),
            ("windows", "msbuild -version"),
            ("windows", "msbuild APIExample.sln /t:Clean"),
            ("windows", "msbuild APIExample.sln /preprocess:out.xml"),
            ("windows", "msbuild APIExample.sln /targets"),
            ("windows", "msbuild APIExample.sln /validate"),
            ("windows", "cmake --build build --target help"),
            ("android", "../fake/gradlew assembleDebug"),
            ("ios", "/tmp/xcodebuild -scheme APIExample build"),
            ("windows", "PATH=./fake msbuild APIExample.sln /t:Build"),
        ]:
            with self.subTest(platform=platform):
                manifest = base_manifest()
                command = manifest["platforms"][platform]["verification"]["output"]["commands"][0]
                command["command"] = command_text

                self.assert_error_contains(
                    manifest,
                    f"platforms.{platform}.verification.output.commands[0] is not a recognized {platform} build command",
                )

    def test_build_command_cannot_escape_contract_working_directory(self):
        manifest = base_manifest()
        command = manifest["platforms"]["ios"]["verification"]["output"]["commands"][0]
        command["command"] = (
            "xcodebuild -project ../APIExample-Audio/APIExample.xcodeproj "
            "-scheme APIExample build"
        )

        self.assert_error_contains(
            manifest,
            "platforms.ios.verification.output.commands[0] is not a recognized ios build command",
        )

    def test_xcode_default_build_command_from_project_rules_is_accepted(self):
        manifest = base_manifest()
        command = manifest["platforms"]["macos"]["verification"]["output"]["commands"][0]
        command["command"] = (
            "xcodebuild -workspace APIExample.xcworkspace -scheme APIExample "
            "-configuration Release"
        )

        self.assertEqual(validate_manifest(manifest), [])

    def test_windows_build_pass_requires_windows_host_provenance(self):
        manifest = base_manifest()
        manifest["platforms"]["windows"]["verification"]["dispatch"]["host_platform"] = "darwin"

        self.assert_error_contains(
            manifest,
            "platforms.windows.verification.output.build_result=PASS requires host_platform=win32",
        )

    def test_rejects_platform_target_outside_its_platform_root(self):
        manifest = base_manifest()
        target = manifest["contract"]["output"]["platform_targets"]["android"]
        target["target_project"] = "iOS/APIExample/"
        target["files_allowed"] = ["iOS/APIExample/"]

        self.assert_error_contains(
            manifest,
            "contract.output.platform_targets.android.target_project must be inside Android/",
        )

    def test_rejects_platform_target_that_escapes_root_with_parent_segments(self):
        manifest = base_manifest()
        target = manifest["contract"]["output"]["platform_targets"]["android"]
        target["target_project"] = "Android/../iOS/APIExample/"
        target["files_allowed"] = ["Android/../iOS/APIExample/"]

        self.assert_error_contains(
            manifest,
            "contract.output.platform_targets.android.target_project must be inside Android/",
        )

    def test_rejects_dispatch_path_outside_workspace(self):
        manifest = base_manifest()
        manifest["platforms"]["android"]["implementation"]["dispatch"]["prompt"] = "../prompt.md"

        self.assert_error_contains(manifest, "must be relative to the execution workspace")

    def test_files_changed_accepts_deleted_path_inside_contract_scope(self):
        manifest = base_manifest()
        manifest["platforms"]["android"]["implementation"]["output"]["files_changed"] = [
            "Android/APIExample/deleted-case.java"
        ]

        self.assertEqual(validate_manifest(manifest), [])

    def test_knowledge_update_requires_a_durable_file_change(self):
        manifest = base_manifest()
        for platform in PLATFORMS:
            manifest["platforms"][platform]["implementation"]["output"]["files_changed"] = ["AGENTS.md"]
        manifest["knowledge_updates"] = [
            {
                "source": "pilot",
                "impact_platforms": PLATFORMS,
                "symptom": "Build blocked",
                "root_cause": "Host mismatch",
                "guardrail": "Use target CI",
                "verification": "CI build",
                "updated_at": "2026-07-10",
            }
        ]

        self.assert_error_contains(manifest, "knowledge_updates require at least one durable knowledge")

    def test_durable_knowledge_accepts_both_skill_directory_conventions(self):
        self.assertTrue(is_durable_knowledge_path(".agent/skills/review-case/SKILL.md"))
        self.assertTrue(
            is_durable_knowledge_path(
                "Android/APIExample/.agents/skills/review-case/SKILL.md"
            )
        )

    def test_evidence_files_are_rehashed_from_manifest_directory(self):
        manifest = base_manifest()
        with tempfile.TemporaryDirectory() as tmpdir:
            root = Path(tmpdir)
            artifacts = [manifest["contract"]]
            for platform in PLATFORMS:
                artifacts.extend(manifest["platforms"][platform].values())
            for index, item in enumerate(artifacts):
                dispatch = item["dispatch"]
                for path_field, hash_field in [
                    ("prompt", "prompt_sha256"),
                    ("artifact", "artifact_sha256"),
                    ("input_snapshot", "input_snapshot_sha256"),
                    ("command_log", "command_log_sha256"),
                    ("repository_delta", "repository_delta_sha256"),
                ]:
                    if path_field not in dispatch:
                        continue
                    path = root / dispatch[path_field]
                    path.parent.mkdir(parents=True, exist_ok=True)
                    if path_field == "repository_delta":
                        content = json.dumps(
                            {"changed_files": item["output"]["files_changed"]}
                        ).encode()
                    else:
                        content = f"{index}:{path_field}\n".encode()
                    path.write_bytes(content)
                    dispatch[hash_field] = hashlib.sha256(content).hexdigest()

            self.assertEqual(validate_evidence_files(manifest, root), [])
            prompt = root / manifest["contract"]["dispatch"]["prompt"]
            prompt.write_text("tampered\n", encoding="utf-8")

            errors = validate_evidence_files(manifest, root)
            self.assertTrue(any("contract.dispatch.prompt_sha256 does not match" in error for error in errors))

    def test_evidence_validation_rejects_delta_content_mismatch(self):
        manifest = base_manifest()
        implementation = manifest["platforms"]["android"]["implementation"]
        with tempfile.TemporaryDirectory() as tmpdir:
            root = Path(tmpdir)
            delta_path = root / implementation["dispatch"]["repository_delta"]
            delta_path.parent.mkdir(parents=True)
            delta_path.write_text(
                json.dumps({"changed_files": ["Android/APIExample/other.java"]}),
                encoding="utf-8",
            )
            implementation["dispatch"]["repository_delta_sha256"] = hashlib.sha256(
                delta_path.read_bytes()
            ).hexdigest()

            errors = validate_evidence_files(manifest, root)

            self.assertTrue(
                any("repository_delta changed_files do not match" in error for error in errors),
                errors,
            )


if __name__ == "__main__":
    unittest.main()
