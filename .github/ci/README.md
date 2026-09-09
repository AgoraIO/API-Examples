# CI Implementation

GitHub Actions entry points live in `.github/workflows/`. This directory contains their
policy helpers and the release packaging scripts loaded by Jenkins.

| Path | Caller and responsibility | Validation |
| --- | --- | --- |
| `Jenkinsfile_bitbucket.groovy` | External Jenkins shared library loads the release pipeline. | Verify job configuration and release evidence in the release ticket. |
| `build/` | Jenkins platform Groovy entry points call packaging scripts, which use each project's `cloud_build.*` adapters. IPA entry points also depend on external Jenkins configuration. | Run the signing-preflight unit tests below; actual signing, private SDK injection and artifact verification belong to Jenkins. |
| `policy/` | `repository-policy.yml` runs AI asset validation and its regression tests; `.git-hooks/` supplies the local credential and commit-message checks. | Follow [AI validation](../../docs/ai/README.md) for venv setup and policy checks. |

After creating the venv described above, run the packaging helper tests from the repository root:

```bash
.venv/bin/python -m unittest discover --start-directory .github/ci/build/tests --pattern 'test_check_ios_signing_assets.py'
```

Keep the Jenkins entry point and `build/` paths stable: external jobs and existing scripts
refer to them directly. A lack of repository-local callers does not establish that an
external entry point is unused. GitHub compilation uses public SDK dependencies and
produces compile evidence; Jenkins owns release artifacts and signing evidence.
