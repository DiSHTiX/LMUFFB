# Understeer Investigation Report (Revised)

## Executive Summary

After reviewing the code in `FFBEngine.h` and `FFB_formulas.md`, the "Understeer Effect" issues reported by users are caused by a **misunderstanding of how the system works** combined with **suboptimal default values in the T300 preset**.

**Key Finding**: The `optimal_slip_angle` parameter is **only used in the fallback grip estimator** (when `mGripFract` returns 0.0). Since LMU always returns 0.0 for `mGripFract`, the fallback is *always* active, making this parameter effectively the primary control for understeer sensitivity.

## User Reports

### User 1:
> Findings from more testing: the Understeer Effect seems to be working. In fact, with the LMP2 it is even too sensitive and makes the wheel too light. I had to set the understeer effect slider to 0.84, and even then it was too strong.

### User 2:
> With regards to the understeer effect I have to say that it is not working for me (Fanatec CLS DD). I tried the "test understeer only" preset and if I set the understeer effect to anything from 1 to 200 I can't feel anything, no FFB. Only below 1 there is some weight in the FFB when turning. When I turn more than I should and the front tires lose grip, I expect the wheel to go light, but that is not the case. The wheel stays just as heavy. So I cannot feel the point of the front tires losing grip. I tried GT3 and LMP2, same result.

### Developer Clarification:
- The value `0.84` was out of `200.0` max — an extremely low setting.
- LMU always returns 0.0 for `mGripFract`, so the fallback estimator is always active.

I was under the impression that the optimal slip angle is 0.06 for LMP2/prototypes/hypercars, and 0.10 for GP3. Isn't this the case?

I want the user to dynamically feel the loss of grip, and be able to prevent it, and just approach. I don't want a "reactive" effect, that gives information to the user when it is too late. (it seems you have already removed it).

---

## Code Analysis

### References
- `src/FFBEngine.h` (lines 239, 546-632, 1061-1068)
- `src/Config.h` (lines 67-68)
- `src/Config.cpp` (lines 62-63)
- `docs/dev_docs/FFB_formulas.md`

---

### 1. The Grip Fallback Estimator (FFBEngine.h lines 546-632)

When the game returns `mGripFract ≈ 0.0` (which is *always* the case in LMU), the `calculate_grip()` function uses a **Combined Friction Circle** approximation:

```cpp
// FFBEngine.h line 597
double lat_metric = std::abs(result.slip_angle) / (double)m_optimal_slip_angle;

// FFBEngine.h lines 601-606
double ratio1 = calculate_manual_slip_ratio(w1, car_speed);
double ratio2 = calculate_manual_slip_ratio(w2, car_speed);
double avg_ratio = (std::abs(ratio1) + std::abs(ratio2)) / 2.0;
double long_metric = avg_ratio / (double)m_optimal_slip_ratio;

// FFBEngine.h lines 608-618
double combined_slip = std::sqrt((lat_metric * lat_metric) + (long_metric * long_metric));
if (combined_slip > 1.0) {
    double excess = combined_slip - 1.0;
    result.value = 1.0 / (1.0 + excess * 2.0);  // Sigmoid-like curve
} else {
    result.value = 1.0;  // Full grip
}

// FFBEngine.h line 622 - Safety floor
result.value = std::max(0.2, result.value);
```

**Key Insight**: The `m_optimal_slip_angle` acts as a **normalization threshold**. If set to `0.06 rad`, then driving at `0.06 rad` slip produces `lat_metric = 1.0`, meaning "grip starts dropping". If set to `0.10 rad`, it takes a slip of `0.10 rad` to trigger the same drop.

---

### 2. The Understeer Effect Application (FFBEngine.h lines 1061-1068)

```cpp
// FFBEngine.h lines 1064-1068
double grip_loss = (1.0 - avg_grip) * m_understeer_effect;
double grip_factor = 1.0 - grip_loss;

// FIX: Clamp to 0.0 to prevent negative force (inversion) if effect > 1.0
grip_factor = (std::max)(0.0, grip_factor);
```

