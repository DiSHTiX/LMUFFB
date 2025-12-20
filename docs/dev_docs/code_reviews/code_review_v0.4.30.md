# Code Review Report - v0.4.30

**Date:** 2025-12-20
**Reviewer:** Antigravity Agent
**Status:** Passed (with fixes)

## 1. Summary of Staged Changes
The staged changes primarily addressed a critical coordinate system mismatch in the **Seat of Pants (SoP)** effect.
*   **Change:** Removed the sign inversion for `lat_g`.
*   **Reasoning:** Determining that the internal engine uses the Game Coordinate System (+ = Left), matching the Base Torque and Rear Align Torque.
*   **Verification:** Updates to `tests/test_ffb_engine.cpp` confirmed the new expectation (Positive Force for Left Slide/Right Turn).

## 2. Findings & Issues
While the SoP fix was correct, the regression tests revealed that other effects were still operating under the incorrect coordinate assumptions, leading to "fighting" forces where effects opposed the aligning torque instead of assisting it.

### Issues Identified:
1.  **Rear Align Torque Mismatch:** The regression test `Test: Coordinate System - Rear Torque Inversion (v0.4.19)` failed. The calculation was inverted (`-calc_rear_lat_force`), causing it to pull in the wrong direction (destabilizing) relative to the fixed SoP.
2.  **Scrub Drag Mismatch:** The regression test `Test: Coordinate System - Scrub Drag Direction` failed. The damping force was amplifying lateral velocity instead of opposing it.

## 3. Fixes Implemented
The following changes were applied to `FFBEngine.h` to resolve the test failures and ensure holistic alignment of all lateral forces:

### A. Rear Align Torque
*   **Action:** Removed negative sign from the torque calculation.
*   **Code:** `double rear_torque = calc_rear_lat_force * REAR_ALIGN_TORQUE_COEFFICIENT * m_rear_align_effect;`
*   **Result:** Positive Lateral Velocity (Slide Left) -> Positive Rear Force (Workaround) -> Positive Torque (Left Pull). This now aligns with the Base Torque (Aligning) and SoP (Weight).

### B. Scrub Drag
*   **Action:** Inverted the direction logic.
*   **Code:** `scrub_drag_force = -drag_dir * m_scrub_drag_gain * 5.0 * fade;`
*   **Result:** Positive Lateral Velocity (Slide Left) -> Negative Force (Right Pull). This provides correct **Damping** to resist the slide.

## 4. Verification
After applying the fixes, the full test suite was executed.
*   **Command:** `tests\test_ffb_engine.exe`
*   **Result:**
    *   **Tests Passed:** 124
    *   **Tests Failed:** 0

All coordinate system tests and holistic alignment tests (checking SoP, Rear Torque, and Scrub Drag together) are now passing.

## 5. Conclusion
The codebase is now stable with consistent coordinate systems across all lateral effects. The fix for v0.4.30 is complete and verified.
