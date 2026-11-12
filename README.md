# CSC350 MiniGit Week 1: C++, Git, and GitHub

A beginner-friendly C++17 classroom repository for learning filesystem paths, CMake, local testing, GitHub Issues, pull requests, code review, and CI. Start with [`docs/tutorial.md`](docs/tutorial.md), then choose a small task from [`TODO.md`](TODO.md).

## Windows setup

Install Visual Studio 2022 with **Desktop development with C++** and the CMake tools. Clone with GitHub Desktop, then open the project folder in Visual Studio. Use the CMake configure/build buttons or a Developer PowerShell from the repository root:

```powershell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

## Classroom workflow

1. Read the task in `TODO.md` and create or open its GitHub Issue.
2. Create a branch for that one Issue.
3. Implement the smallest useful change and its test.
4. Configure, build, and run CTest locally.
5. Push the branch and open a PR; explain the change and test evidence.
6. Review a classmate's PR and respond to feedback.
7. Merge only after the required review and CI checks pass.

See [`docs/tutorial.md`](docs/tutorial.md) for the end-to-end walkthrough. Supporting material is in `docs/`, and starter Issue descriptions are in `issue-drafts/`.
