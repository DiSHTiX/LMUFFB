# Implementation Plan: Signal Processing Enhancements from AIRA

## Context

This plan describes the implementation of two signal processing improvements adopted from Marvin's AIRA (Advanced iRacing Assistant):

1. **Smoothstep Speed Gating**: Replace the current linear speed gate interpolation with a smooth S-curve (Hermite interpolation) for more natural FFB transitions.

2. **Min/Max Threshold Mapping (InverseLerp)**: Enhance the Slope Detection grip-to-effect conversion with a configurable min/max threshold system that provides a dead zone, linear response region, and saturation.

### Problem Statement

**Current Linear Speed Gate Issues:**
- The existing speed gate uses linear interpolation: `(speed - lower) / range`
- This creates perceivable "kinks" at the lower and upper thresholds
- Sudden transition from 0% to linear ramp feels unnatural
- Users report abrupt FFB changes when accelerating from pit lane

**Current Slope Detection Threshold Issues:**
- Uses a single threshold (`m_slope_negative_threshold`) with linear excess calculation
- No dead zone to filter minor sensor noise
- No saturation point - effect intensity scales indefinitely
- Less predictable tuning compared to AIRA's min/max approach

### Proposed Solution

1. **Smoothstep Speed Gate**: Apply `t² × (3 - 2t)` transformation to the normalized speed value
2. **InverseLerp Threshold System**: Add `m_slope_min_threshold` and `m_slope_max_threshold` parameters

---

## Reference Documents

1. **Source Implementation:**
   - `docs/dev_docs/tech_from_other_apps/Marvin's AIRA Refactored FFB_Effects_Technical_Report.md` - Section "Signal Processing Techniques"

2. **Comparison Report:**
   - `docs/dev_docs/investigations/aira_vs_lmuffb_comparison_report.md`

3. **Related Implementation Plans:**
   - `docs/dev_docs/implementation_plans/plan_slope_detection.md`
   - `docs/dev_docs/implementation_plans/plan_slope_detection_fixes_v0.7.1.md`

---

## Codebase Analysis Summary

### Current Speed Gate Implementation

**Location:** `FFBEngine.h` lines 1036-1040

```cpp
// Current Implementation (Linear)
double speed_gate_range = (double)m_speed_gate_upper - (double)m_speed_gate_lower;
if (speed_gate_range < 0.1) speed_gate_range = 0.1;
ctx.speed_gate = (ctx.car_speed - (double)m_speed_gate_lower) / speed_gate_range;
ctx.speed_gate = (std::max)(0.0, (std::min)(1.0, ctx.speed_gate));
```

**Usage Points (Effects that apply speed_gate):**

| Effect | Location | Application |
|--------|----------|-------------|
| Understeer | FFBEngine.h:1075 | `output_force *= ctx.speed_gate` |
| SoP Lateral | FFBEngine.h:1347 | `ctx.sop_base_force *= ctx.speed_gate` |
| Rear Torque | FFBEngine.h:1348 | `ctx.rear_torque *= ctx.speed_gate` |
| Yaw Kick | FFBEngine.h:1349 | `ctx.yaw_force *= ctx.speed_gate` |
| ABS Pulse | FFBEngine.h:1382 | `* ctx.speed_gate` |
| Lockup Vibration | FFBEngine.h:1457 | `* ctx.speed_gate` |
| Road Texture | FFBEngine.h:1541 | `ctx.road_noise *= ctx.speed_gate` |
| Bottoming | FFBEngine.h:1579 | `* ctx.speed_gate` |

### Current Slope Detection Threshold

**Location:** `FFBEngine.h` lines 846-851

```cpp
// Current Implementation (Single Threshold)
if (m_slope_current < (double)m_slope_negative_threshold) {
    // Slope is negative -> tire is sliding
    double excess = (double)m_slope_negative_threshold - m_slope_current;
    current_grip_factor = 1.0 - (excess * 0.1 * (double)m_slope_sensitivity);
}
```

**Issues:**
1. No dead zone - any slope below threshold triggers effect
2. No maximum threshold - effect scales with `excess * 0.1 * sensitivity`
3. Sensitivity is arbitrary multiplier, not a clear range

### Settings Architecture

| Setting | FFBEngine.h | Config.h | Preset |
|---------|-------------|----------|--------|
| `m_speed_gate_lower` | Line 306 | Line 96 | ✓ |
| `m_speed_gate_upper` | Line 307 | Line 97 | ✓ |
| `m_slope_negative_threshold` | Line 319 | Line 107 | ✓ |
| `m_slope_sensitivity` | Line 318 | Line 106 | ✓ |

