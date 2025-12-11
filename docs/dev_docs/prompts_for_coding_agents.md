
Please initialize this session by following the **Standard Task Workflow** defined in `AGENTS.md`.

1.  **Sync**: Run `git fetch && git reset --hard origin/main` (or pull) to ensure you see the latest files.
2.  **Load Memory**: Read `AGENTS_MEMORY.md` to review build workarounds and architectural insights.
3.  **Load Rules**: Read `AGENTS.md` to confirm constraints.

Perform the following task:

**Task: verify issues and implement fixes**

Carefully read the document "docs\dev_docs\grip_calculation_analysis_v0.4.5.md", verify all issues described and, if confirmed, implement the recommended fixes.
===

Here is the prompt to instruct the AI coding agent.

***

Please initialize this session by following the **Standard Task Workflow** defined in `AGENTS.md`.

1.  **Sync**: Run `git fetch && git reset --hard origin/main` (or pull) to ensure you see the latest files.
2.  **Load Memory**: Read `AGENTS_MEMORY.md` to review build workarounds and architectural insights.
3.  **Load Rules**: Read `AGENTS.md` to confirm constraints.

Perform the following task:

**Task: Implement Physics Workarounds, Advanced Slip Calculations, and New FFB Effects (v0.4.5)**

**Context:**
Le Mans Ultimate (LMU) version 1.2 currently returns 0.0 for `mTireLoad` and `mGripFract` in the shared memory, breaking several FFB effects. We need to implement physics-based approximations to restore this functionality. Additionally, we want to leverage available data (Patch Velocity, Wheel Rotation, Ride Height) to create new effects and more robust slip calculations.
Refer to **`docs/dev_docs/workaounds_and_improvements_ffb_v0.4.4+.md`** for the detailed physics analysis and formulas required for this task.

**Implementation Requirements:**

1.  **Physics Approximations (Workarounds):**
    *   **Approximate `mTireLoad`:** Create a calculated value `calc_load`.
        *   Formula: `mSuspForce` + (Estimated Unsprung Mass ~300N).
        *   *Optional Enhancement:* Add `(mFrontDownforce / 2)` if available in `TelemInfoV01`.
    *   **Approximate `mGripFract`:** Create a calculated value `calc_grip`.
        *   Logic: Derive from Slip Angle. If `abs(SlipAngle) > Optimal (0.15 rad)`, reduce grip.
        *   Formula: `1.0 - max(0.0, (abs(calculated_slip_angle) - 0.15) * falloff_factor)`.

2.  **Advanced Slip & Lockup Calculation:**
    *   Implement a **Manual Slip Ratio** calculation to detect Lockups/Spin even if game telemetry is broken.
    *   **Formula:** `Ratio = (V_wheel - V_car) / V_car`.
        *   `V_wheel`: `mRotation` (rad/s) * `Radius` (m).
        *   `Radius`: Convert `mStaticUndeflectedRadius` (unsigned char, cm) to meters (`val / 100.0`).
        *   `V_car`: `mLocalVel.z`.
    *   **GUI Option:** Add a toggle/dropdown to select the source for Lockup/Spin effects: **"Game Data (mSlipRatio)"** vs **"Calculated (Wheel Speed)"**.

3.  **Universal Bottoming Logic:**
    *   Implement two detection methods for the Bottoming effect:
        *   **Method A (Scraping):** Trigger if `mRideHeight` < Threshold (e.g., 0.002m).
        *   **Method B (Suspension Force):** Trigger if derivative of `mSuspForce` exceeds a threshold (Spike detection).
    *   **GUI Option:** Add a setting to choose between "Chassis Scraping" or "Suspension Bump Stops".

4.  **New & Improved Effects:**
    *   **Scrub Drag:** New effect. If `mLateralPatchVel` is high, add a constant force *opposing* the slide direction to simulate rubber dragging resistance.
    *   **Alternative Understeer:** Use the `calc_grip` (from Step 1) to drive the Understeer effect if the raw `mGripFract` is detected as 0.

5.  **GUI & Visualization (`GuiLayer.cpp`):**
    *   Add sliders/checkboxes for all new options (Calculation Methods, Scrub Drag Gain).
    *   Update **Troubleshooting Graphs**: Add traces for `Calc Load`, `Calc Grip`, `Calc Slip Ratio`, and `Ride Height`.

6.  **Safety & Robustness:**
    *   Add sanity checks for all new inputs (`mSuspForce`, `mRideHeight`, `mStaticUndeflectedRadius`).
    *   Print a "One-time" console warning if these new inputs are missing/zero.

**Deliverables:**
1.  **Source Code:** Updated `FFBEngine.h`, `GuiLayer.cpp`, `Config.h`, `Config.cpp`.
2.  **Tests:** Update `tests/test_ffb_engine.cpp` to verify:
    *   Manual Slip Ratio calculation (math correctness).
    *   Radius unit conversion (cm to m).
    *   Bottoming logic triggers (Ride height vs Force).
3.  **Documentation:**
    *   Update `docs/dev_docs/FFB_formulas.md` with the new approximation formulas.
    *   Update `docs/ffb_effects.md` describing the new "Scrub Drag" and "Universal Bottoming".
    *   Update `docs/dev_docs/telemetry_data_reference.md` marking the new fields (`mSuspForce`, `mRideHeight`, etc.) as "Used".
    *   Update `CHANGELOG.md` (v0.4.5).

**Constraints:**
*   **Thread Safety:** Ensure `Config` changes from GUI do not race with `FFBEngine` reads (use the existing mutex).
*   **Performance:** Do not allocate memory inside `calculate_force`.
*   **Units:** Pay strict attention to `mStaticUndeflectedRadius` being in **centimeters** (integer). Cast to double before division.

===


**Context:**
(...)

**Implementation Requirements:**
(...)

**Deliverables:**
(...)

**Constraints:**
(...)

===

Please initialize this session by following the **Standard Task Workflow** defined in `AGENTS.md`.

1.  **Sync**: Run `git fetch && git reset --hard origin/main` (or pull) to ensure you see the latest files.
2.  **Load Memory**: Read `AGENTS_MEMORY.md` to review build workarounds and architectural insights.
3.  **Load Rules**: Read `AGENTS.md` to confirm constraints.

Perform the following task:

Read document docs\dev_docs\FFB App Issues And Debugging v0.4.2.md and implement the requested fixes for the described issues.

**Task: (...)**

**Context:**
(...)

**Implementation Requirements:**
(...)

**Deliverables:**
(...)

**Constraints:**
(...)

