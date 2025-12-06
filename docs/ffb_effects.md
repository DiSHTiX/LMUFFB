# FFB Effects & Customization Guide

This document details the Force Feedback effects implemented in LMUFFB, how they are derived from telemetry, and how to customize them.

## 1. Understeer (Front Grip Loss)
*   **Goal**: To communicate when the front tires are losing grip and sliding (pushing).
*   **Telemetry**: Derived from `mGripFract` (Grip Fraction) of the **Front Left (FL)** and **Front Right (FR)** tires.
*   **Mechanism**: Modulates the main steering force.
    *   `Output = GameForce * (1.0 - (1.0 - FrontGrip) * UndersteerGain)`
    *   As front grip drops, the wheel becomes lighter ("goes light"), simulating the loss of pneumatic trail and self-aligning torque.
*   **Customization**:
    *   **Understeer Effect (Slider)**: Controls the intensity of the lightening effect.

## 2. Oversteer (Rear Grip Loss)
*   **Goal**: To communicate when the rear tires are losing grip (loose/sliding), allowing the driver to catch a slide early.
*   **Telemetry**: Derived from `mGripFract` of the **Rear Left (RL)** and **Rear Right (RR)** tires, and `mLocalAccel.x` (Lateral G).
*   **Mechanism**:
    1.  **Seat of Pants (SoP)**: Injects Lateral G-force into the wheel torque. This provides a "weight" cue that correlates with the car's yaw/cornering force.
    2.  **Rear Traction Loss**: Monitors the rear tires. If rear grip drops significantly while the front has grip, it boosts the SoP effect or adds a specific vibration/cue to signal rotation.
*   **Customization**:
    *   **SoP Effect (Slider)**: Controls the strength of the Lateral G injection.
    *   **Oversteer Boost (Slider)**: Controls how much the SoP effect is amplified when rear grip is lost.

## 3. Braking Lockup
*   **Goal**: To signal when tires have stopped rotating during braking (flat-spotting risk).
*   **Telemetry**:
    *   `mUnfilteredBrake` > 0.
    *   `mSlipRatio` or `mGripFract`: If the tire rotation speed deviates massively from the road speed (Slip Ratio -> -1.0 means locked).
*   **Mechanism**: Injects a high-frequency, harsh vibration ("Rumble") when locking is detected.
*   **Customization**:
    *   **Lockup Rumble (Toggle)**: Enable/Disable.
    *   **Lockup Gain (Slider)**: Intensity of the vibration.

## 4. Wheel Spin (Acceleration Slip)
*   **Goal**: To signal when the driven wheels are spinning under power.
*   **Telemetry**:
    *   `mUnfilteredThrottle` > 0.
    *   `mSlipRatio`: If positive and high (wheel spinning faster than road speed).
*   **Mechanism**: Injects a medium-frequency vibration ("Scrub") to indicate power-slide or burnout.
*   **Customization**:
    *   **Wheel Spin Rumble (Toggle)**: Enable/Disable.
    *   **Wheel Spin Gain (Slider)**: Intensity.

## 5. Road & Slide Texture
*   **Slide Texture**: Adds "scrubbing" vibration when any tire is sliding laterally (high Slip Angle).
*   **Road Texture**: Adds "bumps" based on suspension velocity changes (High-Pass Filter).

---

## Comparison of Implementation

| Effect | iRFFB (iRacing) | Marvin's AIRA (iRacing) | LMUFFB (LMU/rF2) |
| :--- | :--- | :--- | :--- |
| **Oversteer** | **SoP (Lateral G)** + Yaw logic | **Layered Effect**: Separate "Slip" channel. | **SoP + Rear Grip Logic**: Uses direct rear tire grip telemetry to boost SoP. |
| **Lockup** | Not explicit (part of "Understeer" feel in iRacing logic) | **Pedal Haptics** (often sent to pedals, but can be on wheel) | **Wheel Rumble**: Direct rumble effect on the steering wheel when `SlipRatio` indicates locking. |
| **Wheel Spin** | Not explicit | **Pedal Haptics** / Wheel Rumble | **Wheel Rumble**: Direct rumble effect when `SlipRatio` indicates positive spin. |
