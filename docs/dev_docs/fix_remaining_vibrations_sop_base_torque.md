# Investigation & Implementation Plan: Fixing Remaining Low Speed Vibrations (SoP & Base Torque)

## 1. Issue Analysis

**Reported Issue:**
Despite the introduction of the "Stationary Vibration Gate" (Speed Gate) in v0.6.23, users report remaining vibrations when the car is stationary or moving at very low speeds (< 5 km/h).

Reference documents for the previous implementation:
* docs\dev_docs\fix vibrations from  Clutch Bite, Low RPM.md
* docs\dev_docs\Fix Violent Shaking when Stopping and no road textures.md

**Root Cause:**
*   The current `speed_gate` logic (fading out forces below a speed threshold) is currently ONLY applied to **AC (Vibration) Effects**:
    *   Road Texture
    *   ABS Pulse
    *   Lockup Vibration
    *   Bottoming Crunch
*   The **DC (Physics) Effects** were intentionally left active to preserve steering weight. However, at idle, the game physics engine often outputs noisy telemetry for these channels:
    *   **Base Torque**: The steering shaft torque fluctuates due to engine vibration.
    *   **SoP (Seat of Pants)**: Lateral G and Yaw Acceleration sensors pick up chassis vibration (resonance).
    *   **Rear Aligning Torque**: The derived rear slip angle fluctuates due to noisy lateral velocity at near-zero speeds.

**Conclusion:**
The `speed_gate` scalar must be applied to the **Base Torque** and **SoP / Rear Axle** sections to ensure a completely "calm" wheel at standstill.

---

## 2. Implementation Plan

We will update `src/FFBEngine.h` to apply the `speed_gate` scalar to the final `output_force` (Base Torque) and `sop_total` (SoP Effects).

### Change 1: Apply Speed Gate to Base Torque

Currently, Base Torque uses "Idle Smoothing" (increasing LPF) but does not reduce amplitude. We will now apply the mute gate.

**Location:** `src/FFBEngine.h` inside `calculate_force()`

```cpp
// ... existing code ...

// Apply Gain and Grip Modulation
double output_force = (base_input * (double)m_steering_shaft_gain) * grip_factor;

// [NEW] Apply Speed Gate to Base Torque
// This eliminates "Engine Rumble" from the steering shaft at standstill
output_force *= speed_gate;

// ...
```

### Change 2: Apply Speed Gate to SoP / Rear Axle

The SoP section calculates forces from Lateral G, Rear Aligning Torque, and Yaw Kick. We will apply the gate to the summed result.

**Location:** `src/FFBEngine.h` inside `calculate_force()`

```cpp
// ... inside SoP calculation block ...

double sop_total = sop_base_force; // Lateral G contribution

// ... [Rear Aligning Torque calculation] ...
sop_total += rear_torque;

// ... [Yaw Kick calculation] ...
sop_total += yaw_force;

// [NEW] Apply Speed Gate to SoP
// This eliminates vibration from noisy Lateral G and Yaw Accel sensors at idle
sop_total *= speed_gate;

double total_force = output_force + sop_total;
```

---

## 3. Impact Assessment

*   **Parking / Pits (0-5 km/h):** The wheel will be completely silent and still. Users will lose "static steering weight" at exactly 0 km/h, but this is a worthy trade-off to eliminate the violent shaking. As soon as the car moves ( > 1.0 m/s or 3.6 km/h), the weight will smoothly fade in.
*   **Driving (> 18 km/h):** No change. `speed_gate` is 1.0, so full physics are passed through.
*   **Min Force:** Since `total_force` will be completely muted (0.0) at standstill, the `Min Force` boost logic (which checks `abs > 0.0001`) will arguably not trigger, preventing the "magnetic centering" buzz.

## 4. Verification

We need to verify that `total_force` is effectively 0.0 when `car_speed` is 0, regardless of engine vibration noise in inputs.

**Test Case:** `tests/test_ffb_engine.cpp`

```cpp
static void test_stationary_silence() {
    // Setup engine with defaults (Gate: 1.0m/s to 5.0m/s)
    FFBEngine engine;
    InitializeEngine(engine);
    
    TelemInfoV01 data = CreateBasicTestTelemetry(0.0); // 0 Speed
    
    // Inject Noise into Physics Channels
    data.mSteeringShaftTorque = 5.0; // Heavy engine vibration
    data.mLocalAccel.x = 2.0;        // Lateral shake
    data.mLocalRotAccel.y = 10.0;    // Yaw rotation noise
    
    double force = engine.calculate_force(&data);
    
    // Expect 0.0 because speed_gate should be 0.0
    if (std::abs(force) < 0.001) {
        std::cout << "[PASS] Stationary Silence: Forces muted at 0 speed." << std::endl;
    } else {
        std::cout << "[FAIL] Stationary Silence: Force leaked! " << force << std::endl;
    }
}
```
