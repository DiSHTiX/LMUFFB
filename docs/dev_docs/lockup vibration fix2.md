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


### 1. Current Formula & Physics Analysis

**Current Formula:**
Currently, we calculate the **Slip Ratio** ($\lambda$).
$$ \lambda = \frac{V_{patch}}{V_{ground}} $$
*   **Trigger:** If $\lambda < -0.10$ (10% slip).
*   **Amplitude:** Linearly scales from 10% to 50% slip (hardcoded range).
*   **Frequency:** Based on Car Speed.

**Is this the best we can do?**
Strictly speaking, **no**. While Slip Ratio is the standard metric, real-world ABS systems and pro drivers use an additional metric: **Wheel Angular Deceleration**.
*   **The Concept:** Before the slip ratio becomes critical, the wheel's rotation speed drops rapidly. If the wheel is decelerating significantly faster than the car is decelerating, a lockup is imminent.
*   **Can we use it?** Yes. We have `mRotation` (Angular Velocity). We can calculate its derivative (Acceleration).
*   **Risk:** Derivatives are noisy. It might trigger false positives over bumps.
*   **Recommendation:** For now, refining the **Slip Ratio** is safer and more robust. If that isn't enough, we can explore Angular Deceleration in v0.6.0.

### 2. Manual Slip Calc vs. Game Data

**Is Manual Calc inferior?**
Technically, **yes**.
*   **Game Data (`mLongitudinalPatchVel`):** Accounts for tire carcass deformation, pressure, and temperature. It is the "True Physics" slip.
*   **Manual Calc:** Assumes the tire is a rigid disk with a fixed radius. It ignores that tires "stretch" under braking.
*   **Integration:** We should not mix them. We should use Game Data as primary, and Manual Calc only if the Game Data is blocked (0.0).

### 3. Dynamic Ramp & Thresholds

You are absolutely correct. **20% is too late.**
*   **Peak Braking:** Usually occurs around **12% to 15%** slip for slick tires.
*   **The Goal:** You want 100% vibration intensity exactly at the **Peak (12-15%)**, not after it. If you feel max vibration, you know you are at max braking power. Pushing harder loses grip.

**Proposed Customization (3 Sliders):**
Instead of 3 separate thresholds (which is complex to tune), I propose defining the **Window**:

1.  **Start Threshold (Warning):** When the vibration *starts* (0% Amp).
    *   *Recommendation:* **4% - 6%**. (Just entering the braking zone).
2.  **Full Effect Threshold (Limit):** When the vibration hits **100% Amp**.
    *   *Recommendation:* **12% - 15%**. (The optimal peak).
3.  **Rear Boost:** (As discussed).

**Non-Linearity:**
A **Square Curve** ($x^2$) is often better than Linear.
*   *Linear:* Vibration builds up steadily.
*   *Square:* Vibration stays subtle for longer, then ramps up sharply as you hit the limit. This creates a distinct "Wall" of vibration at the limit.

---

### Updated Implementation Plan

I will update the prompt to include:
1.  **Fix Manual Slip:** Use `abs()` for velocity.
2.  **Rear Differentiation:** Frequency 0.3x, Amplitude Boost slider.
3.  **Configurable Ramp:**
    *   **Start %:** Slider (1% - 10%).
    *   **Full %:** Slider (5% - 20%).
    *   **Curve:** We will use a **Quadratic Ramp** (Square) by default for that "sharp limit" feel.

Here is the prompt for the agent.

***

## Prompt

Please initialize this session by following the **Standard Task Workflow** defined in `AGENTS.md`.

1.  **Sync**: Run `git fetch && git reset --hard origin/main` (or pull).
2.  **Load Memory**: Read `AGENTS_MEMORY.md`.
3.  **Load Rules**: Read `AGENTS.md`.

Perform the following task:

**Task: Advanced Lockup Tuning (Thresholds, Rear Boost, Manual Slip Fix)**