---

## FFB Effects Impact Analysis

### Part 1: Smoothstep Speed Gating

**Affected Effects:** ALL effects that use `ctx.speed_gate`

| Effect | Technical Impact | User Experience |
|--------|-----------------|-----------------|
| **Understeer** | Speed gate value changes from linear to S-curve | Smoother FFB engagement when accelerating from pit lane |
| **SoP Lateral** | Same transformation | More natural lateral feeling at low speeds |
| **Rear Torque** | Same transformation | Less abrupt rear-end perception activation |
| **Yaw Kick** | Same transformation | Gentler oversteer snap-back at low speeds |
| **ABS Pulse** | Same transformation | Smoother ABS feedback onset |
| **Lockup Vibration** | Same transformation | Less jarring brake lockup feel at slow speeds |
| **Road Texture** | Same transformation | Gradual road feel onset when leaving pits |
| **Bottoming** | Same transformation | Smoother curb/bump engagement |

**Expected Feel Change:**
- Effects fade in more smoothly from 0-20 kph range
- No perceivable "click" at the transition points
- Pit lane driving feels more polished and premium
- All current presets remain compatible (no setting changes)

### Part 2: Min/Max Threshold Mapping

**Affected Effects:** Slope Detection grip estimation (feeds into Understeer Effect)

| Aspect | Current Behavior | New Behavior |
|--------|------------------|--------------|
| Dead Zone | None | Slopes between 0 and min_threshold ignored |
| Response Curve | Linear with arbitrary sensitivity | Linear between min and max thresholds |
| Saturation | None (scales indefinitely) | Effect saturates at max_threshold |
| Tuning | Abstract "sensitivity" value | Intuitive min/max range in same units as slope |

**Expected Feel Change:**
- More predictable understeer effect activation
- Small slope variations (noise) won't trigger effect
- Effect reaches maximum at defined threshold
- Tuning becomes more intuitive (set min = "start feeling it", max = "maximum effect")

---

## Proposed Changes

### Change 1: Smoothstep Speed Gate (Core Algorithm)

**File:** `FFBEngine.h`  
**Location:** Lines 1036-1040 (inside `calculate_force()`)

**Pseudo-code:**
```cpp
// BEFORE: Linear interpolation
ctx.speed_gate = (ctx.car_speed - (double)m_speed_gate_lower) / speed_gate_range;
ctx.speed_gate = (std::max)(0.0, (std::min)(1.0, ctx.speed_gate));

// AFTER: Smoothstep interpolation
double t = (ctx.car_speed - (double)m_speed_gate_lower) / speed_gate_range;
t = (std::max)(0.0, (std::min)(1.0, t));  // Clamp to [0, 1]
ctx.speed_gate = t * t * (3.0 - 2.0 * t);  // Hermite S-curve
```

**No new settings required.** Existing `m_speed_gate_lower` and `m_speed_gate_upper` continue to control the range.

### Change 2: Add Helper Function for Smoothstep

**File:** `FFBEngine.h`  
**Location:** After line 790 (helper functions section)

**Pseudo-code:**
```cpp
// Helper: Smoothstep interpolation - v0.8.0
// Returns 0 when x <= edge0, 1 when x >= edge1
// Uses Hermite polynomial: t²(3 - 2t) for smooth fade
inline double smoothstep(double edge0, double edge1, double x) {
    double range = edge1 - edge0;
    if (range < 0.0001) return (x < edge0) ? 0.0 : 1.0;
    
    double t = (x - edge0) / range;
    t = (std::max)(0.0, (std::min)(1.0, t));
    return t * t * (3.0 - 2.0 * t);
}
```

### Change 3: Add Helper Function for InverseLerp

**File:** `FFBEngine.h`  
**Location:** After smoothstep helper

**Pseudo-code:**
```cpp
// Helper: Inverse linear interpolation - v0.8.0
// Returns normalized position of value between min and max
// Returns 0 if value <= min, 1 if value >= max
inline double inverse_lerp(double min_val, double max_val, double value) {
    double range = max_val - min_val;
    if (range < 0.0001) return (value < min_val) ? 0.0 : 1.0;
    
    double t = (value - min_val) / range;
    return (std::max)(0.0, (std::min)(1.0, t));
}
```

### Change 4: Min/Max Threshold for Slope Detection

**File:** `FFBEngine.h`  
**New Settings:** Add after line 319

