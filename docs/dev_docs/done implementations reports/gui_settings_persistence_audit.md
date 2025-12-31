# Audit Report: GUI Settings Persistence & Consistency

**Audit Date:** 2025-12-31  
**Code Version:** v0.6.24+  
**Status Legend:** ✅ Fixed | ⚠️ Partially Fixed | ❌ Not Fixed

## 1. Executive Summary
This report documents a comprehensive audit of GUI settings persistence, comparing interactive settings in `src/GuiLayer.cpp` against persistence logic in `src/Config.cpp` and the `Preset` structure in `src/Config.h`. 

**Key Distinction:** This audit differentiates between:
- **Main Configuration** (`config.ini` root section) - Active settings that persist across sessions
- **User Presets** (`config.ini` `[Preset:Name]` sections) - Saved snapshots of settings

## 2. Identified Persistence Issues

### 2.1. Settings Missing from Main Configuration ❌
The following settings can be adjusted in the GUI but are **never saved** to the main `config.ini` section. They reset to defaults on every application restart.

| GUI Setting | Internal Variable | Status | Location |
| :--- | :--- | :--- | :--- |
| **Mute Below (Speed Gate)** | `engine.m_speed_gate_lower` | ❌ **Missing** | `Config::Save()` line ~473 |
| **Full Above (Speed Gate)** | `engine.m_speed_gate_upper` | ❌ **Missing** | `Config::Save()` line ~473 |
| **Road Fallback Scale** | `engine.m_road_fallback_scale` | ❌ **Missing** | `Config::Save()` line ~473 |
| **Understeer Affects SoP** | `engine.m_understeer_affects_sop` | ❌ **Missing** | `Config::Save()` line ~473 |

**Note:** `texture_load_cap` **IS** correctly saved/loaded in main config (lines 426, 574).

### 2.2. Settings Missing from User Presets ❌
Custom presets fail to capture several settings. While these fields exist in the `Preset` struct and are handled by `Apply()`/`UpdateFromEngine()`, they are **missing from serialization loops**.

| Setting | In Struct? | In Apply()? | In Save Loop? | In Load Loop? | Impact |
| :--- | :---: | :---: | :---: | :---: | :--- |
| **Texture Load Cap** | ❌ | ❌ | ❌ | ❌ | Not captured in presets at all |
| **Speed Gate Lower** | ✅ | ✅ | ❌ | ❌ | Struct exists but not serialized |
| **Speed Gate Upper** | ✅ | ✅ | ❌ | ❌ | Struct exists but not serialized |
| **Road Fallback Scale** | ✅ | ✅ | ❌ | ❌ | Struct exists but not serialized |
| **Understeer Affects SoP** | ✅ | ✅ | ❌ | ❌ | Struct exists but not serialized |

**Files Affected:**
- `Config.h` lines 8-306 (Preset struct)
- `Config.cpp` lines 477-533 (Save loop - missing fields)
- `Config.cpp` lines 305-367 (LoadPresets parser - missing fields)

### 2.3. Data Validation & Clamping Bugs ⚠️
Inconsistencies exist between GUI ranges and preset loading clamps. **Main config loading is correct**, but preset loading has bugs.

| Parameter | GUI Max | Main Config Clamp | Preset Load Clamp | Status |
| :--- | :---: | :---: | :---: | :--- |
| **Brake Load Cap** | 10.0x | ✅ `[1.0, 10.0]` (line 677) | ❌ `3.0` (line 328) | ⚠️ Preset bug |
| **Lockup Gain** | 3.0x | ✅ `[0.0, 3.0]` (line 674) | ❌ `2.0` (line 321) | ⚠️ Preset bug |

**Impact:** User presets saved with `brake_load_cap=5.0` or `lockup_gain=2.5` will be incorrectly clamped when loaded.

### 2.4. Missing Configuration Versioning ❌
No version tracking exists in the config file format.

