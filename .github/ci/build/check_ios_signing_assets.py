#!/usr/bin/env python3
"""Validate the iOS signing assets that Jenkins will use for IPA export."""

from __future__ import annotations

import argparse
import hashlib
import math
import os
import plistlib
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass
from datetime import datetime, timezone
from email.utils import parsedate_to_datetime
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Set, Tuple


IDENTITY_PATTERN = re.compile(
    r'^\s*\d+\)\s+([0-9A-Fa-f]{40})\s+"([^"]+)"'
)
DISTRIBUTION_CERTIFICATE_ALIASES = frozenset(
    {
        "apple distribution",
        "ios distribution",
        "iphone distribution",
    }
)


@dataclass(frozen=True)
class ProvisioningProfile:
    path: Path
    name: str
    team_ids: Tuple[str, ...]
    expires_at: datetime
    certificates: Dict[str, bytes]


def normalize_datetime(value: datetime) -> datetime:
    if value.tzinfo is None:
        return value.replace(tzinfo=timezone.utc)
    return value.astimezone(timezone.utc)


def parse_identities(output: str) -> Dict[str, str]:
    identities: Dict[str, str] = {}
    for line in output.splitlines():
        match = IDENTITY_PATTERN.match(line)
        if match:
            identities[match.group(1).upper()] = match.group(2)
    return identities


def certificate_selector_matches_identity(selector: str, identity_name: str) -> bool:
    normalized_selector = selector.strip().casefold()
    normalized_identity = identity_name.casefold()
    if normalized_selector in normalized_identity:
        return True
    return normalized_selector in DISTRIBUTION_CERTIFICATE_ALIASES and any(
        alias in normalized_identity for alias in DISTRIBUTION_CERTIFICATE_ALIASES
    )


def parse_certificate_expiration(output: str) -> datetime:
    for line in output.splitlines():
        if line.startswith("notAfter="):
            return normalize_datetime(parsedate_to_datetime(line.split("=", 1)[1]))
    raise ValueError("openssl output does not contain notAfter")


def expiration_status(
    expires_at: datetime,
    now: datetime,
    critical_days: int,
    warning_days: int,
) -> Tuple[str, int]:
    remaining_seconds = (normalize_datetime(expires_at) - normalize_datetime(now)).total_seconds()
    days_left = math.floor(remaining_seconds / 86400)
    if remaining_seconds <= 0:
        return "expired", days_left
    if remaining_seconds < critical_days * 86400:
        return "critical", days_left
    if remaining_seconds < warning_days * 86400:
        return "warning", days_left
    return "ok", days_left


def load_export_options(path: Path) -> Tuple[str, str, List[str]]:
    with path.open("rb") as stream:
        options = plistlib.load(stream)

    team_id = options.get("teamID")
    certificate_selector = options.get("signingCertificate")
    provisioning_profiles = options.get("provisioningProfiles")
    if not isinstance(team_id, str) or not team_id:
        raise ValueError("ExportOptions.plist does not define teamID")
    if not isinstance(certificate_selector, str) or not certificate_selector:
        raise ValueError("ExportOptions.plist does not define signingCertificate")
    if not isinstance(provisioning_profiles, dict) or not provisioning_profiles:
        raise ValueError("ExportOptions.plist does not define provisioningProfiles")

    profile_names = sorted(
        {value for value in provisioning_profiles.values() if isinstance(value, str) and value}
    )
    if not profile_names:
        raise ValueError("ExportOptions.plist has no provisioning profile names")
    return team_id, certificate_selector, profile_names


def profile_directories(explicit_directories: List[Path]) -> List[Path]:
    if explicit_directories:
        return explicit_directories

    configured = os.environ.get("IOS_PROVISIONING_PROFILE_DIRS")
    if configured:
        return [Path(value).expanduser() for value in configured.split(os.pathsep) if value]

    home = Path.home()
    return [
        home / "Library/MobileDevice/Provisioning Profiles",
        home / "Library/Developer/Xcode/UserData/Provisioning Profiles",
    ]


def profile_files(directories: Iterable[Path]) -> List[Path]:
    files: Set[Path] = set()
    for directory in directories:
        if not directory.is_dir():
            continue
        files.update(directory.rglob("*.mobileprovision"))
        files.update(directory.rglob("*.provisionprofile"))
    return sorted(files)


def decode_profile(path: Path, security: str) -> Optional[ProvisioningProfile]:
    result = subprocess.run(
        [security, "cms", "-D", "-i", str(path)],
        check=False,
        capture_output=True,
    )
    if result.returncode != 0:
        return None

    try:
        payload = plistlib.loads(result.stdout)
        name = payload["Name"]
        expires_at = normalize_datetime(payload["ExpirationDate"])
    except (KeyError, TypeError, ValueError, plistlib.InvalidFileException):
        return None

    team_ids = payload.get("TeamIdentifier", ())
    if isinstance(team_ids, str):
        team_ids = (team_ids,)
    elif isinstance(team_ids, list):
        team_ids = tuple(value for value in team_ids if isinstance(value, str))
    else:
        team_ids = ()

    certificates: Dict[str, bytes] = {}
    for certificate in payload.get("DeveloperCertificates", ()):
        if isinstance(certificate, bytes):
            fingerprint = hashlib.sha1(certificate).hexdigest().upper()
            certificates[fingerprint] = certificate

    if not isinstance(name, str) or not certificates:
        return None
    return ProvisioningProfile(path, name, team_ids, expires_at, certificates)


