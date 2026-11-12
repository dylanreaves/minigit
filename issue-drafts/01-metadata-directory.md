# Issue 1 Build the MiniGit metadata directory path

## Goal
Add one helper that returns the `.minigit` directory below a repository root.

## Acceptance criteria
- Use `std::filesystem::path`.
- Return `root / ".minigit"`.
- Add focused tests for an ordinary root and a nested root.
- This helper only builds a path; it does not create anything on disk.
- CMake build and CTest pass locally and in Windows/Ubuntu CI.

## Out of scope
Do not add `HEAD`, config, objects, or directory creation in this Issue.

## Evidence to include in the PR
List the three local commands and report the CI results.
