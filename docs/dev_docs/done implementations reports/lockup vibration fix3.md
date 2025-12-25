Here is the unified, comprehensive technical report and implementation plan for **Batch 2** of the Braking FFB Overhaul.

# Technical Report: Braking FFB Overhaul (Batch 2) - Predictive Logic & Advanced Haptics

**Date:** December 25, 2025
**Target Version:** v0.6.0
**Subject:** Implementation of Predictive Lockup (Angular Deceleration), ABS Haptics, Pressure-Based Scaling, and GUI Reorganization.

---

## 1. Executive Summary

Following the foundational fixes in Batch 1 (Axle differentiation, Manual Slip fix), Batch 2 focuses on **Latency Reduction** and **Physical Fidelity**.

Current FFB implementations rely solely on **Slip Ratio**, which is a *reactive* metric; the tire must already be sliding significantly before the driver feels it. To support Esports-level performance, we introduce **Predictive Lockup** logic based on **Angular Deceleration**, allowing the driver to feel the onset of a lockup *before* the tire saturates.

Additionally, we are moving from "Input-Based" scaling (Pedal Position) to "Physics-Based" scaling (Brake Pressure). This ensures that Brake Bias changes and ABS interventions are correctly felt through the wheel. Finally, the GUI will be restructured to house these advanced features in a dedicated **"Braking & Lockup"** section, decoupling braking forces from road textures.

---

## 2. Physics Engine Enhancements (`FFBEngine.h`)

### A. Predictive Lockup (Hybrid Thresholding)

**The Problem:** Relying solely on Slip Ratio introduces a delay (latency) between the physical event (wheel stopping) and the FFB trigger.
**The Solution:** Monitor **Angular Deceleration** ($\alpha_{wheel}$). If the wheel decelerates violently, we predict a lockup is imminent and **dynamically lower the Slip Ratio threshold**.

**Risk Mitigation Strategy:**
Raw angular acceleration is noisy. To prevent false positives (e.g., hitting a curb causing a spike), we implement a multi-layer gating system:
1.  **Brake Gate:** Effect is disabled if Brake Input < 2%.
2.  **Airborne Gate:** Effect is disabled if `mSuspForce < 50N` (Wheel in air).
3.  **Bump Rejection:** Effect is disabled if Suspension Velocity > Threshold (e.g., 1.0 m/s).
4.  **Relative Deceleration:** The wheel must be slowing down significantly faster than the chassis.

**Implementation Logic:**

```cpp
// 1. Calculate Angular Deceleration (rad/s^2)
double wheel_accel = (w.mRotation - w.prevRotation) / dt;
w.prevRotation = w.mRotation; // Update state

// 2. Calculate Chassis Angular Equivalent
// How fast *should* the wheel be slowing down given the car's braking?
// mLocalAccel.z is negative during braking.
double chassis_accel_ang = (data->mLocalAccel.z / tire_radius); 

// 3. Bump Detection (Suspension Velocity)
double susp_vel = std::abs(w.mVerticalTireDeflection - w.prevDeflection) / dt;
bool is_bumpy = (susp_vel > m_lockup_bump_reject); // User configurable (e.g. 1.0)

// 4. Gating
bool brake_active = (data->mUnfilteredBrake > 0.02); // 2% Deadzone
bool is_grounded = (w.mSuspForce > 50.0);

// 5. Hybrid Thresholding
double trigger_threshold = m_lockup_full_pct / 100.0; // Default (e.g. 15%)

if (brake_active && is_grounded && !is_bumpy) {
    // Check: Is wheel slowing down 2x faster than car? AND is it violent (> sensitivity)?
    // Note: Accel is negative.
    double sensitivity_threshold = -1.0 * m_lockup_prediction_sens; // e.g. -50 rad/s^2
    
    if (wheel_accel < chassis_accel_ang * 2.0 && wheel_accel < sensitivity_threshold) {
        // PREDICTION: Lockup Imminent.
        // Instantly lower the threshold to the "Start" value to trigger vibration early.
        trigger_threshold = m_lockup_start_pct / 100.0; // e.g. 5%
    }
}

// 6. Final Trigger
if (current_slip_ratio < -trigger_threshold) {
    // Trigger Vibration...
}
```

