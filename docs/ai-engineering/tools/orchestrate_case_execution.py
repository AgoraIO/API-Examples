#!/usr/bin/env python3
"""Run one cross-platform API Examples requirement workflow."""

import argparse
import concurrent.futures
import hashlib
import json
import os
import signal
import stat
import subprocess
import sys
from pathlib import Path

from generate_case_backlog import parse_matrix_cell, split_markdown_row
from prepare_case_execution import (
    DEFAULT_REPOSITORY_PROFILE,
    PLATFORMS,
    collect_sdk_version_checks,
    load_repository_profile,
    prepare_case_execution,
)
from validate_acceptance_manifest import (
    PLATFORM_ROOTS,
    is_platform_build_command,
    path_is_within,
    validate_contract,
    validate_evidence_files,
    validate_manifest,
    validate_platform_implementation,
    validate_platform_verification,
)


ROLE_ORDER = ["contract", "implementation", "verification"]
PHASE_ORDER = ["contract", "implementation", "verification"]
REPO_ROOT = Path(__file__).resolve().parents[3]
DEFAULT_ROUTING_CONFIG = REPO_ROOT / "docs/ai-engineering/role-routing.json"
COMPLETED_DEPENDENCY_STATUSES = {"PASS", "WAIVED"}
RESULT_VALUES = ["PASS", "FAIL", "BLOCKED", "SKIPPED"]
ROLE_STATUS_VALUES = ["PASS", "FAIL", "BLOCKED", "WAIVED"]

STRING = {"type": "string"}
STRING_LIST = {"type": "array", "items": STRING}
RESULT = {"type": "string", "enum": RESULT_VALUES}
REFERENCE = {
    "type": "object",
    "additionalProperties": False,
    "required": ["required", "source_case", "contract_result"],
    "properties": {
        "required": {"type": "boolean"},
        "source_case": STRING,
        "contract_result": RESULT,
    },
}
PLATFORM_TARGET = {
    "type": "object",
    "additionalProperties": False,
    "required": [
        "required",
        "target_project",
        "key_constraints",
        "files_allowed",
        "waiver_reason",
    ],
    "properties": {
        "required": {"type": "boolean"},
        "target_project": STRING,
        "key_constraints": STRING_LIST,
        "files_allowed": STRING_LIST,
        "waiver_reason": STRING,
    },
}
PLATFORM_TARGETS = {
    "type": "object",
    "additionalProperties": False,
    "required": PLATFORMS,
    "properties": {platform: PLATFORM_TARGET for platform in PLATFORMS},
}
MATRIX_UPDATE = {
    "type": "object",
    "additionalProperties": False,
    "required": ["feature", "platform_unit", "from", "to", "to_cell", "evidence"],
    "properties": {
        "feature": STRING,
        "platform_unit": STRING,
        "from": STRING,
        "to": STRING,
        "to_cell": STRING,
        "evidence": STRING,
    },
}
COMMAND_LIST = {
    "type": "array",
    "items": {
        "type": "object",
        "additionalProperties": False,
        "required": ["kind", "command", "result"],
        "properties": {
            "kind": {"type": "string", "enum": ["build", "test", "static", "runtime"]},
            "command": STRING,
            "result": RESULT,
        },
    },
}
SKIPPED_CHECK_LIST = {
    "type": "array",
    "items": {
        "type": "object",
        "additionalProperties": False,
        "required": ["name", "reason"],
        "properties": {"name": STRING, "reason": STRING},
    },
}
ROLE_OUTPUT_PROPERTIES = {
    "contract": {
        "scenario": STRING,
        "key_apis": STRING_LIST,
        "non_goals": STRING_LIST,
        "reference": REFERENCE,
        "cross_platform_requirements": STRING_LIST,
        "platform_targets": PLATFORM_TARGETS,
    },
    "implementation": {
        "target_project": STRING,
        "query_cases": STRING,
        "upsert_case": STRING,
        "files_changed": STRING_LIST,
        "matrix_updates": {"type": "array", "items": MATRIX_UPDATE},
    },
    "verification": {
        "result": RESULT,
        "findings": STRING_LIST,
        "parity_result": RESULT,
        "entry_point": STRING,
        "ux_notes": STRING,
        "commands": COMMAND_LIST,
        "build_result": RESULT,
        "skipped_checks": SKIPPED_CHECK_LIST,
    },
}


def write_json(path, payload):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")


def read_json(path):
    return json.loads(path.read_text(encoding="utf-8"))


def load_role_routing(path):
    routing = read_json(path)
    if routing.get("version") != 1:
        raise ValueError("role routing version must be 1")
    profiles = routing.get("profiles")
    roles = routing.get("roles")
    phases = routing.get("phases")
    if not isinstance(profiles, dict) or not profiles:
        raise ValueError("role routing profiles must be a non-empty object")
    if not isinstance(roles, dict) or set(roles) != set(ROLE_ORDER):
        raise ValueError(f"role routing must define exactly: {', '.join(ROLE_ORDER)}")
    if phases != PHASE_ORDER:
        raise ValueError(f"role routing phases must be: {', '.join(PHASE_ORDER)}")
    for role, route in roles.items():
        if route.get("phase") != role:
            raise ValueError(f"role routing {role}.phase must be {role}")
        profile = route.get("profile")
        if profile not in profiles:
            raise ValueError(f"role routing {role}.profile={profile} is not defined")
        if "reasoning_effort" not in profiles[profile]:
            raise ValueError(f"role routing profile {profile} needs reasoning_effort")
    return routing


def init_workspace(args):
    run_dir = Path(args.run_dir).resolve()
    run_dir.mkdir(parents=True, exist_ok=True)
    matrix_path = Path(args.matrix).resolve()
    routing_path = Path(args.routing_config).resolve()
    profile_path = Path(args.repository_profile).resolve()
    routing = load_role_routing(routing_path)
    load_repository_profile(profile_path)
    package = prepare_case_execution(
        matrix_path,
        args.feature,
        index=args.index,
        sdk_family=args.sdk_family,
        key_apis=args.key_api,
        target_sdk_version=args.target_sdk_version,
        repository_profile=profile_path,
    )
    package["matrix_path"] = stable_matrix_path(matrix_path)
    package["routing_config"] = repo_relative_path(routing_path)
    package["routing_config_sha256"] = sha256_file(routing_path)
    package["repository_profile"] = repo_relative_path(profile_path)
    package["repository_profile_sha256"] = sha256_file(profile_path)
    write_json(run_dir / "execution-package.json", package)
    write_json(run_dir / "acceptance-manifest.json", package["acceptance_manifest_seed"])

    contract = package["acceptance_manifest_seed"]["contract"]
    write_artifact_seed(run_dir, "contract", "contract", None, contract)
    write_role_schema(run_dir / "role-schemas/contract.json", "contract")
    write_prompt(run_dir, "contract", "contract", None, package, routing, {})
    for platform in PLATFORMS:
        unit = package["acceptance_manifest_seed"]["platforms"][platform]
        for role in ["implementation", "verification"]:
            name = artifact_name(role, platform)
            write_artifact_seed(run_dir, name, role, platform, unit[role])
            write_role_schema(run_dir / "role-schemas" / f"{name}.json", role)
            write_prompt(run_dir, name, role, platform, package, routing, {})
    print(f"Execution workspace initialized: {run_dir}")
    return 0


