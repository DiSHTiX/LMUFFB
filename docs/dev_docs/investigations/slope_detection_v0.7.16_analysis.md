# Slope Detection Log Analysis Report (v0.7.16)

**Date:** 2026-02-09
**Subject:** Analysis of Telemetry Logs for Slope Detection Stability
**Version:** 0.7.16
**Logs Analyzed:** 6 sessions (Panis Racing #48 / Circuit de la Sarthe)

---

## 1. Executive Summary

Analysis of the telemetry logs generated on 2026-02-09 reveals **critical instability** in the Slope Detection algorithm. The calculated slope values exhibit extreme variance, frequently exceeding physically possible values (e.g., reaching -8000 to +2000 range), whereas expected values should remain within -5 to +5.

This instability is caused by **numerical explosion** when the slip angle derivative (`dAlpha/dt`) is just above the minimum active threshold (0.02 rad/s) while the lateral G derivative (`dG/dt`) is significant. This results in massive negative slope calculations, causing the grip factor to instantly drop to the floor (0.2), creating chaotic FFB behavior and triggering oscillation detectors.

## 2. Quantitative Findings

Aggregated data from the processed logs:

| Metric | Observed Range | Expected / Target | Status |
|--------|----------------|-------------------|--------|
| **Slope Mean** | -1.19 to -0.44 | -2.0 to -0.5 (during cornering) | ⚠️ Unstable |
| **Slope Std Dev** | **20.51 - 69.20** | < 2.0 | ❌ **CRITICAL FAILURE** |
| **Slope Range** | -8182 to +2121 | -10 to +10 | ❌ **CRITICAL FAILURE** |
| **Active Time** | 8.9% - 14.3% | 30% - 60% (track dependent) | ⚠️ Low Trigger Rate |
| **Oscillations** | 24 - 341 events | 0 events | ❌ Frequent Oscillations |

### specific Log Breakdown

1.  **Log 22:39:21 (Panis Racing):** Range -330 to +382. 58 oscillation events.
2.  **Log 22:39:25 (Panis Racing):** Range -1467 to +869. 242 oscillation events.
3.  **Log 22:39:31 (Panis Racing):** Range -676 to +1151. 341 oscillation events.

## 3. Visual Analysis Findings

Review of the time-series plots (`_timeseries.png`) and histograms (`_dalpha_hist.png`) provides visual confirmation of the numerical instability:

### 3.1. "Barcode" Slope Artifacts
In the **Calculated Slope** subplot, the signal resembles a "barcode" rather than a continuous physical measurement.
*   **Observation:** The slope switches between extreme positive and extreme negative values within single frames.
*   **Correlation:** This aligns precisely with the **Grip Factor** subplot (Green line), which shows "grass-like" transient drops to the floor (0.2) rather than smooth curves.
*   **Conclusion:** The algorithm is not tracking tire physics; it is amplifying high-frequency noise.

### 3.2. Binary Grip Behavior
The **Grip Factor** output rarely settles at intermediate values (e.g., 0.6 or 0.7).
*   **Observation:** The signal is effectively binary: it is either **1.0 (Full Grip)** or **0.2 (Floor)**.
*   **Cause:** The calculated slope magnitude is so large (often > 200) that it instantly saturates the Min/Max thresholds (-0.3 to -2.0), forcing the output to the clamp limit immediately.

### 3.3. Derivative Scale Mismatch
In the **Derivatives** subplot:
*   `dAlpha/dt` (Orange) is barely visible, hovering near the zero line.
*   `dG/dt` (Blue) shows significant spikes due to road noise or bumps.
*   **Conclusion:** The massive disparity in scale means `dG` acts as pure noise acting on a near-zero denominator, resulting in the observed chaotic output.

## 4. Root Cause Analysis

### 4.1. Mathematical Instability
The slope corresponds to the derivative of lateral force with respect to slip angle:
$$Slope = \frac{dG}{dt} / \frac{d\alpha}{dt}$$

The algorithm enforces a threshold on $\frac{d\alpha}{dt}$ (default 0.02 rad/s). However, when $\frac{d\alpha}{dt}$ is slightly above this threshold (e.g., 0.0224 rad/s) and lateral G is changing rapidly (e.g., -4.4 G/s due to weight transfer or bumps), the result is catastrophic:

**Example from Log Line 373:**
*   `dG_dt`: -4.4081 G/s
*   `dAlpha_dt`: 0.0224 rad/s (Valid > 0.02 threshold)
*   **Result:** $-4.4081 / 0.0224 \approx -196.79$

This value (-196) is orders of magnitude beyond the `Max Threshold` (-2.0), causing an instantaneous 100% signal for grip loss.

### 4.2. Lack of Output Clamping
The raw slope value appears to be fed directly into the smoothing function without a reasonable hard clamp (e.g., limiting slope to range [-20, +20]) before it interacts with the logic. While `GripFactor` is clamped between 0.2 and 1.0, the input driving it swings wildly, defeating the purpose of the smoothing filter (EMA), which cannot cope with spikes of magnitude 200+.

## 5. Impact on Driver Experience

*   **"Notchy" FFB:** The rapid switching between "Inactive" (Slope 0, Grip 1.0) and "Exploded" (Slope -200, Grip 0.2) creates severe jolts in the force feedback.
*   **Binary Feel:** Instead of a progressive loss of grip, the user experiences binary On/Off behavior when the threshold is crossed during transient maneuvers.
*   **False Positives:** Bumps causing high `dG/dt` while steering is relatively steady (low but active `dAlpha/dt`) will trigger false understeer signals.

## 6. Log Analyzer Enhancements

To bridge the gap between text reports and visual plots, the Log Analyzer text report should serve as a "textual visualization" by including specific density and volatility metrics.

### 6.1. "Singularity" Event Detection
*   **Metric:** Count events where `abs(dAlpha_dt) < 0.05` AND `abs(Slope) > 10.0`.
*   **Purpose:** Specifically identifies the division-by-small-number artifacts distinct from general noise.

### 6.2. Signal Volatility Metrics (New)
*   **Zero-Crossing Rate (Hz):** How many times per second the Slope signal crosses zero.
    *   *Interpretation:* > 5Hz indicates noise; < 2Hz indicates physical tire behavior.
*   **Binary State Residence (%):** Percentage of active time the Grip Factor is near the rails (>0.95 or <0.25).
    *   *Interpretation:* High values (>50%) indicate the feedback is binary/digital rather than analog/progressive.
*   **Derivative Energy Ratio:** Ratio of `RMS(dG/dt)` to `RMS(dAlpha/dt)`.
    *   *Interpretation:* A massive ratio (e.g., >100) confirms that the numerator (G-force noise) is swamping the denominator (Steering input).

### 6.3. Correlation Plots for Stability
*   **Scatter Plot:** `dAlpha_dt` (x-axis) vs `Slope` (y-axis).
    *   *Expectation:* This should show a "butterfly" or asymptotic pattern where slope explodes as x approaches zero.
*   **Histogram:** Distribution of `dAlpha_dt` values when `Slope` is considered "Active". Only 8-14% active time suggests the threshold might be cutting off valid data or the driving style wasn't aggressive, but looking at where the unstable slopes occur is vital.

## 6. Comprehensive Unit Test Strategy

To prevent regression and ensure stability, the following unit tests must be added to the test suite (`tests/test_slope_detection.cpp`):

### 6.1. Singularity & Boundary Tests
*   **`TestSlope_NearThreshold_Singularity`**:
    *   Input: `dAlpha/dt` = 0.02001 (just above threshold), `dG/dt` = 5.0 (moderate bump).
    *   Assert: Resulting Grip Factor should NOT drop instantly to 0.2. Slope output must be clamped.
*   **`TestSlope_ZeroCrossing`**:
    *   Input: Sequence of slip angles crossing zero: `-0.01` -> `0.00` -> `0.01`.
    *   Assert: No NaN or Infinity in output.

### 6.2. Impulse & Noise Tests
*   **`TestSlope_ImpulseRejection`**:
    *   Input: Steady cornering, inject single frame spike in Lat G (simulating curb strike).
    *   Assert: Variance in Grip Factor should be attenuated by smoothing; no instantaneous change > 10%.
*   **`TestSlope_NoiseImmunity`**:
    *   Input: Synthetic noisy signal (Sine wave + Gaussian noise) for both G and Alpha.
    *   Assert: Standard deviation of Output Slope should be < 2.0x Standard deviation of Input trend.

### 6.3. Ramp Logic Tests
*   **`TestConfidenceRamp_Progressive`**:
    *   Input: Increasing `dAlpha/dt` from 0.0 to 0.10.
    *   Assert: Confidence (and effect weight) should increase linearly/smoothly from 0.0 to 1.0. No step changes.

## 7. Implementation Plan for Fixes

### Phase 1: Safety Clamping (Hotfix)
**Objective:** Eliminate infinite/exploded values immediately.

1.  **Modify `FFBEngine::calculate_slope_grip`**:
    *   Insert a hard clamp on the calculated raw slope **before** it enters the smoothing buffer.
    *   Limit: `[-20.0, 20.0]`.
    *   *Rationale:* A slope of -20 means losing 20G of lateral force per 1 radian of slip. This is physically impossible for a stable tire curve; anything beyond this is noise/artifact.

### Phase 2: Confidence Ramp (Stability)
**Objective:** Remove the binary "Active/Inactive" toggle that causes jolts.

1.  **Refactor `Slope Active` Logic**:
    *   Remove `bool active = dAlpha > threshold`.
    *   Introduce `float confidence = inverse_lerp(lower_thresh, upper_thresh, abs(dAlpha))`.
    *   Suggested thresholds:
        *   `Lower`: 0.01 rad/s (Start blending in)
        *   `Upper`: 0.10 rad/s (Full confidence)
2.  **Apply Confidence to Output**:
    *   `GripLoss = (RawGripLoss * confidence)`.
    *   When `dAlpha` is low (near singularity), confidence is low, effectively muting the exploded slope value.

### Phase 3: Smoothing & Filtering (Refinement)
1.  **Denominator Regularization (Optional)**:
    *   Change division to: `dG / (sign(dAlpha) * max(abs(dAlpha), epsilon))` to prevent division by zero, though Clamping + Confidence Ramp usually handles this better.
2.  **Verify Smoothing Order**:
    *   Ensure smoothing (EMA) is applied to the *final grip factor* or the *clamped slope*, never the raw exploded slope.

## 8. Conclusion
The Slope Detection feature in v0.7.16 is mathematically functional but practically unstable due to division-by-small-number artifacts. Immediate patching (Phase 1 & 2) is required to prompt a stable release.
