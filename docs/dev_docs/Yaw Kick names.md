Here are the best options for naming this effect in the GUI, keeping in mind that it needs to be short enough for a label but descriptive enough for a user to understand what it feels like.

### Top Recommendation
**"SoP Yaw (Kick)"**

*   **Why it works:**
    *   **Context:** It groups the effect under "SoP" (Seat of Pants), so the user knows it relates to chassis movement, not steering rack forces.
    *   **Physics:** It identifies "Yaw" (rotation) as the source.
    *   **Sensation:** It uses "(Kick)" to describe the tactile feeling. Unlike Lateral G (which is a sustained weight), Acceleration is felt as a sudden impulse or kick.

---

### Alternative Options

Here is a breakdown by naming philosophy:

#### 1. Descriptive (Focus on the Sensation)
*   **Yaw Kick** (Short, punchy, accurate)
*   **Rotation Snap** (Describes the sudden movement of the car)
*   **Yaw Impulse** (More technical, implies a short burst of force)
*   **Rear Jolt** (Focuses on the rear axle behavior)

#### 2. Functional (Focus on the Purpose)
*   **Slide Cue** (Tells the user *why* they need it: to detect a slide)
*   **Oversteer Cue** (Specific to oversteer initiation)
*   **Slide Onset** (Implies it happens at the *start* of the slide)
*   **Rotation Cue** (Simple and direct)

#### 3. Technical (Focus on the Data)
*   **Yaw Acceleration** (Accurate, but perhaps too long for a GUI column)
*   **Rotational Accel**
*   **Yaw Accel Gain**

### Recommendation for the Prompt
I suggest using **"SoP Yaw (Kick)"** for the GUI label. It balances technical accuracy with the visceral description of the effect.

In the code variables, use `m_sop_yaw_gain` to keep it consistent with the existing `m_sop_effect`.