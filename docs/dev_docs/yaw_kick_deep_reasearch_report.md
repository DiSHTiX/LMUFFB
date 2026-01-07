# **Advanced Implementation of Yaw Acceleration Feedback in Racing Simulation**

## **1\. Theoretical Framework of Vehicle Lateral Dynamics**

The fidelity of racing simulation relies not merely on the visual representation of speed, but on the accurate transmission of mechanical forces to the driver. In the domain of fixed-base simulators, where the vestibular cues of linear and rotational acceleration are absent, the steering torque—Force Feedback (FFB)—becomes the primary channel for communicating the vehicle's dynamic state. This report provides an exhaustive analysis of implementing a "Yaw Kick" effect based on yaw acceleration telemetry (mLocalRotAccel.y), specifically within the *rFactor 2* and *Le Mans Ultimate* ecosystems.

### **1.1 The Physics of Yaw Acceleration**

To validate amplitude ranges and processing strategies, one must first ground the analysis in the fundamental equations of motion governing a race car. Yaw acceleration ($\\ddot{\\psi}$ or $\\dot{r}$) is the time derivative of yaw rate ($r$). It serves as a direct metric of the net unbalanced yaw moment acting on the vehicle chassis.

According to Newton’s Second Law for rotation, the relationship is defined as:

$$\\sum M\_z \= I\_{zz} \\cdot \\ddot{\\psi}$$  
Where $\\sum M\_z$ is the summation of all yaw moments acting on the Center of Gravity (CoG) and $I\_{zz}$ is the vehicle's polar moment of inertia about the vertical axis.1 In a GT3 or LMP vehicle, these moments are generated primarily by tire forces (lateral and longitudinal) and aerodynamic forces.

#### **1.1.1 The Steady-State Null Hypothesis**

During steady-state cornering, a vehicle traverses a constant radius arc at a constant velocity. In this condition, the sum of moments is zero (or balanced by the damping of the yaw velocity), and the yaw rate is constant. Consequently, the yaw acceleration is theoretically zero.1

* **Implication for FFB:** Any non-zero mLocalRotAccel signal detected during a perfectly smooth, steady-state corner represents either signal noise, road texture interactions, or micro-corrections by the driver. It does *not* represent a change in the vehicle's handling state. Therefore, a robust FFB algorithm must essentially ignore or heavily damp signals in the near-zero range to preserve the "clean" feeling of alignment torque.2

#### **1.1.2 The Transient Event: Oversteer Onset**

The "Yaw Kick" target phenomenon—oversteer onset—is a transient dynamic event. It occurs when the rear tires saturate (exceed their peak slip angle). At this moment, the restoring lateral force at the rear axle diminishes or plateaus, while the front axle may still be generating increasing force (or the driver may be applying throttle, adding a yaw moment via torque vectoring or differential locking).

* **The Moment Imbalance:** This loss of rear grip creates a sudden, massive imbalance in $\\sum M\_z$. Since the polar moment of inertia ($I\_{zz}$) is a constant physical property of the car, this unbalanced moment directly translates into a spike in yaw acceleration ($\\ddot{\\psi}$).  
* **The Haptic Cue:** In a real car, the driver feels this rotation acceleration through the inner ear (semicircular canals) before visual cues (horizon rotation) become apparent. In simulation, this vestibular cue is missing. The "Yaw Kick" attempts to substitute this by injecting a torque impulse into the steering wheel that mimics the *jerk* or sudden acceleration of the chassis, alerting the driver to the loss of rear authority.3

### **1.2 Telemetry Architecture in rFactor 2**

The specific environment for this application is the *rFactor 2* physics engine (isiMotor2.5), utilized by *Le Mans Ultimate*. The telemetry variable mLocalRotAccel is exposed via the internal plugin interface.

* **Coordinate System:** The engine uses a local coordinate system where y represents the vertical axis. Thus, mLocalRotAccel.y is the rotational acceleration around the vertical axis—pure yaw.4  
* **Signal Purity:** Unlike filtered outputs from some consumer sims, *rFactor 2* often outputs raw physics calculation steps. This means mLocalRotAccel can contain high-frequency noise resulting from the rigid-body solver interacting with the high-frequency tire model (CPM \- Contact Patch Model). This necessitates a rigorous signal processing stage to make the data usable for haptics.6

## ---

**2\. Validation of Amplitude Ranges**

