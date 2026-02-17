# Proposal: Ancillary Code Extraction for `FFBEngine.h`

## Overview
This proposal focuses on reducing the noise in `FFBEngine.h` by extracting **ancillary utilities** and **generic mathematical tools** while keeping the core FFB physics, data structures, and effect algorithms in the main file. 

The goal is to ensure that when a developer opens `FFBEngine.h`, they see the "Force Feedback Formula" rather than generic signal processing or string parsing logic.

## What Stays in `FFBEngine.h` (The "Core")
- **Data Structures**: `FFBSnapshot`, `FFBCalculationContext`, `GripResult`.
- **Physics Modeling**: `calculate_slip_angle`, `calculate_grip`, `calculate_kinematic_load`.
- **Effect Algorithms**: `calculate_sop_lateral`, `calculate_abs_pulse`, `calculate_lockup_vibration`, etc.
- **Orchestration**: The `calculate_force` method.

## What Moves Out (The "Ancillary")

### 1. Mathematical & DSP Utilities (`src/MathUtils.h`)
These are generic tools that aren't specific to FFB logic but are used as building blocks.
- **Math Constants**: `PI`, `TWO_PI`, etc.
- **`struct BiquadNotch`**: This is a generic DSP filter. It should be moved and better documented (as noted in recent code comments).
- **Interpolation Helpers**: `inverse_lerp()`, `smoothstep()`.
- **Calculus Helpers**: `calculate_sg_derivative()` (Savitzky-Golay).
- **Signal Conditioners**: `apply_slew_limiter()`, `apply_adaptive_smoothing()`.

### 2. Performance & Monitoring Stats (`src/PerfStats.h`)
- **`class ChannelStats`**: This is a generic helper for tracking min/max/avg over time. It is used for telemetry monitoring but isn't part of the FFB algorithm itself.

### 3. Vehicle Metadata & Content Parsing (`src/VehicleUtils.h / .cpp`)
The logic for parsing strings to identify car classes is "content logic" rather than "FFB physics logic."
- `ParseVehicleClass()`
- `GetDefaultLoadForClass()`
- `VehicleClassToString()`
- `ParsedVehicleClass` enum.

---

## Proposed File Structure

### `src/MathUtils.h`
```cpp
// Generic math and signal processing tools
namespace ffb_math {
    struct BiquadNotch { ... }; // Documented & Cleaned
    double smoothstep(double edge0, double edge1, double x);
    double inverse_lerp(double min_val, double max_val, double value);
    // ...
}
```

### `src/FFBEngine.h` (Simplified)
```cpp
#include "MathUtils.h"
#include "PerfStats.h"
#include "VehicleUtils.h"

// Core FFB Data
struct FFBSnapshot { ... };
struct FFBCalculationContext { ... };

class FFBEngine {
    // Core FFB Physics
    double calculate_slip_angle(...) { ... }
    
    // Core Effect Logic 
    void calculate_sop_lateral(...) { ... }
    
    // Core Orchestration
    double calculate_force(...) {
        // Uses MathUtils for filters, but the FFB formula stays here.
    }
};
```

---

## Benefits of this Approach
1.  **Readability**: The 2100-line file will likely shrink by 400-600 lines of "boilerplate" math and string handling.
2.  **Focus**: `FFBEngine.h` becomes a pure representation of the FFB model.
3.  **Reusability**: `MathUtils.h` and `PerfStats.h` can be used by other parts of the application (e.g., the GUI or other input handlers) without including the heavy FFB Engine.
4.  **Testability**: The generic filters and math functions can be unit-tested in isolation in `test_math_utils.cpp`.

## Next Steps
1.  **Create `MathUtils.h`**: Move constants, interpolators, and `BiquadNotch` (with improved documentation).
2.  **Create `PerfStats.h`**: Move `ChannelStats`.
3.  **Create `VehicleUtils.h/.cpp`**: Move vehicle class parsing and lookup tables.
4.  **Clean up `FFBEngine.h`**: Replace extracted code with includes.
