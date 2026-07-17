import json
import subprocess
import sys
import tempfile
import textwrap
import time
import unittest
from pathlib import Path
from unittest import mock


REPO_ROOT = Path(__file__).resolve().parents[3]
ORCHESTRATOR = REPO_ROOT / "docs/ai-engineering/tools/orchestrate_case_execution.py"
VALIDATOR = REPO_ROOT / "docs/ai-engineering/tools/validate_acceptance_manifest.py"
PLATFORMS = ["android", "ios", "macos", "windows"]
TOOLS_DIR = REPO_ROOT / "docs/ai-engineering/tools"
if str(TOOLS_DIR) not in sys.path:
    sys.path.insert(0, str(TOOLS_DIR))

import orchestrate_case_execution as orchestrator
from orchestrate_case_execution import collect_repository_fingerprints, run_codex_role


class RequirementOrchestratorTest(unittest.TestCase):
    TARGET_SDK_VERSION = "4.6.4"

    def write_matrix(self):
        handle = tempfile.NamedTemporaryFile("w", suffix=".md", delete=False)
        handle.write(
            textwrap.dedent(
                """
                | Feature | SDK Family | Key APIs | Android full | iOS UIKit | macOS | Windows | Notes |
                | --- | --- | --- | --- | --- | --- | --- | --- |
                | Join channel audio | Full RTC | `joinChannel`, `setAudioProfile` | `DONE(app/JoinChannelAudio.java)` | `DONE(APIExample/JoinChannelAudio.swift)` | `PARTIAL(APIExample/JoinChannelAudio.swift)` | `MISSING` | Keep official platforms aligned. |

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

    def run_orchestrator(self, *args):
        return subprocess.run(
            [sys.executable, str(ORCHESTRATOR), *args],
            cwd=REPO_ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
        )

    def init_workspace(self, matrix_path, run_dir):
        result = self.run_orchestrator(
            "init",
            "--matrix",
            str(matrix_path),
            "--feature",
            "Join channel audio",
            "--target-sdk-version",
            self.TARGET_SDK_VERSION,
            "--run-dir",
            str(run_dir),
        )
        self.assertEqual(result.returncode, 0, result.stderr)

    def write_fake_codex(
        self, directory, implementation_sleep=0, fail_implementation_platform=None
    ):
        path = Path(directory) / "fake-codex"
        path.write_text(
            textwrap.dedent(
                f"""\
                #!/usr/bin/env python3
                import json
                import sys
                import time
                from pathlib import Path

                if sys.argv[1:] == ["--version"]:
                    print("codex-cli 0.test")
                    raise SystemExit(0)

                args = sys.argv[1:]
                output_path = Path(args[args.index("--output-last-message") + 1])
                name = output_path.stem
                fail_name = {json.dumps(f'{fail_implementation_platform}-implementation' if fail_implementation_platform else '')}
                if name == fail_name:
                    print(json.dumps({{"type": "thread.started", "thread_id": f"run-{{name}}"}}), flush=True)
                    raise SystemExit(7)
                targets = {{
                    "android": "Android/APIExample/",
                    "ios": "iOS/APIExample/",
                    "macos": "macOS/",
                    "windows": "windows/",
                }}
                build_commands = {{
                    "android": "./gradlew assembleDebug",
                    "ios": "xcodebuild -scheme APIExample build",
                    "macos": "xcodebuild -scheme APIExample build",
                    "windows": "msbuild APIExample.sln /t:Build",
                }}
                status = "PASS"
                if name == "contract":
                    output = {{
                        "scenario": "Keep Join channel audio aligned across official platforms.",
                        "key_apis": ["joinChannel", "setAudioProfile"],
                        "non_goals": ["Release packaging"],
                        "reference": {{
                            "required": True,
                            "source_case": "Android/APIExample/app/src/main/java/io/agora/api/example/examples/basic/JoinChannelAudio.java",
                            "contract_result": "PASS",
                        }},
                        "cross_platform_requirements": ["Equivalent join and leave behavior"],
                        "platform_targets": {{
                            platform: {{
                                "required": True,
                                "target_project": target,
                                "key_constraints": [f"Keep changes in {{target}}"],
                                "files_allowed": [target],
                                "waiver_reason": "",
                            }}
                            for platform, target in targets.items()
                        }},
                    }}
                else:
                    platform, role = name.rsplit("-", 1)
                    if role == "implementation":
                        time.sleep({implementation_sleep})
                        output = {{
                            "target_project": targets[platform],
                            "query_cases": "No duplicate case found.",
                            "upsert_case": "Platform case updated.",
                            "files_changed": [f"{{targets[platform]}}AGENTS.md"],
                            "matrix_updates": [],
                        }}
                    else:
                        if platform == "windows" and sys.platform != "win32":
                            status = "BLOCKED"
                            output = {{
                                "result": "BLOCKED",
                                "findings": ["Windows build requires a Windows host."],
                                "parity_result": "PASS",
                                "entry_point": "Basic > Join channel audio",
                                "ux_notes": "Static review only on this host.",
                                "commands": [],
                                "build_result": "BLOCKED",
                                "skipped_checks": [{{
                                    "name": "Windows MSBuild",
                                    "reason": "Current host is not Windows.",
                                }}],
                            }}
                        else:
                            output = {{
                                "result": "PASS",
                                "findings": ["No blocking findings."],
                                "parity_result": "PASS",
                                "entry_point": "Basic > Join channel audio",
                                "ux_notes": "Matches adjacent examples.",
                                "commands": [{{
                                    "kind": "build",
                                    "command": build_commands[platform],
                                    "result": "PASS",
                                }}],
                                "build_result": "PASS",
                                "skipped_checks": [],
                            }}
                result = {{
                    "status": status,
                    "evidence": f"{{name}} evidence",
                    "summary": f"{{name}} completed",
                    "output": output,
                }}
                output_path.parent.mkdir(parents=True, exist_ok=True)
                output_path.write_text(json.dumps(result), encoding="utf-8")
                print(json.dumps({{"type": "thread.started", "thread_id": f"run-{{name}}"}}), flush=True)
                if name != "contract" and name.endswith("-verification") and output["commands"]:
                    print(json.dumps({{
                        "type": "item.completed",
                        "item": {{
                            "id": f"build-{{platform}}",
                            "type": "command_execution",
                            "command": build_commands[platform],
                            "status": "completed",
                            "exit_code": 0,
                        }},
                    }}), flush=True)
                """
            ),
            encoding="utf-8",
        )
        path.chmod(0o755)
        return path

    def dispatch_contract(self, run_dir, codex_bin):
        result = self.run_orchestrator(
            "dispatch",
            "--run-dir",
            str(run_dir),
            "--phase",
            "contract",
            "--model",
            "test-model",
            "--codex-bin",
            str(codex_bin),
        )
        self.assertEqual(result.returncode, 0, result.stderr + result.stdout)

    def test_init_creates_shared_contract_and_eight_platform_role_tasks(self):
        matrix_path = self.write_matrix()
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            self.init_workspace(matrix_path, run_dir)

            manifest = json.loads((run_dir / "acceptance-manifest.json").read_text())
            package = json.loads((run_dir / "execution-package.json").read_text())
            artifact_names = sorted(path.stem for path in (run_dir / "role-artifacts").glob("*.json"))
            expected = ["contract"] + [
                f"{platform}-{role}"
                for platform in PLATFORMS
                for role in ["implementation", "verification"]
            ]
            self.assertEqual(artifact_names, sorted(expected))
            self.assertEqual(sorted(manifest["platforms"]), PLATFORMS)
            self.assertNotIn("roles", manifest)
            self.assertEqual(
                package["repository_profile"],
                "docs/ai-engineering/repository-profile.json",
            )
            self.assertEqual(len(package["repository_profile_sha256"]), 64)

    def test_execution_configuration_rejects_repository_profile_drift(self):
        routing_path = REPO_ROOT / "docs/ai-engineering/role-routing.json"
        package = {
            "routing_config": "docs/ai-engineering/role-routing.json",
            "routing_config_sha256": orchestrator.sha256_file(routing_path),
            "repository_profile": "docs/ai-engineering/repository-profile.json",
            "repository_profile_sha256": "0" * 64,
        }

        with self.assertRaisesRegex(ValueError, "repository profile content changed"):
            orchestrator.validate_execution_configuration(package, routing_path)

    def test_contract_input_snapshot_binds_repository_profile(self):
        matrix_path = self.write_matrix()
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            self.init_workspace(matrix_path, run_dir)
            codex_bin = self.write_fake_codex(tmpdir)
            self.dispatch_contract(run_dir, codex_bin)

            package = json.loads((run_dir / "execution-package.json").read_text())
            artifact = json.loads((run_dir / "role-artifacts/contract.json").read_text())
            snapshot_path = run_dir / artifact["dispatch"]["input_snapshot"]
            snapshot = json.loads(snapshot_path.read_text())

            self.assertEqual(snapshot["repository_profile"], package["repository_profile"])
            self.assertEqual(
                snapshot["repository_profile_sha256"],
                package["repository_profile_sha256"],
            )

    def test_init_accepts_new_requirement_outside_the_matrix_backlog(self):
        matrix_path = self.write_matrix()
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            result = self.run_orchestrator(
                "init",
                "--matrix",
                str(matrix_path),
                "--feature",
                "Spatial audio",
                "--sdk-family",
                "Full RTC",
                "--key-api",
                "enableSpatialAudio",
                "--target-sdk-version",
                self.TARGET_SDK_VERSION,
                "--run-dir",
                str(run_dir),
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            package = json.loads((run_dir / "execution-package.json").read_text())
            self.assertEqual(package["requirement"]["feature"], "Spatial audio")
            self.assertEqual(package["requirement"]["key_apis"], ["enableSpatialAudio"])
            contract_prompt = (run_dir / "role-prompts/contract.md").read_text()
            self.assertNotIn("If Contract marks this platform required=false", contract_prompt)

    def test_repository_matrix_identity_is_portable(self):
        identity = orchestrator.stable_matrix_path(
            REPO_ROOT / "docs/ai-engineering/case-maintenance-matrix.md"
        )

        self.assertEqual(identity, "docs/ai-engineering/case-maintenance-matrix.md")

    def test_platform_implementation_waits_for_shared_contract(self):
        matrix_path = self.write_matrix()
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            self.init_workspace(matrix_path, run_dir)

            result = self.run_orchestrator(
                "dispatch",
                "--run-dir",
                str(run_dir),
                "--phase",
                "implementation",
                "--platform",
                "android",
                "--model",
                "test-model",
                "--dry-run",
            )

            self.assertEqual(result.returncode, 1)
            self.assertIn("Contract gate returned status BLOCKED", result.stderr)

    def test_contract_semantic_failure_is_blocked_before_implementation(self):
        matrix_path = self.write_matrix()
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            self.init_workspace(matrix_path, run_dir)
            artifact_path = run_dir / "role-artifacts/contract.json"
            artifact = json.loads(artifact_path.read_text())
            artifact["status"] = "PASS"
            artifact["output"]["reference"]["contract_result"] = "PASS"
            artifact["output"]["cross_platform_requirements"] = []
            artifact_path.write_text(json.dumps(artifact), encoding="utf-8")

            with self.assertRaisesRegex(ValueError, "Contract semantic gate failed"):
                orchestrator.validate_completed_contract(run_dir)

            blocked = json.loads(artifact_path.read_text())
            self.assertEqual(blocked["status"], "BLOCKED")
            self.assertIn("cross_platform_requirements", blocked["evidence"])

    def test_implementation_phase_dispatches_four_platform_agents_with_scoped_deltas(self):
        matrix_path = self.write_matrix()
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            self.init_workspace(matrix_path, run_dir)
            codex_bin = self.write_fake_codex(tmpdir)
            self.dispatch_contract(run_dir, codex_bin)

            result = self.run_orchestrator(
                "dispatch",
                "--run-dir",
                str(run_dir),
                "--phase",
                "implementation",
                "--model",
                "test-model",
                "--codex-bin",
                str(codex_bin),
            )

            self.assertEqual(result.returncode, 0, result.stderr + result.stdout)
            for platform in PLATFORMS:
                artifact = json.loads(
                    (run_dir / f"role-artifacts/{platform}-implementation.json").read_text()
                )
                self.assertEqual(artifact["dispatch"]["mode"], "codex-exec")
                self.assertEqual(artifact["dispatch"]["run_id"], f"run-{platform}-implementation")
                self.assertEqual(artifact["output"]["files_changed"], [])

    def test_implementation_reconciles_each_agent_before_starting_the_next(self):
        tasks = [
            {"name": "android-implementation", "platform": "android"},
            {"name": "ios-implementation", "platform": "ios"},
        ]
        events = []

        with mock.patch.object(
            orchestrator,
            "refresh_implementation_input_snapshot",
            side_effect=lambda run_dir, task: events.append(f"snapshot:{task['name']}"),
        ), mock.patch.object(
            orchestrator,
            "collect_repository_fingerprints",
            side_effect=[{"state": "android"}, {"state": "ios"}],
        ), mock.patch.object(
            orchestrator,
            "run_codex_role",
            side_effect=lambda **kwargs: events.append(f"run:{kwargs['name']}"),
        ), mock.patch.object(
            orchestrator,
            "reconcile_implementation_changes",
            side_effect=lambda run_dir, task_group, before: events.append(
                f"reconcile:{task_group[0]['name']}"
            ),
        ), mock.patch.object(
            orchestrator,
            "validate_completed_implementation",
            side_effect=lambda run_dir, platform: events.append(f"validate:{platform}"),
        ):
            orchestrator.run_implementation_tasks(Path("/tmp/run"), tasks)

        self.assertEqual(
            events,
            [
                "snapshot:android-implementation",
                "run:android-implementation",
                "reconcile:android-implementation",
                "validate:android",
                "snapshot:ios-implementation",
                "run:ios-implementation",
                "reconcile:ios-implementation",
                "validate:ios",
            ],
        )

    def test_implementation_semantic_failure_blocks_verification(self):
        matrix_path = self.write_matrix()
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            self.init_workspace(matrix_path, run_dir)
            codex_bin = self.write_fake_codex(tmpdir)
            self.dispatch_contract(run_dir, codex_bin)
            artifact_path = run_dir / "role-artifacts/android-implementation.json"
            artifact = json.loads(artifact_path.read_text())
            artifact["status"] = "PASS"
            artifact["output"]["query_cases"] = ""
            artifact_path.write_text(json.dumps(artifact), encoding="utf-8")

            with self.assertRaisesRegex(ValueError, "Implementation semantic gate failed"):
                orchestrator.validate_completed_implementation(run_dir, "android")

            blocked = json.loads(artifact_path.read_text())
            self.assertEqual(blocked["status"], "BLOCKED")
            self.assertIn("query_cases", blocked["evidence"])

    def test_implementation_refreshes_snapshot_after_previous_platform_delta(self):
        task = {
            "name": "ios-implementation",
            "snapshot_dependencies": {"contract": {}},
            "resolved_routing": {"routing_config": "routing.json"},
            "input_snapshot": "old.json",
            "input_snapshot_sha256": "old-sha",
            "expected_repository_state": {"head": "old"},
        }
        snapshot_path = Path("/tmp/run/input-snapshots/ios-new.json")

        with mock.patch.object(
            orchestrator,
            "write_input_snapshot",
            return_value=(snapshot_path, "new-sha"),
        ), mock.patch.object(
            orchestrator,
            "read_json",
            return_value={"repository_state": {"head": "after-android"}},
        ):
            orchestrator.refresh_implementation_input_snapshot(Path("/tmp/run"), task)

        self.assertEqual(task["input_snapshot"], "input-snapshots/ios-new.json")
        self.assertEqual(task["input_snapshot_sha256"], "new-sha")
        self.assertEqual(task["expected_repository_state"], {"head": "after-android"})

    def test_repository_fingerprints_include_untracked_file_contents(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            root = Path(tmpdir)
            subprocess.run(["git", "init", "-q"], cwd=root, check=True)
            path = root / "new-example.swift"
            path.write_text("first\n", encoding="utf-8")
            before = collect_repository_fingerprints(root)

            path.write_text("second\n", encoding="utf-8")
            after = collect_repository_fingerprints(root)

            self.assertNotEqual(before["new-example.swift"], after["new-example.swift"])

    def test_artifact_loading_rejects_stale_dependency_snapshot(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            artifact_dir = run_dir / "role-artifacts"
            artifact_dir.mkdir(parents=True)
            package_path = run_dir / "execution-package.json"
            package_path.write_text('{"requirement":{}}\n', encoding="utf-8")
            contract_path = artifact_dir / "contract.json"
            contract_path.write_text('{"status":"PASS"}\n', encoding="utf-8")
            snapshot_path = run_dir / "input-snapshots/android-verification.json"
            snapshot_path.parent.mkdir(parents=True)
            snapshot_path.write_text(
                json.dumps(
                    {
                        "execution_package_sha256": orchestrator.sha256_file(package_path),
                        "dependency_artifact_sha256": {"contract": "0" * 64},
                    }
                ),
                encoding="utf-8",
            )
            artifact_path = artifact_dir / "android-verification.json"
            artifact = {
                "dispatch": {
                    "mode": "codex-exec",
                    "input_snapshot": "input-snapshots/android-verification.json",
                    "input_snapshot_sha256": orchestrator.sha256_file(snapshot_path),
                }
            }
            artifact_path.write_text(json.dumps(artifact), encoding="utf-8")

            with self.assertRaisesRegex(ValueError, "dependency artifact sha256 mismatch"):
                orchestrator.attach_dispatch_hashes(
                    artifact,
                    "android-verification",
                    artifact_path,
                    run_dir,
                )

    def test_implementation_retry_delta_covers_all_attempt_changes(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            contract_path = run_dir / "role-artifacts/contract.json"
            contract_path.parent.mkdir(parents=True)
            contract_path.write_text(
                json.dumps(
                    {
                        "output": {
                            "platform_targets": {
                                "android": {"files_allowed": ["Android/APIExample/"]}
                            }
                        }
                    }
                ),
                encoding="utf-8",
            )
            artifact_path = run_dir / "role-artifacts/android-implementation.json"
            artifact = {
                "status": "BLOCKED",
                "dispatch": {
                    "mode": "codex-exec",
                    "run_id": "run-1",
                    "input_snapshot_sha256": "snapshot-1",
                },
                "output": {"files_changed": []},
            }
            artifact_path.write_text(json.dumps(artifact), encoding="utf-8")
            first_before = {"Android/APIExample/A.java": {"sha256": "before-a"}}
            first_after = {"Android/APIExample/A.java": {"sha256": "after-a"}}
            first_task = {
                "name": "android-implementation",
                "platform": "android",
                "input_snapshot": "input-snapshots/android-1.json",
                "input_snapshot_sha256": "snapshot-1",
                "previous_files_changed": [],
            }
            with mock.patch.object(
                orchestrator, "collect_repository_fingerprints", return_value=first_after
            ):
                orchestrator.reconcile_implementation_changes(
                    run_dir, [first_task], first_before
                )

            artifact = json.loads(artifact_path.read_text())
            artifact["status"] = "PASS"
            artifact["dispatch"]["run_id"] = "run-2"
            artifact["dispatch"]["input_snapshot_sha256"] = "snapshot-2"
            artifact_path.write_text(json.dumps(artifact), encoding="utf-8")
            second_after = {
                **first_after,
                "Android/APIExample/B.java": {"sha256": "after-b"},
            }
            second_task = {
                "name": "android-implementation",
                "platform": "android",
                "input_snapshot": "input-snapshots/android-2.json",
                "input_snapshot_sha256": "snapshot-2",
                "previous_files_changed": ["Android/APIExample/A.java"],
            }
            with mock.patch.object(
                orchestrator, "collect_repository_fingerprints", return_value=second_after
            ):
                orchestrator.reconcile_implementation_changes(
                    run_dir, [second_task], first_after
                )

            artifact = json.loads(artifact_path.read_text())
            delta = json.loads(
                (run_dir / "repository-deltas/android-implementation.json").read_text()
            )
            expected = ["Android/APIExample/A.java", "Android/APIExample/B.java"]
            self.assertEqual(artifact["output"]["files_changed"], expected)
            self.assertEqual(delta["changed_files"], expected)
            self.assertEqual([attempt["run_id"] for attempt in delta["attempts"]], ["run-1", "run-2"])

    def test_failed_implementation_still_reconciles_successful_artifacts(self):
        matrix_path = self.write_matrix()
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            self.init_workspace(matrix_path, run_dir)
            codex_bin = self.write_fake_codex(
                tmpdir, fail_implementation_platform="windows"
            )
            self.dispatch_contract(run_dir, codex_bin)

            result = self.run_orchestrator(
                "dispatch",
                "--run-dir",
                str(run_dir),
                "--phase",
                "implementation",
                "--model",
                "test-model",
                "--codex-bin",
                str(codex_bin),
            )

            self.assertEqual(result.returncode, 1)
            android = json.loads(
                (run_dir / "role-artifacts/android-implementation.json").read_text()
            )
            self.assertEqual(android["output"]["files_changed"], [])
            self.assertIn("repository_delta", android["dispatch"])

    def test_failed_verification_mutation_creates_persistent_scope_violation(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            prompt_path = run_dir / "role-prompts/ios-verification.md"
            prompt_path.parent.mkdir(parents=True)
            prompt_path.write_text("verify", encoding="utf-8")
            artifact_path = run_dir / "role-artifacts/ios-verification.json"

            with mock.patch(
                "orchestrate_case_execution.repository_state",
                return_value={"state": "after"},
            ):
                with self.assertRaisesRegex(ValueError, "modified repository files"):
                    run_codex_role(
                        command=[sys.executable, "-c", "raise SystemExit(7)"],
                        name="ios-verification",
                        role="verification",
                        platform="ios",
                        prompt_path=prompt_path,
                        artifact_path=artifact_path,
                        raw_result_path=run_dir / "raw-results/ios-verification.json",
                        log_path=run_dir / "dispatch-logs/ios-verification.jsonl",
                        stderr_path=run_dir / "dispatch-logs/ios-verification.stderr.log",
                        model_profile="review",
                        model="test-model",
                        reasoning_effort="high",
                        codex_version="codex-cli test",
                        working_directory=Path(tmpdir),
                        working_directory_manifest="iOS/APIExample/",
                        input_snapshot="input-snapshots/ios-verification.json",
                        input_snapshot_sha256="a" * 64,
                        expected_repository_state={"state": "before"},
                        timeout_seconds=5,
                    )

            self.assertTrue(
                (run_dir / "repository-deltas/ios-verification-scope-violation.json").exists()
            )

    def test_macos_rejects_windows_build_pass(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            prompt_path = run_dir / "role-prompts/windows-verification.md"
            prompt_path.parent.mkdir(parents=True)
            prompt_path.write_text("verify", encoding="utf-8")
            raw_result_path = run_dir / "raw-results/windows-verification.json"
            raw_result_path.parent.mkdir(parents=True)
            raw_result_path.write_text(
                json.dumps(
                    {
                        "status": "PASS",
                        "evidence": "MSBuild passed.",
                        "summary": "Windows verified.",
                        "output": {
                            "result": "PASS",
                            "findings": [],
                            "parity_result": "PASS",
                            "entry_point": "Basic > Join channel audio",
                            "ux_notes": "Matches adjacent examples.",
                            "commands": [
                                {
                                    "kind": "build",
                                    "command": "msbuild APIExample.sln /t:Build",
                                    "result": "PASS",
                                }
                            ],
                            "build_result": "PASS",
                            "skipped_checks": [],
                        },
                    }
                ),
                encoding="utf-8",
            )
            command = [
                sys.executable,
                "-c",
                (
                    "import json; "
                    "print(json.dumps({'type':'thread.started','thread_id':'run-windows'})); "
                    "print(json.dumps({'type':'item.completed','item':"
                    "{'id':'build','type':'command_execution','command':"
                    "'msbuild APIExample.sln /t:Build','exit_code':0}}))"
                ),
            ]

            with mock.patch.object(orchestrator.sys, "platform", "darwin"), mock.patch.object(
                orchestrator,
                "repository_state",
                return_value={"state": "clean"},
            ):
                with self.assertRaisesRegex(ValueError, "requires a Windows host"):
                    run_codex_role(
                        command=command,
                        name="windows-verification",
                        role="verification",
                        platform="windows",
                        prompt_path=prompt_path,
                        artifact_path=run_dir / "role-artifacts/windows-verification.json",
                        raw_result_path=raw_result_path,
                        log_path=run_dir / "dispatch-logs/windows-verification.jsonl",
                        stderr_path=run_dir / "dispatch-logs/windows-verification.stderr.log",
                        model_profile="review",
                        model="test-model",
                        reasoning_effort="high",
                        codex_version="codex-cli test",
                        working_directory=Path(tmpdir),
                        working_directory_manifest="windows/",
                        input_snapshot="input-snapshots/windows-verification.json",
                        input_snapshot_sha256="a" * 64,
                        expected_repository_state={"state": "clean"},
                        timeout_seconds=5,
                    )

    def test_verification_semantic_failure_becomes_retryable(self):
        matrix_path = self.write_matrix()
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            self.init_workspace(matrix_path, run_dir)
            codex_bin = self.write_fake_codex(tmpdir)
            self.dispatch_contract(run_dir, codex_bin)
            artifact_path = run_dir / "role-artifacts/ios-verification.json"
            artifact = json.loads(artifact_path.read_text())
            artifact["status"] = "PASS"
            artifact["output"]["result"] = "BLOCKED"
            artifact_path.write_text(json.dumps(artifact), encoding="utf-8")

            with self.assertRaisesRegex(ValueError, "Verification semantic gate failed"):
                orchestrator.validate_completed_verification(run_dir, "ios")

            blocked = json.loads(artifact_path.read_text())
            self.assertEqual(blocked["status"], "BLOCKED")
            self.assertIn("output.result", blocked["evidence"])

    def test_platform_filter_dispatches_only_requested_agent(self):
        matrix_path = self.write_matrix()
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            self.init_workspace(matrix_path, run_dir)
            codex_bin = self.write_fake_codex(tmpdir)
            self.dispatch_contract(run_dir, codex_bin)

            result = self.run_orchestrator(
                "dispatch",
                "--run-dir",
                str(run_dir),
                "--phase",
                "implementation",
                "--platform",
                "windows",
                "--model",
                "test-model",
                "--dry-run",
            )

            self.assertEqual(result.returncode, 0, result.stderr + result.stdout)
            payload = json.loads(result.stdout)
            self.assertEqual([item["platform"] for item in payload["commands"]], ["windows"])
            command = payload["commands"][0]["command"]
            self.assertIn("-C", command)
            self.assertIn(str(REPO_ROOT / "windows"), command)

    def test_platform_prompt_uses_target_selected_by_completed_contract(self):
        matrix_path = self.write_matrix()
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            self.init_workspace(matrix_path, run_dir)
            codex_bin = self.write_fake_codex(tmpdir)
            self.dispatch_contract(run_dir, codex_bin)
            contract_path = run_dir / "role-artifacts/contract.json"
            contract = json.loads(contract_path.read_text())
            contract["output"]["platform_targets"]["ios"]["target_project"] = "iOS/APIExample-OC/"
            contract["output"]["platform_targets"]["ios"]["files_allowed"] = ["iOS/APIExample-OC/"]
            contract_path.write_text(json.dumps(contract), encoding="utf-8")

            result = self.run_orchestrator(
                "dispatch",
                "--run-dir",
                str(run_dir),
                "--phase",
                "implementation",
                "--platform",
                "ios",
                "--model",
                "test-model",
                "--dry-run",
            )

            self.assertEqual(result.returncode, 0, result.stderr + result.stdout)
            prompt = (run_dir / "role-prompts/ios-implementation.md").read_text()
            self.assertIn("Target: iOS/APIExample-OC/", prompt)
            self.assertIn("If Contract marks this platform required=false", prompt)

    def test_verification_uses_corresponding_platform_implementation(self):
        matrix_path = self.write_matrix()
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            self.init_workspace(matrix_path, run_dir)
            codex_bin = self.write_fake_codex(tmpdir)
            self.dispatch_contract(run_dir, codex_bin)
            implementation = self.run_orchestrator(
                "dispatch",
                "--run-dir",
                str(run_dir),
                "--phase",
                "implementation",
                "--model",
                "test-model",
                "--codex-bin",
                str(codex_bin),
            )
            self.assertEqual(implementation.returncode, 0, implementation.stderr)

            result = self.run_orchestrator(
                "dispatch",
                "--run-dir",
                str(run_dir),
                "--phase",
                "verification",
                "--platform",
                "windows",
                "--model",
                "test-model",
                "--dry-run",
            )

            self.assertEqual(result.returncode, 0, result.stderr + result.stdout)
            prompt = (run_dir / "role-prompts/windows-verification.md").read_text()
            self.assertIn("windows-implementation completed", prompt)
            self.assertIn("macOS cannot provide Windows build evidence", prompt)
            self.assertIn("Do not download Windows SDK archives", prompt)
            implementation_prompt = (run_dir / "role-prompts/windows-implementation.md").read_text()
            self.assertNotIn("cannot provide Windows build evidence", implementation_prompt)

    def test_platform_dispatch_timeout_keeps_other_platform_scope_isolated(self):
        matrix_path = self.write_matrix()
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            self.init_workspace(matrix_path, run_dir)
            codex_bin = self.write_fake_codex(tmpdir, implementation_sleep=2)
            self.dispatch_contract(run_dir, codex_bin)

            result = self.run_orchestrator(
                "dispatch",
                "--run-dir",
                str(run_dir),
                "--phase",
                "implementation",
                "--platform",
                "android",
                "--model",
                "test-model",
                "--codex-bin",
                str(codex_bin),
                "--timeout-seconds",
                "1",
            )

            self.assertEqual(result.returncode, 1)
            self.assertIn("android-implementation timed out after 1 seconds", result.stderr)
            self.assertTrue((run_dir / "dispatch-logs/android-implementation.jsonl").exists())
            self.assertFalse((run_dir / "dispatch-logs/ios-implementation.jsonl").exists())

    def test_timeout_terminates_agent_child_processes(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            prompt_path = run_dir / "role-prompts/android-implementation.md"
            prompt_path.parent.mkdir(parents=True)
            prompt_path.write_text("implement", encoding="utf-8")
            marker = Path(tmpdir) / "late-child-write.txt"
            child_code = (
                "import time; from pathlib import Path; "
                f"time.sleep(0.4); Path({str(marker)!r}).write_text('late')"
            )
            parent_code = (
                "import subprocess, sys, time; "
                f"subprocess.Popen([sys.executable, '-c', {child_code!r}]); "
                "time.sleep(10)"
            )

            with self.assertRaisesRegex(ValueError, "timed out"):
                run_codex_role(
                    command=[sys.executable, "-c", parent_code],
                    name="android-implementation",
                    role="implementation",
                    platform="android",
                    prompt_path=prompt_path,
                    artifact_path=run_dir / "role-artifacts/android-implementation.json",
                    raw_result_path=run_dir / "raw-results/android-implementation.json",
                    log_path=run_dir / "dispatch-logs/android-implementation.jsonl",
                    stderr_path=run_dir / "dispatch-logs/android-implementation.stderr.log",
                    model_profile="deep",
                    model="test-model",
                    reasoning_effort="high",
                    codex_version="codex-cli test",
                    working_directory=Path(tmpdir),
                    working_directory_manifest="Android/APIExample/",
                    input_snapshot="input-snapshots/android-implementation.json",
                    input_snapshot_sha256="a" * 64,
                    expected_repository_state={"state": "clean"},
                    timeout_seconds=0.1,
                )
            time.sleep(0.6)

            self.assertFalse(marker.exists())

    def test_full_multi_platform_run_assembles_blocked_v4_manifest(self):
        matrix_path = self.write_matrix()
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            self.init_workspace(matrix_path, run_dir)
            codex_bin = self.write_fake_codex(tmpdir)
            self.dispatch_contract(run_dir, codex_bin)
            for phase in ["implementation", "verification"]:
                result = self.run_orchestrator(
                    "dispatch",
                    "--run-dir",
                    str(run_dir),
                    "--phase",
                    phase,
                    "--model",
                    "test-model",
                    "--codex-bin",
                    str(codex_bin),
                )
                self.assertEqual(result.returncode, 0, result.stderr + result.stdout)

            result = self.run_orchestrator(
                "assemble",
                "--run-dir",
                str(run_dir),
                "--matrix",
                str(matrix_path),
                "--final-status",
                "BLOCKED",
                "--cross-platform-result",
                "BLOCKED",
                "--cross-platform-evidence",
                "Target SDK and Windows-host build evidence are pending.",
            )

            self.assertEqual(result.returncode, 0, result.stderr + result.stdout)
            final_path = run_dir / "final-acceptance-manifest.json"
            final_manifest = json.loads(final_path.read_text())
            self.assertEqual(final_manifest["version"], 4)
            self.assertEqual(final_manifest["cross_platform_acceptance"]["result"], "BLOCKED")
            self.assertEqual(sorted(final_manifest["platforms"]), PLATFORMS)
            self.assertTrue(final_manifest["release"]["required"])
            self.assertEqual(final_manifest["release"]["target_sdk_version"], "4.6.4")
            validation = subprocess.run(
                [sys.executable, str(VALIDATOR), str(final_path)],
                cwd=REPO_ROOT,
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
            )
            self.assertEqual(validation.returncode, 0, validation.stderr)

    def test_assemble_reports_only_repository_gates_without_external_metadata(self):
        matrix_path = self.write_matrix()
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            self.init_workspace(matrix_path, run_dir)
            codex_bin = self.write_fake_codex(tmpdir)
            self.dispatch_contract(run_dir, codex_bin)
            for phase in ["implementation", "verification"]:
                result = self.run_orchestrator(
                    "dispatch",
                    "--run-dir",
                    str(run_dir),
                    "--phase",
                    phase,
                    "--model",
                    "test-model",
                    "--codex-bin",
                    str(codex_bin),
                )
                self.assertEqual(result.returncode, 0, result.stderr + result.stdout)

            result = self.run_orchestrator(
                "assemble",
                "--run-dir",
                str(run_dir),
                "--matrix",
                str(matrix_path),
                "--final-status",
                "PASS",
                "--cross-platform-result",
                "PASS",
                "--cross-platform-evidence",
                "All required platforms passed independent verification.",
            )

            self.assertEqual(result.returncode, 1)
            self.assertIn("sdk-version-android", result.stderr)
            self.assertNotIn("qa_acceptance", result.stderr)

    def test_assemble_rejects_matrix_path_changed_since_init(self):
        matrix_path = self.write_matrix()
        other_matrix_path = self.write_matrix()
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            self.init_workspace(matrix_path, run_dir)

            result = self.run_orchestrator(
                "assemble",
                "--run-dir",
                str(run_dir),
                "--matrix",
                str(other_matrix_path),
            )

            self.assertEqual(result.returncode, 1)
            self.assertIn("matrix path changed since init", result.stderr)

    def test_assemble_rejects_requirement_changed_since_init(self):
        matrix_path = self.write_matrix()
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            self.init_workspace(matrix_path, run_dir)
            manifest_path = run_dir / "acceptance-manifest.json"
            manifest = json.loads(manifest_path.read_text())
            manifest["requirement"]["feature"] = "Different requirement"
            manifest_path.write_text(json.dumps(manifest), encoding="utf-8")

            result = self.run_orchestrator(
                "assemble",
                "--run-dir",
                str(run_dir),
                "--matrix",
                str(matrix_path),
            )

            self.assertEqual(result.returncode, 1)
            self.assertIn("requirement changed since init", result.stderr)

    def test_assemble_rejects_missing_completed_prompt_evidence(self):
        matrix_path = self.write_matrix()
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            self.init_workspace(matrix_path, run_dir)
            codex_bin = self.write_fake_codex(tmpdir)
            self.dispatch_contract(run_dir, codex_bin)
            (run_dir / "role-prompts/contract.md").unlink()

            result = self.run_orchestrator(
                "assemble",
                "--run-dir",
                str(run_dir),
                "--matrix",
                str(matrix_path),
            )

            self.assertEqual(result.returncode, 1)
            self.assertIn("contract.dispatch.prompt does not exist", result.stderr)

    def test_retrying_platform_implementation_invalidates_old_verification(self):
        matrix_path = self.write_matrix()
        with tempfile.TemporaryDirectory() as tmpdir:
            run_dir = Path(tmpdir) / "run"
            self.init_workspace(matrix_path, run_dir)
            codex_bin = self.write_fake_codex(tmpdir)
            self.dispatch_contract(run_dir, codex_bin)
            for phase in ["implementation", "verification"]:
                result = self.run_orchestrator(
                    "dispatch",
                    "--run-dir",
                    str(run_dir),
                    "--phase",
                    phase,
                    "--model",
                    "test-model",
                    "--codex-bin",
                    str(codex_bin),
                )
                self.assertEqual(result.returncode, 0, result.stderr + result.stdout)
            implementation_path = run_dir / "role-artifacts/windows-implementation.json"
            implementation = json.loads(implementation_path.read_text())
            implementation["status"] = "BLOCKED"
            implementation_path.write_text(json.dumps(implementation), encoding="utf-8")

            result = self.run_orchestrator(
                "dispatch",
                "--run-dir",
                str(run_dir),
                "--phase",
                "implementation",
                "--platform",
                "windows",
                "--retry",
                "--model",
                "test-model",
                "--codex-bin",
                str(codex_bin),
            )

            self.assertEqual(result.returncode, 0, result.stderr + result.stdout)
            verification = json.loads(
                (run_dir / "role-artifacts/windows-verification.json").read_text()
            )
            self.assertEqual(verification["status"], "BLOCKED")
            self.assertEqual(verification["dispatch"]["mode"], "pending")

    def test_removed_single_platform_phases_are_rejected(self):
        result = self.run_orchestrator(
            "dispatch",
            "--run-dir",
            "/tmp/not-used",
            "--phase",
            "target-test",
            "--model",
            "test-model",
            "--dry-run",
        )
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("invalid choice", result.stderr)


if __name__ == "__main__":
    unittest.main()
