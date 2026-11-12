## Related Issue
Closes #<number>

## What changed
- 

## How I tested
- [ ] `cmake -S . -B build`
- [ ] `cmake --build build --config Release`
- [ ] `ctest --test-dir build -C Release --output-on-failure`
- [ ] Windows CI passed
- [ ] Ubuntu CI passed

## Reviewer focus
Please check:
- [ ] The change matches the Issue acceptance criteria.
- [ ] Tests cover the behavior and avoid platform-specific path separators.
- [ ] CMake includes any new source or registered test.
- [ ] CI results and local test evidence are present.
- [ ] The PR stays focused on one Issue.

Do not merge until the instructor approves.
