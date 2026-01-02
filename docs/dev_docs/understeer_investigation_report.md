# Understeer Effect Investigation Report

## Executive Summary
Users reported issues with the "Understeer Effect" where the steering wheel becomes too light (User 1) or completely loses Force Feedback (User 2) when the setting is adjusted. Investigation into the `FFBEngine.h` and `Config.cpp` files reveals that the mathematical implementation of the effect combined with an excessively large allowable range (0-200) causes the FFB output to be clamped to zero under common driving conditions.

## User Reports Analysis

### User 1 (Unwanted Lightness)
> "In fact, with the LMP2 it is even too sensitive and makes the wheel too light. I had to set the understeer effect slider to 0.84, and even then it was too strong."

This user is experiencing the effect of the reduction formula applied to "normal" driving conditions. If the game reports any grip value less than perfect (1.0) — for example, 0.90 while cornering — a setting of `0.84` results in a significant force reduction, which the user perceives as an overly "light" wheel.

### User 2 (Total Signal Loss)
> "If I set the understeer effect to anything from 1 to 200 I can't feel anything, no FFB. Only below 1 there is some weight..."

This confirms that values greater than 1.0 are catastrophic for the FFB signal. With a setting of `200`, a microscopic grip drop of just **0.5%** (`0.995` grip) is sufficient to reduce the force by 100% (`0.005 * 200 = 1.0`), resulting in zero torque output.

## Technical Analysis

### The Logic (`FFBEngine.h`)
The understeer effect is calculated in `FFBEngine::calculate_force` (approx. line 1064):

```cpp
// grip_factor: 1.0 = full force, 0.0 = no force (full understeer)
// m_understeer_effect: 0.0 = disabled, 1.0 = full effect
double grip_loss = (1.0 - avg_grip) * m_understeer_effect;
double grip_factor = 1.0 - grip_loss;

// FIX: Clamp to 0.0 to prevent negative force (inversion) if effect > 1.0
grip_factor = (std::max)(0.0, grip_factor);

// ...

// Apply Gain and Grip Modulation
double output_force = (base_input * (double)m_steering_shaft_gain) * grip_factor;
```

**The Flaw:**
The formula linearly scales the "grip loss" (`1.0 - avg_grip`).
*   If `m_understeer_effect` is **2.0** and Grip is **0.5** (heavy slide), `grip_loss` = 0.5 * 2.0 = 1.0. `grip_factor` = 0.0. (Correct behavior for excessive slide).
*   If `m_understeer_effect` is **200.0** and Grip is **0.995** (tiny hesitation/noise), `grip_loss` = 0.005 * 200.0 = 1.0. `grip_factor` = 0.0. (Catastrophic behavior).

Technically, `mGripFract` from the telemetry is rarely a steady 1.0. It fluctuates, especially in rFactor 2 / LMU physics. Any value above `1.0` in the multiplier makes the system extremely volatile.

### The Range (`Config.cpp`)
The configuration allows a maximum value of **200.0**:

```cpp
if (engine.m_understeer_effect < 0.0f || engine.m_understeer_effect > 200.0f) {
    engine.m_understeer_effect = (std::max)(0.0f, (std::min)(200.0f, engine.m_understeer_effect));
}
```

This range appears to be intended for a "Percentage" style input (0-200%), but the code treats it as a raw multiplier. 

## Recommendations

1.  **Correct the Scale**: If the UI is displaying percentages (e.g., "100%"), the code should likely divide by 100 before applying it as a multiplier. Alternatively, cap the raw multiplier to a reasonable physics-based range (e.g., 0.0 to 2.0). 
    *   *Immediate Fix:* Interpret user input `X` as `X / 100.0` or limit the slider to `2.0`.

2.  **Add Thresholding**: Implement a "Threshold" for the understeer effect so that it only kicks in when grip drops below a certain point (e.g., 0.90), preventing noise or minor scrubbing from killing the force.
    ```cpp
    if (avg_grip < 0.9) {
        // apply effect
    }
    ```

3.  **Non-Linear Response**: Instead of a linear reduction which hits zero abruptly, consider a curve that preserves some weight unless grip is totally lost.

4.  **UI Updates**: Update the tooltip or UI label to explain that "1.0" (or 100%) means "1:1 mapping of grip loss to force loss", and higher values will exaggerate the loss.