```cpp
// ===== SLOPE DETECTION (v0.7.0 → v0.8.0 enhancements) =====
bool m_slope_detection_enabled = false;
int m_slope_sg_window = 15;
float m_slope_sensitivity = 0.5f;            // DEPRECATED (v0.8.0) - kept for config compat
float m_slope_negative_threshold = -0.3f;    // v0.8.0: Becomes m_slope_min_threshold
float m_slope_smoothing_tau = 0.04f;

// NEW v0.8.0: Min/Max Threshold System (replaces single threshold + sensitivity)
float m_slope_min_threshold = -0.3f;  // Slope below this: effect starts (dead zone edge)
float m_slope_max_threshold = -2.0f;  // Slope at or below this: effect at 100%
```

**File:** `FFBEngine.h`  
**Modified Function:** `calculate_slope_grip()` (lines 846-851)

**Pseudo-code:**
```cpp
// BEFORE: Single threshold with excess calculation
if (m_slope_current < (double)m_slope_negative_threshold) {
    double excess = (double)m_slope_negative_threshold - m_slope_current;
    current_grip_factor = 1.0 - (excess * 0.1 * (double)m_slope_sensitivity);
}

// AFTER: Min/Max threshold with inverse_lerp
double current_grip_factor = 1.0;
if (m_slope_current < (double)m_slope_min_threshold) {
    // Slope is below minimum (dead zone) -> calculate effect intensity
    // Note: More negative slope = more grip loss
    // m_slope_min_threshold = -0.3 (start of effect)
    // m_slope_max_threshold = -2.0 (full effect)
    double effect_intensity = inverse_lerp(
        (double)m_slope_min_threshold,  // min (starts at 0%)
        (double)m_slope_max_threshold,  // max (reaches 100%)
        m_slope_current                 // current value
    );
    current_grip_factor = 1.0 - effect_intensity;
}
```

---

## Parameter Synchronization Checklist

### New Parameters: `m_slope_min_threshold`, `m_slope_max_threshold`

| Step | File | Status |
|------|------|--------|
| Declaration in FFBEngine.h | `float m_slope_min_threshold = -0.3f;` | Required |
| Declaration in FFBEngine.h | `float m_slope_max_threshold = -2.0f;` | Required |
| Declaration in Preset struct (Config.h) | `float slope_min_threshold = -0.3f;` | Required |
| Declaration in Preset struct (Config.h) | `float slope_max_threshold = -2.0f;` | Required |
| Entry in `Preset::SetSlopeDetection()` | Add new parameters | Required |
| Entry in `Preset::Apply()` | Copy to engine | Required |
| Entry in `Preset::UpdateFromEngine()` | Copy from engine | Required |
| Entry in `Config::Save()` | Write to file | Required |
| Entry in `Config::Load()` | Read from file | Required |
| Validation logic | Ensure min > max (more negative) | Required |

### Migration Logic for Deprecated `m_slope_sensitivity`

**File:** `Config.cpp` (in Load function validation section)

```cpp
// Migration: v0.7.x -> v0.8.0
// If old sensitivity value exists but new thresholds are at defaults,
// attempt to convert sensitivity to reasonable thresholds
if (engine.m_slope_min_threshold == -0.3f && 
    engine.m_slope_max_threshold == -2.0f &&
    engine.m_slope_sensitivity != 0.5f) {
    // Legacy config - apply migration
    // Old formula: excess = threshold - slope; factor = 1 - (excess * 0.1 * sens)
    // For sens=1.0 and excess=10, factor=0. So max_threshold ≈ min - 10
    double sens = (double)engine.m_slope_sensitivity;
    engine.m_slope_max_threshold = engine.m_slope_min_threshold - (10.0f / (0.1f * sens));
    std::cout << "[Config] Migrated slope_sensitivity " << sens 
              << " to max_threshold " << engine.m_slope_max_threshold << std::endl;
}
```

---

## Initialization Order Analysis

**No Circular Dependencies:** Both changes are contained within `FFBEngine.h` and `Config.h`. No new header files are introduced.

**Constructor Order:**
1. `FFBEngine` default constructor calls `Preset::ApplyDefaultsToEngine()`
2. New threshold members are initialized with default values in class definition
3. `Apply()` copies Preset values into engine members
4. No order-dependent initialization required

---

## Test Plan (TDD-Ready)

### Test Group 1: Smoothstep Speed Gate

#### Test 1.1: `test_smoothstep_endpoints`
**Purpose:** Verify smoothstep returns correct values at endpoints

