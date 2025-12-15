# vJoy and Joystick Gremlin Removal Plan - v0.4.15

**Date:** 2025-12-15  
**Objective:** Remove user-facing annoyances related to vJoy and Joystick Gremlin  
**Reason:** Testing has confirmed these components are not needed for the application to function

---

## Phase 1: Immediate Changes (v0.4.15) - User Experience Fixes

**Goal:** Remove all user-facing annoyances without major code refactoring

### Changes Being Made NOW:

#### 1. Remove Startup Popups
- ✅ Remove vJoy DLL not found error popup in `main.cpp`
- ✅ Remove vJoy version mismatch warning popup in `main.cpp`
- ✅ Remove rF2 shared memory plugin conflict warning popup in `main.cpp`

#### 2. Update User Documentation
- ✅ `README.md` - Remove all vJoy installation instructions
- ✅ `README.md` - Remove all Joystick Gremlin references
- ✅ `README.md` - Remove rF2 shared memory plugin installation section
- ✅ `README.md` - Simplify to DirectInput-only setup
- ✅ `README.txt` - Same changes as README.md

#### 3. Update CHANGELOG
- ✅ Add v0.4.15 entry documenting user-facing changes

### What's Being LEFT IN PLACE (for now):

- `src/DynamicVJoy.h` - Keep file, code still loads DLL silently if present
- `src/Config.h` - Keep vJoy config variables (backward compatibility)
- `src/Config.cpp` - Keep vJoy config loading/saving
- `src/GuiLayer.cpp` - Keep vJoy monitoring checkbox (hidden/disabled state)
- `main.cpp` - Keep vJoy acquisition logic (runs silently, no popups)
- `src/GameConnector.cpp` - Keep `CheckLegacyConflict()` method (just don't show popup)

**Rationale:** Leaving code in place allows:
1. Users who somehow have vJoy to continue using it
2. Easier rollback if issues are discovered
3. Backward compatibility with existing config files
4. Future re-enablement if needed

---

## Phase 2: Future Code Cleanup (v0.5.0+) - POSTPONED

**Goal:** Complete removal of vJoy infrastructure from codebase

### Changes for LATER:

#### Source Code Cleanup
- Delete `src/DynamicVJoy.h` entirely
- Remove vJoy includes from `main.cpp`
- Remove vJoy config variables from `src/Config.h` and `src/Config.cpp`
- Remove vJoy GUI elements from `src/GuiLayer.cpp`
- Remove `CheckLegacyConflict()` from `src/GameConnector.h/.cpp`
- Remove all vJoy-related constants and state variables

#### Documentation Cleanup
- Archive old vJoy-related bug reports
- Update all dev_docs to remove vJoy references
- Clean up old installation guides

#### Testing
- Verify no vJoy code paths are executed
- Ensure config migration works for users with old vJoy settings
- Test that old config files don't cause errors

---

## Implementation Status

### Phase 1 (v0.4.15) - ✅ COMPLETE
- [x] Create implementation plan
- [x] Remove vJoy error popups from main.cpp
- [x] Remove rF2 plugin warning popup from main.cpp
- [x] Update README.md
- [x] Update README.txt
- [x] Update CHANGELOG.md
- [x] Test that app runs without popups
- [x] Verify DirectInput FFB still works

### Phase 2 (v0.5.0+) - POSTPONED
- [ ] Not started - deferred to future release

---

## Testing Checklist (Phase 1)

- [ ] App starts without any vJoy-related popups
- [ ] App starts without rF2 plugin warning
- [ ] DirectInput FFB works correctly
- [ ] No errors in console about vJoy
- [ ] README files are clear and don't mention vJoy/Gremlin
- [ ] Existing users with vJoy in config.ini don't get errors

---

**Current Status:** Phase 1 Implementation in Progress  
**Next Release:** v0.4.15 (User Experience Fixes Only)  
**Future Release:** v0.5.0+ (Complete Code Cleanup)