### B. Vibration Gamma (Non-Linear Response)

**The Problem:** A linear ramp ($0\% \to 100\%$ amplitude) can feel "mushy." Pros often want silence until the limit, then a sharp wall of force.
**The Solution:** Apply a Gamma curve to the severity calculation.

**Implementation:**
```cpp
// Normalize slip into 0.0 - 1.0 range
double normalized = (slip_abs - start_ratio) / window;
double severity = (std::min)(1.0, (std::max)(0.0, normalized));

// Apply Configurable Gamma
// 1.0 = Linear, 2.0 = Quadratic, 3.0 = Cubic (Sharp/Late)
severity = std::pow(severity, (double)m_lockup_gamma);
```

### C. Pressure-Based Scaling & ABS Haptics

**The Problem:** Currently, lockup amplitude scales with Pedal Input. If Brake Bias is 70% Front, a Rear lockup (low pressure) feels as strong as a Front lockup. Also, ABS activation is felt only as a generic vibration, not a distinct pulse.
**The Solution:** Use `mBrakePressure` for scaling and detect ABS valve modulation.

**Implementation:**
```cpp
// --- ABS PULSE ---
// Calculate Pressure Derivative
double pressure_delta = (w.mBrakePressure - w.prevBrakePressure) / dt;
w.prevBrakePressure = w.mBrakePressure;

// Detect ABS: High Pedal (>50%) but fluctuating Pressure
bool abs_active = (data->mUnfilteredBrake > 0.5) && (std::abs(pressure_delta) > 2.0);

if (m_abs_pulse_enabled && abs_active) {
    m_abs_phase += 20.0 * dt * TWO_PI; // 20Hz Pulse
    m_abs_phase = std::fmod(m_abs_phase, TWO_PI);
    total_force += std::sin(m_abs_phase) * m_abs_gain * 2.0;
}

// --- PRESSURE SCALING ---
// Replace 'severity * m_lockup_gain' with:
double pressure_factor = w.mBrakePressure;
// Fallback for Engine Braking (Pressure 0, but high slip)
if (pressure_factor < 0.1 && std::abs(slip) > 0.5) pressure_factor = 0.5;

double amp = severity * pressure_factor * m_lockup_gain * ...;
```

### D. Brake Fade (Temperature)

**The Logic:** If `mBrakeTemp > 800°C` (Carbon), reduce FFB detail to simulate "spongy" brakes.
```cpp
if (w.mBrakeTemp > 800.0) {
    double fade = 1.0 - ((w.mBrakeTemp - 800.0) / 200.0);
    fade = std::max(0.0, fade);
    lockup_amplitude *= fade;
    road_texture *= fade;
}
```

---

## 3. Configuration & Persistence (`src/Config.h`)

We need to add the new variables to the `Preset` struct and the Save/Load logic.

**New Variables:**
*   `float lockup_gamma` (Default 2.0)
*   `float lockup_prediction_sens` (Default 50.0 rad/s²)
*   `float lockup_bump_reject` (Default 1.0 m/s)
*   `float brake_load_cap` (Default 1.5x) - *Split from global cap*
*   `bool abs_pulse_enabled` (Default true)
*   `float abs_gain` (Default 1.0)

**Renamed Variables:**
*   `max_load_factor` $\to$ `texture_load_cap` (Affects Road/Slide only).

---

## 4. GUI Reorganization (`src/GuiLayer.cpp`)

We will introduce a dedicated **"Braking & Lockup"** section. This declutters the "Tactile Textures" section and groups all braking logic together.

### A. New Section: "Braking & Lockup"

