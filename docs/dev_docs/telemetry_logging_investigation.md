# Telemetry Logging Investigation (Motec/CSV)

## Goal
Enable logging of LMU/rFactor 2 telemetry data to analyze physics inputs (Suspension Deflection, Slip Angle, etc.) and FFB Output. This will help in tuning the physics engine and debugging issues.

## Proposed Format
We should support **CSV** initially for simplicity, and potentially **Motec i2 Pro** (ld file) later if needed, though CSV is often readable by generic tools.

## Data Points to Log
From `rF2Data.h`:
- Time (mDeltaTime, or absolute time)
- Inputs: Throttle, Brake, Steering
- Car Physics: 
  - LocalAccel (X, Y, Z)
  - LocalVel (X, Y, Z)
  - LocalRot (Yaw, Pitch, Roll)
- Wheel Physics (FL, FR, RL, RR):
  - mSlipAngle
  - mSlipRatio
  - mTireLoad
  - mVerticalTireDeflection
  - mLateralPatchVel (Critical for slide effect)
- FFB Output:
  - Final calculated force
  - Contribution from individual effects (Lockup, Slide, SoP)

## Implementation Plan (Future)
1. Create a `TelemetryLogger` class.
2. Run it on a separate low-priority thread (or the same thread if using buffered I/O) to avoid blocking the FFB loop.
3. Use a ring buffer to store frames, write to disk in chunks.
4. Add a GUI toggle "Record Telemetry".

## Risks
- Disk I/O latency could stall the FFB loop if done synchronously. **Must be asynchronous.**
- File size growth (400Hz logging = huge files). Maybe decimate to 50Hz or 100Hz for logging.
