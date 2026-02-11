# Contributing Rules

This repository enforces shared rules so every staff member works with the same standards.

## Mandatory Setup (one time after clone)

```bash
./scripts/setup_hooks.sh
```

This enables versioned git hooks from `.githooks/`.

## Commit Message Format

Use:

```text
type(scope): subject
```

Allowed `type` values:
- `feat`
- `fix`
- `chore`
- `docs`
- `refactor`
- `test`
- `build`
- `ci`

Examples:
- `feat(gameplay): add relay warning message`
- `fix(input): map movement to azerty`

## Forbidden Tracked Paths

Do not commit generated Unreal/build files:
- `Binaries/`
- `DerivedDataCache/`
- `Intermediate/`
- `Saved/`
- `.vs/`
- `build/`

## Formatting

All C/C++ files must follow `.clang-format`.

Local check:

```bash
scripts/check_format.sh all
```

## CI Enforcement

GitHub Actions workflow `Repo Rules` blocks merges when:
- forbidden generated paths are tracked
- formatting check fails
- legacy CMake prototype no longer builds

## Pull Requests

Use the PR template and include:
- short summary
- why the change exists
- quick proof of test/run
