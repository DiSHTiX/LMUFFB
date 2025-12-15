You will have to work on the files downloaded from this repo https://github.com/coasting-nc/LMUFFB and start working on the tasks described below. Therefore, if you haven't done it already, clone this repo https://github.com/coasting-nc/LMUFFB and start working on the tasks described below.

Please initialize this session by following the **Standard Task Workflow** defined in `AGENTS.md`.

1.  **Sync**: Run `git fetch && git reset --hard origin/main` for the LMUFFB repository to ensure you see the latest files.
1.  **Load Memory**: Read `AGENTS_MEMORY.md` from the root dir of the LMUFFB repository to review build workarounds and architectural insights. 
2.  **Load Rules**: Read `AGENTS.md` from the root dir of the LMUFFB repository to confirm instructions. 

Once you have reviewed these documents, please proceed with the following task:

**Task: Implement "SoP Injector" (Yaw Acceleration Cue)**

**Reference Documents:**
*   `docs/dev_docs/Yaw, Gyroscopic Damping , Dynamic Weight, Per-Wheel Hydro-Grain, and Adaptive Optimal Slip Angle implementation.md` (Sections 1 and 5).
*   `docs/dev_docs/FFB_formulas.md` (For documentation updates).

**Context:**
We are enhancing the "Seat of Pants" (SoP) effect to be more "Informative" and "Visceral". Currently, SoP relies on Lateral G-Force (`mLocalAccel.x`), which represents weight transfer. However, this is reactive. To provide a predictive "Kick" when the rear end breaks traction (especially on low-grip surfaces where G-force might drop), we need to inject **Yaw Acceleration** (`mLocalRotAccel.y`) into the signal.

**Implementation Requirements:**

1.  **Update `FFBEngine.h`**:
    *   Add a new configuration variable: `float m_sop_yaw_gain = 0.0f;` (Default to 0.0 so it is an opt-in feature).
    *   In `calculate_force`:
        *   Read `data->mLocalRotAccel.y` (Yaw Acceleration).
        *   Calculate the force: `double yaw_force = data->mLocalRotAccel.y * m_sop_yaw_gain * 5.0;` (Use 5.0 as the base scaling factor as per the reference doc).
        *   Add `yaw_force` to `sop_total`.
    *   Update `FFBSnapshot` struct to include `float ffb_yaw_kick` and populate it in the snapshot logic.

2.  **Update Configuration (`src/Config.h` & `src/Config.cpp`)**:
    *   Update `Preset` struct to include `float sop_yaw_gain`.
    *   Add a fluent setter: `Preset& SetSoPYaw(float v)`.
    *   Update `Config::Save` and `Config::Load` to persist `sop_yaw_gain` to `config.ini`.
    *   Update `Config::LoadPresets` to initialize this value (default 0.0f).

3.  **Update GUI (`src/GuiLayer.cpp`)**:
    *   **Tuning Window**: Add a slider for "SoP Yaw (Kick)" in the "Effects" section (Range 0.0 to 2.0). Add a tooltip explaining it provides a cue for rotation onset.
    *   **Debug Window**: Add a new trace line for "Yaw Kick" in the "FFB Components (Output)" graph (Header A) so users can visualize this specific force contribution.

4.  **Update Documentation**:
    *   Update `docs/dev_docs/FFB_formulas.md`: Add the Yaw Acceleration component to the "Seat of Pants (SoP) & Oversteer" section formulas.

5.  **Add Unit Test (`tests/test_ffb_engine.cpp`)**:
    *   Create a test `test_sop_yaw_kick()`.
    *   Scenario: Zero Lateral G, Zero Steering Force, but Non-Zero Yaw Acceleration (e.g., `mLocalRotAccel.y = 5.0`).
    *   Assert that the calculated force is non-zero and matches the expected formula.

**Deliverables:**
1.  Modified `FFBEngine.h`.
2.  Modified `src/Config.h` and `src/Config.cpp`.
3.  Modified `src/GuiLayer.cpp`.
4.  Updated `docs/dev_docs/FFB_formulas.md`.
5.  Modified `tests/test_ffb_engine.cpp` with the new test case.
6.  **Verification**: Run the tests (`./run_tests` in the build folder) and ensure `test_sop_yaw_kick` passes.