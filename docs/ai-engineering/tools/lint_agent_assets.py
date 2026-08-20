#!/usr/bin/env python3
"""Lint the repository AI engineering assets: AGENTS.md, SKILL.md, and skill bundles.

Each rule guards a drift class that has actually broken these assets before:

- `skills-directory-name`  a project puts its skills in the legacy `.agent/skills/`
- `skill-frontmatter`      a SKILL.md has no YAML frontmatter, so it cannot be selected
- `skill-name`             `name` is missing, malformed, or disagrees with its directory
- `skill-description`      `description` is missing or exceeds the specification limit
- `asset-reference`        an AGENTS.md or SKILL.md points at a file that does not exist

Exit code is 0 when every rule passes and 1 when any finding is reported.
"""

import argparse
import json
import os
import re
import sys
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]

SKILLS_DIR_NAME = ".agents"
LEGACY_SKILLS_DIR_NAMES = (".agent",)

# Directories that never hold committed agent assets. Pruning them keeps the walk
# fast on a repository that also contains platform build output.
SKIP_DIR_NAMES = {
    ".cxx",
    ".git",
    ".gradle",
    ".idea",
    "Pods",
    "__pycache__",
    "build",
    "libs",
    "node_modules",
    "scope",
    "sdk",
    "xcuserdata",
}

ASSET_FILE_NAMES = ("AGENTS.md", "SKILL.md")

FRONTMATTER_RE = re.compile(r"\A---\r?\n(.*?)\r?\n---\r?\n", re.DOTALL)
FRONTMATTER_KEY_RE = re.compile(r"^([A-Za-z0-9_-]+):\s*(.*)$")
FOLD_INDICATORS = {">", "|", ">-", "|-", ">+", "|+"}

# Agent Skills specification: 1-64 chars, lowercase alphanumerics and single hyphens,
# no leading or trailing hyphen.
SKILL_NAME_RE = re.compile(r"^[a-z0-9]+(?:-[a-z0-9]+)*$")
MAX_SKILL_NAME_LENGTH = 64
MAX_SKILL_DESCRIPTION_LENGTH = 1024

INLINE_CODE_RE = re.compile(r"`([^`\n]+)`")
PLACEHOLDER_CHARS = set("<>*?|$()[]")
# Only references that identify an agent asset are resolved. Project source paths are
# written relative to a source root that varies per platform, so checking them here
# would report noise instead of drift.
ASSET_REFERENCE_MARKERS = (
    f"{SKILLS_DIR_NAME}/skills/",
    "references/",
    "docs/ai-engineering/",
)
ASSET_REFERENCE_SUFFIXES = ("AGENTS.md", "ARCHITECTURE.md", "SKILL.md")


class Finding:
    """A single rule violation, reported with the location that must be edited."""

    def __init__(self, rule, path, line, message):
        self.rule = rule
        self.path = path
        self.line = line
        self.message = message

    def as_dict(self):
        return {
            "rule": self.rule,
            "path": self.path,
            "line": self.line,
            "message": self.message,
        }

    def format(self):
        location = f"{self.path}:{self.line}" if self.line else self.path
        return f"{location}: [{self.rule}] {self.message}"


def relative_path(path, root):
    try:
        return Path(path).resolve().relative_to(Path(root).resolve()).as_posix()
    except ValueError:
        return Path(path).as_posix()


def walk_directories(root):
    """Yield (dirpath, dirnames) with build and vendor directories pruned in place."""
    for dirpath, dirnames, _ in os.walk(root):
        dirnames[:] = sorted(name for name in dirnames if name not in SKIP_DIR_NAMES)
        yield dirpath, dirnames


def find_asset_files(root, names=ASSET_FILE_NAMES):
    found = []
    for dirpath, _ in walk_directories(root):
        for name in names:
            candidate = Path(dirpath) / name
            if candidate.is_file():
                found.append(candidate)
    return sorted(found)


def parse_frontmatter(text):
    """Return the frontmatter fields, or None when the block is absent.

    Only the subset of YAML used by these skills is supported: top-level scalars and
    folded block scalars introduced with `>` or `|`. This avoids a PyYAML dependency
    that the repository does not otherwise carry.
    """
    match = FRONTMATTER_RE.match(text)
    if not match:
        return None
    fields = {}
    current_key = None
    for raw_line in match.group(1).splitlines():
        if not raw_line.strip():
            continue
        indented = raw_line[:1].isspace()
        key_match = FRONTMATTER_KEY_RE.match(raw_line.strip())
        if key_match and not indented:
            current_key = key_match.group(1)
            value = key_match.group(2).strip()
            fields[current_key] = "" if value in FOLD_INDICATORS else value
        elif current_key is not None and indented:
            continuation = raw_line.strip()
            existing = fields.get(current_key, "")
            fields[current_key] = f"{existing} {continuation}".strip()
    return fields


def check_skill_directory_names(root):
    findings = []
    for dirpath, dirnames in walk_directories(root):
        for legacy_name in LEGACY_SKILLS_DIR_NAMES:
            if legacy_name not in dirnames:
                continue
            legacy_dir = Path(dirpath) / legacy_name
            if not (legacy_dir / "skills").is_dir():
                continue
            findings.append(
                Finding(
                    "skills-directory-name",
                    relative_path(legacy_dir, root),
                    0,
                    f"skills must live in `{SKILLS_DIR_NAME}/skills/`; "
                    f"rename this legacy `{legacy_name}/skills/` directory",
                )
            )
    return findings


