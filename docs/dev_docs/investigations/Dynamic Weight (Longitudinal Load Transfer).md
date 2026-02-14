### Recommended Effects using Real Data

#### A. Dynamic Weight (Longitudinal Load Transfer) - *High Priority*
You previously investigated using Longitudinal G. However, using **Real Tire Load** is vastly superior for Hypercars because it accounts for **Aerodynamics**.
*   **The Physics:** A Hypercar braking at 300 km/h has massive downforce *plus* weight transfer. The front tires are crushed into the track. The steering should feel incredibly heavy. As speed bleeds off and downforce vanishes, the wheel lightens.
*   **The Formula:** Scale the `Master Gain` based on the ratio of *Current Front Load* to *Static Front Load*.
*   **Benefit:** Provides a natural "sense of speed" and braking confidence that G-force alone cannot simulate (because G-force ignores Aero load).


### Implementation Plan

Here is how to modify `FFBEngine.h` to implement these features.

#### Step 1: Add Configuration
Add these to `src/Config.h` (and update `Config.cpp` to parse them):
```cpp
// In Preset struct
float dynamic_weight_gain = 1.0f; // 0.0 = Off, 1.0 = 1:1 Load Scaling
bool use_real_grip_if_available = true;
```

#### Step 2: Modify `FFBEngine.h`

We need to update `calculate_force` to detect the data and apply the new logic.

**In `src/FFBEngine.h`:**

```cpp
// Add this helper to calculate the static load reference dynamically
// We use a running average of load when the car is moving slowly (minimal aero/transfer)
void update_static_load_reference(const TelemWheelV01& fl, const TelemWheelV01& fr, double speed, double dt) {
    // Only update "static" reference at low speeds (e.g., pit limiter speed) 
    // to capture mechanical weight without aero.
    if (speed > 2.0 && speed < 15.0) { 
        double current_load = (fl.mTireLoad + fr.mTireLoad) / 2.0;
        // Very slow LPF to find the baseline
        m_static_front_load += (dt / 5.0) * (current_load - m_static_front_load);
    }
    // Safety fallback if uninitialized
    if (m_static_front_load < 1000.0) m_static_front_load = 4000.0; 
}

// Class member to store static load
double m_static_front_load = 0.0; 
```

**Update `calculate_force` logic:**

