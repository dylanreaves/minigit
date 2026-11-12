# Issue 4 Read text back and handle a missing file

## Goal
Add a small helper to read a text file and report a useful failure when the path does not exist or cannot be read.

## Dependency
Issue 3 merged.

## Acceptance criteria
- Read a file created by the writing helper and verify the exact contents.
- Test the missing-file case and verify it is reported without crashing.
- Keep the result/error handling understandable to a beginner.
- Use temporary test files and clean them up.
- CTest and both CI platforms pass.

## Out of scope
Do not add binary serialization, index data structures, or a command-line interface.
