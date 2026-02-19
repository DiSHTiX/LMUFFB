# Deep Research Query: FFB Strength and Tire Load Normalization for Telemetry-Based Apps



## Research Objective

Investigate and define the optimal mathematical and logic frameworks for normalizing Force Feedback (FFB) strength and tire-load-dependent tactile effects across diverse vehicle classes in a racing simulator. The goal is to provide a consistent "weight" and "detail" experience for the user regardless of whether they are driving a low-downforce GT3 car or a high-downforce Hypercar, while accounting for varying physical wheelbase torque capabilities.



## Context: The LMUFFB App

LMUFFB is a C++ application that intercepts telemetry from Le Mans Ultimate (similar to rFactor 2) and generates its own FFB signal using DirectInput.

- It uses the game's **Steering Shaft Torque** (in Nm) as a base.

- It calculates additional effects (Understeer, SoP, Road Texture, ABS/Lockup) using telemetry like **Tire Load** (in Newtons), **Slip Angles**, and **G-forces**.



## Technical Problem 1: Overall FFB Strength (Steering Weight)

Different car classes produce vastly different peak steering torque values:

- **GT3/GTE**: Peak torque at the rack is roughly **25-30 Nm**.

- **Hypercars/Prototypes**: Peak torque can reach **45-50 Nm** due to massive aerodynamic downforce.



**The Issue**: If a user sets their "Master Gain" or "Max Torque Reference" for a GT3 car, the FFB will be way too heavy or clip heavily when they switch to a Hypercar. Conversely, settings tuned for a Hypercar feel "limp" in a GT3.



**Current Implementation**: No automatic normalization. Users must manually change a `max_torque_ref` setting or `gain`.

**Proposed Implementation**: Use a class-aware lookup table of "Reference Peak Torques" (e.g., 25Nm for GT3, 45Nm for HC) to scale the base Nm signal to a common internal baseline (e.g., 30Nm) before final output scaling.



### Investigation Questions for Strength:

1. What is the standard industry approach for "Auto-Strength" in FFB utilities (e.g., irFFB, custom Simucube profiles)?

2. Should normalization be based on a **fixed class-based constant**, or a **session-learned peak** (monitoring the maximum Nm actually seen during a few laps)?

3. How do simulators like iRacing or Assetto Corsa handle the "Max Force" setting vs. the physical car's steering rack physics?

4. Is it better to normalize the input Nm to a common range, or to dynamically adjust the output gain?



## Technical Problem 2: Tire Load Normalization (Tactile Effects)

Effects like **Road Texture** and **Braking Haptics** are scaled by the current tire load to make them feel "heavier" and more detailed at high speeds/loads.

- **GT3 Static Load**: ~3500N per corner.

- **Hypercar High-Speed Load**: Can exceed **12000N** due to aero.



**The Issue**: If these effects are normalized by a "chasing peak" (the highest load seen in the session so far), the normalization factor eventually reaches 1.0 even at high speed, meaning the "extra detail" intended for high-load scenarios is neutralized.



**Current Implementation (WIP)**: Move from "Chasing Peak" to **Static Load Baseline**.

- Use the corner weight of the car at rest (or learned at low speeds) as the 1.0x baseline.

- At high speeds, the `Load / StaticLoad` ratio might be 2.5x or 3.0x, allowing road textures to naturally become stronger as aero load increases.



### Investigation Questions for Load:

1. What is the mathematically correct way to normalize tactile effects that depend on tire load? Should they be relative to **Static Weight**, **Suspension Travel**, or a **Global Peak**?

2. How do reputable FFB algorithms (like those in high-end direct drive software) handle the scaling of "Road Feel" as a function of speed and downforce?

3. Should there be a "soft-knee" or logarithmic compression applied to load-scaling to prevent haptic effects from becoming violent at 300km/h while remaining perceptible at 50km/h?



## Proposed Solutions to Analyze:

1. **Class-Aware Reference Mapping**: Using a lookup table (GT3=X Nm, HC=Y Nm) to normalize base forces.

2. **Dynamic Peak Chasing with Hysteresis**: Learning the session peak Nm and using it as the 1.0x reference, but with very slow decay.

3. **Static Load Anchoring**: Normalizing tactile effects against the static corner weight (learned at < 20 km/h).

4. **Pure Physics Passthrough**: Avoiding normalization and letting the user's "Max Torque" setting handle everything (with potential for frequent clipping).



## Search Keywords & References:

- "FFB Auto Strength algorithms"

- "Steering rack torque normalization in racing simulators"

- "Telemetry-based FFB road texture scaling tire load"

- "irFFB Auto-Strength implementation details"

- "Wheelbase torque clipping vs car steering Nm"

- "Tactile effect normalization static vs dynamic load"