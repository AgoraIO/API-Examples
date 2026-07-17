#!/usr/bin/env python3
"""Validate cross-platform API Examples acceptance manifests."""

import hashlib
import json
import re
import shlex
import sys
from pathlib import Path

from generate_case_backlog import PLATFORM_PROJECTS, parse_matrix_cell


REPO_ROOT = Path(__file__).resolve().parents[3]
MANIFEST_VERSION = 4
PLATFORMS = ["android", "ios", "macos", "windows"]
PLATFORM_ROOTS = {
    "android": "Android/",
    "ios": "iOS/",
    "macos": "macOS/",
    "windows": "windows/",
}
FINAL_STATUSES = {"PASS", "PASS WITH RISKS", "BLOCKED"}
RESULT_STATUSES = {"PASS", "FAIL", "BLOCKED", "SKIPPED"}
ROLE_STATUSES = {"PASS", "FAIL", "BLOCKED", "WAIVED"}
MATRIX_STATUSES = {"DONE", "PARTIAL", "MISSING", "N/A", "UNKNOWN"}
REQUIRED_TOP_LEVEL = {
    "version",
    "final_status",
    "requirement",
    "contract",
    "platforms",
    "cross_platform_acceptance",
    "release",
    "knowledge_updates",
}
REQUIREMENT_FIELDS = {"feature", "sdk_family", "key_apis", "target_sdk_version"}
RELEASE_FIELDS = {"required", "target_sdk_version", "checks", "skipped_checks"}
CONTRACT_OUTPUT_FIELDS = {
    "scenario",
    "key_apis",
    "non_goals",
    "reference",
    "cross_platform_requirements",
    "platform_targets",
}
IMPLEMENTATION_OUTPUT_FIELDS = {
    "target_project",
    "query_cases",
    "upsert_case",
    "files_changed",
    "matrix_updates",
}
VERIFICATION_OUTPUT_FIELDS = {
    "result",
    "findings",
    "parity_result",
    "entry_point",
    "ux_notes",
    "commands",
    "build_result",
    "skipped_checks",
}
DISPATCH_MODES = {"codex-exec", "pending"}
COMMAND_KINDS = {"build", "test", "static", "runtime"}
REASONING_EFFORTS = {"none", "minimal", "low", "medium", "high", "xhigh"}
MODEL_PROVENANCE_FIELDS = ["model_profile", "model", "reasoning_effort", "codex_version"]
LEAD_AGENT_IDS = {"lead", "lead-agent", "lead_agent", "main", "main-agent", "main_agent", "coordinator"}
KNOWLEDGE_UPDATE_FIELDS = [
    "source",
    "impact_platforms",
    "symptom",
    "root_cause",
    "guardrail",
    "verification",
    "updated_at",
]
DURABLE_KNOWLEDGE_PATHS = {
    "docs/ai-engineering/knowledge-index.md",
    "docs/ai-engineering/release-iteration-gate.md",
    "docs/ai-engineering/release-known-issues.md",
    "docs/ai-engineering/case-maintenance-matrix.md",
    "docs/ai-engineering/repository-profile.json",
    "docs/ai-engineering/role-routing.json",
}
PLACEHOLDER_RE = re.compile(r"<[^>\n]+>")
SHA256_RE = re.compile(r"^[0-9a-f]{64}$")
SDK_VERSION_RE = re.compile(r"^[0-9]+\.[0-9]+\.[0-9]+$")


def is_non_empty(value):
    return value is not None and value != "" and value != []


def validate_no_placeholders(value, path, errors):
    if isinstance(value, str) and PLACEHOLDER_RE.search(value):
        errors.append(f"{path} contains placeholder value: {value}")
    elif isinstance(value, dict):
        for key, child in value.items():
            validate_no_placeholders(child, f"{path}.{key}", errors)
    elif isinstance(value, list):
        for index, child in enumerate(value):
            validate_no_placeholders(child, f"{path}[{index}]", errors)


def validate_manifest_shape(manifest, errors):
    for key in sorted(REQUIRED_TOP_LEVEL - set(manifest)):
        errors.append(f"missing top-level field: {key}")
    for key in sorted(set(manifest) - REQUIRED_TOP_LEVEL):
        errors.append(f"unsupported top-level field: {key}")
    if manifest.get("version") != MANIFEST_VERSION:
        errors.append(f"version must be {MANIFEST_VERSION}")
    if manifest.get("final_status") not in FINAL_STATUSES:
        errors.append(f"final_status must be one of {sorted(FINAL_STATUSES)}")
    requirement = manifest.get("requirement")
    if not isinstance(requirement, dict):
        errors.append("requirement must be an object")
    else:
        for field in sorted(REQUIREMENT_FIELDS):
            if not is_non_empty(requirement.get(field)):
                errors.append(f"requirement.{field} is required")
        for field in sorted(set(requirement) - REQUIREMENT_FIELDS):
            errors.append(f"unsupported requirement field: {field}")
        target_sdk_version = requirement.get("target_sdk_version")
        if is_non_empty(target_sdk_version) and not SDK_VERSION_RE.fullmatch(
            str(target_sdk_version)
        ):
            errors.append("requirement.target_sdk_version must use x.y.z format")


def validate_manifest(manifest):
    if not isinstance(manifest, dict):
        return ["manifest must be a JSON object"]
    errors = []
    seen_agent_ids = {}
    validate_no_placeholders(manifest, "manifest", errors)
    validate_manifest_shape(manifest, errors)
    targets = validate_contract(manifest, errors, seen_agent_ids)
    validate_platforms(manifest, targets, errors, seen_agent_ids)
    validate_unique_run_ids(manifest, errors)
    validate_cross_platform_acceptance(manifest, errors)
    validate_release(manifest, errors)
    validate_knowledge_updates(manifest, errors)
    return errors


