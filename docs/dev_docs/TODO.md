# LMUFFB To-Do List


## Troubleshooting 24

DONE: delay from the app due to SoP and slip angle smoothing
DONE: mask the flatspot vibration
DONE:yaw kick cutoffs by speed and force.

User report
"potentially leave the ingame FFB as is and add the SoP effects ontop? (...) for DD users this could be an amazing feature. I tested it with VRS DFP Pro"
if it is not possible to have both FFB coming from game and lmuFFB app, implement it by using the steering shaft torque as a surrogate of the game FFB (which is probably what the game FFB is based on). Try if we can remove the annoying "vibration" from this signal with an added smoothing filter, or other type of filter. This might reduce detail and get rid of the vibration. In fact, the game FFB seems less detailed than the raw steering shaft torque , and the reason might be an added smoothing filter.


loses wheel or connection to game when app not in focus
(make the app always on top? auto reconnect game / rebind wheel when disconnected? other mechanisms to avoid these disconnections?)



improvements to the formulas

---

## Troubleshooting 25

DONE: remove warnings about missing telemetry data

DONE: add slider for optimal slip angle (and slip ratio)
DONE: list effects affected by grip and load approximation, and list those that are not affected

DONE: add smoothing (and slider) for steering shaft torque
DONE: expose sliders for additional smoothing: yaw kick, gyroscopic damping, Chassis Inertia (Load)

Yaw Kick Smoothing, Gyroscopic Damping Smoothing, Chassis Inertia (Load) Smoothing

update tooltips

test default values after 0-100% normalization of sliders

test if some vibration effects are muted
check lockup vibration effect, feel it before bracking up, enough to prevent it

yaw kick further fixes? smoothing? higher thresholds? non linear transformation? 
experiment with gyro damping to compensate yaw kick

console prints, add timestamp

the game exited from the session, and there were still forces
in particular self align torque and slide vibratio
improve logic of detecting when not driving / not live, and stop ffb

lockup vibration is not working
help me troubleshoot
error in formula?
raw telemetry values missing?

check other 2 telemetry raw values that might be missing

spin vibration might also not be working

check screenshot

we need a button for ..disconnect from game? reset data from dame? signal session finished?
the telemetry persist even after quitting the game (slide texture and rear align torque)
