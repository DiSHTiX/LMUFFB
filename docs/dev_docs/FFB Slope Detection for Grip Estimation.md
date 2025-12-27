# Query and plan

## Query 

How effective and accurate is the Slope Detection logic for a FFB app, in order to estimate the grip level, optimal slip angle, and optimal slip ratio for the FFB signal? What are the challenges and limitations?

## Plan

(1) Research the concept of "Slope Detection logic" in the context of Sim Racing Force Feedback (FFB) applications to understand how it analyzes the relationship between Self Aligning Torque (SAT) and tire slip.
(2) Investigate the theoretical method of using the derivative (slope) of the Force vs. Slip curve to identify the peak friction point, corresponding to the Optimal Slip Angle and Optimal Slip Ratio.
(3) Assess the accuracy of slope-based estimation in determining the actual Grip Level compared to ground-truth telemetry data from simulation engines.
(4) Analyze the effectiveness of this logic in communicating the limit of adhesion to the driver, specifically looking for how well it translates the drop in pneumatic trail into perceptible FFB cues.
(5) Identify signal processing challenges, such as the impact of high-frequency noise on derivative calculations and the trade-offs between smoothing filters and input latency.
(6) Explore limitations caused by different tire models (e.g., peaky vs. plateau friction curves) and suspension geometries (e.g., caster angles) that may decouple the SAT peak from the lateral force peak.
(7) Examine how external variables like road texture, curbs, and rapid load transfer affect the reliability of the slope detection algorithm in isolating tire physics from environmental forces.

