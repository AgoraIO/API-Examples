#!/usr/bin/env python3
"""Prepare one cross-platform API Examples requirement package."""

import argparse
import json
import re
import sys
from pathlib import Path

from generate_case_backlog import DEFAULT_MATRIX, REPO_ROOT, generate_execution_units


PLATFORMS = ["android", "ios", "macos", "windows"]
DEFAULT_REPOSITORY_PROFILE = REPO_ROOT / "docs/ai-engineering/repository-profile.json"
PROFILE_SOURCE_FIELDS = {
    "gradle-property": {"path", "kind", "key"},
    "cocoapods": {"path", "kind", "package"},
    "archive-name": {"path", "kind", "prefix", "suffix"},
}
SEMVER_CAPTURE = r"([0-9]+\.[0-9]+\.[0-9]+)"
SDK_VERSION_RE = re.compile(r"^[0-9]+\.[0-9]+\.[0-9]+$")
DEFAULT_PLATFORM_TARGETS = {
    "android": "Android/APIExample/",
    "ios": "iOS/APIExample/",
    "macos": "macOS/",
    "windows": "windows/",
}
PLATFORM_UNIT_GROUPS = {
    "Android full": "android",
    "Android audio": "android",
    "Android Compose": "android",
    "iOS UIKit": "ios",
    "iOS SwiftUI": "ios",
    "iOS Objective-C": "ios",
    "iOS audio": "ios",
    "macOS": "macos",
    "Windows": "windows",
}
ROLE_CONTRACTS = {
    "contract": [
        "shared scenario, APIs, and non-goals",
        "cross-platform behavior contract",
        "target project and allowed files for every platform",
        "reference contract when required",
    ],
    "implementation": [
        "target project",
        "query-cases and upsert-case results",
        "changed files",
        "matrix update proposals",
    ],
    "verification": [
        "findings-first review and parity result",
        "entry point and UX consistency",
        "exact target command strings and command kinds for JSONL evidence binding",
        "target build commands and result",
        "skipped checks with reasons",
    ],
}


def select_execution_unit(units, feature=None, index=0):
    filtered = [unit for unit in units if feature is None or unit["feature"] == feature]
    if not filtered:
        suffix = f" for feature={feature}" if feature else ""
        raise ValueError(f"no execution units found{suffix}")
    if index < 0 or index >= len(filtered):
        raise ValueError(f"execution unit index {index} is out of range; {len(filtered)} unit(s) available")
    return filtered[index]


def source_case_from_candidate(candidate):
    if not candidate:
        return ""
    return f"{candidate['project'].rstrip('/')}/{candidate['path'].lstrip('/')}"


def resolve_source_case(candidate):
    source_case = source_case_from_candidate(candidate)
    if not source_case:
        return ""
    direct_path = REPO_ROOT / source_case
    if direct_path.exists():
        return source_case
    project_root = REPO_ROOT / candidate["project"]
    basename = Path(candidate["path"].rstrip("/")).name
    if not project_root.exists() or not basename:
        return source_case
    matches = [
        path
        for path in project_root.rglob(basename)
        if "build" not in path.relative_to(project_root).parts
    ]
    if not matches:
        return source_case
    matches.sort(key=lambda path: (len(path.parts), path.as_posix()))
    return matches[0].relative_to(REPO_ROOT).as_posix()


def pending_dispatch(name):
    return {
        "mode": "pending",
        "prompt": f"role-prompts/{name}.md",
        "artifact": f"role-artifacts/{name}.json",
    }


def pending_artifact(name, summary, output):
    return {
        "agent_id": f"{name}-agent-pending",
        "dispatch": pending_dispatch(name),
        "status": "BLOCKED",
        "evidence": f"Pending {name} gate.",
        "summary": summary,
        "output": output,
    }


