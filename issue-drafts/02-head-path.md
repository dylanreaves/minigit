# Issue 2 Build the HEAD file path

## Goal
Add one helper that returns the `HEAD` file path inside `.minigit`.

## Dependency
Issue 1 must be merged first, because this helper should reuse the metadata directory helper.

## Acceptance criteria
- Use `std::filesystem::path`.
- Return `metadataDirectory(root) / "HEAD"`.
- Test a normal root and a nested root.
- Do not create files or directories.
- CMake build and CTest pass locally and in Windows/Ubuntu CI.

## Out of scope
Do not write `HEAD` file contents or implement commits.