def check_skill_metadata(skill_path, root):
    findings = []
    rel = relative_path(skill_path, root)
    text = skill_path.read_text(encoding="utf-8", errors="replace")
    fields = parse_frontmatter(text)
    if fields is None:
        findings.append(
            Finding(
                "skill-frontmatter",
                rel,
                1,
                "missing YAML frontmatter; the file must open with a `---` block "
                "declaring `name` and `description`",
            )
        )
        return findings

    expected_name = skill_path.parent.name
    name = fields.get("name", "").strip()
    if not name:
        findings.append(Finding("skill-name", rel, 1, "frontmatter is missing `name`"))
    else:
        if name != expected_name:
            findings.append(
                Finding(
                    "skill-name",
                    rel,
                    1,
                    f"`name: {name}` must match the skill directory name `{expected_name}`",
                )
            )
        if len(name) > MAX_SKILL_NAME_LENGTH:
            findings.append(
                Finding(
                    "skill-name",
                    rel,
                    1,
                    f"`name` is {len(name)} characters; the limit is {MAX_SKILL_NAME_LENGTH}",
                )
            )
        if not SKILL_NAME_RE.match(name):
            findings.append(
                Finding(
                    "skill-name",
                    rel,
                    1,
                    f"`name: {name}` must use lowercase alphanumerics separated by "
                    "single hyphens, with no leading or trailing hyphen",
                )
            )

    description = fields.get("description", "").strip()
    if not description:
        findings.append(
            Finding("skill-description", rel, 1, "frontmatter is missing `description`")
        )
    elif len(description) > MAX_SKILL_DESCRIPTION_LENGTH:
        findings.append(
            Finding(
                "skill-description",
                rel,
                1,
                f"`description` is {len(description)} characters; "
                f"the limit is {MAX_SKILL_DESCRIPTION_LENGTH}",
            )
        )
    return findings


def is_asset_reference(ref):
    if not ref or ref.startswith(("http://", "https://", "#")):
        return False
    if any(char in ref for char in PLACEHOLDER_CHARS) or " " in ref:
        return False
    if ref.endswith(ASSET_REFERENCE_SUFFIXES):
        return True
    return any(marker in ref for marker in ASSET_REFERENCE_MARKERS)


def resolve_reference(ref, source_path, root):
    """Resolve a reference against the source directory, then each ancestor.

    Asset docs write sibling paths (`ARCHITECTURE.md`), project-relative paths
    (`APIExample/.agents/skills/...`), and repository-relative paths
    (`docs/ai-engineering/knowledge-index.md`). Walking up from the source directory
    accepts all three without accepting a path that exists nowhere.
    """
    root = Path(root).resolve()
    candidates = [root / ref]
    current = Path(source_path).resolve().parent
    while True:
        candidates.append(current / ref)
        if current == root or root not in current.parents:
            break
        current = current.parent
    return any(candidate.exists() for candidate in candidates)


def check_asset_references(asset_path, root):
    findings = []
    rel = relative_path(asset_path, root)
    seen = set()
    for line_number, line in enumerate(
        asset_path.read_text(encoding="utf-8", errors="replace").splitlines(), start=1
    ):
        for match in INLINE_CODE_RE.finditer(line):
            ref = match.group(1).strip().rstrip(",.;:")
            if not is_asset_reference(ref):
                continue
            if ref in seen:
                continue
            seen.add(ref)
            if not resolve_reference(ref, asset_path, root):
                findings.append(
                    Finding(
                        "asset-reference",
                        rel,
                        line_number,
                        f"referenced path does not exist: {ref}",
                    )
                )
    return findings


def lint(root=REPO_ROOT):
    root = Path(root)
    findings = list(check_skill_directory_names(root))
    asset_files = find_asset_files(root)
    for asset_path in asset_files:
        if asset_path.name == "SKILL.md":
            findings.extend(check_skill_metadata(asset_path, root))
        findings.extend(check_asset_references(asset_path, root))
    findings.sort(key=lambda finding: (finding.path, finding.line, finding.rule))
    return {
        "checked_files": [relative_path(path, root) for path in asset_files],
        "findings": [finding.as_dict() for finding in findings],
    }, findings


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--root", default=str(REPO_ROOT), help="Repository root to lint")
    parser.add_argument("--json", action="store_true", help="Emit findings as JSON")
    args = parser.parse_args(argv)

    try:
        payload, findings = lint(Path(args.root))
    except OSError as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        return 1

    if args.json:
        print(json.dumps(payload, indent=2, ensure_ascii=False))
    else:
        for finding in findings:
            print(finding.format(), file=sys.stderr)
        checked = len(payload["checked_files"])
        if findings:
            print(
                f"\n{len(findings)} finding(s) across {checked} agent asset file(s)",
                file=sys.stderr,
            )
        else:
            print(f"OK: {checked} agent asset file(s) passed all rules")
    return 1 if findings else 0


if __name__ == "__main__":
    sys.exit(main())
