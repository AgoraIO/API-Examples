#!/usr/bin/env python3
"""Validate durable AI guidance and cross-file references."""

from __future__ import annotations

import re
import sys
from pathlib import Path
from urllib.parse import unquote

import yaml


ROOT = Path(__file__).resolve().parents[3]
PROJECTS = (
    ("Android Views full", "Android/APIExample"),
    ("Android Views audio", "Android/APIExample-Audio"),
    ("Android Compose", "Android/APIExample-Compose"),
    ("iOS UIKit full", "iOS/APIExample"),
    ("iOS UIKit audio", "iOS/APIExample-Audio"),
    ("iOS SwiftUI", "iOS/APIExample-SwiftUI"),
    ("iOS Objective-C", "iOS/APIExample-OC"),
    ("macOS AppKit", "macOS"),
    ("Windows MFC", "windows"),
)
ENTRY_FILES = (
    "AGENTS.md",
    ".agents/skills/cross-platform-api-change/SKILL.md",
    ".github/pull_request_template.md",
    "docs/ai/README.md",
    "docs/ai/change-contract-template.md",
    "docs/ai/project-matrix.md",
    "docs/ai/release-risks.md",
)
PLATFORMS = ("Android", "iOS", "macOS", "windows")
PROJECT_SKILLS = ("query-cases", "upsert-case", "review-case")
TABLE_FILES = (
    "docs/ai/project-matrix.md",
    "docs/ai/change-contract-template.md",
    ".github/pull_request_template.md",
)
ALLOWED_FRONTMATTER_KEYS = {"name", "description", "license", "allowed-tools", "metadata"}
MARKDOWN_LINK = re.compile(r"(?<!!)\[[^]]+\]\(([^)]+)\)")
CODE_PATH = re.compile(r"`((?:references/|\.agents/skills/|docs/ai/|\.github/)[^`\n]*)`")
AGENTS_IMPORT = re.compile(r"(?<![\w/])@(?:\./)?AGENTS\.md\b")


def required_entry_files() -> set[str]:
    required = set(ENTRY_FILES)
    guide_directories = {".", *PLATFORMS, *(path for _, path in PROJECTS)}
    for directory in guide_directories:
        for filename in ("AGENTS.md", "CLAUDE.md"):
            required.add((Path(directory) / filename).as_posix())
        if directory != ".":
            required.add(f"{directory}/ARCHITECTURE.md")
    for _, project in PROJECTS:
        for skill in PROJECT_SKILLS:
            required.add(f"{project}/.agents/skills/{skill}/SKILL.md")
    return required


