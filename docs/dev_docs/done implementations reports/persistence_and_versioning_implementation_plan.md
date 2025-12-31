# Implementation Plan: Persistence Fixes & Versioning

**Plan Date:** 2025-12-31  
**Target Version:** v0.6.25+  
**Status Legend:** ✅ Already Fixed | ⚠️ Partially Fixed | ❌ Needs Implementation

## 1. Overview
This plan addresses persistence gaps where GUI settings are not properly saved to `config.ini` or handled within the preset system. It also introduces configuration versioning and comprehensive testing.

**Key Finding:** Some issues mentioned in the original audit have been **partially fixed** in the main config loading, but **preset loading still has bugs**. This plan focuses on completing the fixes.

## 2. Core Fixes (Persistence & Consistency)

### 2.1. Add `texture_load_cap` to Preset Structure ❌
**Status:** Missing from preset system (but correctly saved in main config)

**Current State:**
- ✅ Main config: Saved at line 426, loaded at line 574
- ❌ Preset struct: Not defined in `Preset` struct
- ❌ Preset methods: Not in `Apply()` or `UpdateFromEngine()`
- ❌ Preset serialization: Not in save/load loops

**Implementation:**

**File:** `src/Config.h` (around line 34, after `brake_load_cap`)
```cpp
float brake_load_cap = 3.0f;    // Existing line
float texture_load_cap = 1.5f;  // NEW: Add this field
```

**File:** `src/Config.h` (in `Preset::Apply()`, around line 205)
```cpp
engine.m_brake_load_cap = brake_load_cap;
engine.m_texture_load_cap = texture_load_cap;  // NEW: Add this line
engine.m_abs_pulse_enabled = abs_pulse_enabled;
```

**File:** `src/Config.h` (in `Preset::UpdateFromEngine()`, around line 265)
```cpp
brake_load_cap = engine.m_brake_load_cap;
texture_load_cap = engine.m_texture_load_cap;  // NEW: Add this line
abs_pulse_enabled = engine.m_abs_pulse_enabled;
```

**File:** `src/Config.cpp` (in preset save loop, after line 508)
```cpp
file << "brake_load_cap=" << p.brake_load_cap << "\n";
file << "texture_load_cap=" << p.texture_load_cap << "\n";  // NEW
file << "abs_pulse_enabled=" << p.abs_pulse_enabled << "\n";
```

**File:** `src/Config.cpp` (in `LoadPresets()` parser, after line 328)
```cpp
else if (key == "brake_load_cap") current_preset.brake_load_cap = (std::min)(10.0f, std::stof(value));
else if (key == "texture_load_cap") current_preset.texture_load_cap = std::stof(value);  // NEW
else if (key == "abs_pulse_enabled") current_preset.abs_pulse_enabled = std::stoi(value);
```

---

### 2.2. Add Missing Fields to Main Configuration ❌
**Status:** Four fields missing from main config save/load

**Current State:**
- ❌ `speed_gate_lower` - Not saved/loaded
- ❌ `speed_gate_upper` - Not saved/loaded
- ❌ `road_fallback_scale` - Not saved/loaded
- ❌ `understeer_affects_sop` - Not saved/loaded

**Implementation:**

**File:** `src/Config.cpp` (in `Config::Save()`, after line 473)
```cpp
file << "chassis_inertia_smoothing=" << engine.m_chassis_inertia_smoothing << "\n";
// NEW: Add these 4 lines
file << "speed_gate_lower=" << engine.m_speed_gate_lower << "\n";
file << "speed_gate_upper=" << engine.m_speed_gate_upper << "\n";
file << "road_fallback_scale=" << engine.m_road_fallback_scale << "\n";
file << "understeer_affects_sop=" << engine.m_understeer_affects_sop << "\n";

// 3. User Presets
file << "\n[Presets]\n";
```

**File:** `src/Config.cpp` (in `Config::Load()`, after line 626)
```cpp
else if (key == "chassis_inertia_smoothing") engine.m_chassis_inertia_smoothing = std::stof(value);
// NEW: Add these 4 lines
else if (key == "speed_gate_lower") engine.m_speed_gate_lower = std::stof(value);
else if (key == "speed_gate_upper") engine.m_speed_gate_upper = std::stof(value);
else if (key == "road_fallback_scale") engine.m_road_fallback_scale = std::stof(value);
else if (key == "understeer_affects_sop") engine.m_understeer_affects_sop = std::stoi(value);
```

