#!/usr/bin/env bash
set -euo pipefail

mode="${1:-all}"

if [[ "$mode" == "staged" ]]; then
  files="$(git diff --cached --name-only --diff-filter=ACMR)"
elif [[ "$mode" == "all" ]]; then
  files="$(git ls-files)"
else
  echo "Usage: $0 [all|staged]"
  exit 2
fi

if [[ -z "$files" ]]; then
  exit 0
fi

forbidden_regex='^(Binaries/|DerivedDataCache/|Intermediate/|Saved/|\.vs/|build/)'
violation=0

while IFS= read -r file; do
  [[ -z "$file" ]] && continue
  if [[ "$file" =~ $forbidden_regex ]]; then
    echo "Forbidden tracked path: $file"
    violation=1
  fi
done <<< "$files"

if [[ $violation -ne 0 ]]; then
  echo "Rule violation: Unreal generated files and build outputs must not be tracked."
  exit 1
fi
