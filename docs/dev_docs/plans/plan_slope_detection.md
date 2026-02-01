# Implementation Plan: Slope Detection Algorithm for Dynamic Grip Estimation

## Context

This plan describes the implementation of the **Slope Detection Algorithm**, a physics-based approach to dynamically estimate tire grip levels in real-time. The algorithm replaces the current static, user-configured optimal slip angle/ratio thresholds with an adaptive signal processing model that monitors the derivative (slope) of the Self-Aligning Torque (SAT) vs. Slip Angle relationship.

### Problem Statement

The current lmuFFB implementation uses **static optimal slip values** (`m_optimal_slip_angle`, `m_optimal_slip_ratio`) that require manual tuning per car and do not adapt to:
- Tire temperature changes
- Tire wear
- Rain/wet conditions
- Aerodynamic load changes
- Different tire compounds

### Proposed Solution

The Slope Detection Algorithm monitors the **rate of change (derivative)** of lateral force/G-force vs. slip angle. Instead of asking "has the driver exceeded 5.7 degrees?", it asks "is more steering input producing more grip, or less?"

**Core Principle:**
- **Positive Slope (>0)**: Grip is building. No intervention needed.
- **Zero Slope (≈0)**: At peak grip. This is the optimal slip angle - detected automatically.
- **Negative Slope (<0)**: Past peak, tire is sliding. FFB should lighten to signal understeer.

---

## Reference Documents

1. **Research Reports:**
   - `docs/dev_docs/FFB Slope Detection for Grip Estimation.md` - Comprehensive analysis of slope detection theory
   - `docs/dev_docs/FFB Slope Detection for Grip Estimation2.md` - Signal processing and Savitzky-Golay filter analysis

2. **Preliminary Implementation Plans:**
   - `docs/dev_docs/slope_detection_implementation_plan.md` - Initial technical approach
   - `docs/dev_docs/slope_detection_implementation_plan2.md` - Detailed implementation specifications

3. **Related TODO Item:**
   - `docs/dev_docs/TODO.md` - Section "Optimal slip angle in real time"

---

## Proposed Changes

### 1. New Configuration Parameters

**File: `src/Config.h`** - Add to `Preset` struct (~line 70):

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `slope_detection_enabled` | `bool` | `false` | Enable dynamic slope detection |
| `slope_sg_window` | `int` | `15` | Savitzky-Golay window size (samples) |
| `slope_sensitivity` | `float` | `1.0f` | Sensitivity multiplier for slope-to-grip conversion |
| `slope_negative_threshold` | `float` | `-0.1f` | Slope below which grip loss is detected |
| `slope_smoothing_tau` | `float` | `0.02f` | Output smoothing time constant (seconds) |

### 2. FFBEngine Core Changes

**File: `src/FFBEngine.h`**

#### 2.1 New Member Variables (public section, ~line 244):

```cpp
// ===== SLOPE DETECTION =====
bool m_slope_detection_enabled = false;
int m_slope_sg_window = 15;
float m_slope_sensitivity = 1.0f;
float m_slope_negative_threshold = -0.1f;
float m_slope_smoothing_tau = 0.02f;
```

#### 2.2 Internal State Buffers (private section, ~line 350):

```cpp
// Slope Detection Buffers (Circular)
static constexpr int SLOPE_BUFFER_MAX = 41;
std::array<double, SLOPE_BUFFER_MAX> m_slope_lat_g_buffer = {};
std::array<double, SLOPE_BUFFER_MAX> m_slope_slip_buffer = {};
int m_slope_buffer_index = 0;
int m_slope_buffer_count = 0;

// Slope Detection State (Public for diagnostics)
double m_slope_current = 0.0;
double m_slope_grip_factor = 1.0;
double m_slope_smoothed_output = 1.0;
```

#### 2.3 New Helper Functions:

1. **`CalculateSGDerivative()`** - Savitzky-Golay derivative calculation:
   - Input: Buffer array, count, window size, dt
   - Output: Derivative (slope) at the center point
   - Uses symmetric SG coefficients for 1st derivative

2. **`CalculateSlopeGrip()`** - Main slope detection function:
   - Input: lateral_g, slip_angle, dt
   - Process:
     1. Push samples into circular buffers
     2. Calculate derivatives using Savitzky-Golay
     3. Compute slope (dG/dSlip)
     4. Convert slope to grip factor (sigmoid response for negative slopes)
     5. Apply output smoothing (time-corrected LPF)
   - Output: Smoothed grip factor (0.2 to 1.0)

#### 2.4 Modify `calculate_grip()` Function (~lines 576-601):

