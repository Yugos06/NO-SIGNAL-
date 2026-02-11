#!/usr/bin/env bash
set -euo pipefail

mode="${1:-all}"

if [[ "$mode" == "staged" ]]; then
  candidates="$(git diff --cached --name-only --diff-filter=ACMR)"
elif [[ "$mode" == "all" ]]; then
  candidates="$(git ls-files)"
else
  echo "Usage: $0 [all|staged]"
  exit 2
fi

if [[ -z "$candidates" ]]; then
  exit 0
fi

cpp_files=()

while IFS= read -r file; do
  [[ -z "$file" ]] && continue

  case "$file" in
    Binaries/*|DerivedDataCache/*|Intermediate/*|Saved/*|build/*)
      continue
      ;;
  esac

  case "$file" in
    *.c|*.cc|*.cpp|*.cxx|*.h|*.hpp|*.inl|*.inc)
      cpp_files+=("$file")
      ;;
  esac
done <<< "$candidates"

if [[ ${#cpp_files[@]} -eq 0 ]]; then
  exit 0
fi

if ! command -v clang-format >/dev/null 2>&1; then
  echo "clang-format not found in PATH."
  echo "Install clang-format and retry (required for C/C++ changes)."
  exit 2
fi

fail=0

for file in "${cpp_files[@]}"; do
  if ! diff -u "$file" <(clang-format "$file") >/dev/null; then
    echo "Formatting mismatch: $file"
    fail=1
  fi
done

if [[ $fail -ne 0 ]]; then
  echo "Run clang-format on reported files before committing."
  exit 1
fi