def write_artifact_seed(run_dir, name, role, platform, artifact):
    payload = {"name": name, "role": role, **artifact}
    if platform:
        payload["platform"] = platform
    write_json(run_dir / "role-artifacts" / f"{name}.json", payload)


def write_role_schema(path, role):
    output_properties = ROLE_OUTPUT_PROPERTIES[role]
    schema = {
        "type": "object",
        "additionalProperties": False,
        "required": ["status", "evidence", "summary", "output"],
        "properties": {
            "status": {"type": "string", "enum": ROLE_STATUS_VALUES},
            "evidence": STRING,
            "summary": STRING,
            "waiver_reason": STRING,
            "output": {
                "type": "object",
                "additionalProperties": False,
                "required": list(output_properties),
                "properties": output_properties,
            },
        },
    }
    write_json(path, schema)


def write_prompt(run_dir, name, role, platform, package, routing, dependencies):
    requirement = package["requirement"]
    target = "all official platforms"
    if platform:
        target = resolved_platform_target(package, platform, dependencies)
    host_note = host_constraint(platform) if role == "verification" else ""
    waiver_note = ""
    if platform:
        waiver_note = (
            "If Contract marks this platform required=false, do not modify or verify it. "
            "Return status=WAIVED, copy Contract's waiver_reason into the top-level "
            "waiver_reason field, use SKIPPED for Verification result/parity_result/build_result, "
            "and report no executed build command."
        )
    body = f"""# {name} Agent Task

Requirement: {requirement['feature']}
SDK family: {requirement['sdk_family']}
Key APIs: {', '.join(requirement['key_apis'])}
Target SDK version: {requirement['target_sdk_version']}
Role: {role}
Platform: {platform or 'shared'}
Target: {target}
Model profile: {routing['roles'][role]['profile']}

Required output:
{json.dumps(package['role_contracts'][role], indent=2, ensure_ascii=False)}

Dependency artifacts:
{format_dependency_context(dependencies)}

The dispatcher starts platform work in the Contract-selected target so nested AGENTS.md files load automatically. Confirm those instructions before acting. Keep platform source changes inside the Contract target and never share source files across platform roots.

{host_note}

For Verification, report every executed check with kind=build|test|static|runtime and copy the command string exactly from Codex command telemetry. The dispatcher binds the declared result to the JSONL exit code.

{waiver_note}

Return only the JSON object required by the supplied output schema. The dispatcher records identity and provenance.
"""
    path = run_dir / "role-prompts" / f"{name}.md"
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(body, encoding="utf-8")
    return path


def host_constraint(platform):
    if platform == "windows" and sys.platform != "win32":
        host = "macOS" if sys.platform == "darwin" else sys.platform
        return (
            f"This host is {host}. {host} cannot provide Windows build evidence. "
            "Do not download Windows SDK archives, emulate Windows, cross-compile, or use a "
            "substitute compiler as platform evidence. Run repository-local static checks only "
            "and report unavailable Windows build/runtime evidence as BLOCKED."
        )
    return ""


def resolved_platform_target(package, platform, dependencies):
    contract = dependencies.get("contract", {})
    output = contract.get("output", {}) if isinstance(contract, dict) else {}
    targets = output.get("platform_targets", {}) if isinstance(output, dict) else {}
    if platform in targets and targets[platform].get("target_project"):
        return targets[platform]["target_project"]
    return package["acceptance_manifest_seed"]["contract"]["output"]["platform_targets"][platform][
        "target_project"
    ]


def format_dependency_context(dependencies):
    if not dependencies:
        return "None."
    sections = []
    for name, artifact in dependencies.items():
        payload = {
            "status": artifact.get("status"),
            "summary": artifact.get("summary"),
            "output": artifact.get("output"),
        }
        sections.append(f"- {name}:\n```json\n{json.dumps(payload, indent=2, ensure_ascii=False)}\n```")
    return "\n".join(sections)


def dispatch_workspace(args):
    run_dir = Path(args.run_dir).resolve()
    package = read_json(run_dir / "execution-package.json")
    routing_path = Path(args.routing_config).resolve()
    routing = load_role_routing(routing_path)
    validate_execution_configuration(package, routing_path)
    specs = dispatch_specs(run_dir, args.phase, args.platform, args.retry)
    route = routing["roles"][args.phase]
    profile = route["profile"]
    model = resolve_model(profile, args.model)
    codex_version = "dry-run" if args.dry_run else read_codex_version(args.codex_bin)
    tasks = []
    for spec in specs:
        dependencies = dependencies_for_spec(run_dir, spec)
        working_directory = resolve_working_directory(package, spec, dependencies)
        resolved = {
            "version": 1,
            "routing_config": repo_relative_path(routing_path),
            "routing_config_sha256": sha256_file(routing_path),
            "phase": args.phase,
            "name": spec["name"],
            "platform": spec["platform"],
            "profile": profile,
            "model": model,
            "reasoning_effort": routing["profiles"][profile]["reasoning_effort"],
            "codex_version": codex_version,
            "working_directory": repo_relative_path(working_directory),
        }
        write_json(run_dir / "resolved-role-routing" / f"{spec['name']}.json", resolved)
        snapshot_path, snapshot_sha256 = write_input_snapshot(
            run_dir, args.phase, spec["name"], dependencies, resolved
        )
        input_snapshot = read_json(snapshot_path)
        expected_repository_state = input_snapshot["repository_state"]
        prompt_path = write_prompt(
            run_dir, spec["name"], spec["role"], spec["platform"], package, routing, dependencies
        )
        schema_path = run_dir / "role-schemas" / f"{spec['name']}.json"
        write_role_schema(schema_path, spec["role"])
        raw_result_path = run_dir / "raw-results" / f"{spec['name']}.json"
        command = build_codex_command(
            args.codex_bin,
            route,
            model,
            routing["profiles"][profile]["reasoning_effort"],
            schema_path,
            raw_result_path,
            working_directory,
        )
        tasks.append(
            {
                **spec,
                "command": command,
                "prompt_path": prompt_path,
                "artifact_path": run_dir / "role-artifacts" / f"{spec['name']}.json",
                "raw_result_path": raw_result_path,
                "log_path": run_dir / "dispatch-logs" / f"{spec['name']}.jsonl",
                "stderr_path": run_dir / "dispatch-logs" / f"{spec['name']}.stderr.log",
                "model_profile": profile,
                "model": model,
                "reasoning_effort": routing["profiles"][profile]["reasoning_effort"],
                "codex_version": codex_version,
                "working_directory": working_directory,
                "working_directory_manifest": repo_relative_path(working_directory),
                "input_snapshot": run_relative_path(snapshot_path, run_dir),
                "input_snapshot_sha256": snapshot_sha256,
                "expected_repository_state": expected_repository_state,
                "snapshot_dependencies": dependencies,
                "resolved_routing": resolved,
                "previous_files_changed": (
                    read_json(run_dir / "role-artifacts" / f"{spec['name']}.json")
                    .get("output", {})
                    .get("files_changed", [])
                    if spec["role"] == "implementation"
                    else []
                ),
                "timeout_seconds": args.timeout_seconds,
            }
        )
    if args.dry_run:
        print(
            json.dumps(
                {
                    "phase": args.phase,
                    "commands": [
                        {
                            "name": task["name"],
                            "platform": task["platform"],
                            "command": task["command"],
                        }
                        for task in tasks
                    ],
                },
                indent=2,
                ensure_ascii=False,
            )
        )
        return 0
    if args.phase == "implementation":
        for task in tasks:
            invalidate_platform_verification(run_dir, package, task["platform"])
        run_implementation_tasks(run_dir, tasks)
    elif args.phase == "verification":
        run_verification_tasks(run_dir, tasks)
    else:
        run_tasks(tasks)
    if args.phase == "contract":
        validate_completed_contract(run_dir)
    print(f"Codex phase completed: {args.phase} ({', '.join(task['name'] for task in tasks)})")
    return 0


