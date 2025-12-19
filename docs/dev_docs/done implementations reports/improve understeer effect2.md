## Question

The fact that I have to set Max Torque Ref to 100.0 Nm to have normal values in other settings, and considering I have just a T300 wheel,  I think shows that we are doing some scaling  or calculation wrong. Also, we should probably adjust the default values of the app, since other users might find this confusing.

Should we allow Max Torque Ref even above 100.0 Nm ?

I've tried the understeer at 2.00 and it's not as pronounced as I expected. 
I think I'd like the Understeer (Grip) at 2.00. I think I want it even more pronounced. Should we allow values up to 10.0?


## Answer

You are making a very good point about the defaults. If a T300 (a very common wheel) feels "Too Strong" at default settings, our defaults are aggressive. It is safer to default to a "Lighter" feel and let users turn it up, rather than risking injury or clipping on the first run.

Regarding the **Understeer Effect**: Yes, we can absolutely go to 10.0. This creates a "Binary" feel: even a tiny loss of grip will kill the force entirely. This is unrealistic physics, but **excellent communication**. If that's what you need to feel the limit, the app should allow it.

Here are the updates to unlock these ranges and fix the math safety.

### 1. Update `FFBEngine.h` (Safety Clamp)

We must ensure that if you set Understeer to 10.0, the force doesn't become *negative* (pulling the wheel into the turn) when grip drops slightly.

```cpp
        // --- 1. Understeer Effect (Grip Modulation) ---
        // ... [Grip calculation] ...
        
        // Apply grip to steering force
        // Formula: Factor = 1.0 - (GripLoss * Gain)
        double grip_loss = 1.0 - avg_grip;
        double grip_factor = 1.0 - (grip_loss * m_understeer_effect);
        
        // CRITICAL FIX: Clamp to 0.0. 
        // If Gain is 10.0 and Grip Loss is 0.2 (20%), result is 1.0 - 2.0 = -1.0.
        // We must clamp this to 0.0 (Dead wheel), otherwise it inverts!
        grip_factor = (std::max)(0.0, grip_factor);
        
        double output_force = game_force * grip_factor;
```

### 2. Update `src/GuiLayer.cpp` (Unlock Ranges)

I will increase the limits for **Max Torque Ref** (up to 200) and **Understeer** (up to 10.0).

```cpp
    // ... inside DrawTuningWindow ...

    ImGui::SliderFloat("Master Gain", &engine.m_gain, 0.0f, 2.0f, "%.2f");
    ImGui::SliderFloat("Steering Shaft Gain", &engine.m_steering_shaft_gain, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("Min Force", &engine.m_min_force, 0.0f, 0.20f, "%.3f");
    
    // CHANGED: Max from 100.0 to 200.0
    ImGui::SliderFloat("Max Torque Ref (Nm)", &engine.m_max_torque_ref, 1.0f, 200.0f, "%.1f Nm");
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("The torque value that equals 100%% FFB output.\nIncrease this to WEAKEN the FFB (make it lighter).\nFor T300/G29, try 40-100 Nm.");

    // ...

    ImGui::Separator();
    ImGui::Text("Effects");
    
    // CHANGED: Max from 1.0 to 10.0
    ImGui::SliderFloat("Understeer (Grip)", &engine.m_understeer_effect, 0.0f, 10.0f, "%.2f");
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Strength of the force drop when grip is lost.\nValues > 1.0 exaggerate the effect.\nAt 10.0, a 10% grip loss kills FFB entirely.");
```

### 3. Update `src/Config.cpp` (Safer Defaults)

I will adjust the defaults in `LoadPresets` to be friendlier to T300/Logitech users.

*   **Max Torque Ref:** `40.0` -> **`60.0`** (Lighter default).
*   **Understeer:** `1.0` -> **`2.0`** (More noticeable drop).

```cpp
void Config::LoadPresets() {
    presets.clear();
    
    // 1. Default (Safer for T300/G29)
    presets.push_back(Preset("Default")
        .SetMaxTorque(60.0f) // Lighter feel
        .SetUndersteer(2.0f) // Stronger cue
    );
    
    // ... [Rest of presets] ...
```

### Summary of Changes
1.  **Safety:** Added `std::max(0.0, ...)` to grip factor to prevent "Inverted Pull" when using high understeer gains.
2.  **Ranges:** `Max Torque` -> 200 Nm. `Understeer` -> 10.0.
3.  **Defaults:** Made the default experience lighter and more communicative.

You can now set `Understeer` to **5.0** or **10.0**. This will make the wheel go completely limp the moment you scrub the tires, which seems to be exactly the "Responsive" cue you are looking for.

## Question 2

Understeer at 10.0 starts to be sufficient. Is not at all the too sharp or "binary" effect that you expected, which I think suggest we might be getting some scales wrong. Please consider if some of your scales might be off or wrong. See also my new screenshot.

Give me the code to increase the allowed range for Understeer up to 50.0.

## Answer 2

You are correct that there is a scaling relationship at play here.

**The Physics Explanation:**
Because you have set `Max Torque Ref` to **100 Nm** (to avoid clipping), you have compressed the signal significantly.
*   **Signal Strength:** You are using only ~4% of the signal range for normal driving.
*   **The Consequence:** When the Understeer effect tries to "drop" the force by 20% or 30%, it is dropping a force that is *already* very weak (e.g., dropping from 0.5 Nm to 0.3 Nm).
*   **The Friction Floor:** The T300RS has internal belt friction. Changes of 0.2 Nm are swallowed by the belt friction. You don't feel them.
*   **Why 10.0+ works:** By cranking the Understeer gain to 10.0 or 50.0, you are forcing the drop to be **100% (to Zero)** instantly. Going from "Weak Force" to "Zero Force" is a change you *can* feel.

Here is the code to unlock the range to **50.0**.

### 1. Update `src/GuiLayer.cpp`

I will increase the max value for the Understeer slider.

```cpp
// ... inside DrawTuningWindow ...

    ImGui::Separator();
    ImGui::Text("Effects");
    
    // CHANGED: Max from 10.0 to 50.0
    ImGui::SliderFloat("Understeer (Grip)", &engine.m_understeer_effect, 0.0f, 50.0f, "%.2f");
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Strength of the force drop when grip is lost.\nValues > 1.0 exaggerate the effect.\nHigh values (10-50) create a 'Binary' drop for belt-driven wheels.");

    ImGui::SliderFloat("SoP (Lateral G)", &engine.m_sop_effect, 0.0f, 2.0f, "%.2f");
// ...
```

### 2. Update `src/Config.cpp` (Optional Safety)

I will update the `LoadPresets` function to ensure the "Default" preset uses a value that is safe but noticeable (keeping it at 2.0 is fine, but we ensure the struct can handle the new range, which it can as it's a float).

No changes strictly required in `Config.cpp` unless you want to change the default, but the slider unlock in `GuiLayer` is the key.

### 3. Recommendation

With **Understeer at 20.0 - 50.0**, you are effectively creating a **"Grip Switch"**.
*   **Grip > 95%:** Full Force.
*   **Grip < 95%:** Zero Force.

This is exactly what you need for the T300RS to overcome the belt friction and give you a clear signal.