def relative(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def validate_skill(path: Path, errors: list[str]) -> None:
    text = path.read_text(encoding="utf-8")
    if not text.startswith("---\n"):
        errors.append(f"{relative(path)}: missing YAML frontmatter")
        return

    end = text.find("\n---\n", 4)
    if end < 0:
        errors.append(f"{relative(path)}: unterminated YAML frontmatter")
        return

    frontmatter = text[4:end]
    try:
        data = yaml.safe_load(frontmatter)
    except yaml.YAMLError as error:
        problem = getattr(error, "problem", None) or str(error).splitlines()[0]
        errors.append(f"{relative(path)}: invalid YAML frontmatter: {problem}")
        return

    if not isinstance(data, dict):
        errors.append(f"{relative(path)}: YAML frontmatter must be a mapping")
        return

    unexpected = {str(key) for key in data if key not in ALLOWED_FRONTMATTER_KEYS}
    if unexpected:
        errors.append(f"{relative(path)}: unsupported frontmatter keys: {', '.join(sorted(unexpected))}")

    if "name" not in data:
        errors.append(f"{relative(path)}: missing skill name")
    elif not isinstance(data["name"], str) or not data["name"].strip():
        errors.append(f"{relative(path)}: skill name must be a non-empty string")
    else:
        name = data["name"].strip()
        if name != path.parent.name:
            errors.append(f"{relative(path)}: skill name '{name}' does not match directory '{path.parent.name}'")
        if len(name) > 64 or not re.fullmatch(r"[a-z0-9]+(?:-[a-z0-9]+)*", name):
            errors.append(f"{relative(path)}: skill name must be lowercase hyphen-case and at most 64 characters")

    if "description" not in data:
        errors.append(f"{relative(path)}: missing skill description")
    elif not isinstance(data["description"], str) or not data["description"].strip():
        errors.append(f"{relative(path)}: skill description must be a non-empty string")
    else:
        description = data["description"].strip()
        if len(description) > 1024:
            errors.append(f"{relative(path)}: skill description must contain at most 1024 characters")


def resolve_reference(source: Path, target: str) -> Path | None:
    target = unquote(target.strip())
    if target.startswith("<") and target.endswith(">"):
        target = target[1:-1]
    target = target.split("#", 1)[0].split("?", 1)[0]
    if not target or target.startswith(("http://", "https://", "mailto:")):
        return None
    if target.startswith(("docs/ai/", ".github/")):
        return ROOT / target
    return source.parent / target


def validate_references(path: Path, errors: list[str]) -> None:
    text = path.read_text(encoding="utf-8")
    if path.name == "CLAUDE.md" and not AGENTS_IMPORT.search(text):
        errors.append(f"{relative(path)}: adapter must import the same-directory @AGENTS.md")
    if "docs/ai-engineering" in text or ".agent/skills" in text:
        errors.append(f"{relative(path)}: contains a retired AI asset path")
    if re.search(r"(?i)\[TODO:[^]]*\]|<TODO>|\bTODO:", text):
        errors.append(f"{relative(path)}: contains an unfinished TODO scaffold")

    targets = [match.group(1).split(maxsplit=1)[0] for match in MARKDOWN_LINK.finditer(text)]
    targets.extend(match.group(1).strip().rstrip(".,;:") for match in CODE_PATH.finditer(text))
    for target in targets:
        resolved = resolve_reference(path, target)
        if resolved is None:
            continue
        resolved = resolved.resolve()
        try:
            resolved.relative_to(ROOT)
        except ValueError:
            errors.append(f"{relative(path)}: reference leaves the repository: {target}")
            continue
        if not resolved.exists():
            errors.append(f"{relative(path)}: missing reference: {target}")


def validate_project_tables(errors: list[str]) -> None:
    expected = [project for project, _ in PROJECTS]
    for filename in TABLE_FILES:
        lines = (ROOT / filename).read_text(encoding="utf-8").splitlines()
        header_index = next(
            (index for index, line in enumerate(lines) if re.match(r"^\|\s*Project\s*\|", line)),
            None,
        )
        if header_index is None:
            errors.append(f"{filename}: missing project table")
            continue
        rows = []
        for line in lines[header_index + 2 :]:
            if not line.startswith("|"):
                break
            rows.append(line.strip("|").split("|", 1)[0].strip())
        if rows != expected:
            errors.append(f"{filename}: project table must contain the canonical nine rows in order")


def main() -> int:
    errors: list[str] = []
    for filename in sorted(required_entry_files()):
        if not (ROOT / filename).is_file():
            errors.append(f"missing required AI entry file: {filename}")

    skill_files = sorted(path for path in ROOT.glob("**/.agents/skills/*/SKILL.md") if path.is_file())
    if not skill_files:
        errors.append("no skills found under .agents/skills")
    for path in skill_files:
        validate_skill(path, errors)

    markdown_files = {ROOT / filename for filename in ENTRY_FILES if filename.endswith(".md")}
    markdown_files.update(ROOT.glob("**/AGENTS.md"))
    markdown_files.update(ROOT.glob("**/CLAUDE.md"))
    markdown_files.update(ROOT.glob("**/ARCHITECTURE.md"))
    markdown_files.update(ROOT.glob("docs/ai/**/*.md"))
    markdown_files.update(ROOT.glob("**/.agents/**/*.md"))
    for path in sorted(markdown_files):
        if path.is_file():
            validate_references(path, errors)

    if all((ROOT / filename).is_file() for filename in TABLE_FILES):
        validate_project_tables(errors)

    if errors:
        for error in errors:
            print(f"ERROR: {error}", file=sys.stderr)
        print(f"AI asset validation failed with {len(errors)} error(s).", file=sys.stderr)
        return 1

    print(f"AI asset validation passed: {len(skill_files)} skills, 9 projects, no broken references.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