def validate_contract(manifest, errors, seen_agent_ids):
    contract = manifest.get("contract")
    output = validate_artifact(
        contract,
        "contract",
        CONTRACT_OUTPUT_FIELDS,
        manifest.get("final_status"),
        True,
        errors,
        seen_agent_ids,
    )
    if not output:
        return {}
    for field in ["scenario", "key_apis"]:
        if not is_non_empty(output.get(field)):
            errors.append(f"contract.output.{field} is required")
    requirement_apis = manifest.get("requirement", {}).get("key_apis", [])
    contract_apis = output.get("key_apis", [])
    if isinstance(requirement_apis, list) and isinstance(contract_apis, list):
        missing_apis = [api for api in requirement_apis if api not in contract_apis]
        if missing_apis:
            errors.append(
                "contract.output.key_apis must include every requirement.key_apis value: "
                + ", ".join(str(api) for api in missing_apis)
            )
    if contract.get("status") == "PASS" and not is_non_empty(
        output.get("cross_platform_requirements")
    ):
        errors.append("contract.output.cross_platform_requirements is required when Contract passes")
    reference = output.get("reference")
    if not isinstance(reference, dict):
        errors.append("contract.output.reference must be an object")
    else:
        required = bool(reference.get("required"))
        source_case = reference.get("source_case")
        if required:
            if not is_non_empty(source_case):
                errors.append("contract.output.reference.source_case is required")
            elif "://" not in source_case:
                validate_paths([source_case], "contract.output.reference.source_case", errors)
        result = reference.get("contract_result")
        validate_result(result, "contract.output.reference.contract_result", manifest.get("final_status"), errors)
        if required and contract.get("status") == "PASS" and result != "PASS":
            errors.append("reference-required work needs contract.output.reference.contract_result=PASS")

    targets = output.get("platform_targets")
    if not isinstance(targets, dict):
        errors.append("contract.output.platform_targets must be an object")
        return {}
    for platform in PLATFORMS:
        if platform not in targets:
            errors.append(f"contract.output.platform_targets missing platform: {platform}")
            continue
        target = targets[platform]
        if not isinstance(target, dict):
            errors.append(f"contract.output.platform_targets.{platform} must be an object")
            continue
        for field in [
            "required",
            "target_project",
            "key_constraints",
            "files_allowed",
            "waiver_reason",
        ]:
            if field not in target:
                errors.append(f"contract.output.platform_targets.{platform}.{field} is required")
        if target.get("required") is False and not is_non_empty(target.get("waiver_reason")):
            errors.append(
                f"contract.output.platform_targets.{platform}.waiver_reason is required when the platform is not required"
            )
        if contract.get("status") == "PASS" and target.get("required") is True:
            if not is_non_empty(target.get("key_constraints")):
                errors.append(
                    f"contract.output.platform_targets.{platform}.key_constraints is required when Contract passes"
                )
            if not is_non_empty(target.get("files_allowed")):
                errors.append(
                    f"contract.output.platform_targets.{platform}.files_allowed is required when Contract passes"
                )
        target_project = target.get("target_project")
        if is_non_empty(target_project):
            validate_paths(
                [target_project],
                f"contract.output.platform_targets.{platform}.target_project",
                errors,
            )
            if not path_is_within(target_project, PLATFORM_ROOTS[platform]):
                errors.append(
                    f"contract.output.platform_targets.{platform}.target_project must be inside {PLATFORM_ROOTS[platform]}"
                )
        validate_paths(
            target.get("files_allowed", []),
            f"contract.output.platform_targets.{platform}.files_allowed",
            errors,
        )
        allowed_paths = target.get("files_allowed", [])
        for allowed_path in allowed_paths if isinstance(allowed_paths, list) else []:
            if not path_is_within(allowed_path, PLATFORM_ROOTS[platform]):
                errors.append(
                    f"contract.output.platform_targets.{platform}.files_allowed must stay inside {PLATFORM_ROOTS[platform]}: {allowed_path}"
                )
    for platform in sorted(set(targets) - set(PLATFORMS)):
        errors.append(f"contract.output.platform_targets has unsupported platform: {platform}")
    return targets


def validate_platforms(manifest, targets, errors, seen_agent_ids):
    platforms = manifest.get("platforms")
    if not isinstance(platforms, dict):
        errors.append("platforms must be an object")
        return
    for platform in PLATFORMS:
        if platform not in platforms:
            errors.append(f"platforms missing required platform: {platform}")
            continue
        unit = platforms[platform]
        if not isinstance(unit, dict):
            errors.append(f"platforms.{platform} must be an object")
            continue
        target = targets.get(platform, {}) if isinstance(targets, dict) else {}
        required = bool(target.get("required", True))
        implementation = validate_artifact(
            unit.get("implementation"),
            f"platforms.{platform}.implementation",
            IMPLEMENTATION_OUTPUT_FIELDS,
            manifest.get("final_status"),
            required,
            errors,
            seen_agent_ids,
        )
        verification = validate_artifact(
            unit.get("verification"),
            f"platforms.{platform}.verification",
            VERIFICATION_OUTPUT_FIELDS,
            manifest.get("final_status"),
            required,
            errors,
            seen_agent_ids,
        )
        validate_platform_implementation(platform, target, implementation, verification, manifest, errors)
        verification_dispatch = (
            unit.get("verification", {}).get("dispatch", {})
            if isinstance(unit.get("verification"), dict)
            else {}
        )
        validate_platform_verification(
            platform, required, verification, verification_dispatch, manifest, errors
        )
        for role in ["implementation", "verification"]:
            artifact = unit.get(role, {})
            dispatch = artifact.get("dispatch", {}) if isinstance(artifact, dict) else {}
            working_directory = dispatch.get("working_directory")
            expected_working_directory = target.get("target_project")
            if (
                is_non_empty(working_directory)
                and expected_working_directory
                and normalize_manifest_path(working_directory).rstrip("/")
                != normalize_manifest_path(expected_working_directory).rstrip("/")
            ):
                errors.append(
                    f"platforms.{platform}.{role}.dispatch.working_directory must match Contract target {expected_working_directory}"
                )
        if not required:
            for role in ["implementation", "verification"]:
                artifact = unit.get(role, {})
                if artifact.get("status") != "WAIVED":
                    errors.append(f"platforms.{platform}.{role}.status must be WAIVED when platform is not required")
    for platform in sorted(set(platforms) - set(PLATFORMS)):
        errors.append(f"platforms has unsupported platform: {platform}")


