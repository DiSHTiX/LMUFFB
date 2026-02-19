Comparing the provided summary with the generated research on Force Feedback (FFB) normalization reveals several key differences in technical depth, implementation strategy, and specific industry references.

### Issues and Corrections for the User's Report

1. **Lack of Signal Conditioning (Soft-Knee Compression):** The report lacks a mechanism for handling signals that exceed the "learned peak." Without a soft-knee or limiting function, any transient spike (like a curb strike or collision) that hasn't been "learned" yet will result in harsh digital clipping.


2. **Inaccuracy in Strength Normalization Table:** The table lists "Static Weight Scaling" as a method for steering weight. In high-fidelity simulations, steering weight is derived from rack torque physics rather than vehicle mass; using mass as a primary scaling factor for steering weight is physically inaccurate.
3. **Linear Decay Rate Concerns:** A linear decay of 0.01 Nm/s is extremely slow. It would take approximately 50 minutes for a 30 Nm peak to decay to zero. This could prevent the application from adapting to car setup changes or different driving styles within a single session without a manual reset.


4. **Telemetry Jitter:** The report does not mention signal filtering (e.g., EMA or low-pass). Telemetry data is often discrete and "notchy"; without filtering, the normalization factors themselves can introduce robotic vibrations into the wheelbase.

### Additional Information in the User's Report

* **rFactor 2 "Torque Capability":** The report identifies rFactor 2's specific implementation of a "Torque Capability" setting, which serves as a manual physical limit for normalization.


* **Specific LMUFFB Issue IDs:** It references internal development markers (Issue #95 and #120), which provide context for the current implementation status.
* **Le Mans Ultimate Internal Values:** While the user report mentions 25-45 Nm for car classes, the underlying app logic for LMUFFB specifically utilized 4000 Nm and 8000 Nm as the internal "peak load" normalization constants for GT3 and Hypercars respectively in version v0.7.45.



### Proposed Solutions and Improvements

* **Refined Learning Window:** The user's suggestion to limit static load learning to a window of 2-15 m/s is a superior practical solution. This range is high enough to avoid sensor noise at a standstill but low enough that aerodynamic downforce does not yet influence the tire load reading, ensuring a pure "static" baseline.


* **Dynamic Peak as Primary Strategy:** The recommendation to use **Session-Learned Peak Normalization** as the primary method is more flexible than a fixed lookup table. Industry standards like iRacing's "Auto" feature and irFFB prove that monitoring `mSteeringShaftTorque` in real-time better handles track-specific setup variations (e.g., high-downforce vs. low-downforce kits).


* **User-Defined Torque Reference:** Scaling the normalization factor to a `m_max_torque_ref` (the user's physical wheel capability) is a better user-centric approach than scaling to an arbitrary internal baseline. It ensures the driver perceives a consistent "percentage" of their wheel's total power across all car classes.



### Summary of Framework Comparison

| Feature | Generated Research | User Summary |
| --- | --- | --- |
| **Primary Strength Logic** | Class-Aware (Fixed) | Dynamic (Learned) |
| **Clipping Management** | Soft-Knee Mathematical Curves | Not Addressed |
| **Load Normalization** | Static Load Anchoring | Static Load Anchoring |
| **Learning Trigger** | Session Initialization | Speed-Gated (2-15 m/s) |
| **Filter Logic** | EMA / Smoothing Tau | Not Addressed |

The user summary is highly effective for a specific implementation roadmap, while the generated research provides the necessary mathematical safety nets (soft-knee, filtering) to ensure those features do not result in a harsh or "robotic" user experience.