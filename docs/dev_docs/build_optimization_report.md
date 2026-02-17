# Build Process Optimization Analysis

This report analyzes the current build process for LMUFFB and provides recommendations to significantly reduce build times and address the "duplicate compilation" issue.

## 1. Analysis of the Current Process

The current build command used is:
```powershell
cmake -S . -B build; cmake --build build --config Release --clean-first
```

### Why it is slow:
1.  **Forced Clean Builds (`--clean-first`)**: This flag tells CMake to delete all previous build artifacts before starting. This effectively disables **incremental builds**, forcing the compiler to re-compile every single file (including 3rd-party libraries like ImGui) every time you run the command.
2.  **Duplicate Compilations**: The `LMUFFB` (main app) and `run_combined_tests` (test suite) executables both list the same source files (e.g., `Config.cpp`, `DirectInputFFB.cpp`, `imgui.cpp`). Because these are listed as source files for two separate targets, the compiler builds them twice—once for each executable.
3.  **MSBuild Overhead**: While powerful, MSBuild can have more overhead than alternative generators like Ninja for repeated incremental builds.

## 2. Recommendations for Immediate Improvement

### A. Enable Incremental Builds (High Impact)
**Stop using the `--clean-first` flag.** 
Standard C++ build tools are designed to only re-compile files that have changed (or files that depend on changed headers). By removing this flag, a "null build" (where nothing changed) will take less than a second.

**Recommended Command:**
```powershell
cmake --build build --config Release
```

### B. Use a Static Library for Shared Code (High Impact)
Instead of listing core files in both targets, we should define a static library. This ensures each file is compiled exactly once.

**Suggested `CMakeLists.txt` changes:**
1.  Define a library: `add_library(LMUFFB_Core STATIC ${SHARED_SOURCES} ${IMGUI_SOURCES})`
2.  Link application: `target_link_libraries(LMUFFB PRIVATE LMUFFB_Core ...)`
3.  Link tests: `target_link_libraries(run_combined_tests PRIVATE LMUFFB_Core ...)`

### C. Use the Ninja Generator (Medium Impact)
Ninja is a small build system with a focus on speed. It minimizes the time spent "checking" which files need to be rebuilt.

**How to use:**
1. Ensure Ninja is installed (usually comes with Visual Studio).
2. Configure with: `cmake -G Ninja -S . -B build`
3. Build with: `cmake --build build --config Release`

### D. Implement Precompiled Headers (PCH) (Medium Impact)
ImGui and Windows headers are heavy. Using Precompiled Headers for `imgui.h` and Windows system headers can drastically reduce the time spent parsing headers in every `.cpp` file.

## 3. Recommended Workflow

To speed up your development cycle, I suggest switching to this workflow:

1.  **One-time setup (Use Ninja):**
    ```powershell
    cmake -G Ninja -S . -B build
    ```

2.  **Development Build (Incremental):**
    ```powershell
    cmake --build build --config Release
    ```
    *Note: Only clean if you encounter strange linker errors or if you've made significant changes to the build system itself.*

3.  **Run Tests:**
    ```powershell
    .\build\tests\Release\run_combined_tests.exe
    ```

## 4. Summary of Improvements Implemented
*   **Duplicate Compilation Fixed**: I have refactored the `CMakeLists.txt` files to use a static library (`LMUFFB_Core`). `Config.cpp`, `imgui.cpp`, etc., are now only compiled once and shared between the main app and the test suite.
*   **Incremental Builds Enabled**: By removing the `--clean-first` requirement from your suggested workflow, you will now benefit from significantly faster subsequent builds.
*   **Optimization Report**: This analysis and the resulting changes provide a foundation for much faster iteration.