The `grip_factor` is then applied to the base force:
```cpp
// FFBEngine.h line 1091
double output_force = (base_input * (double)m_steering_shaft_gain) * grip_factor;
```

**Critical**: The `grip_factor` is clamped to a **minimum of 0.0**, preventing negative (inverted) forces. However, it can reach 0.0, causing total force loss.

---

### 3. Default Configuration Analysis

| Source | `optimal_slip_angle` | `understeer` (Effect Strength) |
| --- | --- | --- |
| **Default Preset** (Config.h) | **0.10** rad | 50.0 |
| **T300 Preset** (Config.cpp line 62) | **0.06** rad | 0.5 |
| FFB_formulas.md (Reference) | 0.10 rad | N/A |

**Issue**: The hardcoded T300 preset uses `0.06 rad`, which is the physical peak slip angle for prototypes. This makes the fallback estimator trigger grip loss *at* the optimal limit, not beyond it.

---

## Root Cause Analysis

### Case A: User 1 ("Too Light") — Correct Behavior, Wrong Threshold

| Condition | Value |
| --- | --- |
| Car | LMP2 |
| `optimal_slip_angle` | 0.06 rad (T300 Preset) |
| Actual Slip at Limit | 0.06 rad |
| `lat_metric` | 0.06 / 0.06 = **1.0** |
| `combined_slip` | ~1.0 (Pure Cornering) |
| `grip` | 1.0 (No Loss Yet) |

**But:** At `slip = 0.065` (just 0.3° beyond optimal):
- `lat_metric` = 0.065 / 0.06 = 1.083
- `combined_slip` = 1.083
- `excess` = 0.083
- `grip` = 1.0 / (1.0 + 0.083 * 2.0) = **0.857**

With `understeer_effect = 50.0`:
- `grip_loss` = (1.0 - 0.857) * 50.0 = 7.15 → clamped
- `grip_factor` = max(0.0, 1.0 - 7.15) = **0.0** 

**Result**: Total force loss at a slip angle only 0.3° past optimal. This is User 1's experience — the system is too aggressive.

---

### Case B: User 2 ("No Effect") — Paradoxical Behavior

User 2 reports the opposite: increasing the effect doesn't create the expected lightening.

**Hypothesis**: If the T300 preset's base understeer value (0.5) is interpreted as "50%" by the user but the code expects raw values up to 200, there may be a mismatch in expectations.

Looking at the GUI (GuiLayer.cpp line 961):
```cpp
FloatSetting("Understeer Effect", &engine.m_understeer_effect, 0.0f, 200.0f, "%.2f", ...)
```

The slider range is **0.0 to 200.0**, so a setting of "1.0" is actually just 0.5% of the maximum effect.

**If** User 2 is testing with values like 1.0-200.0 while using a **different preset** that has `optimal_slip_angle = 0.10` (or game grip data somehow works), the calculated `grip` may remain at 1.0, causing no lightening.

---

## Issue Summary

| Problem | Root Cause | Evidence |
| --- | --- | --- |
| Too Light (User 1) | `optimal_slip_angle = 0.06` is too low; system punishes optimal driving | T300 preset line 62 |
| No Effect (User 2) | Possible preset mismatch or `grip` calculation returning 1.0 | Requires testing |
| Confusing Slider Scale | 200.0 max with non-percentage units | GuiLayer.cpp line 961 |

---

## Recommendations

### 1. Update T300 Preset Default
Change `optimal_slip_angle` from **0.06** to **0.10** in the T300 preset (Config.cpp line 62).

```cpp
// Change from:
p.optimal_slip_angle = 0.06f;
// To:
p.optimal_slip_angle = 0.10f;
```