def validate_artifact(artifact, path, required_output, final_status, required, errors, seen_agent_ids):
    if not isinstance(artifact, dict):
        errors.append(f"{path} must be an object")
        return {}
    status = artifact.get("status")
    if status not in ROLE_STATUSES:
        errors.append(f"{path}.status must be one of {sorted(ROLE_STATUSES)}")
    elif required and status == "WAIVED":
        errors.append(
            f"{path}.status=WAIVED is only valid when the platform is not required"
        )
    elif required and status in {"FAIL", "BLOCKED"} and final_status != "BLOCKED":
        errors.append(f"{path}.status={status} requires final_status=BLOCKED")
    if status == "WAIVED" and not is_non_empty(artifact.get("waiver_reason")):
        errors.append(f"{path}.waiver_reason is required when status is WAIVED")
    for field in ["agent_id", "evidence", "summary"]:
        if not is_non_empty(artifact.get(field)):
            errors.append(f"{path}.{field} is required")
    agent_id = artifact.get("agent_id")
    if is_non_empty(agent_id):
        normalized = str(agent_id).lower()
        if normalized in LEAD_AGENT_IDS:
            errors.append(f"{path}.agent_id cannot be the Lead Agent")
        elif normalized in seen_agent_ids:
            errors.append(f"{path}.agent_id duplicates {seen_agent_ids[normalized]}.agent_id")
        else:
            seen_agent_ids[normalized] = path
    validate_dispatch(path, artifact.get("dispatch"), status, final_status, required, errors)
    if path.endswith(".implementation") and required and final_status != "BLOCKED":
        dispatch = artifact.get("dispatch", {})
        if not is_non_empty(dispatch.get("repository_delta")):
            errors.append(f"{path}.dispatch.repository_delta is required")
        else:
            validate_run_relative_path(
                dispatch["repository_delta"], f"{path}.dispatch.repository_delta", errors
            )
        value = dispatch.get("repository_delta_sha256")
        if not isinstance(value, str) or not SHA256_RE.fullmatch(value):
            errors.append(f"{path}.dispatch.repository_delta_sha256 must be a lowercase sha256 hex digest")
    output = artifact.get("output")
    if not isinstance(output, dict):
        errors.append(f"{path}.output must be an object")
        return {}
    for field in sorted(required_output - set(output)):
        errors.append(f"{path}.output.{field} is required")
    return output


def validate_dispatch(path, dispatch, status, final_status, required, errors):
    field_name = f"{path}.dispatch"
    if not isinstance(dispatch, dict):
        errors.append(f"{field_name} is required")
        return
    mode = dispatch.get("mode")
    if mode not in DISPATCH_MODES:
        errors.append(f"{field_name}.mode must be one of {sorted(DISPATCH_MODES)}")
    for field in ["prompt", "artifact"]:
        if not is_non_empty(dispatch.get(field)):
            errors.append(f"{field_name}.{field} is required")
        else:
            validate_run_relative_path(dispatch[field], f"{field_name}.{field}", errors)
    if is_non_empty(dispatch.get("input_snapshot")):
        validate_run_relative_path(dispatch["input_snapshot"], f"{field_name}.input_snapshot", errors)
    if required and mode == "pending" and final_status != "BLOCKED" and status != "WAIVED":
        errors.append(f"{field_name}.mode=pending requires final_status=BLOCKED")
    if final_status == "BLOCKED" or status == "WAIVED" or not required:
        return
    if mode != "codex-exec":
        errors.append(f"{field_name}.mode={mode} cannot pass final acceptance")
    for field in [
        "run_id",
        "evidence",
        "input_snapshot",
        "working_directory",
        "command_log",
        "host_platform",
        *MODEL_PROVENANCE_FIELDS,
    ]:
        if not is_non_empty(dispatch.get(field)):
            errors.append(f"{field_name}.{field} is required when final_status is not BLOCKED")
    if is_non_empty(dispatch.get("working_directory")):
        validate_paths(
            [dispatch["working_directory"]], f"{field_name}.working_directory", errors
        )
    if is_non_empty(dispatch.get("command_log")):
        validate_run_relative_path(
            dispatch["command_log"], f"{field_name}.command_log", errors
        )
    effort = dispatch.get("reasoning_effort")
    if is_non_empty(effort) and effort not in REASONING_EFFORTS:
        errors.append(f"{field_name}.reasoning_effort must be one of {sorted(REASONING_EFFORTS)}")
    for field in [
        "prompt_sha256",
        "artifact_sha256",
        "input_snapshot_sha256",
        "command_log_sha256",
    ]:
        value = dispatch.get(field)
        if not is_non_empty(value):
            errors.append(f"{field_name}.{field} is required when final_status is not BLOCKED")
        elif not isinstance(value, str) or not SHA256_RE.fullmatch(value):
            errors.append(f"{field_name}.{field} must be a lowercase sha256 hex digest")


