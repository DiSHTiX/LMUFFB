


Here is the final, comprehensive implementation plan. It is divided into four distinct, modular stages. This allows you to implement, test, and verify each component independently without breaking the existing application.

---

### **Stage 1: Dynamic Normalization for Structural Forces**
**Objective:** Normalize the core steering rack and Seat-of-Pants (SoP) forces dynamically across all car classes. This stage introduces the **Leaky Integrator**, **Contextual Spike Rejection**, and **EMA Gain Smoothing**. It does *not* touch the UI or tactile textures.

**1. Add State Variables to `FFBEngine.h`:**
```cpp
// Inside FFBEngine class
double m_session_peak_torque = 25.0; 
double m_smoothed_structural_mult = 1.0 / 25.0;
double m_rolling_average_torque = 0.0;
double m_last_raw_torque = 0.0;
```

**2. Implement Peak Follower in `FFBEngine.cpp` (`calculate_force`):**
*Place this right after the telemetry variables are read, before effect calculations.*
```cpp
// 1. Contextual Spike Rejection (Lightweight MAD alternative)
double current_abs_torque = std::abs(raw_torque_input);
double alpha_slow = ctx.dt / (1.0 + ctx.dt); // 1-second rolling average
m_rolling_average_torque += alpha_slow * (current_abs_torque - m_rolling_average_torque);

double lat_g_abs = std::abs(data->mLocalAccel.x / 9.81);
double torque_slew = std::abs(raw_torque_input - m_last_raw_torque) / ctx.dt;
m_last_raw_torque = raw_torque_input;

// Flag as spike if torque jumps > 3x the rolling average, or exceeds physical slew/G limits
bool is_contextual_spike = current_abs_torque > (m_rolling_average_torque * 3.0);
bool is_clean_state = (lat_g_abs < 8.0) && (torque_slew < 1000.0) && !restricted && !is_contextual_spike;

// 2. Leaky Integrator (Exponential Decay + Floor)
if (is_clean_state && m_torque_source == 0) {
    if (current_abs_torque > m_session_peak_torque) {
        m_session_peak_torque = current_abs_torque; // Fast attack
    } else {
        // Exponential decay (e.g., 0.5% reduction per second)
        double decay_factor = 1.0 - (0.005 * ctx.dt); 
        m_session_peak_torque *= decay_factor; 
    }
    // Absolute safety floor and ceiling
    m_session_peak_torque = std::clamp(m_session_peak_torque, 15.0, 80.0);
}

// 3. EMA Filtering on the Gain Multiplier (Zero-latency physics)
double target_structural_mult = (m_torque_source == 1) ? 1.0 : (1.0 / m_session_peak_torque);
double alpha_gain = ctx.dt / (0.25 + ctx.dt); // 250ms smoothing
m_smoothed_structural_mult += alpha_gain * (target_structural_mult - m_smoothed_structural_mult);
```

**3. Apply to Structural Sum in `FFBEngine.cpp` (Summation Section):**
```cpp
// --- 6. SUMMATION ---
double structural_sum = output_force + ctx.sop_base_force + ctx.rear_torque + ctx.yaw_force + ctx.gyro_force + ctx.soft_lock_force;
structural_sum *= ctx.gain_reduction_factor;

// Apply smoothed multiplier ONLY to structural forces
double norm_structural = structural_sum * m_smoothed_structural_mult;

// Textures remain untouched for now
double texture_sum = ctx.road_noise + ctx.slide_noise + ctx.spin_rumble + ctx.bottoming_crunch + ctx.abs_pulse_force + ctx.lockup_rumble;
double norm_texture = texture_sum / max_torque_safe; // Legacy scaling

double total_sum = norm_structural + norm_texture;
```

---

### **Stage 2: Hardware Scaling Redefinition (UI & Config)**
**Objective:** Eliminate the confusing `m_max_torque_ref` (which caused the 100 Nm issue) and replace it with explicit hardware and target rim torque settings.

**1. Update `Config.h` (Preset Struct):**
```cpp
// Remove: float max_torque_ref = 100.0f;
// Add:
float wheelbase_max_nm = 15.0f; // Physical max of the user's wheelbase
float target_rim_nm = 10.0f;    // Desired peak force at the rim
```
*(Note: You will also need to update `Config.cpp` parsing/saving logic and `FFBEngine.h` variables to match these new names).*

**2. Update `GuiLayer_Common.cpp` (ImGui UI):**
```cpp
// Replace the old Max Torque Ref slider with:
FloatSetting("Wheelbase Max Torque", &engine.m_wheelbase_max_nm, 1.0f, 50.0f, "%.1f Nm", "The absolute maximum physical torque your wheelbase can produce (e.g., 15.0 for Simagic Alpha).");
FloatSetting("Target Rim Torque", &engine.m_target_rim_nm, 1.0f, 50.0f, "%.1f Nm", "The maximum force you want to feel in your hands during heavy cornering.");
```