**Expected Behavior:**
- At speed = lower: gate = 0.0
- At speed = upper: gate = 1.0
- At speed = midpoint: gate = 0.5

**Test Script:**
```cpp
static void test_smoothstep_endpoints() {
    FFBEngine engine;
    
    // Test at lower threshold
    double result_0 = engine.smoothstep(1.0, 5.0, 1.0);
    ASSERT_NEAR(result_0, 0.0, 0.001);
    
    // Test at upper threshold
    double result_1 = engine.smoothstep(1.0, 5.0, 5.0);
    ASSERT_NEAR(result_1, 1.0, 0.001);
    
    // Test at midpoint
    double result_mid = engine.smoothstep(1.0, 5.0, 3.0);
    ASSERT_NEAR(result_mid, 0.5, 0.001);
}
```

#### Test 1.2: `test_smoothstep_vs_linear_comparison`
**Purpose:** Verify smoothstep produces different values than linear at 25% and 75% points

**Expected Behavior:**
- Linear at t=0.25: result = 0.25
- Smoothstep at t=0.25: result = 0.15625 (below linear)
- Linear at t=0.75: result = 0.75
- Smoothstep at t=0.75: result = 0.84375 (above linear)

**Test Script:**
```cpp
static void test_smoothstep_vs_linear_comparison() {
    FFBEngine engine;
    
    // At t=0.25 (speed=2.0 for range [1.0, 5.0])
    double linear_25 = 0.25;
    double smooth_25 = engine.smoothstep(1.0, 5.0, 2.0);
    ASSERT_NEAR(smooth_25, 0.15625, 0.001);  // t²(3-2t) = 0.0625 * 2.5
    ASSERT_TRUE(smooth_25 < linear_25);
    
    // At t=0.75 (speed=4.0 for range [1.0, 5.0])
    double linear_75 = 0.75;
    double smooth_75 = engine.smoothstep(1.0, 5.0, 4.0);
    ASSERT_NEAR(smooth_75, 0.84375, 0.001);  // t²(3-2t) = 0.5625 * 1.5
    ASSERT_TRUE(smooth_75 > linear_75);
}
```

#### Test 1.3: `test_smoothstep_in_calculate_force`
**Purpose:** Verify end-to-end that calculate_force uses smoothstep

**Multi-Frame Telemetry Script:**
| Frame | Speed (m/s) | Expected Linear Gate | Expected Smoothstep Gate |
|-------|-------------|---------------------|-------------------------|
| 1 | 1.0 | 0.0 | 0.0 |
| 2 | 2.0 | 0.25 | 0.156 |
| 3 | 3.0 | 0.50 | 0.50 |
| 4 | 4.0 | 0.75 | 0.844 |
| 5 | 5.0 | 1.0 | 1.0 |

**Test Script:**
```cpp
static void test_smoothstep_in_calculate_force() {
    FFBEngine engine;
    InitializeEngine(engine);
    engine.m_speed_gate_lower = 1.0f;
    engine.m_speed_gate_upper = 5.0f;
    engine.m_road_texture_enabled = true;
    engine.m_road_texture_gain = 1.0f;
    
    TelemInfoV01 data = CreateBasicTestTelemetry(2.0);  // 25% point
    data.mWheel[0].mVerticalTireDeflection = 0.001;
    engine.calculate_force(&data);
    
    // Road texture is multiplied by speed_gate
    // With smoothstep at t=0.25, gate = 0.15625
    // With linear at t=0.25, gate = 0.25
    // Force with smoothstep should be ~62.5% of linear
    
    // We verify by checking the snapshot
    auto snap = engine.GetDebugBatch().back();
    // texture_road = raw * gate
    // We can't easily extract gate from snapshot, so we compare forces
    
    // Alternative: Run at midpoint where linear == smoothstep
    TelemInfoV01 data_mid = CreateBasicTestTelemetry(3.0);
    engine.calculate_force(&data_mid);
    // At midpoint, both should give 0.5 gate
}
```

#### Test 1.4: `test_smoothstep_edge_cases`
**Purpose:** Test boundary conditions

**Test Script:**
```cpp
static void test_smoothstep_edge_cases() {
    FFBEngine engine;
    
    // Below lower threshold
    double below = engine.smoothstep(1.0, 5.0, 0.0);
    ASSERT_NEAR(below, 0.0, 0.001);
    
    // Above upper threshold
    double above = engine.smoothstep(1.0, 5.0, 10.0);
    ASSERT_NEAR(above, 1.0, 0.001);
    
    // Edge case: range too small (should clamp)
    double tiny_range = engine.smoothstep(1.0, 1.0001, 1.0);
    ASSERT_TRUE(tiny_range >= 0.0 && tiny_range <= 1.0);
}
```