def validate_platform_implementation(platform, target, output, verification, manifest, errors):
    if not output:
        return
    expected_target = target.get("target_project") if isinstance(target, dict) else None
    if expected_target and output.get("target_project") != expected_target:
        errors.append(
            f"platforms.{platform}.implementation.output.target_project must match contract target {expected_target}"
        )
    for field in ["query_cases", "upsert_case"]:
        if not is_non_empty(output.get(field)):
            errors.append(f"platforms.{platform}.implementation.output.{field} is required")
    validate_paths(
        output.get("files_changed", []),
        f"platforms.{platform}.implementation.output.files_changed",
        errors,
        require_exists=False,
    )
    allowed = target.get("files_allowed", []) if isinstance(target, dict) else []
    for changed_path in output.get("files_changed", []):
        if not any(path_is_within(changed_path, allowed_path) for allowed_path in allowed):
            errors.append(
                f"platforms.{platform}.implementation.output.files_changed is outside Contract files_allowed: {changed_path}"
            )
    updates = output.get("matrix_updates", [])
    if not isinstance(updates, list):
        errors.append(f"platforms.{platform}.implementation.output.matrix_updates must be an array")
        return
    for index, update in enumerate(updates):
        path = f"platforms.{platform}.implementation.output.matrix_updates[{index}]"
        if not isinstance(update, dict):
            errors.append(f"{path} must be an object")
            continue
        for field in ["feature", "platform_unit", "from", "to", "to_cell", "evidence"]:
            if not is_non_empty(update.get(field)):
                errors.append(f"{path}.{field} is required")
        if update.get("feature") != manifest.get("requirement", {}).get("feature"):
            errors.append(f"{path}.feature must match requirement.feature")
        target_project = target.get("target_project") if isinstance(target, dict) else ""
        expected_units = {
            unit
            for unit, project in PLATFORM_PROJECTS.items()
            if path_is_within(target_project, project) or path_is_within(project, target_project)
        }
        if update.get("platform_unit") not in expected_units:
            errors.append(
                f"{path}.platform_unit must match Contract target {target_project}; "
                f"expected one of {sorted(expected_units)}"
            )
        if update.get("from") not in MATRIX_STATUSES:
            errors.append(f"{path}.from must be one of {sorted(MATRIX_STATUSES)}")
        if update.get("to") not in MATRIX_STATUSES:
            errors.append(f"{path}.to must be one of {sorted(MATRIX_STATUSES)}")
        to_cell = update.get("to_cell")
        if is_non_empty(to_cell):
            cell_status, _ = parse_matrix_cell(to_cell)
            if cell_status != update.get("to"):
                errors.append(f"{path}.to_cell status must match to={update.get('to')}")
            if any(character in str(to_cell) for character in "\n\r|`"):
                errors.append(f"{path}.to_cell contains unsupported Markdown characters")
        if update.get("to") == "DONE":
            if manifest.get("final_status") != "PASS":
                errors.append(f"{update.get('platform_unit')} matrix update to DONE requires final_status=PASS")
            if verification.get("build_result") != "PASS":
                errors.append(
                    f"{update.get('platform_unit')} matrix update to DONE requires build_result=PASS"
                )
            if verification.get("parity_result") != "PASS":
                errors.append(
                    f"{update.get('platform_unit')} matrix update to DONE requires parity_result=PASS"
                )
            if verification.get("skipped_checks"):
                errors.append(f"{update.get('platform_unit')} matrix update to DONE cannot include skipped checks")
    if isinstance(target, dict) and target.get("required") is False:
        if output.get("files_changed"):
            errors.append(f"platforms.{platform}.implementation.output.files_changed must be empty when platform is not required")
        if updates:
            errors.append(f"platforms.{platform}.implementation.output.matrix_updates must be empty when platform is not required")


