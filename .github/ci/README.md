# CI Implementation

GitHub Actions entry points live in `.github/workflows/`. This directory contains their
policy helpers and the release packaging scripts loaded by Jenkins.

| Path | Caller and responsibility | Validation |
| --- | --- | --- |
| `Jenkinsfile_bitbucket.groovy` | External Jenkins shared library loads the release pipeline. | Verify job configuration and release evidence in the release ticket. |
| `build/` | Jenkins platform Groovy entry points call packaging scripts, which use each project's `cloud_build.*` adapters. IPA entry points also depend on external Jenkins configuration. | Run the signing-preflight unit tests below; actual signing, private SDK injection and artifact verification belong to Jenkins. |
| `policy/` | `repository-policy.yml` runs AI asset validation and native template lifecycle regressions; `.git-hooks/` supplies the local credential and commit-message checks. | Follow [AI validation](../../docs/ai/README.md) for venv setup and policy checks. |

After creating the venv described above, run the packaging helper tests from the repository root:

```bash
.venv/bin/python -m unittest discover --start-directory .github/ci/build/tests --pattern 'test_check_ios_signing_assets.py'
```

Keep the Jenkins entry point and `build/` paths stable: external jobs and existing scripts
refer to them directly. A lack of repository-local callers does not establish that an
external entry point is unused. GitHub compilation uses public SDK dependencies and
produces compile evidence; Jenkins owns release artifacts and signing evidence.

## Template checks

`policy/check_templates.py` extracts the canonical creation templates rather than maintaining
copies of their implementation. Its controlled SDK, UI and Token dependencies live in
`policy/tests/template_fixtures/`. These regressions exercise pending permission/Token exit,
repeated cleanup/setup, reopen, reordered responses and failure paths; they do not run RTC
sessions or prove SDK binary compatibility. The Apple check also compiles the OC template
against a Swift-generated header using the actual project's NetworkManager declaration.

From the repository root:

```bash
# macOS with Xcode command-line tools (Swift, Clang, Foundation)
python3 .github/ci/policy/check_templates.py apple
# JDK 17 and Kotlin CLI on PATH; KOTLINC can select a local compiler wrapper
python3 .github/ci/policy/check_templates.py kotlin
# Real Android/RTC interfaces, using the SDK selected for this checkout
python3 .github/ci/policy/check_templates.py android-sdk \
  --sdk-jar /path/to/agora-rtc-sdk.jar --android-jar /path/to/android.jar
```

Repository Policy runs Apple/Kotlin lifecycle checks on macOS/Ubuntu hosted runners; Ubuntu
supplies the Kotlin CLI. Missing compilers, missing template sections or missing inputs fail
the check rather than silently skipping it. No SDK download or credentials are needed for
these controlled regressions.

`compile.yml` additionally calls `policy/stage_compile_templates.py` before compiling Android
Audio/Compose and all four iOS projects. The helper adds complete template sources to the
actual app target, including Xcode Sources membership, so its SDK and project interfaces are
checked with the normal build. It intentionally mutates the checkout: run it only in a
disposable CI/scratch checkout, never a developer working tree with pending changes. It does
not add menu entries or run the example. Template/fixture checks do not replace the normal
application build, permission UI integration, case registration or device validation.
