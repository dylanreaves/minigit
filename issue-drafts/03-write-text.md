# Issue 3 Create a parent directory before writing text

## Goal
Add a small filesystem helper that creates the parent directory of a requested file path, then writes the supplied text.

## Dependency
Issue 1 and Issue 2 merged.

## Acceptance criteria
- Keep path construction separate from file writing.
- Create missing parent directories when needed.
- Return a clear success/failure result; do not silently ignore filesystem errors.
- Test writing into a new nested temporary directory and verify the text exists.
- Clean up temporary test files.
- CTest and both CI platforms pass.

## Out of scope
Do not implement binary objects, commit storage, or a complete Repository class.
