


Based on a deep review of the `lmuFFB` codebase and the provided research report, I have identified exactly why users are reporting 100 Nm as the "ideal" value for `m_max_torque_ref`, and how the application's normalization architecture needs to be overhauled to align with industry best practices.

Here is the comprehensive investigation and the required architectural fixes.

---

### Part 1: The 100 Nm Mystery Investigated

Users reporting that 100 Nm is the "sweet spot" for `m_max_torque_ref` is a symptom of two distinct mathematical quirks in the current `FFBEngine.cpp` pipeline, depending on which `m_torque_source` they are using.

Currently, `m_max_torque_ref` does **not** represent the user's wheelbase strength or their desired rim torque. Instead, it acts as the **denominator** that normalizes the car's absolute physics torque into a `0.0 - 1.0` DirectInput signal. 

Here is why 100 Nm is happening:

#### Quirk A: The "Math Cancellation" (When using In-Game FFB / 400Hz)
If a user is using `m_torque_source == 1` (which is the default and recommended for LMU 1.2+), look at how the base force is calculated in `FFBEngine.cpp`:
```cpp
// 1. Input is multiplied by m_max_torque_ref
double raw_torque_input = (m_torque_source == 1) ? (double)genFFBTorque * (double)m_max_torque_ref : data->mSteeringShaftTorque;

// ... effects are added to total_sum ...

// 2. Output is divided by m_max_torque_ref
double norm_force = total_sum / max_torque_safe; 
```
Because `genFFBTorque` is already a normalized `` value provided by the game, multiplying it by `m_max_torque_ref` and then dividing it by `m_max_torque_ref` at the end **causes the value to perfectly cancel out**. 
Changing `m_max_torque_ref` from 20 Nm to 100 Nm does absolutely nothing to the base steering weight in this mode. Users leave it at 100 Nm simply because the default presets (`T300`, `Simagic`, `Moza`) hardcode it to ~100.0f, and changing it yields no perceived difference in steering weight.

#### Quirk B: The "Clipping Avoidance" (When using Shaft Torque / 100Hz)
If a user is using `m_torque_source == 0` (Raw Physics Torque), the game outputs absolute Newton-meters (e.g., 35 Nm for an LMP2). 
If `m_max_torque_ref` is set to 35 Nm, the app outputs 100% signal (`35 / 35 = 1.0`). If the user hits a curb and the physics spike to 50 Nm, the signal violently clips at 1.0.
By setting `m_max_torque_ref` to **100 Nm**, the user forces the app to output a much smaller signal (`35 / 100 = 35%`). This provides massive headroom, completely eliminating clipping. The user then simply turns up the gain on their physical wheelbase software to compensate.

**Conclusion on the Sweet Spot:**
There is no "sweet spot" for `m_max_torque_ref` in the current architecture because it is fundamentally acting as a manual physics limiter rather than a user comfort setting. 

---

### Part 2: Required Architectural Fixes (The "Physical Target" Model)

To fix this and properly normalize forces across GT3s, LMP2s, and Hypercars as outlined in the research report, we must replace the manual `m_max_torque_ref` with a **Session-Learned Dynamic Normalization** system, and separate the hardware scaling.

Here is the blueprint for updating `FFBEngine.cpp` and `FFBEngine.h`.

#### 1. Implement the Asymmetric Peak Follower (Steering Weight)
We need to automatically learn the car's peak torque over a few laps so the user doesn't have to guess it.

**In `FFBEngine.h`:**
```cpp
// Add to FFBEngine class
double m_session_peak_torque = 25.0; // Safe starting baseline
double m_peak_torque_smoothed = 25.0;
```

