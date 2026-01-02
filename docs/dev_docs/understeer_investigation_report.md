# Understeer Investigation Report (Final)

## Executive Summary
The "Understeer Effect" issue is confirmed to be a systemic problem caused by the interaction between the application's **Grip Estimation Logic** (which acts as the default physics engine for LMU due to missing API data) and the **T300 Preset Configuration**.

Specifically, the T300 preset defines the "Optimal Slip Angle" at an unrealistically low value (**0.06 rad / 3.4°**). Because modern race cars (LMP2/GT3) naturally operate effectively at higher slip angles (5-7°), the estimator calculates that the car is in a state of drastic grip loss (50% or less) during normal cornering. This perceived "loss," multiplied by the Understeer Effect slider, causes the FFB to collapse or disappear entirely.

## Detailed Technical Analysis

### 1. The "Fallback" is the Standard
User feedback confirms that `mGripFract` is consistently `0.0` for LMU cars. Therefore, the function `calculate_grip` in `FFBEngine.h` **always** executes the fallback estimation logic. This is not an edge case; it is the primary operating mode of the application.

### 2. The Tuning Discrepancy (The Root Cause)
The Grip Estimator uses a "Friction Circle" model defined by `m_optimal_slip_angle`.
*   **Math:** `Lateral_Metric = Current_Slip_Angle / Optimal_Slip_Angle`.
*   **Logic:** If `Metric > 1.0`, the system assumes traction is breaking and penalizes Grip.

**The Config Bug:**
*   The **T300 Preset** sets `optimal_slip_angle` to **0.06 radians (3.4 degrees)**.
*   **Reality:** LMP2 and GT3 slicks typically produce peak cornering force at **5.0 to 7.0 degrees** (0.09 - 0.12 rad).

**The Consequence:**
When a user corners an LMP2 car hard (e.g., at 5.0 degrees slip):
1.  `Metric` = 0.087 rad / 0.06 rad = **1.45**.
2.  The Friction Circle math sees this as **45% past the limit**.
3.  The Penalty Curve (`1.0 / (1.0 + Excess * 2.0)`) kicks in:
    *   `Value` = 1.0 / (1.0 + (0.45 * 2.0)) = 1.0 / 1.9 = **0.52**.
4.  **Result:** The engine calculates **52% Grip** for a car that is actually cornering perfectly fine.

### 3. The Understeer Multiplier (The "Kill Switch")
The Understeer Effect formula is:
```cpp
Grip_Loss = (1.0 - Estimated_Grip) * Effect_Setting
Final_Force = Base_Force * (1.0 - Grip_Loss)
```

**Scenario: User Settings "0.84" (User 1)**
*   Estimated Grip: **0.52** (due to the tuning error above).
*   Grip Loss: (1.0 - 0.52) = **0.48**.
*   Multiplier: 0.48 * 0.84 = **0.40**.
*   Final Force: 60% of original.
*   *Result:* The wheel feels "too light" during normal cornering.

**Scenario: User Settings "2.0" (User 2)**
*   Estimated Grip: **0.52**.
*   Grip Loss: 0.48 * 2.0 = **0.96**.
*   Final Force: 4% of original (Effectively Zero).
*   *Result:* Total loss of FFB.

### 4. "Double Dipping" on Physics
The `mSteeringShaftTorque` from the game engine usually typically includes the physical drop of Self-Aligning Torque (SAT) as the tires pass their peak.
*   **Game Physics:** Torque drops naturally at the limit.
*   **App Logic:** We calculate a "Grip Factor" and force the torque to drop *again*.
*   By using a `0.06` optimum in the app, we force the software drop to happen *before* the physics drop (3.4° vs ~6°), creating a strange "hole" in the FFB in the middle of the corner.

## Recommendations for Remediation

### 1. Update Preset Defaults (Critical)
The default `optimal_slip_angle` in the T300 preset (and typically others) must be increased to match realistic tire physics.
*   **Change:** `0.06` -> **0.10** or **0.12** (approx 6-7 degrees).
*   This ensures "100% Grip" is maintained up to the actual limit of adhesion, preventing premature FFB cutouts.

### 2. Refine the Drop-Off Curve
The current curve `1 / (1 + 2x)` is extremely aggressive. A softer roll-off would prevent the "on/off" feel reported by User 2.
*   **Proposal:** Change to `1 / (1 + x)`.

### 3. Cap the UI Range
As previously noted, a slider that goes to 200.0 is dangerous.
*   **Action:** Limit valid input to range **0.0 - 2.0**.

### 4. Implement Thresholding
Prevent the Understeer Effect from acting until grip drops significantly (e.g., below 0.9), to act as a "save mechanism" rather than a constant filter.

## Conclusion
The users are experiencing correctly calculated math based on incorrect physical constants. The software believes the car is constantly sliding because it has been told that "3.4 degrees" is the maximum valid slip angle. Fixing the `optimal_slip_angle` default is the primary solution.