---

### 2.3. Add Missing Fields to User Preset Serialization ❌
**Status:** Four fields exist in struct but not serialized

**Current State:**
- ✅ Fields exist in `Preset` struct
- ✅ Fields handled in `Apply()` and `UpdateFromEngine()`
- ❌ Fields missing from save loop
- ❌ Fields missing from load parser

**Implementation:**

**File:** `src/Config.cpp` (in preset save loop, after line 530)
```cpp
file << "chassis_inertia_smoothing=" << p.chassis_smoothing << "\n";
// NEW: Add these 4 lines
file << "speed_gate_lower=" << p.speed_gate_lower << "\n";
file << "speed_gate_upper=" << p.speed_gate_upper << "\n";
file << "road_fallback_scale=" << p.road_fallback_scale << "\n";
file << "understeer_affects_sop=" << p.understeer_affects_sop << "\n";
file << "\n";
```

**File:** `src/Config.cpp` (in `LoadPresets()` parser, after line 363)
```cpp
else if (key == "chassis_inertia_smoothing") current_preset.chassis_smoothing = std::stof(value);
// NEW: Add these 4 lines
else if (key == "speed_gate_lower") current_preset.speed_gate_lower = std::stof(value);
else if (key == "speed_gate_upper") current_preset.speed_gate_upper = std::stof(value);
else if (key == "road_fallback_scale") current_preset.road_fallback_scale = std::stof(value);
else if (key == "understeer_affects_sop") current_preset.understeer_affects_sop = std::stoi(value);
```

---

### 2.4. Fix Preset Parser Clamping Logic ❌
**Status:** Main config clamping is correct, but preset parser has bugs

**Current State:**
- ✅ `Config::Load()` correctly clamps `brake_load_cap` to [1.0, 10.0] at line 677
- ✅ `Config::Load()` correctly clamps `lockup_gain` to [0.0, 3.0] at line 674
- ❌ `LoadPresets()` incorrectly clamps `brake_load_cap` to 3.0 at line 328
- ❌ `LoadPresets()` incorrectly clamps `lockup_gain` to 2.0 at line 321

**Implementation:**

**File:** `src/Config.cpp` (line 328)
```cpp
// BEFORE:
else if (key == "brake_load_cap") current_preset.brake_load_cap = (std::min)(3.0f, std::stof(value));

// AFTER:
else if (key == "brake_load_cap") current_preset.brake_load_cap = (std::min)(10.0f, std::stof(value));
```

**File:** `src/Config.cpp` (line 321)
```cpp
// BEFORE:
else if (key == "lockup_gain") current_preset.lockup_gain = (std::min)(2.0f, std::stof(value));

// AFTER:
else if (key == "lockup_gain") current_preset.lockup_gain = (std::min)(3.0f, std::stof(value));
```

---

## 3. Configuration Versioning ❌

### 3.1. Add `ini_version` Field to Save
**File:** `src/Config.cpp` (in `Config::Save()`, after line 407)
```cpp
void Config::Save(const FFBEngine& engine, const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        // NEW: Add version tracking as first line
        file << "ini_version=" << LMUFFB_VERSION << "\n";
        
        file << "ignore_vjoy_version_warning=" << m_ignore_vjoy_version_warning << "\n";
        // ... rest of save logic
```

### 3.2. Parse `ini_version` in Load
**File:** `src/Config.cpp` (in `Config::Load()`, in parsing loop around line 557)
```cpp
try {
    if (key == "ignore_vjoy_version_warning") m_ignore_vjoy_version_warning = std::stoi(value);
    // NEW: Add version parsing
    else if (key == "ini_version") {
        // Store for future migration logic
        std::string config_version = value;
        std::cout << "[Config] Loading config version: " << config_version << std::endl;
    }
    else if (key == "enable_vjoy") m_enable_vjoy = std::stoi(value);
```

**Note:** Ensure `LMUFFB_VERSION` is accessible (defined in `GuiLayer.cpp` around line 110, may need to be moved to a header).

---

## 4. Test Specifications

### Test 1: Texture Load Cap in Presets
**Purpose:** Verify `texture_load_cap` is captured in user presets.

**Prerequisites:** Section 2.1 implemented.

