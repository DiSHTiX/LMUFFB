Based on a detailed comparison between the **Old Version (v0.4.12+)** and the **New Version (v0.6.2)**, the document has undergone significant restructuring to reflect code updates.

While the new document is more organized and reflects newer logic, **some specific implementation details, hardcoded constants, and contextual explanations have been lost or abstracted** in the transition.

Here is the breakdown of what was lost or obscured:

### 1. Hardcoded Physics Constants (Abstracted)
In the old version, specific "magic numbers" were explicitly defined. In the new version, these have been replaced by variable names, hiding the underlying default values.
*   **Friction Circle Limits:**
    *   **Old:** Explicitly defined the limits as **0.10** (Lateral) and **0.12** (Longitudinal).
        *   *Formula:* $\text{Metric}_{\text{lat}} = \alpha / 0.10$
    *   **New:** Replaced with generic variables `OptAlpha` and `OptRatio`.
        *   *Formula:* $\text{Metric}_{\text{lat}} = |\alpha| / \text{OptAlpha}$
*   **Slip Angle Smoothing:**
    *   **Old:** Explicitly listed the time constant target: $\tau \approx 0.0225s$ (derived from 400Hz).
    *   **New:** Refers to it generally as `m_shaft_smooth` or "Time-Corrected LPF" without the specific baseline value.

### 2. Developer Context & "Why" Explanations
The old document contained notes explaining *why* certain math was performed, particularly regarding the game engine's quirks. This context is missing in the new version.
*   **Coordinate System Quirk (The "Left is Right" issue):**
    *   **Old:** Contained a specific "Coordinate Note" explaining that LMU's **+X is Left**, which necessitated inverting the Drag direction (`DragDir = -1.0`).
    *   **New:** Simply provides the formula `(SideVel > 0 ? -1 : 1)` without explaining the coordinate system anomaly that necessitates it.
*   **Yaw Acceleration Inversion:**
    *   **Old:** Explicitly noted that the calculation was inverted (`-1.0`) because the SDK note says to "negate any rotation or torque data."
    *   **New:** Shows the negative sign in the formula but removes the reference to the SDK requirement.

### 3. Discrepancies in Legacy Formulas
There is a numerical mismatch in the "Legacy" section for Suspension Bottoming between the two files.
*   **Old:** $\sqrt{\text{Load} - 8000} \times K \times \mathbf{0.0025}$
*   **New:** $\sqrt{\text{ExcessLoad}} \times \mathbf{0.05}$
*   *Note:* Unless the unit of `ExcessLoad` changed or `K` defaults changed drastically, the scalar in the documentation jumped by a factor of 20x, or the previous specific scalar was lost in the rewrite.

### 4. Removed "Workaround" Details
The old document detailed specific workarounds for missing telemetry that appear to be cleaned up (or hidden) in the new doc.
*   **Rear Aligning Torque:**
    *   **Old:** Explicitly showed the step-by-step derivation of `F_z_rear` using `SuspForce + 300.0` (Unsprung Mass).
    *   **New:** Lists the formula for torque but omits the specific `+ 300.0` unsprung mass addition step in the text (though it might still exist in code).

### Summary
If you are using this documentation purely for **usage**, the new version is superior.
However, if you are using it for **development/debugging**, the **Old Version** contained valuable context regarding **coordinate systems (X+ Left)** and **specific friction coefficients (0.10/0.12)** that are not present in the v0.6.2 document.