def dispatch_specs(run_dir, phase, requested_platforms, retry):
    if phase == "contract":
        if requested_platforms:
            raise ValueError("--platform is not valid for the shared contract phase")
        candidates = [{"name": "contract", "role": "contract", "platform": None}]
    else:
        platforms = requested_platforms or PLATFORMS
        candidates = [
            {"name": artifact_name(phase, platform), "role": phase, "platform": platform}
            for platform in platforms
        ]
    selected = []
    for spec in candidates:
        path = run_dir / "role-artifacts" / f"{spec['name']}.json"
        if not path.exists():
            raise ValueError(f"missing role artifact: {spec['name']}")
        artifact = read_json(path)
        already_dispatched = artifact.get("dispatch", {}).get("mode") == "codex-exec"
        status = artifact.get("status")
        if already_dispatched:
            if retry and status in {"FAIL", "BLOCKED"}:
                selected.append(spec)
            elif requested_platforms or phase == "contract":
                raise ValueError(f"role {spec['name']} has already been dispatched with status {status}")
            continue
        selected.append(spec)
    if not selected:
        raise ValueError(f"all {phase} role artifacts are already complete")
    return selected


def validate_completed_contract(run_dir):
    artifact_path = run_dir / "role-artifacts/contract.json"
    artifact = read_json(artifact_path)
    if artifact.get("status") != "PASS":
        if artifact.get("status") not in {"FAIL", "BLOCKED"}:
            artifact["status"] = "BLOCKED"
            artifact["evidence"] = "Contract is required and cannot be waived."
            artifact["summary"] = "Contract requires correction before platform implementation."
            write_json(artifact_path, artifact)
        raise ValueError(f"Contract gate returned status {artifact.get('status')}")
    manifest = read_json(run_dir / "acceptance-manifest.json")
    manifest["contract"] = strip_artifact_metadata(artifact)
    errors = []
    validate_contract(manifest, errors, {})
    if not errors:
        return
    artifact["status"] = "BLOCKED"
    artifact["evidence"] = "Contract semantic gate failed: " + "; ".join(errors)
    artifact["summary"] = "Contract requires correction before platform implementation."
    write_json(artifact_path, artifact)
    raise ValueError("Contract semantic gate failed: " + "; ".join(errors))


def dependencies_for_spec(run_dir, spec):
    dependency_names = []
    if spec["role"] == "implementation":
        dependency_names = ["contract"]
    elif spec["role"] == "verification":
        dependency_names = ["contract", artifact_name("implementation", spec["platform"])]
    dependencies = {}
    for name in dependency_names:
        if name == "contract":
            validate_completed_contract(run_dir)
        if name.endswith("-implementation"):
            validate_completed_implementation(run_dir, spec["platform"])
        path = run_dir / "role-artifacts" / f"{name}.json"
        if not path.exists():
            raise ValueError(f"role {spec['name']} requires missing dependency artifact: {name}")
        artifact = read_json(path)
        status = artifact.get("status")
        allowed_statuses = {"PASS"} if name == "contract" else COMPLETED_DEPENDENCY_STATUSES
        if status not in allowed_statuses:
            raise ValueError(
                f"role {spec['name']} requires {name} status "
                f"{'PASS' if name == 'contract' else 'PASS or WAIVED'}, got {status}"
            )
        dependencies[name] = artifact
    return dependencies


def invalidate_platform_verification(run_dir, package, platform):
    seed = package["acceptance_manifest_seed"]["platforms"][platform]["verification"]
    name = artifact_name("verification", platform)
    write_artifact_seed(run_dir, name, "verification", platform, seed)


def resolve_working_directory(package, spec, dependencies):
    if not spec["platform"]:
        return REPO_ROOT
    target = resolved_platform_target(package, spec["platform"], dependencies)
    platform_root = PLATFORM_ROOTS[spec["platform"]]
    if not path_is_within(target, platform_root):
        raise ValueError(
            f"{spec['platform']} target must be inside {platform_root}: {target}"
        )
    path = (REPO_ROOT / target).resolve()
    try:
        path.relative_to(REPO_ROOT)
    except ValueError as exc:
        raise ValueError(f"platform target must be inside the repository: {target}") from exc
    if not path.is_dir():
        raise ValueError(f"platform target directory does not exist: {target}")
    return path


def run_tasks(tasks):
    errors = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=min(len(tasks), len(PLATFORMS))) as executor:
        future_tasks = {executor.submit(run_codex_role, **task_args(task)): task for task in tasks}
        for future in concurrent.futures.as_completed(future_tasks):
            task = future_tasks[future]
            try:
                future.result()
            except (OSError, ValueError, json.JSONDecodeError) as exc:
                errors.append((task["name"], str(exc)))
    if errors:
        errors.sort()
        raise ValueError("; ".join(detail for _, detail in errors))


def run_verification_tasks(run_dir, tasks):
    errors = []
    try:
        run_tasks(tasks)
    except ValueError as exc:
        errors.append(str(exc))
    for task in tasks:
        try:
            validate_completed_verification(run_dir, task["platform"])
        except (OSError, ValueError, KeyError, json.JSONDecodeError) as exc:
            errors.append(str(exc))
    if errors:
        raise ValueError("; ".join(errors))


def run_implementation_tasks(run_dir, tasks):
    errors = []
    for task in tasks:
        refresh_implementation_input_snapshot(run_dir, task)
        before = collect_repository_fingerprints()
        try:
            run_codex_role(**task_args(task))
        except (OSError, ValueError, KeyError, json.JSONDecodeError) as exc:
            errors.append((task["name"], str(exc)))
        try:
            reconcile_implementation_changes(run_dir, [task], before)
        except (OSError, ValueError, KeyError, json.JSONDecodeError) as exc:
            errors.append((task["name"], str(exc)))
        try:
            validate_completed_implementation(run_dir, task["platform"])
        except (OSError, ValueError, KeyError, json.JSONDecodeError) as exc:
            errors.append((task["name"], str(exc)))
    if errors:
        errors.sort()
        raise ValueError("; ".join(detail for _, detail in errors))