def installed_identities(security: str) -> Dict[str, str]:
    result = subprocess.run(
        [security, "find-identity", "-v", "-p", "codesigning"],
        check=False,
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        raise RuntimeError(result.stderr.strip() or "security find-identity failed")
    return parse_identities(result.stdout)


def certificate_expiration(certificate: bytes, openssl: str) -> datetime:
    result = subprocess.run(
        [openssl, "x509", "-inform", "DER", "-noout", "-enddate"],
        input=certificate,
        check=False,
        capture_output=True,
    )
    if result.returncode != 0:
        message = result.stderr.decode(errors="replace").strip()
        raise RuntimeError(message or "openssl could not read certificate")
    return parse_certificate_expiration(result.stdout.decode(errors="replace"))


def report_expiration(
    label: str,
    expires_at: datetime,
    now: datetime,
    critical_days: int,
    warning_days: int,
) -> None:
    status, days_left = expiration_status(
        expires_at, now, critical_days, warning_days
    )
    print(f"{label} expires: {expires_at.isoformat()} ({days_left} days remaining)")
    if status == "expired":
        print(f"CRITICAL: {label} has expired", file=sys.stderr)
    elif status == "critical":
        print(
            f"CRITICAL: {label} expires in less than {critical_days} days",
            file=sys.stderr,
        )
    elif status == "warning":
        print(f"WARNING: {label} expires in less than {warning_days} days")


def validate(args: argparse.Namespace) -> int:
    if args.critical_days < 0:
        raise ValueError("critical-days must not be negative")
    if args.warning_days < args.critical_days:
        raise ValueError("warning-days must be greater than or equal to critical-days")

    security = shutil.which("security")
    openssl = shutil.which("openssl")
    if not security:
        raise RuntimeError("macOS security command was not found")
    if not openssl:
        raise RuntimeError("openssl command was not found")

    team_id, certificate_selector, profile_names = load_export_options(
        args.export_options
    )
    directories = profile_directories(args.profile_directory)
    profiles = []
    for path in profile_files(directories):
        profile = decode_profile(path, security)
        if profile is not None:
            profiles.append(profile)
    identities = installed_identities(security)
    now = datetime.now(timezone.utc)

    print("iOS signing asset preflight")
    print(f"Export options: {args.export_options}")
    print(f"Team ID: {team_id}")
    print(f"Certificate selector: {certificate_selector}")

    for profile_name in profile_names:
        candidates = [
            profile
            for profile in profiles
            if profile.name == profile_name and team_id in profile.team_ids
        ]
        viable: List[Tuple[ProvisioningProfile, Set[str]]] = []
        for profile in candidates:
            matching_fingerprints = {
                fingerprint
                for fingerprint in profile.certificates
                if fingerprint in identities
                and certificate_selector_matches_identity(
                    certificate_selector, identities[fingerprint]
                )
            }
            if matching_fingerprints:
                viable.append((profile, matching_fingerprints))

        if not viable:
            searched = ", ".join(str(path) for path in directories)
            print(
                f"WARNING: no installed signing identity matches profile '{profile_name}' "
                f"for team {team_id}; searched {searched}",
                file=sys.stderr,
            )
            continue

        profile, fingerprints = max(viable, key=lambda item: item[0].expires_at)
        print(f"Provisioning profile: {profile.name} ({profile.path})")
        report_expiration(
            f"Provisioning profile '{profile.name}'",
            profile.expires_at,
            now,
            args.critical_days,
            args.warning_days,
        )

        certificate_options = [
            (
                certificate_expiration(profile.certificates[fingerprint], openssl),
                fingerprint,
            )
            for fingerprint in fingerprints
        ]
        expires_at, fingerprint = max(certificate_options)
        print(f"Signing identity: {identities[fingerprint]}")
        print(f"Certificate SHA-1: {fingerprint}")
        report_expiration(
            "Signing certificate",
            expires_at,
            now,
            args.critical_days,
            args.warning_days,
        )

    print("iOS signing asset preflight completed")
    return 0


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--export-options", required=True, type=Path)
    parser.add_argument(
        "--profile-directory",
        action="append",
        default=[],
        type=Path,
        help="Provisioning profile directory; may be supplied more than once",
    )
    parser.add_argument(
        "--critical-days",
        type=int,
        default=int(os.environ.get("IOS_SIGNING_CRITICAL_DAYS", "30")),
    )
    parser.add_argument(
        "--warning-days",
        type=int,
        default=int(os.environ.get("IOS_SIGNING_WARNING_DAYS", "90")),
    )
    return parser.parse_args()


def main() -> int:
    try:
        return validate(parse_args())
    except (OSError, RuntimeError, ValueError, plistlib.InvalidFileException) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
