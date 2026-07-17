#!/usr/bin/env python3
"""Generate platform execution units from the case-maintenance matrix."""

import argparse
import json
import re
import sys
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
DEFAULT_MATRIX = REPO_ROOT / "docs/ai-engineering/case-maintenance-matrix.md"
NON_PLATFORM_COLUMNS = {"Feature", "SDK Family", "Key APIs", "Notes"}
PLATFORM_PROJECTS = {
    "Android full": "Android/APIExample/",
    "Android audio": "Android/APIExample-Audio/",
    "Android Compose": "Android/APIExample-Compose/",
    "iOS UIKit": "iOS/APIExample/",
    "iOS SwiftUI": "iOS/APIExample-SwiftUI/",
    "iOS Objective-C": "iOS/APIExample-OC/",
    "iOS audio": "iOS/APIExample-Audio/",
    "macOS": "macOS/",
    "Windows": "windows/",
}
SEVERITY_PRIORITY = {
    "High": 10,
    "Medium": 20,
    "Low": 30,
    "Unspecified": 90,
}
STATUS_PRIORITY_OFFSET = {
    "MISSING": 0,
    "PARTIAL": 5,
}
CONFIRMED_GAP_ALIASES = {
    "Basic audio-only join channel": ["Join channel audio"],
}


def split_markdown_row(line):
    return [cell.strip() for cell in line.strip().strip("|").split("|")]


def strip_code(value):
    value = value.strip()
    if value.startswith("`") and value.endswith("`"):
        return value[1:-1]
    return value


def parse_status(cell):
    status, detail = parse_matrix_cell(cell)
    if status in {"MISSING", "PARTIAL"}:
        return status, detail
    return "", ""


def parse_matrix_cell(cell):
    value = strip_code(cell)
    if value in {"MISSING", "UNKNOWN"}:
        return value, ""
    match = re.fullmatch(r"(DONE|PARTIAL|N/A)\((.+)\)", value)
    if match:
        return match.group(1), match.group(2)
    return "", ""


def parse_done_path(cell):
    status, detail = parse_matrix_cell(cell)
    return detail if status == "DONE" else ""


def parse_key_apis(cell):
    apis = re.findall(r"`([^`]+)`", cell)
    if apis:
        return [api.strip() for api in apis]
    return [part.strip() for part in cell.split(",") if part.strip()]


def find_matrix_table(lines):
    for index, line in enumerate(lines):
        if not line.startswith("|"):
            continue
        header = split_markdown_row(line)
        if "Feature" in header and "Key APIs" in header:
            rows = []
            for row_line in lines[index + 2 :]:
                if not row_line.startswith("|"):
                    break
                rows.append(split_markdown_row(row_line))
            return header, rows
    raise ValueError("could not find pilot matrix table with Feature and Key APIs columns")


def find_confirmed_gap_table(lines):
    for index, line in enumerate(lines):
        if not line.startswith("|"):
            continue
        header = split_markdown_row(line)
        if {"Gap", "Affected Units", "Severity"}.issubset(set(header)):
            rows = []
            for row_line in lines[index + 2 :]:
                if not row_line.startswith("|"):
                    break
                rows.append(split_markdown_row(row_line))
            return header, rows
    return [], []


def parse_severity(value):
    label = re.split(r"\s+[-—]\s+", value.strip(), maxsplit=1)[0].strip()
    return label if label in SEVERITY_PRIORITY else "Unspecified"


def gap_names_for_lookup(gap_name):
    return [gap_name, *CONFIRMED_GAP_ALIASES.get(gap_name, [])]


def build_gap_severity_map(lines):
    header, rows = find_confirmed_gap_table(lines)
    if not header:
        return {}
    gap_index = header.index("Gap")
    affected_index = header.index("Affected Units")
    severity_index = header.index("Severity")
    mapping = {}
    for row in rows:
        if len(row) != len(header):
            continue
        severity = parse_severity(row[severity_index])
        affected_units = [unit.strip() for unit in row[affected_index].split(",") if unit.strip()]
        for unit in affected_units:
            for gap_name in gap_names_for_lookup(row[gap_index]):
                mapping[(gap_name, unit)] = severity
    return mapping


def find_reference_candidates(header, row, target_platform):
    candidates = []
    for column_index, column_name in enumerate(header):
        if column_name in NON_PLATFORM_COLUMNS or column_name == target_platform:
            continue
        if column_name not in PLATFORM_PROJECTS:
            continue
        done_path = parse_done_path(row[column_index])
        if not done_path:
            continue
        candidates.append(
            {
                "platform_unit": column_name,
                "project": PLATFORM_PROJECTS[column_name],
                "path": done_path,
            }
        )
    return candidates


def generate_execution_units(matrix_path):
    lines = matrix_path.read_text(encoding="utf-8").splitlines()
    header, rows = find_matrix_table(lines)
    gap_severities = build_gap_severity_map(lines)
    feature_index = header.index("Feature")
    sdk_index = header.index("SDK Family")
    key_api_index = header.index("Key APIs")
    notes_index = header.index("Notes") if "Notes" in header else None
    execution_units = []
    errors = []

    for row in rows:
        if len(row) != len(header):
            errors.append(f"row has {len(row)} cells but header has {len(header)} cells: {row}")
            continue
        for column_index, column_name in enumerate(header):
            if column_name in NON_PLATFORM_COLUMNS:
                continue
            status, status_note = parse_status(row[column_index])
            if not status:
                continue
            if column_name not in PLATFORM_PROJECTS:
                errors.append(f"unknown platform column with actionable status: {column_name}")
                continue
            severity = gap_severities.get((row[feature_index], column_name), "Unspecified")
            priority = SEVERITY_PRIORITY[severity] + STATUS_PRIORITY_OFFSET[status]
            execution_units.append(
                {
                    "feature": row[feature_index],
                    "sdk_family": row[sdk_index],
                    "key_apis": parse_key_apis(row[key_api_index]),
                    "platform_unit": column_name,
                    "target_project": PLATFORM_PROJECTS[column_name],
                    "status": status,
                    "status_note": status_note,
                    "severity": severity,
                    "priority": priority,
                    "reference_candidates": find_reference_candidates(header, row, column_name),
                    "notes": row[notes_index] if notes_index is not None else "",
                }
            )

    if errors:
        raise ValueError("\n".join(errors))
    execution_units.sort(key=lambda unit: (unit["priority"], unit["feature"], unit["platform_unit"]))
    return {"execution_units": execution_units}


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--matrix", default=str(DEFAULT_MATRIX), help="Path to case-maintenance-matrix.md")
    args = parser.parse_args(argv)

    try:
        payload = generate_execution_units(Path(args.matrix))
    except (OSError, ValueError) as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        return 1

    print(json.dumps(payload, indent=2, ensure_ascii=False))
    return 0


if __name__ == "__main__":
    sys.exit(main())