def is_platform_build_command(platform, command, working_directory=None):
    if (
        platform not in PLATFORM_ROOTS
        or not isinstance(command, str)
        or not is_non_empty(working_directory)
        or not path_is_within(working_directory, PLATFORM_ROOTS[platform])
    ):
        return False
    try:
        outer_tokens = shlex.split(command)
    except ValueError:
        return False
    if not outer_tokens:
        return False
    executable = outer_tokens[0].replace("\\", "/").rsplit("/", 1)[-1].lower()
    if executable in {"sh", "bash", "zsh"}:
        command_option = next(
            (
                index
                for index, token in enumerate(outer_tokens[:-1])
                if token.startswith("-") and "c" in token
            ),
            None,
        )
        if command_option is None:
            return False
        command = outer_tokens[command_option + 1]
    elif executable in {"cmd", "cmd.exe", "powershell", "powershell.exe", "pwsh", "pwsh.exe"}:
        command_option = next(
            (
                index
                for index, token in enumerate(outer_tokens[:-1])
                if token.lower() in {"/c", "-command", "-c"}
            ),
            None,
        )
        if command_option is None:
            return False
        command = " ".join(outer_tokens[command_option + 1 :])

    try:
        lexer = shlex.shlex(command, posix=True, punctuation_chars=";&|")
        lexer.whitespace_split = True
        lexer.commenters = ""
        tokens = list(lexer)
    except ValueError:
        return False
    segments = [[]]
    for token in tokens:
        if token in {";", "|", "||", "&"}:
            return False
        if token == "&&":
            if not segments[-1]:
                return False
            segments.append([])
        else:
            segments[-1].append(token)
    if not segments[-1]:
        return False
    if len(segments) != 1:
        return False
    final = segments[-1]
    if command_overrides_path(final):
        return False
    remaining = command_tokens_without_environment(final)
    if not remaining:
        return False
    raw_executable = remaining[0].replace("\\", "/").lower()
    final_executable, arguments = command_executable(final), command_arguments(final)
    if platform == "android":
        if raw_executable not in {"./gradlew", "gradlew.bat", "./gradlew.bat"}:
            return False
        normalized_arguments = [argument.lower() for argument in arguments]
        blocked_modes = {
            "--dry-run",
            "-m",
            "help",
            "tasks",
            "properties",
            "projects",
            "dependencies",
            "components",
            "model",
            "buildenvironment",
        }
        if blocked_modes.intersection(normalized_arguments) or any(
            argument.startswith("--dry-run=") for argument in normalized_arguments
        ):
            return False
        if any(
            argument in {"-p", "--project-dir"} or argument.startswith("--project-dir=")
            for argument in normalized_arguments
        ):
            return False
        return final_executable in {"gradlew", "gradlew.bat"} and any(
            re.search(r"(?:^|:)(?:assemble|build|bundle)[A-Za-z0-9_.-]*$", argument, re.I)
            for argument in arguments
        )
    if platform in {"ios", "macos"}:
        if raw_executable not in {"xcodebuild", "/usr/bin/xcodebuild"}:
            return False
        normalized_arguments = [argument.lower() for argument in arguments]
        if not xcodebuild_has_only_build_arguments(arguments):
            return False
        for option in ["-project", "-workspace"]:
            if option in normalized_arguments:
                index = normalized_arguments.index(option)
                if index + 1 >= len(arguments) or not is_safe_relative_build_path(
                    arguments[index + 1]
                ):
                    return False
        build_actions = {"build", "archive", "test", "build-for-testing"}
        normalized_argument_set = set(normalized_arguments)
        if normalized_argument_set & build_actions:
            return True
        if "clean" in normalized_argument_set:
            return False
        return bool(normalized_argument_set & {"-project", "-workspace", "-scheme"})
    if final_executable in {"msbuild", "msbuild.exe", "devenv", "devenv.com", "devenv.exe"}:
        if "/" in raw_executable:
            return False
        normalized_arguments = [argument.lower() for argument in arguments]
        if any(
            argument in {"-version", "/version", "-help", "/help", "/?"}
            for argument in normalized_arguments
        ):
            return False
        blocked_prefixes = (
            "/preprocess",
            "-preprocess",
            "/pp",
            "-pp",
            "/getproperty",
            "-getproperty",
            "/getitem",
            "-getitem",
            "/gettargetresult",
            "-gettargetresult",
            "/targets",
            "-targets",
            "/ts",
            "-ts",
            "/validate",
            "-validate",
            "/val",
            "-val",
        )
        if any(argument.startswith(blocked_prefixes) for argument in normalized_arguments):
            return False
        project_arguments = [
            arguments[index]
            for index, argument in enumerate(normalized_arguments)
            if argument.endswith((".sln", ".slnx", ".vcxproj", ".proj"))
        ]
        if not project_arguments or not all(
            is_safe_relative_build_path(argument) for argument in project_arguments
        ):
            return False
        if final_executable.startswith("devenv"):
            return any(
                argument in {"/build", "/rebuild"} for argument in normalized_arguments
            )
        targets = [
            argument.split(":", 1)[1]
            for argument in normalized_arguments
            if argument.startswith(("/t:", "-t:", "/target:", "-target:"))
        ]
        if targets:
            target_names = {
                name
                for value in targets
                for name in re.split(r"[;,]", value)
                if name
            }
            return bool(target_names & {"build", "rebuild"})
        return True
    if (
        raw_executable not in {"cmake", "cmake.exe"}
        or final_executable not in {"cmake", "cmake.exe"}
        or "--build" not in arguments
    ):
        return False
    build_index = arguments.index("--build")
    if build_index + 1 >= len(arguments) or not is_safe_relative_build_path(
        arguments[build_index + 1]
    ):
        return False
    normalized_arguments = [argument.lower() for argument in arguments]
    if "--target" in normalized_arguments:
        target_index = normalized_arguments.index("--target")
        targets = normalized_arguments[target_index + 1 :]
        if targets and all(target in {"clean", "help"} for target in targets):
            return False
    return True


def is_safe_relative_build_path(path_text):
    if not isinstance(path_text, str) or not path_text:
        return False
    normalized = path_text.replace("\\", "/")
    path = Path(normalized)
    return (
        not path.is_absolute()
        and not re.match(r"^[A-Za-z]:/", normalized)
        and ".." not in path.parts
    )


def xcodebuild_has_only_build_arguments(arguments):
    actions = {"build", "archive", "test", "build-for-testing", "clean"}
    options_with_values = {
        "-project",
        "-workspace",
        "-scheme",
        "-target",
        "-configuration",
        "-destination",
        "-arch",
        "-sdk",
        "-xcconfig",
        "-deriveddatapath",
        "-archivepath",
        "-resultbundlepath",
        "-resultstreampath",
        "-packagecachepath",
        "-clonedsourcepackagesdirpath",
        "-jobs",
        "-testplan",
        "-only-testing",
        "-skip-testing",
        "-enablecodecoverage",
        "-parallel-testing-enabled",
        "-maximum-concurrent-test-device-destinations",
        "-maximum-concurrent-test-simulator-destinations",
    }
    flags = {
        "-alltargets",
        "-parallelizetargets",
        "-quiet",
        "-verbose",
        "-hideshellscriptenvironment",
        "-allowprovisioningupdates",
        "-allowprovisioningdeviceregistration",
        "-skipunavailableactions",
        "-disableautomaticpackageresolution",
        "-onlyusepackageversionsfromresolvedfile",
        "-skippackageupdates",
        "-skippackagepluginvalidation",
        "-skipmacrovalidation",
        "-showbuildtimingsummary",
    }
    index = 0
    while index < len(arguments):
        argument = arguments[index]
        normalized = argument.lower()
        if normalized in actions or normalized in flags or (
            "=" in argument and not argument.startswith("-")
        ):
            index += 1
            continue
        if normalized in options_with_values:
            if index + 1 >= len(arguments):
                return False
            index += 2
            continue
        return False
    return True