def validate_completed_implementation(run_dir, platform):
    artifact_path = run_dir / "role-artifacts" / f"{platform}-implementation.json"
    artifact = read_json(artifact_path)
    contract = read_json(run_dir / "role-artifacts/contract.json")
    target = contract.get("output", {}).get("platform_targets", {}).get(platform, {})
    required = bool(target.get("required", True))
    expected_status = "PASS" if required else "WAIVED"
    errors = []
    if artifact.get("status") != expected_status:
        errors.append(
            f"{platform} Implementation gate requires status {expected_status}, "
            f"got {artifact.get('status')}"
        )
    manifest = read_json(run_dir / "acceptance-manifest.json")
    manifest["final_status"] = "PASS"
    verification_stub = {
        "build_result": "PASS",
        "parity_result": "PASS",
        "skipped_checks": [],
    }
    output = artifact.get("output")
    if not isinstance(output, dict) or not output:
        errors.append(f"{platform} Implementation output must be a non-empty object")
    else:
        validate_platform_implementation(
            platform,
            target,
            output,
            verification_stub,
            manifest,
            errors,
        )
    if not errors:
        return
    if artifact.get("status") not in {"FAIL", "BLOCKED"}:
        artifact["status"] = "BLOCKED"
        artifact["evidence"] = "Implementation semantic gate failed: " + "; ".join(errors)
        artifact["summary"] = "Implementation requires correction before Verification."
        write_json(artifact_path, artifact)
    raise ValueError("Implementation semantic gate failed: " + "; ".join(errors))


def validate_completed_verification(run_dir, platform):
    artifact_path = run_dir / "role-artifacts" / f"{platform}-verification.json"
    artifact = read_json(artifact_path)
    contract = read_json(run_dir / "role-artifacts/contract.json")
    target = contract.get("output", {}).get("platform_targets", {}).get(platform, {})
    required = bool(target.get("required", True))
    status = artifact.get("status")
    if required and status in {"FAIL", "BLOCKED"}:
        return
    errors = []
    if required and status != "PASS":
        errors.append(f"{platform} Verification gate requires PASS, FAIL, or BLOCKED, got {status}")
    if not required and status != "WAIVED":
        errors.append(f"{platform} Verification gate requires status WAIVED, got {status}")
    manifest = read_json(run_dir / "acceptance-manifest.json")
    manifest["final_status"] = "PASS WITH RISKS"
    output = artifact.get("output")
    if not isinstance(output, dict) or not output:
        errors.append(f"{platform} Verification output must be a non-empty object")
    else:
        validate_platform_verification(
            platform,
            required,
            output,
            artifact.get("dispatch", {}),
            manifest,
            errors,
        )
    if not errors:
        return
    if status not in {"FAIL", "BLOCKED"}:
        artifact["status"] = "BLOCKED"
        artifact["evidence"] = "Verification semantic gate failed: " + "; ".join(errors)
        artifact["summary"] = "Verification requires correction before acceptance."
        write_json(artifact_path, artifact)
    raise ValueError("Verification semantic gate failed: " + "; ".join(errors))


def refresh_implementation_input_snapshot(run_dir, task):
    snapshot_path, snapshot_sha256 = write_input_snapshot(
        run_dir,
        "implementation",
        task["name"],
        task["snapshot_dependencies"],
        task["resolved_routing"],
    )
    task["input_snapshot"] = run_relative_path(snapshot_path, run_dir)
    task["input_snapshot_sha256"] = snapshot_sha256
    task["expected_repository_state"] = read_json(snapshot_path)["repository_state"]


def task_args(task):
    allowed = {
        "command",
        "name",
        "role",
        "platform",
        "prompt_path",
        "artifact_path",
        "raw_result_path",
        "log_path",
        "stderr_path",
        "model_profile",
        "model",
        "reasoning_effort",
        "codex_version",
        "working_directory",
        "working_directory_manifest",
        "input_snapshot",
        "input_snapshot_sha256",
        "expected_repository_state",
        "timeout_seconds",
    }
    return {key: value for key, value in task.items() if key in allowed}


def resolve_bound_repository_profile(package):
    profile_value = package.get("repository_profile")
    if not isinstance(profile_value, str) or not profile_value:
        raise ValueError("execution package repository profile is missing; re-run init")
    profile_path = (REPO_ROOT / profile_value).resolve()
    try:
        profile_path.relative_to(REPO_ROOT.resolve())
    except ValueError as exc:
        raise ValueError("execution package repository profile must be inside the repository") from exc
    if package.get("repository_profile_sha256") != sha256_file(profile_path):
        raise ValueError("repository profile content changed since init; re-run init")
    load_repository_profile(profile_path)
    return profile_path


def validate_execution_configuration(package, routing_path):
    if package.get("routing_config") != repo_relative_path(routing_path):
        raise ValueError("routing config path changed since init; re-run init")
    if package.get("routing_config_sha256") != sha256_file(routing_path):
        raise ValueError("routing config content changed since init; re-run init")
    resolve_bound_repository_profile(package)


def resolve_model(profile, override):
    model = override or os.environ.get(f"CODEX_MODEL_{profile.upper()}")
    if not model:
        raise ValueError(f"model is required; pass --model or set CODEX_MODEL_{profile.upper()}")
    return model


def read_codex_version(codex_bin):
    try:
        result = subprocess.run(
            [codex_bin, "--version"],
            cwd=REPO_ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=10,
        )
    except subprocess.TimeoutExpired as exc:
        raise ValueError("timed out while reading Codex version") from exc
    if result.returncode != 0:
        raise ValueError(f"failed to read Codex version: {result.stderr.strip()}")
    if not result.stdout.strip():
        raise ValueError("Codex version output is empty")
    return result.stdout.strip()


def write_input_snapshot(run_dir, phase, name, dependencies, resolved_routing):
    package = read_json(run_dir / "execution-package.json")
    dependency_hashes = {
        dependency: sha256_file(run_dir / "role-artifacts" / f"{dependency}.json")
        for dependency in dependencies
    }
    snapshot = {
        "version": 1,
        "phase": phase,
        "name": name,
        "execution_package_sha256": sha256_file(run_dir / "execution-package.json"),
        "repository_state": repository_state(),
        "dependency_artifact_sha256": dependency_hashes,
        "routing_config": resolved_routing["routing_config"],
        "routing_config_sha256": resolved_routing["routing_config_sha256"],
        "repository_profile": package["repository_profile"],
        "repository_profile_sha256": package["repository_profile_sha256"],
    }
    serialized = json.dumps(snapshot, indent=2, ensure_ascii=False) + "\n"
    snapshot_sha256 = sha256_text(serialized)
    path = run_dir / "input-snapshots" / f"{name}-{snapshot_sha256[:12]}.json"
    path.parent.mkdir(parents=True, exist_ok=True)
    if path.exists() and path.read_text(encoding="utf-8") != serialized:
        raise ValueError(f"input snapshot hash collision: {path}")
    path.write_text(serialized, encoding="utf-8")
    return path, snapshot_sha256