**Rationale**: 0.10 rad provides a 40% buffer zone beyond typical LMP2 peak slip (0.06-0.08 rad), ensuring drivers can lean on the tires without premature force loss.

### 2. Improve Tooltip Clarity
Update the "Optimal Slip Angle" tooltip (GuiLayer.cpp lines 1068-1072):

**Current:**
> The slip angle (radians) where the tire generates peak grip.

**Proposed:**
> The slip angle threshold above which grip loss begins. Set HIGHER than the car's peak slip angle (e.g., 0.10 for LMDh, 0.12 for GT3) to allow aggressive driving without force loss at the limit.

### 3. Consider Rescaling Understeer Effect
The 0-200 range is confusing. Consider either:
- Rescaling to 0.0-2.0 (percentage-like)
- Adding a "%" format string showing `value / 2.0` as percentage
- Adding documentation clarifying the non-linear impact

---

## Why "Refine the Drop-Off Curve" Was Not Recommended

The original report (v1.0) proposed changing the grip drop-off formula from:
```cpp
result.value = 1.0 / (1.0 + excess * 2.0);  // Original: Steeper curve
```
to:
```cpp
result.value = 1.0 / (1.0 + excess);         // Proposed: Gentler curve
```

**This recommendation was removed because:**

1. **The Real Problem is the Threshold, Not the Curve Shape**
   
   The user's issue (User 1: "too light") is not caused by the curve being too steep — it's caused by the `optimal_slip_angle` being set too low (0.06 rad). When you're already at the peak grip angle, *any* drop-off feels premature.
   
   With the threshold corrected to 0.10 rad, the existing `2.0` multiplier provides a **predictive, proactive feel** — exactly what the developer requested:
   > "I want the user to dynamically feel the loss of grip, and be able to prevent it, and just approach."

2. **The Current Curve is Already Progressive**
   
   Let's compare the grip values at various excess slip levels:

   | Excess | Current (`* 2.0`) | Proposed (`* 1.0`) |
   | --- | --- | --- |
   | 0.1 | 0.833 (17% loss) | 0.909 (9% loss) |
   | 0.2 | 0.714 (29% loss) | 0.833 (17% loss) |
   | 0.3 | 0.625 (38% loss) | 0.769 (23% loss) |
   | 0.5 | 0.500 (50% loss) | 0.667 (33% loss) |
   | 1.0 | 0.333 (67% loss) | 0.500 (50% loss) |
   
   The current formula already provides a smooth, continuous drop-off — not a "cliff" as the original report claimed. The `2.0` multiplier simply makes the transition happen over a shorter range (more predictive).

3. **Changing the Curve Would Break Existing Tuning**
   
   Users who have calibrated their `understeer_effect` slider based on the current curve behavior would experience different feel with the same settings. This is a breaking change with marginal benefit.

4. **The Adjustment is Already User-Tunable**
   
   If a user wants a gentler curve, they can:
   - Increase `optimal_slip_angle` to delay the onset
   - Decrease `understeer_effect` to reduce the force reduction
   
   These controls already exist and are exposed in the GUI.

**Conclusion**: The curve shape is correct for predictive feedback. The problem was the threshold value, which is addressed by Recommendation #1.

---

## Proposed Regression Tests

### Test 1: `test_optimal_slip_buffer_zone`
**Goal**: Verify driving at 60% of optimal slip does NOT trigger force reduction.

```cpp
// Setup
engine.m_optimal_slip_angle = 0.10f;
engine.m_understeer_effect = 50.0f;

// Simulate telemetry with slip_angle = 0.06 rad (60% of 0.10)
// Expected: grip = 1.0 (combined_slip = 0.6 < 1.0)
// Expected: grip_factor = 1.0
ASSERT_TRUE(grip_factor > 0.99);
```

### Test 2: `test_progressive_loss_curve`
**Goal**: Verify smooth grip loss beyond threshold.