def command_tokens_without_environment(tokens):
    remaining = list(tokens)
    if remaining and remaining[0] == "env":
        remaining.pop(0)
        while remaining and (remaining[0].startswith("-") or "=" in remaining[0]):
            remaining.pop(0)
    while remaining and "=" in remaining[0] and not remaining[0].startswith(("/", "./")):
        remaining.pop(0)
    return remaining


def command_overrides_path(tokens):
    remaining = list(tokens)
    if remaining and remaining[0] == "env":
        remaining.pop(0)
        while remaining and remaining[0].startswith("-"):
            remaining.pop(0)
    for token in remaining:
        if "=" not in token or token.startswith(("/", "./")):
            break
        name, _, _ = token.partition("=")
        if name.upper() == "PATH":
            return True
    return False


def command_executable(tokens):
    remaining = command_tokens_without_environment(tokens)
    if not remaining:
        return ""
    return remaining[0].replace("\\", "/").rsplit("/", 1)[-1].lower()


def command_arguments(tokens):
    remaining = command_tokens_without_environment(tokens)
    return remaining[1:] if remaining else []


def validate_platform_verification(
    platform, required, output, dispatch, manifest, errors
):
    if not output:
        return
    dispatch = dispatch if isinstance(dispatch, dict) else {}
    command_log = dispatch.get("command_log")
    host_platform = dispatch.get("host_platform")
    for field in ["entry_point", "ux_notes"]:
        if required and not is_non_empty(output.get(field)):
            errors.append(f"platforms.{platform}.verification.output.{field} is required")
    for field in ["result", "parity_result", "build_result"]:
        validate_result(
            output.get(field),
            f"platforms.{platform}.verification.output.{field}",
            manifest.get("final_status"),
            errors,
            allow_skipped=not required,
        )
    commands = output.get("commands", [])
    if not required:
        for field in ["result", "parity_result", "build_result"]:
            if output.get(field) != "SKIPPED":
                errors.append(
                    f"platforms.{platform}.verification.output.{field} must be SKIPPED when platform is not required"
                )
        if commands:
            errors.append(
                f"platforms.{platform}.verification.output.commands must be empty when platform is not required"
            )
    if required and manifest.get("final_status") != "BLOCKED" and not commands:
        errors.append(f"platforms.{platform}.verification.output.commands must not be empty")
    for index, command in enumerate(commands):
        if not isinstance(command, dict):
            errors.append(f"platforms.{platform}.verification.output.commands[{index}] must be an object")
            continue
        if not command.get("command"):
            errors.append(f"platforms.{platform}.verification.output.commands[{index}].command is required")
        if command.get("kind") not in COMMAND_KINDS:
            errors.append(
                f"platforms.{platform}.verification.output.commands[{index}].kind must be one of {sorted(COMMAND_KINDS)}"
            )
        elif command.get("kind") == "build" and not is_platform_build_command(
            platform, command.get("command"), dispatch.get("working_directory")
        ):
            errors.append(
                f"platforms.{platform}.verification.output.commands[{index}] is not a recognized {platform} build command"
            )
        validate_result(
            command.get("result"),
            f"platforms.{platform}.verification.output.commands[{index}].result",
            manifest.get("final_status"),
            errors,
        )
        if command.get("result") in {"PASS", "FAIL"} and not is_non_empty(
            command.get("evidence")
        ):
            errors.append(
                f"platforms.{platform}.verification.output.commands[{index}].evidence is required for executed commands"
            )
        elif command.get("result") in {"PASS", "FAIL"} and (
            not command_log
            or not str(command.get("evidence")).startswith(f"{command_log}#")
            or "exit_code=" not in str(command.get("evidence"))
        ):
            errors.append(
                f"platforms.{platform}.verification.output.commands[{index}].evidence must bind to dispatch.command_log and exit_code"
            )
    for index, skipped in enumerate(output.get("skipped_checks", [])):
        if not isinstance(skipped, dict) or not skipped.get("reason"):
            errors.append(
                f"platforms.{platform}.verification.output.skipped_checks[{index}].reason is required"
            )
    if required and manifest.get("final_status") != "BLOCKED":
        for field in ["result", "parity_result", "build_result"]:
            if output.get(field) != "PASS":
                errors.append(
                    f"non-BLOCKED acceptance requires platforms.{platform}.verification.output.{field}=PASS"
                )
        if output.get("build_result") == "PASS" and not any(
            command.get("kind") == "build" and command.get("result") == "PASS"
            for command in commands
            if isinstance(command, dict)
        ):
            errors.append(
                f"platforms.{platform}.verification.output.build_result=PASS requires a bound PASS build command"
            )
    if platform == "windows" and output.get("build_result") == "PASS" and host_platform != "win32":
        errors.append(
            "platforms.windows.verification.output.build_result=PASS requires host_platform=win32"
        )
    if required and manifest.get("final_status") == "PASS" and output.get("skipped_checks"):
        errors.append("final_status=PASS cannot include skipped checks")


