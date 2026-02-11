#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <commit-msg-file>"
  exit 2
fi

msg_file="$1"
msg="$(head -n 1 "$msg_file" | tr -d '\r')"

pattern='^(feat|fix|chore|docs|refactor|test|build|ci)(\([a-z0-9._/-]+\))?: .+'

if [[ ! "$msg" =~ $pattern ]]; then
  echo "Invalid commit message format."
  echo "Expected: type(scope): subject"
  echo "Allowed types: feat|fix|chore|docs|refactor|test|build|ci"
  echo "Example: feat(gameplay): add relay warning UI"
  exit 1
fi