def run_git(arguments):
    result = subprocess.run(
        ["git", *arguments], cwd=REPO_ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE
    )
    if result.returncode != 0:
        raise ValueError(f"git {' '.join(arguments)} failed: {result.stderr.strip()}")
    return result.stdout.rstrip("\n")


def run_git_bytes(arguments):
    result = subprocess.run(
        ["git", *arguments], cwd=REPO_ROOT, stdout=subprocess.PIPE, stderr=subprocess.PIPE
    )
    if result.returncode != 0:
        detail = result.stderr.decode("utf-8", errors="replace").strip()
        raise ValueError(f"git {' '.join(arguments)} failed: {detail}")
    return result.stdout


def collect_repository_fingerprints(repo_root=REPO_ROOT, include_tracked=True):
    repo_root = Path(repo_root)
    fingerprints = {}
    if include_tracked:
        staged = run_git_bytes_at(repo_root, ["ls-files", "--stage", "-z"])
        for record in filter(None, staged.split(b"\0")):
            metadata, separator, raw_path = record.partition(b"\t")
            if not separator:
                continue
            mode, object_id, stage = metadata.split()
            path_text = os.fsdecode(raw_path)
            fingerprints[path_text] = {
                "type": "index",
                "mode": mode.decode("ascii"),
                "object_id": object_id.decode("ascii"),
                "stage": stage.decode("ascii"),
            }
        dirty = run_git_bytes_at(repo_root, ["diff", "--name-only", "-z"])
        for raw_path in filter(None, dirty.split(b"\0")):
            path_text = os.fsdecode(raw_path)
            fingerprints[path_text] = fingerprint_repository_path(repo_root / path_text)

    untracked = run_git_bytes_at(
        repo_root, ["ls-files", "-z", "--others", "--exclude-standard"]
    )
    for raw_path in filter(None, untracked.split(b"\0")):
        path_text = os.fsdecode(raw_path)
        fingerprints[path_text] = fingerprint_repository_path(repo_root / path_text)
    return dict(sorted(fingerprints.items()))


def run_git_bytes_at(repo_root, arguments):
    result = subprocess.run(
        ["git", *arguments], cwd=repo_root, stdout=subprocess.PIPE, stderr=subprocess.PIPE
    )
    if result.returncode != 0:
        detail = result.stderr.decode("utf-8", errors="replace").strip()
        raise ValueError(f"git {' '.join(arguments)} failed: {detail}")
    return result.stdout


def fingerprint_repository_path(path):
    try:
        metadata = path.lstat()
    except FileNotFoundError:
        return {"type": "missing", "mode": "", "sha256": ""}
    mode = oct(stat.S_IMODE(metadata.st_mode))
    if path.is_symlink():
        return {
            "type": "symlink",
            "mode": mode,
            "sha256": sha256_text(os.readlink(path)),
        }
    if path.is_file():
        return {"type": "file", "mode": mode, "sha256": sha256_file(path)}
    return {"type": "other", "mode": mode, "sha256": ""}


def repository_state():
    return {
        "head": run_git(["rev-parse", "HEAD"]),
        "status": run_git(["status", "--short"]),
        "diff_sha256": sha256_bytes(run_git_bytes(["diff", "--binary", "HEAD"])),
        "untracked_files": collect_repository_fingerprints(include_tracked=False),
    }


def changed_repository_paths(before, after):
    return sorted(
        path
        for path in set(before) | set(after)
        if before.get(path) != after.get(path)
    )


def normalize_repo_path_text(path_text):
    path = Path(path_text)
    if path.is_absolute():
        return repo_relative_path(path)
    normalized = str(path_text).replace("\\", "/")
    return normalized[2:] if normalized.startswith("./") else normalized


def reconcile_implementation_changes(run_dir, tasks, before):
    after = collect_repository_fingerprints()
    changed = changed_repository_paths(before, after)
    dispatched_platforms = {task["platform"] for task in tasks}
    unexpected = [
        path
        for path in changed
        if not any(
            platform in dispatched_platforms and path_is_within(path, root)
            for platform, root in PLATFORM_ROOTS.items()
        )
    ]
    contract = read_json(run_dir / "role-artifacts/contract.json")
    targets = contract.get("output", {}).get("platform_targets", {})
    scope_errors = []
    for task in tasks:
        platform = task["platform"]
        platform_changes = [
            path for path in changed if path_is_within(path, PLATFORM_ROOTS[platform])
        ]
        allowed = targets.get(platform, {}).get("files_allowed", [])
        outside_allowed = [
            path
            for path in platform_changes
            if not any(path_is_within(path, allowed_path) for allowed_path in allowed)
        ]
        if outside_allowed:
            scope_errors.append(
                f"{platform} Implementation modified files outside Contract files_allowed: "
                + ", ".join(outside_allowed)
            )

        artifact_path = run_dir / "role-artifacts" / f"{task['name']}.json"
        artifact = read_json(artifact_path)
        previous = {
            normalize_repo_path_text(path) for path in task.get("previous_files_changed", [])
        }
        delta_path = run_dir / "repository-deltas" / f"{task['name']}.json"
        prior_delta = read_json(delta_path) if delta_path.exists() else {}
        before_values = dict(prior_delta.get("before", {}))
        after_values = dict(prior_delta.get("after", {}))
        for path in previous:
            before_values.setdefault(path, None)
            after_values.setdefault(path, after.get(path))
        for path in platform_changes:
            before_values.setdefault(path, before.get(path))
            after_values[path] = after.get(path)
        cumulative_paths = previous | set(platform_changes) | set(prior_delta.get("changed_files", []))
        net_changes = sorted(
            path for path in cumulative_paths if before_values.get(path) != after_values.get(path)
        )
        artifact["output"]["files_changed"] = net_changes
        current_dispatch = artifact.get("dispatch", {})
        current_snapshot = current_dispatch.get("input_snapshot_sha256")
        attempt = {
            "input_snapshot": task.get("input_snapshot"),
            "input_snapshot_sha256": task.get("input_snapshot_sha256"),
            "run_id": (
                current_dispatch.get("run_id")
                if current_snapshot == task.get("input_snapshot_sha256")
                else ""
            ),
            "status": (
                artifact.get("status")
                if current_snapshot == task.get("input_snapshot_sha256")
                else "DISPATCH_ERROR"
            ),
            "changed_files": platform_changes,
        }
        delta = {
            "version": 2,
            "platform": platform,
            "changed_files": net_changes,
            "before": {path: before_values.get(path) for path in net_changes},
            "after": {path: after_values.get(path) for path in net_changes},
            "attempts": [*prior_delta.get("attempts", []), attempt],
        }
        write_json(delta_path, delta)
        artifact["dispatch"]["repository_delta"] = run_relative_path(delta_path, run_dir)
        artifact["dispatch"]["repository_delta_sha256"] = sha256_file(delta_path)
        write_json(artifact_path, artifact)
    if unexpected:
        violation_path = run_dir / "repository-deltas/implementation-scope-violations.json"
        write_json(
            violation_path,
            {
                "version": 1,
                "changed_files": unexpected,
                "before": {path: before.get(path) for path in unexpected},
                "after": {path: after.get(path) for path in unexpected},
            },
        )
        scope_errors.append(
            "Implementation modified files outside dispatched platform roots: "
            + ", ".join(unexpected)
        )
    if scope_errors:
        raise ValueError("; ".join(scope_errors))


