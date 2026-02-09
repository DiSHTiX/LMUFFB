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

## 3. Root Cause Analysis

### 3.1. Mathematical Instability
The slope corresponds to the derivative of lateral force with respect to slip angle:
$$Slope = \frac{dG}{dt} / \frac{d\alpha}{dt}$$

The algorithm enforces a threshold on $\frac{d\alpha}{dt}$ (default 0.02 rad/s). However, when $\frac{d\alpha}{dt}$ is slightly above this threshold (e.g., 0.0224 rad/s) and lateral G is changing rapidly (e.g., -4.4 G/s due to weight transfer or bumps), the result is catastrophic:

**Example from Log Line 373:**
*   `dG_dt`: -4.4081 G/s
*   `dAlpha_dt`: 0.0224 rad/s (Valid > 0.02 threshold)
*   **Result:** $-4.4081 / 0.0224 \approx -196.79$

This value (-196) is orders of magnitude beyond the `Max Threshold` (-2.0), causing an instantaneous 100% signal for grip loss.

### 3.2. Lack of Output Clamping
The raw slope value appears to be fed directly into the smoothing function without a reasonable hard clamp (e.g., limiting slope to range [-20, +20]) before it interacts with the logic. While `GripFactor` is clamped between 0.2 and 1.0, the input driving it swings wildly, defeating the purpose of the smoothing filter (EMA), which cannot cope with spikes of magnitude 200+.

## 4. Impact on Driver Experience

*   **"Notchy" FFB:** The rapid switching between "Inactive" (Slope 0, Grip 1.0) and "Exploded" (Slope -200, Grip 0.2) creates severe jolts in the force feedback.
*   **Binary Feel:** Instead of a progressive loss of grip, the user experiences binary On/Off behavior when the threshold is crossed during transient maneuvers.
*   **False Positives:** Bumps causing high `dG/dt` while steering is relatively steady (low but active `dAlpha/dt`) will trigger false understeer signals.

## 5. Recommendations

### 5.1. Immediate Code Fixes
1.  **Hard Clamp Raw Slope:** Implement a clamp on the predicted slope value immediately after calculation.
    ```cpp
    double slope = dG_dt / dAlpha_dt;
    slope = std::clamp(slope, -20.0, 20.0); // Prevent infinity/explosions
    ```
2.  **Denominator Regularization:** Add a small epsilon or scaling factor to the denominator to soften the asymptotic behavior near the threshold.
3.  **Review Max Threshold:** Ensure the `Slope Max Threshold` setting interprets values beyond -2.0 consistently, but the input must be sane first.

### 5.2. Algorithm Tuning
*   **Increase Window Size:** The default window (15) might be too small for the noisy telemetry of LMU/rF2 engine. Testing with 21 or 31 is recommended.
*   **Confidence Scaling:** Instead of a hard binary threshold for `Active`, use a smooth ramp (0.0 to 1.0) based on `dAlpha` magnitude. E.g., confidence is 0% at 0.02 rad/s and 100% at 0.10 rad/s. This would attenuate the "exploded" slope values calculated near the lower bound.

## 6. Conclusion
The Slope Detection feature in v0.7.16 is mathematically functional but practically unstable due to division-by-small-number artifacts. It requires immediate patching to clamp inputs and smooth the activation transition to be usable.
