# Report: Robustness & Game Integration

## 1. Introduction and Context
This report addresses stability issues and integration bugs. Specifically, the "Troubleshooting 25" list mentions that FFB forces (especially "holding" forces like Slide or Self-Aligning Torque) can get "stuck" when the user exits the game or pauses the session. This requires a robust way to detect "Not Driving" states.
Additionally, we need to verify the fix for the LMU 1.2 "Zero Lateral Force" bug and ensure the new logging mechanisms (Timestamps) are effective for debugging.

**Problems Identified:**
*   **Stuck Forces**: Game exit or session switch leaves residual torque on the wheel.
*   **LMU 1.2 Bug**: Rear wheels report 0.0 lateral force, requiring a workaround. We need to confirm this workaround is active and transparent to the user.
*   **Debuggability**: Console logs lack timestamps, making it hard to correlate events.
*   **Emergency Stop**: Users need a manual way to cut forces if automatics fail.

## 2. Proposed Solution

### 2.1. "Stuck Force" Prevention
*   **Timeout Logic**: Implement a "Deadman Switch" in `FFBEngine`. If the `mElapsedTime` from telemetry hasn't changed for > 1.0 second, or if `mDeltaTime` is zero for consecutive frames, dampen all forces to 0.0 over a short period (0.5s fade out).
*   **State Reset**: Create a `Reset()` method in `FFBEngine` that zeroes all internal integrators, smoothers, and phases (`m_slide_phase`, `m_sop_lat_g_smoothed`, etc.). Call this method automatically when `GameConnector` detects a disconnection.

### 2.2. LMU 1.2 Workaround Verification
*   **Console Alerts**: We already implemented a warning "Warning: Data for mLateralForce (Rear) ...". We will ensure this logic includes a "Cooldown" so it doesn't spam the console every frame, but repeats every ~60 seconds if the info is still missing.
*   **Debug Value**: Expose the "Calculated Rear Lateral Force" vs "Raw Rear Lateral Force" in the `FFBSnapshot` so it can be viewed in the graphs.

### 2.3. Logging Improvements
*   **Timestamp Helper**: Create a standard logging function `Log(const char* msg)` that prepends `[HH:MM:SS.ms]`. Replace all `std::cout` calls with this.

### 2.4. Emergency Controls
*   **Reset Button**: Add a red "DISCONNECT / RESET" button in the top bar of the GUI. This creates a "Panic Switch" for the user.

## 3. Implementation Plan

### 3.1. `src/FFBEngine.h`
1.  **Add `Reset()` method**:
    ```cpp
    void Reset() {
        m_sop_lat_g_smoothed = 0.0;
        m_yaw_accel_smoothed = 0.0;
        m_steering_shaft_torque_smoothed = 0.0;
        // ... set all _prev_ values to 0 ...
    }
    ```
2.  **Add Timeout in `calculate_force`**:
    ```cpp
    static double last_game_time = 0.0;
    static double wall_clock_timeout = 0.0;
    if (data->mElapsedTime == last_game_time) {
         wall_clock_timeout += dt_wall; 
         if (wall_clock_timeout > 1.0) return 0.0f; // Fade out
    } else {
         wall_clock_timeout = 0.0;
         last_game_time = data->mElapsedTime;
    }
    ```

### 3.2. `src/GuiLayer.cpp`
1.  **Add Panic Button**:
    ```cpp
    ImGui::SameLine();
    if (ImGui::Button("RESET FFB")) {
        engine.Reset();
        DirectInputFFB::Get().SetForce(0);
    }
    ```

## 4. Testing Plan

### 4.1. Disconnection Test
*   **Setup**: Start driving in LMU.
*   **Action**: Alt-Tab and kill the LMU process (Task Manager) or simply click "Exit to Monitor".
*   **Verification**: The FFB on the wheel should drop to 0 within 1 second. No residual "pulling" force.

### 4.2. Panic Button
*   **Setup**: Induce a high-force situation (e.g., turn wheel to lock).
*   **Action**: Click "RESET FFB".
*   **Verification**: Wheel should instantly go limp.

### 4.3. Logs
*   **Verification**: Check console output. It should look like:
    `[22:15:01.123] Connected to LMU`
    `[22:15:05.444] Warning: mLateralForce missing...`
