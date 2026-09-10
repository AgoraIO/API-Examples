# AI-Assisted Development

This directory contains durable guidance for coordinating one requirement across the nine
RTC API Example projects. It is intended for both developers and coding agents and is part
of the repository's normal review process.

## Entry Points

| File | Purpose |
| --- | --- |
| [AGENTS.md](../../AGENTS.md) | Repository-wide rules and navigation. |
| [cross-platform-api-change](../../.agents/skills/cross-platform-api-change/SKILL.md) | Executable workflow for cross-platform changes. |
| [change-contract-template.md](change-contract-template.md) | Canonical contract to complete before implementation. |
| [project-matrix.md](project-matrix.md) | Project inventory, SDK sources, and scope rules. |
| [release-risks.md](release-risks.md) | Verified repository-specific build and release risks. |
| [pull_request_template.md](../../.github/pull_request_template.md) | Review record for scope, differences, and validation. |
| [validate_ai_assets.py](../../.github/ci/policy/validate_ai_assets.py) | Local and CI validation for skill metadata, links, entry points, and the nine-project matrix. |

## Working Agreement

1. Translate the requirement into one testable change contract.
2. Make an explicit scope decision for every project before implementation.
3. Implement each required project from the same contract.
4. Compare behavior and validation evidence across platforms before completion.
5. Keep the final contract and evidence in the issue or pull request.

After changing AI guidance, use Python 3.10 or newer in a virtual environment, install the
locked validator dependency, and run both the tests and repository check from the repository root:

```bash
python3 -m venv .venv
.venv/bin/python -m pip install --requirement .github/ci/policy/requirements.txt
.venv/bin/python -m unittest discover --start-directory .github/ci/policy/tests --pattern 'test_validate_ai_assets.py'
.venv/bin/python .github/ci/policy/validate_ai_assets.py
```

On Windows, create the environment with `py -3 -m venv .venv` and use
`.venv/Scripts/python.exe` in place of `.venv/bin/python`. The environment is git-ignored.
Reuse it for later checks; do not install the validator into the system Python environment.

The same checks run in `.github/workflows/repository-policy.yml` for pushes and pull requests.
That workflow also compiles and executes selected Android/iOS template lifecycle regressions.
After editing creation templates, run the relevant native checks documented in
[CI template checks](../../.github/ci/README.md#template-checks). The post-release compile
workflow also builds the complete Android Audio/Compose and four iOS templates in their real
application targets. Structural validation alone does not establish template API correctness
or safe asynchronous cleanup.

The requirement contract owns shared behavior. Platform implementations may differ only
when the SDK, UI framework, permission model, or operating system requires it, and the
difference must be recorded.

## Repository Boundaries

- Treat source code, project `AGENTS.md`, and `ARCHITECTURE.md` files as the current case
  inventory.
- Do not maintain generated case catalogs, source hashes, agent routing tables, prompt
  snapshots, task transcripts, or run logs in this directory.
- Keep reusable workflow rules in the skill and repository facts in these documents.
- Update `project-matrix.md` when a project, SDK dependency source, or CI owner changes.
- Add an item to `release-risks.md` only after verifying it against current repository code
  or release automation.
- Keep the workflow and contract template aligned between the Agora and Shengwang
  repositories. Record brand and distribution differences only in repository-specific
  sections of the matrix and risk documents.
