--> 

# FFB Mathematical Formulas (v0.6.2) 

The final output sent to the DirectInput driver is a normalized value between **-1.0** and **1.0**. 

## 1. The Master Formula 

where normalization divides by m_max_torque_ref (with a floor of 1.0 Nm). 

The total force is a summation of base physics, seat-of-pants effects, and dynamic vibrations, scaled by the **Traction Loss Multiplier** : 

_Note: reduces total force implementation during wheel spin (see Section E.3)._ 

## 2. Signal Scalers (Decoupling) 

To ensure consistent feel across different wheels (e.g. G29 vs Simucube), effect intensities are automatically scaled based on the user's Max Torque Ref. 

 Reference Torque : 20.0 Nm. (Updated from legacy 4000 unitless reference). Decoupling Scale : K_decouple = m_max_torque_ref / 20.0. Note: This ensures that 10% road texture feels the same physical intensity regardless of wheel strength. 

## 3. Component Breakdown 

**A. Load Factors (Safe Caps)** 

Texture and vibration effects are scaled by normalized tire load ( Load / 4000N ) to simulate connection with the road. 

1. **Texture Load Factor (Road/Slide)** : 

 Input : AvgLoad = (FL.Load + FR.Load) / 2.0. Robustness Check : Uses a hysteresis counter; if AvgLoad < 1.0 while |Velocity| > 1.0 m/s , it defaults to 4000N (1.0 Load Factor) to prevent signal loss during telemetry glitches. 

 Max Cap : 2.0. 

 ⚠ API Source of Truth All telemetry data units and field names are defined in src/lmu_sm_interface/InternalsPlugin.hpp. Critical: mSteeringShaftTorque is in Newton-meters (Nm). 

_F_ (^) final =Clamp Normalize( ( _F_ (^) total) × _K_ (^) gain , −1.0, 1.0) _F_ (^) total =( _F_ (^) base + _F_ (^) sop + _F_ (^) vib-lock + _F_ (^) vib-spin + _F_ (^) vib-slide + _F_ (^) vib-road + _F_ (^) vib-bottom + _F_ (^) gyro + _F_ (^) abs ) × _M_ spin-drop 

## M spin-drop 

_F_ (^) load-texture =Clamp(AvgLoad/4000.0, 0.0, _m_ (^) texture-load-cap) 


2. **Brake Load Factor (Lockup)** : 

 Max Cap : 3.0 (Allows stronger vibration under high-downforce braking). 

**B. Base Force Components** 

**1. Base Force Calculation ( )** Modulates the raw steering torque ( mSteeringShaftTorque ) based on front tire grip. 

 Operation Modes ( ): 

 Mode 0 (Native) :. (Default precision mode). Mode 1 (Synthetic) :. 

 Used for debugging direction only. Deadzone : Applied if to prevent center oscillation. 

 Mode 2 (Muted) :. 

 Steering Shaft Smoothing : Time-Corrected LPF ( ) applied to raw torque. 

**2. Grip Estimation & Fallbacks** If telemetry grip ( mGripFract ) is missing or invalid (< 0.0001), the engine approximates it: 

 Low Speed Trap : If CarSpeed < 5.0 m/s , Grip is forced to 1.0 (Prevents singularities at parking speeds). Combined Friction Circle : 

 Metric Formulation : 

 (Lateral Slip Angle). Default : 0.10 rad. (Longitudinal Slip Ratio). Default : 0.12 (12%). 

 Safety Clamp : Approx Grip is usually clamped to min 0.2 to prevent total loss of force. 

**3. Kinematic Load Reconstruction** If mSuspForce is missing (encrypted content), tire load is estimated from chassis physics: 

 Static : Mass (1100kg default) distributed by Weight Bias (55% Rear). Aero :. Transfer : 

 Longitudinal:. 

_F_ (^) load-brake =Clamp(AvgLoad/4000.0, 0.0, _m_ (^) brake-load-cap) 

## F base 

_F_ (^) base =BaseInput × _K_ (^) shaft-smooth × _K_ (^) shaft-gain ×(1.0 − (GripLoss × _K_ (^) understeer)) 

## m base-force-mode 

## BaseInput = T shaft 

BaseInput = Sign( _T_ (^) shaft ) × _m_ max-torque-ref ∣ _T_ (^) shaft ∣ <0.5Nm 

## BaseInput = 0.0 

## τ = m shaft-smooth 

Metric (^) lat =∣ _α_ ∣/OptAlpha Metric (^) long =∣ _κ_ ∣/OptRatio Combined = Metric (^) lat^2 + Metriclong^2 

