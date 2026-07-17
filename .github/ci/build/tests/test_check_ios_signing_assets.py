import hashlib
import importlib.util
import plistlib
import sys
import tempfile
import unittest
from datetime import datetime, timezone
from pathlib import Path
from types import SimpleNamespace
from unittest import mock


MODULE_PATH = Path(__file__).parents[1] / "check_ios_signing_assets.py"
SPEC = importlib.util.spec_from_file_location("check_ios_signing_assets", MODULE_PATH)
MODULE = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
sys.modules[SPEC.name] = MODULE
SPEC.loader.exec_module(MODULE)


class SigningAssetCheckTests(unittest.TestCase):
    def test_load_export_options_deduplicates_profile_names(self):
        options = {
            "teamID": "TEAM123",
            "signingCertificate": "Apple Distribution",
            "provisioningProfiles": {
                "example.app": "shared-profile",
                "example.app.extension": "shared-profile",
            },
        }
        with tempfile.NamedTemporaryFile() as stream:
            plistlib.dump(options, stream)
            stream.flush()
            team_id, selector, profiles = MODULE.load_export_options(
                Path(stream.name)
            )

        self.assertEqual(team_id, "TEAM123")
        self.assertEqual(selector, "Apple Distribution")
        self.assertEqual(profiles, ["shared-profile"])

    def test_parse_identities_returns_sha1_to_name_mapping(self):
        output = (
            '  1) 553B538240B4CEF55467E6515514766F6767692C '
            '"iPhone Distribution: Example (TEAM123)"\n'
            "     1 valid identities found\n"
        )

        self.assertEqual(
            MODULE.parse_identities(output),
            {
                "553B538240B4CEF55467E6515514766F6767692C": (
                    "iPhone Distribution: Example (TEAM123)"
                )
            },
        )

    def test_certificate_selector_matches_distribution_aliases(self):
        for identity_name in (
            "Apple Distribution: Example (TEAM123)",
            "iOS Distribution: Example (TEAM123)",
            "iPhone Distribution: Example (TEAM123)",
        ):
            with self.subTest(identity_name=identity_name):
                self.assertTrue(
                    MODULE.certificate_selector_matches_identity(
                        "iPhone Distribution", identity_name
                    )
                )

        self.assertFalse(
            MODULE.certificate_selector_matches_identity(
                "iPhone Distribution", "Apple Development: Example (TEAM123)"
            )
        )

    def test_decode_profile_records_embedded_certificate_fingerprint(self):
        certificate = b"test certificate bytes"
        payload = plistlib.dumps(
            {
                "Name": "shared-profile",
                "TeamIdentifier": ["TEAM123"],
                "ExpirationDate": datetime(2030, 1, 1),
                "DeveloperCertificates": [certificate],
            }
        )
        completed = SimpleNamespace(returncode=0, stdout=payload, stderr=b"")

        with mock.patch.object(MODULE.subprocess, "run", return_value=completed):
            profile = MODULE.decode_profile(Path("profile.mobileprovision"), "security")

        self.assertIsNotNone(profile)
        fingerprint = hashlib.sha1(certificate).hexdigest().upper()
        self.assertEqual(profile.certificates, {fingerprint: certificate})
        self.assertEqual(profile.team_ids, ("TEAM123",))

    def test_expiration_status_is_critical_inside_critical_window(self):
        now = datetime(2026, 1, 1, tzinfo=timezone.utc)

        self.assertEqual(
            MODULE.expiration_status(
                datetime(2026, 1, 21, tzinfo=timezone.utc), now, 30, 90
            )[0],
            "critical",
        )
        self.assertEqual(
            MODULE.expiration_status(
                datetime(2026, 3, 2, tzinfo=timezone.utc), now, 30, 90
            )[0],
            "warning",
        )
        self.assertEqual(
            MODULE.expiration_status(
                datetime(2026, 6, 1, tzinfo=timezone.utc), now, 30, 90
            )[0],
            "ok",
        )

    def test_parse_certificate_expiration_reads_openssl_date(self):
        expires_at = MODULE.parse_certificate_expiration(
            "notAfter=Dec 11 02:30:30 2026 GMT\n"
        )

        self.assertEqual(
            expires_at,
            datetime(2026, 12, 11, 2, 30, 30, tzinfo=timezone.utc),
        )

    def test_validate_matches_profile_certificate_to_installed_identity(self):
        certificate = b"matching distribution certificate"
        fingerprint = hashlib.sha1(certificate).hexdigest().upper()
        profile = MODULE.ProvisioningProfile(
            path=Path("profile.mobileprovision"),
            name="shared-profile",
            team_ids=("TEAM123",),
            expires_at=datetime(2030, 1, 1, tzinfo=timezone.utc),
            certificates={fingerprint: certificate},
        )
        options = {
            "teamID": "TEAM123",
            "signingCertificate": "iPhone Distribution",
            "provisioningProfiles": {"example.app": "shared-profile"},
        }

        with tempfile.NamedTemporaryFile() as stream:
            plistlib.dump(options, stream)
            stream.flush()
            args = SimpleNamespace(
                export_options=Path(stream.name),
                profile_directory=[Path("profiles")],
                critical_days=30,
                warning_days=90,
            )
            with mock.patch.object(
                MODULE.shutil, "which", side_effect=lambda value: value
            ), mock.patch.object(
                MODULE, "profile_files", return_value=[profile.path]
            ), mock.patch.object(
                MODULE, "decode_profile", return_value=profile
            ), mock.patch.object(
                MODULE,
                "installed_identities",
                return_value={
                    fingerprint: "Apple Distribution: Example (TEAM123)"
                },
            ), mock.patch.object(
                MODULE,
                "certificate_expiration",
                return_value=datetime(2030, 1, 1, tzinfo=timezone.utc),
            ) as certificate_expiration:
                result = MODULE.validate(args)

        self.assertEqual(result, 0)
        certificate_expiration.assert_called_once_with(certificate, "openssl")

    def test_validate_missing_identity_only_warns(self):
        options = {
            "teamID": "TEAM123",
            "signingCertificate": "Apple Distribution",
            "provisioningProfiles": {"example.app": "shared-profile"},
        }

        with tempfile.NamedTemporaryFile() as stream:
            plistlib.dump(options, stream)
            stream.flush()
            args = SimpleNamespace(
                export_options=Path(stream.name),
                profile_directory=[Path("profiles")],
                critical_days=30,
                warning_days=90,
            )
            with mock.patch.object(
                MODULE.shutil, "which", side_effect=lambda value: value
            ), mock.patch.object(
                MODULE, "profile_files", return_value=[]
            ), mock.patch.object(
                MODULE, "installed_identities", return_value={}
            ):
                result = MODULE.validate(args)

        self.assertEqual(result, 0)


if __name__ == "__main__":
    unittest.main()
