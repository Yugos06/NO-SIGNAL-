#!/usr/bin/env bash
set -euo pipefail

range="${1:-HEAD~20..HEAD}"
pattern='^(feat|fix|chore|docs|refactor|test|build|ci)(\([a-z0-9._/-]+\))?: .+'

if ! git rev-parse --verify "${range##*..}" >/dev/null 2>&1; then
  echo "Invalid revision range: $range"
  exit 2
fi

violations=0

while IFS= read -r subject; do
  [[ -z "$subject" ]] && continue
  if [[ ! "$subject" =~ $pattern ]]; then
    echo "Invalid commit subject: $subject"
    violations=1
  fi
done < <(git log --format=%s "$range")

if [[ $violations -ne 0 ]]; then
  echo "Commit message rule violated."
  echo "Expected: type(scope): subject"
  echo "Allowed types: feat|fix|chore|docs|refactor|test|build|ci"
  exit 1
fi
