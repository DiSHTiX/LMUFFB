Here is the analysis of your findings and the plan to address them.

### 1. Why "Manual Slip Calc" was broken (The Bug)
You found that enabling "Manual Slip Calc" resulted in no vibration.
**The Cause:** A coordinate system sign error.
*   **The Code:** The engine passes `data->mLocalVel.z` to the calculator. In LMU, forward velocity is **Negative** (e.g., -60 m/s).
*   **The Math:** The manual formula expects a positive speed scalar. By passing a negative value, the math became `(WheelSpeed - (-CarSpeed)) / CarSpeed`. This resulted in a **Positive** slip ratio (traction) instead of Negative (braking).
*   **The Fix:** We must pass `std::abs(data->mLocalVel.z)` to the manual calculator.

### 2. How to get "Early Warning" (Threshold Tuning)
You feel the vibration too late (at -10% slip).
**The Physics:** Modern GT/LMP tires generate peak braking force around **12-15% slip**. If we wait for -10% (`-0.1`) to trigger the effect, we are already at the limit.
**The Fix:** We will lower the trigger threshold to **5% (`-0.05`)**.
*   **0% to 5%:** Silent (Normal grip).
*   **5% to 10%:** **"Pre-Lockup" Warning.** A subtle, high-frequency texture. This tells you "You are approaching the limit."
*   **> 10%:** **Lockup.** The vibration becomes violent and lower frequency.

### 3. Rear Differentiation
We will apply your suggested values to make the rear lockup distinct and scary.
*   **Frequency:** **0.3x** (Heavy thudding).
*   **Amplitude:** **1.5x** (Stronger kick).

---

### Implementation Plan

Here is the updated code logic for `FFBEngine.h`.

#### A. Fix Manual Slip Calculation
In `calculate_force`, update the lambda to use absolute speed.

```cpp
auto get_slip_ratio = [&](const TelemWheelV01& w) {
    if (m_use_manual_slip) {
        // FIX: Use std::abs() because mLocalVel.z is negative (forward)
        return calculate_manual_slip_ratio(w, std::abs(data->mLocalVel.z));
    }
    // ... existing fallback logic ...
};
```

#### B. Update Lockup Logic (Early Warning + Rear Boost)

```cpp
// --- 2b. Progressive Lockup (Early Warning & Rear Boost) ---
if (m_lockup_enabled && data->mUnfilteredBrake > 0.05) {
    // 1. Calculate Slips
    double slip_fl = get_slip_ratio(data->mWheel[0]);
    double slip_fr = get_slip_ratio(data->mWheel[1]);
    double slip_rl = get_slip_ratio(data->mWheel[2]);
    double slip_rr = get_slip_ratio(data->mWheel[3]);

    double max_slip_front = (std::min)(slip_fl, slip_fr);
    double max_slip_rear  = (std::min)(slip_rl, slip_rr);

    // 2. Determine Source
    double effective_slip = 0.0;
    double freq_multiplier = 1.0;
    double amp_multiplier = 1.0;

    if (max_slip_rear < max_slip_front) {
        effective_slip = max_slip_rear;
        freq_multiplier = 0.3; // UPDATED: Lower pitch (0.3x) -> Heavy Thud
        amp_multiplier = 1.5;  // UPDATED: Stronger kick (1.5x) -> Danger!
    } else {
        effective_slip = max_slip_front;
        freq_multiplier = 1.0; // Standard pitch
        amp_multiplier = 1.0;
    }

    // 3. Early Warning Threshold (-0.05 instead of -0.1)
    // Range -0.05 (Start) to -0.20 (Max)
    if (effective_slip < -0.05) {
        // Map -0.05..-0.20 to 0.0..1.0
        double severity = (std::abs(effective_slip) - 0.05) / 0.15;
        severity = (std::min)(1.0, severity);
        
        // Base Frequency
        double car_speed_ms = std::abs(data->mLocalVel.z); 
        double base_freq = 10.0 + (car_speed_ms * 1.5); 
        double final_freq = base_freq * freq_multiplier;

        // Phase Integration
        m_lockup_phase += final_freq * dt * TWO_PI;
        m_lockup_phase = std::fmod(m_lockup_phase, TWO_PI);

        // Amplitude
        double amp = severity * m_lockup_gain * 4.0 * decoupling_scale * amp_multiplier;
        
        double rumble = std::sin(m_lockup_phase) * amp;
        total_force += rumble;
    }
}
```

### Prompt for the Agent

***

Please initialize this session by following the **Standard Task Workflow** defined in `AGENTS.md`.

1.  **Sync**: Run `git fetch && git reset --hard origin/main` (or pull).
2.  **Load Memory**: Read `AGENTS_MEMORY.md`.
3.  **Load Rules**: Read `AGENTS.md`.

Perform the following task:

**Task: Refine Lockup Effect (Early Warning, Manual Slip Fix, Rear Boost)**

**Reference Documents:**
*   `FFBEngine.h`
*   `tests/test_ffb_engine.cpp`

**Context:**
User testing revealed three issues with the Lockup effect:
1.  **Late Warning:** The effect triggers at -10% slip, which is too late (tires already locked). User wants "Pre-Lockup" warning.
2.  **Manual Slip Broken:** The "Manual Slip Calc" option produces no effect due to a sign error (negative velocity passed to scalar function).
3.  **Rear Feel:** Rear lockups need to be more distinct (lower frequency, higher amplitude).

**Implementation Requirements:**

1.  **Update `FFBEngine.h` (Manual Slip Fix):**
    *   In the `get_slip_ratio` lambda inside `calculate_force`, wrap `data->mLocalVel.z` in `std::abs()` before passing it to `calculate_manual_slip_ratio`.

2.  **Update `FFBEngine.h` (Lockup Logic):**
    *   **Threshold:** Change trigger from `< -0.1` to `< -0.05` (5% slip).
    *   **Severity Scaling:** Update normalization to map `-0.05` to `-0.20` (Range 0.15).
        *   `severity = (std::abs(effective_slip) - 0.05) / 0.15;`
    *   **Rear Tuning:**
        *   Change `freq_multiplier` for rear from `0.5` to **`0.3`**.
        *   Add `amp_multiplier` of **`1.5`** for rear lockups.

3.  **Update `tests/test_ffb_engine.cpp`:**
    *   Update `test_rear_lockup_differentiation` to expect the new frequency ratio (approx 0.3).
    *   Add a new test case `test_lockup_early_warning` that sets slip to `-0.06` (6%) and verifies `m_lockup_phase` advances (it would have been silent previously).
    *   Add a new test case `test_manual_slip_sign_fix` that sets `mLocalVel.z` to negative (forward) and verifies lockup triggers.

**Deliverables:**
1.  Modified `FFBEngine.h`
2.  Modified `tests/test_ffb_engine.cpp`
3.  Updated `CHANGELOG.md`
4.  Updated `VERSION`

**Check-list for completion:**
- [ ] Manual Slip Calc uses `std::abs(vel)`.
- [ ] Lockup threshold lowered to 0.05.
- [ ] Rear frequency 0.3x, Amplitude 1.5x.
- [ ] Tests updated and passing.