def build_codex_command(
    codex_bin, route, model, reasoning_effort, schema_path, raw_result_path, working_directory
):
    raw_result_path.parent.mkdir(parents=True, exist_ok=True)
    command = [
        codex_bin,
        "exec",
        "-C",
        str(working_directory),
        "-s",
        route["sandbox"],
    ]
    command.extend(
        [
            "--json",
            "--output-schema",
            str(schema_path),
            "--output-last-message",
            str(raw_result_path),
            "--model",
            model,
            "-c",
            f'model_reasoning_effort="{reasoning_effort}"',
            "-",
        ]
    )
    return command


def run_codex_role(
    command,
    name,
    role,
    platform,
    prompt_path,
    artifact_path,
    raw_result_path,
    log_path,
    stderr_path,
    model_profile,
    model,
    reasoning_effort,
    codex_version,
    working_directory,
    working_directory_manifest,
    input_snapshot,
    input_snapshot_sha256,
    expected_repository_state,
    timeout_seconds,
):
    prompt = prompt_path.read_text(encoding="utf-8")
    log_path.parent.mkdir(parents=True, exist_ok=True)
    print(f"Dispatching {name} (timeout {timeout_seconds}s); logs: {log_path}", flush=True)
    execution_error = None
    with log_path.open("w", encoding="utf-8") as stdout_handle, stderr_path.open(
        "w", encoding="utf-8"
    ) as stderr_handle:
        process = subprocess.Popen(
            command,
            cwd=working_directory,
            stdin=subprocess.PIPE,
            stdout=stdout_handle,
            stderr=stderr_handle,
            text=True,
            start_new_session=sys.platform != "win32",
            creationflags=(
                getattr(subprocess, "CREATE_NEW_PROCESS_GROUP", 0)
                if sys.platform == "win32"
                else 0
            ),
        )
        try:
            process.communicate(input=prompt, timeout=timeout_seconds)
        except subprocess.TimeoutExpired:
            terminate_process_tree(process)
            process.communicate()
            execution_error = ValueError(
                f"Codex dispatch for {name} timed out after {timeout_seconds} seconds"
            )
    stdout = log_path.read_text(encoding="utf-8")
    stderr = stderr_path.read_text(encoding="utf-8")
    if execution_error is None and process.returncode != 0:
        execution_error = ValueError(
            f"Codex dispatch failed for {name}: {(stderr or stdout).strip()}"
        )
    mutation_error = None
    if role in {"contract", "verification"}:
        actual_repository_state = repository_state()
        if actual_repository_state != expected_repository_state:
            violation_path = (
                artifact_path.parent.parent
                / "repository-deltas"
                / f"{name}-scope-violation.json"
            )
            write_json(
                violation_path,
                {
                    "version": 1,
                    "role": role,
                    "name": name,
                    "before": expected_repository_state,
                    "after": actual_repository_state,
                },
            )
            mutation_error = ValueError(
                f"Codex {role} run {name} modified repository files"
            )
    if execution_error or mutation_error:
        details = [str(error) for error in [execution_error, mutation_error] if error]
        raise ValueError("; ".join(details))
    run_id = extract_run_id(stdout)
    if not run_id:
        raise ValueError(f"Codex dispatch for {name} did not report a thread or run id")
    agent_result = read_json(raw_result_path)
    for field in ["status", "evidence", "summary", "output"]:
        if field not in agent_result:
            raise ValueError(f"Codex result for {name} is missing {field}")
    if role == "verification":
        validate_verification_host(platform, agent_result, sys.platform)
        bind_verification_command_evidence(
            agent_result["output"],
            stdout,
            run_relative_path(log_path, artifact_path.parent.parent),
            platform,
            working_directory_manifest,
        )
    artifact = {
        "name": name,
        "role": role,
        **({"platform": platform} if platform else {}),
        "agent_id": f"{name}-{run_id}",
        "dispatch": {
            "mode": "codex-exec",
            "run_id": run_id,
            "prompt": f"role-prompts/{name}.md",
            "prompt_sha256": sha256_text(prompt),
            "artifact": f"role-artifacts/{name}.json",
            "evidence": f"{codex_version} completed {name}; log: dispatch-logs/{name}.jsonl",
            "command_log": run_relative_path(log_path, artifact_path.parent.parent),
            "command_log_sha256": sha256_file(log_path),
            "model_profile": model_profile,
            "model": model,
            "reasoning_effort": reasoning_effort,
            "codex_version": codex_version,
            "working_directory": working_directory_manifest,
            "host_platform": sys.platform,
            "input_snapshot": input_snapshot,
            "input_snapshot_sha256": input_snapshot_sha256,
        },
        "status": agent_result["status"],
        "evidence": agent_result["evidence"],
        "summary": agent_result["summary"],
        "output": agent_result["output"],
    }
    if "waiver_reason" in agent_result:
        artifact["waiver_reason"] = agent_result["waiver_reason"]
    write_json(artifact_path, artifact)


def terminate_process_tree(process):
    if process.poll() is not None:
        return
    if sys.platform == "win32":
        result = subprocess.run(
            ["taskkill", "/F", "/T", "/PID", str(process.pid)],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            check=False,
        )
        if result.returncode != 0 and process.poll() is None:
            process.kill()
        return
    try:
        os.killpg(os.getpgid(process.pid), signal.SIGKILL)
    except ProcessLookupError:
        return


def validate_verification_host(platform, agent_result, host_platform):
    if platform != "windows" or host_platform == "win32":
        return
    output = agent_result.get("output", {})
    has_build_execution = any(
        isinstance(command, dict)
        and command.get("kind") == "build"
        and command.get("result") in {"PASS", "FAIL"}
        for command in output.get("commands", [])
    )
    if agent_result.get("status") == "WAIVED":
        if output.get("build_result") == "PASS" or has_build_execution:
            raise ValueError("WAIVED Windows Verification cannot report build execution")
        return
    if (
        agent_result.get("status") != "BLOCKED"
        or output.get("result") != "BLOCKED"
        or output.get("build_result") != "BLOCKED"
        or has_build_execution
    ):
        raise ValueError(
            "Windows Verification build evidence requires a Windows host; "
            f"current host_platform={host_platform}"
        )