```cpp
if (ImGui::TreeNodeEx("Braking & Lockup", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed)) {
    ImGui::NextColumn(); ImGui::NextColumn();

    // 1. Core Settings
    BoolSetting("Lockup Vibration", &engine.m_lockup_enabled);
    if (engine.m_lockup_enabled) {
        FloatSetting("  Strength", &engine.m_lockup_gain, 0.0f, 2.0f, ...);
        
        // NEW: Split Load Cap
        FloatSetting("  Brake Load Cap", &engine.m_brake_load_cap, 1.0f, 3.0f, "%.2fx", 
            "Limits vibration intensity under high downforce.\n"
            "Higher = Stronger vibration at high speed.");

        ImGui::Separator();
        ImGui::Text("Response Curve");
        ImGui::NextColumn(); ImGui::NextColumn();

        // NEW: Gamma
        FloatSetting("  Gamma", &engine.m_lockup_gamma, 0.5f, 3.0f, "%.1f", 
            "1.0=Linear, 2.0=Quadratic, 3.0=Cubic (Late/Sharp).");
            
        FloatSetting("  Start Slip %", &engine.m_lockup_start_pct, 1.0f, 10.0f, "%.1f%%");
        FloatSetting("  Full Slip %", &engine.m_lockup_full_pct, 5.0f, 25.0f, "%.1f%%");
        
        ImGui::Separator();
        ImGui::Text("Prediction (Advanced)");
        ImGui::NextColumn(); ImGui::NextColumn();

        // NEW: Prediction Controls
        FloatSetting("  Sensitivity", &engine.m_lockup_prediction_sens, 20.0f, 100.0f, "%.0f", 
            "Angular Deceleration Threshold.\nLower = More sensitive (triggers earlier).\nHigher = Less sensitive.");
            
        FloatSetting("  Bump Rejection", &engine.m_lockup_bump_reject, 0.1f, 5.0f, "%.1f m/s", 
            "Suspension velocity threshold to disable prediction.\nIncrease for bumpy tracks (Sebring).");
    }

    ImGui::Separator();
    ImGui::Text("ABS & Hardware");
    ImGui::NextColumn(); ImGui::NextColumn();

    // NEW: ABS
    BoolSetting("ABS Pulse", &engine.m_abs_pulse_enabled, "Injects 20Hz pulse when ABS modulates pressure.");
    if (engine.m_abs_pulse_enabled) {
        FloatSetting("  Pulse Gain", &engine.m_abs_gain, 0.0f, 2.0f);
    }

    ImGui::TreePop();
}
```

### B. Updated Section: "Tactile Textures"

*   **Rename:** "Load Cap" $\to$ "Texture Load Cap".
*   **Tooltip:** "Affects Road and Slide textures ONLY. Does not affect Braking."

---

## 5. Verification & Test Plan

### A. Physics Tests (`tests/test_ffb_engine.cpp`)

1.  **`test_predictive_lockup_trigger`**
    *   **Scenario:** Brake active, Slip Ratio = 0 (No slip yet).
    *   **Input:** `wheel_accel = -100.0` (Violent stop).
    *   **Assert:** `trigger_threshold` drops to `start_pct`. (Internal state check or verify if force > 0 when slip is small).

2.  **`test_bump_rejection`**
    *   **Scenario:** Same as above, but `susp_velocity = 2.0` (Hit a curb).
    *   **Assert:** Prediction disabled. Threshold remains at `full_pct`.

3.  **`test_abs_pulse_detection`**
    *   **Scenario:** Pedal = 1.0. Pressure oscillates (1.0 -> 0.8 -> 1.0).
    *   **Assert:** `m_abs_phase` advances.

4.  **`test_split_load_caps`**
    *   **Scenario:** High Aero Load (3.0x).
    *   **Config:** `TextureCap = 1.0`, `BrakeCap = 3.0`.
    *   **Assert:** Road Texture is clamped to 1.0x. Lockup is allowed to 3.0x.

### B. Platform Tests (`tests/test_windows_platform.cpp`)

1.  **`test_config_migration_v060`**
    *   Verify that `max_load_factor` from old configs is correctly mapped to `texture_load_cap`.
    *   Verify `brake_load_cap` initializes to default (1.5).

---

## 6. Deliverables Checklist

- [ ] **FFBEngine.h**: Implement `calculate_angular_acceleration`, Hybrid Thresholding, ABS logic, and Gamma curve.
- [ ] **Config.h/cpp**: Add new variables, handle migration of load cap.
- [ ] **GuiLayer.cpp**: Implement new "Braking & Lockup" section, split sliders.
- [ ] **Tests**: Add predictive and ABS unit tests.
- [ ] **Documentation**: Update `CHANGELOG.md` with "Braking Overhaul Batch 2".