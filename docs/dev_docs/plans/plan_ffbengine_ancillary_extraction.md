# Implementation Plan - FFBEngine Ancillary Code Extraction (Task 1a)

**Context:**  
Refactor `FFBEngine.h` (currently ~2150 lines) by extracting ancillary utilities and generic logic into separate modules. This will improve readability, maintainability, and testability of the core FFB physics engine. The goal is to separate "Pure FFB Physics" from "Math Helpers", "Stats/Monitoring", and "Content Parsing".

**Reference Documents:**
- Proposal: `docs/dev_docs/proposals/FFBEngine_Ancillary_Extraction.md`
- Architecture Template: `gemini_orchestrator/templates/A.1_architect_prompt.md`

## Codebase Analysis Summary

### Current Architecture
- **`FFBEngine.h`**: Massive header-only implementation containing:
    - Core FFB Physics (Slip angle, Grip, Effects)
    - Mathematical Constants (`PI`, `TWO_PI`)
    - DSP structs (`BiquadNotch`)
    - Statistical helpers (`ChannelStats`)
    - Vehicle metadata parsing (`ParseVehicleClass`, `ParsedVehicleClass` enum)
    - All implementations are inline.
- **`CMakeLists.txt`**: Builds `LMUFFB_Core` library from `FFBEngine.h` (no `.cpp` counterpart).
- **Tests**: 
    - `tests/test_math_utils.cpp`, `test_perf_stats.cpp`, `test_vehicle_utils.cpp` exist but actively test `FFBEngine` instantiations via `FFBEngineTestAccess`.

### Impacted Functionalities
1.  **Math Utilities**: `BiquadNotch`, `smoothstep`, `inverse_lerp`, `apply_slew_limiter` will be moved.
2.  **Performance Stats**: `ChannelStats` struct will be moved.
3.  **Vehicle Parsing**: `ParseVehicleClass`, `GetDefaultLoadForClass`, `ParsedVehicleClass` enum will be moved.
4.  **FFB Engine**: Will strictly include these new files instead of defining them.
5.  **Tests**: Existing utility tests must be refactored to test the new modules directly, removing the dependency on `FFBEngine`.

## FFB Effect Impact Analysis
*   **No functional changes to FFB effects are intended.**
*   The refactoring is purely structural. Binary output of math functions must remain bit-exact (or within floating-point tolerance).
*   **User Perspective**: No change in feel, settings, or presets.

## Proposed Changes

### 1. Create `src/MathUtils.h` (Header-Only)
-   **Namespace**: `ffb_math`
-   **Contents**:
    -   `constexpr double PI`, `TWO_PI`
    -   `struct BiquadNotch` (Move implementation here)
    -   `smoothstep`, `inverse_lerp` (Templated or inline)
    -   `apply_slew_limiter`, `apply_adaptive_smoothing`
    -   `calculate_sg_derivative`
-   **Dependencies**: `<cmath>`, `<algorithm>`, `<array>`, `<utility>`

### 2. Create `src/PerfStats.h` (Header-Only)
-   **Contents**:
    -   `struct ChannelStats` (Move implementation here)
-   **Dependencies**: `<cmath>`, `<limits>`

### 3. Create `src/VehicleUtils.h` and `src/VehicleUtils.cpp`
-   **Header (`.h`)**:
    -   `enum class ParsedVehicleClass`
    -   Function declarations:
        -   `ParsedVehicleClass ParseVehicleClass(const char* className, const char* vehicleName);`
        -   `double GetDefaultLoadForClass(ParsedVehicleClass vclass);`
        -   `const char* VehicleClassToString(ParsedVehicleClass vclass);`
-   **Source (`.cpp`)**:
    -   Move implementations of the above functions from `FFBEngine.h`.
    -   Includes `<string>`, `<algorithm>`, `<vector>`, `"VehicleUtils.h"`.

### 4. Build System Updates
-   **`CMakeLists.txt`**: Add `src/VehicleUtils.cpp` to `CORE_SOURCES` list (approx line 128).

### 5. Clean up `src/FFBEngine.h`
-   **Removals**: Delete all moved structs, constants, enums, and functions.
-   **Includes**: Add `#include "MathUtils.h"`, `#include "PerfStats.h"`, `#include "VehicleUtils.h"`.
-   **Updates**:
    -   Update usage of `ParsedVehicleClass` (now in global/namespace scope, or aliased).
    -   Update usage of `BiquadNotch` (now `ffb_math::BiquadNotch` or `using`).
    -   Update usage of math functions (`ffb_math::smoothstep`, etc).

### 6. Refactor Tests (Pre-Implementation Verification)
-   **`tests/test_math_utils.cpp`**: 
    -   Remove dependency on `FFBEngine` instance.
    -   Update calls: `FFBEngineTestAccess::CallSmoothstep(...)` -> `ffb_math::smoothstep(...)`.
-   **`tests/test_perf_stats.cpp`**:
    -   Instantiate `ChannelStats` directly.
-   **`tests/test_vehicle_utils.cpp`**:
    -   Call `ParseVehicleClass` directly.
-   **`tests/test_ffb_common.h`**:
    -   Remove obsolete accessor declarations from `FFBEngineTestAccess` to ensure nothing is accidentally using the old path.

## Version Increment Rule
*   Increment version in `VERSION` and `src/Version.h` by **+1 to the rightmost number** (e.g., `0.7.35` -> `0.7.36`).

## Test Plan (Refactoring Focused)