def validate_cross_platform_acceptance(manifest, errors):
    acceptance = manifest.get("cross_platform_acceptance")
    if not isinstance(acceptance, dict):
        errors.append("cross_platform_acceptance must be an object")
        return
    result = acceptance.get("result")
    validate_result(result, "cross_platform_acceptance.result", manifest.get("final_status"), errors)
    if not is_non_empty(acceptance.get("evidence")):
        errors.append("cross_platform_acceptance.evidence is required")
    if manifest.get("final_status") != "BLOCKED" and result != "PASS":
        errors.append("non-BLOCKED acceptance requires cross_platform_acceptance.result=PASS")


def validate_result(result, field_name, final_status, errors, allow_skipped=False):
    if result not in RESULT_STATUSES:
        errors.append(f"{field_name} must be one of {sorted(RESULT_STATUSES)}")
    elif result in {"FAIL", "BLOCKED"} and final_status != "BLOCKED":
        errors.append(f"{field_name}={result} requires final_status=BLOCKED")
    elif result == "SKIPPED" and final_status == "PASS" and not allow_skipped:
        errors.append(f"{field_name}=SKIPPED cannot use final_status=PASS")


def validate_release(manifest, errors):
    release = manifest.get("release")
    if not isinstance(release, dict):
        errors.append("release must be an object")
        return
    if release.get("required") is not True:
        errors.append("release.required must be true")
    for field in sorted(set(release) - RELEASE_FIELDS):
        errors.append(f"unsupported release field: {field}")
    requirement = manifest.get("requirement", {})
    target_sdk_version = requirement.get("target_sdk_version")
    if release.get("target_sdk_version") != target_sdk_version:
        errors.append("release.target_sdk_version must match requirement.target_sdk_version")
    checks = release.get("checks", [])
    if not checks:
        errors.append("release.checks must include SDK version checks")
    check_names = {
        check.get("name") for check in checks if isinstance(check, dict) and check.get("name")
    }
    expected_sdk_checks = {f"sdk-version-{platform}" for platform in PLATFORMS}
    for name in sorted(expected_sdk_checks - check_names):
        errors.append(f"release.checks missing required check: {name}")
    for index, check in enumerate(checks):
        if not isinstance(check, dict):
            errors.append(f"release.checks[{index}] must be an object")
            continue
        if not check.get("name"):
            errors.append(f"release.checks[{index}].name is required")
        result = check.get("result")
        validate_result(result, f"release.checks[{index}].result", manifest.get("final_status"), errors)
        if result == "PASS" and not check.get("evidence"):
            errors.append(f"release.checks[{index}].evidence is required when result is PASS")
        if result == "SKIPPED" and not check.get("reason"):
            errors.append(f"release.checks[{index}].reason is required when result is SKIPPED")
        if result in {"FAIL", "BLOCKED"} and not check.get("reason"):
            errors.append(f"release.checks[{index}].reason is required when result is {result}")
        if check.get("name") in expected_sdk_checks:
            if check.get("expected_version") != target_sdk_version:
                errors.append(
                    f"{check.get('name')} expected_version must match {target_sdk_version}"
                )
            actual_versions = check.get("actual_versions")
            if not isinstance(actual_versions, dict) or not actual_versions:
                errors.append(f"{check.get('name')} actual_versions must be a non-empty object")
            elif result == "PASS" and any(
                version != target_sdk_version for version in actual_versions.values()
            ):
                errors.append(
                    f"{check.get('name')} actual versions must all match {target_sdk_version}"
                )
            if manifest.get("final_status") != "BLOCKED" and result != "PASS":
                errors.append(
                    f"non-BLOCKED acceptance requires {check.get('name')}=PASS"
                )
    for index, skipped in enumerate(release.get("skipped_checks", [])):
        if not isinstance(skipped, dict) or not skipped.get("reason"):
            errors.append(f"release.skipped_checks[{index}].reason is required")
    if manifest.get("final_status") != "BLOCKED" and release.get("skipped_checks"):
        errors.append("non-BLOCKED acceptance cannot include skipped release checks")

def validate_knowledge_updates(manifest, errors):
    updates = manifest.get("knowledge_updates", [])
    for index, update in enumerate(updates):
        if not isinstance(update, dict):
            errors.append(f"knowledge_updates[{index}] must be an object")
            continue
        for field in KNOWLEDGE_UPDATE_FIELDS:
            if not is_non_empty(update.get(field)):
                errors.append(f"knowledge_updates[{index}].{field} is required")
    changed_files = []
    platforms = manifest.get("platforms", {})
    if isinstance(platforms, dict):
        for platform in PLATFORMS:
            implementation = platforms.get(platform, {}).get("implementation", {})
            output = implementation.get("output", {}) if isinstance(implementation, dict) else {}
            changed_files.extend(output.get("files_changed", []) if isinstance(output, dict) else [])
    if updates and not any(is_durable_knowledge_path(path) for path in changed_files):
        errors.append("knowledge_updates require at least one durable knowledge doc or skill in files_changed")


def validate_run_relative_path(path_text, field_name, errors):
    if not isinstance(path_text, str):
        errors.append(f"{field_name} must be a string")
        return
    path = Path(path_text)
    if path.is_absolute() or ".." in path.parts:
        errors.append(f"{field_name} must be relative to the execution workspace")


def validate_unique_run_ids(manifest, errors):
    seen = {}
    for artifact_path, artifact in iter_manifest_artifacts(manifest):
        dispatch = artifact.get("dispatch")
        if not isinstance(dispatch, dict) or dispatch.get("mode") != "codex-exec":
            continue
        run_id = dispatch.get("run_id")
        if not is_non_empty(run_id):
            continue
        normalized = str(run_id).lower()
        if normalized in seen:
            errors.append(
                f"{artifact_path}.dispatch.run_id duplicates {seen[normalized]}.dispatch.run_id"
            )
        else:
            seen[normalized] = artifact_path


