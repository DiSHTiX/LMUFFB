# Understanding LMUFFB's FFB Signal Processing
## Response to Community Feedback on Torque Values and UI Philosophy

**Date**: 2026-01-25  
**Original Post**: Understanding the Percentages and Nm Values  
**Status**: For Community Discussion

---

## Summary of Questions

The user raised excellent points about the current UI being confusing, particularly around:
1. What do the **Nm values in parentheses** mean (e.g., "SoP Lateral G 147.1% (~7.2 Nm)")?
2. What is the **percentage reference**?
3. Why does **increasing Max Torque Ref** increase the Nm numbers but make the wheel lighter?
4. Why do torque numbers **exceed the wheel base's physical max** without clipping?
5. A proposed **"FFB Bucket" philosophy** for the UI.

---

## Answers to Each Question

### 1. What Do the Nm Values in Parentheses Mean?

The `~7.2 Nm` value is an **estimation of the physical torque contribution** of that specific effect at its current gain setting. 

However, this is **not the actual torque being sent to your wheel** — it's a *normalized reference value* based on a mathematical formula that helps compare effects relative to each other.

**The formula** (from `GuiLayer.cpp`):

```cpp
auto FormatDecoupled = [&](float val, float base_nm) {
    float scale = (engine.m_max_torque_ref / 20.0f); 
    if (scale < 0.1f) scale = 0.1f;
    float estimated_nm = val * base_nm * scale;
    // Display: "147.1% (~7.2 Nm)"
};
```

**What this means:**
- `val` = Your slider value (e.g., 1.47 for 147%)
- `base_nm` = A physics constant for that effect (e.g., `BASE_NM_SOP_LATERAL = 1.0 Nm`, `BASE_NM_YAW_KICK = 5.0 Nm`)
- `scale = Max Torque Ref / 20.0` = How much the torque reference amplifies the display

**Example calculation:**
- SoP Lateral G at 147% (1.47), with Base Nm = 1.0, and Max Torque Ref = 98 Nm:
  - `scale = 98 / 20 = 4.9`
  - `estimated_nm = 1.47 × 1.0 × 4.9 = 7.2 Nm` 

**Important**: This is purely for **display purposes** to help you gauge relative effect strength. It is NOT the actual torque being output.

---

### 2. What Is the Percentage Reference?

The percentage represents the **gain multiplier** for each effect:

| Percentage | Meaning |
|------------|---------|
| 0% (0.0) | Effect disabled |
| 100% (1.0) | Effect at "default" strength (1:1 with physics) |
| 200% (2.0) | Effect doubled |

**The reference is always 100% = 1.0 multiplier.**

When you set "SoP Lateral G" to 147%, you're telling the system to multiply the raw Lateral G physics by 1.47x before mixing it into the final signal.

---

### 3. Why Does Increasing Max Torque Ref Increase the Nm Numbers But Make the Wheel Lighter?

This is the most confusing part of the current UI, and your observation is **completely correct**. Here's what's happening:

**Max Torque Ref is the "Calibration Scale"** — it represents the **expected peak torque of the simulated car**, NOT your wheel base.

**How it works:**

```
Final Output = (Sum of all effects) / Max Torque Ref × Master Gain
```

| Max Torque Ref | Effect on Output |
|----------------|------------------|
| Lower (e.g., 30 Nm) | Stronger forces (more clipping risk) |
| Higher (e.g., 100 Nm) | Lighter forces (less clipping, more headroom) |

**Why the UI shows higher Nm with higher Max Torque Ref:**
- The displayed Nm is scaled by `Max Torque Ref / 20.0`
- Higher Max Torque Ref → Higher displayed Nm
- But the *actual output* is **divided** by Max Torque Ref
- So the wheel feels lighter despite the higher number

**This is undeniably confusing!** The display formula was intended to show "how much torque this effect would produce in the simulation," but this mental model conflicts with the user's expectation of "how much torque my wheel will output."

---

### 4. Why Can Torque Numbers Exceed My Wheel's Max Without Clipping?

**Excellent observation!** This gets to the heart of why the current system differs from your "bucket" analogy.

**Key insight**: The numbers in the UI are **potential maximum contributions**, not actual real-time outputs.

**Why you don't always clip:**
1. **Effects are not all active simultaneously at maximum** — For example:
   - Lateral G is at maximum only in high-speed corners
   - Yaw Kick fires only for milliseconds at slide onset
   - Lockup Vibration only when braking past the limit
   
2. **Many effects are subtractive, not additive** — For example:
   - Understeer Effect **reduces** force (makes wheel lighter)
   - Effects like Gyro Damping **resist movement** rather than adding direction