```cpp
// Setup
engine.m_optimal_slip_angle = 0.10f;
engine.m_understeer_effect = 1.0f;  // Minimal effect for test

// Slip = 0.10 (1.0x optimal) → grip = 1.0, factor = 1.0
// Slip = 0.12 (1.2x optimal) → grip = 0.714, factor = 0.714
// Slip = 0.14 (1.4x optimal) → grip = 0.556, factor = 0.556

ASSERT_TRUE(factors are decreasing progressively);
```

### Test 3: `test_grip_floor_clamp`
**Goal**: Verify grip never drops below 0.2 (current safety floor).

```cpp
// Setup extreme slip
// Expected: calculated grip is floored at 0.2
ASSERT_GE(result.value, 0.2);
```

### Test 4: `test_understeer_output_clamp`
**Goal**: Verify `grip_factor` is clamped to [0.0, 1.0] and force never inverts.

```cpp
// Setup extreme understeer_effect = 200.0, grip = 0.5
// grip_loss = (1.0 - 0.5) * 200.0 = 100.0
// grip_factor_raw = 1.0 - 100.0 = -99.0
// grip_factor_clamped = max(0.0, -99.0) = 0.0

ASSERT_GE(grip_factor, 0.0);
ASSERT_LE(grip_factor, 1.0);
```

---

## Appendix: Formula Reference

### Combined Friction Circle (Grip Estimation)

$$
\text{lat\_metric} = \frac{|\alpha|}{\alpha_{\text{opt}}} \quad
\text{long\_metric} = \frac{|\kappa|}{\kappa_{\text{opt}}}
$$

$$
\text{combined\_slip} = \sqrt{\text{lat\_metric}^2 + \text{long\_metric}^2}
$$

$$
\text{grip} = \begin{cases} 
1.0 & \text{if combined\_slip} \leq 1.0 \\
\frac{1.0}{1.0 + (\text{combined\_slip} - 1.0) \times 2.0} & \text{otherwise}
\end{cases}
$$

### Understeer Effect Application

$$
\text{grip\_loss} = (1.0 - \text{grip}) \times \text{understeer\_effect}
$$

$$
\text{grip\_factor} = \max(0.0, 1.0 - \text{grip\_loss})
$$

$$
F_{\text{output}} = F_{\text{base}} \times \text{grip\_factor}
$$

---

## Implementation Code Snippets

The following code changes implement the recommendations above.

### Change 1: Update T300 Preset Default (Config.cpp)

**File**: `src/Config.cpp`  
**Location**: Line 62 (inside LoadPresets(), T300 preset block)

```cpp
// BEFORE (line 62):
p.optimal_slip_angle = 0.06f;

// AFTER:
p.optimal_slip_angle = 0.10f;
```

**Full context** (lines 33-63):
```cpp
    // 2. T300 (Custom optimized)
    {
        Preset p("T300", true);
        p.invert_force = true;
        p.gain = 1.0f;
        p.max_torque_ref = 100.1f;
        p.min_force = 0.01f;
        p.steering_shaft_gain = 1.0f;
        p.steering_shaft_smoothing = 0.0f;
        p.understeer = 0.5f;
        p.base_force_mode = 0;
        // ... other settings ...
        p.optimal_slip_angle = 0.10f;   // CHANGED from 0.06f
        p.optimal_slip_ratio = 0.12f;
        // ... rest of preset ...
    }
```

---

### Change 2: Improve Tooltip Clarity (GuiLayer.cpp)

**File**: `src/GuiLayer.cpp`  
**Location**: Lines 1068-1072 (Optimal Slip Angle setting)