def default_platform_targets():
    return {
        platform: {
            "required": True,
            "target_project": target,
            "key_constraints": [
                f"Use the {platform} project rules and keep source changes inside {target}"
            ],
            "files_allowed": [target],
            "waiver_reason": "",
        }
        for platform, target in DEFAULT_PLATFORM_TARGETS.items()
    }


def load_repository_profile(profile_path=DEFAULT_REPOSITORY_PROFILE):
    path = Path(profile_path)
    profile = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(profile, dict):
        raise ValueError("repository profile must be a JSON object")
    if set(profile) != {"version", "sdk_version_sources"}:
        raise ValueError("repository profile must contain only version and sdk_version_sources")
    if profile.get("version") != 1:
        raise ValueError("repository profile version must be 1")
    sources = profile.get("sdk_version_sources")
    if not isinstance(sources, dict) or set(sources) != set(PLATFORMS):
        raise ValueError("repository profile must define SDK version sources for every platform")
    for platform in PLATFORMS:
        entries = sources[platform]
        if not isinstance(entries, list) or not entries:
            raise ValueError(f"repository profile {platform} SDK version sources must be non-empty")
        seen_paths = set()
        for index, source in enumerate(entries):
            if not isinstance(source, dict):
                raise ValueError(f"repository profile {platform} source {index} must be an object")
            kind = source.get("kind")
            expected_fields = PROFILE_SOURCE_FIELDS.get(kind)
            if expected_fields is None:
                raise ValueError(
                    f"repository profile {platform} source {index} has unknown kind {kind}"
                )
            if set(source) != expected_fields:
                raise ValueError(
                    f"repository profile {platform} source {index} must contain "
                    + ", ".join(sorted(expected_fields))
                )
            if not all(
                isinstance(source[field], str) and source[field] for field in expected_fields
            ):
                raise ValueError(
                    f"repository profile {platform} source {index} fields must be non-empty strings"
                )
            source_path = Path(source["path"])
            if source_path.is_absolute() or ".." in source_path.parts:
                raise ValueError(
                    "repository profile source path must be repository-relative: "
                    + source["path"]
                )
            normalized_path = source_path.as_posix()
            if normalized_path in seen_paths:
                raise ValueError(
                    f"repository profile contains duplicate source path: {normalized_path}"
                )
            seen_paths.add(normalized_path)
    return profile


def resolve_target_sdk_versions(target_sdk_version, platform_sdk_versions=None):
    if not target_sdk_version:
        raise ValueError("target_sdk_version is required")
    if not SDK_VERSION_RE.fullmatch(str(target_sdk_version)):
        raise ValueError("target_sdk_version must use x.y.z format")
    versions = {platform: target_sdk_version for platform in PLATFORMS}
    seen = set()
    for value in platform_sdk_versions or []:
        if not isinstance(value, str) or value.count("=") != 1:
            raise ValueError("platform SDK version override must use platform=x.y.z")
        platform, version = value.split("=", 1)
        if platform not in PLATFORMS:
            raise ValueError(f"unknown platform: {platform}")
        if platform in seen:
            raise ValueError(f"duplicate platform SDK version: {platform}")
        if not SDK_VERSION_RE.fullmatch(version):
            raise ValueError(f"{platform} SDK version must use x.y.z format")
        seen.add(platform)
        versions[platform] = version
    return versions


def sdk_version_pattern(source):
    kind = source["kind"]
    if kind == "gradle-property":
        return rf"(?m)^\s*{re.escape(source['key'])}\s*=\s*{SEMVER_CAPTURE}\s*$"
    if kind == "cocoapods":
        package = re.escape(source["package"])
        return rf"pod\s+['\"]{package}['\"]\s*,\s*['\"]{SEMVER_CAPTURE}['\"]"
    if kind == "archive-name":
        return re.escape(source["prefix"]) + SEMVER_CAPTURE + re.escape(source["suffix"])
    raise ValueError(f"unsupported SDK version source kind: {kind}")


