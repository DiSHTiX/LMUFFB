# Implementation Plan: Persistence Fixes & Versioning

## 1. Overview
This plan addresses the persistence gaps where certain GUI settings are not saved to the `config.ini` file or properly handled within the preset system. It also introduces automatic versioning for the configuration file and a comprehensive test suite to ensure all GUI elements are correctly synchronized with the disk.

## 2. Core Fixes (Persistence & Consistency)

### 2.1. Update `Preset` Structure (`src/Config.h`)
Add the missing fields to the `Preset` struct and update the `Apply` and `UpdateFromEngine` methods.
*   **Field**: `float texture_load_cap = 1.5f;`
*   **Update `Preset::Apply`**: Ensure `engine.m_texture_load_cap = texture_load_cap;`.
*   **Update `Preset::UpdateFromEngine`**: Ensure `texture_load_cap = engine.m_texture_load_cap;`.

### 2.2. Main Configuration Save/Load (`src/Config.cpp`)
Update `Config::Save` and `Config::Load` to include parameters that were previously excluded:
*   `speed_gate_lower`
*   `speed_gate_upper`
*   `road_fallback_scale`
*   `understeer_affects_sop`

### 2.3. User Preset Serialization (`src/Config.cpp`)
Update the user preset loop in `Config::Save` and the parser in `Config::LoadPresets` to include:
*   `texture_load_cap`
*   `speed_gate_lower`
*   `speed_gate_upper`
*   `road_fallback_scale`
*   `understeer_affects_sop`

### 2.4. Fix Parser Clamping Logic
Correct the artificial limits in `Config::LoadPresets` (parser) to match GUI ranges:
*   `brake_load_cap`: Change `(std::min)(3.0f, ...)` to `(std::min)(10.0f, ...)`.
*   `lockup_gain`: Change `(std::min)(2.0f, ...)` to `(std::min)(3.0f, ...)`.

## 3. Configuration Versioning
To handle future format changes, we will save the current app version in the `ini` file.

### 3.1. `Config::Save`
Add a new field at the beginning of the file:
```cpp
file << "ini_version=" << LMUFFB_VERSION << "\n";
```
(Note: Ensure `LMUFFB_VERSION` or equivalent is accessible, likely defined in `FFBEngine.h` or a dedicated version header).

### 3.2. `Config::Load`
Parse the `ini_version` field. If missing, assume it's pre-v0.6.24. This will allow for conditional logic if we ever need to migrate old values.

## 4. Testing Plan

### 4.1. Automated Sustainability Test (`tests/test_persistence.cpp`)
Create a new test file that performs the following steps:
1.  **Engine State Randomization**: Set every adjustable member of `FFBEngine` to a unique, non-default value within its valid range.
2.  **Save Main Config**: Call `Config::Save()`.
3.  **Reset Engine**: Apply default settings to the engine.
4.  **Load Main Config**: Call `Config::Load()`.
5.  **Verify Consistency**: Compare every engine member against the saved state. Fail if ANY value does not match.
6.  **Preset Verification**:
    *   Create a user preset with specific values.
    *   Save and Load presets.
    *   Apply the preset and verify the engine state matches.

### 4.2. Manual Verification
*   Adjust a slider in the GUI.
*   Restart the application.
*   Verify the slider position is preserved.
*   Create a custom preset, change settings, switch presets back and forth.
*   Verify the custom preset restores exactly the values that were saved.

## 5. Implementation Steps
1.  **Step 1**: Modify `Config.h` to add `texture_load_cap` to `Preset`.
2.  **Step 2**: Synchronize all save/load loops in `Config.cpp`.
3.  **Step 3**: Fix the clamping constants in `LoadPresets`.
4.  **Step 4**: Add the `ini_version` field to `Save`.
5.  **Step 5**: Implement the persistence unit tests.
