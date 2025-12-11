Based on the analysis of the new features and the existing codebase, here is the stability risk assessment for the FFB components.

The introduction of **Calculated Physics** (deriving values rather than reading them) introduces three primary mathematical risks: **Singularities** (Division by Zero), **Noise Amplification** (Derivatives), and **Oscillation** (Sign flipping).

Here is the breakdown by component and the required mitigations.

---

### 1. New Implementations: Stability Risks & Mitigations

#### A. Manual Slip Ratio Calculation (Inference)
*   **The Formula:** $\text{Ratio} = \frac{V_{wheel} - V_{car}}{V_{car}}$
*   **The Risk: Singularity (Division by Zero).**
    *   When the car is stopped or moving very slowly ($V_{car} \approx 0$), the denominator becomes tiny.
    *   A wheel rotating at just 1 rad/s while the car is stationary results in a Slip Ratio of **Infinity**.
    *   **Result:** The FFB engine will output `NaN` (Not a Number) or `Infinity`, causing the FFB driver to crash or the wheel to snap to max force instantly.
*   **Mitigation:** **Low Speed Trap.**
    *   If `abs(V_car) < 2.0` m/s (approx 7 kph), force `Ratio = 0.0`.
    *   Alternatively, add an epsilon to the denominator: `V_car + 0.001`, but the hard threshold is safer for FFB.

#### B. Scrub Drag (New Effect)
*   **The Logic:** Apply a constant force opposing the direction of `mLateralPatchVel`.
*   **The Risk: Oscillation (The "Ping-Pong" Effect).**
    *   This acts like a friction force. If the car is sliding very slowly (e.g., 0.01 m/s), the force pushes it back.
    *   If the force is too strong, it pushes the velocity past zero to -0.01 m/s.
    *   The force flips direction instantly.
    *   **Result:** The steering wheel buzzes or vibrates violently around the center when driving straight or nearly straight.
*   **Mitigation:** **Linear Fade-In (Windowing).**
    *   Do not apply full force immediately.
    *   Define a "blending window" (e.g., 0.0 to 0.5 m/s).
    *   If `Vel < 0.5`, `Force = MaxForce * (Vel / 0.5)`.
    *   This ensures the force approaches zero smoothly as the slide stops.

#### C. Approximate `mTireLoad` (Workaround)
*   **The Logic:** `Load = SuspForce + Aero`.
*   **The Risk: Unbounded Growth.**
    *   Aerodynamic downforce scales with the square of speed ($V^2$). At 300 km/h, the load can be massive.
    *   If this calculated load is used as a multiplier for textures (Road/Slide), the texture amplitude might become 5x or 10x stronger at high speed than low speed.
    *   **Result:** Hitting a small bump at 300 km/h could break a wrist or clip the signal hard.
*   **Mitigation:** **Hard Output Clamp.**
    *   Apply a `std::min` cap to the final `LoadFactor`.
    *   Example: `LoadFactor = min(2.0, CalculatedLoad / ReferenceLoad)`. Never let it exceed 2x the static weight.

#### D. Universal Bottoming (Method B: Force Inference)
*   **The Logic:** Detect a spike in the *derivative* (rate of change) of `mSuspForce`.
*   **The Risk: Noise Amplification.**
    *   Differentiation amplifies high-frequency noise. A single "bad frame" of telemetry where `mSuspForce` jumps slightly due to a sensor glitch will look like a massive spike to the derivative logic.
    *   **Result:** False positives. The wheel "crunches" randomly even on smooth roads.
*   **Mitigation:** **Threshold + Debounce.**
    *   Set the trigger threshold very high (it must be a *violent* spike).
    *   Optional: Require the spike to persist for 2 consecutive frames (Debouncing), though this adds latency. A better approach is simply a very high threshold (e.g., 100,000 N/s).

#### E. Tire Radius (Unit Conversion)
*   **The Logic:** `mStaticUndeflectedRadius` is `unsigned char` (cm).
*   **The Risk: Integer Division / Truncation.**
    *   If you write `radius / 100`, C++ might treat it as integer division (result 0).
    *   **Result:** Radius = 0. Wheel Speed = 0. Slip Ratio = -1.0 (Lockup). The wheel vibrates constantly.
*   **Mitigation:** **Explicit Casting.**
    *   Must use `(double)radius / 100.0`.

---

### 2. Existing Components: Stability Risks & Mitigations

#### F. Road Texture (High Pass Filter)
*   **The Logic:** `Delta = CurrentDeflection - PrevDeflection`.
*   **The Risk: Teleport/Reset Spikes.**
    *   When the player resets to the pits or restarts the race, `CurrentDeflection` changes instantly (e.g., from compressed on track to static in pits).
    *   The `Delta` becomes huge (e.g., 0.1m in 0.002s).
    *   **Result:** A massive "BANG" in the wheel every time you restart.
*   **Mitigation:** **Delta Clamping.**
    *   Limit the maximum `Delta` per frame to a physically reasonable value (e.g., equivalent to hitting a large curb, maybe 0.01m). If `Delta > 0.01`, ignore it or clamp it.

#### G. SoP (Lateral G)
*   **The Logic:** Injects Lateral G force.
*   **The Risk: Wall Impacts.**
    *   Hitting a wall generates massive G-forces (50G+).
    *   **Result:** The wheel snaps violently, potentially causing injury on high-torque Direct Drive wheels.
*   **Mitigation:** **Input Saturation.**
    *   Clamp the `LatG` input before calculation.
    *   Example: `LatG = std::max(-5.0, std::min(5.0, raw_lat_g))`. (Cap at 5G).

---

### Summary of Required Mitigations for the AI Agent

When prompting the AI, you must explicitly list these constraints to ensure the code is safe:

1.  **Manual Slip Ratio:** "Implement a check: if `CarSpeed < 2.0 m/s`, set Slip Ratio to 0.0 to avoid division by zero."
2.  **Scrub Drag:** "Implement a linear fade-in window for the Scrub Drag force between 0.0 and 0.5 m/s lateral velocity to prevent oscillation."
3.  **Approximate Load:** "Clamp the final `CalculatedLoad` factor to a maximum of 2.0 to prevent excessive forces at high aerodynamic speeds."
4.  **Road Texture:** "Clamp the calculated `suspension_delta` to a maximum of +/- 0.01 meters per frame to prevent violent spikes during car resets/teleports."
5.  **Tire Radius:** "Ensure `mStaticUndeflectedRadius` is cast to `double` or `float` before dividing by 100.0."