A critical objective is to determine typical magnitudes for yaw acceleration to establish valid thresholds for signal processing. Analysis of vehicle dynamics literature, telemetry logs from GT3/GTE machinery, and simulator data confirms the user's estimated ranges are scientifically sound, though nuanced by vehicle configuration.

### **2.1 Normal Cornering and Smooth Surfaces**

Range: 0.0 to 0.5 rad/s²  
As established, steady-state cornering yields near-zero yaw acceleration. However, real-world data and high-fidelity simulation are rarely "steady."

* **Micro-corrections:** A professional driver makes continuous micro-adjustments to the steering angle ($\\delta$) to maintain the limit. These inputs create small fluctuations in yaw moment.  
* **Aerodynamic Buffeting:** At high speeds (e.g., Mulsanne Straight), aerodynamic turbulence can induce minor yaw oscillations.  
* **Analysis:** Telemetry traces from GTE cars (e.g., approx. 2011 spec) at Silverstone show that during the mid-corner phase, yaw rate changes are gradual. The derivative (acceleration) remains low, typically below **0.2 rad/s²**.7  
* **Validation:** The user's estimate of **0.2 to 1.0 rad/s²** for "Road Details / Micro-corrections" is accurate, though "Normal Cornering" on a smooth track is likely at the bottom end of this (0.0 \- 0.2 rad/s²).

### **2.2 Road Imperfections and Surface Texture**

Range: 0.5 to 2.0 rad/s² (High Frequency)  
When a vehicle strikes a curb or traverses rough asphalt, the vertical load on the tires ($F\_z$) fluctuates rapidly. Since lateral force ($F\_y$) is a function of vertical load ($F\_y \\approx \\mu F\_z$), these vertical oscillations cause rapid, high-frequency fluctuations in the yaw moment.

* **Characteristics:** These events are distinguished by their oscillatory nature. The car is not rotating into a new trajectory; it is vibrating around its current trajectory.  
* **Magnitude:** Curb strikes can generate high instantaneous peaks. Telemetry from GT3 cars hitting "sausage curbs" can show spikes exceeding **5.0 rad/s²**, but the *duration* is milliseconds. The energy (integral of acceleration) is low.2  
* **Threshold Conflict:** This overlaps with the magnitude of a slide initiation. A simple amplitude threshold of 2.0 rad/s² might trigger a "Yaw Kick" on a curb strike, confusing the driver. This necessitates frequency-domain separation (discussed in Section 3).

### **2.3 Onset of Oversteer (Slide Initiation)**

Range: 2.0 to 6.0 rad/s² (Step Function)  
This is the signal of interest. The transition from "grip" to "slip" at the rear axle is often abrupt in modern GT3/LMP cars due to stiff sidewall construction and aerodynamic reliance.

* **Dynamics:** A "snap" oversteer event is characterized by a rapid divergence in yaw rate. If a car's yaw rate accelerates from 2 deg/s to 30 deg/s (\~0.5 rad/s) in 0.2 seconds, the average acceleration is 2.5 rad/s². Peak instantaneous acceleration during the "break" of traction often reaches **4.0 to 6.0 rad/s²**.  
* **Validation:** The user's estimate of **\> 2.0 rad/s²** is a scientifically valid baseline for detecting the *initiation* of a meaningful handling event.8 This aligns with Electronic Stability Control (ESC) intervention thresholds, which typically monitor for yaw rate deviations that imply accelerations in this magnitude range.10

### **2.4 Full Spin / Loss of Control**

Range: 6.0 to \>15.0 rad/s²  
Once the vehicle has fully departed controlled flight, aerodynamic damping and tire sliding friction are the only forces opposing the spin.

* **Magnitude:** In a violent spin, particularly one induced by a collision or touching grass under power, yaw acceleration can spike massively.  
* **Normalization:** The user's choice of **10.0 rad/s²** as a "maximum reference" is robust. While physical peaks may exceed this, the useful *haptic* range ends here. Differentiating between a 10 rad/s² spin and a 15 rad/s² spin provides no actionable information to the driver—both require maximum counter-correction. Mapping 10.0 rad/s² to 100% force output ensures the driver feels the limit of the hardware's capability during terminal events.9

### **2.5 Summary Table of Amplitude Ranges**