### Test Group 2: InverseLerp Threshold System

#### Test 2.1: `test_inverse_lerp_basic`
**Purpose:** Verify inverse_lerp returns correct normalized values

**Test Script:**
```cpp
static void test_inverse_lerp_basic() {
    FFBEngine engine;
    
    // At min threshold
    double at_min = engine.inverse_lerp(-0.3, -2.0, -0.3);
    ASSERT_NEAR(at_min, 0.0, 0.001);
    
    // At max threshold
    double at_max = engine.inverse_lerp(-0.3, -2.0, -2.0);
    ASSERT_NEAR(at_max, 1.0, 0.001);
    
    // At midpoint (-1.15)
    double at_mid = engine.inverse_lerp(-0.3, -2.0, -1.15);
    ASSERT_NEAR(at_mid, 0.5, 0.001);
    
    // Below max (saturated)
    double saturated = engine.inverse_lerp(-0.3, -2.0, -5.0);
    ASSERT_NEAR(saturated, 1.0, 0.001);
    
    // Above min (dead zone)
    double dead_zone = engine.inverse_lerp(-0.3, -2.0, 0.0);
    ASSERT_NEAR(dead_zone, 0.0, 0.001);
}
```

#### Test 2.2: `test_slope_min_max_thresholds_grip`
**Purpose:** Verify slope detection uses min/max thresholds correctly

**Multi-Frame Telemetry Script:**
| Frame | Slope | Effect Intensity | Grip Factor |
|-------|-------|------------------|-------------|
| 1 | 0.0 | 0.0 (dead zone) | 1.0 |
| 2 | -0.2 | 0.0 (dead zone) | 1.0 |
| 3 | -0.3 | 0.0 (at min) | 1.0 |
| 4 | -0.8 | 0.294 | 0.706 |
| 5 | -1.15 | 0.5 | 0.5 |
| 6 | -2.0 | 1.0 (at max) | 0.0 → 0.2 (floor) |
| 7 | -5.0 | 1.0 (saturated) | 0.2 (floor) |

**Test Script:**
```cpp
static void test_slope_min_max_thresholds_grip() {
    FFBEngine engine;
    InitializeEngine(engine);
    engine.m_slope_detection_enabled = true;
    engine.m_slope_min_threshold = -0.3f;
    engine.m_slope_max_threshold = -2.0f;
    
    // Manually set slope values and verify grip
    
    // Dead zone (slope = -0.2, above min)
    engine.m_slope_current = -0.2;
    double grip_dead = engine.calculate_slope_grip(0.0, 0.0, 0.01);
    ASSERT_NEAR(grip_dead, 1.0, 0.05);
    
    // At min threshold (slope = -0.3)
    engine.m_slope_current = -0.3;
    engine.m_slope_smoothed_output = 1.0; // Reset
    grip = calculate_slope_grip(...);
    ASSERT_NEAR(grip, 1.0, 0.05);
    
    // Midpoint (slope = -1.15)
    engine.m_slope_current = -1.15;
    engine.m_slope_smoothed_output = 1.0;
    grip = calculate_slope_grip(...);
    ASSERT_NEAR(grip, 0.5, 0.1);  // After smoothing
    
    // At max (slope = -2.0)
    engine.m_slope_current = -2.0;
    grip = calculate_slope_grip(...);
    ASSERT_NEAR(grip, 0.2, 0.05);  // Floor
}
```

#### Test 2.3: `test_slope_threshold_config_persistence`
**Purpose:** Verify new settings are saved and loaded correctly

**Test Script:**
```cpp
static void test_slope_threshold_config_persistence() {
    std::string test_file = "test_slope_minmax.ini";
    
    FFBEngine engine_save;
    engine_save.m_slope_min_threshold = -0.5f;
    engine_save.m_slope_max_threshold = -3.0f;
    Config::Save(engine_save, test_file);
    
    FFBEngine engine_load;
    Config::Load(engine_load, test_file);
    
    ASSERT_NEAR(engine_load.m_slope_min_threshold, -0.5f, 0.001);
    ASSERT_NEAR(engine_load.m_slope_max_threshold, -3.0f, 0.001);
    
    std::remove(test_file.c_str());
}
```

