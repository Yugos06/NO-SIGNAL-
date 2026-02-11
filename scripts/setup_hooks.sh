#!/usr/bin/env bash
set -euo pipefail

git config core.hooksPath .githooks
chmod +x .githooks/pre-commit .githooks/commit-msg
chmod +x scripts/check_repo_rules.sh scripts/check_format.sh scripts/check_commit_message.sh scripts/check_commit_messages_range.sh

echo "Git hooks enabled for this repository."
echo "Installed hooks path: $(git config --get core.hooksPath)"
