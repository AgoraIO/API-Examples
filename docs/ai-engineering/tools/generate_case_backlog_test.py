import json
import subprocess
import sys
import tempfile
import textwrap
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
GENERATOR = REPO_ROOT / "docs/ai-engineering/tools/generate_case_backlog.py"

sys.path.insert(0, str(GENERATOR.parent))
from generate_case_backlog import parse_matrix_cell


class CaseBacklogGeneratorTest(unittest.TestCase):
    def run_generator(self, matrix_text):
        with tempfile.NamedTemporaryFile("w", suffix=".md", delete=False) as handle:
            handle.write(matrix_text)
            matrix_path = handle.name
        try:
            return subprocess.run(
                [sys.executable, str(GENERATOR), "--matrix", matrix_path],
                cwd=REPO_ROOT,
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                check=False,
            )
        finally:
            Path(matrix_path).unlink(missing_ok=True)

    def test_generates_execution_units_for_missing_and_partial_cells(self):
        matrix = textwrap.dedent(
            """
            | Feature | SDK Family | Key APIs | Android full | iOS UIKit | macOS | Windows | Notes |
            | --- | --- | --- | --- | --- | --- | --- | --- |
            | Join channel audio | Full RTC | `joinChannel`, `setAudioProfile` | `DONE(basic/JoinChannelAudio.java)` | `DONE(Basic/JoinChannelAudio/)` | `DONE(Basic/JoinChannelAudio/)` | `MISSING` | Windows has no basic audio-only join case. |
            | Media metadata | Full RTC | `registerMediaMetadataObserver` | `DONE(advanced/MediaMetadata.java)` | `DONE(Advanced/VideoMetadata/)` | `DONE(Advanced/VideoMetadata/)` | `PARTIAL(Advanced/Metadata; smoke pending)` | Runtime metadata smoke pending. |
            | Audio mixing | Full RTC | `startAudioMixing` | `DONE(advanced/PlayAudioFiles.java)` | `DONE(Advanced/AudioMixing/)` | `DONE(Advanced/AudioMixing/)` | `DONE(Advanced/AudioMixing/)` | Full coverage. |

            ## Confirmed Gaps

            | Gap | Affected Units | Severity |
            | --- | --- | --- |
            | Basic audio-only join channel | Windows | High - missing foundational case |
            """
        )

        result = self.run_generator(matrix)

        self.assertEqual(result.returncode, 0, result.stderr)
        payload = json.loads(result.stdout)
        self.assertEqual(len(payload["execution_units"]), 2)
        self.assertEqual(payload["execution_units"][0]["feature"], "Join channel audio")
        self.assertEqual(payload["execution_units"][0]["platform_unit"], "Windows")
        self.assertEqual(payload["execution_units"][0]["target_project"], "windows/")
        self.assertEqual(payload["execution_units"][0]["status"], "MISSING")
        self.assertEqual(
            payload["execution_units"][0]["key_apis"],
            ["joinChannel", "setAudioProfile"],
        )
        self.assertEqual(payload["execution_units"][0]["severity"], "High")
        self.assertEqual(payload["execution_units"][0]["priority"], 10)
        self.assertEqual(
            payload["execution_units"][0]["reference_candidates"],
            [
                {
                    "platform_unit": "Android full",
                    "project": "Android/APIExample/",
                    "path": "basic/JoinChannelAudio.java",
                },
                {
                    "platform_unit": "iOS UIKit",
                    "project": "iOS/APIExample/",
                    "path": "Basic/JoinChannelAudio/",
                },
                {
                    "platform_unit": "macOS",
                    "project": "macOS/",
                    "path": "Basic/JoinChannelAudio/",
                },
            ],
        )
        self.assertEqual(payload["execution_units"][1]["platform_unit"], "Windows")
        self.assertEqual(payload["execution_units"][1]["status"], "PARTIAL")
        self.assertEqual(payload["execution_units"][1]["status_note"], "Advanced/Metadata; smoke pending")
        self.assertEqual(payload["execution_units"][1]["severity"], "Unspecified")
        self.assertEqual(payload["execution_units"][1]["priority"], 95)

    def test_hidden_cell_is_parsed_but_not_added_to_backlog(self):
        matrix = textwrap.dedent(
            """
            | Feature | SDK Family | Key APIs | Android full | iOS UIKit | macOS | Windows | Notes |
            | --- | --- | --- | --- | --- | --- | --- | --- |
            | Rhythm player | Full RTC | `startRhythmPlayer` | `HIDDEN(advanced/RhythmPlayer.java)` | `HIDDEN(Advanced/RhythmPlayer/)` | `N/A(no case)` | `N/A(no case)` | Deprecated API retained for reference. |
            """
        )

        result = self.run_generator(matrix)

        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(json.loads(result.stdout)["execution_units"], [])
        self.assertEqual(
            parse_matrix_cell("HIDDEN(advanced/RhythmPlayer.java)"),
            ("HIDDEN", "advanced/RhythmPlayer.java"),
        )

    def test_unknown_platform_column_fails(self):
        matrix = textwrap.dedent(
            """
            | Feature | SDK Family | Key APIs | Android full | iOS UIKit | macOS | Unknown Platform | Notes |
            | --- | --- | --- | --- | --- | --- | --- | --- |
            | Feature A | Full RTC | `apiA` | `DONE(path)` | `DONE(path)` | `DONE(path)` | `MISSING` | Missing. |
            """
        )

        result = self.run_generator(matrix)

        self.assertNotEqual(result.returncode, 0)
        self.assertIn("Unknown Platform", result.stderr)

    def test_confirmed_gap_alias_applies_priority(self):
        matrix = textwrap.dedent(
            """
            | Feature | SDK Family | Key APIs | Android full | iOS UIKit | macOS | Windows | Notes |
            | --- | --- | --- | --- | --- | --- | --- | --- |
            | Join channel audio | Full RTC | `joinChannel`, `setAudioProfile` | `DONE(basic/JoinChannelAudio.java)` | `DONE(Basic/JoinChannelAudio/)` | `DONE(Basic/JoinChannelAudio/)` | `MISSING` | Windows has no basic audio-only join case. |

            ## Confirmed Gaps

            | Gap | Affected Units | Severity |
            | --- | --- | --- |
            | Basic audio-only join channel | Windows | High - missing foundational case |
            """
        )

        result = self.run_generator(matrix)

        self.assertEqual(result.returncode, 0, result.stderr)
        unit = json.loads(result.stdout)["execution_units"][0]
        self.assertEqual(unit["feature"], "Join channel audio")
        self.assertEqual(unit["platform_unit"], "Windows")
        self.assertEqual(unit["severity"], "High")
        self.assertEqual(unit["priority"], 10)

    def test_legacy_project_variant_column_fails_even_without_actionable_status(self):
        matrix = textwrap.dedent(
            """
            | Feature | SDK Family | Key APIs | Android full | Android Compose | iOS UIKit | macOS | Windows | Notes |
            | --- | --- | --- | --- | --- | --- | --- | --- | --- |
            | Feature A | Full RTC | `apiA` | `DONE(path)` | `DONE(path)` | `DONE(path)` | `DONE(path)` | `DONE(path)` | Covered. |
            """
        )

        result = self.run_generator(matrix)

        self.assertNotEqual(result.returncode, 0)
        self.assertIn("platform columns must be exactly", result.stderr)
        self.assertIn("Android Compose", result.stderr)


if __name__ == "__main__":
    unittest.main()
