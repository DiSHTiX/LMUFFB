# Release Summary: v0.5.14

**Date:** 2025-12-25  
**Version:** 0.5.14  
**Status:** ✅ **READY FOR RELEASE**  
**Test Results:** 307/307 Passing (163 FFB Engine + 144 Windows Platform)

---

## Overview

Version 0.5.14 is a code quality and project organization release that builds on the lockup vibration improvements from v0.5.13. This release focuses on maintainability, test accuracy, and project structure without changing any user-facing functionality.

---

## What's New

### 🗂️ Project Structure Reorganization

**Moved to `src/` directory:**
- `main.cpp` → `src/main.cpp`
- `FFBEngine.h` → `src/FFBEngine.h`

**Benefits:**
- All source code now consolidated in one location
- Cleaner project root directory
- Follows standard C++ project conventions
- Easier navigation for developers

**Files Updated:** 6 files (CMakeLists.txt, main.cpp, FFBEngine.h, Config.h, GuiLayer.h, test_ffb_engine.cpp)

---

### 🔧 Code Quality Improvements

#### 1. **Extracted Magic Number** (FFBEngine.h)
- **Before:** Hardcoded `0.01` in axle differentiation logic
- **After:** Named constant `AXLE_DIFF_HYSTERESIS = 0.01`
- **Benefit:** Better documentation and maintainability

```cpp
// Added constant with clear documentation
static constexpr double AXLE_DIFF_HYSTERESIS = 0.01;  // 1% slip buffer to prevent mode chattering

// Updated usage
if (max_slip_rear < (max_slip_front - AXLE_DIFF_HYSTERESIS)) {
    freq_multiplier = LOCKUP_FREQ_MULTIPLIER_REAR;
}
```

#### 2. **Test Baseline Alignment** (test_ffb_engine.cpp)
- **Before:** `test_progressive_lockup` used test-specific thresholds (10%/50%)
- **After:** Uses production defaults (5%/15%)
- **Benefit:** Tests now validate exactly what ships to users

```cpp
// Now uses production defaults
engine.m_lockup_start_pct = 5.0f;   // Production default
engine.m_lockup_full_pct = 15.0f;   // Production default
```

#### 3. **Enhanced Test Precision** (test_ffb_engine.cpp)
- **Before:** `test_split_load_caps` only verified >1.5x ratio
- **After:** Explicit 3x ratio verification with detailed diagnostics
- **Benefit:** More precise validation and better debugging

```cpp
// Added precise ratio verification
double expected_ratio = 3.0;
double actual_ratio = std::abs(force_high) / (std::abs(force_low) + 0.0001);

if (std::abs(actual_ratio - expected_ratio) < 0.5) {
    std::cout << "[PASS] Brake load cap applies 3x scaling (Ratio: " << actual_ratio << ")" << std::endl;
}
```

---

## Technical Details

### Files Modified

**Core Changes:**
1. `VERSION` - Updated to 0.5.14
2. `CHANGELOG.md` - Added v0.5.14 entry
3. `CMakeLists.txt` - Updated source paths
4. `src/main.cpp` - Fixed include paths
5. `src/FFBEngine.h` - Added constant, fixed include path
6. `src/Config.h` - Fixed include path
7. `src/GuiLayer.h` - Fixed include path
8. `tests/test_ffb_engine.cpp` - Updated include path, improved tests

**Documentation:**
- `docs/dev_docs/PROJECT_REORGANIZATION.md` - Complete reorganization guide
- `docs/dev_docs/code_reviews/CODE_QUALITY_IMPROVEMENTS_v0.5.13.md` - Detailed improvement documentation

---

## Verification

### Build Status ✅
```bash
cmake -S . -B build
cmake --build build --config Release
# Exit code: 0 (Success)
```

### Test Results ✅
```
FFB Engine Tests:    163/163 PASSED
Platform Tests:      144/144 PASSED
Total:               307/307 PASSED
```

### Code Quality ✅
- No compiler warnings
- All lints addressed
- Clean git diff
- Backward compatible

---

## Upgrade Notes

### For Users
- **No action required** - This is a code quality release
- All functionality remains identical to v0.5.13
- Configuration files are fully compatible

### For Developers
- Update include paths if you have custom modifications:
  - `#include "FFBEngine.h"` → `#include "src/FFBEngine.h"` (from tests)
  - `#include "../FFBEngine.h"` → `#include "FFBEngine.h"` (from src/)
- CMake will automatically handle the new structure

---

## Changelog Entry

```markdown
## [0.5.14] - 2025-12-25

### Changed
- **Project Structure Reorganization**: Moved `main.cpp` and `FFBEngine.h` 
  from project root to `src/` directory for better organization and cleaner 
  project structure.

### Improved
- **Code Quality Enhancements**:
  - **Extracted Magic Number**: Replaced hardcoded `0.01` hysteresis value 
    with named constant `AXLE_DIFF_HYSTERESIS`
  - **Test Baseline Alignment**: Updated `test_progressive_lockup` to use 
    production defaults (5%/15% thresholds)
  - **Enhanced Test Precision**: Improved `test_split_load_caps` with 
    explicit 3x ratio verification
```

---

## Next Steps

1. ✅ Version updated to 0.5.14
2. ✅ CHANGELOG updated
3. ✅ All tests passing
4. ✅ Build verified
5. ✅ Documentation complete

**Ready for:**
- Git commit
- Tag as v0.5.14
- Release to users

---

**Prepared by:** AI Assistant  
**Date:** 2025-12-25  
**Status:** ✅ PRODUCTION READY