**Reference Documents:**
*   `FFBEngine.h`
*   `src/Config.h` / `src/Config.cpp`
*   `src/GuiLayer.cpp`
*   `tests/test_ffb_engine.cpp`

**Context:**
User testing revealed the need for precise tuning of the Lockup effect to act as an "Early Warning" system for threshold braking.
1.  **Manual Slip Fix:** The "Manual Slip Calc" option is broken due to a sign error (negative velocity).
2.  **Rear Differentiation:** Rear lockups need to be distinct (Lower Frequency) and have a configurable intensity boost.
3.  **Configurable Ramp:** The user wants to define the slip window (Start % vs Full %) to match the tire's peak grip.

**Implementation Requirements:**

1.  **Update `FFBEngine.h`**:
    *   **Add Members:**
        *   `float m_lockup_start_pct = 5.0f;` (Default 5.0%).
        *   `float m_lockup_full_pct = 15.0f;` (Default 15.0%).
        *   `float m_lockup_rear_boost = 1.5f;` (Default 1.5x).
    *   **Fix Manual Slip:** In `calculate_force`, wrap `data->mLocalVel.z` in `std::abs()` before passing to `calculate_manual_slip_ratio`.
    *   **Update Lockup Logic:**
        *   Calculate slip for **all 4 wheels**.
        *   Determine if Rear Slip is worse than Front Slip.
        *   **Frequency:** If Rear is worse, use `freq_multiplier = 0.3`. Else `1.0`.
        *   **Amplitude Boost:** If Rear is worse, apply `m_lockup_rear_boost`.
        *   **Ramp Logic:**
            *   `start = m_lockup_start_pct / 100.0`
            *   `full = m_lockup_full_pct / 100.0`
            *   `width = full - start`
            *   `normalized = (abs(slip) - start) / width`
            *   `severity = clamp(normalized, 0.0, 1.0)`
            *   **Curve:** Use `severity * severity` (Quadratic) to make the limit feel sharper.

2.  **Update Configuration (`src/Config` files)**:
    *   Add `lockup_start_pct`, `lockup_full_pct`, and `lockup_rear_boost` to `Preset` struct.
    *   Update `Save`, `Load`, `LoadPresets`, `Apply`, and `UpdateFromEngine`.
    *   **Defaults:** Start=5.0, Full=15.0, RearBoost=1.5.

3.  **Update GUI (`src/GuiLayer.cpp`)**:
    *   In the "Lockup Vibration" section:
        *   Add Slider: **"Start Slip %"** (Range: 1.0% to 10.0%). Tooltip: "Slip % where vibration begins (Early Warning)."
        *   Add Slider: **"Full Slip %"** (Range: 5.0% to 25.0%). Tooltip: "Slip % where vibration hits 100% (Peak Grip Limit)."
        *   Add Slider: **"Rear Boost"** (Range: 1.0x to 3.0x). Tooltip: "Amplitude multiplier for rear wheel lockups."

4.  **Update Tests (`tests/test_ffb_engine.cpp`)**:
    *   Update `test_rear_lockup_differentiation` to verify the frequency drop (0.3x).
    *   Add `test_lockup_ramp_config` to verify that changing `m_lockup_start_pct` changes the trigger point.
    *   Add `test_manual_slip_sign_fix` to verify manual calc works with negative velocity.

**Deliverables:**
1.  Modified `FFBEngine.h`
2.  Modified `src/Config.h` & `src/Config.cpp`
3.  Modified `src/GuiLayer.cpp`
4.  Modified `tests/test_ffb_engine.cpp`
5.  Updated `CHANGELOG.md` & `VERSION`

**Check-list for completion:**
- [ ] Manual Slip uses `abs(vel)`.
- [ ] Lockup logic uses configurable Start/Full thresholds.
- [ ] Rear frequency 0.3x, Amplitude configurable.
- [ ] GUI exposes 3 new sliders.
- [ ] Tests pass.