| Vehicle State | Yaw Accel (ψ¨​) Range | Frequency Character | Driver Action Required | Signal Type |
| :---- | :---- | :---- | :---- | :---- |
| **Steady Cornering** | 0.0 – 0.2 rad/s² | DC / Low (\< 1 Hz) | Maintenance | Noise / Null |
| **Road Texture** | 0.2 – 1.0 rad/s² | High (20 – 100 Hz) | Ignore (Filtering) | Oscillatory |
| **Curb Strike** | 1.0 – 8.0 rad/s² | Very High (\> 50 Hz) | Ignore (Filtering) | Impulse / Spike |
| **Slide Initiation** | **2.0 – 5.0 rad/s²** | **Low-Mid (2 – 10 Hz)** | **Counter-Steer** | **Step / Surge** |
| **Deep Slide / Spin** | 5.0 – \>10.0 rad/s² | Low (0.5 – 2 Hz) | Full Lock / Brake | Sustained Surge |

## ---

**3\. Signal Characteristics and Processing Architectures**

The central challenge identified in the research is distinguishing "Road Texture" (which can have high acceleration but is irrelevant to handling) from "Slide Onset" (which is the target cue). The research indicates that while amplitude discrimination is useful, **Frequency Domain Analysis** is the decisive factor.

### **3.1 Frequency Domain Differentiation**

Vehicle handling dynamics (macroscopic movement of the chassis mass) and road texture (vibration of unsprung mass) occupy distinct frequency spectra.

* **Handling Band (0 \- 5 Hz):** The yaw motion of a vehicle is governed by its large polar moment of inertia ($I\_{zz}$), typically around 2500-3000 kg·m² for a GT car.8 This massive inertia acts as a physical low-pass filter. It is physically impossible for the chassis to oscillate in yaw at 20 Hz. Therefore, true oversteer events, even "snappy" ones, occur in the **0.5 Hz to 5 Hz** range.12  
* **Texture/Vibration Band (10 \- 100 Hz):** Road texture inputs, engine vibrations, and suspension harmonics occur at much higher frequencies. Steering wheel vibrations specifically peak around **35 \- 50 Hz** (primary resonance) and **70 Hz** (harmonics).14  
* **Separation Strategy:** This clear spectral separation validates the use of frequency-based filtering. A slide is a *low-frequency, high-amplitude* event. Texture is a *high-frequency, variable-amplitude* event.

#### **3.1.1 The Low-Pass Filter (LPF) Solution**

To isolate the "Yaw Kick," a **Low-Pass Filter** is strictly required.

* **Cutoff Frequency:** The research suggests a cutoff frequency ($f\_c$) between **8 Hz and 12 Hz**. This passes the fastest possible chassis yaw movements (limits of handling) while attenuating the 20Hz+ road noise and curb spikes.15  
* **Filter Topology:** A **2nd-order Butterworth** filter is recommended. It offers a flat passband (preserving the magnitude of the slide signal) and a steep roll-off (-12 dB/octave) to crush high-frequency noise. A Moving Average filter should be avoided as it introduces variable latency, which is detrimental to the "reflex" nature of catching a slide.

### **3.2 Gamma Transformation vs. Derivative Analysis**

The user inquires about using Gamma curves or Derivative analysis for signal conditioning.

#### **3.2.1 Gamma / Power Law ($\\gamma$)**

Gamma transformation maps the input signal $x$ to output $y$ via $y \= x^{\\gamma}$.

* **Mechanism:** In FFB, a Gamma value $\> 1.0$ (e.g., 1.5 or 2.0) creates a curve that is flat near zero and steepens at higher values.17  
* **Application to Yaw:** This is highly effective for the "Yaw Kick."  
  * *Region 0 \- 1.5 rad/s²:* The curve suppresses this "grey zone" of micro-corrections and road noise, acting as a "soft" deadzone.  
  * *Region \> 2.0 rad/s²:* The curve ramps up gain aggressively.  
* **Recommendation:** Gamma is superior to a hard deadzone because it eliminates the "clunk" or "step" feeling when the signal crosses the threshold. It provides organic differentiation between "texture" (damped) and "slide" (amplified).

#### **3.2.2 Dynamic Range Compression (DRC)**

While referenced in audio engineering 18, DRC is generally **inappropriate** for this specific alert signal.

* *Audio Context:* DRC makes quiet sounds louder.  
* *FFB Context:* Applying DRC would amplify road noise and steering jitter, raising the "noise floor" of the force feedback.  
* *Inverse Approach:* An **Expander** or **Noise Gate** is the correct audio analog. We want to *silence* the quiet signals and *pass* the loud ones. The Gamma function described above effectively acts as a static Expander.