Add conditional path that uses `CalculateSlopeGrip()` when `m_slope_detection_enabled` is true, falling back to the existing static threshold logic otherwise.

### 3. Config Persistence

**File: `src/Config.cpp`**

#### 3.1 Save Logic (~line 470):
```cpp
file << "slope_detection_enabled=" << (engine.m_slope_detection_enabled ? "1" : "0") << "\n";
file << "slope_sg_window=" << engine.m_slope_sg_window << "\n";
file << "slope_sensitivity=" << engine.m_slope_sensitivity << "\n";
file << "slope_negative_threshold=" << engine.m_slope_negative_threshold << "\n";
file << "slope_smoothing_tau=" << engine.m_slope_smoothing_tau << "\n";
```

#### 3.2 Load Logic (~line 622):
```cpp
else if (key == "slope_detection_enabled") engine.m_slope_detection_enabled = (value == "1");
else if (key == "slope_sg_window") engine.m_slope_sg_window = std::stoi(value);
else if (key == "slope_sensitivity") engine.m_slope_sensitivity = std::stof(value);
else if (key == "slope_negative_threshold") engine.m_slope_negative_threshold = std::stof(value);
else if (key == "slope_smoothing_tau") engine.m_slope_smoothing_tau = std::stof(value);
```

#### 3.3 Validation Logic (~line 640):
- `slope_sg_window`: Clamp to [5, 41] range
- `slope_sensitivity`: Clamp to [0.1, 10.0] range
- `slope_smoothing_tau`: Reset to 0.02f if < 0.001f

#### 3.4 Preset Synchronization:
Update `ApplyDefaultsToEngine()` and `SyncFromEngine()` methods.

### 4. GUI Integration

**File: `src/GuiLayer.cpp`** (~line 1140, after Optimal Slip settings):

Add a new collapsible section "Slope Detection (Experimental)":
- Enable/Disable checkbox
- Filter Window slider (5-41 samples)
- Sensitivity slider (0.1x - 5.0x)
- Advanced Settings (collapsed by default):
  - Slope Threshold slider
  - Output Smoothing slider
- Live Diagnostics display:
  - Current Slope value
  - Grip Factor percentage

### 5. Built-in Preset Updates

Update preset defaults to include slope detection parameters:

```cpp
// All presets - conservative defaults
preset.slope_detection_enabled = false;  // Off by default
preset.slope_sg_window = 15;
preset.slope_sensitivity = 1.0f;
preset.slope_negative_threshold = -0.1f;
preset.slope_smoothing_tau = 0.02f;
```

---

## Test Plan (TDD-Ready)

The following tests should be written **BEFORE** implementing the feature code. Run them to verify they fail (Red Phase), then implement the code to make them pass (Green Phase).

### 1. Unit Tests: Slope Detection Buffer Initialization

**File:** `tests/test_ffb_engine.cpp`

```cpp
static void test_slope_detection_buffer_init()
```

| Aspect | Description |
|--------|-------------|
| **Purpose** | Verify slope detection buffers are properly initialized |
| **Input** | Freshly created `FFBEngine` instance |
| **Expected** | `m_slope_buffer_count == 0`, `m_slope_buffer_index == 0`, `m_slope_current == 0.0` |
| **Assertion** | All three conditions must be true |

### 2. Unit Tests: Savitzky-Golay Derivative Calculation

```cpp
static void test_slope_sg_derivative()
```

| Aspect | Description |
|--------|-------------|
| **Purpose** | Verify SG derivative calculation works for linear ramp |
| **Input** | Buffer filled with linear ramp (y = i * 0.1), window = 9, dt = 0.01 |
| **Expected** | Derivative ≈ 10.0 units/sec (0.1 per sample at 100 Hz) |
| **Assertion** | `abs(derivative - 10.0) < 1.0` |

### 3. Unit Tests: Grip at Peak (Zero Slope)

```cpp
static void test_slope_grip_at_peak()
```

| Aspect | Description |
|--------|-------------|
| **Purpose** | Verify grip factor is 1.0 when slope is zero (at peak) |
| **Input** | Constant lateral G (1.2), constant slip (0.05), dt = 0.0025 (400 Hz) |
| **Expected** | `m_slope_smoothed_output > 0.95` (near 1.0) |
| **Assertion** | Grip factor remains high with zero slope |

### 4. Unit Tests: Grip Past Peak (Negative Slope)

```cpp
static void test_slope_grip_past_peak()
```