Since the logic tests **already exist**, the task is to **port** them to the new architecture.

**1. `test_math_utils.cpp` Refactor**
*   **Goal**: Prove `MathUtils.h` works without `FFBEngine`.
*   **Change**: `FFBEngineTestAccess::CallSmoothstep(...)` -> `ffb_math::smoothstep(...)`.
*   **Assertions**: Current logic (pass).
*   **Test Count**: Baseline + 0 (No new logic, just refactoring).

**2. `test_perf_stats.cpp` Refactor**
*   **Goal**: Prove `ChannelStats` works standalone.
*   **Change**: Direct instantiation.
*   **Assertions**: Current logic (pass).
*   **Test Count**: Baseline + 0.

**3. `test_vehicle_utils.cpp` Refactor**
*   **Goal**: Prove vehicle parsing works standalone.
*   **Change**: Call global `ParseVehicleClass(...)` instead of `engine.ParseVehicleClass(...)`.
*   **Assertions**: Current logic (pass).
*   **Test Count**: Baseline + 0.

**4. `test_ffb_engine.exe` (Integration)**
*   **Goal**: Prove `FFBEngine` still compiles and runs correctly using the new modules.
*   **Change**: None (except maybe checking for `ParsedVehicleClass` enum scope changes in other tests).
*   **Assertions**: All existing physics tests MUST pass.
*   **Test Count**: Baseline + 0.

## Deliverables Checklist
- [x] `src/MathUtils.h` created.
- [x] `src/PerfStats.h` created.
- [x] `src/VehicleUtils.h` and `.cpp` created.
- [x] `CMakeLists.txt` updated.
- [x] `src/FFBEngine.h` cleaned up.
- [x] `tests/test_math_utils.cpp` refactored and passing.
- [x] `tests/test_perf_stats.cpp` refactored and passing.
- [x] `tests/test_vehicle_utils.cpp` refactored and passing.
- [x] Full test suite (`run_combined_tests.exe`) passing.

## Implementation Notes
-   **Namespace Management**: Defined `ffb_math` namespace for math utilities to prevent collisions. Added `using namespace ffb_math;` inside `FFBEngine.h` to minimize code churn in the main engine file during this phase.
-   **Backward Compatibility**: Added `using ParsedVehicleClass = ::ParsedVehicleClass;` inside `FFBEngine` to maintain compatibility with existing code that references `FFBEngine::ParsedVehicleClass`.
-   **Test Refactoring**: 
    -   Tests in `tests/test_math_utils.cpp`, `tests/test_perf_stats.cpp`, and `tests/test_vehicle_utils.cpp` were updated to include the new headers and call functions directly.
    -   `FFBEngineTestAccess` was cleaned up to remove wrappers for functions that are now public in their own modules.
    -   Some integration tests (`test_ffb_slope_detection.cpp`, etc.) required updates to call the new utility functions directly where they were previously accessing them through the engine instance.
-   **Build System**: `VehicleUtils.cpp` was successfully added to `CORE_SOURCES` in `CMakeLists.txt`.

### Unforeseen Issues
1.  **Direct Test Usage**: A significant number of integration tests (e.g., `test_ffb_slope_detection.cpp`, `test_ffb_smoothstep.cpp`, `test_ffb_coverage_target.cpp`) were directly accessing helper methods (like `calculate_sg_derivative`, `smoothstep`) via the `FFBEngine` instance. Moving these to free functions or namespaces broke these tests instantly, requiring immediate refactoring of test files that were not originally in scope for this task.
2.  **Test Accessor Obsolescence**: The `FFBEngineTestAccess` class contained wrappers for many of the extracted functions. These wrappers broke because the underlying private/protected methods on `FFBEngine` no longer existed. This necessitated a cleanup of `test_ffb_common.h` simultaneous with the extraction.

### Plan Deviations
1.  **Simultaneous Test Refactoring**: The plan suggested refactoring unit tests "Pre-Implementation". However, due to the tight coupling of `FFBEngineTestAccess`, it was more practical to refactor `FFBEngine` and the tests simultaneously to resolve compilation errors in one pass.
2.  **Compatibility Layers**: Explicitly added `using namespace ffb_math;` and `using ParsedVehicleClass = ::ParsedVehicleClass;` to `FFBEngine.h`. This was a tactical decision to avoid touching ~200 lines of internal engine logic that referenced these types, reducing the risk of introducing logic bugs during the structural refactor.

### Challenges Encountered
1.  **Compilation Cascades**: The project has a high degree of inter-dependency in headers. Modifying `FFBEngine.h` triggered full rebuilds, and missing includes in the new utility files (like `<algorithm>` or `<cmath>`) caused standard library errors that were previously masked by `FFBEngine.h`'s heavy inclusion list.
2.  **Ambiguous Overloads**: Moving `operator<<` for `ChannelStats` or `ParsedVehicleClass` required care to ensure they were visible to the test runner's logging system without causing ambiguous overload errors.

### Recommendations for Future Plans
1.  **Grep First**: When planning an extraction, perform a codebase-wide grep for the functions/types to be moved. This would have revealed the extent of usage in `tests/` and allowed for a better estimate of the refactoring effort.
2.  **Alias Bridge**: The strategy of using `using` aliases in the legacy header to maintain backward compatibility (internal bridge) proved highly effective. It allows for a phased migration where the engine internals can be updated to use the fully qualified names in a subsequent, lower-risk pass.

