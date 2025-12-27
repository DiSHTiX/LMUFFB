# Report: UI/UX Overhaul & Presets

## 1. Introduction and Context
The "Troubleshooting 25" list identifies that the application has become complex ("lmuFFB has now so many advanced options. This might be confusing for users"). A simplified interface is requested. Additionally, the preset system is outdated (test presets need removal, real hardware presets need addition), and the graphs consume too much screen real estate.

**Problems Identified:**
*   **Complexity Overload**: New users see 50+ sliders and don't know where to start.
*   **Outdated Presets**: "Test Preset" / "Guide Preset" are no longer relevant. Users need "Moza", "Simucube", "T300".
*   **Graph Clutter**: The debug graphs are useful but large. They should be reorganized or made more compact.
*   **Workflow Friction**: Users have to manually click "Save". An auto-save (on exit or periodic) is requested.

## 2. Proposed Solution

### 2.1. Basic Mode (Simplified UI)
*   **Toggle**: Add a global boolean `m_basic_mode` (Default: `true` for new installs).
*   **View**: When enabled, the GUI hides all "Advanced" groups (Signal Filtering, Frequencies, Latency sliders, etc.).
*   **Exposed Controls**: Only show the "Big 5":
    1.  Master Gain
    2.  SoP Strength (Lateral G + Rear Align combined or just main sliders)
    3.  Smoothing "Feel" (One slider controlling both SoP and Slip smoothing)
    4.  Understeer Strength
    5.  Curb/Road Texture Strength

### 2.2. Preset Overhaul
*   **Remove**: Delete "Test", "Guide".
*   **Add**:
    *   **"Entry Level / Gear / Belt"** (Logitech/Thrustmaster): High Min Force, High Smoothing, Moderate Texture.
    *   **"Direct Drive (Linear)"**: Zero Min Force, Low Smoothing, High Fidelity.
    *   **"Direct Drive (High Torque)"**: Max Torque Ref = 20Nm, Linear settings.

### 2.3. Auto-Save
*   **Implementation**: Call `Config::Save(engine)` in the `Shutdown()` method of the app. Add a "Config Saved" toast notification or log message when specific major changes occur.

### 2.4. Graph Compactness
*   **Action**: Reduce the vertical height of `ImGui::PlotLines`. Combine "Input Steer" and "Output Torque" into a single multi-line plot if ImGui allows, or position them side-by-side.

## 3. Implementation Plan

### 3.1. `src/GuiLayer.cpp`
1.  **Basic Mode Toggle**:
    ```cpp
    if (ImGui::Checkbox("Basic Mode", &Config::m_basic_mode)) { ... }
    ```
2.  **Conditional Rendering**:
    ```cpp
    if (!Config::m_basic_mode) {
        // Render Advanced TreeNodes
        if (ImGui::TreeNode("Signal Filtering")) { ... }
    }
    ```
3.  **Graph Tweak**: Change `ImVec2(0, 80)` size to `ImVec2(0, 40)`.

### 3.2. `src/Config.cpp`
1.  **Update `LoadPresets`**: Hardcode the new values for the proposed Hardware presets.

## 4. Testing Plan

### 4.1. UX Walkthrough
*   **Setup**: Launch app fresh.
*   **Verification**: "Basic Mode" should be On. Only key sliders visible.
*   **Action**: Toggle "Basic Mode" Off.
*   **Verification**: All advanced options appear.

### 4.2. Auto-Save
*   **Action**: Change "Master Gain" to 142%. Close App (X button). Re-open App.
*   **Verification**: Master Gain is 142%.

### 4.3. Preset Logic
*   **Action**: Select "Logitech G29".
*   **Verification**: `m_min_force` sets to ~0.10. `m_steering_shaft_smoothing` sets to higher value (e.g. 0.05).
*   **Action**: Select "Direct Drive".
*   **Verification**: `m_min_force` sets to 0.0. Smoothing reduces to near 0.
