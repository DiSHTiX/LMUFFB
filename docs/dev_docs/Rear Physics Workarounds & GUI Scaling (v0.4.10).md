# Technical Specification: Rear Physics Workarounds & GUI Scaling (v0.4.10)

**Target Version:** v0.4.10
**Date:** December 13, 2025
**Priority:** Critical (Fixes broken effects and invisible graphs)

## 1. Problem Statement

Analysis of version 0.4.9 reveals three critical issues that render specific FFB effects non-functional and debugging tools useless:

1.  **Dead Rear Effects (API Failure):** The Le Mans Ultimate (LMU) 1.2 Shared Memory interface reports `0.0` for `mLateralForce` on rear tires (similar to the known Tire Load bug). Consequently, the **Rear Aligning Torque** effect and **Oversteer Boost** logic—which depend on this force—calculate zero output.
2.  **Invisible Data (Scaling Mismatch):** The FFB Engine was recently updated to output Torque (Newton-meters, range ~0-20 Nm). However, the GUI plots in `GuiLayer.cpp` are still scaled for Force (Newtons, range ±1000). This causes active signals (e.g., 3.0 Nm) to appear as flat lines on the graph.
3.  **Usability (Defaults):** The default `SoP Scale` (5.0) is too weak for the new Nm-based math, and the GUI slider prevents setting it correctly (incorrect min/max).

## 2. Implementation Guide

### Component A: Physics Engine (`FFBEngine.h`)

We must implement a "Calculated Physics" workaround for the rear axle, similar to what was done for the front axle in v0.4.5.

#### 1. Helper: Approximate Rear Load
Add a helper function to estimate vertical load on rear tires using suspension force.

```cpp
// In FFBEngine class
double approximate_rear_load(const TelemWheelV01& w) {
    // Base: Suspension Force + Est. Unsprung Mass (300N)
    // This captures weight transfer (braking/accel) and aero downforce implicitly via suspension compression
    return w.mSuspForce + 300.0;
}
```

#### 2. Calculation: Rear Lateral Force
In `calculate_force`, derive the lateral force since the game returns 0.

**Formula:** $F_{lat} = \alpha \times F_z \times K$
*   $\alpha$: Rear Slip Angle (Raw). Use `m_grip_diag.rear_slip_angle` (calculated in v0.4.7).
*   $F_z$: Calculated Rear Load.
*   $K$: Stiffness Constant (Use **15.0**).

**Implementation Logic:**
```cpp
// Inside calculate_force, after calculating rear grip/slip angles:

// 1. Calculate Rear Loads
double calc_load_rl = approximate_rear_load(data->mWheel[2]);
double calc_load_rr = approximate_rear_load(data->mWheel[3]);
double avg_rear_load = (calc_load_rl + calc_load_rr) / 2.0;

// 2. Calculate Rear Lateral Force (Workaround for missing mLateralForce)
// Use the raw slip angle we calculated earlier in the grip logic
double rear_slip_angle = m_grip_diag.rear_slip_angle; 
double calc_rear_lat_force = rear_slip_angle * avg_rear_load * 15.0;

// 3. Safety Clamp (Prevent explosions if slip angle spikes)
calc_rear_lat_force = (std::max)(-6000.0, (std::min)(6000.0, calc_rear_lat_force));

// 4. Apply to Rear Torque Logic (Replace data->mWheel...mLateralForce)
// Old: double rear_lat_force = (data->mWheel[2].mLateralForce + ...
// New:
double rear_torque = calc_rear_lat_force * 0.00025 * m_oversteer_boost; 
sop_total += rear_torque;
```

#### 3. Update Snapshot
Update the `FFBSnapshot` struct and population logic to include the new calculated data.

```cpp
// In FFBSnapshot struct
float calc_rear_load; // Add this

// In calculate_force snapshot population
snap.ffb_rear_torque = (float)rear_torque; // Ensure this uses the NEW calculated value
snap.calc_rear_load = (float)avg_rear_load;
```

---

### Component B: GUI Layer (`GuiLayer.cpp`)

