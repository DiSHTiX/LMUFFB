# vJoy and Joystick Gremlin Removal Plan - v0.4.15

**Date:** 2025-12-15  
**Objective:** Remove all vJoy and Joystick Gremlin dependencies and references from the project  
**Reason:** Testing has confirmed these components are not needed for the application to function

---

## Files to Modify

### 1. Source Code Files (C++/H)

#### `main.cpp`
- Remove `#include "src/DynamicVJoy.h"` (line 12)
- Remove `const int VJOY_DEVICE_ID = 1;` (line 17)
- Remove entire vJoy loading logic (lines 36-56)
- Remove vJoy acquisition state machine (lines 98-120)
- Remove vJoy cleanup (lines 130-132)
- Remove rF2 plugin conflict check (lines 175-178)

#### `src/DynamicVJoy.h`
- **DELETE ENTIRE FILE** - No longer needed

#### `src/Config.h`
- Remove `m_ignore_vjoy_version_warning` (line 112)
- Remove `m_enable_vjoy` (line 113)
- Remove `m_output_ffb_to_vjoy` (line 114)

#### `src/Config.cpp`
- Remove vJoy static variable definitions (lines ~4-6)
- Remove vJoy config loading/saving logic

#### `src/GuiLayer.cpp`
- Remove vJoy monitoring checkbox and warning (lines ~369-378)

#### `src/GameConnector.h` & `src/GameConnector.cpp`
- Remove `CheckLegacyConflict()` method

---

### 2. Documentation Files

#### `README.md`
- Remove all vJoy installation instructions
- Remove all Joystick Gremlin references
- Remove rF2 plugin installation section
- Simplify setup to DirectInput only

#### `README.txt`
- Same changes as README.md

#### `CHANGELOG.md`
- Add entry for v0.4.15 documenting removal

---

### 3. Other Documentation
- Update all dev_docs that mention vJoy or Joystick Gremlin
- Mark old bug reports as archived/obsolete

---

## Implementation Order

1. ✅ Create this plan document
2. ⏳ Remove vJoy from source code
3. ⏳ Remove Joystick Gremlin from documentation  
4. ⏳ Remove rF2 plugin checks
5. ⏳ Update README files
6. ⏳ Update CHANGELOG
7. ⏳ Test compilation
8. ⏳ Update version to 0.4.15

---

## Testing Checklist

- [ ] Project compiles without errors
- [ ] No vJoy DLL loading attempts
- [ ] No popup warnings about rF2 plugin
- [ ] DirectInput FFB works correctly
- [ ] GUI loads without vJoy options
- [ ] Config save/load works without vJoy fields

---

**Status:** Planning Complete - Ready for Implementation