**3. Update Final Output Scaling in `FFBEngine.cpp`:**
```cpp
// Update decoupling scale so textures scale with the user's desired rim torque
ctx.decoupling_scale = (double)m_target_rim_nm / 20.0;
if (ctx.decoupling_scale < 0.1) ctx.decoupling_scale = 0.1;

// ... (Summation happens here. Remove the old `norm_texture` division by max_torque_safe) ...
double total_normalized_signal = norm_structural + texture_sum;

// --- 7. OUTPUT SCALING ---
// Map the 0.0-1.0 normalized signal to the physical wheelbase
double hardware_mapping_ratio = (double)m_target_rim_nm / (double)m_wheelbase_max_nm;
double norm_force = total_normalized_signal * hardware_mapping_ratio * m_gain;
```

---

### **Stage 3: Tactile Haptics Normalization (Static Load + Soft-Knee)**
**Objective:** Anchor tactile effects (road texture, ABS) to the car's static weight, and use the **Giannoulis Soft-Knee** algorithm to prevent high-speed aero downforce from causing violent vibrations.

**1. Add State Variables to `FFBEngine.h`:**
```cpp
bool m_static_load_latched = false;
double m_smoothed_tactile_mult = 1.0;
```

**2. Update `update_static_load_reference` in `FFBEngine.cpp`:**
```cpp
void FFBEngine::update_static_load_reference(double current_load, double speed, double dt) {
    if (m_static_load_latched) return; // Do not update if latched

    if (speed > 2.0 && speed < 15.0) {
        if (m_static_front_load < 100.0) {
             m_static_front_load = current_load;
        } else {
             m_static_front_load += (dt / 5.0) * (current_load - m_static_front_load);
        }
    } else if (speed >= 15.0 && m_static_front_load > 1000.0) {
        // Latch the value once we exceed 15 m/s (aero begins to take over)
        m_static_load_latched = true;
    }
    
    if (m_static_front_load < 1000.0) m_static_front_load = 4500.0; // Safe fallback
}
```

**3. Implement Giannoulis Soft-Knee in `FFBEngine.cpp` (`calculate_force`):**
```cpp
// Replace the old texture_load_factor logic with:
double x = ctx.avg_load / m_static_front_load; // Input load multiplier
double T = 1.5;  // Threshold (Start compressing at 1.5x static weight)
double W = 0.5;  // Knee Width
double R = 4.0;  // Compression Ratio (4:1)

double lower_bound = T - (W / 2.0);
double upper_bound = T + (W / 2.0);
double compressed_load_factor = x;

if (x > upper_bound) {
    // Linear compressed region
    compressed_load_factor = T + ((x - T) / R);
} else if (x > lower_bound) {
    // Giannoulis quadratic soft-knee transition
    double diff = x - lower_bound;
    compressed_load_factor = x + (((1.0 / R) - 1.0) * (diff * diff)) / (2.0 * W);
}

// EMA Smoothing on the tactile multiplier (100ms)
double alpha_tactile = ctx.dt / (0.1 + ctx.dt);
m_smoothed_tactile_mult += alpha_tactile * (compressed_load_factor - m_smoothed_tactile_mult);

ctx.texture_load_factor = m_smoothed_tactile_mult;
```

---

### **Stage 4: Persistent Storage of Static Load**
**Objective:** Save the learned static load per vehicle to `config.ini` so the user doesn't have to drive between 2-15 m/s every time they leave the garage.

**1. Add Map to `Config.h`:**
```cpp
class Config {
public:
    // ... existing code ...
    static std::map<std::string, double> m_saved_static_loads;
};
```

**2. Update `Config.cpp` (Load/Save Logic):**
```cpp
// In Config::Save()
file << "\n\n";
for (const auto& pair : m_saved_static_loads) {
    file << pair.first << "=" << pair.second << "\n";
}

// In Config::Load()
// Add parsing logic for the section to populate m_saved_static_loads
```

**3. Update `InitializeLoadReference` in `FFBEngine.cpp`:**
```cpp
void FFBEngine::InitializeLoadReference(const char* className, const char* vehicleName) {
    std::string vName = vehicleName ? vehicleName : "Unknown";
    
    // Check if we already have a saved static load for this specific car
    auto it = Config::m_saved_static_loads.find(vName);
    if (it != Config::m_saved_static_loads.end()) {
        m_static_front_load = it->second;
        m_static_load_latched = true; // Skip the 2-15 m/s learning phase
        std::cout << " Loaded persistent static load for " << vName << ": " << m_static_front_load << "N\n";
    } else {
        // Fallback to class defaults and require learning
        ParsedVehicleClass vclass = ParseVehicleClass(className, vehicleName);
        m_static_front_load = GetDefaultLoadForClass(vclass) * 0.5;
        m_static_load_latched = false; 
    }
}
```

**4. Save the Latched Value in `FFBEngine.cpp`:**
```cpp
// Inside update_static_load_reference, right after setting m_static_load_latched = true:
if (speed >= 15.0 && m_static_front_load > 1000.0 && !m_static_load_latched) {
    m_static_load_latched = true;
    
    // Save to config map
    std::string vName = m_vehicle_name;
    Config::m_saved_static_loads = m_static_front_load;
    
    // Trigger a background save of the config file
    // (Assuming you have a safe way to call Config::Save without blocking the FFB thread, 
    // otherwise flag it for the GUI thread to save on the next frame).
}
```