#### 1. Fix Plot Scaling (CRITICAL)
The `ImGui::PlotLines` function takes `scale_min` and `scale_max` arguments. These must be updated for **ALL** FFB component plots to match the Newton-meter scale.

*   **Target Scale:** **-20.0f** to **+20.0f** (or -30/30 for Base Torque).
*   **Affected Plots:**
    *   `Base Torque`
    *   `SoP (Base Chassis G)`
    *   `Oversteer Boost`
    *   `Rear Align Torque`
    *   `Scrub Drag Force`
    *   `Understeer Cut`
    *   `Road Texture`
    *   `Slide Texture`
    *   `Lockup Vib` / `Spin Vib` / `Bottoming`
*   **Exception:** `Total Output` must remain **-1.0 to 1.0**.
*   **Exception:** `Clipping` must remain **0.0 to 1.1**.

#### 2. Fix SoP Slider
The current slider forces a minimum of 100.0, which is too high.
*   **Current:** `ImGui::SliderFloat("SoP Scale", ..., 100.0f, 5000.0f, ...)`
*   **New:** `ImGui::SliderFloat("SoP Scale", &engine.m_sop_scale, 0.0f, 200.0f, "%.1f");`

#### 3. Update Graphs (Multi-line & Data Sources)

**Header B: Internal Physics**
Change "Calc Front Load" to "Calc Load (Front/Rear)" and plot both lines on the same graph.

```cpp
// Example Multi-line Plot Logic
ImGui::Text("Calc Load (Front/Rear)");
// 1. Draw Front (Cyan)
ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
ImGui::PlotLines("##CLoadF", plot_calc_front_load.data.data(), ..., 0.0f, 10000.0f, ...);
ImGui::PopStyleColor();

// 2. Reset Cursor to draw on top
ImVec2 pos = ImGui::GetItemRectMin();
ImGui::SetCursorScreenPos(pos);

// 3. Draw Rear (Magenta) - Transparent Background
ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0,0,0,0)); 
ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.0f, 0.0f, 1.0f, 1.0f));
ImGui::PlotLines("##CLoadR", plot_calc_rear_load.data.data(), ..., 0.0f, 10000.0f, ...);
ImGui::PopStyleColor(2);

if (ImGui::IsItemHovered()) ImGui::SetTooltip("Cyan: Front, Magenta: Rear");
```

**Header C: Raw Telemetry**
*   Rename `Raw Rear Lat Force` to `Calc Rear Lat Force`.
*   Feed it with the new `calc_rear_lat_force` (via snapshot) instead of the dead game data.

---

### Component C: Configuration (`Config.cpp`)

Update the default values to align with the new Nm scaling.

*   **SoP Scale:** Change default from `5.0f` to **`20.0f`**.
    *   *Rationale:* 1G Lateral $\times$ 0.15 Gain $\times$ 20 Scale = 3.0 Nm. This is a perceptible force on most wheels.

---

### Component D: Testing (`tests/test_ffb_engine.cpp`)

Add a specific test case to verify the Rear Force Workaround.

**Test Logic:**
1.  Create a `TelemInfoV01` struct.
2.  Set `mLateralForce` to **0.0** (Simulate broken game API).
3.  Set `mSuspForce` to **3000.0** (Simulate load).
4.  Set `mGripFract` (Rear) to **0.5** (Simulate sliding/grip loss).
5.  Set `mLocalAccel.x` to **9.81** (1G).
6.  **Assert:** The calculated `ffb_rear_torque` in the snapshot must be **> 0.0**.
    *   *Why:* If the workaround works, the engine calculates force from the slip angle (derived from grip/slide) and load, ignoring the 0.0 input.

## Summary of Changes

| File | Change |
| :--- | :--- |
| `FFBEngine.h` | Add `approximate_rear_load`, implement `calc_rear_lat_force`, update `FFBSnapshot`. |
| `GuiLayer.cpp` | Fix plot scales (±20 Nm), fix SoP slider range, implement multi-line Load plot. |
| `Config.cpp` | Update default `sop_scale` to 20.0. |
| `tests/test_ffb_engine.cpp` | Add test for Rear Force Workaround. |