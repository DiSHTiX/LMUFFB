
## Rear Align Torque, Lateral G Boost (Slide), and Yaw Kick

The interaction between "Lateral G Boost (Slide)" (formerly Oversteer Weight) and "Rear Align Torque" is crucial for a natural and intuitive oversteer feel.

### 1. The Interaction: "Heavy Counter-Steer"

When both effects are present during a slide, they combine to create a sensation of **"Heavy Counter-Steer."**

*   **Lateral G Boost (Slide):** This effect makes the wheel feel **heavier** (more resistance) as the car's body swings out. It's like feeling the inertia of the chassis through the steering.
*   **Rear Align Torque:** This effect provides a **directional pull** in the counter-steering direction. It's the wheel actively trying to straighten itself or align with the direction of travel.

**When they combine:**
Instead of the wheel going light and vague (which happens in some sims during a slide), it becomes **heavy and pulls strongly** in the direction you need to counter-steer.

**Are they confusing or well-blended?**
If tuned correctly, they are **well-blended and complementary**. They provide two distinct but synergistic pieces of information:

1.  **"I am sliding, and the car has a lot of momentum."** (Lateral G Boost)
2.  **"Turn the wheel THIS WAY to catch it."** (Rear Align Torque)

This combination is highly informative and is often praised in sims like Assetto Corsa for making slides "catchable."

### 2. Criteria for a Natural and Intuitive Blend

To make this blend feel natural, we need to consider the **magnitude, timing, and frequency** of each component.

#### A. Magnitude Balance (The "Volume Knob")
*   **Problem:** If one effect is too strong, it can mask the other.
    *   Too much **Lateral G Boost**: The wheel feels like a brick, and you can't feel the subtle directional pull of the Rear Align Torque.
    *   Too much **Rear Align Torque**: The wheel snaps violently, but it feels "light" or "digital" because it lacks the inertia of the chassis.
*   **Criteria:**
    *   **Rear Align Torque** should be strong enough to provide a clear, active counter-steering cue.
    *   **Lateral G Boost** should add a layer of "weight" or "inertia" on top, making the counter-steer feel substantial, but not so much that it becomes a struggle to turn the wheel.
*   **Tuning Goal:** The driver should feel the *direction* of the counter-steer (Rear Align) and the *effort* required to hold it (Lateral G Boost).

#### B. Timing (The "Predictive Cue")
*   **Problem:** If both effects kick in at the exact same time, they might feel like one undifferentiated "blob" of force.
*   **Criteria:**
    *   **Yaw Kick (already implemented):** This is the *earliest* cue. It's a sharp, momentary impulse that signals the *onset* of rotation.
    *   **Rear Align Torque:** Should build up very quickly after the Yaw Kick, as the slip angle develops. This is the active "pull."
    *   **Lateral G Boost (Slide):** Should build up slightly more gradually, reflecting the inertia of the car's mass swinging out. It's a sustained force that tells you about the *magnitude* of the slide.
*   **Tuning Goal:** A sequence of cues: **Kick (onset) $\to$ Pull (direction) $\to$ Weight (momentum)**.

#### C. Frequency (The "Texture")
*   **Problem:** If both effects use similar frequencies, they can interfere.
*   **Criteria:**
    *   **Rear Align Torque:** This is a **low-frequency, sustained force**. It's a steady pull, not a vibration.
    *   **Lateral G Boost (Slide):** This is also a **low-frequency, sustained force**. It's a steady increase in resistance.
    *   **Slide Texture (separate effect):** This is a **high-frequency vibration** (the "sandpaper" feel). This is crucial for adding texture without interfering with the directional forces.
*   **Tuning Goal:** Keep the directional forces (Lateral G, Rear Align) smooth and distinct from the high-frequency textures.

### 3. Tuning Recommendations for the User

To achieve a natural blend, users should:

1.  **Start with "Rear Align Torque" first:** Tune this until the counter-steering pull feels clear and responsive.
2.  **Then add "Lateral G Boost (Slide)":** Increase this gradually to add the sensation of chassis momentum without making the wheel too heavy to turn.
3.  **Use "Yaw Kick" for early warning:** This should be a sharp, short impulse at the very start of the slide.
4.  **Monitor "Clipping":** If the total force is clipping, reduce `Master Gain` or increase `Max Torque Ref` to ensure all these distinct forces have headroom.

By understanding these individual roles and their combined effect, the user can tune a highly informative and intuitive oversteer experience.


### Tuning Tips for Rear Align Torque, Lateral G Boost (Slide), and Yaw Kick

To achieve a natural blend, users should:

1.  **Start with "Rear Align Torque" first:** Tune this until the counter-steering pull feels clear and responsive.
2.  **Then add "Lateral G Boost (Slide)":** Increase this gradually to add the sensation of chassis momentum without making the wheel too heavy to turn.
3.  **Use "Yaw Kick" for early warning:** This should be a sharp, short impulse at the very start of the slide.
4.  **Monitor "Clipping":** If the total force is clipping, reduce `Master Gain` or increase `Max Torque Ref` to ensure all these distinct forces have headroom.

By understanding these individual roles and their combined effect, the user can tune a highly informative and intuitive oversteer experience.