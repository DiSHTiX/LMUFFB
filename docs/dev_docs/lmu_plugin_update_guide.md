# LMU Plugin Update Guide

This document outlines the procedure for updating the Le Mans Ultimate (LMU) Shared Memory Plugin interface files in `src/lmu_sm_interface/`. These files are provided by Studio 397 (the game developers) and may need periodic updates to support new game versions.

## Files Involved
- `InternalsPlugin.hpp`
- `PluginObjects.hpp`
- `SharedMemoryInterface.hpp`

## Critical Maintenance Note

The project uses a **Wrapper Header** strategy to handle missing dependencies in the official files.

**Do NOT modify `SharedMemoryInterface.hpp` directly.**

Instead, the project uses `src/lmu_sm_interface/LmuSharedMemoryWrapper.h` to inject the necessary standard library headers (`<optional>`, `<utility>`, `<cstdint>`, `<cstring>`) before including the vendor file.

### Usage
Always include the wrapper in your code:
```cpp
#include "lmu_sm_interface/LmuSharedMemoryWrapper.h"
```

## Update Procedure

1.  **Replace Files**: Overwrite the existing files in `src/lmu_sm_interface/` with the new versions from the game SDK folder.
    -   **Source Location**: `Program Files (x86)\Steam\steamapps\common\Le Mans Ultimate\Support\SharedMemoryInterface`
2.  **Do Nothing Else**: You do NOT need to edit `SharedMemoryInterface.hpp` to add includes. The wrapper handles this.
3.  **Compile**: Run a full build to verify compatibility.
4.  **Test**: Run `run_combined_tests` to ensure the interface changes haven't introduced regressions.