#### **3.2.3 Derivative Analysis (Jerk)**

Calculating the derivative of acceleration ($\\dddot{\\psi}$, or Yaw Jerk) is theoretically attractive because it highlights the *change* in state.3

* **The Noise Problem:** Numerical differentiation amplifies noise. Telemetry from simulators is discrete (sampled). Differentiating a noisy 400Hz signal twice (Position $\\to$ Velocity $\\to$ Accel $\\to$ Jerk) results in a signal dominated by quantization noise unless heavily smoothed.  
* **Verdict:** Avoid calculating Jerk directly. The "Step" nature of the Yaw Acceleration signal during a slide is already perceived as a "Kick" (Jerk) by the human hand.

### **3.3 Proposed Signal Processing Pipeline**

Based on the synthesis of these findings, the following real-time processing pipeline is proposed for the application:

1. **Input:** mLocalRotAccel.y (Raw Telemetry).  
2. **Pre-Filter:** Low-Pass Butterworth Filter ($f\_c \= 10$ Hz). Removes road noise/curb spikes.  
3. **Normalization:** $NormSignal \= \\min(|FilteredSignal| / 10.0, 1.0)$. Maps 0-10 rad/s² to 0.0-1.0.  
4. **Non-Linearity:** $ShapedSignal \= (NormSignal)^{\\gamma}$. Where $\\gamma \\approx 1.5 \- 2.0$. Suppresses low-amplitude noise.  
5. **Directionality Check:** Multiply by $-1 \\times \\text{sign}(v\_x)$ (if necessary for coordinate matching) to ensuring the torque opposes the slide.  
6. **Output:** Send to FFB mixing stage.

## ---

**4\. Best Practices for Yaw-to-FFB Mapping**

Integrating this processed signal into the final Force Feedback output requires an understanding of both simulator hardware and human physiology.

### **4.1 Usage in Professional and Commercial Systems**

* **Motion Simulators:** In professional setups (e.g., Cruden, Ansible), yaw acceleration is mapped primarily to the **Motion Platform** (vestibular cueing) rather than the steering wheel.20 The steering wheel is reserved for Self-Aligning Torque (SAT).  
* **Commercial FFB (Simucube/Fanatec):** Direct Drive software often includes "Slew Rate Limits" and "Reconstruction Filters".19 These are essentially hardware-level low-pass filters.  
* **The "Yaw Kick" as an Artificial Effect:** It is crucial to recognize that the "Yaw Kick" is a *canned effect* (synthetic). In a real car, the steering wheel does not necessarily "kick" due to yaw acceleration; it goes light due to the drop in pneumatic trail. The "Kick" is a substitute for the missing G-force. Therefore, it should be layered *on top* of the physics-based SAT, not replace it.

### **4.2 Human Perception Thresholds**

To tune the effect effectively, one must adhere to the psychophysical limits of the human driver.

#### **4.2.1 Absolute Detection Thresholds**

* **Torque:** The human hand is remarkably sensitive. In a static environment, the detection threshold for steering torque is approximately **0.02 to 0.1 Nm**.22  
* **Vibration:** For vibrotactile cues, humans can detect accelerations as low as **0.05 m/s²** at the steering rim.24  
* **Implication:** The "Yaw Kick" does not need to be massive to be felt. A sudden impulse of **0.5 Nm** is well above the absolute threshold.

#### **4.2.2 Just Noticeable Difference (JND)**

However, racing is not a static environment. The driver is already fighting cornering forces (SAT). The ability to detect a *change* in force (the Kick) against a background force (SAT) is governed by **Weber's Law**.

* **Weber Fraction:** Research indicates the Weber fraction for hand/arm force perception is between **7% and 15%**.26  
* **Dynamic Scaling:** This is a critical insight for the algorithm.  
  * If the wheel is loaded with **5 Nm** of cornering force, a "Kick" of 0.2 Nm (4%) will likely be **imperceptible**. The Kick must be at least $5.0 \\times 0.10 \= 0.5$ Nm.  
  * If the wheel is loaded with **15 Nm** (high downforce), the Kick needs to be **1.5 Nm** to be noticed.  
* **Best Practice:** The gain of the Yaw Kick effect should be **adaptive**. It should scale with the instantaneous FFB load (or an average thereof) to maintain perceptibility without becoming dangerously violent at low speeds.

### **4.3 Reference Values and Standardization**