def bind_verification_command_evidence(
    output, jsonl, log_path, platform, working_directory_manifest
):
    commands = output.get("commands", []) if isinstance(output, dict) else []
    events = extract_command_events(jsonl)
    used = set()
    for index, declared in enumerate(commands):
        result = declared.get("result")
        if result not in {"PASS", "FAIL"}:
            continue
        if declared.get("kind") == "build" and not is_platform_build_command(
            platform, declared.get("command"), working_directory_manifest
        ):
            raise ValueError(
                f"Verification command {index} is not a recognized {platform} build command: "
                f"{declared.get('command')}"
            )
        declared_text = normalize_command(declared.get("command"))
        match_index = next(
            (
                event_index
                for event_index, event in enumerate(events)
                if event_index not in used and normalize_command(event["command"]) == declared_text
            ),
            None,
        )
        if match_index is None:
            raise ValueError(
                f"Verification command {index} has no matching command_execution event: {declared.get('command')}"
            )
        used.add(match_index)
        event = events[match_index]
        actual_result = "PASS" if event["exit_code"] == 0 else "FAIL"
        if result != actual_result:
            raise ValueError(
                f"Verification command {declared.get('command')} declared {result} but exit_code={event['exit_code']}"
            )
        declared["evidence"] = (
            f"{log_path}#{event['id']}; exit_code={event['exit_code']}"
        )


def extract_command_events(jsonl):
    events = []
    for line in jsonl.splitlines():
        try:
            event = json.loads(line)
        except json.JSONDecodeError:
            continue
        item = event.get("item")
        if event.get("type") != "item.completed" or not isinstance(item, dict):
            continue
        if item.get("type") != "command_execution" or not isinstance(item.get("exit_code"), int):
            continue
        events.append(
            {
                "id": str(item.get("id") or f"command-{len(events)}"),
                "command": item.get("command"),
                "exit_code": item["exit_code"],
            }
        )
    return events


def normalize_command(command):
    if isinstance(command, list):
        command = " ".join(str(part) for part in command)
    return " ".join(str(command or "").split())


def extract_run_id(jsonl):
    for line in jsonl.splitlines():
        try:
            event = json.loads(line)
        except json.JSONDecodeError:
            continue
        for field in ["thread_id", "run_id", "session_id"]:
            if event.get(field):
                return str(event[field])
        thread = event.get("thread")
        if isinstance(thread, dict) and thread.get("id"):
            return str(thread["id"])
    return ""


def assemble_workspace(args):
    run_dir = Path(args.run_dir).resolve()
    scope_violations = sorted(
        (run_dir / "repository-deltas").glob("*scope-violation*.json")
    )
    if scope_violations:
        raise ValueError(
            "repository scope violation is unresolved; restore the repository and re-run init"
        )
    package = read_json(run_dir / "execution-package.json")
    profile_path = resolve_bound_repository_profile(package)
    matrix_path = Path(args.matrix).resolve()
    if package.get("matrix_path") != stable_matrix_path(matrix_path):
        raise ValueError("matrix path changed since init; re-run init")
    manifest = read_json(run_dir / "acceptance-manifest.json")
    if manifest.get("requirement") != package.get("requirement"):
        raise ValueError("requirement changed since init; re-run init")
    artifacts = load_role_artifacts(run_dir / "role-artifacts")
    manifest["final_status"] = args.final_status
    manifest["contract"] = strip_artifact_metadata(artifacts["contract"])
    for platform in PLATFORMS:
        manifest["platforms"][platform] = {
            role: strip_artifact_metadata(artifacts[artifact_name(role, platform)])
            for role in ["implementation", "verification"]
        }
    manifest["cross_platform_acceptance"] = {
        "result": args.cross_platform_result,
        "evidence": args.cross_platform_evidence,
        "differences": args.cross_platform_difference,
    }
    manifest["release"]["checks"] = collect_sdk_version_checks(
        manifest["requirement"]["target_sdk_version"], profile_path=profile_path
    )
    errors = validate_manifest(manifest)
    errors.extend(validate_evidence_files(manifest, run_dir))
    if errors:
        for error in errors:
            print(f"ERROR: {error}", file=sys.stderr)
        return 1
    updates = []
    for platform in PLATFORMS:
        updates.extend(manifest["platforms"][platform]["implementation"]["output"]["matrix_updates"])
    if updates and manifest["final_status"] != "BLOCKED":
        update_matrix(matrix_path, updates)
    output_path = run_dir / "final-acceptance-manifest.json"
    write_json(output_path, manifest)
    print(f"Acceptance manifest valid: {output_path}")
    return 0

def load_role_artifacts(artifact_dir):
    artifacts = {}
    for artifact_path in sorted(artifact_dir.glob("*.json")):
        artifact = read_json(artifact_path)
        if not isinstance(artifact, dict):
            raise ValueError(f"role artifact must be an object: {artifact_path.name}")
        name = artifact.get("name") or artifact_path.stem
        attach_dispatch_hashes(artifact, name, artifact_path, artifact_dir.parent)
        artifacts[name] = artifact
    expected = {"contract"}
    expected.update(
        artifact_name(role, platform)
        for platform in PLATFORMS
        for role in ["implementation", "verification"]
    )
    missing = expected - set(artifacts)
    if missing:
        raise ValueError(f"missing role artifacts: {', '.join(sorted(missing))}")
    return artifacts


def attach_dispatch_hashes(artifact, name, artifact_path, run_dir):
    dispatch = artifact.setdefault("dispatch", {})
    dispatch.setdefault("artifact", f"role-artifacts/{name}.json")
    dispatch["artifact_sha256"] = sha256_file(artifact_path)
    prompt_path = resolve_run_path(run_dir, dispatch.get("prompt", f"role-prompts/{name}.md"))
    if prompt_path.exists():
        actual_sha256 = sha256_file(prompt_path)
        expected_sha256 = dispatch.get("prompt_sha256")
        if expected_sha256 and expected_sha256 != actual_sha256:
            raise ValueError(f"prompt sha256 mismatch for {name}: {dispatch.get('prompt')}")
        dispatch["prompt_sha256"] = actual_sha256
    snapshot_value = dispatch.get("input_snapshot")
    if snapshot_value:
        snapshot_path = resolve_run_path(run_dir, snapshot_value)
        if not snapshot_path.exists():
            raise ValueError(f"input snapshot does not exist for {name}: {snapshot_value}")
        actual_sha256 = sha256_file(snapshot_path)
        if dispatch.get("input_snapshot_sha256") != actual_sha256:
            raise ValueError(f"input snapshot sha256 mismatch for {name}: {snapshot_value}")
        snapshot = read_json(snapshot_path)
        if not isinstance(snapshot, dict):
            raise ValueError(f"input snapshot must be an object for {name}")
        package_path = run_dir / "execution-package.json"
        if not package_path.is_file():
            raise ValueError(f"execution package does not exist for {name}")
        if snapshot.get("execution_package_sha256") != sha256_file(package_path):
            raise ValueError(f"execution package sha256 mismatch for {name}")
        dependency_hashes = snapshot.get("dependency_artifact_sha256", {})
        if not isinstance(dependency_hashes, dict):
            raise ValueError(f"input snapshot dependencies must be an object for {name}")
        for dependency, expected_sha256 in dependency_hashes.items():
            if Path(dependency).name != dependency:
                raise ValueError(f"invalid dependency artifact name for {name}: {dependency}")
            dependency_path = run_dir / "role-artifacts" / f"{dependency}.json"
            if not dependency_path.is_file():
                raise ValueError(f"dependency artifact does not exist for {name}: {dependency}")
            if sha256_file(dependency_path) != expected_sha256:
                raise ValueError(
                    f"dependency artifact sha256 mismatch for {name}: {dependency}"
                )
    command_log_value = dispatch.get("command_log")
    if command_log_value:
        command_log_path = resolve_run_path(run_dir, command_log_value)
        if not command_log_path.exists():
            raise ValueError(f"command log does not exist for {name}: {command_log_value}")
        actual_sha256 = sha256_file(command_log_path)
        if dispatch.get("command_log_sha256") != actual_sha256:
            raise ValueError(f"command log sha256 mismatch for {name}: {command_log_value}")
    repository_delta_value = dispatch.get("repository_delta")
    if repository_delta_value:
        repository_delta_path = resolve_run_path(run_dir, repository_delta_value)
        if not repository_delta_path.exists():
            raise ValueError(
                f"repository delta does not exist for {name}: {repository_delta_value}"
            )
        actual_sha256 = sha256_file(repository_delta_path)
        if dispatch.get("repository_delta_sha256") != actual_sha256:
            raise ValueError(
                f"repository delta sha256 mismatch for {name}: {repository_delta_value}"
            )
        delta = read_json(repository_delta_path)
        if not isinstance(delta, dict):
            raise ValueError(f"repository delta must be an object for {name}")
        delta_files = {
            normalize_repo_path_text(path) for path in delta.get("changed_files", [])
        }
        artifact_files = {
            normalize_repo_path_text(path)
            for path in artifact.get("output", {}).get("files_changed", [])
        }
        if delta_files != artifact_files:
            raise ValueError(f"repository delta changed_files mismatch for {name}")


