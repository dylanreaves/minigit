# CMake and CI for Week 1

## What happens when I run CMake?

`cmake -S . -B build` reads `CMakeLists.txt` and creates build files in `build/`. `cmake --build build --config Release` compiles the targets. `ctest --test-dir build -C Release --output-on-failure` runs tests registered by `add_test`.

## When do I edit CMakeLists.txt?

- Editing a `.cpp` file already listed in `add_library` or `add_executable`: usually no CMake change.
- Adding a new implementation `.cpp`: add its path to the relevant `add_library(...)` list.
- Adding a test source to the existing test executable: add it to `add_executable(...)`.
- Adding a separate test program: create an `add_executable`, link it to the library, and register it with `add_test`.

Then configure, build, and test again.

## When do I edit `.github/workflows/ci.yml`?

Usually not when a new source file or test is correctly included in CMake. CI already configures, builds and runs CTest. Edit the workflow only if a new dependency must be installed, a new platform-specific setup is needed, or the project has a new build step.

## Windows checks

Run the commands in the repo root from Developer PowerShell or a terminal where CMake and a C++ compiler are available:

```powershell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

If configure fails because no compiler is detected, install Visual Studio's C++ workload, restart the Developer PowerShell, and try again. Record the first useful error for your submission sheet.