## ApproxGrip = (1.0 if Combined < 1.0 else 1.0/(1.0 + (Combined − 1.0) × 2.0)) 

_F_ (^) _z_ = _F_ (^) static + _F_ (^) aero + _F_ (^) long-transfer + _F_ lat-transfer 

## 2.0 × Velocity^2 

(Accel (^) _Z_ /9.81) ×2000.0 


 Lateral: (Roll Stiffness). 

**C. Seat of Pants (SoP) & Oversteer** 

1. **Lateral G Force ( )** : 

 Input : mLocalAccel.x (Clamped to +/5.0 G ). Smoothing : Time-Corrected LPF ( mapped from scalar). Formula :. 

2. **Lateral G Boost ( )** : 

 Amplifies the SoP force when the car is oversteering (Front Grip > Rear Grip). Condition : if (FrontGrip > RearGrip) Formula : SoP_Total *= (1.0 + ((FrontGrip RearGrip) * K_oversteer_boost * 2.0)) 

3. **Yaw Acceleration ("The Kick")** : 

 Input : mLocalRotAccel.y (rad/s²). Note : Inverted (-1.0) to comply with SDK requirement to negate rotation data. Conditioning : 

 Low Speed Cutoff : 0.0 if Speed < 5.0 m/s. Noise Gate : 0.0 if rad/s². 

 Logic : Provides a "predictive kick" when car rotation starts, before lateral G builds up. Formula :. Note : Negative sign provides counter-steering torque. 

4. **Rear Aligning Torque ( )** : 

 Workaround : Uses RearSlipAngle * RearLoad * Stiffness(15.0) to estimate lateral force. Derivation : RearLoad = SuspForce + 300.0 (where 300N is estimated Unsprung Mass). Formula :. Clamp : Lateral Force clamped to +/6000N. 

**D. Braking & Lockup (Advanced)** 

**1. Progressive Lockup ( )** 

 Safety Trap : If CarSpeed < 2.0 m/s , Slip Ratio is forced to 0.0 to prevent false lockup detection at standstill. Predictive Logic (v0.6.0) : Triggers early if WheelDecel > CarDecel * 2.0 (Wheel stopping faster than car). Bump Rejection : Logic disabled if SuspVelocity > m_lockup_bump_reject (e.g. 1.0 m/s). Severity : (Quadratic). Logic : 

(Accel (^) _X_ /9.81) ×2000.0 × 0.6 

## F sop-base 

## τ ≈ 0.0225 − 0.1s 

_G_ (^) smooth × _K_ (^) sop × _K_ (^) sop-scale × _K_ decouple 

## F boost 

## ∣ Accel ∣ < 0.2 

−YawAccel (^) smooth × _K_ (^) yaw ×5.0Nm × _K_ decouple 

## T rear 

− _F_ (^) lat-rear ×0.001 × _K_ (^) rear × _K_ decouple 

## F vib-lock 

Severity = pow(NormSlip, _m_ (^) lockup-gamma) 


 Axle Diff : Rear lockups use 0.3x Frequency and 1.5x Amplitude. Pressure Scaling : Scales with Brake Pressure (Bar). Fallback to 0.5 if engine braking (Pressure < 0.1 bar). Oscillator : sin(Phase) (Wrapped via fmod to prevent phase explosion on stutter). 

**2. ABS Pulse ( )** 

 Trigger : Brake > 50% AND Pressure Modulation Rate > 2.0 bar/s. Formula : sin(20Hz) * K_abs * 2.0Nm. 

**E. Dynamic Textures & Vibrations** 

**1. Slide Texture (Scrubbing)** 

 Scope : Max(FrontSlipVel, RearSlipVel) (Worst axle dominates). Frequency :. Cap 250Hz. (Updated from old 40Hz base). Amplitude :. Note : Work-based scaling (1.0 Grip) ensures vibration only occurs during actual scrubbing. 

**2. Road Texture (Bumps)** 

 Main Input : Delta of mVerticalTireDeflection (effectively a High-Pass Filter on suspension movement). Safety Clamp : Delta is clamped to +/0.01m per frame to prevent physics explosions on teleport or restart. Formula : (DeltaL + DeltaR) * 50.0 * K_road * F_load_texture * Scale. Scrub Drag (Fade-In) : 

 Adds constant resistance when sliding laterally. Coordinate Note : LMU uses +X = Left. Drag must oppose velocity, so we invert direction. Fade-In : Linear scale 0% to 100% between 0.0 m/s and 0.5 m/s lateral velocity. Formula : (SideVel > 0? -1 : 1) * K_drag * 5.0Nm * Fade * Scale. 

