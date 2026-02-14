Here is the analysis of the current implementation and a plan to implement a **"Shadow Mode"**—running estimators in the background to compare them against real telemetry.

---

### 1. Codebase Analysis: Current Usage of Grip & Load

I have reviewed `src/FFBEngine.h` to see how `mGripFract` and `mTireLoad` are handled when valid data is present.

#### A. Tire Grip (`mGripFract`)
**Current Logic:**
In `calculate_grip`, the code prioritizes real data:
```cpp
// src/FFBEngine.h
result.original = (w1.mGripFract + w2.mGripFract) / 2.0;
result.value = result.original;

// Fallback condition only triggers if value is near zero
if (result.value < 0.0001 && avg_load > 100.0) {
    // ... Run Estimators ...
}
```
**Critique:**
1.  **Logic is Sound:** It correctly prefers real data.
2.  **Linear Mapping:** In `calculate_force`, the grip is applied linearly:
    ```cpp
    double grip_loss = (1.0 - ctx.avg_grip) * m_understeer_effect;
    ctx.grip_factor = (std::max)(0.0, 1.0 - grip_loss);
    ```
    *Potential Improvement:* Real `mGripFract` from rFactor 2 engines is often noisy and might not drop linearly with "feeling." We might need a **Gamma curve** or a **Threshold** (e.g., ignore grip loss until it drops below 0.9) to make the real data feel as good as the tuned estimators.

#### B. Tire Load (`mTireLoad`)
**Current Logic (in app version 0.7.42, note that load normalization logic was changed after that):**
In `calculate_force`:
```cpp
// src/FFBEngine.h
double raw_load = (fl.mTireLoad + fr.mTireLoad) / 2.0;
// ...
double raw_load_factor = ctx.avg_load / 4000.0; // <--- MAGIC NUMBER
```
**Critique:**
1.  **The "4000.0" Constant:** The code normalizes load by dividing by `4000.0` (Newtons). This assumes a generic GT3 car (~400kg per corner static + aero).
    *   **Issue:** Hypercars generate massive downforce. At speed, `mTireLoad` might reach 8000N-10000N. This results in a `raw_load_factor` of 2.0 or 2.5, which is then clamped by `m_texture_load_cap` (default 1.5).
    *   **Result:** Road texture details might be artificially crushed/clipped at high speeds in the Hypercar because the baseline `4000.0` is too low for this vehicle class.

---

### 2. Proposed Changes: Implementing "Shadow Mode"

To validate the formulas, we need to calculate the *Estimated* values even when *Real* values are present, and log both.

#### Step 1: Modify `FFBEngine.h` to store Shadow Data

We need to add members to store the estimated values for logging purposes, and update the calculation functions to populate them unconditionally.

**In `src/FFBEngine.h` (Class Definition):**
```cpp
public:
    // ... existing members ...
    
    // SHADOW TELEMETRY (For Validation)
    double m_shadow_grip_slope = 0.0;      // What Slope Detection WOULD have output
    double m_shadow_grip_friction = 0.0;   // What Friction Circle WOULD have output
    double m_shadow_load_kinematic = 0.0;  // What Kinematic Model WOULD have output
```

**In `src/FFBEngine.h` (Inside `calculate_grip`):**
```cpp
// ... inside calculate_grip ...

// 1. Always calculate the "Shadow" estimates for logging
if (m_slope_detection_enabled && is_front && data) {
    // Calculate but don't assign to result.value yet
    m_shadow_grip_slope = calculate_slope_grip(
        data->mLocalAccel.x / 9.81, 
        result.slip_angle, 
        dt, 
        data
    );
}

// Calculate Friction Circle estimate (Legacy fallback)
// ... (copy logic from the else block) ...
double lat_metric = std::abs(result.slip_angle) / (double)m_optimal_slip_angle;
// ... calculation ...
m_shadow_grip_friction = 1.0 / (1.0 + excess * 2.0); 

// 2. Existing Logic (Keep as is)
if (result.value < 0.0001 && avg_load > 100.0) {
    // Use the estimates calculated above
    result.value = (m_slope_detection_enabled && is_front) ? m_shadow_grip_slope : m_shadow_grip_friction;
    result.approximated = true;
}
```

**In `src/FFBEngine.h` (Inside `calculate_force` - Load Section):**
```cpp
// ... inside calculate_force ...

// Always calculate Kinematic Load for comparison
double kin_load_fl = calculate_kinematic_load(data, 0);
double kin_load_fr = calculate_kinematic_load(data, 1);
m_shadow_load_kinematic = (kin_load_fl + kin_load_fr) / 2.0;

// Existing Fallback Logic
if (m_missing_load_frames > 20) {
    // ... use kinematic ...
}
```

