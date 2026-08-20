import json
import subprocess
import sys
import tempfile
import textwrap
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
LINTER = REPO_ROOT / "docs/ai-engineering/tools/lint_agent_assets.py"

sys.path.insert(0, str(LINTER.parent))
from lint_agent_assets import lint, parse_frontmatter


class LintAgentAssetsTest(unittest.TestCase):
    def write(self, root, rel_path, text):
        path = Path(root) / rel_path
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(textwrap.dedent(text).lstrip("\n"), encoding="utf-8")
        return path

    def write_skill(self, root, project, skill, name=None, description="Does a thing. Use when asked.", body="Body.\n"):
        name = skill if name is None else name
        path = Path(root) / f"{project}/.agents/skills/{skill}/SKILL.md"
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(
            f"---\nname: {name}\ndescription: {description}\n---\n\n{body}",
            encoding="utf-8",
        )
        return path

    def write_clean_tree(self, root):
        """A minimal tree that must satisfy every rule."""
        self.write(root, "AGENTS.md", "# Root\n\nSee `Android/AGENTS.md`.\n")
        self.write(root, "Android/AGENTS.md", "# Android\n\nSee `APIExample/AGENTS.md`.\n")
        self.write(
            root,
            "Android/APIExample/AGENTS.md",
            """
            # APIExample

            | Skill | Path |
            |-------|------|
            | upsert-case | `.agents/skills/upsert-case/` |

            See `ARCHITECTURE.md`.
            """,
        )
        self.write(root, "Android/APIExample/ARCHITECTURE.md", "# Architecture\n")
        self.write_skill(root, "Android/APIExample", "upsert-case")

    def rules(self, findings):
        return sorted(finding.rule for finding in findings)

    def messages(self, findings):
        return "\n".join(finding.format() for finding in findings)

    def test_clean_tree_reports_no_findings(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            self.write_clean_tree(tmpdir)

            payload, findings = lint(tmpdir)

            self.assertEqual(findings, [], self.messages(findings))
            self.assertIn("Android/APIExample/.agents/skills/upsert-case/SKILL.md", payload["checked_files"])
            self.assertEqual(payload["findings"], [])

    def test_flags_legacy_agent_skills_directory(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            self.write_clean_tree(tmpdir)
            self.write(
                tmpdir,
                "iOS/APIExample/.agent/skills/review-case/SKILL.md",
                """
                ---
                name: review-case
                description: Reviews a case. Use when reviewing.
                ---

                Body.
                """,
            )

            _, findings = lint(tmpdir)

            self.assertIn("skills-directory-name", self.rules(findings))
            self.assertIn("iOS/APIExample/.agent", self.messages(findings))

    def test_ignores_dot_agent_directory_without_skills(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            self.write_clean_tree(tmpdir)
            (Path(tmpdir) / "iOS/.agent/notes").mkdir(parents=True)

            _, findings = lint(tmpdir)

            self.assertEqual(findings, [], self.messages(findings))

    def test_flags_skill_without_frontmatter(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            self.write_clean_tree(tmpdir)
            self.write(
                tmpdir,
                "Android/APIExample/.agents/skills/query-cases/SKILL.md",
                "## What this skill is for\n\nBody.\n",
            )

            _, findings = lint(tmpdir)

            self.assertEqual(self.rules(findings), ["skill-frontmatter"])

    def test_flags_name_that_disagrees_with_directory(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            self.write_clean_tree(tmpdir)
            self.write_skill(tmpdir, "Android/APIExample", "query-cases", name="query-case")

            _, findings = lint(tmpdir)

            self.assertEqual(self.rules(findings), ["skill-name"])
            self.assertIn("must match the skill directory name `query-cases`", self.messages(findings))

    def test_flags_malformed_name_charset(self):
        for bad_name in ["Upsert_Case", "-upsert", "upsert-", "upsert--case"]:
            with self.subTest(name=bad_name), tempfile.TemporaryDirectory() as tmpdir:
                self.write_clean_tree(tmpdir)
                self.write(
                    tmpdir,
                    f"Android/APIExample/.agents/skills/{bad_name}/SKILL.md",
                    f"""
                    ---
                    name: {bad_name}
                    description: Does a thing. Use when asked.
                    ---

                    Body.
                    """,
                )

                _, findings = lint(tmpdir)

                self.assertIn("skill-name", self.rules(findings), self.messages(findings))

    def test_flags_missing_and_oversized_description(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            self.write_clean_tree(tmpdir)
            self.write(
                tmpdir,
                "Android/APIExample/.agents/skills/query-cases/SKILL.md",
                """
                ---
                name: query-cases
                ---

                Body.
                """,
            )

            _, findings = lint(tmpdir)

            self.assertEqual(self.rules(findings), ["skill-description"])

        with tempfile.TemporaryDirectory() as tmpdir:
            self.write_clean_tree(tmpdir)
            self.write_skill(tmpdir, "Android/APIExample", "query-cases", description="x" * 1025)

            _, findings = lint(tmpdir)

            self.assertEqual(self.rules(findings), ["skill-description"])
            self.assertIn("the limit is 1024", self.messages(findings))

    def test_accepts_description_at_the_limit(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            self.write_clean_tree(tmpdir)
            self.write_skill(tmpdir, "Android/APIExample", "query-cases", description="x" * 1024)

            _, findings = lint(tmpdir)

            self.assertEqual(findings, [], self.messages(findings))

    def test_flags_agents_md_pointing_at_missing_skill_directory(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            self.write_clean_tree(tmpdir)
            self.write(
                tmpdir,
                "Android/APIExample/AGENTS.md",
                """
                # APIExample

                | review-case | `.agents/skills/review-case/` |

                See `ARCHITECTURE.md`.
                """,
            )

            _, findings = lint(tmpdir)

            self.assertEqual(self.rules(findings), ["asset-reference"])
            self.assertIn(".agents/skills/review-case/", self.messages(findings))

    def test_flags_skill_pointing_at_missing_bundled_reference(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            self.write_clean_tree(tmpdir)
            self.write_skill(
                tmpdir,
                "Android/APIExample",
                "upsert-case",
                body="Use `APIExample/.agents/skills/upsert-case/references/fragment-template.java`.\n",
            )

            _, findings = lint(tmpdir)

            self.assertEqual(self.rules(findings), ["asset-reference"])
            self.assertIn("fragment-template.java", self.messages(findings))

    def test_resolves_project_relative_reference_via_ancestor_walk(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            self.write_clean_tree(tmpdir)
            self.write_skill(
                tmpdir,
                "Android/APIExample",
                "upsert-case",
                body="Use `APIExample/.agents/skills/upsert-case/references/fragment-template.java`.\n",
            )
            self.write(
                tmpdir,
                "Android/APIExample/.agents/skills/upsert-case/references/fragment-template.java",
                "class Template {}\n",
            )

            _, findings = lint(tmpdir)

            self.assertEqual(findings, [], self.messages(findings))

    def test_skips_placeholder_and_url_references(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            self.write_clean_tree(tmpdir)
            self.write_skill(
                tmpdir,
                "Android/APIExample",
                "upsert-case",
                body=(
                    "Read `<platform>/AGENTS.md` and `<project>/ARCHITECTURE.md`.\n"
                    "Glob `.agents/skills/*/SKILL.md`.\n"
                    "See https://agentskills.io/specification for the spec.\n"
                ),
            )

            _, findings = lint(tmpdir)

            self.assertEqual(findings, [], self.messages(findings))

    def test_prunes_build_and_vendor_directories(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            self.write_clean_tree(tmpdir)
            self.write(
                tmpdir,
                "Android/APIExample/build/.agents/skills/stale/SKILL.md",
                "no frontmatter here\n",
            )
            self.write(tmpdir, "Pods/AGENTS.md", "Points at `missing/AGENTS.md`.\n")

            payload, findings = lint(tmpdir)

            self.assertEqual(findings, [], self.messages(findings))
            self.assertFalse([path for path in payload["checked_files"] if "build/" in path])
            self.assertFalse([path for path in payload["checked_files"] if path.startswith("Pods/")])

    def test_parses_folded_block_scalar_description(self):
        fields = parse_frontmatter(
            textwrap.dedent(
                """
                ---
                name: upsert-case
                description: >
                  Add a new case or modify an existing one.
                  Use when: adding a demo screen.
                ---

                Body.
                """
            ).lstrip("\n")
        )

        self.assertEqual(fields["name"], "upsert-case")
        self.assertEqual(
            fields["description"],
            "Add a new case or modify an existing one. Use when: adding a demo screen.",
        )

    def test_returns_none_without_frontmatter(self):
        self.assertIsNone(parse_frontmatter("## Heading\n\nBody.\n"))

    def test_cli_reports_findings_and_exits_nonzero(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            self.write_clean_tree(tmpdir)
            self.write(
                tmpdir,
                "Android/APIExample/.agents/skills/query-cases/SKILL.md",
                "## No frontmatter\n",
            )

            result = subprocess.run(
                [sys.executable, str(LINTER), "--root", tmpdir, "--json"],
                cwd=REPO_ROOT,
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                check=False,
            )

            self.assertEqual(result.returncode, 1, result.stderr)
            payload = json.loads(result.stdout)
            self.assertEqual(
                [finding["rule"] for finding in payload["findings"]], ["skill-frontmatter"]
            )

    def test_cli_passes_on_the_live_repository(self):
        result = subprocess.run(
            [sys.executable, str(LINTER)],
            cwd=REPO_ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
        )

        self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == "__main__":
    unittest.main()