| Feature | Status | Impact |
| :--- | :--- | :--- |
| `ini_version` field | ❌ Not implemented | Cannot detect config format version |
| Migration logic | ❌ Not implemented | Future format changes will break old configs |

## 3. Workflow & UX Issues

### 3.1. Lack of Auto-Save ❌
The application relies on manual saving for most parameters.

**Problem:** Changes to sliders are held in memory only. If the app crashes or closes without clicking "Save Current Config", changes are lost.

**Exception:** Only toggling graphs or selecting a device triggers immediate `Config::Save()`.

### 3.2. Unsaved State on Shutdown ❌
`GuiLayer::Shutdown()` captures window geometry but **does not call `Config::Save()`**.

**Result:** Final slider adjustments and window position changes are lost unless the user manually saved.

## 4. Implementation Recommendations

### Priority 1: Fix Preset Clamping Bugs (CRITICAL)
**File:** `src/Config.cpp`, function `LoadPresets()`
- Line 328: Change `(std::min)(3.0f, std::stof(value))` → `(std::min)(10.0f, std::stof(value))`
- Line 321: Change `(std::min)(2.0f, std::stof(value))` → `(std::min)(3.0f, std::stof(value))`

### Priority 2: Add Missing Fields to Preset Serialization
**File:** `src/Config.h`
- Add `float texture_load_cap = 1.5f;` to `Preset` struct (around line 34)
- Update `Preset::Apply()` to include `engine.m_texture_load_cap = texture_load_cap;`
- Update `Preset::UpdateFromEngine()` to include `texture_load_cap = engine.m_texture_load_cap;`

**File:** `src/Config.cpp`
- Add 5 missing fields to preset save loop (after line 530):
  ```cpp
  file << "texture_load_cap=" << p.texture_load_cap << "\n";
  file << "speed_gate_lower=" << p.speed_gate_lower << "\n";
  file << "speed_gate_upper=" << p.speed_gate_upper << "\n";
  file << "road_fallback_scale=" << p.road_fallback_scale << "\n";
  file << "understeer_affects_sop=" << p.understeer_affects_sop << "\n";
  ```
- Add 5 missing fields to preset load loop (after line 363):
  ```cpp
  else if (key == "texture_load_cap") current_preset.texture_load_cap = std::stof(value);
  else if (key == "speed_gate_lower") current_preset.speed_gate_lower = std::stof(value);
  else if (key == "speed_gate_upper") current_preset.speed_gate_upper = std::stof(value);
  else if (key == "road_fallback_scale") current_preset.road_fallback_scale = std::stof(value);
  else if (key == "understeer_affects_sop") current_preset.understeer_affects_sop = std::stoi(value);
  ```

### Priority 3: Add Missing Fields to Main Config
**File:** `src/Config.cpp`
- Add to `Config::Save()` (after line 473):
  ```cpp
  file << "speed_gate_lower=" << engine.m_speed_gate_lower << "\n";
  file << "speed_gate_upper=" << engine.m_speed_gate_upper << "\n";
  file << "road_fallback_scale=" << engine.m_road_fallback_scale << "\n";
  file << "understeer_affects_sop=" << engine.m_understeer_affects_sop << "\n";
  ```
- Add to `Config::Load()` (after line 626):
  ```cpp
  else if (key == "speed_gate_lower") engine.m_speed_gate_lower = std::stof(value);
  else if (key == "speed_gate_upper") engine.m_speed_gate_upper = std::stof(value);
  else if (key == "road_fallback_scale") engine.m_road_fallback_scale = std::stof(value);
  else if (key == "understeer_affects_sop") engine.m_understeer_affects_sop = std::stoi(value);
  ```

### Priority 4: Implement Configuration Versioning
**File:** `src/Config.cpp`
- Add to `Config::Save()` (at beginning, after line 407):
  ```cpp
  file << "ini_version=" << LMUFFB_VERSION << "\n";
  ```
