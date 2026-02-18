Based on the analysis of the codebase and the provided log, here are the findings regarding the telemetry frequency issue.

### 1. Code Review of Frequency Estimation
The frequency estimation logic is located in **`src/main.cpp`** inside the `FFBThread()` function. It uses the `RateMonitor` class (defined in `src/RateMonitor.h`) to count how many times a value changes per second.

**Current Status:**
The log confirms that the standard telemetry channels (`mSteeringShaftTorque`, `mLocalAccel`, `mLocalVel`, etc.) are updating at **100 Hz**. This is the standard physics tick rate for the legacy rFactor 2 shared memory plugin, which LMU inherits.

### 2. Are we logging the frequency of all used channels?
**No.** You are missing several critical channels that are heavily used by your `FFBEngine` for advanced effects. If these are also running at 100 Hz, it explains why effects like "Road Texture" or "Slope Detection" might feel aliased or "notchy."

**Missing Channels (Should be added to logging):**
You should add `RateMonitor` instances for the following variables in `main.cpp`:

1.  **`mVerticalTireDeflection`** (in `mWheel[i]`):
    *   *Usage:* Used in `FFBEngine::calculate_road_texture`.
    *   *Importance:* This drives the road texture/rumble. If this is 100 Hz, road texture will feel like a low-frequency buzz rather than detailed road noise.
2.  **`mLateralPatchVel`** (in `mWheel[i]`):
    *   *Usage:* Used in `FFBEngine::calculate_slope_grip` and `calculate_slip_angle`.
    *   *Importance:* This is the primary input for your Slope Detection algorithm. 100 Hz is very low for calculating derivatives ($d/dt$), likely causing the noise/instability you are trying to filter out.
3.  **`mRotation`** (in `mWheel[i]`):
    *   *Usage:* Used in `FFBEngine::calculate_lockup_vibration` and `calculate_wheel_spin`.
    *   *Importance:* Used to detect rapid changes in wheel speed for lockups.
4.  **`mSuspForce`** (in `mWheel[i]`):
    *   *Usage:* Used for load approximation and bottoming detection.
5.  **`mGripFract`** (in `mWheel[i]`):
    *   *Usage:* Used for the "native" grip calculation path.

### 3. Are we logging channels we don't use?
**Yes.** The following channels are being monitored in `main.cpp` but appear to have no impact on the FFB calculation in `FFBEngine.cpp`:

1.  **`mPos` (X, Y, Z)**:
    *   *Log:* `Pos: X=100.0, Y=100.0, Z=100.4`
    *   *Usage:* The FFB engine uses velocities and accelerations, not absolute world coordinates.
2.  **`mEngineRPM`**:
    *   *Log:* `RPM=100.0`
    *   *Usage:* While passed to the monitor, `FFBEngine.cpp` does not seem to use RPM for any vibration generation (engine vibration is not implemented).

### 4. Channels that might be at 400 Hz (The "Direct Torque" Candidate)
Based on the files in `src/lmu_sm_interface`, specifically **`SharedMemoryInterface.hpp`**, there is a specific candidate for 400 Hz data.

In `SharedMemoryInterface.hpp`:
```cpp
struct SharedMemoryGeneric {
    SharedMemoryEvent events[SharedMemoryEvent::SME_MAX];
    long gameVersion;
    float FFBTorque; // <--- THIS IS THE CANDIDATE
    ApplicationStateV01 appInfo;
};
```

In `main.cpp`, you are already monitoring this:
```cpp
if (g_localData.generic.FFBTorque != lastGenTorque) {
    genTorqueMonitor.RecordEvent();
    lastGenTorque = g_localData.generic.FFBTorque;
}
```

**The Problem in your Log:**
```
[18:48:06] Torque: Shaft=100.0, Generic=0.0
```
The `Generic=0.0` indicates that `g_localData.generic.FFBTorque` is **not changing**.

*   **Possibility A:** The game is not writing to this field (requires a specific game setting, e.g., "Write Shared Memory" option in LMU might need to be set to a specific mode).
*   **Possibility B:** The app is not reading the `SharedMemoryGeneric` struct correctly or frequently enough, though the loop rate is 400Hz.
*   **Possibility C:** The car is stationary in the log, so torque is constant 0. However, `Shaft=100.0` implies the steering shaft torque *is* fluctuating (likely due to noise or FFB holding center), so `FFBTorque` should likely also be fluctuating if it were active.

### 5. Information in `src/lmu_sm_interface`
I reviewed the files in `src/lmu_sm_interface`.

*   **`InternalsPlugin.hpp`**: This defines the standard `TelemInfoV01` structure. This structure is historically tied to the rFactor 2 plugin system, which typically updates at **100 Hz** (or sometimes 50Hz depending on the plugin version). The comments mention `UpdateScoring` happens ~5 times/sec, but does not explicitly guarantee 400Hz for `UpdateTelemetry`.
*   **`SharedMemoryInterface.hpp`**: This defines the memory layout used by LMU. The presence of `float FFBTorque` in the `SharedMemoryGeneric` struct (separate from the `TelemInfoV01` array) strongly suggests this is the special "high frequency" channel added for LMU to bypass the 100Hz limit of the legacy struct.

### Summary & Recommendation

1.  **The Issue:** The game is indeed sending standard telemetry (`TelemInfoV01`) at 100 Hz. This limits the quality of your derivative-based effects (Slope Detection).
2.  **The Solution:** You must utilize `g_localData.generic.FFBTorque`.
3.  **Action Items:**
    *   **Investigate why `Generic` torque is 0.0 Hz.** Ensure the game is configured to output this data. If `Shaft` torque is moving, `Generic` torque should be too.
    *   **Add Logging:** Add `RateMonitor` for `mWheel[0].mVerticalTireDeflection` and `mWheel[0].mLateralPatchVel` to confirm if *all* physics data is 100 Hz.
    *   **Code Change:** If you can get `FFBTorque` to update at 400 Hz, ensure `Config::m_torque_source` is set to `1` (Direct Torque) in your preset to use it. Currently, your log implies it is dead.