| Aspect | Description |
|--------|-------------|
| **Purpose** | Verify grip factor reduces when slope is negative (past peak) |
| **Input** | Increasing slip (0.05 to 0.09), decreasing G (1.5 to 1.1) over 20 frames |
| **Expected** | `0.2 < m_slope_smoothed_output < 0.9` |
| **Assertion** | Grip factor is reduced but not below safety floor |

### 5. Unit Tests: Slope Detection vs Static Comparison

```cpp
static void test_slope_vs_static_comparison()
```

| Aspect | Description |
|--------|-------------|
| **Purpose** | Verify both slope and static methods detect grip loss |
| **Input** | Two engines configured differently, same telemetry with 12% slip |
| **Expected** | Both engines detect grip loss (< 0.95 for slope, < 0.8 for static) |
| **Assertion** | Both detection methods should agree on grip loss |

### 6. Unit Tests: Config Persistence

```cpp
static void test_slope_config_persistence()
```

| Aspect | Description |
|--------|-------------|
| **Purpose** | Verify slope settings save and load correctly |
| **Input** | Non-default values (enabled=true, window=21, sensitivity=2.5f) |
| **Expected** | Values survive save/load cycle |
| **Assertion** | All loaded values match saved values |

### 7. Unit Tests: Latency Characteristics

```cpp
static void test_slope_latency_characteristics()
```

| Aspect | Description |
|--------|-------------|
| **Purpose** | Verify buffer fills correctly and latency is as expected |
| **Input** | Window size = 15, dt = 0.0025 (400 Hz) |
| **Expected** | Buffer fills in exactly `window_size` frames, latency = ~17.5ms |
| **Assertion** | Frame count matches window size |

### 8. Unit Tests: Noise Rejection

```cpp
static void test_slope_noise_rejection()
```

| Aspect | Description |
|--------|-------------|
| **Purpose** | Verify SG filter rejects noise while preserving signal |
| **Input** | Constant G (1.2) + random noise (±0.1), 50 frames |
| **Expected** | `abs(m_slope_current) < 1.0` (near zero despite noise) |
| **Assertion** | Noise is filtered, slope remains stable |

---

## Documentation Updates

1. **CHANGELOG.md** - Add entry describing the new Slope Detection feature
2. **VERSION** - Increment version number (suggest v0.7.0 for this feature)
3. **README.md** - Add brief description in Features section (optional)

---

## User Settings & Presets Impact

### New Settings
Five new configurable parameters are being added (see Section 1). All have sensible defaults and the feature is **disabled by default** to ensure backward compatibility.

### Migration Logic
**No migration required.** When loading old configuration files:
- Missing settings will use default values
- `slope_detection_enabled = false` ensures existing users see no change in behavior
- Static threshold logic remains as fallback

### Preset Updates
All built-in presets should be updated to include the new settings with the feature disabled by default. This is a non-breaking change.

---

## Deliverables Checklist

### Code Changes
- [ ] `src/FFBEngine.h` - Add slope detection members and helper functions
- [ ] `src/Config.h` - Add new settings to Preset struct
- [ ] `src/Config.cpp` - Add save/load/validation logic
- [ ] `src/GuiLayer.cpp` - Add GUI controls for slope detection

### Test Files
- [ ] `tests/test_ffb_engine.cpp` - Add 8 new unit tests (~80 lines)

### Documentation
- [ ] `CHANGELOG.md` - Add feature entry
- [ ] `VERSION` - Update version number

### Verification
- [ ] All existing tests pass (no regressions)
- [ ] All new tests pass
- [ ] Build succeeds in Release mode
- [ ] GUI displays correctly and values persist on restart

---

## Technical Notes

### Expected Latency
At 400 Hz telemetry rate:
- Window 9: ~10ms latency
- Window 15: ~17.5ms latency (recommended)
- Window 25: ~30ms latency

### Signal Processing Rationale
Savitzky-Golay filter is chosen over simple moving average because:
1. Preserves peak shape (critical for detecting SAT drop-off)
2. Provides derivative as direct output
3. Superior noise rejection without excessive phase lag

### Safety Considerations
- Grip factor has a floor of 0.2 to prevent complete FFB loss
- Feature is disabled by default to avoid surprising existing users
- Static threshold logic remains as fallback

---

## Future Deprecation Path (Informational)

If Slope Detection proves superior after user testing:
1. **v0.8.0**: Slope Detection enabled by default for new users
2. **v0.9.0**: Static threshold sliders moved to "Legacy" section
3. **v1.0.0**: Static threshold logic removed (or kept as hidden fallback)

---

*Plan created: 2026-02-01*
*Estimated implementation effort: ~200 lines of C++ code across 4 files*
