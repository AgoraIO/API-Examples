# Git Hook Installation Guide

This project uses Git hooks to prevent sensitive information leaks and check commit messages. After cloning the code for the first time, please follow these steps to install the hooks:

## One-click installation

Execute the following command to install all necessary Git hooks with one click:

```bash
./.git-hooks/install-hooks.sh
```

## Hook Features

After installation, the following features will be enabled:

1. **Sensitive Information Detection** (Based on Gitleaks)

- Detect sensitive information such as API keys and tokens in the code

- Block code submissions containing sensitive information

2. **Commit Message Inspection**

- Detect and block commit messages containing Chinese characters

- Ensure commit messages use only English
## Manual Installation

If the automatic installation fails, you can manually follow these steps:

1. Install pre-commit:

```bash

pip install pre-commit

```

2. Install the pre-commit hook:

```bash

pre-commit install

```

3. Install the commit-msg hook:
   ```bash
   pre-commit install --hook-type commit-msg
   cp .git-hooks/check-commit-message.sh .git/hooks/commit-msg
   chmod +x .git/hooks/commit-msg
   ```

## Frequently Asked Questions

**Q: How to temporarily skip hook checks?**

A: In special cases, you can use the `--no-verify` parameter to skip checks:

```bash

git commit -m "Your commit message" --no-verify

```

However, use this parameter with caution to avoid leaking sensitive information.

**Q: How to allow Chinese content in specific files?**

A: Modify the `allowlist` section in the `.gitleaks.toml` file and add the corresponding file paths.

**Q: Getting a "Chinese characters detected" prompt when committing?**

A: Commit messages must not contain Chinese characters. Please write commit messages in English only.