* **Maximum Reference:** The user's **10.0 rad/s²** reference is appropriate for normalization. While a collision might generate 20 rad/s², 10.0 rad/s² represents the upper limit of *recoverable* vehicle dynamics.  
* **Standards:** There is no ISO standard specifically for "Sim Racing Yaw Kick." However, automotive standards for **Electronic Stability Control (ESC)** activation provide the best proxy. ESC systems trigger when yaw rate deviation suggests an unrecoverable trajectory—often correlating to yaw accelerations in the **2.0 \- 4.0 rad/s²** range.10 Adopting these automotive safety thresholds gives the simulation effect a basis in real-world vehicle dynamics engineering.

## ---

**5\. Related Concepts and Advanced Algorithms**

The user asks how yaw acceleration relates to other metrics and if combined algorithms are better.

### **5.1 Relation to Sideslip and Yaw Rate**

Yaw acceleration is the precursor to sideslip.

* **Sequence of Events:** Unbalanced Moment $\\to$ **Yaw Acceleration** $\\to$ Change in Yaw Rate $\\to$ Change in Heading $\\to$ **Sideslip Angle** ($\\beta$).  
* **Latency:** Using yaw acceleration allows the FFB to alert the driver *before* the sideslip angle becomes large. It is a **leading indicator**, whereas sideslip is a **lagging indicator** of the moment imbalance.30

### **5.2 The "Stability Index" Algorithm**

The user asks if combining metrics is more robust. Yes. Relying solely on yaw acceleration can yield false positives (e.g., aggressive turn-in for a slalom produces high yaw accel but is stable).

* **The Phase Plane Approach:** Vehicle stability is best defined by the **Phase Plane** ($\\beta$ vs $\\dot{\\beta}$ or $r$). Stability boundaries are defined as regions in this plane.32  
* Proposed "Gated" Algorithm:  
  Instead of a simple threshold, use a logical gate:  
  * **Condition A:** Abs(RearSlipAngle) \> Peak\_Slip\_Angle (approx 3-5 degrees for slicks). This confirms the rear tires are saturated.34  
  * **Condition B:** YawAcceleration \> 2.0 rad/s². This confirms the car is rotating rapidly.  
  * **Trigger:** Activate "Yaw Kick" only when **A AND B** are true.  
  * **Result:** This filter eliminates false positives from "agile" maneuvers (high yaw accel, low slip) and ensures the kick is reserved for genuine loss-of-control events, effectively simulating the "seat of the pants" feeling of the rear end stepping out.

## **6\. Recommendations for Implementation in Le Mans Ultimate**

1. **Safety First:** Implement a hard **Output Slew Rate Limiter** on the final torque signal. Physics glitches in rFactor 2 can cause instantaneous infinite acceleration. A limiter (e.g., max 10 Nm/ms change) protects the user's hardware and hands.19  
2. **Context Awareness:** Disable the effect below a minimum speed (e.g., 20 kph) to prevent the wheel from kicking while maneuvering in the pits.1  
3. **User Tuning:** Expose the **Gamma** and **Gain** parameters to the user. Different wheelbases (5 Nm vs 25 Nm) have vastly different dynamic ranges and friction characteristics, requiring different tuning to overcome static friction.35

## **7\. Conclusion**

The "Yaw Kick" is a vital bridge between the mathematical precision of the *rFactor 2* physics engine and the sensory experience of the driver. By validating the **2.0 rad/s²** threshold, utilizing **Low-Pass Filtering (10 Hz)** to separate handling from texture, and applying **Gamma Correction** to organicize the response, a developer can create a highly immersive and informative cue. Furthermore, by adhering to **Weber's Law** for signal scaling and integrating **Sideslip** data for gating, the application moves beyond a simple vibration effect to a sophisticated driver aid that mirrors the logic of real-world stability control systems.

### **Appendix: Data Tables**

**Table 1: Validated Yaw Acceleration Ranges**

| Vehicle State | Yaw Accel (ψ¨​) | Frequency Band | Signal Nature | Action |
| :---- | :---- | :---- | :---- | :---- |
| Steady Cornering | \< 0.2 rad/s² | DC \- 1 Hz | Static / Noise | Filter Out |
| Surface Texture | 0.2 \- 1.5 rad/s² | 20 \- 100 Hz | Oscillatory | Filter Out |
| **Slide Onset** | **2.0 \- 5.0 rad/s²** | **1 \- 10 Hz** | **Impulsive** | **Amplify/Kick** |
| Spin | \> 5.0 rad/s² | 0.5 \- 2 Hz | Ballistic | Saturate |