def resolve_repo_path(path_text):
    path = Path(path_text)
    resolved = path.resolve() if path.is_absolute() else (REPO_ROOT / path).resolve()
    try:
        resolved.relative_to(REPO_ROOT.resolve())
    except ValueError:
        return None
    return resolved


def validate_paths(values, field_name, errors, require_exists=True):
    if not isinstance(values, list):
        errors.append(f"{field_name} must be an array")
        return
    for path_text in values:
        if not isinstance(path_text, str) or not path_text:
            errors.append(f"{field_name} contains an empty or non-string path")
            continue
        if "://" in path_text:
            continue
        resolved = resolve_repo_path(path_text)
        if resolved is None:
            errors.append(f"{field_name} path must be inside the repository: {path_text}")
        elif require_exists and not resolved.exists():
            errors.append(f"{field_name} path does not exist: {path_text}")


def is_durable_knowledge_path(path_text):
    if not isinstance(path_text, str):
        return False
    normalized = normalize_manifest_path(path_text)
    return (
        normalized in DURABLE_KNOWLEDGE_PATHS
        or normalized.endswith("/ARCHITECTURE.md")
        or normalized.startswith(".agent/skills/")
        or "/.agent/skills/" in normalized
        or normalized.startswith(".agents/skills/")
        or "/.agents/skills/" in normalized
    )


def normalize_manifest_path(path_text):
    normalized = str(path_text).replace("\\", "/")
    path = Path(normalized)
    if path.is_absolute():
        try:
            return path.resolve().relative_to(REPO_ROOT).as_posix()
        except ValueError:
            return path.as_posix()
    try:
        return (REPO_ROOT / path).resolve().relative_to(REPO_ROOT.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


def path_is_within(path_text, allowed_text):
    path = normalize_manifest_path(path_text).rstrip("/")
    allowed = normalize_manifest_path(allowed_text).rstrip("/")
    return path == allowed or path.startswith(f"{allowed}/")


def iter_manifest_artifacts(manifest):
    contract = manifest.get("contract")
    if isinstance(contract, dict):
        yield "contract", contract
    platforms = manifest.get("platforms")
    if not isinstance(platforms, dict):
        return
    for platform in PLATFORMS:
        unit = platforms.get(platform)
        if not isinstance(unit, dict):
            continue
        for role in ["implementation", "verification"]:
            artifact = unit.get(role)
            if isinstance(artifact, dict):
                yield f"platforms.{platform}.{role}", artifact


def sha256_file(path):
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def resolve_evidence_path(root, path_text):
    if not isinstance(path_text, str):
        return None
    path = Path(path_text)
    if path.is_absolute() or ".." in path.parts:
        return None
    resolved = (root / path).resolve()
    try:
        resolved.relative_to(root.resolve())
    except ValueError:
        return None
    return resolved


def validate_evidence_files(manifest, evidence_root):
    errors = []
    root = Path(evidence_root).resolve()
    fields = [
        ("prompt", "prompt_sha256"),
        ("artifact", "artifact_sha256"),
        ("input_snapshot", "input_snapshot_sha256"),
        ("command_log", "command_log_sha256"),
        ("repository_delta", "repository_delta_sha256"),
    ]
    for artifact_path, artifact in iter_manifest_artifacts(manifest):
        dispatch = artifact.get("dispatch")
        if not isinstance(dispatch, dict) or dispatch.get("mode") != "codex-exec":
            continue
        for path_field, hash_field in fields:
            path_text = dispatch.get(path_field)
            expected = dispatch.get(hash_field)
            if not is_non_empty(path_text) or not is_non_empty(expected):
                continue
            path = resolve_evidence_path(root, path_text)
            field_name = f"{artifact_path}.dispatch.{path_field}"
            if path is None:
                errors.append(f"{field_name} must resolve inside the execution workspace")
            elif not path.is_file():
                errors.append(f"{field_name} does not exist: {path_text}")
            elif sha256_file(path) != expected:
                errors.append(
                    f"{artifact_path}.dispatch.{hash_field} does not match {path_text}"
                )
            elif path_field == "repository_delta":
                try:
                    delta = json.loads(path.read_text(encoding="utf-8"))
                except (OSError, json.JSONDecodeError) as exc:
                    errors.append(f"{field_name} is not valid JSON: {exc}")
                    continue
                if not isinstance(delta, dict):
                    errors.append(f"{field_name} must contain a JSON object")
                    continue
                delta_files = {
                    normalize_manifest_path(value)
                    for value in delta.get("changed_files", [])
                    if isinstance(value, str)
                }
                output = artifact.get("output", {})
                artifact_files = {
                    normalize_manifest_path(value)
                    for value in output.get("files_changed", [])
                    if isinstance(value, str)
                }
                if delta_files != artifact_files:
                    errors.append(
                        f"{artifact_path}.dispatch.repository_delta changed_files do not match output.files_changed"
                    )
    return errors


def main(argv):
    if len(argv) != 2:
        print("usage: validate_acceptance_manifest.py <manifest.json>", file=sys.stderr)
        return 2
    manifest_path = Path(argv[1])
    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    except OSError as exc:
        print(f"failed to read manifest: {exc}", file=sys.stderr)
        return 2
    except json.JSONDecodeError as exc:
        print(f"invalid JSON: {exc}", file=sys.stderr)
        return 2
    errors = validate_manifest(manifest)
    errors.extend(validate_evidence_files(manifest, manifest_path.parent))
    if errors:
        for error in errors:
            print(f"ERROR: {error}", file=sys.stderr)
        return 1
    print("Acceptance manifest valid")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