3. **The "Decoupling Scale"** — The system divides all raw effects by `Max Torque Ref` before sending them to your wheel. This normalization step is what provides the headroom.

4. **Different frequency domains** — SoP/Lateral G is low-frequency (sustained force), while Lockup Vibration is high-frequency (rapid oscillation). They don't compete in the same way that two sustained forces would.

---

### 5. The "FFB Bucket" Philosophy — Is It Valid?

**Your analogy is absolutely valid and represents a more intuitive mental model!**

Your proposed approach:
> *"The FFB bucket size is the max physical torque of my wheel base (8 Nm for RS50). Each effect should show what percentage of that bucket it can fill."*

This is **closer to how irFFB and similar tools work** and is definitely more intuitive for end users.

---

## Analysis of Suggested UI Improvements

| Suggestion | Feasibility | Notes |
|------------|-------------|-------|
| **Input for wheel base max torque** | ✅ High | Easy to add; could replace or complement Max Torque Ref |
| **Master Gain as 0-100% (no attenuation)** | ⚠️ Medium | 100% = full output, not "no attenuation" — terminology might still confuse. Could be renamed "Output Scale" |
| **Min Force as 0-30%** | ✅ High | Already effectively this; just display formatting change |
| **Sliders show % of wheel max + Nm calculated** | ✅ High | Formula: `(effect_contribution / wheel_max_nm) × 100%` |
| **Sum of all effects with clipping indicator** | ✅ High | Very useful! Could show: `Total: 12.4 Nm / 8.0 Nm = 155% (High Clipping Risk)` |
| **Color coding (green/orange/red)** | ✅ High | Easy implementation; see existing clipping graph for precedent |
| **Effect priority system** | ⚠️ Medium-High | This is essentially a **Dynamic Range Compressor** or **Side-chain/Ducking** system. Complex but very valuable. |

---

## About Effect Priority (Advanced Topic)

Your example scenario:
> *"LMP2 in a high-speed corner touching curbs while the rear steps out. Instead of losing the understeer feel due to clipping, reduce the curb effect to free up space."*

This is actually a feature mentioned in our internal roadmap under **"Signal Masking Mitigation"**:

From `ffb_effects.md`:
> **Priority System**: Future versions should implement "Side-chaining" or "Ducking". For example, if a severe Lockup event occurs, reduce Road Texture gain to ensure the Lockup signal is clear.

**How it would work:**
1. Define priority levels for each effect (e.g., Grip Information > Slide Onset > Textures)
2. When the sum of effects approaches the wheel's max, compress lower-priority effects first
3. This maintains the "information content" of the most important feedback

This is absolutely realistic and is something we've been considering. Your feedback reinforces that this should be prioritized!

---

## Summary of Current System vs. Proposed "Bucket" System

| Aspect | Current System | Proposed "Bucket" System |
|--------|----------------|--------------------------|
| **Reference point** | Max Torque Ref (game physics headroom) | Wheel Base Max Torque (physical limit) |
| **Percentage meaning** | Gain multiplier (100% = 1.0×) | % of physical headroom used |
| **Displayed Nm** | Theoretical physics contribution | Actual expected output |
| **Clipping indicator** | Graphs panel only | Real-time sum with color coding |
| **Intuitive?** | Requires understanding FFB physics | Maps directly to hardware |

---

## Conclusion

**You're not wrong.** The current UI prioritizes *physics accuracy* (how much each effect contributes to the simulated steering feel) over *hardware awareness* (how much of your wheel's torque budget you're using).

For users who just want to "make it feel good without clipping," the bucket philosophy is more intuitive. For users tuning specific physics effects, the current system provides more granularity.

**Possible solution**: A **"Basic Mode"** toggle that:
1. Asks for wheel max torque
2. Displays all effects as % of that budget
3. Shows a real-time "total load" meter with clipping warning
4. Hides advanced physics parameters

This aligns with the planned "Basic Mode" mentioned in the README.

---

## Action Items for Development Team

1. [ ] Add "Wheel Base Max Torque" input (optional, for display calculation)
2. [ ] Add real-time "Total Estimated Load" readout with clipping indicator
3. [ ] Consider Basic Mode implementation prioritizing the "bucket" mental model
4. [ ] Evaluate priority/ducking system for effect compression
5. [ ] Improve tooltips to clarify the difference between "physics contribution" and "output torque"

---

*Thank you for this thoughtful feedback! Understanding how users conceptualize FFB helps us build a better tool for everyone.*