**Table 2: Perception Thresholds for Tuning**

| Metric | Value | Implications for FFB |
| :---- | :---- | :---- |
| Absolute Torque Threshold | 0.02 \- 0.1 Nm | Minimum force for deadzone removal. |
| Vibrotactile Threshold | 0.05 m/s² | Limit for "road texture" effects. |
| **Weber Fraction (JND)** | **7% \- 15%** | **Kick Strength must be \>10% of current FFB torque.** |

**Table 3: Signal Processing Specifications**

| Component | Specification | Rationale |
| :---- | :---- | :---- |
| **Primary Filter** | 2nd-Order Butterworth LPF | Cutoff @ 10 Hz to remove road noise. |
| **Non-Linearity** | Gamma $\\gamma \\approx 1.8$ | Softens transition; suppresses micro-corrections. |
| **Gating Metric** | Rear Slip Angle \> 4° | Prevents false positives during agile cornering. |
| **Max Reference** | 10.0 rad/s² | Normalization ceiling for 100% output. |

#### **Works cited**

1. Getting to grips with your yaw moments \- OptimumG Students, accessed January 7, 2026, [https://students.optimumg.com/wp-content/uploads/2017/05/Gettingtogrips.pdf](https://students.optimumg.com/wp-content/uploads/2017/05/Gettingtogrips.pdf)  
2. VEHICLE DYNAMICS, accessed January 7, 2026, [https://www.jsae.or.jp/files\_publish/page/1219/en\_vol74\_8-17.pdf](https://www.jsae.or.jp/files_publish/page/1219/en_vol74_8-17.pdf)  
3. lmuFFB App | Page 6 | Le Mans Ultimate Community, accessed January 7, 2026, [https://community.lemansultimate.com/index.php?threads/lmuffb-app.10440/page-6](https://community.lemansultimate.com/index.php?threads/lmuffb-app.10440/page-6)  
4. rF2SharedMemoryMapPlugin/Monitor/rF2SMMonitor/rF2SMMonitor/rF2Data.cs at master · TheIronWolfModding/rF2SharedMemoryMapPlugin \- GitHub, accessed January 7, 2026, [https://github.com/TheIronWolfModding/rF2SharedMemoryMapPlugin/blob/master/Monitor/rF2SMMonitor/rF2SMMonitor/rF2Data.cs](https://github.com/TheIronWolfModding/rF2SharedMemoryMapPlugin/blob/master/Monitor/rF2SMMonitor/rF2SMMonitor/rF2Data.cs)  
5. SimTelemetry/SimTelemetry.Game.Rfactor/GamePlugin/Include/InternalsPlugin.hpp at master · nlhans/SimTelemetry · GitHub, accessed January 7, 2026, [https://github.com/nlhans/SimTelemetry/blob/master/SimTelemetry.Game.Rfactor/GamePlugin/Include/InternalsPlugin.hpp](https://github.com/nlhans/SimTelemetry/blob/master/SimTelemetry.Game.Rfactor/GamePlugin/Include/InternalsPlugin.hpp)  
6. SimTelemetry/SimTelemetry.Game.Rfactor/GamePlugin/Source/PluginData.cpp at master · nlhans/SimTelemetry · GitHub, accessed January 7, 2026, [https://github.com/nlhans/SimTelemetry/blob/master/SimTelemetry.Game.Rfactor/GamePlugin/Source/PluginData.cpp](https://github.com/nlhans/SimTelemetry/blob/master/SimTelemetry.Game.Rfactor/GamePlugin/Source/PluginData.cpp)  
7. Integrated Optimisation for Dynamic Modelling, Path Planning and Energy Management in Hybrid Race Vehicles by Kieran Reeves MSc \- Lancaster EPrints, accessed January 7, 2026, [https://eprints.lancs.ac.uk/id/eprint/153035/1/2020ReevesPhD.pdf](https://eprints.lancs.ac.uk/id/eprint/153035/1/2020ReevesPhD.pdf)  
8. Final Thesis PDF \- Auburn University, accessed January 7, 2026, [https://etd.auburn.edu/bitstream/handle/10415/379/WHITEHEAD\_RANDALL\_4.pdf?isAllowed=y\&sequence=1](https://etd.auburn.edu/bitstream/handle/10415/379/WHITEHEAD_RANDALL_4.pdf?isAllowed=y&sequence=1)  
9. Research on Yaw Moment Control System for Race Cars Using ..., accessed January 7, 2026, [https://www.mdpi.com/2624-8921/5/2/29](https://www.mdpi.com/2624-8921/5/2/29)  
10. Electronic stability control \- Wikipedia, accessed January 7, 2026, [https://en.wikipedia.org/wiki/Electronic\_stability\_control](https://en.wikipedia.org/wiki/Electronic_stability_control)  
11. Development of an Electronic Stability Control Algorithm for All-Terrain Vehicles, accessed January 7, 2026, [https://www.sae.org/publications/technical-papers/content/2023-01-0661/](https://www.sae.org/publications/technical-papers/content/2023-01-0661/)  
12. On the Frequency Domain Analysis of Tire Relaxation Effects on Transient On-Center Vehicle Handling Performance, accessed January 7, 2026, [https://cecas.clemson.edu/ayalew/Papers/Vehicle%20Systems%20Dynamics%20and%20Control/Papers/On%20the%20Frequency%20Domain%20Analysis%20of%20Tire%20Relaxation%20Effects%20on%20Transient%20On-Center%20Vehicle%20Handling%20Performance/755\_1.pdf](https://cecas.clemson.edu/ayalew/Papers/Vehicle%20Systems%20Dynamics%20and%20Control/Papers/On%20the%20Frequency%20Domain%20Analysis%20of%20Tire%20Relaxation%20Effects%20on%20Transient%20On-Center%20Vehicle%20Handling%20Performance/755_1.pdf)  
13. Road safety: The influence of vibration frequency on driver drowsiness, reaction time, and driving performance \- PubMed, accessed January 7, 2026, [https://pubmed.ncbi.nlm.nih.gov/37813019/](https://pubmed.ncbi.nlm.nih.gov/37813019/)  
14. Analysis of the Impact of Vibrations on the Driver of a Motor Vehicle \- MDPI, accessed January 7, 2026, [https://www.mdpi.com/2076-3417/15/10/5510](https://www.mdpi.com/2076-3417/15/10/5510)  
15. Model Based Handling Analyses \- Diva-portal.org, accessed January 7, 2026, [http://www.diva-portal.org/smash/get/diva2:1285767/FULLTEXT01.pdf](http://www.diva-portal.org/smash/get/diva2:1285767/FULLTEXT01.pdf)  
16. What is the most common vibration frequency in a vehicle travelling down a highway?, accessed January 7, 2026, [https://www.researchgate.net/post/What-is-the-most-common-vibration-frequency-in-a-vehicle-travelling-down-a-highway](https://www.researchgate.net/post/What-is-the-most-common-vibration-frequency-in-a-vehicle-travelling-down-a-highway)  
17. What is gamma in steerng wheel settings? : r/assettocorsa \- Reddit, accessed January 7, 2026, [https://www.reddit.com/r/assettocorsa/comments/k9kf0f/what\_is\_gamma\_in\_steerng\_wheel\_settings/](https://www.reddit.com/r/assettocorsa/comments/k9kf0f/what_is_gamma_in_steerng_wheel_settings/)  
18. Dynamic Range Across Music Genres and the Perception of Dynamic Compression in Hearing-Impaired Listeners \- PMC \- PubMed Central, accessed January 7, 2026, [https://pmc.ncbi.nlm.nih.gov/articles/PMC4753356/](https://pmc.ncbi.nlm.nih.gov/articles/PMC4753356/)  
19. Assetto Corsa and Simucube 2 \- Games \- Granite Devices Community, accessed January 7, 2026, [https://community.granitedevices.com/t/assetto-corsa-and-simucube-2/2956?page=28](https://community.granitedevices.com/t/assetto-corsa-and-simucube-2/2956?page=28)  
20. GRID1: The 1DOF yaw racing simulator for eSports \- SimCraft, accessed January 7, 2026, [https://simcraft.com/racing-simulators/products/grid-1/](https://simcraft.com/racing-simulators/products/grid-1/)  
21. A Review of Driving Simulation Technology and Applications | VI-grade, accessed January 7, 2026, [https://www.vi-grade.com/dynatc/attachments-1208-fd32/a-review-of-driving-simulation-technology-and-applications.pdf](https://www.vi-grade.com/dynatc/attachments-1208-fd32/a-review-of-driving-simulation-technology-and-applications.pdf)  
22. Sensitivity to Haptic Sound-Localization Cues at Different Body Locations \- MDPI, accessed January 7, 2026, [https://www.mdpi.com/1424-8220/21/11/3770](https://www.mdpi.com/1424-8220/21/11/3770)  
23. Human Perception Measures for Product Design and Development—A Tutorial to Measurement Methods and Analysis \- MDPI, accessed January 7, 2026, [https://www.mdpi.com/2414-4088/1/4/28](https://www.mdpi.com/2414-4088/1/4/28)  
24. Influence of Steering Wheel Torque Feedback, accessed January 7, 2026, [https://www.nads-sc.uiowa.edu/dscna/2003/papers/Toffin\_Influence%20of%20Steering%20Wheel%20Torque%20Feedback....pdf](https://www.nads-sc.uiowa.edu/dscna/2003/papers/Toffin_Influence%20of%20Steering%20Wheel%20Torque%20Feedback....pdf)  
25. Human, Whole-Body & Hand-Arm Vibration – Online Course \- Dewesoft, accessed January 7, 2026, [https://dewesoft.com/academy/online/human-body-vibration](https://dewesoft.com/academy/online/human-body-vibration)  
26. Driver Perception of Steady-State Steering Feel \- ePrints Soton, accessed January 7, 2026, [https://eprints.soton.ac.uk/466382/1/1125040.pdf](https://eprints.soton.ac.uk/466382/1/1125040.pdf)  
27. The influence of steering wheel size when tuning power assistance \- Chalmers Publication Library, accessed January 7, 2026, [https://publications.lib.chalmers.se/records/fulltext/218165/local\_218165.pdf](https://publications.lib.chalmers.se/records/fulltext/218165/local_218165.pdf)  
28. Driver estimation of steering wheel vibration intensity : questionnaire-based survey \- HCD Studios, accessed January 7, 2026, [https://hcdstudios.com/wp-content/uploads/2019/08/gg2005deo.pdf](https://hcdstudios.com/wp-content/uploads/2019/08/gg2005deo.pdf)  
29. A review of human sensory dynamics for application to models of driver steering and speed control \- PubMed Central, accessed January 7, 2026, [https://pmc.ncbi.nlm.nih.gov/articles/PMC4903114/](https://pmc.ncbi.nlm.nih.gov/articles/PMC4903114/)  
30. Design and Evaluation of Rear Axle Side Slip Stability Control for Passenger Cars \- Chalmers Publication Library, accessed January 7, 2026, [https://publications.lib.chalmers.se/records/fulltext/154376/154376.pdf](https://publications.lib.chalmers.se/records/fulltext/154376/154376.pdf)  
31. Vehicle Sideslip Estimation \- JPL Robotics, accessed January 7, 2026, [https://www-robotics.jpl.nasa.gov/media/documents/Grip\_CSM2009.pdf](https://www-robotics.jpl.nasa.gov/media/documents/Grip_CSM2009.pdf)  
32. Research on control strategy of vehicle stability based on dynamic stable region regression analysis \- Frontiers, accessed January 7, 2026, [https://www.frontiersin.org/journals/neurorobotics/articles/10.3389/fnbot.2023.1149201/full](https://www.frontiersin.org/journals/neurorobotics/articles/10.3389/fnbot.2023.1149201/full)  
33. Simulation and Experimental: Enhanced Stability Control of Electric Vehicle Based on Phase Plane Boundary Analysis \- Journal Article \- SAE Mobilus, accessed January 7, 2026, [https://saemobilus.sae.org/articles/simulation-experimental-enhanced-stability-control-electric-vehicle-based-phase-plane-boundary-analysis-10-09-02-0017](https://saemobilus.sae.org/articles/simulation-experimental-enhanced-stability-control-electric-vehicle-based-phase-plane-boundary-analysis-10-09-02-0017)  
34. Setup steering angle vs. profile steering angle... \- KW Studios Forum, accessed January 7, 2026, [https://forum.kw-studios.com/index.php?threads/setup-steering-angle-vs-profile-steering-angle.14274/](https://forum.kw-studios.com/index.php?threads/setup-steering-angle-vs-profile-steering-angle.14274/)  
35. Thrustmaster T598: FFB Settings in games : r/simracing \- Reddit, accessed January 7, 2026, [https://www.reddit.com/r/simracing/comments/1h2k20h/thrustmaster\_t598\_ffb\_settings\_in\_games/](https://www.reddit.com/r/simracing/comments/1h2k20h/thrustmaster_t598_ffb_settings_in_games/)