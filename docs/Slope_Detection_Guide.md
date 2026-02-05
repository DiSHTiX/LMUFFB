# Slope Detection Algorithm - Technical Guide

**Version:** 0.7.11  
**Status:** Stable / Recommended  
**Last Updated:** February 5, 2026

---

## Table of Contents

1. [Overview](#overview)
2. [Why Slope Detection?](#why-slope-detection)
3. [How It Works](#how-it-works)
4. [Understanding the Settings](#understanding-the-settings)
5. [Latency Explained](#latency-explained)
6. [Tuning Guide](#tuning-guide)
7. [Troubleshooting](#troubleshooting)
8. [Technical Deep Dive](#technical-deep-dive)

---

## Overview

**Slope Detection** is an adaptive algorithm in lmuFFB v0.7.11 that dynamically estimates tire grip by monitoring the **rate of change** (slope) of the tire's performance curve in real-time, rather than using static thresholds.

**Key Benefits:**
- 🎯 **Adaptive** - Automatically adjusts to different tire compounds, temperatures, and wear states
- 🏁 **Track-Agnostic** - Works consistently across all tracks without manual tuning
- 📊 **Physically Accurate** - Detects the actual saturation point of the tire, not an arbitrary threshold
- 🔄 **Dynamic** - Responds to changing conditions during the session (tire warm-up, degradation)

**Trade-off:**
- Introduces small latency (6-50ms depending on settings) due to signal processing

---

## Why Slope Detection?

### The Problem with Static Thresholds

Traditional grip estimation uses **fixed thresholds** like "Optimal Slip Angle = 0.10 rad". This approach has several limitations:

❌ **One-Size-Fits-All** - A single threshold can't accommodate:
- Different tire compounds (street vs slick)
- Temperature variations (cold vs optimal vs overheated)
- Tire wear progression
- Different car setups (high vs low downforce)

❌ **Binary Detection** - Either you're below the threshold (full grip) or above it (reduced grip), with no smooth transition representing the actual tire curve

❌ **Requires Manual Tuning** - Users must adjust the threshold for each car/track combination to get realistic feedback

### The Tire Performance Curve

Real tires follow a characteristic performance curve:

```
Lateral Force (G)
    │
1.8 │         ╭─────╮         ← Peak (Maximum Grip)
    │        ╱       ╲
1.5 │       ╱         ╲
    │      ╱           ╲╲
1.2 │     ╱             ╲╲    ← Post-Peak (Sliding)
    │    ╱               ╲╲╲
0.9 │   ╱                 ╲╲╲
    │  ╱
0.6 │ ╱
    │╱
    └────────────────────────── Slip Angle (rad)
    0   0.05  0.10  0.15  0.20
    
        ↑              ↑
    Building Grip   Saturating
```

**Key Insight:** The **slope** (steepness) of this curve tells us where we are:
- **Positive slope (rising):** Tire is building grip - safe to push harder
- **Zero slope (plateau):** Tire is at peak grip - maximum cornering force
- **Negative slope (falling):** Tire is saturating - sliding/losing grip

Slope Detection monitors this slope in real-time to provide accurate grip feedback regardless of the tire's characteristics.

---

## How It Works

### The Algorithm (Simplified)

1. **Data Collection:** Every 2.5ms (400Hz), lmuFFB receives:
   - Lateral G-force (how much the car is cornering)
   - Slip Angle (difference between where the tire points vs where it's moving)

2. **Buffering:** Recent samples are stored in a circular buffer (5-41 samples depending on settings)

3. **Derivative Calculation:** A **Savitzky-Golay (SG) filter** calculates the slope:
   ```
   Slope = Change in Lateral G ÷ Change in Slip Angle
   ```
   
4. **Grip Factor Estimation:**
   - **Slope > Min Threshold** → Grip Factor = 1.0 (100% grip)
   - **Slope < Min Threshold** → Grip Factor decreases linearly towards the **Max Threshold**.

5. **FFB Adjustment:** The Understeer Effect is scaled by the grip factor:
   ```
   FFB Force = Base Force × (1.0 - Grip Loss × Understeer Effect)
   ```

### Why Savitzky-Golay Filtering?

Telemetry data is **noisy**. Raw derivatives would produce this:

```
Raw Derivative (No Filtering)
    │  ╱╲  ╱╲╱╲
    │ ╱  ╲╱    ╲╱╲  ╱╲
────┼──────────────────────  → Unusable (too jittery)
    │      ╲╱      ╲╱
```

SG filtering smooths the signal while preserving the true trend:

```
SG Filtered Derivative
    │      ╭───────╮
    │     ╱         ╲
────┼────────────────────────  → Clean, usable slope
    │                 ╲
```

**SG filtering provides:**
✅ Smooth derivatives from noisy data  
✅ Preserves the shape of the underlying signal  
✅ Mathematically rigorous (fits a polynomial to the data window)  
✅ Widely used in scientific data analysis

---

## Understanding the Settings

### Enable Slope Detection
**Toggle:** ON / OFF  
**Default:** OFF

When enabled, replaces the static "Optimal Slip Angle" threshold with dynamic slope monitoring. The "Optimal Slip Angle" and "Optimal Slip Ratio" settings are ignored when this is ON.

> ⚠️ **Note:** When Slope Detection is enabled, **Lateral G Boost (Slide)** is automatically disabled to prevent oscillations caused by asymmetric calculation methods between axles.

---

### Filter Window
**Range:** 5 - 41 samples  
**Default:** 15 samples  
**Unit:** Number of telemetry samples

**What it does:** Determines how many recent samples are used to calculate the slope.

**Effect on Latency:**
```
Latency (ms) = (Window Size ÷ 2) × 2.5ms

Examples:
Window =  5  →  6.25 ms latency  (Very Responsive, Noisier)
Window = 15  → 18.75 ms latency  (Default - Balanced)
Window = 21  → 26.25 ms latency  (Smooth, Higher Latency)
Window = 31  → 38.75 ms latency  (Very Smooth, Sluggish)
```

**Must be ODD:** The algorithm requires an odd number for symmetry (5, 7, 9, 11, ... 41). The GUI enforces this automatically.

**Tuning Guidance:**
- **Start with 15** - Good balance for most users
- **Lower to 7-11** if you want sharper, more immediate feedback (accept some noise)
- **Raise to 21-31** if FFB feels jittery or twitchy (smoother but slower)

---

### Min/Max Threshold System (v0.7.11)
The core of the updated algorithm is the move from a single "Sensitivity" multiplier to a predictable **linear mapping** between two threshold values.

#### Min Threshold
**Range:** -5.0 to 0.0  
**Default:** -0.3
**Unit:** (Lateral G / Slip Angle rad)

**What it does:** The slope value where the understeer effect **begins**.
- If the tire curve is steeper than this (e.g., -0.1), you have 100% FFB weight.
- Think of this as the "Dead Zone" edge for slope detection.
- **Tuning:** Move closer to 0.0 (e.g., -0.1) for earlier warnings. Move further away (e.g., -0.8) if the wheel feels light too easily.

#### Max Threshold
**Range:** -10.0 to -0.1  
**Default:** -2.0
**Unit:** (Lateral G / Slip Angle rad)

**What it does:** The slope value where the understeer effect **fully saturates**.
- If the slope is equal to or more negative than this (e.g., -3.5), the grip loss reaches its maximum (limited by the floor setting).
- **Tuning:** Move closer to Min (e.g., -1.0) for a "sharper" drop-off. Move further away (e.g., -5.0) for a more gradual, progressive lightening.

> 💡 **Legacy Note:** If you are upgrading from v0.7.3, lmuFFB will automatically migrate your old "Sensitivity" setting into an equivalent Max Threshold.

---

### Advanced Settings (Collapsed by Default)

*(Section merged into Min/Max Threshold System above)*

---

#### Output Smoothing
**Range:** 0.005s - 0.100s  
**Default:** 0.040s (40ms) (v0.7.1)
**Unit:** Seconds (time constant)

Applies an exponential moving average to the final grip factor to prevent abrupt FFB changes.

**Formula:**
```
α = dt / (tau + dt)
Smoothed Output = Previous Output + α × (New Grip Factor - Previous Output)
```

**Higher values** = slower transitions (smoother FFB)  
**Lower values** = faster transitions (more responsive)

**Tuning Guidance:**
- Default (0.02s) is well-tested
- Reduce to 0.01s if FFB feels laggy
- Increase to 0.05s if you experience sudden FFB jerks

---

### v0.7.3 Stability Fixes

These parameters address the "sticky understeer" and oscillation issues from v0.7.0-0.7.2.

#### Alpha Threshold
**Range:** 0.001 - 0.100  
**Default:** 0.020  
**Unit:** rad/s (steering rate)

**What it does:** Minimum change in slip angle (dAlpha/dt) required to calculate the slope. Below this threshold, the slope **decays toward zero** instead of being held constant.

**Effect:**
- **Higher values (0.05+):** More stable on straights, slower response to corner entry
- **Lower values (0.01-):** Faster response, but may trigger on minor steering inputs
- **Default (0.02):** Balanced - filters out noise while remaining responsive

**Why this matters:** In v0.7.0-0.7.2, if you cornered hard then straightened the wheel, the slope value would "stick" at its last calculated value, making the understeer effect persist on straights. Now it smoothly fades away.

**Tuning Guidance:**
- Increase if you feel understeer on straights or during gentle lane changes
- Decrease if the wheel doesn't lighten quickly enough when approaching the limit

---

#### Decay Rate
**Range:** 0.5 - 20.0  
**Default:** 5.0  
**Unit:** 1/s (inverse seconds)

**What it does:** Controls how fast the slope returns to zero when you're not actively cornering (i.e., when dAlpha/dt is below the threshold).

**Effect:**
```
Decay Rate =  5.0  →  ~200ms to reach neutral (Default)
Decay Rate = 10.0  →  ~100ms to reach neutral (Fast recovery)
Decay Rate =  2.0  →  ~500ms to reach neutral (Slow fade)
```

**Why this matters:** After exiting a corner, you want the "light wheel" feeling to fade quickly so straights feel normal again. Too slow = sticky understeer. Too fast = abrupt transitions.

**Tuning Guidance:**
- Increase (10-15) if you want instant recovery when straightening the wheel
- Decrease (2-3) if transitions feel too sudden or artificial
- Default (5.0) provides natural fade that matches tire physics

---

#### Confidence Gate
**Type:** Toggle (ON / OFF)  
**Default:** ON  

**What it does:** Scales the grip loss effect by the magnitude of dAlpha/dt (steering activity). When dAlpha/dt is low, the algorithm has low "confidence" in its slope calculation, so it reduces the effect.

**Mathematical Scaling:**
```
Confidence = min(1.0, |dAlpha/dt| / 0.1)

If dAlpha/dt = 0.02  →  Confidence = 20%  →  Only 20% of grip loss applied
If dAlpha/dt = 0.10+ →  Confidence = 100% →  Full grip loss applied
```

**Effect:**
- **ON (Default):** Smooth, progressive transitions. Low-speed maneuvering doesn't trigger false alarms.
- **OFF:** Previous behavior - full effect always applies when slope is negative. Can cause jolts during parking lot speeds or minor corrections.

**Why this matters:** Prevents random FFB jolts when driving slowly or making tiny steering adjustments (e.g., highway driving). The effect only reaches full strength during active cornering.

**Tuning Guidance:**
- **Keep ON** unless you specifically want the old binary behavior
- Turn OFF if you want maximum sensitivity (e.g., autocross where every mm of slip matters)

---

### Live Diagnostics
```
Live Slope: 0.142 | Grip: 100% | Cur derivative: -0.15
```

**Live Slope:** The current filtered slope value used for grip loss calculation.  
**Grip:** The current grip percentage (100% = full grip, lower = reduced grip).  
**Slope Graph:** A live scrolling graph available in the "Internal Physics" header to visualize the tire curve derivative.

Use these to understand what the algorithm is detecting during driving.

---

## Latency Explained

### What is Latency?

**Latency** is the time delay between a physical event (tire starts to slide) and when you feel it in the FFB.

**Sources of latency in lmuFFB:**
1. **Game Engine:** 2.5ms per frame (400Hz physics)
2. **SG Filter:** (Window / 2) × 2.5ms
3. **Output Smoothing:** ~1× tau (default 20ms)
4. **DirectInput API:** 1-2ms
5. **Wheel Hardware:** 5-30ms (varies by model)

**Total typical latency:** 25-60ms with default settings

---

### Why Can't We Have 0 Latency?

**Short Answer:** You cannot calculate a **derivative** (rate of change) from a single instant in time. You need at least 2 samples, which introduces delay.

**Long Answer:**

#### 1. Derivatives Require Time Comparison

To calculate slope, you need:
```
Slope = (Later Value - Earlier Value) / Time Between Them
         ↑               ↑
      Future         Past
```

You **cannot** know the "later value" until time has passed. This is a fundamental limitation of physics, not a software issue.

**Minimum possible latency:** 1 sample = 2.5ms (using only the current and previous frame)

---

#### 2. Noise Requires Averaging

Raw telemetry looks like this:
```
Sample 1: Lateral G = 1.523
Sample 2: Lateral G = 1.487  (Dropped 0.036 G in 2.5ms?!)
Sample 3: Lateral G = 1.531  (Jumped 0.044 G?!)
Sample 4: Lateral G = 1.509
```

If we calculate slope from just 2 samples:
```
Slope = (1.487 - 1.523) / 0.0025 = -14.4  ← FALSE ALARM!
```

The tire isn't actually losing grip - this is just **measurement noise** from:
- Suspension vibrations
- Track surface bumps
- Numerical precision limits
- Sensor sampling artifacts

**SG filtering averages multiple samples** to reveal the true trend:
```
5-sample average: Slope = -0.03  (Slight decline - tire at peak)
```

This is the correct reading, but it requires looking at 5 samples = 12.5ms of history.

---

#### 3. The Latency-Noise Trade-off

```
Window Size vs Performance

Latency (ms)
    │
 50 │                         ● (Window=41)
    │                    ●
 40 │               ●
    │          ●
 30 │     ●                    ← Sweet Spot (Window=15-21)
    │ ●                           Good balance
 20 │●
    │
 10 │● (Window=5)              ← Too Jittery
    │
  0 └────────────────────────────────── Noise Rejection
      Low                          High
      
      ↑                             ↑
   Fast but noisy            Smooth but laggy
```

**The goal:** Find the smallest window that still filters noise effectively.

**Why 5-41 is the range:**
- **Minimum (5):** Below this, noise dominates and the slope calculation is unreliable
- **Maximum (41):** Beyond this, latency becomes perceptible and the tire state may have changed significantly

---

### Why Low Latency Isn't Always Better

**Human perception:** Studies show humans cannot detect FFB changes faster than ~15-20ms. Below this threshold, you're fighting noise for zero perceptual benefit.

**Example:**
```
Scenario: You enter a corner too fast

Window = 5 (6ms latency):
  FFB becomes jittery and unstable as noise causes false grip loss signals
  → Confused driver, harder to correct

Window = 15 (19ms latency):
  FFB smoothly transitions as tire approaches and exceeds peak grip
  → Clear feedback, easy to modulate

Window = 31 (39ms latency):
  FFB change lags behind, tire may have recovered grip before wheel lightens
  → Delayed feedback, harder to react
```

**Best practice:** Use the **largest window that still feels responsive enough** for your driving style. More filtering = more reliable signal.

---

## Tuning Guide

### Quick Start (Recommended Settings)

**For Most Users (v0.7.11 Recommended):**
```
Enable Slope Detection: ON
Filter Window: 15
Min Threshold: -0.3
Max Threshold: -2.0
Alpha Threshold: 0.02
Decay Rate: 5.0
Confidence Gate: ON
```

Then adjust "Understeer Effect" slider (in the main Front Axle section) to taste:
- **0.5-0.7:** Subtle grip loss feel
- **1.0:** Realistic 1:1 grip scaling (recommended)
- **1.5-2.0:** Exaggerated light-wheel warning

---

### Fine-Tuning Process

#### Step 1: Find Your Ideal Window Size

1. **Enable Slope Detection** and set Sensitivity to 1.0
2. **Drive a familiar corner** at high speed (one where you know the limit)
3. **Experiment with window sizes:**

**If FFB feels jittery/noisy:**
- Increase window: 15 → 21 → 27
- Check live diagnostics - slope should be smooth, not jumping wildly

**If FFB feels laggy/delayed:**
- Decrease window: 15 → 11 → 7
- Wheel should lighten BEFORE you feel the backend step out

**Target:** Smooth transition as you approach the limit, without feeling disconnected

---

#### Step 2: Adjust Thresholds

**If wheel doesn't lighten enough when understeering:**
- Move **Max Threshold** closer to Min (e.g., -2.0 → -1.2) - makes the drop steeper.
- OR move **Min Threshold** closer to zero (e.g., -0.3 → -0.15) - makes the effect start sooner.

**If wheel becomes too light, feels unrealistic:**
- Move **Max Threshold** further from Min (e.g., -2.0 → -5.0) - makes the drop more gradual.
- OR move **Min Threshold** further from zero (e.g., -0.3 → -0.6) - adds a larger dead-zone.

---

#### Step 3: Validate Across Conditions

Test in different scenarios:
- **Cold tires** (first lap out of pits): Should feel more grip loss
- **Optimal temp** (3-5 laps in): Should feel most responsive
- **Worn tires** (late stint): Should feel gradual degradation

If slope detection adapts well to these changing conditions without manual adjustment, you've found your sweet spot!

---

### Presets for Different Wheel Types

#### Direct Drive (High Bandwidth, Low Noise)
```
Filter Window: 11
Min Threshold: -0.2
Max Threshold: -1.5
Output Smoothing: 0.015s
```
DD wheels can handle faster response without feeling twitchy.

---

#### Belt Drive (Moderate Bandwidth)
```
Filter Window: 15  (Default)
Sensitivity: 1.0
Output Smoothing: 0.020s
```
Balanced settings work best.

---

#### Gear Drive (Lower Bandwidth, More Noise)
```
Filter Window: 21
Min Threshold: -0.4  (larger dead-zone)
Max Threshold: -3.0  (more gradual)
Output Smoothing: 0.030s
```
Gear-driven wheels benefit from more smoothing to hide mechanical noise.

---

## Troubleshooting

### "FFB feels jittery, wheel shakes when cornering"

**Cause:** Filter window too small, noise not being filtered effectively

**Solution:**
1. Increase Filter Window: Try 21, 25, or 31
2. Increase Output Smoothing: 0.04s → 0.06s
3. Lower Sensitivity: 0.5 → 0.3
4. Check that window size is **odd** (should be automatic)

---

### "Wheel lightens too late, I'm already sliding"

**Cause:** Too much latency

**Solution:**
1. Decrease Filter Window: Try 11, 9, or 7
2. Decrease Output Smoothing: 0.02s → 0.01s
3. Check live diagnostics - slope should go negative BEFORE you feel understeer

---

### "Wheel doesn't lighten at all when I push too hard"

**Possible Causes:**

**A. Slope Detection not actually enabled**
- Check the toggle is ON
- Restart lmuFFB if in doubt

**B. Sensitivity too low**
- Increase Sensitivity to 1.5 or 2.0
- Check "Understeer Effect" slider isn't set to 0

**C. Live diagnostics show slope staying positive**
- You may not be pushing hard enough to exceed peak grip
- Try a slower corner where you can really overdrive

---

### "Ride height or suspension changes affect FFB"

**Cause:** Slope detection is working correctly! Different ride heights change the tire's slip angle characteristics.

**This is a feature, not a bug.** The algorithm adapts to your setup changes, just like a real car would feel different with different suspension settings.

---

### "Oscillations when turning in or cornering"

**Cause:** Conflict between Slope Detection (Front only) and Lateral G Boost (Global effect).

**Fix in v0.7.1:** lmuFFB now automatically disables **Lateral G Boost (Slide)** when Slope Detection is enabled. This eliminates the feedback loop that caused oscillations in v0.7.0. If you still experience oscillations, try increasing the **Filter Window** (e.g., 21 or 25).

---

### "Slope goes negative randomly even on straights"

**Cause (v0.7.0-0.7.2):** Measurement noise from curbs, bumps, or flatspotted tires.

**Fixed in v0.7.3:** The Alpha Threshold and Decay Rate parameters now prevent this. The slope automatically decays to zero when steering input is minimal.

**If still experiencing issues:**
1. Increase Alpha Threshold: 0.02 → 0.03 (requires more steering activity)
2. Increase Filter Window: 15 → 21 (more smoothing)
3. Verify Confidence Gate is ON (should filter out low-confidence triggers)

---

## Technical Deep Dive

### Savitzky-Golay Mathematics

The SG filter fits a **quadratic polynomial** to the data window and analytically computes its derivative.

**For a window of size M (half-width):**

```math
Derivative Coefficient w_k = k × (Scaling Factor)

where Scaling Factor = 2M(M+1) / [(2M+1) × S₂]

and S₂ = M(M+1)(2M+1) / 3
```

**Example for Window=15 (M=7):**
```
Coefficients: w[-7] = -7×SF, w[-6] = -6×SF, ... w[0] = 0, ... w[7] = 7×SF

Derivative = Σ(w[k] × G[k]) / (Δt × Σ|w[k]|)
```

**Key properties:**
- Coefficients are anti-symmetric: w[-k] = -w[k]
- Center point has zero weight (doesn't bias toward current value)
- Edge points have maximum weight (captures the overall trend)

**Reference:** Savitzky, A.; Golay, M.J.E. (1964). "Smoothing and Differentiation of Data by Simplified Least Squares Procedures". *Analytical Chemistry* 36 (8): 1627-1639.

---

### Grip Factor Calculation

```cpp
// 1. Calculate current slope (from SG filter derivatives)
double slope = dG_dt / dAlpha_dt;

// 2. Perform Linear Mapping (v0.7.11)
// Clamp value between min and max, then normalize to 0.0-1.0
double loss_percent = inverse_lerp(m_slope_min_threshold, m_slope_max_threshold, slope);

// 3. Apply Scaling and Safety Floor
// 0% loss (loss_percent=0) -> 1.0 factor
// 100% loss (loss_percent=1) -> 0.2 factor (default floor)
double current_grip_factor = 1.0 - (loss_percent * 0.8 * confidence);

// 4. Apply Final Smoothing (EMA)
double alpha = dt / (m_slope_smoothing_tau + dt);
m_slope_smoothed_output = lerp(m_slope_smoothed_output, current_grip_factor, alpha);
```

**Safety Features:**
- Grip factor cannot go below 0.2 (20%) to prevent complete FFB loss
- Slope spikes are ignored when slip angle isn't changing (dt < 0.001)
- Slope detection only applies to front axle (lateral G is a whole-vehicle measurement)

---

### Front Axle Only

**Why?** Lateral G-force is measured at the **center of mass**, not at individual tires. The slope of G-vs-slip represents the **vehicle's overall lateral response**, which is dominated by the front tires (steering inputs).

**Rear grip** continues to use static threshold detection based on wheel slip and slip angle, which is appropriate for detecting oversteer and rear grip loss.

This asymmetry is intentional and physically justified.

---

## Comparison: Slope Detection vs Static Threshold

| Aspect | Static Threshold | Slope Detection |
|--------|------------------|-----------------|
| **Adaptability** | Fixed, must be tuned per car/track | Automatically adapts to conditions |
| **Latency** | ~0ms (instant) | 6-50ms (configurable) |
| **Accuracy** | Approximation based on arbitrary threshold | Detects actual tire saturation point |
| **Tuning Required** | High (Optimal Slip Angle must be dialed in) | Low (mostly set-and-forget) |
| **Noise Sensitivity** | Low (simple comparison) | Medium (requires filtering) |
| **Tire Degradation** | No adaptation (threshold stays fixed) | Adapts as grip degrades |
| **Best For** | Users who want instant response and minimal latency | Users who want accurate, adaptive, and organic understeer feel |

**Both methods are valid.** Choose the one that matches your priorities.

---

## Frequently Asked Questions

### Q: Should I use Slope Detection or stick with Static Threshold?

**Use Slope Detection if:**
- You drive multiple car types (GT3, LMP2, etc.) and want one setting to work for all
- You want realistic tire warm-up and degradation feel
- You're willing to accept 15-25ms of latency for more accurate feedback

**Use Static Threshold if:**
- You prioritize absolute minimum latency
- You drive mostly one car and can dial in the perfect threshold
- You prefer the "binary" feel of grip/no-grip

---

### Q: Can I use Slope Detection for drifting?

**Not recommended.** Drifting involves sustained large slip angles where the tire is always past peak. Slope detection assumes you're operating near the peak and transitioning across it.

For drifting, use:
- Static threshold method with high Optimal Slip Angle (0.15-0.20)
- Focus on "Rear Align Torque" and "Oversteer Boost" effects
- Enable "Slide Texture" for scrub feedback

---

### Q: Does Slope Detection work with keyboard/mouse?

**Yes**, but you won't feel the FFB changes (obviously). The grip estimation still happens and affects the car's behavior if FFB is routed to vJoy or similar virtual controller.

---

### Q: Why is my latency different from the displayed value?

The displayed latency is **only the SG filter contribution**. Total latency includes:
- Game engine frame time
- Output smoothing
- DirectInput API
- Your wheel's internal processing

Actual perceived latency will be higher than displayed.

---

### Q: Can I reduce latency below 6ms?

**Current answer (v0.7.0):** Not without bypassing the SG filter entirely. A 1-sample derivative (2.5ms latency) is possible but:
- Would be extremely noisy
- Defeats the purpose of slope detection
- Better to just disable the feature

**6ms (Window=5) is the practical minimum** for usable slope calculation.

**Coming in v0.7.1:** We're planning to lower the minimum window size from 5 to 3, which will provide:
- **3.75ms latency** (40% reduction from current minimum)
- Still mathematically valid (SG filtering requires minimum 3 points)
- Warning tooltip for users selecting very small windows
- Suitable for Direct Drive wheels and advanced users who can tolerate some noise

This will be an **optional enhancement** - the default will remain at window=15 for most users.

---

*(Section removed: Low-latency enhancements were implemented in v0.7.4)*

---

## Conclusion

Slope Detection represents a significant advancement in FFB grip estimation, moving from static approximations to dynamic, physics-based detection. With proper tuning, it provides adaptive, consistent feedback across all driving conditions.

**Recommended approach:**
1. Start with defaults (Window=15, Sensitivity=1.0)
2. Adjust window size for your wheel type and driving style
3. Fine-tune sensitivity to match your preference
4. Let the algorithm adapt - avoid constant tweaking

**Remember:** Slope Detection is an evolving feature. Your feedback helps improve it for future releases. Share your findings on the forum!

---

## Additional Resources

- **Implementation Plan:** `docs/dev_docs/implementation_plans/plan_slope_minmax_thresholds.md`
- **Source Code:** `src/FFBEngine.h` (calculate_slope_grip)

---

**Document Version:** 1.3 (v0.7.11)
**lmuFFB Version:** 0.7.11  
**Author:** lmuFFB Development Team  
**License:** This document is distributed with lmuFFB under the same MIT license.