**In `FFBEngine.cpp` (Inside `calculate_force`):**
```cpp
// 1. Outlier Rejection (G-Force & Slew Gating)
double lat_g_abs = std::abs(data->mLocalAccel.x / 9.81);
double torque_slew = std::abs(raw_torque_input - m_last_raw_torque) / ctx.dt;
m_last_raw_torque = raw_torque_input;

bool is_clean_state = (lat_g_abs < 8.0) && (torque_slew < 1000.0) && !restricted;

// 2. Fast-Attack, Slow-Decay Envelope Follower
if (is_clean_state && m_torque_source == 0) {
    double current_abs_torque = std::abs(raw_torque_input);
    if (current_abs_torque > m_session_peak_torque) {
        // Fast Attack (Instant)
        m_session_peak_torque = current_abs_torque;
    } else {
        // Slow Decay (e.g., 0.05 Nm per second to prevent hunting)
        m_session_peak_torque -= (0.05 * ctx.dt);
    }
    // Safety floors/ceilings
    m_session_peak_torque = std::clamp(m_session_peak_torque, 15.0, 120.0);
}

// 3. EMA Smoothing for the Peak (Prevents digital notchiness)
double alpha_peak = ctx.dt / (0.25 + ctx.dt); // ~250ms time constant
m_peak_torque_smoothed += alpha_peak * (m_session_peak_torque - m_peak_torque_smoothed);
```

#### 2. Fix Tactile Haptics Normalization (Static Load Anchoring)
Currently, `ctx.texture_load_factor` uses `ctx.avg_load / m_auto_peak_load`. As the research report notes, aero downforce ruins this at high speeds. We must anchor textures to the *Static Load*.

**In `FFBEngine.cpp`:**
```cpp
// Replace the current texture_load_factor logic with:

// 1. Use the existing static load learner (which learns between 2-15 m/s)
double load_multiplier = ctx.avg_load / m_static_front_load;

// 2. Apply Soft-Knee Compression for Safety (Protects against Eau Rouge compressions)
double compressed_load_factor = load_multiplier;
double knee_threshold = 1.5; // Start compressing at 1.5x static weight
double knee_width = 0.5;

if (load_multiplier > (knee_threshold + knee_width/2.0)) {
    // Hard compressed region
    compressed_load_factor = knee_threshold + (load_multiplier - knee_threshold) * 0.25; // 4:1 ratio
} else if (load_multiplier > (knee_threshold - knee_width/2.0)) {
    // Soft knee quadratic transition
    double diff = load_multiplier - (knee_threshold - knee_width/2.0);
    compressed_load_factor = (load_multiplier) - (diff * diff) / (2.0 * knee_width);
}

ctx.texture_load_factor = compressed_load_factor;
```

#### 3. Redefine Hardware Scaling (The New `m_max_torque_ref`)
We must split the old `m_max_torque_ref` into two distinct user settings in the GUI/Config:
1. `m_wheelbase_max_nm`: The physical maximum of the user's wheelbase (e.g., 15 Nm for Simagic Alpha).
2. `m_target_rim_nm`: The maximum force the user *wants to feel* during peak cornering (e.g., 10 Nm).

**In `FFBEngine.cpp` (Final Output Stage):**
```cpp
// Calculate the normalization denominator based on the source
double normalization_denominator = (m_torque_source == 1) ? 1.0 : m_peak_torque_smoothed;

// Normalize the physics sum to a 0.0 - 1.0 range
double normalized_signal = total_sum / normalization_denominator;

// Map to physical hardware
// If the user wants 10 Nm on a 15 Nm wheelbase, the max signal sent to DirectInput is 0.66 (66%)
double hardware_mapping_ratio = m_target_rim_nm / m_wheelbase_max_nm;

double norm_force = normalized_signal * hardware_mapping_ratio;
norm_force *= m_gain; // Master volume trim
```

### Summary of Action Plan for the Codebase
1. **Deprecate `m_max_torque_ref`**: Remove it from `Config.cpp` and `GuiLayer_Win32.cpp`. Replace it with `m_wheelbase_max_nm` and `m_target_rim_nm`.
2. **Update `calculate_force`**: Insert the Asymmetric Peak Follower to dynamically track `data->mSteeringShaftTorque`.
3. **Update Texture Scaling**: Change `ctx.texture_load_factor` to use `m_static_front_load` combined with the Soft-Knee compression algorithm.
4. **Update Decoupling Scale**: Change `ctx.decoupling_scale` to use `m_target_rim_nm / 20.0` so that synthetic effects (like ABS and Road Texture) scale correctly with the user's desired rim strength, rather than the car's physics peak.