```cpp
// BEFORE:
FloatSetting("Optimal Slip Angle", &engine.m_optimal_slip_angle, 0.05f, 0.20f, "%.2f rad", 
    "The slip angle (radians) where the tire generates peak grip.\nTuning parameter for the Grip Estimator.\nMatch this to the car's physics (GT3 ~0.10, LMDh ~0.06).\n"
    "Lower = Earlier understeer warning.\n"
    "Higher = Later warning.\n"
    "Affects: Understeer Effect, Lateral G Boost (Slide), Slide Texture.");

// AFTER:
FloatSetting("Optimal Slip Angle", &engine.m_optimal_slip_angle, 0.05f, 0.20f, "%.2f rad", 
    "The slip angle THRESHOLD above which grip loss begins.\n"
    "Set this HIGHER than the car's physical peak slip angle.\n"
    "Recommended: 0.10 for LMDh/LMP2, 0.12 for GT3.\n\n"
    "Lower = More sensitive (force drops earlier).\n"
    "Higher = More buffer zone before force drops.\n\n"
    "NOTE: If the wheel feels too light at the limit, INCREASE this value.\n"
    "Affects: Understeer Effect, Lateral G Boost (Slide), Slide Texture.");
```

---

### Change 3: Rescale Understeer Effect Display (GuiLayer.cpp) [Optional]

**File**: `src/GuiLayer.cpp`  
**Location**: Line 961 (Understeer Effect setting)

This change adds a percentage-based display format while keeping the internal 0-200 range for backward compatibility.

```cpp
// BEFORE:
FloatSetting("Understeer Effect", &engine.m_understeer_effect, 0.0f, 200.0f, "%.2f", 
    "Reduces the strength of the Steering Shaft Torque when front tires lose grip (Understeer).\n"
    "Helps you feel the limit of adhesion.\n"
    "0% = No feeling.\n"
    "High = Wheel goes light immediately upon sliding. "
    "Note: grip is calculated based on the Optimal Slip Angle setting.");

// AFTER:
// Create a custom format that shows percentage
auto understeer_fmt = [&]() {
    static char buf[32];
    snprintf(buf, 32, "%.1f%% (%.1f)", 
             engine.m_understeer_effect / 2.0f,  // Show as 0-100%
             engine.m_understeer_effect);         // Also show raw value
    return (const char*)buf;
};

FloatSetting("Understeer Effect", &engine.m_understeer_effect, 0.0f, 200.0f, understeer_fmt(), 
    "Reduces the strength of the Steering Shaft Torque when front tires lose grip.\n\n"
    "Scale: 0-100% (displayed), 0-200 (internal).\n"
    "  - 0% = Effect disabled.\n"
    "  - 50% (100 internal) = Moderate effect.\n"
    "  - 100% (200 internal) = Maximum sensitivity.\n\n"
    "If the wheel goes TOO light, reduce this value.\n"
    "If you can't feel understeer, increase this value.\n\n"
    "Tip: Start at 25% (50) and adjust based on feel.\n"
    "Interacts with: Optimal Slip Angle setting.");
```

**Alternative (Simpler)**: Just update the tooltip without changing the format:

```cpp
FloatSetting("Understeer Effect", &engine.m_understeer_effect, 0.0f, 200.0f, "%.1f", 
    "Reduces steering force when front tires lose grip.\n\n"
    "SCALE GUIDE:\n"
    "  0-10: Subtle effect (recommended for aggressive driving)\n"
    "  10-50: Moderate effect (good starting point)\n"
    "  50-100: Strong effect\n"
    "  100-200: Extreme (will go fully light on any slide)\n\n"
    "If the wheel feels too light at the limit:\n"
    "  1. First, INCREASE 'Optimal Slip Angle' above.\n"
    "  2. If still too light, DECREASE this value.\n\n"
    "Note: The effect is multiplied by calculated grip loss.");
```

---

## Document History
| Version | Date | Author | Notes |
| --- | --- | --- | --- |
| 1.0 (Draft) | 2026-01-02 | Antigravity | Initial analysis based on user reports |
| 2.0 (Revised) | 2026-01-02 | Antigravity | Full code review, formula verification, corrected recommendations |
| 2.1 | 2026-01-02 | Antigravity | Added implementation snippets, explained curve recommendation removal |