#### Step 2: Expand `AsyncLogger.h`

Add columns to the CSV to record the shadow values alongside the real values.

**In `src/AsyncLogger.h` (`LogFrame` struct):**
```cpp
struct LogFrame {
    // ... existing ...
    float grip_fl;      // Real Game Grip
    float load_fl;      // Real Game Load
    
    // NEW VALIDATION COLUMNS
    float shadow_grip_slope;
    float shadow_grip_friction;
    float shadow_load_kinematic;
    // ...
};
```

**In `src/AsyncLogger.h` (`WriteFrame`):**
```cpp
// Add to CSV output
<< frame.shadow_grip_slope << "," 
<< frame.shadow_grip_friction << "," 
<< frame.shadow_load_kinematic << ","
```

---

### 3. Log Analyzer Expansion

We need to update the Python tool to visualize the correlation between the Real data (for grip and load) and our Estimates.

**New Analyzer Feature: `validation_plots.py`**

```python
def plot_validation_analysis(df, output_dir):
    """
    Compares Real Telemetry vs lmuFFB Estimators.
    Only useful for cars that output real data for tire grip and load.
    """
    
    # 1. Grip Validation (Real vs Slope Detection)
    plt.figure(figsize=(12, 6))
    plt.plot(df['Time'], df['GripFL'], label='Real Game Grip (FL)', color='green', alpha=0.6)
    plt.plot(df['Time'], df['ShadowGripSlope'], label='Slope Estimator', color='red', alpha=0.6, linestyle='--')
    plt.title("Validation: Real Grip vs Slope Detection")
    plt.ylabel("Grip Fraction")
    plt.legend()
    plt.savefig(f"{output_dir}/valid_grip_slope.png")
    
    # 2. Load Validation (Real vs Kinematic)
    plt.figure(figsize=(12, 6))
    plt.plot(df['Time'], df['LoadFL'], label='Real Tire Load (FL)', color='blue', alpha=0.6)
    plt.plot(df['Time'], df['ShadowLoadKinematic'], label='Kinematic Estimator', color='orange', alpha=0.6, linestyle='--')
    plt.title("Validation: Real Load vs Kinematic Model")
    plt.ylabel("Load (N)")
    plt.legend()
    plt.savefig(f"{output_dir}/valid_load_kinematic.png")
    
    # 3. Correlation Scatter
    plt.figure(figsize=(8, 8))
    plt.scatter(df['GripFL'], df['ShadowGripSlope'], alpha=0.05, s=1)
    plt.xlabel("Real Grip")
    plt.ylabel("Estimated Grip")
    plt.plot([0, 1], [0, 1], 'r--') # Perfect correlation line
    plt.title("Correlation: Real vs Estimated")
    plt.savefig(f"{output_dir}/valid_correlation.png")
```

### 4. Testing Strategy

Once the code changes are compiled:

1.  **Drive the a car that outputs real tire grip and load data:**
    *   **Test A (Understeer):** Induce steady-state understeer in a long corner (e.g., constant radius).
        *   *Check:* Does `ShadowGripSlope` drop at the same time `GripFL` drops?
        *   *Check:* Does `ShadowGripSlope` stay low, or does it recover too early (indicating the "Hold Timer" needs tuning)?
    *   **Test B (Curbs):** Hit curbs aggressively.
        *   *Check:* Does `LoadFL` spike? Does `ShadowLoadKinematic` spike similarly? If Kinematic is too smooth, we might need to blend in vertical acceleration.
    *   **Test C (Braking):** Heavy braking.
        *   *Check:* Does `ShadowLoadKinematic` correctly predict the weight transfer to the front? If not, `m_approx_weight_bias` or `m_approx_mass_kg` in `FFBEngine.h` needs tuning.

2.  **Analyze the Logs:**
    *   Use the new plots to tune `m_slope_sensitivity` and `m_slope_sg_window` until the Red line (Estimate) matches the Green line (Real) as closely as possible.
    *   Once tuned on the reference car (from real tire grip and load data), these settings become the "Gold Standard" defaults for cars that *don't* output telemetry.

### 5. Immediate Action Items

1.  **Update `src/FFBEngine.h`**: Implement the "Shadow Mode" logic described in Step 1.
2.  **Update `src/AsyncLogger.h`**: Add the shadow columns.
3.  **Compile & Distribute**: Create a build for testers.
4.  **Data Collection**: Gather logs from users driving cars that output real tire grip and load data to perform the correlation analysis.