**Test Steps:**
1. Create `FFBEngine` instance
2. Set `engine.m_texture_load_cap = 2.8`
3. Call `Config::AddUserPreset("TextureCapTest", engine)`
4. Open `config.ini` file
5. **Assert:** `[Preset:TextureCapTest]` section exists
6. **Assert:** Section contains line `texture_load_cap=2.8`
7. Create new `FFBEngine` with defaults
8. Call `Config::LoadPresets()`
9. Find preset index for "TextureCapTest"
10. Call `Config::ApplyPreset(index, engine)`
11. **Assert:** `engine.m_texture_load_cap == 2.8`

**Expected Result:** ✅ Pass after implementation

---

### Test 2: Main Config - Speed Gate Persistence
**Purpose:** Verify speed gate parameters persist in main config.

**Prerequisites:** Section 2.2 implemented.

**Test Steps:**
1. Create `FFBEngine` instance
2. Set `engine.m_speed_gate_lower = 2.5` (9.0 km/h)
3. Set `engine.m_speed_gate_upper = 7.0` (25.2 km/h)
4. Call `Config::Save(engine, "test_config.ini")`
5. Open `test_config.ini`
6. **Assert:** File contains `speed_gate_lower=2.5`
7. **Assert:** File contains `speed_gate_upper=7.0`
8. Create new `FFBEngine` with defaults
9. Call `Config::Load(engine, "test_config.ini")`
10. **Assert:** `engine.m_speed_gate_lower == 2.5`
11. **Assert:** `engine.m_speed_gate_upper == 7.0`

**Expected Result:** ✅ Pass after implementation

---

### Test 3: Main Config - Road Fallback & Understeer SoP
**Purpose:** Verify `road_fallback_scale` and `understeer_affects_sop` persist.

**Prerequisites:** Section 2.2 implemented.

**Test Steps:**
1. Create `FFBEngine` instance
2. Set `engine.m_road_fallback_scale = 0.12`
3. Set `engine.m_understeer_affects_sop = true`
4. Call `Config::Save(engine, "test_config.ini")`
5. **Assert:** File contains `road_fallback_scale=0.12`
6. **Assert:** File contains `understeer_affects_sop=1`
7. Create new `FFBEngine` with defaults
8. Call `Config::Load(engine, "test_config.ini")`
9. **Assert:** `engine.m_road_fallback_scale == 0.12`
10. **Assert:** `engine.m_understeer_affects_sop == true`

**Expected Result:** ✅ Pass after implementation

---

### Test 4: Preset Serialization - All New Fields
**Purpose:** Verify all 5 new fields are saved and loaded in presets.

**Prerequisites:** Sections 2.1 and 2.3 implemented.

**Test Steps:**
1. Create `FFBEngine` instance
2. Set all 5 fields to non-default values:
   - `engine.m_texture_load_cap = 2.2`
   - `engine.m_speed_gate_lower = 3.0`
   - `engine.m_speed_gate_upper = 9.0`
   - `engine.m_road_fallback_scale = 0.08`
   - `engine.m_understeer_affects_sop = true`
3. Call `Config::AddUserPreset("AllFieldsTest", engine)`
4. Verify `config.ini` contains all 5 fields in preset section
5. Create new `FFBEngine` with defaults
6. Load and apply "AllFieldsTest" preset
7. **Assert:** All 5 fields match original values

**Expected Result:** ✅ Pass after implementation

---

### Test 5: Preset Clamping - Brake Load Cap (Regression)
**Purpose:** Verify preset parser doesn't clamp `brake_load_cap` to 3.0.

**Prerequisites:** Section 2.4 implemented.

**Test Steps:**
1. Manually create `config.ini` with:
   ```ini
   [Preset:HighBrake]
   brake_load_cap=8.5
   ```
2. Call `Config::LoadPresets()`
3. Find preset "HighBrake"
4. **Assert:** `preset.brake_load_cap == 8.5` (not 3.0)
5. Apply preset to engine
6. **Assert:** `engine.m_brake_load_cap == 8.5`

**Expected Result:** ✅ Pass after fix (currently fails - clamped to 3.0)

---

### Test 6: Preset Clamping - Lockup Gain (Regression)
**Purpose:** Verify preset parser doesn't clamp `lockup_gain` to 2.0.

**Prerequisites:** Section 2.4 implemented.

**Test Steps:**
1. Manually create `config.ini` with:
   ```ini
   [Preset:HighLockup]
   lockup_gain=2.9
   ```