- Add to `Config::Load()` (in parsing loop):
  ```cpp
  else if (key == "ini_version") {
      // Store for future migration logic
      std::string config_version = value;
  }
  ```

### Priority 5: Implement Auto-Save on Shutdown
**File:** `src/GuiLayer.cpp`
- Add `Config::Save(engine)` call in `GuiLayer::Shutdown()` before window cleanup

## 5. Test Specifications

### Test 1: Main Config Persistence - Speed Gate Parameters
**Purpose:** Verify `speed_gate_lower` and `speed_gate_upper` persist across sessions.

**Test Steps:**
1. Launch application, load default config
2. Set `speed_gate_lower = 2.0` (7.2 km/h)
3. Set `speed_gate_upper = 8.0` (28.8 km/h)
4. Call `Config::Save(engine)`
5. Create new `FFBEngine` instance
6. Call `Config::Load(engine)`
7. **Assert:** `engine.m_speed_gate_lower == 2.0`
8. **Assert:** `engine.m_speed_gate_upper == 8.0`

**Expected Result:** ❌ **Currently Fails** - Values reset to defaults (1.0, 5.0)

---

### Test 2: Main Config Persistence - Road Fallback & Understeer SoP
**Purpose:** Verify `road_fallback_scale` and `understeer_affects_sop` persist.

**Test Steps:**
1. Launch application
2. Set `road_fallback_scale = 0.15`
3. Set `understeer_affects_sop = true`
4. Call `Config::Save(engine)`
5. Create new `FFBEngine` instance
6. Call `Config::Load(engine)`
7. **Assert:** `engine.m_road_fallback_scale == 0.15`
8. **Assert:** `engine.m_understeer_affects_sop == true`

**Expected Result:** ❌ **Currently Fails** - Values reset to defaults

---

### Test 3: Preset Persistence - Texture Load Cap
**Purpose:** Verify `texture_load_cap` is saved and restored in user presets.

**Test Steps:**
1. Launch application
2. Set `engine.m_texture_load_cap = 2.5`
3. Call `Config::AddUserPreset("TestPreset", engine)`
4. Verify `config.ini` contains `[Preset:TestPreset]` section
5. **Assert:** Section contains `texture_load_cap=2.5`
6. Restart application, call `Config::LoadPresets()`
7. Find preset named "TestPreset"
8. **Assert:** `preset.texture_load_cap == 2.5`
9. Apply preset to engine
10. **Assert:** `engine.m_texture_load_cap == 2.5`

**Expected Result:** ❌ **Currently Fails** - Field not in struct or serialization

---

### Test 4: Preset Persistence - Speed Gate in Presets
**Purpose:** Verify speed gate parameters are saved in user presets.

**Test Steps:**
1. Set `engine.m_speed_gate_lower = 3.0`
2. Set `engine.m_speed_gate_upper = 10.0`
3. Call `Config::AddUserPreset("SpeedGateTest", engine)`
4. **Assert:** `config.ini` contains `speed_gate_lower=3.0`
5. **Assert:** `config.ini` contains `speed_gate_upper=10.0`
6. Reload presets, apply "SpeedGateTest"
7. **Assert:** `engine.m_speed_gate_lower == 3.0`
8. **Assert:** `engine.m_speed_gate_upper == 10.0`

**Expected Result:** ❌ **Currently Fails** - Fields not serialized

---

### Test 5: Preset Clamping - Brake Load Cap (Regression Test)
**Purpose:** Verify preset loading doesn't incorrectly clamp `brake_load_cap` to 3.0.

**Test Steps:**
1. Manually create `config.ini` with:
   ```ini
   [Preset:HighBrakeCap]
   brake_load_cap=7.5
   ```
2. Call `Config::LoadPresets()`
3. Find preset "HighBrakeCap"
4. **Assert:** `preset.brake_load_cap == 7.5` (not 3.0)
5. Apply preset to engine
6. **Assert:** `engine.m_brake_load_cap == 7.5`

