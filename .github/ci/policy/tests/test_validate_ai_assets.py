import contextlib
import importlib.util
import io
import shutil
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch


MODULE_PATH = Path(__file__).resolve().parents[1] / "validate_ai_assets.py"
SPEC = importlib.util.spec_from_file_location("validate_ai_assets", MODULE_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(f"Unable to load {MODULE_PATH}")
VALIDATOR = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(VALIDATOR)


class ValidateSkillFrontmatterTest(unittest.TestCase):
    def setUp(self) -> None:
        self.temporary_directory = tempfile.TemporaryDirectory()
        self.original_root = VALIDATOR.ROOT
        VALIDATOR.ROOT = Path(self.temporary_directory.name)

    def tearDown(self) -> None:
        VALIDATOR.ROOT = self.original_root
        self.temporary_directory.cleanup()

    def validate(self, frontmatter: str) -> list[str]:
        skill_path = VALIDATOR.ROOT / ".agents" / "skills" / "sample-skill" / "SKILL.md"
        skill_path.parent.mkdir(parents=True)
        skill_path.write_text(f"---\n{frontmatter}\n---\n\n# Sample\n", encoding="utf-8")
        errors: list[str] = []
        VALIDATOR.validate_skill(skill_path, errors)
        return errors

    def test_accepts_valid_yaml_frontmatter(self) -> None:
        errors = self.validate("name: sample-skill\ndescription: >\n  A valid description.\nlicense: MIT")
        self.assertEqual([], errors)

    def test_rejects_malformed_yaml_description(self) -> None:
        errors = self.validate("name: sample-skill\ndescription: [unterminated")
        self.assertTrue(any("invalid YAML frontmatter" in error for error in errors))

    def test_rejects_null_description(self) -> None:
        errors = self.validate("name: sample-skill\ndescription: null")
        self.assertTrue(any("description must be a non-empty string" in error for error in errors))

    def test_rejects_missing_description_value(self) -> None:
        errors = self.validate("name: sample-skill\ndescription:\nlicense: MIT")
        self.assertTrue(any("description must be a non-empty string" in error for error in errors))


class AssetIntegrityTest(unittest.TestCase):
    """Exercise the real asset tree, including failures that metadata tests miss."""

    def setUp(self) -> None:
        self.temporary_directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary_directory.cleanup)
        self.root = Path(self.temporary_directory.name).resolve()
        source_root = MODULE_PATH.parents[3]
        directories = {".", "Android", "iOS", *(path for _, path in VALIDATOR.PROJECTS)}
        for directory in directories:
            source = source_root / directory
            target = self.root / directory
            target.mkdir(parents=True, exist_ok=True)
            for filename in ("AGENTS.md", "CLAUDE.md", "ARCHITECTURE.md", "README.md"):
                if (source / filename).is_file():
                    shutil.copy2(source / filename, target / filename)
            if (source / ".agents").is_dir():
                shutil.copytree(source / ".agents", target / ".agents")
        shutil.copytree(source_root / "docs" / "ai", self.root / "docs" / "ai")
        shutil.copytree(source_root / ".github", self.root / ".github",
                        ignore=shutil.ignore_patterns("__pycache__", "*.pyc"))

    def validate(self) -> tuple[int, str]:
        output = io.StringIO()
        with patch.object(VALIDATOR, "ROOT", self.root), contextlib.redirect_stdout(output), contextlib.redirect_stderr(output):
            result = VALIDATOR.main()
        return result, output.getvalue()

    def assert_missing_file_rejected(self, relative_path: str) -> None:
        target = self.root / relative_path
        contents = target.read_bytes()
        target.unlink()
        try:
            result, output = self.validate()
            self.assertEqual(1, result, output)
            self.assertIn(f"missing required AI entry file: {relative_path}", output)
        finally:
            target.write_bytes(contents)

    def test_current_repository_assets_pass(self) -> None:
        result, output = self.validate()
        self.assertEqual(0, result, output)

    def test_missing_project_skills_fail_even_if_directories_remain(self) -> None:
        for role in ("query-cases", "upsert-case", "review-case"):
            with self.subTest(role=role):
                self.assert_missing_file_rejected(f"Android/APIExample/.agents/skills/{role}/SKILL.md")

    def test_missing_root_and_project_adapters_fail(self) -> None:
        for filename in ("CLAUDE.md", "iOS/APIExample-SwiftUI/CLAUDE.md"):
            with self.subTest(filename=filename):
                self.assert_missing_file_rejected(filename)

    def test_missing_platform_architecture_fails(self) -> None:
        for filename in ("Android/ARCHITECTURE.md", "iOS/ARCHITECTURE.md"):
            with self.subTest(filename=filename):
                self.assert_missing_file_rejected(filename)

    def test_directory_cannot_replace_required_entry_file(self) -> None:
        target = self.root / "AGENTS.md"
        target.unlink()
        target.mkdir()
        result, output = self.validate()
        self.assertEqual(1, result, output)
        self.assertIn("missing required AI entry file: AGENTS.md", output)

    def test_adapter_must_import_its_project_guide(self) -> None:
        target = self.root / "Android/APIExample/CLAUDE.md"
        target.write_text("# Adapter\n\nSee @MISSING.md\n", encoding="utf-8")
        result, output = self.validate()
        self.assertEqual(1, result, output)
        self.assertIn("adapter must import the same-directory @AGENTS.md", output)

    def test_missing_referenced_template_fails(self) -> None:
        target = self.root / "Android/APIExample/.agents/skills/upsert-case/references/fragment-template.java"
        target.unlink()
        result, output = self.validate()
        self.assertEqual(1, result, output)
        self.assertIn("missing reference: references/fragment-template.java", output)

    def test_invalid_project_skill_yaml_fails(self) -> None:
        target = self.root / "Android/APIExample/.agents/skills/upsert-case/SKILL.md"
        target.write_text("---\nname: upsert-case\ndescription: [unterminated\n---\n", encoding="utf-8")
        result, output = self.validate()
        self.assertEqual(1, result, output)
        self.assertIn("invalid YAML frontmatter", output)


if __name__ == "__main__":
    unittest.main()