2. Call `Config::LoadPresets()`
3. Find preset "HighLockup"
4. **Assert:** `preset.lockup_gain == 2.9` (not 2.0)
5. Apply preset to engine
6. **Assert:** `engine.m_lockup_gain == 2.9`

**Expected Result:** ✅ Pass after fix (currently fails - clamped to 2.0)

---

### Test 7: Main Config Clamping - Brake Load Cap (Regression)
**Purpose:** Verify main config correctly clamps `brake_load_cap` to [1.0, 10.0].

**Prerequisites:** None (already implemented correctly).

**Test Steps:**
1. Create `config.ini` with `brake_load_cap=6.5`
2. Call `Config::Load(engine)`
3. **Assert:** `engine.m_brake_load_cap == 6.5` (within range)
4. Create `config.ini` with `brake_load_cap=15.0`
5. Call `Config::Load(engine)`
6. **Assert:** `engine.m_brake_load_cap == 10.0` (clamped to max)
7. Create `config.ini` with `brake_load_cap=0.5`
8. Call `Config::Load(engine)`
9. **Assert:** `engine.m_brake_load_cap == 1.0` (clamped to min)

**Expected Result:** ✅ Already passes (lines 677-679)

---

### Test 8: Main Config Clamping - Lockup Gain (Regression)
**Purpose:** Verify main config correctly clamps `lockup_gain` to [0.0, 3.0].

**Prerequisites:** None (already implemented correctly).

**Test Steps:**
1. Create `config.ini` with `lockup_gain=2.7`
2. Call `Config::Load(engine)`
3. **Assert:** `engine.m_lockup_gain == 2.7` (within range)
4. Create `config.ini` with `lockup_gain=5.0`
5. Call `Config::Load(engine)`
6. **Assert:** `engine.m_lockup_gain == 3.0` (clamped to max)

**Expected Result:** ✅ Already passes (lines 674-676)

---

### Test 9: Configuration Versioning
**Purpose:** Verify `ini_version` field is saved and parsed.

**Prerequisites:** Section 3 implemented.

**Test Steps:**
1. Call `Config::Save(engine, "test_config.ini")`
2. Open `test_config.ini`
3. **Assert:** First line is `ini_version=` followed by version string
4. Call `Config::Load(engine, "test_config.ini")`
5. **Assert:** No parsing errors
6. **Assert:** Console output shows "Loading config version: ..."

**Expected Result:** ✅ Pass after implementation

---

### Test 10: Comprehensive Round-Trip Test
**Purpose:** Verify ALL adjustable parameters survive save/load cycle.

**Prerequisites:** All sections implemented.

**Test Steps:**
1. Create `FFBEngine` instance
2. Set every adjustable parameter to a unique, non-default value
3. Call `Config::Save(engine, "roundtrip.ini")`
4. Create new `FFBEngine` with defaults
5. Call `Config::Load(engine, "roundtrip.ini")`
6. **Assert:** Every parameter matches original value
7. Call `Config::AddUserPreset("RoundTrip", engine)`
8. Create new `FFBEngine` with defaults
9. Load and apply "RoundTrip" preset
10. **Assert:** Every parameter matches original value

**Expected Result:** ✅ Pass after all implementations

---

## 5. Implementation Checklist

- [ ] **Step 1:** Add `texture_load_cap` to `Preset` struct (Section 2.1)
- [ ] **Step 2:** Add 4 fields to main config save/load (Section 2.2)
- [ ] **Step 3:** Add 4 fields to preset serialization (Section 2.3)
- [ ] **Step 4:** Fix preset parser clamping (Section 2.4)
- [ ] **Step 5:** Add `ini_version` to save (Section 3.1)
- [ ] **Step 6:** Parse `ini_version` in load (Section 3.2)
- [ ] **Step 7:** Run all 10 test specifications
- [ ] **Step 8:** Update `CHANGELOG.md` with fixes
- [ ] **Step 9:** Increment version to v0.6.25

---

## 6. Appendix: Implementation Priority

| Priority | Section | Complexity | Impact | Risk |
|----------|---------|------------|--------|------|
| **P0** | 2.4 | Low | High | Low |
| **P1** | 2.1 | Medium | High | Low |
| **P1** | 2.2 | Low | High | Low |
| **P1** | 2.3 | Low | High | Low |
| **P2** | 3.1, 3.2 | Low | Medium | Low |

**Recommendation:** Implement in order P0 → P1 → P2, running tests after each section.