**Expected Result:** ❌ **Currently Fails** - Clamped to 3.0 at line 328

---

### Test 6: Preset Clamping - Lockup Gain (Regression Test)
**Purpose:** Verify preset loading doesn't incorrectly clamp `lockup_gain` to 2.0.

**Test Steps:**
1. Manually create `config.ini` with:
   ```ini
   [Preset:HighLockup]
   lockup_gain=2.8
   ```
2. Call `Config::LoadPresets()`
3. Find preset "HighLockup"
4. **Assert:** `preset.lockup_gain == 2.8` (not 2.0)
5. Apply preset to engine
6. **Assert:** `engine.m_lockup_gain == 2.8`

**Expected Result:** ❌ **Currently Fails** - Clamped to 2.0 at line 321

---

### Test 7: Main Config Clamping - Brake Load Cap (Regression Test)
**Purpose:** Verify main config loading correctly clamps `brake_load_cap` to [1.0, 10.0].

**Test Steps:**
1. Manually create `config.ini` with `brake_load_cap=7.5`
2. Call `Config::Load(engine)`
3. **Assert:** `engine.m_brake_load_cap == 7.5`
4. Test boundary: Set `brake_load_cap=15.0` in file
5. Call `Config::Load(engine)`
6. **Assert:** `engine.m_brake_load_cap == 10.0` (clamped)

**Expected Result:** ✅ **Currently Passes** - Correctly clamped at lines 677-679

---

### Test 8: Main Config Clamping - Lockup Gain (Regression Test)
**Purpose:** Verify main config loading correctly clamps `lockup_gain` to [0.0, 3.0].

**Test Steps:**
1. Manually create `config.ini` with `lockup_gain=2.8`
2. Call `Config::Load(engine)`
3. **Assert:** `engine.m_lockup_gain == 2.8`
4. Test boundary: Set `lockup_gain=5.0` in file
5. Call `Config::Load(engine)`
6. **Assert:** `engine.m_lockup_gain == 3.0` (clamped)

**Expected Result:** ✅ **Currently Passes** - Correctly clamped at lines 674-676

---

### Test 9: Configuration Versioning
**Purpose:** Verify `ini_version` field is saved and can be read.

**Test Steps:**
1. Call `Config::Save(engine)`
2. Open `config.ini`
3. **Assert:** First line contains `ini_version=` followed by version string
4. Call `Config::Load(engine)`
5. **Assert:** Version is parsed without errors

**Expected Result:** ❌ **Currently Fails** - No versioning implemented

---

### Test 10: Comprehensive Preset Round-Trip Test
**Purpose:** Verify ALL preset fields survive save/load cycle.

**Test Steps:**
1. Create `FFBEngine` with randomized values for ALL adjustable parameters
2. Call `Config::AddUserPreset("FullTest", engine)`
3. Create new `FFBEngine` with defaults
4. Load and apply "FullTest" preset
5. **Assert:** Every single parameter matches original engine state

**Expected Result:** ❌ **Currently Fails** - 5 fields missing from serialization

---

## 6. Appendix: Field Coverage Matrix

| Field Name | Main Save | Main Load | Preset Struct | Preset Save | Preset Load | Apply() | UpdateFrom() |
|------------|:---------:|:---------:|:-------------:|:-----------:|:-----------:|:-------:|:------------:|
| `texture_load_cap` | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ |
| `speed_gate_lower` | ❌ | ❌ | ✅ | ❌ | ❌ | ✅ | ✅ |
| `speed_gate_upper` | ❌ | ❌ | ✅ | ❌ | ❌ | ✅ | ✅ |
| `road_fallback_scale` | ❌ | ❌ | ✅ | ❌ | ❌ | ✅ | ✅ |
| `understeer_affects_sop` | ❌ | ❌ | ✅ | ❌ | ❌ | ✅ | ✅ |

**Legend:** ✅ Implemented | ❌ Missing