#### Test 2.4: `test_slope_sensitivity_migration`
**Purpose:** Verify legacy sensitivity values are migrated

**Test Script:**
```cpp
static void test_slope_sensitivity_migration() {
    // Create legacy config file with sensitivity but no thresholds
    std::string test_file = "test_slope_migration.ini";
    {
        std::ofstream file(test_file);
        file << "slope_detection_enabled=1\n";
        file << "slope_sensitivity=2.0\n";  // Higher sensitivity
        file << "slope_negative_threshold=-0.3\n";
        // No slope_min_threshold or slope_max_threshold
    }
    
    FFBEngine engine;
    Config::Load(engine, test_file);
    
    // With sensitivity=2.0, max_threshold should be calculated
    // Old formula: factor = 1 - (excess * 0.1 * 2.0)
    // At factor=0, excess = 5.0
    // max_threshold = min - 5.0 = -5.3
    ASSERT_TRUE(engine.m_slope_max_threshold < engine.m_slope_min_threshold);
    
    std::remove(test_file.c_str());
}
```

### Boundary Condition Tests

#### Test 2.5: `test_inverse_lerp_edge_range`
**Purpose:** Test edge case where min == max

**Test Script:**
```cpp
static void test_inverse_lerp_edge_range() {
    FFBEngine engine;
    
    // Min == Max (degenerate case)
    double same = engine.inverse_lerp(-0.3, -0.3, -0.3);
    ASSERT_TRUE(same == 0.0 || same == 1.0);  // Implementation dependent
    
    // Very small range
    double tiny = engine.inverse_lerp(-0.3, -0.30001, -0.30001);
    ASSERT_NEAR(tiny, 1.0, 0.001);
}
```

---

## GUI Changes

### Change 5: Add Min/Max Threshold Sliders

**File:** `GuiLayer.cpp`  
**Location:** Inside Slope Detection settings section (around line 1170)

**Pseudo-code:**
```cpp
// Current single threshold slider
FloatSetting("  Slope Threshold", &engine.m_slope_negative_threshold, -1.0f, 0.0f, "%.2f", ...);

// NEW: Replace with min/max pair
FloatSetting("  Effect Start (Min)", &engine.m_slope_min_threshold, -1.0f, 0.0f, "%.2f",
    "Slope value where grip loss effect BEGINS.\n"
    "Values below this (more negative) start triggering understeer.\n"
    "Set to the slope where you first want to feel grip loss.");
    
FloatSetting("  Effect Full (Max)", &engine.m_slope_max_threshold, -5.0f, -0.1f, "%.2f",
    "Slope value where grip loss effect reaches MAXIMUM.\n"
    "Must be more negative than 'Effect Start'.\n"
    "Set to the slope where you want full understeer FFB drop.");
```

---

## Deliverables

### Code Changes

- [ ] Add `smoothstep()` helper function to FFBEngine.h
- [ ] Add `inverse_lerp()` helper function to FFBEngine.h  
- [ ] Modify speed gate calculation to use smoothstep
- [ ] Add `m_slope_min_threshold` and `m_slope_max_threshold` settings
- [ ] Modify `calculate_slope_grip()` to use min/max thresholds
- [ ] Update Preset struct with new settings
- [ ] Update Preset::Apply() and UpdateFromEngine()
- [ ] Update Config::Save() and Config::Load()
- [ ] Add migration logic for legacy sensitivity values
- [ ] Update GUI with new threshold sliders

### Tests

- [ ] test_smoothstep_endpoints
- [ ] test_smoothstep_vs_linear_comparison
- [ ] test_smoothstep_in_calculate_force
- [ ] test_smoothstep_edge_cases
- [ ] test_inverse_lerp_basic
- [ ] test_slope_min_max_thresholds_grip
- [ ] test_slope_threshold_config_persistence
- [ ] test_slope_sensitivity_migration
- [ ] test_inverse_lerp_edge_range

### Documentation

- [ ] Update CHANGELOG.md with v0.8.0 entry
- [ ] Update user-facing documentation for new threshold controls
- [ ] Add tooltip text for new GUI sliders

---

## Implementation Notes

*This section should be filled in by the developer during implementation.*

### Issues Encountered

*(Document any unforeseen issues)*

### Deviations from Plan

*(Document any necessary deviations)*

### Additional Tests Added

*(Document any extra tests created)*

---

## Document History

| Version | Date | Author | Notes |
|---------|------|--------|-------|
| 1.0 | 2026-02-03 | Antigravity | Initial plan |