```cpp
// Inside FFBEngine::calculate_force(...)

// ... [Existing Context Initialization] ...

// 1. DETECT DATA AVAILABILITY
// Check if we have real load data (Hypercars) or need fallback (GT3/LMP2)
bool has_real_data = (fl.mTireLoad > 100.0 && fl.mGripFract > 0.0001);

// Update static reference for Dynamic Weight
update_static_load_reference(fl, fr, ctx.car_speed, ctx.dt);

// ... [Existing Stats Updates] ...

// --- NEW FEATURE: DYNAMIC WEIGHT (Longitudinal Load) ---
double dynamic_weight_factor = 1.0;

if (m_dynamic_weight_gain > 0.0 && has_real_data) {
    // Calculate Load Ratio: Current / Static
    // Example: Braking at high speed -> 12000N / 4000N = 3.0x weight
    // Example: Accelerating -> 2000N / 4000N = 0.5x weight
    double current_front_load = (fl.mTireLoad + fr.mTireLoad) / 2.0;
    double load_ratio = current_front_load / m_static_front_load;
    
    // Apply Gain: Blend between 1.0 and the Load Ratio
    // If gain is 0.5, we get 50% of the physical effect
    dynamic_weight_factor = 1.0 + (load_ratio - 1.0) * m_dynamic_weight_gain;
    
    // Clamp to sane limits (e.g., don't let it go below 20% or above 300%)
    dynamic_weight_factor = std::clamp(dynamic_weight_factor, 0.2, 3.0);
}

// --- MODIFIED: GRIP CALCULATION ---
if (has_real_data && m_use_real_grip_if_available) {
    // DIRECT PHYSICS PATH (Hypercars)
    
    // 1. Front Grip (Understeer)
    // mGripFract: 0.0 = Sliding, 1.0 = Grip (Assuming standard rF2 normalization)
    // Note: Verify if LMU uses 1.0=Slide. Usually rF2 is "Fraction of Grip Remaining".
    // Let's assume 1.0 = Grip based on your previous code comments.
    
    double raw_grip_fl = fl.mGripFract;
    double raw_grip_fr = fr.mGripFract;
    
    // Load-weighted average for front grip
    // We care more about the loaded tire (outside) than the unloaded one
    double total_f_load = fl.mTireLoad + fr.mTireLoad + 1.0;
    ctx.avg_grip = (raw_grip_fl * fl.mTireLoad + raw_grip_fr * fr.mTireLoad) / total_f_load;
    
    // 2. Rear Grip (Oversteer/SoP)
    const auto& rl = data->mWheel[2];
    const auto& rr = data->mWheel[3];
    double total_r_load = rl.mTireLoad + rr.mTireLoad + 1.0;
    
    // Load-weighted rear grip
    ctx.avg_rear_grip = (rl.mGripFract * rl.mTireLoad + rr.mGripFract * rr.mTireLoad) / total_r_load;
    
    // Disable Slope Detection flags for this frame since we have real data
    // (This prevents the slope logic from overriding our real values)
    // You might need to refactor calculate_grip to accept a "force_real" flag
} else {
    // ESTIMATION PATH (Legacy/Encrypted)
    // ... [Your existing calculate_grip / Slope Detection logic] ...
}

// ... [Existing Signal Conditioning] ...

// Apply Dynamic Weight to Base Force
// We apply this BEFORE the grip cut, so the "heaviness" is felt, 
// but understeer can still cut it away.
double output_force = (base_input * m_steering_shaft_gain) * dynamic_weight_factor;

// Apply Grip Cut (Understeer)
// If using real data, ctx.avg_grip is now the real mGripFract
double grip_loss = (1.0 - ctx.avg_grip) * m_understeer_effect;
ctx.grip_factor = std::max(0.0, 1.0 - grip_loss);

output_force *= ctx.grip_factor;

// ... [Rest of pipeline] ...
```

### 4. Addressing your Questions

**Q: Is grip a "leading indicator" for oversteer?**
**A: Yes, but with nuance.**
1.  **Steering Torque (Pneumatic Trail):** This is the *fastest* indicator. The self-aligning torque drops *before* the tire reaches peak slip.
2.  **Grip Fraction (`mGripFract`):** This is the *second* fastest. It tells you the contact patch is saturating. It happens *as* the slide begins, but before the car physically rotates significantly.
3.  **Yaw/Lateral G:** These are *lagging* indicators. The car has already rotated by the time these forces peak.

**Recommendation:**
For the best Oversteer feel, blend **Real Rear Grip** (Leading) with **Yaw Acceleration** (Instant Kick).
*   Use `mGripFract` (Rear) to modulate the "Sustained" SoP force.
*   Keep your existing "Yaw Kick" (derived from `mLocalRotAccel`) for the initial "snap".

**Q: Is Lateral Load Transfer beneficial?**
**A: Yes, for "Limit Feel".**
You can calculate a **"Chassis Roll Factor"**:
```cpp
double roll_factor = std::abs(fl.mTireLoad - fr.mTireLoad) / (fl.mTireLoad + fr.mTireLoad + 1.0);
```
*   `roll_factor` approaches 1.0 when the inside wheel lifts (100% transfer).
*   You can map this to a subtle vibration or a stiffening of the SoP force to tell the driver "You are leaning on the tire to the maximum."

### Summary of Changes
1.  **Dynamic Weight:** Use `mTireLoad` ratio to scale Master Gain. (Heavier braking, lighter accel).
2.  **Hybrid Grip:** Use `mGripFract` directly for Hypercars, fallback to Slope for others.
3.  **Weighted Rear Grip:** Use `mTireLoad` to weight the rear grip calculation, ensuring you feel the *loaded* tire sliding, not the inside wheel spinning.