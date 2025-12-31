# Implementation Plan: Config File Reordering

**Plan Date:** 2025-12-31  
**Target Version:** v0.6.28  
**Objective:** Reorder the saving logic in `Config::Save` so that the `config.ini` file structure mirrors the visual hierarchy of the GUI. This improves readability for users manually editing the file.

## 1. Current State vs. Desired State

Currently, `Config::Save` writes settings in a semi-random order (roughly chronological by implementation date).
The desired state is to group settings by their GUI headers and order them top-to-bottom.

## 2. Target INI Structure

### 2.1 System & Window (Top of File)
- `ini_version`
- `ignore_vjoy_version_warning`
- `enable_vjoy`
- `output_ffb_to_vjoy`
- `always_on_top`
- `last_device_guid`
- `win_pos_x`, `win_pos_y`
- `win_w_small`, `win_h_small`
- `win_w_large`, `win_h_large`
- `show_graphs`

### 2.2 General FFB
- `invert_force`
- `gain`
- `max_torque_ref`
- `min_force`

### 2.3 Front Axle (Understeer)
- `steering_shaft_gain`
- `steering_shaft_smoothing`
- `understeer`
- `base_force_mode`
- `flatspot_suppression`
- `notch_q`
- `flatspot_strength`
- `static_notch_enabled`
- `static_notch_freq`
- `static_notch_width`

### 2.4 Rear Axle (Oversteer)
- `oversteer_boost`
- `sop`
- `rear_align_effect`
- `sop_yaw_gain`
- `yaw_kick_threshold`
- `yaw_accel_smoothing`
- `gyro_gain`
- `gyro_smoothing_factor`
- `sop_smoothing_factor`
- `sop_scale`
- `understeer_affects_sop` (Hidden Setting - Placed here logically)

### 2.5 Physics (Grip & Slip Angle)
- `slip_angle_smoothing`
- `chassis_inertia_smoothing`
- `optimal_slip_angle`
- `optimal_slip_ratio`

### 2.6 Braking & Lockup
- `lockup_enabled`
- `lockup_gain`
- `brake_load_cap`
- `lockup_freq_scale`
- `lockup_gamma`
- `lockup_start_pct`
- `lockup_full_pct`
- `lockup_prediction_sens`
- `lockup_bump_reject`
- `lockup_rear_boost`
- `abs_pulse_enabled`
- `abs_gain`
- `abs_freq`

### 2.7 Tactile Textures
- `texture_load_cap`
- `slide_enabled`
- `slide_gain`
- `slide_freq`
- `road_enabled`
- `road_gain`
- `road_fallback_scale` (Hidden Setting - Placed here logically)
- `spin_enabled`
- `spin_gain`
- `spin_freq_scale`
- `scrub_drag_gain`
- `bottoming_method`

### 2.8 Advanced Settings
- `speed_gate_lower`
- `speed_gate_upper`

## 3. Implementation Details

## 3. Implementation Details

### 3.1 Modify `Config::Save` (Main Config)
The `Config::Save` function in `src/Config.cpp` will be refactored. No logic changes, only line reordering.
-   **Grouping:** Settings will be grouped logically using INI comment headers (lines starting with `;`).
-   **Order:** The write order will strictly follow the "Target INI Structure" defined in Section 2.

### 3.2 Modify `Config::Save` (Presets Section)
The loop that writes user presets should also be updated to match this order for consistency, though the primary focus is the main configuration block.

### 3.3 Modify `Config::Load` (Critical Fix)
**Critical Issue Identified:** The current `Config::Load` implementation reads the entire file line-by-line. Since User Presets (stored at the bottom of the file under `[Presets]`) use identical key names (e.g., `gain=...`) as the Main Configuration, the settings from the *last defined preset* currently overwrite the main global configuration during load.

**Required Fix:**
-   Update `Config::Load` to **stop parsing** (or ignore subsequent lines) immediately upon encountering the `[Presets]` section header or any line starting with `[` (indicating a section change).
-   This ensures that the Main Configuration is determined *solely* by the key-value pairs at the top of the file, preventing pollution from preset data.

### 3.4 Legacy Key Support
While `Config::Save` will only write the modern keys (e.g., `sop_smoothing_factor`, `texture_load_cap`), `Config::Load` **must retain** the `else if` blocks for legacy keys (`smoothing`, `max_load_factor`) to ensuring complete headers backward compatibility with existing user config files.

## 4. Verification Plan

1.  **Manual Check:** Open the App, make distinct changes to one setting in each group.
2.  **Save:** Click "Save Current Config".
3.  **Inspect:** Open `config.ini` and verify the line order matches the plan.
4.  **Load Test:** Restart the App to ensure the reordered file loads correctly (Parsing is order-independent, so this should pass easily).
5.  **Test Suite:** Run existing persistence tests to ensure no keys were accidentally deleted or typoed during the move.

## 5. Risk Assessment
- **Risk:** Low (raised from Very Low due to the `Config::Load` bug discovery).
- **Impact:** INI file readability improved. `Config::Load` robustness significantly increased.
- **Backwards Compatibility:** Fully compatible. The loader identifies values by key string.

## 6. Detailed Automated Test Plan

New tests will be added to `tests/test_persistence_v0628.cpp` to verify fixes and reordering.

### Test Case 1: `Load_ Stops At Presets Header`
**Objective:** Verify that `Config::Load` stops parsing main settings when it hits `[Presets]`.
**Steps:**
1.  Create a temporary `config_test_isolation.ini`.
2.  Write Main Config: `gain=0.5`
3.  Write Header: `[Presets]`
4.  Write Preset Line: `gain=2.0` (This would overwrite main config in the buggy implementation).
5.  Call `Config::Load`.
6.  **Assert:** `engine.m_gain` is `0.5`, **NOT** `2.0`.

### Test Case 2: `Save_ Follows Defined Order`
**Objective:** Verify that `Config::Save` writes keys in the specific order defined in the plan.
**Steps:**
1.  Initialize engine with known values.
2.  Call `Config::Save("config_order_test.ini")`.
3.  Read the file content as a string.
4.  **Assert:** The string `win_pos_x` appears before `gain`.
5.  **Assert:** The string `gain` appears before `steering_shaft_gain`.
6.  **Assert:** The string `steering_shaft_gain` appears before `oversteer_boost`.
7.  **Assert:** The string `[Presets]` appears after all main config keys.

### Test Case 3: `Load_ Supports Legacy Keys`
**Objective:** Verify backward compatibility is maintained.
**Steps:**
1.  Create `config_legacy_test.ini`.
2.  Write: `smoothing=0.1` (Legacy key for `sop_smoothing_factor`).
3.  Write: `max_load_factor=2.0` (Legacy key for `texture_load_cap`).
4.  Call `Config::Load`.
5.  **Assert:** `engine.m_sop_smoothing_factor` is `0.1`.
6.  **Assert:** `engine.m_texture_load_cap` is `2.0`.

### Test Case 4: `Structure_ Includes Comments`
**Objective:** Verify that the new `Config::Save` adds helper comments for readability.
**Steps:**
1.  Call `Config::Save("config_comment_test.ini")`.
2.  Read file content.
3.  **Assert:** File contains string `; --- System & Window ---`.
4.  **Assert:** File contains string `; --- General FFB ---`.

