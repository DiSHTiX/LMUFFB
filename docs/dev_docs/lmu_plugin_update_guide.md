# LMU Plugin Update Guide

This document outlines the procedure for updating the Le Mans Ultimate (LMU) Shared Memory Plugin interface files in `src/lmu_sm_interface/`. These files are provided by Studio 397 (the game developers) and may need periodic updates to support new game versions.

## Files Involved
- `InternalsPlugin.hpp`
- `PluginObjects.hpp`
- `SharedMemoryInterface.hpp`

## Critical Maintenance Note

When updating `SharedMemoryInterface.hpp` from the official source, **you must manually verify and restore standard C++ library includes**.

The official files provided by Studio 397 often omit these headers (likely relying on precompiled headers or MSVC-specific behavior), which causes compilation errors in our project environment.

### Required Includes
Ensure the following headers are present at the top of `src/lmu_sm_interface/SharedMemoryInterface.hpp`:

```cpp
#include <optional>  // Required for std::optional
#include <utility>   // Required for std::exchange, std::swap
#include <cstdint>   // Required for uint32_t, uint8_t
#include <cstring>   // Required for memcpy
```

### Symptoms of Missing Includes
If these are missing, the build will fail with errors such as:
- `error C2039: 'optional': is not a member of 'std'`
- `error C2039: 'exchange': is not a member of 'std'`
- `error C3064: 'uint32_t': must be a simple type`
- `error C3861: 'MakeSharedMemoryLock': identifier not found`

## Update Procedure

1.  **Replace Files**: Overwrite the existing files in `src/lmu_sm_interface/` with the new versions from the game SDK folder.
    -   **Source Location**: `Program Files (x86)\Steam\steamapps\common\Le Mans Ultimate\Support\SharedMemoryInterface`
2.  **Verify Includes**: Open `SharedMemoryInterface.hpp` and check for the headers listed above. Add them if missing.
3.  **Compile**: Run a full build to verify compatibility.
4.  **Test**: Run `run_combined_tests` to ensure the interface changes haven't introduced regressions.
