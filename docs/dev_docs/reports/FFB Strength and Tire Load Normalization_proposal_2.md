


Yes, the proposed fixes are highly modular and can absolutely be implemented in stages. Furthermore, separating the normalization of structural forces (steering, SoP) from tactile effects (textures, vibrations) is not only possible, but it actually aligns perfectly with how the `lmuFFB` codebase is already structured.

In `FFBEngine.cpp`, the FFB signal is already explicitly split into `structural_sum` and `texture_sum` right before the final output scaling. This makes it incredibly easy to apply dynamic normalization to the steering rack while leaving the textures exactly as they are today.

Here is the breakdown of the possible separations and a 3-stage implementation plan.

---

### The Separations Possible

You can separate the implementation along two axes:
1. **By Signal Component:** You can normalize the **Structural Forces** (Base Steering, SoP, Rear Align, Yaw Kick, Gyro, Soft Lock) independently from the **Tactile Textures** (Road, Slide, Spin, Lockup, ABS, Bottoming).
2. **By Feature:** You can implement the **Dynamic Peak Follower** (the math) without touching the **UI/Config** (leaving `m_max_torque_ref` in the GUI temporarily as a fallback/texture scaler).

---

### Staged Implementation Plan

#### Stage 1: Dynamic Normalization for Structural Forces Only
**Goal:** Fix the core steering weight across different cars (GT3 vs Hypercar) without touching the UI, Config, or Texture logic. 

In this stage, we introduce the Asymmetric Peak Follower and apply it *only* to `structural_sum`. Textures will continue to use the old `m_max_torque_ref` logic.

**Code Snippets:**
*In `FFBEngine.h` (Add to class):*
```cpp
double m_session_peak_torque = 25.0; 
double m_peak_torque_smoothed = 25.0;
double m_last_raw_torque = 0.0;
```

*In `FFBEngine.cpp` (Inside `calculate_force`, right after telemetry processing):*
```cpp
// 1. Asymmetric Peak Follower (Gated by collision/slew limits)
double lat_g_abs = std::abs(data->mLocalAccel.x / 9.81);
double torque_slew = std::abs(raw_torque_input - m_last_raw_torque) / ctx.dt;
m_last_raw_torque = raw_torque_input;

bool is_clean_state = (lat_g_abs < 8.0) && (torque_slew < 1000.0) && !restricted;

if (is_clean_state && m_torque_source == 0) {
    double current_abs_torque = std::abs(raw_torque_input);
    if (current_abs_torque > m_session_peak_torque) {
        m_session_peak_torque = current_abs_torque; // Fast attack
    } else {
        m_session_peak_torque -= (0.05 * ctx.dt);   // Slow decay
    }
    m_session_peak_torque = std::clamp(m_session_peak_torque, 15.0, 120.0);
}

// Smooth the peak to prevent digital stepping
double alpha_peak = ctx.dt / (0.25 + ctx.dt);
m_peak_torque_smoothed += alpha_peak * (m_session_peak_torque - m_peak_torque_smoothed);
```

*In `FFBEngine.cpp` (Modify the Summation section):*
```cpp
// --- 6. SUMMATION ---
double structural_sum = output_force + ctx.sop_base_force + ctx.rear_torque + ctx.yaw_force + ctx.gyro_force + ctx.soft_lock_force;
structural_sum *= ctx.gain_reduction_factor;

// NORMALIZE ONLY STRUCTURAL FORCES dynamically
double structural_denominator = (m_torque_source == 1) ? 1.0 : m_peak_torque_smoothed;
double norm_structural = structural_sum / structural_denominator;

// LEAVE TEXTURES ALONE (They still use the old max_torque_safe logic)
double texture_sum = ctx.road_noise + ctx.slide_noise + ctx.spin_rumble + ctx.bottoming_crunch + ctx.abs_pulse_force + ctx.lockup_rumble;
double norm_texture = texture_sum / max_torque_safe;

// --- 7. OUTPUT SCALING ---
double norm_force = (norm_structural + norm_texture) * m_gain;
```
*Result of Stage 1:* Steering weight is now perfectly consistent across all cars. Textures and UI remain untouched.

---

#### Stage 2: Hardware Scaling Redefinition (UI & Config)
**Goal:** Now that the physics are dynamically normalized, `m_max_torque_ref` is obsolete for structural forces. We replace it with `m_wheelbase_max_nm` and `m_target_rim_nm` to give the user intuitive control over their hardware.

**Code Snippets:**
*In `Config.h` and `FFBEngine.h`:*
```cpp
// Remove m_max_torque_ref
// Add:
float m_wheelbase_max_nm = 15.0f; // e.g., Simagic Alpha
float m_target_rim_nm = 10.0f;    // What the user actually wants to feel
```

*In `FFBEngine.cpp` (Update Decoupling and Final Output):*
```cpp
// Update decoupling scale so textures scale with the user's desired rim torque
ctx.decoupling_scale = (double)m_target_rim_nm / 20.0;
if (ctx.decoupling_scale < 0.1) ctx.decoupling_scale = 0.1;

// ... (Summation happens here, textures are no longer divided by max_torque_safe) ...
double total_normalized_signal = norm_structural + texture_sum; // Textures are pre-scaled by decoupling_scale

// Map the 0.0-1.0 normalized signal to the physical wheelbase
double hardware_mapping_ratio = (double)m_target_rim_nm / (double)m_wheelbase_max_nm;
double norm_force = total_normalized_signal * hardware_mapping_ratio * m_gain;
```
*Result of Stage 2:* The 100 Nm "sweet spot" confusion is eliminated. Users tell the app exactly what wheelbase they have and exactly how heavy they want the wheel to feel.

---

#### Stage 3: Tactile Haptics Normalization (Static Load + Soft-Knee)
**Goal:** Fix the issue where road textures and vibrations become violently strong at high speeds due to aerodynamic downforce.

**Code Snippets:**
*In `FFBEngine.cpp` (Replace the current `texture_load_factor` logic):*
```cpp
// Use the static load reference (which ignores aero) instead of the peak load
double load_multiplier = ctx.avg_load / m_static_front_load;

// Apply Soft-Knee Compression
double compressed_load_factor = load_multiplier;
double knee_threshold = 1.5; 
double knee_width = 0.5;

if (load_multiplier > (knee_threshold + knee_width/2.0)) {
    // Hard compressed region (4:1 ratio)
    compressed_load_factor = knee_threshold + (load_multiplier - knee_threshold) * 0.25; 
} else if (load_multiplier > (knee_threshold - knee_width/2.0)) {
    // Soft knee quadratic transition
    double diff = load_multiplier - (knee_threshold - knee_width/2.0);
    compressed_load_factor = load_multiplier - (diff * diff) / (2.0 * knee_width);
}

ctx.texture_load_factor = compressed_load_factor;
```
*Result of Stage 3:* Textures feel alive and dynamic at low speeds, but no longer clip or shake the rig violently at the end of long straights in high-downforce cars.

### Recommendation
I highly recommend starting with **Stage 1**. It requires zero changes to the GUI, zero changes to the Config parser, and zero changes to the texture math. It strictly isolates the fix to the steering rack and SoP forces, allowing you to test the dynamic normalization in isolation.