def collect_sdk_version_checks(
    target_sdk_versions,
    repo_root=REPO_ROOT,
    sources=None,
    profile_path=DEFAULT_REPOSITORY_PROFILE,
):
    if sources is None:
        sources = load_repository_profile(profile_path)["sdk_version_sources"]
    checks = []
    for platform in PLATFORMS:
        target_sdk_version = target_sdk_versions[platform]
        entries = sources[platform]
        actual_versions = {}
        problems = []
        for source in entries:
            path_text = source["path"]
            path = Path(repo_root) / path_text
            if not path.exists():
                actual_versions[path_text] = ""
                problems.append(f"missing {path_text}")
                continue
            pattern = sdk_version_pattern(source)
            matches = sorted(set(re.findall(pattern, path.read_text(encoding="utf-8"))))
            if len(matches) != 1:
                actual_versions[path_text] = ""
                problems.append(f"expected one SDK version in {path_text}, found {len(matches)}")
                continue
            actual_versions[path_text] = matches[0]
            if matches[0] != target_sdk_version:
                problems.append(f"{path_text}={matches[0]}")
        result = "PASS" if not problems else "BLOCKED"
        checks.append(
            {
                "name": f"sdk-version-{platform}",
                "result": result,
                "expected_version": target_sdk_version,
                "actual_versions": actual_versions,
                "evidence": (
                    "; ".join(f"{path}={version}" for path, version in actual_versions.items())
                    if actual_versions
                    else ""
                ),
                "reason": "" if not problems else f"Expected {target_sdk_version}; " + "; ".join(problems),
            }
        )
    return checks


def build_manifest_seed(requirement, source_case, version_sources):
    reference_required = bool(source_case)
    reference_result = "BLOCKED" if reference_required else "SKIPPED"
    targets = default_platform_targets()
    contract = pending_artifact(
        "contract",
        "Pending shared product, architecture, and reference contract.",
        {
            "scenario": f"Implement {requirement['feature']} consistently across official platforms.",
            "key_apis": requirement["key_apis"],
            "non_goals": [],
            "reference": {
                "required": reference_required,
                "source_case": source_case,
                "contract_result": reference_result,
            },
            "cross_platform_requirements": [],
            "platform_targets": targets,
        },
    )
    platforms = {}
    for platform, target in DEFAULT_PLATFORM_TARGETS.items():
        platforms[platform] = {
            "implementation": pending_artifact(
                f"{platform}-implementation",
                f"Pending {platform} implementation.",
                {
                    "target_project": target,
                    "query_cases": "Pending target project case query.",
                    "upsert_case": "Pending target project update.",
                    "files_changed": [],
                    "matrix_updates": [],
                },
            ),
            "verification": pending_artifact(
                f"{platform}-verification",
                f"Pending independent {platform} verification.",
                {
                    "result": "BLOCKED",
                    "findings": [],
                    "parity_result": reference_result,
                    "entry_point": f"Pending {platform} entry point verification.",
                    "ux_notes": "Pending verification.",
                    "commands": [],
                    "build_result": "BLOCKED",
                    "skipped_checks": [],
                },
            ),
        }
    return {
        "version": 4,
        "final_status": "BLOCKED",
        "requirement": requirement,
        "contract": contract,
        "platforms": platforms,
        "cross_platform_acceptance": {
            "result": "BLOCKED",
            "evidence": "Pending required platform verification.",
            "differences": [],
        },
        "release": {
            "required": True,
            "target_sdk_versions": dict(requirement["target_sdk_versions"]),
            "checks": collect_sdk_version_checks(
                requirement["target_sdk_versions"], sources=version_sources
            ),
            "skipped_checks": [],
        },
        "knowledge_updates": [],
    }


def build_platform_units(units, feature):
    result = {
        platform: {
            "target_project": DEFAULT_PLATFORM_TARGETS[platform],
            "matrix_candidates": [],
        }
        for platform in PLATFORMS
    }
    for unit in units:
        if unit["feature"] != feature:
            continue
        platform = PLATFORM_UNIT_GROUPS.get(unit["platform_unit"])
        if platform:
            result[platform]["matrix_candidates"].append(unit)
    return result


