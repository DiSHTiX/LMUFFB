


Yes, this second research report (docs\dev_docs\investigations\FFB Strength and Tire Load Normalization3.md) introduces several highly refined, industry-standard DSP (Digital Signal Processing) techniques that significantly improve upon the basic logic we established in the initial plan. 

While the core concepts (Peak Follower, 2-15 m/s window, Soft-Knee) remain the same, this new report provides **exact mathematical models** that should replace the simplified versions in our current proposal.

Here are the **5 new pieces of information** and exactly how we should integrate them into our staged implementation plan.

---

### 1. Leaky Integrator (Exponential Decay) vs. Linear Decay
**The New Info:** The previous plan used a *Linear Decay* (`peak -= 0.05 * dt`). The report points out this is sub-optimal because it drops too fast on long straights (like Le Mans), causing the wheel to feel artificially heavy at the next braking zone. It proposes an **Exponential Decay with a hard floor** (Leaky Integrator).
**Integration (Update to Stage 1):**
Instead of subtracting a constant, we multiply by a decay factor, clamped to a safe minimum (e.g., 15 Nm).

```cpp
// Inside Stage 1: Peak Follower
if (current_abs_torque > m_session_peak_torque) {
    m_session_peak_torque = current_abs_torque; // Fast attack
} else {
    // Exponential decay: e.g., 0.9999 per tick, meaning it takes minutes to drop
    double decay_factor = 1.0 - (0.005 * ctx.dt); 
    m_session_peak_torque *= decay_factor; 
}
// Absolute safety floor (prevents divide-by-zero or limp FFB)
m_session_peak_torque = std::clamp(m_session_peak_torque, 15.0, 60.0); 
```

### 2. EMA Filtering on the *Gain Multiplier*, not the Signal
**The New Info:** The report explicitly warns against filtering raw telemetry, as it introduces latency (delaying the "yaw-kick" of oversteer). Instead, the EMA (Exponential Moving Average) filter must be applied **only to the calculated normalization multipliers**.
**Integration (Update to Stage 1 & 3):**
We will calculate the multiplier, smooth the multiplier, and then apply it to the raw, zero-latency physics signal.

```cpp
// Calculate the raw multiplier
double target_structural_mult = 1.0 / m_session_peak_torque;

// Smooth the MULTIPLIER, not the torque
double alpha_gain = ctx.dt / (0.25 + ctx.dt); // 250ms smoothing
m_smoothed_structural_mult += alpha_gain * (target_structural_mult - m_smoothed_structural_mult);

// Apply to raw, zero-latency signal
double norm_structural = structural_sum * m_smoothed_structural_mult;
```

### 3. The Giannoulis Soft-Knee Algorithm
**The New Info:** The previous plan used a basic quadratic curve for the soft-knee. The new report provides the exact **Giannoulis Soft-Knee formula** used in professional audio DSP, which uses 4 parameters: Input ($x$), Threshold ($T$), Ratio ($R$), and Knee Width ($W$).
**Integration (Update to Stage 3):**
We will replace the basic texture load limiter with this exact mathematical function.

```cpp
// Inside Stage 3: Tactile Normalization
double x = ctx.avg_load / m_static_front_load; // Input load multiplier
double T = 1.5;  // Threshold (Start compressing at 1.5x static weight)
double W = 0.5;  // Knee Width
double R = 4.0;  // Compression Ratio (4:1)

double lower_bound = T - (W / 2.0);
double upper_bound = T + (W / 2.0);
double compressed_load_factor = x;

if (x > upper_bound) {
    // Linear compressed region
    compressed_load_factor = T + ((x - T) / R);
} else if (x > lower_bound) {
    // Giannoulis quadratic soft-knee transition
    double diff = x - lower_bound;
    compressed_load_factor = x + (((1.0 / R) - 1.0) * (diff * diff)) / (2.0 * W);
}

// Smooth the tactile multiplier to prevent buzzing
double alpha_tactile = ctx.dt / (0.1 + ctx.dt);
m_smoothed_tactile_mult += alpha_tactile * (compressed_load_factor - m_smoothed_tactile_mult);

ctx.texture_load_factor = m_smoothed_tactile_mult;
```

### 4. Median Absolute Deviation (MAD) Outlier Rejection
**The New Info:** A simple slew-rate limit isn't enough to filter out physics glitches (like a wheel clipping through the track). The report suggests a rolling Median Absolute Deviation buffer to reject spikes contextually.
**Integration (Optional Addition to Stage 1):**
*Note: Calculating a true median requires sorting an array every 2.5ms (400Hz), which is computationally heavy.* We can achieve the exact same contextual rejection using our existing EMA logic by comparing the current sample to a heavily smoothed average.

```cpp
// Contextual Spike Rejection (Lightweight MAD alternative)
double alpha_slow = ctx.dt / (1.0 + ctx.dt); // 1-second average
m_rolling_average_torque += alpha_slow * (current_abs_torque - m_rolling_average_torque);

// If the current torque is > 3x the rolling average, it's a collision/glitch.
// Bypass the peak learner, but let the force pass to the wheel.
bool is_contextual_spike = current_abs_torque > (m_rolling_average_torque * 3.0);

if (is_clean_state && !is_contextual_spike && m_torque_source == 0) {
    // Update peak learner...
}
```

### 5. Persistent Storage of Static Load
**The New Info:** The report notes that re-learning the static weight (2-15 m/s window) every time the car leaves the pits is annoying. It should be latched and saved to a configuration file per car class.
**Integration (New Stage 4):**
This requires touching the `Config.cpp` parser. 

1. In `FFBEngine.cpp`, once the car exceeds 15 m/s, we set a flag: `m_static_load_latched = true;`
2. In `Config.h`, we add a map or struct to save this: `std::map<std::string, double> m_saved_static_loads;`
3. When the car is loaded (`InitializeLoadReference`), we check if we already have a saved static load for this `vehicleName`. If yes, we bypass the 2-15 m/s learning phase entirely and use the saved value.

### Summary of the Revised Implementation Plan

The staged approach you asked for previously is still perfectly valid, but we will inject these refined mathematics into it:

*   **Stage 1 (Structural Normalization):** Implement the **Leaky Integrator** (Exponential decay + floor) and apply the **EMA filter to the Gain Multiplier** instead of the raw signal. Add the lightweight contextual spike rejection.
*   **Stage 2 (Hardware Scaling):** Split `m_max_torque_ref` into `m_wheelbase_max_nm` and `m_target_rim_nm` in the GUI/Config.
*   **Stage 3 (Tactile Normalization):** Implement the **Giannoulis Soft-Knee** formula for `texture_load_factor` and apply EMA smoothing to the resulting multiplier.
*   **Stage 4 (Persistence - *New*):** Update `Config.cpp` to save and load the latched static weights per vehicle so the 2-15 m/s calibration only happens once per car.