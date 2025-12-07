Based on the user's report and the provided codebase, here is the analysis of the problem and the troubleshooting steps to assist the user.

### Analysis of the Issue

The user is facing two distinct problems:
1.  **"I have no steering in game now":** The user expects the app to pass their steering input to the game, but it is not happening.
2.  **"The wheel is now randomly and intermittently spinning about":** The wheel is moving on its own, likely due to a signal conflict.

#### Root Cause 1: LMUFFB is not an Input Bridge (Missing "Feeder")
The user states: *"It's as if LMUFFB isn't returning the steering angle to vJoy."*

Looking at `main.cpp` and `DirectInputFFB.cpp`, the application **does not read the steering axis** of the physical wheel. It only calculates Force Feedback.
*   **The Code:** The `FFBThread` calculates force and sends it to the wheel. It does *not* poll the wheel's position to update vJoy.
*   **The Docs:** `README.md` (Installation Step 4A) explicitly states: *"You MUST use a tool like **vJoy Feeder**, **Joystick Gremlin**, or **Mojo** to map your physical wheel axis to the vJoy axis. LMUFFB does not yet bridge steering input."*

**Conclusion:** The user stopped the "vJoy Feeder" app after testing it, or assumes LMUFFB replaces it. They need a separate app running in the background to actually steer the car.

#### Root Cause 2: The "Spinning" is caused by an Axis Conflict (Critical Bug/Config Trap)
The user states: *"The wheel is now randomly and intermittently spinning about... turning the wheel doesn't show anything in vJoy monitor."*

There is a conflict between the Code and the Documentation/User Setup:
1.  **The Code (`main.cpp` line 81):**
    ```cpp
    // Update vJoy Axis (for monitoring) if active
    if (vJoyActive) {
        long axis_val = (long)((force + 1.0) * 0.5 * (axis_max - axis_min) + axis_min);
        DynamicVJoy::Get().SetAxis(axis_val, VJOY_DEVICE_ID, 0x30); // 0x30 = HID_USAGE_X
    }
    ```
    The app writes the **Calculated Force** to **vJoy Axis X** at 400Hz. This is intended for debugging/monitoring FFB visually.

2.  **The User Setup:**
    The user has likely bound **Game Steering** to **vJoy Axis X** (as per the README instructions).

3.  **The Result:**
    *   The App calculates FFB Force.
    *   The App writes this Force to vJoy Axis X.
    *   The Game reads vJoy Axis X as **Steering Input**.
    *   **Feedback Loop:** The FFB Force is literally steering the car. If the FFB pushes left, the car steers left, which changes the physics, which changes the FFB. This causes the "random spinning" and "jerking."

---

### Troubleshooting Response for the User

Here is the suggested response to guide the user through fixing both issues.

***

**Subject:** Re: No steering and wheel spinning issues

Hi, thanks for the detailed follow-up. Moving the EXE to the vJoy folder was a good move to fix the DLL issue.

I have analyzed your report against the code, and I believe I know exactly what is happening. There are two separate things we need to fix.

### 1. Fix "No Steering" (The Bridge)
**The Issue:** LMUFFB calculates Force Feedback, but it **does not** read your steering angle. It cannot "bridge" your Moza steering to vJoy on its own.
**The Fix:** You must keep a "Feeder" application running in the background while you play.
1.  Open **vJoy Feeder** (or Joystick Gremlin).
2.  Select your Moza R9.
3.  Ensure that turning your Moza moves the vJoy Axis bars.
4.  **Leave this app open** while playing the game.

### 2. Fix "Wheel Spinning" (The Axis Conflict)
**The Issue:** This is a conflict between the App and your Game bindings.
Currently, LMUFFB outputs the **FFB Force** signal to **vJoy Axis X** (so you can see the force visually in vJoy Monitor).
However, if you bind the **Game Steering** to **vJoy Axis X**, the App is effectively "steering the car" with the Force Feedback signal. This creates a feedback loop where the wheel spins and jerks on its own.

**The Fix:** You need to use a different axis for steering to avoid the FFB signal.

**Please follow these exact steps:**

1.  **Configure vJoy:**
    *   Open **Configure vJoy**.
    *   Ensure **Device 1** has at least **X** and **Y** axes enabled.
2.  **Configure the Feeder (e.g., vJoy Feeder / Gremlin):**
    *   Map your **Moza Steering** to **vJoy Axis Y** (instead of X).
3.  **Configure the Game:**
    *   Go to LMU Controls.
    *   Bind **Steering** to **vJoy Axis Y**.
4.  **Run LMUFFB:**
    *   Now, LMUFFB will write FFB info to Axis X (which the game ignores).
    *   Your Feeder will write Steering to Axis Y (which the game uses).

**Summary Checklist:**
*   [ ] **Feeder App** is running in the background.
*   [ ] **Feeder** maps Moza Wheel -> vJoy **Axis Y**.
*   [ ] **Game** Steering is bound to vJoy **Axis Y**.
*   [ ] **LMUFFB** FFB Device is set to **Moza R9**.

Let me know if this stabilizes the wheel!

***

### Recommendation for Developers (Code Fix)

The current behavior in `main.cpp` is dangerous because it defaults to overwriting the most common steering axis (X) with FFB data.

**Recommended Changes for v0.3.9:**
1.  **Disable vJoy Output by Default:** In `Config.h` / `Config.cpp`, add a boolean `m_output_ffb_to_vjoy` defaulting to `false`.
2.  **Modify `main.cpp`:**
    ```cpp
    // Only write to vJoy if explicitly enabled for debugging
    if (vJoyActive && Config::m_output_ffb_to_vjoy) {
        long axis_val = ...;
        DynamicVJoy::Get().SetAxis(axis_val, VJOY_DEVICE_ID, 0x30);
    }
    ```
3.  **Update Documentation:** Clearly state that vJoy Axis X is used for FFB monitoring *only* if enabled, and users should use Axis X for steering (if the feature is disabled) or Axis Y (if enabled).