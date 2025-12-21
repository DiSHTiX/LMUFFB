You will have to work on the files downloaded from this repo https://github.com/coasting-nc/LMUFFB and start working on the tasks described below. Therefore, if you haven't done it already, clone this repo https://github.com/coasting-nc/LMUFFB and start working on the tasks described below.

Please initialize this session by following the **Standard Task Workflow** defined in `AGENTS.md`.

1.  **Sync**: Run `git fetch && git reset --hard origin/main` for the LMUFFB repository to ensure you see the latest files.
2.  **Load Memory**: Read `AGENTS_MEMORY.md` from the root dir of the LMUFFB repository to review build workarounds and architectural insights. 
3.  **Load Rules**: Read `AGENTS.md` from the root dir of the LMUFFB repository to confirm instructions. 

Once you have reviewed these documents, please proceed with the following task:

**Task: Move Frequency Diagnostics to Main Tuning Window**

**Context:**
The user finds the new "Signal Analysis" (Frequency Estimator) readouts difficult to see in the bottom-right of the Debug Window. They requested to move these readouts to the **Main Tuning Window**, specifically within the "Signal Filtering" section, so they can see the frequency data while adjusting the Notch Filter settings.

**Implementation Requirements:**

1.  **Update `FFBEngine.h`**:
    *   Add a new public member variable `double m_theoretical_freq = 0.0;` to the `FFBEngine` class (alongside the existing `m_debug_freq`).
    *   In `calculate_force`, update this variable with the calculated wheel frequency (`car_speed / circumference`). This allows the GUI to read it directly under the main mutex lock without needing a snapshot.

2.  **Update `src/GuiLayer.cpp`**:
    *   **Remove** the "Signal Analysis" text block (Est. Vibration Freq / Theoretical Wheel Freq) from `DrawDebugWindow` (inside the "Raw Game Telemetry" section).
    *   **Add** the frequency readouts to `DrawTuningWindow` inside the `if (ImGui::TreeNode("Signal Filtering"))` block.
    *   **Layout:**
        *   Display the readouts **below** the "Notch Width (Q)" slider (or below the checkbox if the filter is disabled).
        *   Use a distinct color (e.g., Cyan or Yellow) to make them stand out.
        *   Format: `Est. Freq: %.1f Hz | Theory: %.1f Hz` (Keep it compact).
        *   Add a tooltip explaining that "Matching values indicate a speed-dependent vibration (flat spot)."

**Deliverables:**
*   Updated `FFBEngine.h`
*   Updated `src/GuiLayer.cpp`

**Check-list for completion:**
- [ ] `m_theoretical_freq` added to `FFBEngine` and updated in `calculate_force`.
- [ ] Frequency readouts removed from `DrawDebugWindow`.
- [ ] Frequency readouts added to `DrawTuningWindow` inside the "Signal Filtering" section.
- [ ] Readouts use `engine.m_debug_freq` and `engine.m_theoretical_freq` directly.