# Investigation Report: Slope Detection Issues Post v0.7.1

## Executive Summary

This investigation addresses the persistent issues with the **Slope Detection** feature for grip estimation in lmuFFB, following the fixes implemented in v0.7.1. Despite the improvements (reduced sensitivity, automatic disabling of Lateral G Boost, tooltip additions), users continue to report that the feature is not working as expected. This report analyzes the root causes, proposes fixes, and specifies a comprehensive **Telemetry Logging and Analysis System** to diagnose this and similar FFB effect issues in the future.

**Report Date:** 2026-02-03  
**Author:** lmuFFB Investigation Team  
**Related Issues:** GitHub Issue #25, v0.7.0 Investigation Report  
**Status:** Active Investigation

---

## Table of Contents

1. [User Feedback Summary (Post v0.7.1)](#1-user-feedback-summary-post-v071)
2. [Root Cause Analysis](#2-root-cause-analysis)
3. [Proposed Fixes](#3-proposed-fixes)
4. [Telemetry Logging Feature Specification](#4-telemetry-logging-feature-specification)
5. [Log Analyzer Specification](#5-log-analyzer-specification)
6. [Slope Detection Diagnostic Requirements](#6-slope-detection-diagnostic-requirements)
7. [Appendix: Code References](#appendix-code-references)

---

## 1. User Feedback Summary (Post v0.7.1)

### 1.1 Issues That Persist After v0.7.1 Fixes

Despite the fixes implemented in v0.7.1 (automatic disabling of Lateral G Boost when Slope Detection is ON, reduced default sensitivity, smoother transitions), users are still reporting:

1. **Understeer Effect Not Working:**
   > "I enable Slope detection and turn understeer effect back up to 50% and it still just reduces overall force by that amount, at ALL times. There is simply no understeer effect present."
   > "It somehow feels like the understeer effect is constantly running at 100% understeer. That's why my FFB nearly disappears completely when putting the effect to 100% reduction, even on straights"

2. **dG/dAlpha Oscillating to Extremes:**
   > Looking at the graphs, dG/dAlpha seems to oscillate from max to min, with no intermediate values. This is despite having Filter Window maxed out (41), and sensitivity down to 0.5x. (See attached image in issue)

3. **Car-Specific Issues (McLaren):**
   > "Tested on the McLaren and it just doesn't work (slope detection). Pulling the wheel all over the place, so can't recommend the above settings anymore."
   > "The FFB for the McLaren feels very notchy with strong jolts that make no sense."

4. **Some Success with Certain Cars:**
   > "The new slope effect is great overall, it's been fine in the Merc, Porsche and Lexus so far. Just seems to be a weird issue with the McLaren while using it."
   > "I've tried it (slope) on the LMP2 and prefer it off for that."

### 1.2 Key Observations

| Issue | Symptom | Implication |
|-------|---------|-------------|
| Constant undervsteer feel | FFB lightens constantly, even on straights | Grip factor is constantly low (<1.0) |
| dG/dAlpha oscillation | Slope jumps between extreme values | Either the derivative calculation is unstable OR the input data is noisy/sparse |
| Car-specific behavior | Works on some cars, not others | Different cars may have different telemetry characteristics or physics models |
| Noisy/notchy feel | Strong jolts during normal driving | Slope detection is triggering grip loss spuriously |

---

## 2. Root Cause Analysis

### 2.1 The Fundamental Algorithm Issue: Division by Small dAlpha/dt

The core slope detection algorithm calculates:

```cpp
// FFBEngine.h lines 839-840
if (std::abs(dAlpha_dt) > 0.001) {
    m_slope_current = dG_dt / dAlpha_dt;
}
```

**Problem:** When the slip angle is changing very slowly (driver maintaining steady steering on a straight), `dAlpha_dt` becomes very small. Even tiny noise in `dG_dt` will create massive slope values:

- If `dAlpha_dt = 0.001` (at threshold) and `dG_dt = 0.01` (small G change)
- Then `m_slope_current = 0.01 / 0.001 = 10.0` ← HUGE positive slope
- If `dG_dt = -0.01` (tiny negative), slope = -10.0 ← HUGE negative slope

This explains why the slope oscillates between extreme values even when the car is driving relatively straight.

### 2.2 The "Constant Understeer" Problem: Slope is Sticky-Negative

When the slip angle is NOT changing significantly (driving straight), the algorithm does NOT update `m_slope_current`:

```cpp
// FFBEngine.h lines 839-841
if (std::abs(dAlpha_dt) > 0.001) {
    m_slope_current = dG_dt / dAlpha_dt;
}
// else: If Alpha isn't changing, keep previous slope value (don't update).
```

**Problem:** If the last calculated slope was negative (e.g., during a turn), it will **stay negative** when driving straight, causing constant understeer feel.

**Evidence from user feedback:**
> "It somehow feels like the understeer effect is constantly running at 100% understeer. That's why my FFB nearly disappears completely when putting the effect to 100% reduction, even on straights."

This is exactly what happens when the slope remains stuck at a negative value.

### 2.3 The Latency/Delay Between G and Alpha

The algorithm calculates:
- `dG_dt`: Rate of change of lateral G
- `dAlpha_dt`: Rate of change of slip angle

**Problem:** In real vehicle dynamics, lateral G-force LAGS behind slip angle due to tire relaxation length. The tire takes 10-50ms to build up its lateral force after the slip angle is established.

This means:
- When you turn the wheel: `dAlpha_dt` goes positive immediately
- The lateral G builds up ~20-50ms later
- During this lag window, the algorithm sees `dAlpha` increasing but `dG` staying flat, yielding a **zero or low slope** (perceived as "grip loss")

This explains why quick steering inputs cause false understeer detection.

### 2.4 Car-Specific Telemetry Differences (McLaren Issue)

The McLaren GT3 may have different telemetry characteristics that make the slope detection less stable:

1. **Different tire model parameters:** The McLaren may have a sharper SAT drop-off curve
2. **Different suspension geometry:** Affecting how slip angle relates to actual lateral force
3. **Encrypted/missing telemetry data:** If `mGripFract` is missing, the fallback is triggered for ALL cars, but some cars may have additional missing data that corrupts the calculation

**Investigation needed:** A direct comparison of telemetry logs between McLaren and Mercedes GT3 to see if there's a difference in telemetry data quality or values.

### 2.5 The 0.001 Threshold is Arbitrary

The constant `0.001` for `dAlpha_dt` threshold was chosen without empirical validation:

```cpp
if (std::abs(dAlpha_dt) > 0.001) {
```

**Issue:** At 400Hz telemetry with a 15-sample Savitzky-Golay window:
- The SG filter outputs derivative in units per second
- A slip angle change of 0.01 rad over 100ms = 0.1 rad/s
- With SG filter, `dAlpha_dt` during cornering might be 0.5-2.0 rad/s
- On straights, it might be 0.0001-0.01 rad/s

The `0.001` threshold is likely too LOW, allowing noisy calculations when the driver isn't actively cornering.

---

## 3. Proposed Fixes

### 3.1 Fix 1: Increase the dAlpha/dt Threshold (Immediate)

**Current:** `0.001` (too sensitive to noise)  
**Proposed:** `0.01` or `0.02`

```cpp
// FFBEngine.h line 839
if (std::abs(dAlpha_dt) > 0.02) {  // Was 0.001
    m_slope_current = dG_dt / dAlpha_dt;
}
```

**Rationale:** This ensures slope is only calculated during active cornering, not on straights or during small corrections.

### 3.2 Fix 2: Decay Slope to Zero When Alpha is Stable

**Problem:** The slope value is "sticky" and retains its last value indefinitely.  
**Solution:** When `dAlpha_dt` is below threshold, decay `m_slope_current` toward 0.0 (neutral) instead of keeping it constant.

```cpp
// Proposed new logic
if (std::abs(dAlpha_dt) > SLOPE_ALPHA_THRESHOLD) {
    m_slope_current = dG_dt / dAlpha_dt;
} else {
    // Decay toward 0 (neutral) when not actively cornering
    double decay_rate = 5.0; // Decay constant (adjustable)
    m_slope_current += decay_rate * dt * (0.0 - m_slope_current);
}
```

**Effect:** On straights, the slope gradually returns to 0, and the understeer effect diminishes. During cornering, the slope is actively calculated.

### 3.3 Fix 3: Add Phase Alignment for G-Force Lag

The physics of tire force generation creates a delay between slip angle change and lateral G response. We can compensate by:

1. **Option A (Simple):** Use a shorter window for `dG_dt` than for `dAlpha_dt`, effectively "looking back in time" for the slip angle.

2. **Option B (Advanced):** Introduce a configurable delay parameter that shifts the slip angle data back in time before calculating the derivative ratio.

**Implementation Sketch (Option A):**
```cpp
// Use different window sizes
double dG_dt = calculate_sg_derivative(m_slope_lat_g_buffer, count, m_slope_sg_window, dt);
double dAlpha_dt = calculate_sg_derivative(m_slope_slip_buffer, count, m_slope_sg_window + 4, dt); // Larger window for slip = shifted in time
```

### 3.4 Fix 4: Add a "Confidence" or "Validity" Gate

Only apply slope-based grip reduction when we have high confidence in the calculation:

```cpp
double confidence = std::abs(dAlpha_dt) / 0.1; // 0 to 1 scale
confidence = std::min(1.0, confidence);

double effective_grip_loss = (grip_loss) * confidence;
```

**Effect:** When the driver isn't actively cornering (low `dAlpha_dt`), the slope-based grip modulation is scaled down, preventing false understeer.

### 3.5 Fix 5: Make the Algorithm Configurable (Fallback Option)

If the dynamic approach proves too difficult to tune universally, expose:
- `m_slope_alpha_threshold`: The minimum `dAlpha_dt` required to calculate slope (default 0.02)
- `m_slope_decay_rate`: How quickly slope returns to neutral on straights (default 5.0)

---

## 4. Telemetry Logging Feature Specification

To properly diagnose slope detection and other FFB issues, we need a comprehensive telemetry logging system.

### 4.1 Feature Overview

The lmuFFB app will include a **Telemetry Logging** toggle that:
- Records telemetry data and intermediate FFB calculations during driving sessions
- Saves one log file per driving session (auto-named with timestamp)
- Uses asynchronous I/O to avoid impacting FFB performance

### 4.2 GUI Integration

**Location:** Add to the "Troubleshooting" or "Advanced" section of the GUI

**Controls:**
- **Toggle:** "Enable Telemetry Logging" (checkbox)
- **Status:** Shows "Recording..." with file size when active
- **Marker Button:** "Mark Event" to insert a timestamp marker for later analysis
- **Session Info:** Display current log filename and estimated file size

### 4.3 Log File Format

**Format:** CSV (for compatibility with Excel, Python/Pandas, MegaLogViewer)  
**Filename:** `lmuffb_telemetry_YYYY-MM-DD_HH-MM-SS.csv`  
**Location:** `%APPDATA%/lmuFFB/logs/` or user-configurable

**File Header (Configuration Context):**
```csv
# lmuFFB Telemetry Log v1.0
# Date: 2026-02-03 13:35:00
# Version: 0.7.2
# Car: Porsche 911 GT3 R
# Track: Spa-Francorchamps
# Settings:
#   slope_detection_enabled: true
#   slope_sg_window: 15
#   slope_sensitivity: 0.5
#   slope_negative_threshold: -0.3
#   slope_smoothing_tau: 0.04
#   understeer_effect: 100%
#   lateral_g_boost: 0 (auto-disabled)
Time,DeltaTime,Speed,...
```

### 4.4 Data Channels to Log

| Channel Name | Source | Description | Unit |
|--------------|--------|-------------|------|
| `Time` | Session elapsed | Absolute session time | seconds |
| `DeltaTime` | `data->mDeltaTime` | Frame delta | seconds |
| `Speed` | `data->mLocalVel.z` | Car speed (longitudinal) | m/s |
| `LatAccel` | `data->mLocalAccel.x` | Lateral acceleration | m/s² |
| `LongAccel` | `data->mLocalAccel.z` | Longitudinal acceleration | m/s² |
| `Steering` | `data->mUnfilteredSteering` | Steering input | -1 to 1 |
| `Throttle` | `data->mUnfilteredThrottle` | Throttle input | 0 to 1 |
| `Brake` | `data->mUnfilteredBrake` | Brake input | 0 to 1 |
| | | **Front Axle** | |
| `SlipAngleFL` | Calculated | Front-left slip angle | radians |
| `SlipAngleFR` | Calculated | Front-right slip angle | radians |
| `SlipAngleFront` | `m_grip_diag.front_slip_angle` | Average front slip angle | radians |
| `GripFL` | `fl.mGripFract` | Raw grip fraction FL | 0-1 |
| `GripFR` | `fr.mGripFract` | Raw grip fraction FR | 0-1 |
| `CalcGripFront` | `ctx.avg_grip` | Calculated front grip | 0-1 |
| | | **Slope Detection Specific** | |
| `dG_dt` | Internal | Derivative of Lat G | G/s |
| `dAlpha_dt` | Internal | Derivative of Slip Angle | rad/s |
| `SlopeCurrent` | `m_slope_current` | dG/dAlpha ratio | G/rad |
| `SlopeSmoothed` | `m_slope_smoothed_output` | Smoothed grip factor | 0-1 |
| | | **Rear Axle** | |
| `SlipAngleRear` | `m_grip_diag.rear_slip_angle` | Average rear slip angle | radians |
| `CalcGripRear` | `ctx.avg_rear_grip` | Calculated rear grip | 0-1 |
| `GripDelta` | `ctx.avg_grip - ctx.avg_rear_grip` | Front-rear grip difference | -1 to 1 |
| | | **FFB Output** | |
| `FFBTotal` | Final output | Total FFB force | normalized |
| `FFBBase` | Base steering torque | Raw shaft torque | Nm |
| `FFBGripFactor` | `ctx.grip_factor` | Grip modulation factor | 0-1 |
| `FFBSoP` | SoP effect | Seat of Pants force | Nm |
| `FFBRearAlign` | Rear align torque | Counter-steer cue | Nm |
| `SpeedGate` | `ctx.speed_gate` | Low-speed muting factor | 0-1 |
| `Clipping` | Boolean | Is output clipping? | 0/1 |
| `Marker` | User button | Event marker | 0/1 |

### 4.5 Logging Rate and Decimation

**Native Rate:** 400 Hz (full physics update rate)  
**Logged Rate:** 100 Hz (decimated by factor of 4)

**Rationale:** 
- 400 Hz generates ~100 MB per 10 minutes of driving
- 100 Hz generates ~25 MB per 10 minutes (manageable)
- 100 Hz is still sufficient for all diagnostic needs

**Implementation:**
```cpp
// Log every 4th frame
m_log_frame_counter++;
if (m_log_frame_counter % 4 == 0) {
    AsyncLogger::Get().Log(frame);
}
```

### 4.6 Diagnostic Driving Session Guides

Create ad-hoc markdown guides that instruct users to perform specific driving maneuvers for diagnostic purposes:

**Guide: `docs/diagnostics/slope_detection_test_drive.md`**
```markdown
# Slope Detection Diagnostic Drive

## Purpose
This guide helps generate telemetry data to diagnose slope detection issues.

## Prerequisites
1. Enable Telemetry Logging in lmuFFB
2. Enable Slope Detection
3. Set Understeer Effect to 100%
4. Use a known car (Porsche 911 GT3 R recommended)

## Test Procedure

### Test 1: Straight Line (Baseline)
1. Drive on a straight for 10 seconds at 100 km/h
2. Keep steering centered
3. Press "Mark Event" at the end

Expected: Grip should stay at 1.0 (100%), no understeer effect

### Test 2: Constant Radius Turn
1. Enter a long sweeping corner at moderate speed
2. Maintain constant steering angle and speed for 5 seconds
3. Press "Mark Event" while in steady-state cornering

Expected: Grip should drop slightly below 1.0 proportional to corner severity

### Test 3: Corner Entry (Transient)
1. Approach a hairpin at 60 km/h
2. Turn in quickly and press "Mark Event" at turn-in
3. Continue through apex

Expected: Grip should drop smoothly during turn-in

### Test 4: Zig-Zag Slalom
1. At 60 km/h, perform left-right-left slalom
2. Press "Mark Event" during the maneuver

This tests the algorithm's response to quick direction changes.
```

---

## 5. Log Analyzer Specification

A Python-based log analyzer will parse the CSV logs and generate diagnostic reports and plots.

### 5.1 Core Components

```
lmuffb_log_analyzer/
├── __init__.py
├── loader.py          # CSV loading and parsing
├── stats.py           # Statistical analysis functions
├── plots.py           # Visualization generation
├── reports.py         # Text report generation
├── analyzers/
│   ├── __init__.py
│   ├── slope_analyzer.py   # Slope detection specific analysis
│   ├── grip_analyzer.py    # Grip estimation analysis
│   └── ffb_analyzer.py     # General FFB signal analysis
└── cli.py             # Command-line interface
```

### 5.2 Usage Examples

**Basic Report:**
```bash
python -m lmuffb_log_analyzer report lmuffb_telemetry_2026-02-03_14-30-00.csv
```

Output:
```
============================================================
lmuFFB Telemetry Analysis Report
============================================================
Session: 2026-02-03 14:30:00
Duration: 5 minutes 23 seconds
Frames: 32380 (100 Hz)

--- General Statistics ---
Average Speed: 142.3 km/h
Max Lateral G: 1.85 G
FFB Clipping Events: 23 (0.07%)

--- Slope Detection Analysis ---
Slope Calculation Active: 68.2% of session
Average Slope (when active): 0.42 G/rad
Slope Range: -12.3 to +8.7 G/rad (HIGH VARIANCE - INVESTIGATE)
Negative Slope Events (< -0.3): 234 (0.72%)
Grip at 20% Floor Events: 45 (0.14%)

--- Issues Detected ---
[WARNING] High slope variance suggests unstable calculation
[WARNING] 45 frames hit the 0.2 grip floor - algorithm may be too aggressive
[INFO] Markers found at: 00:32, 01:45, 03:12

============================================================
```

**Slope Detection Plots:**
```bash
python -m lmuffb_log_analyzer slope-plots lmuffb_telemetry_2026-02-03_14-30-00.csv --output ./plots/
```

### 5.3 Plots for Slope Detection Analysis

The analyzer should generate the following plots:

#### 5.3.1 Time-Series Multi-Plot (Primary Diagnostic)
A synchronized 4-panel plot showing:
- Panel 1: Lateral G and Slip Angle (front) vs Time
- Panel 2: dG/dt and dAlpha/dt vs Time
- Panel 3: Slope (dG/dAlpha) vs Time
- Panel 4: Calculated Grip Factor vs Time

**Purpose:** Shows the complete chain from input to output, helps identify where the algorithm is going wrong.

#### 5.3.2 Slip Angle vs Lateral G (Phase Portrait)
X-axis: Slip Angle (radians)
Y-axis: Lateral G

**Purpose:** Visualizes the tire curve being measured. Should show a clear peak. Scatter will indicate noise.

#### 5.3.3 dAlpha/dt Histogram
Distribution of `dAlpha_dt` values

**Purpose:** Shows how often the slip angle is changing. If mostly near 0, the slope calculation will be unreliable.

#### 5.3.4 Slope vs dAlpha/dt Scatter
X-axis: dAlpha/dt
Y-axis: Slope (dG/dAlpha)

**Purpose:** Shows the relationship between "confidence" (high dAlpha/dt) and slope value. Ideally, slopes near 0 dAlpha/dt should cluster near 0 slope.

#### 5.3.5 Grip Factor vs Speed
X-axis: Speed (km/h)
Y-axis: Calculated Grip Factor

**Purpose:** Shows if grip is incorrectly low at low speeds or on straights (where it should be 1.0).

#### 5.3.6 FFB Output vs Grip Factor
X-axis: Grip Factor
Y-axis: FFB Output

**Purpose:** Confirms that understeer effect is correctly modulating FFB based on grip.

### 5.4 Statistical Analysis Functions

```python
def analyze_slope_stability(df):
    """
    Analyze the stability of slope calculations.
    
    Returns:
        dict: {
            'slope_variance': float,
            'slope_range': (min, max),
            'pct_active': float,  # % of frames where dAlpha > threshold
            'pct_floor_hits': float,  # % of frames at 0.2 grip floor
            'stability_score': float,  # 0-100, higher is better
            'issues': list[str]
        }
    """
    pass

def analyze_grip_correlation(df):
    """
    Analyze correlation between calculated grip and expected physics.
    
    Returns:
        dict: {
            'grip_vs_slip_correlation': float,
            'grip_vs_latg_correlation': float,
            'expected_behavior': bool
        }
    """
    pass

def detect_oscillation_events(df, column='SlopeCurrent', threshold=5.0, min_duration=0.1):
    """
    Detect periods where a signal oscillates rapidly between extremes.
    
    Returns:
        list[dict]: List of oscillation events with start time, duration, amplitude
    """
    pass
```

---

## 6. Slope Detection Diagnostic Requirements

This section specifies exactly what data and analysis is needed to diagnose slope detection issues.

### 6.1 Required Data Channels

To diagnose slope detection, the following channels MUST be logged:

| Channel | Why It's Needed |
|---------|-----------------|
| `Time` | Correlate events across multiple plots |
| `Speed` | Filter out low-speed artifacts |
| `LatAccel` | The "G" input to slope calculation |
| `SlipAngleFront` | The "Alpha" input to slope calculation |
| `dG_dt` | First derivative output from SG filter |
| `dAlpha_dt` | First derivative output from SG filter |
| `SlopeCurrent` | The ratio dG/dAlpha |
| `SlopeSmoothed` | Output grip factor (after smoothing) |
| `CalcGripFront` | Final grip value used for understeer |
| `FFBGripFactor` | The grip factor applied to FFB |
| `FFBTotal` | Final FFB output (to see effect on steering) |

### 6.2 Required Plots for Slope Diagnosis

| Plot | What It Reveals |
|------|-----------------|
| **Time-series: Slope vs Grip** | Shows if grip is tracking slope correctly |
| **Time-series: dAlpha_dt** | Shows when slope calculation is "active" (above threshold) |
| **Histogram: dAlpha_dt** | Shows distribution of slip rate changes |
| **Scatter: Slip vs Lat_G** | Shows the tire curve shape and noise level |
| **Time-series: Slope (zoomed to marked event)** | Detailed view of problematic moments |

### 6.3 Required Statistical Analysis

| Metric | Threshold | Issue if Exceeded |
|--------|-----------|-------------------|
| Slope Variance | < 25 | High variance = noisy/unstable |
| Pct Time at Grip Floor (0.2) | < 5% | High = algorithm too aggressive |
| Pct Time dAlpha_dt below threshold | > 70% | Slope is mostly not being calculated |
| Correlation: Grip vs Abs(SlipAngle) | > 0.5 | Low = grip not tracking slip correctly |

### 6.4 Specific Questions the Analysis Should Answer

1. **Is the slope oscillating excessively?**
   - Check slope variance and range
   - If range exceeds ±10, investigate noise in inputs

2. **Is grip stuck at a low value on straights?**
   - Filter for Speed > 100 km/h AND Abs(SlipAngle) < 0.02
   - Check if Grip is consistently < 0.9

3. **Is the dAlpha/dt threshold being hit correctly?**
   - During corners, dAlpha_dt should be > 0.02
   - On straights, dAlpha_dt should be < 0.01

4. **Is there a lag between slip angle change and grip response?**
   - Cross-correlation analysis between `dAlpha_dt` and `SlopeSmoothed`
   - Expected lag: 20-50ms

5. **Is the McLaren telemetry different from Mercedes?**
   - Compare telemetry from both cars
   - Check for differences in SlipAngle noise floor, LatAccel noise floor

### 6.5 Sample Python Code for Slope Analysis

```python
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def load_lmuffb_log(filepath):
    """Load lmuFFB telemetry log, skipping header comments."""
    with open(filepath, 'r') as f:
        lines = f.readlines()
    
    # Skip comment lines starting with #
    data_start = 0
    for i, line in enumerate(lines):
        if not line.startswith('#'):
            data_start = i
            break
    
    return pd.read_csv(filepath, skiprows=data_start)

def analyze_slope_detection(df):
    """Analyze slope detection behavior."""
    results = {}
    
    # Basic stats
    results['slope_mean'] = df['SlopeCurrent'].mean()
    results['slope_std'] = df['SlopeCurrent'].std()
    results['slope_min'] = df['SlopeCurrent'].min()
    results['slope_max'] = df['SlopeCurrent'].max()
    
    # Percentage of time dAlpha_dt is above threshold
    threshold = 0.001  # Current threshold
    active_pct = (np.abs(df['dAlpha_dt']) > threshold).mean() * 100
    results['slope_active_pct'] = active_pct
    
    # Percentage of time at grip floor
    floor_pct = (df['CalcGripFront'] <= 0.21).mean() * 100
    results['grip_floor_pct'] = floor_pct
    
    # Grip on straights (should be ~1.0)
    straight_mask = (df['Speed'] > 27.8) & (np.abs(df['SlipAngleFront']) < 0.02)  # > 100 km/h
    if straight_mask.any():
        results['grip_on_straights_mean'] = df.loc[straight_mask, 'CalcGripFront'].mean()
    
    # Issue detection
    results['issues'] = []
    if results['slope_std'] > 5.0:
        results['issues'].append("HIGH SLOPE VARIANCE - Algorithm is unstable")
    if results['grip_floor_pct'] > 5.0:
        results['issues'].append("FREQUENT FLOOR HITS - Algorithm too aggressive")
    if results['slope_active_pct'] < 30.0:
        results['issues'].append("LOW ACTIVE PCT - Slope rarely calculated (dAlpha_dt threshold too high)")
    if 'grip_on_straights_mean' in results and results['grip_on_straights_mean'] < 0.9:
        results['issues'].append("LOW GRIP ON STRAIGHTS - Slope stuck at negative value")
    
    return results

def plot_slope_timeseries(df, output_path='slope_analysis.png'):
    """Generate 4-panel time-series plot for slope analysis."""
    fig, axes = plt.subplots(4, 1, figsize=(12, 10), sharex=True)
    
    time = df['Time']
    
    # Panel 1: Inputs
    ax1 = axes[0]
    ax1.plot(time, df['LatAccel'], label='Lat G', color='blue', alpha=0.7)
    ax1_twin = ax1.twinx()
    ax1_twin.plot(time, df['SlipAngleFront'], label='Slip Angle', color='orange', alpha=0.7)
    ax1.set_ylabel('Lat G (m/s²)')
    ax1_twin.set_ylabel('Slip Angle (rad)')
    ax1.legend(loc='upper left')
    ax1_twin.legend(loc='upper right')
    ax1.set_title('Inputs: Lateral G and Slip Angle')
    
    # Panel 2: Derivatives
    ax2 = axes[1]
    ax2.plot(time, df['dG_dt'], label='dG/dt', color='blue', alpha=0.7)
    ax2.plot(time, df['dAlpha_dt'], label='dAlpha/dt', color='orange', alpha=0.7)
    ax2.axhline(0.001, color='red', linestyle='--', alpha=0.5, label='Threshold (0.001)')
    ax2.axhline(-0.001, color='red', linestyle='--', alpha=0.5)
    ax2.set_ylabel('Derivative')
    ax2.legend()
    ax2.set_title('Derivatives: dG/dt and dAlpha/dt')
    
    # Panel 3: Slope
    ax3 = axes[2]
    ax3.plot(time, df['SlopeCurrent'], label='Slope (dG/dAlpha)', color='purple')
    ax3.axhline(-0.3, color='red', linestyle='--', alpha=0.5, label='Negative Threshold')
    ax3.set_ylabel('Slope (G/rad)')
    ax3.legend()
    ax3.set_title('Calculated Slope (dG/dAlpha)')
    ax3.set_ylim(-15, 15)  # Clamp for visibility
    
    # Panel 4: Grip
    ax4 = axes[3]
    ax4.plot(time, df['CalcGripFront'], label='Calc Grip', color='green')
    ax4.plot(time, df['FFBGripFactor'], label='FFB Grip Factor', color='red', alpha=0.7)
    ax4.axhline(0.2, color='gray', linestyle='--', alpha=0.5, label='Floor (0.2)')
    ax4.axhline(1.0, color='gray', linestyle='--', alpha=0.5)
    ax4.set_ylabel('Grip Factor')
    ax4.set_xlabel('Time (s)')
    ax4.legend()
    ax4.set_title('Output: Grip Factor')
    
    plt.tight_layout()
    plt.savefig(output_path, dpi=150)
    plt.close()
    
    return output_path
```

---

## 7. Summary and Next Steps

### 7.1 Root Causes Identified

1. **Division by small dAlpha/dt** causes extreme slope values
2. **Slope value is "sticky"** when dAlpha/dt is below threshold
3. **Tire relaxation lag** between slip angle and G-force
4. **The 0.001 threshold is too low**, allowing noisy calculations

### 7.2 Proposed Fixes (In Priority Order)

1. **Immediate:** Increase `dAlpha_dt` threshold from 0.001 to 0.02
2. **Short-term:** Add slope decay toward 0 when not actively cornering
3. **Medium-term:** Add confidence-based grip scaling
4. **Long-term:** Implement phase alignment for tire lag compensation

### 7.3 Telemetry Logging Implementation Path

1. **Phase 1:** Implement AsyncLogger with core channels
2. **Phase 2:** Add slope-specific channels (dG_dt, dAlpha_dt, etc.)
3. **Phase 3:** Create GUI integration (toggle, marker button)
4. **Phase 4:** Develop Python log analyzer with slope diagnostics
5. **Phase 5:** Create diagnostic driving session guides

### 7.4 Immediate Actions

1. [ ] Implement the `dAlpha_dt` threshold increase fix
2. [ ] Implement slope decay to neutral
3. [ ] Add telemetry logging capability
4. [ ] Create test logs comparing McLaren vs Mercedes GT3
5. [ ] Analyze logs to validate fixes

---

## Appendix: Code References

### Key Source Files

| File | Lines | Description |
|------|-------|-------------|
| `src/FFBEngine.h` | 316-320 | Slope detection configuration variables |
| `src/FFBEngine.h` | 416-426 | Slope detection buffers and state |
| `src/FFBEngine.h` | 790-862 | Slope detection algorithm implementation |
| `src/FFBEngine.h` | 660-666 | Slope detection integration in grip calculation |
| `src/FFBEngine.h` | 1329-1334 | Lateral G Boost disabled when slope enabled |
| `src/GuiLayer.cpp` | 1100-1190 | Slope detection UI section |
| `src/Config.h/cpp` | Various | Slope detection persistence |

### Related Documentation

- `docs/dev_docs/slope_detection_implementation_plan.md`
- `docs/dev_docs/slope_detection_implementation_plan2.md`
- `docs/dev_docs/investigations/slope_detection_issues_v0.7.0.md`
- `docs/dev_docs/design proposal for a High-Performance Asynchronous Telemetry Logger.md`
- `docs/dev_docs/github_issues/issue_25_Implement_Slope_Detection_logic.md`

---

*Investigation Completed: 2026-02-03*  
*Next Review: After implementing proposed fixes*  
*Status: Awaiting telemetry logging implementation for deeper analysis*