def strip_artifact_metadata(artifact):
    return {
        key: value
        for key, value in artifact.items()
        if key not in {"name", "role", "platform"}
    }


def resolve_run_path(run_dir, path_text):
    path = Path(path_text)
    if path.is_absolute() or ".." in path.parts:
        raise ValueError(f"execution artifact path must be relative to run directory: {path_text}")
    return run_dir / path


def update_matrix(matrix_path, updates):
    lines = matrix_path.read_text(encoding="utf-8").splitlines()
    for update in updates:
        lines = apply_matrix_update(lines, update)
    matrix_path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def apply_matrix_update(lines, update):
    for index, line in enumerate(lines):
        if not line.startswith("|"):
            continue
        header = split_markdown_row(line)
        if "Feature" not in header or update["platform_unit"] not in header:
            continue
        feature_index = header.index("Feature")
        platform_index = header.index(update["platform_unit"])
        for row_index in range(index + 2, len(lines)):
            row_line = lines[row_index]
            if not row_line.startswith("|"):
                break
            row = split_markdown_row(row_line)
            if len(row) != len(header) or row[feature_index] != update["feature"]:
                continue
            status, _ = parse_matrix_cell(row[platform_index])
            if status != update["from"]:
                raise ValueError(
                    f"matrix cell {update['feature']} / {update['platform_unit']} is {status}, expected {update['from']}"
                )
            row[platform_index] = f"`{matrix_to_cell(update)}`"
            lines[row_index] = "| " + " | ".join(row) + " |"
            return lines
    raise ValueError(f"matrix row not found for {update['feature']} / {update['platform_unit']}")


def matrix_to_cell(update):
    cell = str(update.get("to_cell") or "").strip()
    if cell.startswith("`") and cell.endswith("`"):
        cell = cell[1:-1]
    status, _ = parse_matrix_cell(cell)
    if status != update.get("to") or any(character in cell for character in "\n\r|`"):
        raise ValueError(
            f"matrix to_cell must encode to={update.get('to')}: {update.get('to_cell')}"
        )
    return cell


def artifact_name(role, platform):
    return role if platform is None else f"{platform}-{role}"


def repo_relative_path(path):
    try:
        return path.resolve().relative_to(REPO_ROOT).as_posix()
    except ValueError as exc:
        raise ValueError(f"path must be inside the repository: {path}") from exc


def stable_matrix_path(path):
    resolved = Path(path).resolve()
    try:
        return resolved.relative_to(REPO_ROOT).as_posix()
    except ValueError:
        return str(resolved)


def run_relative_path(path, run_dir):
    return path.resolve().relative_to(run_dir.resolve()).as_posix()


def sha256_file(path):
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def sha256_text(value):
    return hashlib.sha256(value.encode("utf-8")).hexdigest()


def sha256_bytes(value):
    return hashlib.sha256(value).hexdigest()


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)

    init_parser = subparsers.add_parser("init", help="Create a requirement workspace")
    init_parser.add_argument("--matrix", required=True)
    init_parser.add_argument("--feature", help="Requirement feature; defaults to highest-priority gap")
    init_parser.add_argument("--sdk-family", help="Required for a feature outside the matrix backlog")
    init_parser.add_argument("--key-api", action="append", help="Key SDK API; repeat as needed")
    init_parser.add_argument("--target-sdk-version", required=True)
    init_parser.add_argument("--run-dir", required=True)
    init_parser.add_argument("--index", type=int, default=0)
    init_parser.add_argument("--routing-config", default=str(DEFAULT_ROUTING_CONFIG))
    init_parser.add_argument("--repository-profile", default=str(DEFAULT_REPOSITORY_PROFILE))

    dispatch_parser = subparsers.add_parser("dispatch", help="Dispatch one requirement phase")
    dispatch_parser.add_argument("--run-dir", required=True)
    dispatch_parser.add_argument("--phase", required=True, choices=PHASE_ORDER)
    dispatch_parser.add_argument("--platform", action="append", choices=PLATFORMS)
    dispatch_parser.add_argument("--model")
    dispatch_parser.add_argument("--routing-config", default=str(DEFAULT_ROUTING_CONFIG))
    dispatch_parser.add_argument("--codex-bin", default="codex")
    dispatch_parser.add_argument("--timeout-seconds", type=int, default=900)
    dispatch_parser.add_argument("--retry", action="store_true")
    dispatch_parser.add_argument("--dry-run", action="store_true")

    assemble_parser = subparsers.add_parser("assemble", help="Assemble the v4 manifest")
    assemble_parser.add_argument("--run-dir", required=True)
    assemble_parser.add_argument("--matrix", required=True)
    assemble_parser.add_argument(
        "--final-status", choices=["PASS", "PASS WITH RISKS", "BLOCKED"], default="BLOCKED"
    )
    assemble_parser.add_argument(
        "--cross-platform-result", choices=RESULT_VALUES, default="BLOCKED"
    )
    assemble_parser.add_argument(
        "--cross-platform-evidence", default="Pending required platform verification."
    )
    assemble_parser.add_argument("--cross-platform-difference", action="append", default=[])
    args = parser.parse_args(argv)
    try:
        if getattr(args, "timeout_seconds", 1) <= 0:
            raise ValueError("timeout-seconds must be positive")
        if args.command == "init":
            return init_workspace(args)
        if args.command == "dispatch":
            return dispatch_workspace(args)
        if args.command == "assemble":
            return assemble_workspace(args)
    except (OSError, ValueError, KeyError, json.JSONDecodeError) as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        return 1
    return 2


if __name__ == "__main__":
    sys.exit(main())