def prepare_case_execution(
    matrix_path,
    feature=None,
    index=0,
    sdk_family=None,
    key_apis=None,
    target_sdk_version=None,
    platform_sdk_versions=None,
    repository_profile=DEFAULT_REPOSITORY_PROFILE,
):
    target_sdk_versions = resolve_target_sdk_versions(
        target_sdk_version, platform_sdk_versions
    )
    profile = load_repository_profile(repository_profile)
    backlog = generate_execution_units(matrix_path)
    matching = [
        unit for unit in backlog["execution_units"] if feature is None or unit["feature"] == feature
    ]
    selected = select_execution_unit(matching, None, index) if matching else None
    if selected is None and not (feature and sdk_family and key_apis):
        raise ValueError(
            "a requirement outside the actionable matrix needs feature, sdk_family, and key_apis"
        )
    requirement = {
        "feature": feature or selected["feature"],
        "sdk_family": sdk_family or selected["sdk_family"],
        "key_apis": key_apis or selected["key_apis"],
        "target_sdk_versions": target_sdk_versions,
    }
    source_candidate = None
    if selected and selected["reference_candidates"]:
        source_candidate = selected["reference_candidates"][0]
    source_case = resolve_source_case(source_candidate)
    blockers = []
    if not source_case:
        blockers.append("No DONE reference candidate found; Contract must identify a source case.")
    return {
        "requirement": requirement,
        "platform_units": build_platform_units(backlog["execution_units"], requirement["feature"]),
        "reference_contract": {
            "required": bool(source_case),
            "source_candidate": source_candidate,
            "source_case": source_case,
        },
        "role_contracts": ROLE_CONTRACTS,
        "execution_steps": [
            "Run one shared Contract for all official platforms.",
            "Run Android, iOS, macOS, and Windows Implementation agents independently with attributed deltas.",
            "Run independent platform Verification agents in parallel.",
            "Record cross-platform differences and final acceptance.",
            "Validate the manifest before applying matrix updates.",
        ],
        "acceptance_manifest_seed": build_manifest_seed(
            requirement, source_case, profile["sdk_version_sources"]
        ),
        "validation_command": "python3 docs/ai-engineering/tools/validate_acceptance_manifest.py <manifest.json>",
        "blockers": blockers,
    }


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--matrix", default=str(DEFAULT_MATRIX))
    parser.add_argument("--feature", help="Requirement feature; defaults to the highest-priority gap")
    parser.add_argument("--sdk-family", help="Required when the feature is not actionable in the matrix")
    parser.add_argument("--key-api", action="append", help="Key SDK API; repeat for multiple APIs")
    parser.add_argument("--target-sdk-version", required=True)
    parser.add_argument(
        "--platform-sdk-version",
        action="append",
        default=[],
        help="Override one platform as platform=x.y.z; repeat as needed",
    )
    parser.add_argument("--repository-profile", default=str(DEFAULT_REPOSITORY_PROFILE))
    parser.add_argument("--index", type=int, default=0)
    parser.add_argument("--output")
    args = parser.parse_args(argv)
    try:
        package = prepare_case_execution(
            Path(args.matrix),
            args.feature,
            index=args.index,
            sdk_family=args.sdk_family,
            key_apis=args.key_api,
            target_sdk_version=args.target_sdk_version,
            platform_sdk_versions=args.platform_sdk_version,
            repository_profile=Path(args.repository_profile),
        )
    except (OSError, ValueError) as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        return 1
    payload = json.dumps(package, indent=2, ensure_ascii=False) + "\n"
    if args.output:
        Path(args.output).write_text(payload, encoding="utf-8")
    else:
        print(payload, end="")
    return 0


if __name__ == "__main__":
    sys.exit(main())
