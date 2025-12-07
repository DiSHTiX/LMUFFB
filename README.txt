LMUFFB - Le Mans Ultimate Force Feedback
========================================

See README.md for full documentation and images.

Installation
------------
1. Install vJoy Driver (2.1.9.1). Disable all FFB effects in vJoy Config except "Constant Force".
2. Install rFactor 2 Shared Memory Plugin (rFactor2SharedMemoryMapPlugin64.dll) into "Le Mans Ultimate/Plugins/".
3. Run LMUFFB.exe. Ensure vJoyInterface.dll is in the same folder.

Configuration
-------------
1. Game (Le Mans Ultimate):
   - Set Display Mode to Borderless.
   - Bind Steering Axis to vJoy Device (Requires external Feeder) OR Physical Wheel (Experimental).
   - Set FFB Type to None/Weak.
   - Set FFB Smoothing to 0.

2. App (LMUFFB):
   - Select your Physical Wheel in "FFB Device".
   - Adjust Gain (Start at 0.5) and Effects.

Troubleshooting
---------------
- No Steering: Use "vJoy Feeder" to map your wheel to vJoy if using vJoy binding.
- Double FFB / Fighting: Game is sending FFB to real wheel. Ensure Game FFB is bound to vJoy or disabled.
- Missing DLL: Copy vJoyInterface.dll from vJoy SDK to app folder.