**3. Traction Loss (Wheel Spin)** 

 Trigger : Throttle > 5% and SlipRatio > 0.2 (20%). Torque Drop ( ) : The Total Output Force is reduced to simulate "floating" front tires. 

 M_spin-drop = (1.0 (Severity * K_spin * 0.6)) 

 Vibration : 

 Frequency :. Cap 80Hz. Formula :. 

**4. Suspension Bottoming** 

 Triggers : 

 Method A: RideHeight < 2mm. 

## F abs 

## 10Hz + (SlipVel × 5.0) 

Sawtooth( _ϕ_ ) × _K_ (^) slide ×1.5Nm × _F_ (^) load-texture ×(1.0 − Grip) × _K_ decouple 

## M spin-drop 

## 10Hz + (SlipSpeed × 2.5) 

sin( _ϕ_ ) × Severity × _K_ (^) spin ×2.5Nm × _K_ decouple 


 Method B: SuspForceRate > 100,000 N/s. Legacy: TireLoad > 8000.0 N. Formula : sin(50Hz) * K_bottom * 1.0Nm. (Fixed sinusoidal crunch). Legacy Intensity :. (Scalar restored to legacy value for accuracy). 

**F. Post-Processing & Filters** 

**1. Signal Filtering** 

 Notch Filters : 

 Dynamic :. Uses Biquad. Static : Fixed frequency (e.g. 50Hz) Biquad. 

 Frequency Estimator : Tracks zero-crossings of mSteeringShaftTorque (AC coupled). 

**2. Gyroscopic Damping ( )** 

 Input Derivation : 

 Formula :. Smoothing : Time-Corrected LPF. 

**3. Time-Corrected LPF (Algorithm)** Standard exponential smoothing filter used for Slip Angle, Gyro, SoP, and Shaft Torque. 

 Formula : Alpha Calculation : 

 : Delta Time (e.g., 0.0025s) (Tau): Time Constant (User Configurable, or derived from smoothness). Target : ~0.0225s (from 400Hz). 

**4. Min Force (Friction Cancellation)** Applied at the very end of the pipeline to F_norm (before clipping). 

 Logic : If AND : 

 . 

 Purpose : Ensures small forces are always strong enough to overcome the physical friction/deadzone of gear/belt wheels. 

## 7. Telemetry Variable Mapping 

## ExcessLoad ×0.0025 

## F req = Speed/Circumference 

## F gyro 

## SteerAngle = UnfilteredInput × (RangeInRadians/2.0) 

SteerVel = (Angle (^) current −Angle (^) prev)/ _dt_ −SteerVel (^) smooth × _K_ (^) gyro ×(Speed/10.0) × 1.0Nm × _K_ decouple 

## State + = α × ( Input − State ) 

## α = dt /( τ + dt ) 

## dt 

## τ 

## ∣ F ∣ > 0.0001 ∣ F ∣ < K min-force 

_F_ (^) final =Sign( _F_ ) × _K_ min-force 


 Math Symbol API Variable Description 

mSteeringShaftTorque (^) Raw steering torque (Nm) mTireLoad (^) Vertical load on tire (N) mGripFract Tire grip scaler (0.0-1.0) mLocalAccel.x (^) Lateral acceleration (m/s²) mLocalAccel.z (^) Longitudinal acceleration (m/s²) mLocalRotAccel.y Rotational acceleration (rad/s²) mLocalVel.z (^) Car speed (m/s) mLateralPatchVel (^) Scrubbing velocity (m/s) mSuspForce (^) Suspension force (N) mUnfilteredBrake Raw brake input (0.0-1.0) 

## 8. Legend: Physics Constants (Implementation Detail) 

 Constant Name Value Description BASE_NM_LOCKUP 4.0 Nm Reference intensity for lockup vibration 

BASE_NM_SPIN (^) 2.5 Nm Reference intensity for wheel spin BASE_NM_ROAD 2.5 Nm Reference intensity for road bumps REAR_STIFFNESS 15.0 N/(rad·N) Estimated rear tire cornering stiffness WEIGHT_TRANSFER_SCALE (^) 2000.0 N/G Kinematic load transfer scaler UNSPRUNG_MASS 300.0 N Per-corner static unsprung weight estimate BOTTOMING_LOAD 8000.0 N Load required to trigger legacy bottoming BOTTOMING_RATE (^) 100kN/s Suspension force rate for impact bottoming MIN_SLIP_VEL (^) 0.5 m/s Low speed threshold for slip angle calculation _T_ shaft Load GripFract Accel _X_ Accel _Z_ YawAccel Vel _Z_ SlipVellat SuspForce Pedalbrake 


