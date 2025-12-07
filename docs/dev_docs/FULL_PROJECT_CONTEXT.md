# LMUFFB Project Context

This file contains the full source code and documentation of the project.


# File: build_commands.txt
```
Write build files
& 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1' -Arch amd64 -SkipAutomaticLocation; cmake -B build


Do a clean build
& 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1' -Arch amd64 -SkipAutomaticLocation; cmake --build build --config Release


To run the tests:

First compile the test directly:

& 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1' -Arch amd64 -SkipAutomaticLocation; cl /EHsc /std:c++17 /I.. tests\test_ffb_engine.cpp /Fe:test_ffb_engine.exe

The run it:
tests\test_ffb_engine.exe

```

# File: CHANGELOG.md
```markdown
# Changelog

All notable changes to this project will be documented in this file.

## [0.3.16] - 2025-05-23
### Fixed
- **vJoy Startup Check**: Fixed a logic bug where the vJoy DLL was used before verifying if the driver was enabled, potentially causing instability. Added explicit `DynamicVJoy::Get().Enabled()` check in the FFB loop.

## [0.3.13] - 2025-05-23
### Added
- **Complete FFB Visualizations**: Expanded the Troubleshooting Graphs to include individual plots for *all* FFB components (Understeer, Oversteer, Road, Slide, Lockup, Spin, Bottoming, Clipping) and 8 critical Telemetry channels.
- **Refactoring**: Split `SoP Force` (Lateral G) from `Oversteer Boost` (Rear Aligning Torque) in the internal engine debug state for clearer analysis.

## [0.3.12] - 2025-05-23
### Added
- **Visual Troubleshooting Tools**: Added real-time **Rolling Trace Plots** (Oscilloscope style) for FFB components (Base, SoP, Textures) and Telemetry inputs. Accessible via "Show Troubleshooting Graphs" in the main GUI.
- **Internal**: Refactored `FFBEngine` to expose internal calculation states for visualization.

## [0.3.11] - 2025-05-23
### Documentation
- **Direct Mode Priority**: Updated `README` to recommend "Direct Method" (binding physical wheel with 0% FFB) as the primary configuration, demoting "vJoy Bridge" to compatibility mode.
- **Feeder Clarification**: Explicitly documented that "vJoy Demo Feeder" is insufficient for driving; **Joystick Gremlin** is required if using the vJoy bridge method.

## [0.3.10] - 2025-05-23
### Fixed
- **Wheel Spinning Loop**: Implemented a safety switch (`Monitor FFB on vJoy`) which is **Disabled by default**. This prevents the app from writing FFB to vJoy Axis X, which caused a feedback loop if users bound Game Steering to that axis.
- **Steering Input Confusion**: Updated documentation to explicitly state LMUFFB does not bridge steering input; users must use external Feeders or alternative bindings.

## [0.3.9] - 2025-05-23
### Added
- **Smoothing & Caps**: Added configuration sliders for `SoP Smoothing` (Low Pass Filter) and `Load Cap` (Max Tire Load scale) in the GUI ("Advanced Tuning" section). This allows users to fine-tune signal noise vs. responsiveness.
- **Documentation**: 
    - Updated `README` files with precise Le Mans Ultimate in-game settings (Force Feedback Strength 0%, Effects Off, Smoothing 0, Borderless Mode).
    - Clarified vJoy links and troubleshooting steps.

## [0.3.8] - 2025-05-23
### Added
- **vJoy Version Check**: Startup check ensures vJoy driver version is compatible (>= 2.1.8). Warnings can be suppressed via checkbox logic (persisted in config).
- **Licensing**: Added `licenses/vJoy_LICENSE.txt` to comply with MIT attribution.
- **Documentation**: Added investigation regarding bundling vJoy DLLs.

## [0.3.7] - 2025-05-23
### Added
- **Priority Check**: Implemented logic to detect if Le Mans Ultimate (LMU) has locked the FFB device in Exclusive Mode ("Double FFB"). If detected, a warning is logged to the console.
- **Documentation Updates**:
    - Updated `README.md` and `README.txt` to be LMU-specific (replaced "Game" references).
    - Clarified that LMU lacks a "None" FFB option; advised setting Strength to 0% as a workaround.
    - Updated `investigation_vjoyless_implementation.md` with LMU-specific experiments.
    - Updated `plan_troubleshooting_FFB_visualizations.md` to specify "Rolling Trace Plots" for all telemetry/physics values.

## [0.3.6] - 2025-05-23
### Documentation
- **Troubleshooting**: Added comprehensive plans for "FFB Visualizations" (`docs/plan_troubleshooting_FFB_visualizations.md`) and "Guided Configurator" (`docs/plan_guided_configurator.md`).
- **Clarification**: Updated README.md and README.txt to clarify that LMUFFB does not bridge steering input, requiring external "Feeder" tools if vJoy is used for input binding.

## [0.3.5] - 2025-05-23
### Added
- **Safety Defaults**: Changed default settings to Gain 0.5 and SOP 0.0 to prevent violent wheel jerking on first run (especially for Direct Drive wheels).
- **SoP Smoothing**: Implemented a Low Pass Filter (exponential moving average) for lateral G-force data to reduce signal noise and vibration on straights.
- **Improved Error Handling**: Added a clear popup message when `vJoyInterface.dll` is missing.
- **Documentation**: 
    - Added "Guided Install Plan" and "vJoy-less Investigation" documents.
    - Updated README with critical "Double FFB" troubleshooting tips and Borderless Window warnings.

## [0.3.4] - 2025-05-23
### Added
- **Test Suite**: Significantly expanded test coverage (approx 85%) covering Oversteer Boost, Phase Wraparound, Multi-effect interactions, and Safety Clamps.

## [0.3.3] - 2025-05-23
### Fixed
- **Suspension Bottoming**: Fixed a logic bug where the bottoming effect force direction depended on current steering torque, causing unexpected pulls on straights. Now uses a 50Hz vibration pulse (crunch).

## [0.3.2] - 2025-05-23
### Added
- **Suspension Bottoming**: Added a new haptic effect that triggers when tire load exceeds thresholds (simulating bump stops/heavy compression).
- **Physics Refinement**: Updated Slide Texture to use `mLateralPatchVel` for more accurate scrubbing sensation.
- **Documentation**: Added `docs/telemetry_logging_investigation.md` for future CSV logging features.

### Optimized
- **DirectInput**: Removed redundant parameter updates and `DIEP_START` calls in the high-frequency loop to reduce driver overhead.
- **Thread Safety**: Added mutex locking to prevent race conditions when GUI modifies physics engine parameters.

## [0.3.1] - 2025-05-23
### Fixed
- **vJoy Build Issue**: Fixed an import error in `src/DynamicVJoy.h` or `main.cpp` that was causing build failures on some systems (user contribution).

## [0.3.0] - 2025-05-23
### Added
- **Dynamic Physics Engine**: Major overhaul of FFB synthesis.
    - **Phase Integration**: Solved audio-like clicking/popping in vibration effects by using phase accumulators.
    - **Advanced Telemetry**: Integrated `mLateralPatchVel` (Slide Speed) and `mTireLoad` (Vertical Load) into calculations.
    - **Dynamic Frequencies**: 
        - Lockup frequency now scales with Car Speed.
        - Spin/Traction Loss frequency now scales with Slip Speed.
        - Slide Texture frequency now scales with Lateral Slide Speed.
- **Documentation**: Added `docs/implementation_report_v0.3.md`.

## [0.2.2] - 2025-05-23
### Added
- **Dynamic Effects (Initial)**:
    - **Oversteer**: Added Rear Aligning Torque integration (Counter-steer cue).
    - **Progressive Lockup**: Replaced binary on/off rumble with scaled amplitude based on slip severity.
    - **Torque Drop**: Added "Floating" sensation when traction is lost.
- **GUI**: Added sliders for Oversteer Boost, Lockup Gain, and Spin Gain.

## [0.2.0] - 2025-05-22
### Added
- **DirectInput Support**: Implemented native FFB output to physical wheels (bypassing vJoy for forces).
- **Device Selection**: Added GUI dropdown to select specific FFB devices.
- **vJoy Optionality**: Made vJoy a soft dependency via dynamic loading (`LoadLibrary`). App runs even if vJoy is missing.
- **Installer**: Added Inno Setup script (`installer/lmuffb.iss`).
- **Configuration**: Added `config.ini` persistence (Save/Load buttons).
- **Error Handling**: Added GUI Popups for missing Shared Memory.

## [0.1.0] - 2025-05-21
### Added
- **C++ Port**: Initial release of the native C++ application (replacing Python prototype).
- **Core FFB**: Basic Grip Modulation (Understeer) and Seat of Pants (SoP) effects.
- **GUI**: Initial implementation using Dear ImGui.
- **Architecture**: Multi-threaded design (400Hz FFB loop / 60Hz GUI loop).

```

# File: CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.10)
project(LMUFFB_CPP)

set(CMAKE_CXX_STANDARD 17)

# vJoy SDK Location - Users should set this
# We link against the installed vJoy SDK (headers/libs), we do NOT compile vJoy itself.
# ImGui Detection
set(IMGUI_DIR "vendor/imgui")
option(BUILD_HEADLESS "Build without GUI support" OFF)

if(NOT BUILD_HEADLESS)
    if(EXISTS "${CMAKE_SOURCE_DIR}/${IMGUI_DIR}/imgui.cpp")
        message(STATUS "ImGui found. Enabling GUI support.")
        add_definitions(-DENABLE_IMGUI)
        set(IMGUI_SOURCES
            ${IMGUI_DIR}/imgui.cpp
            ${IMGUI_DIR}/imgui_demo.cpp
            ${IMGUI_DIR}/imgui_draw.cpp
            ${IMGUI_DIR}/imgui_tables.cpp
            ${IMGUI_DIR}/imgui_widgets.cpp
            ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
            ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
        )
        include_directories(${IMGUI_DIR})
        include_directories(${IMGUI_DIR}/backends)
        # Link DirectX libraries
        link_libraries(d3d11 d3dcompiler dxguid)
        add_definitions(-DUNICODE -D_UNICODE -DNOMINMAX)
    else()
        message(FATAL_ERROR "ImGui not found in vendor/imgui. To build headless, use -DBUILD_HEADLESS=ON.")
    endif()
else()
    message(STATUS "Building in Headless mode (User Request).")
    set(IMGUI_SOURCES "")
endif()

# include_directories(${VJOY_SDK_DIR}/inc)

# Detect platform and use appropriate vJoy library
# if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64-bit
#    link_directories(${VJOY_SDK_DIR}/lib/amd64)
# else()
    # 32-bit
#    link_directories(${VJOY_SDK_DIR}/lib/i386)
# endif()

add_executable(LMUFFB main.cpp rF2Data.h src/GuiLayer.cpp src/GuiLayer.h src/Config.cpp src/Config.h src/DirectInputFFB.cpp src/DirectInputFFB.h src/DynamicVJoy.h ${IMGUI_SOURCES})

target_link_libraries(LMUFFB dinput8.lib dxguid.lib)

# Read Version
file(STRINGS "VERSION" LMUFFB_VERSION)
add_definitions(-DLMUFFB_VERSION="${LMUFFB_VERSION}")

# Copy Distribution Files
add_custom_command(TARGET LMUFFB POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/README.txt"
            "$<TARGET_FILE_DIR:LMUFFB>/README.txt")

# Copy vJoy DLL to output directory
# if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64-bit
#    add_custom_command(TARGET LMUFFB POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy
#                "${VJOY_SDK_DIR}/lib/amd64/vJoyInterface.dll"
#                "$<TARGET_FILE_DIR:LMUFFB>/vJoyInterface.dll")
# else()
    # 32-bit
#    add_custom_command(TARGET LMUFFB POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy
#                "${VJOY_SDK_DIR}/lib/vJoyInterface.dll"
#                "$<TARGET_FILE_DIR:LMUFFB>/vJoyInterface.dll")
# endif()

```

# File: FFBEngine.h
```cpp
#ifndef FFBENGINE_H
#define FFBENGINE_H

#include <cmath>
#include <algorithm>
#include "rF2Data.h"

// Struct to hold debug values for visualization
struct FFBOutputDebug {
    float base_force = 0.0f;
    float sop_force = 0.0f;
    float understeer_drop = 0.0f; // Reduction amount
    float oversteer_boost = 0.0f; // Boost + Rear Torque
    float texture_road = 0.0f;
    float texture_slide = 0.0f;
    float texture_lockup = 0.0f;
    float texture_spin = 0.0f;
    float texture_bottoming = 0.0f;
    float total_output = 0.0f;
    float clipping = 0.0f; // 1.0 if clipping, 0.0 otherwise
};

// FFB Engine Class
class FFBEngine {
public:
    // Settings (GUI Sliders)
    float m_gain = 0.5f;          // Master Gain (Default 0.5 for safety)
    float m_understeer_effect = 1.0f; // 0.0 - 1.0 (How much grip loss affects force)
    float m_sop_effect = 0.15f;    // 0.0 - 1.0 (Lateral G injection strength - Default 0.15 for balanced feel) (0 to prevent jerking)
    float m_min_force = 0.0f;     // 0.0 - 0.20 (Deadzone removal)
    
    // Configurable Smoothing & Caps (v0.3.9)
    float m_sop_smoothing_factor = 0.05f; // 0.0 (Max Smoothing) - 1.0 (Raw). Default Default 0.05 for responsive feel. (0.1 ~5Hz.)
    float m_max_load_factor = 1.5f;      // Cap for load scaling (Default 1.5x)
    float m_sop_scale = 1000.0f;         // SoP base scaling factor (Default 1000.0)

    // New Effects (v0.2)
    float m_oversteer_boost = 0.0f; // 0.0 - 1.0 (Rear grip loss boost)
    
    bool m_lockup_enabled = false;
    float m_lockup_gain = 0.5f;
    
    bool m_spin_enabled = false;
    float m_spin_gain = 0.5f;

    // Texture toggles
    bool m_slide_texture_enabled = true;
    float m_slide_texture_gain = 0.5f; // 0.0 - 1.0
    
    bool m_road_texture_enabled = false;
    float m_road_texture_gain = 0.5f; // 0.0 - 1.0
    
    // Bottoming Effect (v0.3.2)
    bool m_bottoming_enabled = true;
    float m_bottoming_gain = 1.0f;

    // Internal state
    double m_prev_vert_deflection[2] = {0.0, 0.0}; // FL, FR
    
    // Phase Accumulators for Dynamic Oscillators
    double m_lockup_phase = 0.0;
    double m_spin_phase = 0.0;
    double m_slide_phase = 0.0;
    double m_bottoming_phase = 0.0;
    
    // Smoothing State
    double m_sop_lat_g_smoothed = 0.0;
    
    // Debug State
    FFBOutputDebug m_last_debug;
    rF2Telemetry m_last_telemetry; // Copy of latest telemetry for GUI

    double calculate_force(const rF2Telemetry* data) {
        if (!data) return 0.0;
        m_last_telemetry = *data; // Store for debug
        
        // Reset debug struct
        m_last_debug = FFBOutputDebug();
        
        double dt = data->mDeltaTime;
        const double TWO_PI = 6.28318530718;

        // Front Left and Front Right
        const rF2Wheel& fl = data->mWheels[0];
        const rF2Wheel& fr = data->mWheels[1];

        double game_force = data->mSteeringArmForce;

        // --- PRE-CALCULATION: TIRE LOAD FACTOR ---
        // Calculate this once to use across multiple effects.
        // Heavier load = stronger vibration transfer.
        double avg_load = (fl.mTireLoad + fr.mTireLoad) / 2.0;
        
        // Normalize: 4000N is a reference "loaded" GT tire.
        double load_factor = avg_load / 4000.0;
        
        // SAFETY CLAMP: Cap at 1.5x (or configured max) to prevent violent jolts.
        load_factor = (std::min)((double)m_max_load_factor, (std::max)(0.0, load_factor));

        // --- 1. Understeer Effect (Grip Modulation) ---
        // Grip Fraction (Average of front tires)
        double grip_l = fl.mGripFract;
        double grip_r = fr.mGripFract;
        double avg_grip = (grip_l + grip_r) / 2.0;
        
        // Clamp grip 0-1 for safety
        avg_grip = (std::max)(0.0, (std::min)(1.0, avg_grip));
        
        double grip_factor = 1.0 - ((1.0 - avg_grip) * m_understeer_effect);
        double output_force = game_force * grip_factor;
        
        m_last_debug.base_force = (float)game_force;
        m_last_debug.understeer_drop = (float)(game_force * (1.0 - grip_factor));

        // --- 2. Seat of Pants (SoP) / Oversteer ---
        // Lateral G-force
        double lat_g = data->mLocalAccel.x / 9.81;
        
        // SoP Smoothing (Simple Low Pass Filter)
        // Alpha determines the "weight" of the new value.
        // If m_sop_smoothing_factor is 1.0, alpha is 1.0 (No Smoothing).
        // If m_sop_smoothing_factor is 0.1, alpha is 0.1 (Heavy Smoothing).
        double alpha = (double)m_sop_smoothing_factor; 
        
        // Safety clamp alpha
        alpha = (std::max)(0.001, (std::min)(1.0, alpha));

        m_sop_lat_g_smoothed = m_sop_lat_g_smoothed + alpha * (lat_g - m_sop_lat_g_smoothed);
        
        double sop_base_force = m_sop_lat_g_smoothed * m_sop_effect * (double)m_sop_scale;
        double sop_total = sop_base_force;
        
        // Oversteer Boost: If Rear Grip < Front Grip (car is rotating), boost SoP
        double grip_rl = data->mWheels[2].mGripFract;
        double grip_rr = data->mWheels[3].mGripFract;
        double avg_rear_grip = (grip_rl + grip_rr) / 2.0;
        
        // Delta between front and rear grip
        // If front has 1.0 grip and rear has 0.5, delta is 0.5. Boost SoP.
        double grip_delta = avg_grip - avg_rear_grip;
        if (grip_delta > 0.0) {
            sop_total *= (1.0 + (grip_delta * m_oversteer_boost * 2.0));
        }
        
        // --- 2a. Rear Aligning Torque Integration (Experimental) ---
        // Add a "Counter-Steer" suggestion based on rear axle force.
        // Approx: RearLateralForce * Trail. 
        // We modulate this by oversteer_boost as well.
        double rear_lat_force = (data->mWheels[2].mLateralForce + data->mWheels[3].mLateralForce) / 2.0;
        // Trail approx: 0.03m (3cm). Scaling factor required.
        // Sign correction: If rear force pushes left, wheel should turn right (align).
        // rF2 signs: LatForce +Y is Left? We need to tune this.
        // For now, simpler approximation: if LocalRotAccel.y (Yaw Accel) is high, add counter torque.
        // Let's stick to the grip delta for now but add a yaw-rate damper if needed.
        // Actually, let's inject a fraction of rear lateral force directly.
        double rear_torque = rear_lat_force * 0.05 * m_oversteer_boost; // 0.05 is arb scale
        sop_total += rear_torque;
        
        // Log split components
        m_last_debug.sop_force = (float)sop_base_force; // Pure Lat G
        m_last_debug.oversteer_boost = (float)(sop_total - sop_base_force); // The extra boost + rear torque

        double total_force = output_force + sop_total;
        
        // --- 2b. Progressive Lockup (Dynamic) ---
        // Ensure phase updates even if force is small, but gated by enabled
        if (m_lockup_enabled && data->mUnfilteredBrake > 0.05) {
            double slip_fl = data->mWheels[0].mSlipRatio;
            double slip_fr = data->mWheels[1].mSlipRatio;
            // Use worst slip
            double max_slip = (std::min)(slip_fl, slip_fr); // Slip is negative for braking
            
            // Thresholds: -0.1 (Peak Grip), -1.0 (Locked)
            // Range of interest: -0.1 to -0.5
            if (max_slip < -0.1) {
                double severity = (std::abs(max_slip) - 0.1) / 0.4; // 0.0 to 1.0 scale
                severity = (std::min)(1.0, severity);
                
                // DYNAMIC FREQUENCY: Linked to Car Speed (Slower car = Lower pitch grinding)
                // As the car slows down, the "scrubbing" pitch drops.
                // Speed is in m/s. 
                // Example: 300kmh (83m/s) -> ~80Hz. 50kmh (13m/s) -> ~20Hz.
                double car_speed_ms = std::abs(data->mLocalVel.z); 
                double freq = 10.0 + (car_speed_ms * 1.5); 

                // PHASE ACCUMULATION
                m_lockup_phase += freq * dt * TWO_PI;
                if (m_lockup_phase > TWO_PI) m_lockup_phase -= TWO_PI;

                double amp = severity * m_lockup_gain * 800.0;
                
                // Use the integrated phase
                double rumble = std::sin(m_lockup_phase) * amp;
                m_last_debug.texture_lockup = (float)rumble;
                total_force += rumble;
            }
        }

        // --- 2c. Wheel Spin (Tire Physics Based) ---
        if (m_spin_enabled && data->mUnfilteredThrottle > 0.05) {
            double slip_rl = data->mWheels[2].mSlipRatio;
            double slip_rr = data->mWheels[3].mSlipRatio;
            double max_slip = (std::max)(slip_rl, slip_rr);
            
            if (max_slip > 0.2) {
                double severity = (max_slip - 0.2) / 0.5;
                severity = (std::min)(1.0, severity);
                
                // 1. Torque Drop (Floating feel)
                total_force *= (1.0 - (severity * m_spin_gain * 0.6)); 

                // 2. Vibration Frequency: Based on SLIP SPEED (Not RPM)
                // Calculate how fast the tire surface is moving relative to the road.
                // Slip Speed (m/s) approx = Car Speed (m/s) * Slip Ratio
                double car_speed_ms = std::abs(data->mLocalVel.z);
                double slip_speed_ms = car_speed_ms * max_slip;

                // Mapping:
                // 2 m/s (~7kph) slip -> 15Hz (Judder/Grip fighting)
                // 20 m/s (~72kph) slip -> 60Hz (Smooth spin)
                double freq = 10.0 + (slip_speed_ms * 2.5);
                
                // Cap frequency to prevent ultrasonic feeling on high speed burnouts
                if (freq > 80.0) freq = 80.0;

                // PHASE ACCUMULATION
                m_spin_phase += freq * dt * TWO_PI;
                if (m_spin_phase > TWO_PI) m_spin_phase -= TWO_PI;

                // Amplitude
                double amp = severity * m_spin_gain * 500.0;
                double rumble = std::sin(m_spin_phase) * amp;
                
                m_last_debug.texture_spin = (float)rumble;
                total_force += rumble;
            }
        }

        // --- 3. Slide Texture (Dynamic Sawtooth) ---
        if (m_slide_texture_enabled) {
            double avg_slip = (std::abs(fl.mSlipAngle) + std::abs(fr.mSlipAngle)) / 2.0;
            
            if (avg_slip > 0.15) { // ~8 degrees
                // Frequency: Scrubbing speed
                // Using mLateralPatchVel is more accurate than GroundVel for rubber sliding
                double lat_vel = (std::abs(fl.mLateralPatchVel) + std::abs(fr.mLateralPatchVel)) / 2.0;
                
                // Map 1 m/s -> 40Hz, 10 m/s -> 200Hz
                double freq = 40.0 + (lat_vel * 17.0);
                if (freq > 250.0) freq = 250.0;

                m_slide_phase += freq * dt * TWO_PI;
                if (m_slide_phase > TWO_PI) m_slide_phase -= TWO_PI;

                // Sawtooth wave
                double sawtooth = (m_slide_phase / TWO_PI) * 2.0 - 1.0;

                // Amplitude: Scaled by PRE-CALCULATED global load_factor
                double noise = sawtooth * m_slide_texture_gain * 300.0 * load_factor;
                m_last_debug.texture_slide = (float)noise;
                total_force += noise;
            }
        }
        
        // --- 4. Road Texture (High Pass Filter) ---
        if (m_road_texture_enabled) {
            // Use change in suspension deflection
            double vert_l = fl.mVerticalTireDeflection;
            double vert_r = fr.mVerticalTireDeflection;
            
            // Delta from previous frame
            double delta_l = vert_l - m_prev_vert_deflection[0];
            double delta_r = vert_r - m_prev_vert_deflection[1];
            
            // Store for next frame
            m_prev_vert_deflection[0] = vert_l;
            m_prev_vert_deflection[1] = vert_r;
            
            // Amplify sudden changes
            double road_noise = (delta_l + delta_r) * 5000.0 * m_road_texture_gain; 
            
            // Apply LOAD FACTOR: Bumps feel harder under compression
            road_noise *= load_factor;
            
            m_last_debug.texture_road = (float)road_noise;

            total_force += road_noise;
        }

        // --- 5. Suspension Bottoming (High Load Impulse) ---
        if (m_bottoming_enabled) {
            // Detect sudden high load spikes which indicate bottoming out
            // Using Tire Load as proxy for suspension travel limit (bump stop)
            double max_load = (std::max)(fl.mTireLoad, fr.mTireLoad);
            
            // Threshold: 8000N is a heavy hit for a race car corner/bump
            const double BOTTOM_THRESHOLD = 8000.0;
            
            if (max_load > BOTTOM_THRESHOLD) {
                double excess = max_load - BOTTOM_THRESHOLD;
                
                // Non-linear response (Square root softens the initial onset)
                double bump_magnitude = std::sqrt(excess) * m_bottoming_gain * 0.5;
                
                // FIX: Use a 50Hz "Crunch" oscillation instead of directional DC offset
                double freq = 50.0; 
                
                // Phase Integration
                m_bottoming_phase += freq * dt * TWO_PI;
                if (m_bottoming_phase > TWO_PI) m_bottoming_phase -= TWO_PI;

                // Generate vibration (Sine wave)
                // This creates a heavy shudder regardless of steering direction
                double crunch = std::sin(m_bottoming_phase) * bump_magnitude;
                
                m_last_debug.texture_bottoming = (float)crunch;
                total_force += crunch;
            }
        }

        // --- 6. Min Force (Deadzone Removal) ---
        // Boost small forces to overcome wheel friction
        double max_force_ref = 4000.0; 
        double norm_force = total_force / max_force_ref;
        
        // Apply Master Gain
        norm_force *= m_gain;
        
        // Apply Min Force
        // If force is non-zero but smaller than min_force, boost it.
        // Also handle the zero case if necessary, but typically we want a minimal signal if *any* force exists.
        if (std::abs(norm_force) > 0.0001 && std::abs(norm_force) < m_min_force) {
            // Sign check
            double sign = (norm_force > 0.0) ? 1.0 : -1.0;
            norm_force = sign * m_min_force;
        }
        
        m_last_debug.total_output = (float)norm_force;
        m_last_debug.clipping = (std::abs(norm_force) > 1.0) ? 1.0f : 0.0f;

        // Clip
        return (std::max)(-1.0, (std::min)(1.0, norm_force));
    }
};

#endif // FFBENGINE_H

```

# File: main.cpp
```cpp
#include <windows.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <thread>
#include <chrono>

#include "rF2Data.h"
#include "FFBEngine.h"
#include "src/GuiLayer.h"
#include "src/Config.h"
#include "src/DirectInputFFB.h"
#include "src/DynamicVJoy.h"

// Constants
const char* SHARED_MEMORY_NAME = "$rFactor2SMMP_Telemetry$";
const int VJOY_DEVICE_ID = 1;

#include <atomic>
#include <mutex>

// Threading Globals
std::atomic<bool> g_running(true);
std::atomic<bool> g_ffb_active(true);
rF2Telemetry* g_pTelemetry = nullptr;
FFBEngine g_engine;
std::mutex g_engine_mutex; // Protects settings access if GUI changes them

// --- FFB Loop (High Priority 400Hz) ---
void FFBThread() {
    long axis_min = 1;
    long axis_max = 32768;
    
    // Attempt to load vJoy (but don't acquire yet)
    bool vJoyDllLoaded = false;
    if (DynamicVJoy::Get().Load()) {
        vJoyDllLoaded = true;
        // Version Check
        SHORT ver = DynamicVJoy::Get().GetVersion();
        std::cout << "[vJoy] DLL Version: " << std::hex << ver << std::dec << std::endl;
        // Expected 2.1.9 (0x219)
        if (ver < 0x218 && !Config::m_ignore_vjoy_version_warning) {
             std::string msg = "vJoy Driver Version Mismatch.\n\nDetected: " + std::to_string(ver) + "\nExpected: 2.1.8 or higher.\n\n"
                               "Some features may not work. Please update vJoy.";
             int result = MessageBoxA(NULL, msg.c_str(), "LMUFFB Warning", MB_ICONWARNING | MB_OKCANCEL);
             if (result == IDCANCEL) {
                 Config::m_ignore_vjoy_version_warning = true;
                 Config::Save(g_engine); // Save immediately
             }
        }
    } else {
        std::cerr << "[vJoy] Failed to load vJoyInterface.dll. Please ensure it is in the same folder as the executable." << std::endl;
        MessageBoxA(NULL, "Failed to load vJoyInterface.dll.\n\nPlease ensure vJoy is installed and the DLL is in the app folder.", "LMUFFB Error", MB_ICONERROR | MB_OK);
    }

    // Track acquisition state locally
    bool vJoyAcquired = false;

    std::cout << "[FFB] Loop Started." << std::endl;

    while (g_running) {
        if (g_ffb_active && g_pTelemetry) {
            double force = 0.0;
            {
                // PROTECT SETTINGS: Use mutex because GUI modifies engine parameters
                std::lock_guard<std::mutex> lock(g_engine_mutex);
                force = g_engine.calculate_force(g_pTelemetry);
            }

            // --- DYNAMIC vJoy LOGIC (State Machine) ---
            if (vJoyDllLoaded && DynamicVJoy::Get().Enabled()) {
                // STATE 1: User wants vJoy, but we don't have it -> TRY ACQUIRE
                if (Config::m_output_ffb_to_vjoy && !vJoyAcquired) {
                    VjdStat status = DynamicVJoy::Get().GetStatus(VJOY_DEVICE_ID);
                    if ((status == VJD_STAT_OWN) || ((status == VJD_STAT_FREE) && DynamicVJoy::Get().Acquire(VJOY_DEVICE_ID))) {
                        vJoyAcquired = true;
                        std::cout << "[vJoy] Device " << VJOY_DEVICE_ID << " acquired for debug output." << std::endl;
                    }
                }
                // STATE 2: User disabled vJoy, but we hold it -> RELEASE
                else if (!Config::m_output_ffb_to_vjoy && vJoyAcquired) {
                    DynamicVJoy::Get().Relinquish(VJOY_DEVICE_ID);
                    vJoyAcquired = false;
                    std::cout << "[vJoy] Device " << VJOY_DEVICE_ID << " relinquished." << std::endl;
                }

                // STATE 3: If owned, update axis
                if (vJoyAcquired) {
                    long axis_val = (long)((force + 1.0) * 0.5 * (axis_max - axis_min) + axis_min);
                    DynamicVJoy::Get().SetAxis(axis_val, VJOY_DEVICE_ID, 0x30); 
                }
            }
            
            // Update DirectInput (for FFB)
            DirectInputFFB::Get().UpdateForce(force);
        }

        // Sleep 2ms ~ 500Hz. Ideally use high_resolution_clock wait for precise 400Hz.
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    if (vJoyAcquired) {
        DynamicVJoy::Get().Relinquish(VJOY_DEVICE_ID);
    }
    std::cout << "[FFB] Loop Stopped." << std::endl;
}

// --- GUI / Main Loop (Low Priority 60Hz or Lazy) ---
int main(int argc, char* argv[]) {
    bool headless = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--headless") {
            headless = true;
        }
    }

    std::cout << "Starting LMUFFB (C++ Port)..." << std::endl;

    // Load Configuration
    Config::Load(g_engine);

    // Initialize GUI Early (if not headless)
    if (!headless) {
        if (!GuiLayer::Init()) {
            std::cerr << "Failed to initialize GUI." << std::endl;
            // Fallback? Or exit?
            // If explicit GUI build failed, we probably want to exit or warn.
            // For now, continue but set g_running false if critical.
            // Actually, GuiLayer::Init() handles window creation.
        }
        
        // Initialize DirectInput (Requires HWND)
        DirectInputFFB::Get().Initialize((HWND)GuiLayer::GetWindowHandle());
        
    } else {
        std::cout << "Running in HEADLESS mode." << std::endl;
        // Headless DI init (might fail if HWND is NULL but some drivers allow it, or windowless mode)
        DirectInputFFB::Get().Initialize(NULL);
    }

    // 1. Setup Shared Memory
    // We try to connect. If fails, we still keep the app running (if GUI is active) 
    // so user can see config and try again later or restart game.
    HANDLE hMapFile = OpenFileMappingA(FILE_MAP_READ, FALSE, SHARED_MEMORY_NAME);
    if (hMapFile == NULL) {
        std::cerr << "Could not open file mapping object. Ensure game is running." << std::endl;
        if (!headless) {
             // Show non-blocking error or just a popup but DON'T exit
             MessageBoxA(NULL, "Could not open file mapping object.\n\nApp will remain open. Start Le Mans Ultimate and restart this app or wait for connection logic (future feature).", "LMUFFB Warning", MB_ICONWARNING | MB_OK);
        } else {
             return 1; // Headless has no UI to wait, so exit
        }
    } else {
        g_pTelemetry = (rF2Telemetry*)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, sizeof(rF2Telemetry));
        if (g_pTelemetry == NULL) {
            std::cerr << "Could not map view of file." << std::endl;
            CloseHandle(hMapFile);
        } else {
            std::cout << "Connected to Shared Memory." << std::endl;
        }
    }

    // 2. Start FFB Thread
    std::thread ffb_thread(FFBThread);

    // 3. Main GUI Loop
    std::cout << "[GUI] Main Loop Started." << std::endl;

    while (g_running) {
        // Render returns true if the GUI is active (mouse over, focused).
        // If false, we can sleep longer (Lazy Rendering).
        bool active = GuiLayer::Render(g_engine);
        
        if (active) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60Hz
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // ~10Hz Background
        }
    }
    
    // Save Config on Exit
    Config::Save(g_engine);

    // Cleanup
    if (!headless) GuiLayer::Shutdown();
    if (ffb_thread.joinable()) ffb_thread.join();
    
    DirectInputFFB::Get().Shutdown();
    
    if (g_pTelemetry) UnmapViewOfFile(g_pTelemetry);
    if (hMapFile) CloseHandle(hMapFile);
    
    return 0;
}

```

# File: README.md
```markdown
# lmuFFB

A FFB app for LMU, similar to irFFB and Marvin’s iRacing App

Experimental alpha version.

![lmuFFB GUI](docs/screenshots/main_app.png)
![lmuFFB GUI2](docs/screenshots/ffb_analysis.png)


## Installation & Configuration  

### 1. Prerequisites
*   **vJoy Driver**: Install version **2.1.9.1** (by jshafer817) or compatible. Download from [vJoy releases](https://github.com/jshafer817/vJoy/releases).
    *   *Why vJoy?* The game needs a "dummy" device to bind steering to, so it doesn't try to send its own FFB to your real wheel while lmuFFB is controlling it.
    *   *Tip:* **Disable all vJoy FFB Effects** in the "Configure vJoy" tool, except "Constant Force" (though lmuFFB drives your wheel directly, this prevents vJoy from trying to interfere if you use legacy mode).
*   **rF2 Shared Memory Plugin**: Download `rFactor2SharedMemoryMapPlugin64.dll` from [TheIronWolfModding](https://github.com/TheIronWolfModding/rF2SharedMemoryMapPlugin#download) or use the version bundled with [CrewChief](https://thecrewchief.org/).
    *   **Installation**: Copy `rFactor2SharedMemoryMapPlugin64.dll` to your `Le Mans Ultimate/Plugins/` folder.
    *   *Note:* LMU does not have an in-game menu to enable plugins like rFactor 2 did. It is usually enabled by default if the file is present.

### 2. Step-by-Step Setup

**A. Configure vJoy**
1.  Open **Configure vJoy**.
2.  Set up **Device 1** with at least **X Axis** enabled.
3.  Click Apply.

**B. Configure Le Mans Ultimate (LMU)**
1.  Start LMU.
2.  Go to **Settings > Graphics**:
    *   Set **Display Mode** to **Borderless**. (Prevents crashes/minimizing).
3.  Go to **Controls / Bindings**.
    *   *Screenshot:* ![LMU Controls](docs/screenshots/lmu_controls.png) *(To be added)*
4.  **Steering Axis**: 
    *   **Method A (Direct - Recommended):** Bind to your **Physical Wheel**.
        
        **Step-by-Step Setup:**
        1.  **Clean Slate:** Close Joystick Gremlin and vJoy Feeder if running.
        2.  **Game Setup:**
            *   Start Le Mans Ultimate.
            *   Bind **Steering** directly to your **Physical Wheel** (e.g., Simucube, Fanatec, Moza, Logitech).
            *   **Important:** Set In-Game FFB Strength to **0%** (or "Off").
        3.  **App Setup:**
            *   Start LMUFFB.
            *   **CRITICAL STEP:** In the LMUFFB window, look for the **"FFB Device"** dropdown menu.
            *   Click it and select your **Physical Wheel** from the list.
            *   *Note:* You **must** do this manually. If the dropdown says "Select Device...", the app is calculating forces but sending them nowhere.
        4.  **Verify:**
            *   Check the console for errors. If you select your wheel and **do not** see a red error like `[DI] Failed to acquire`, then it is connected!
            *   Drive the car. You should feel the physics-based FFB.
        
        **Troubleshooting - No FFB:**
        *   **Check Console Messages:** While driving, look for `[DI Warning] Device unavailable` repeating in the console.
            *   **YES, I see the warning:** The game has 'Locked' your wheel in Exclusive Mode. You cannot use the Direct Method. You must use Method B (vJoy Bridge).
            *   **NO, console is clean:** The game might be overwriting the signal. Try **Alt-Tabbing** out of the game. If FFB suddenly kicks in when the game is in the background, it confirms the game is interfering. Use Method B.
        *   **Try Adjusting Settings:** If you feel no FFB, try tweaking these values in lmuFFB:
            *   **Master Gain:** Increase from 0.5 to 1.0 or higher.
            *   **SOP (Seat of Pants):** Increase from 0.0 to 0.3 (you should feel lateral forces in corners).
            *   **Understeer Effect:** Ensure it's at 1.0 (default).
        
        *   *Pros:* Simplest setup. No vJoy required.
        *   *Cons:* If LMU "locks" the device (Exclusive Mode), LMUFFB might fail to send forces. If this happens, try Method B.
    *   **Method B (vJoy Bridge - Compatibility):** Bind to **vJoy Device (Axis Y)**.
        *   *Requirement:* You MUST use **Joystick Gremlin** (or similar) to map your Physical Wheel to vJoy Axis Y. The "vJoy Demo Feeder" is NOT sufficient for driving.
        *   *Why Axis Y?* LMUFFB uses Axis X for FFB monitoring (if enabled). Using Y prevents conflicts.
5.  **Force Feedback**:
    *   **Type**: Set to "None" (if available) or reduce **FFB Strength** to **0%** / **Off**.
        *   *Note:* LMU may not have a "None" option; reducing strength to 0 is the workaround.
    *   **Effects**: Set "Force Feedback Effects" to **Off**.
    *   **Smoothing**: Set to **0** (Raw).
    *   **Advanced**: Set "Collision Strength" and "Steering Torque Sensitivity" to **0%**.
    *   **Tweaks**: Disable "Use Constant Steering Force Effect".

**C. Configure lmuFFB (The App)**
1.  Run `LMUFFB.exe`.
2.  **FFB Device**: In the dropdown, select your **Physical Wheel** (e.g., "Simucube 2 Pro", "Fanatec DD1").
    *   *Note:* Do NOT select the vJoy device here. You want lmuFFB to talk to your real wheel.
3.  **Master Gain**: Start low (0.5) and increase.
4.  **SOP Effect**: Start at **0.0**. This effect injects lateral G-force. On High-Torque wheels (DD), this can cause violent oscillation if set too high.
5.  Drive! You should feel force feedback generated by the app.

### Troubleshooting

- **Wheel Jerking / Fighting**: You likely have a "Double FFB" conflict.
    - Ensure in-game Steering is bound to **vJoy**, NOT your real wheel.
    - Ensure in-game FFB is sending to vJoy.
    - If the wheel oscillates on straights, reduce **SOP Effect** to 0.0 and increase smoothing.
- **No Steering (Car won't turn)**:
    - If you used **Method B (vJoy)**, you need **Joystick Gremlin** running to bridge your wheel to vJoy. The "vJoy Demo Feeder" is for testing only.
- **No FFB**: 
    - Ensure the "FFB Device" in lmuFFB is your real wheel.
    - Check if the Shared Memory Plugin is working (Does "Connected to Shared Memory" appear in the console?).
- **"vJoyInterface.dll not found"**: Ensure the DLL is in the same folder as the executable. You can grab it from `C:\Program Files\vJoy\SDK\lib\amd64\` or download from the [vJoy GitHub](https://github.com/shauleiz/vJoy/tree/master/SDK/lib/amd64/vJoyInterface.dll).
    - *Alternative:* You can try moving `LMUFFB.exe` directly into `C:\Program Files\vJoy\x64\` if you have persistent DLL issues.
- **"Could not open file mapping object"**: Start the game and load a track first.

## Known Issues (v0.3.3)
*   **Telemetry Gaps**: Some users report missing telemetry for Dashboard apps (ERS, Temps). lmuFFB has robust fallbacks, but if `mGripFract` is missing (Tire Temps broken), the Understeer effect may be static. See [Telemetry Report](docs/dev_docs/telemetry_availability_report.md).


## Feedback & Support

For feedback, questions, or support:
*   **LMU Forum Thread**: [irFFB for LMU (lmuFFB)](https://community.lemansultimate.com/index.php?threads/irffb-for-lmu-lmuffb.10440/)
*   **GitHub Issues**: [Report bugs or request features](https://github.com/coasting-nc/LMUFFB/issues)


## Documentation

*   [The Physics of Feel - Driver's Guide](docs/the_physics_of__feel_-_driver_guide.md) - **Comprehensive guide** explaining how lmuFFB translates telemetry into tactile sensations, with telemetry visualizations
*   [FFB Effects & Customization Guide](docs/ffb_effects.md)
*   [FFB Customization Guide (Legacy)](docs/ffb_customization.md)
*   [GUI Framework Options](docs/gui_framework_options.md)
*   [DirectInput Implementation Guide](docs/dev_docs/directinput_implementation.md)
*   [Telemetry Data Reference](docs/dev_docs/telemetry_data_reference.md)
*   [vJoy Compatibility Guide](docs/vjoy_compatibility.md)
*   [Comparisons with Other Apps](docs/comparisons.md)



## Architecture

The application reads telemetry from the rFactor 2 engine (Le Mans Ultimate) via Shared Memory and calculates a synthetic Force Feedback signal to send to DirectInput.

## Features

*   **High Performance Core**: Native C++ Multi-threaded architecture.
    *   **FFB Loop**: Runs at ~400Hz to match game physics.
    *   **GUI Loop**: Runs at ~60Hz with lazy rendering to save CPU.
*   **Real-time Tuning GUI**:
    *   Built with **Dear ImGui** for responsive adjustment of parameters.
    *   Sliders for Master Gain, Understeer (Grip Loss), SoP (Seat of Pants), and Min Force.
    *   Toggles for Texture effects (Slide Rumble, Road Details).
*   **Custom Effects**:
    *   **Grip Modulation**: Feel the wheel lighten as front tires lose grip.
    *   **Dynamic Oversteer**: Counter-steering force suggestion based on rear axle alignment torque (v0.2.2).
    *   **Progressive Lockup**: Feel the edge of tire braking limit before full lock (v0.2.2).
    *   **Traction Loss**: Feel the rear "float" and spin up under power (v0.2.2).
*   **Easy Installation**: Inno Setup installer script included to manage dependencies (vJoy, Plugins).


## Building (for developers)

### Prerequisites for all methods
*   **Compiler**: MSVC (Visual Studio 2022 Build Tools) or generic C++ compiler.
*   **Build System**: CMake (3.10+).
*   **vJoy**:
    *   **Users**: Install the **vJoy Driver**. We recommend version **2.1.9.1** (by jshafer817) for Windows 10/11 compatibility. See [vJoy Compatibility Guide](docs/vjoy_compatibility.md) for download links and details.
    *   **Developers**: The vJoy Installer typically installs the **SDK** (headers and libraries) to `C:\Program Files\vJoy\SDK`. lmuFFB links against this SDK.
*   **Dear ImGui (Optional)**: Download from [GitHub](https://github.com/ocornut/imgui) and place in `vendor/imgui` to enable the GUI.

### Option A: Visual Studio 2022 (IDE)
1.  Open Visual Studio.
2.  Select "Open a local folder" and choose the repo root.
3.  Visual Studio will auto-detect `CMakeLists.txt`.
4.  Open `CMakeSettings.json` (or Project Settings) to verify the variable `VJOY_SDK_DIR` points to your vJoy SDK location (Default: `C:/Program Files/vJoy/SDK`).
5.  Select **Build > Build All**.

### Option B: Visual Studio Code
1.  Install **VS Code**.
2.  Install extensions: **C/C++** (Microsoft) and **CMake Tools** (Microsoft).
3.  Open the repo folder in VS Code.
4.  When prompted to configure CMake, select your installed compiler kit (e.g., *Visual Studio Community 2022 Release - x86_amd64*).
5.  Open `.vscode/settings.json` (or create it) to set the vJoy path:
    ```json
    {
        "cmake.configureSettings": {
            "VJOY_SDK_DIR": "C:/Path/To/vJoy/SDK"
        }
    }
    ```
6.  Click **Build** in the bottom status bar.

### Option C: Command Line (Windows)
1.  Open the **Developer Command Prompt for VS 2022**.
2.  Navigate to the repository root.
3.  Run the following commands:
    ```cmd
    mkdir build
    cd build
    cmake -G "NMake Makefiles" -DVJOY_SDK_DIR="C:/Path/To/vJoy/SDK" ..
    nmake
    ```
    *Alternatively, use `cmake --build .` instead of `nmake`.*


## Building the Installer

To create the `LMUFFB_Setup.exe`:

1.  **Install Inno Setup**: Download from [jrsoftware.org](https://jrsoftware.org/isdl.php).
2.  **Build the Project**: Ensure you have built the `Release` version of `LMUFFB.exe` using Visual Studio.
3.  **Run Compiler**: Open `installer/lmuffb.iss` in Inno Setup Compiler and click **Compile**.
4.  **Output**: The installer will be generated in `installer/Output/`.


```

# File: README.txt
```
lmuFFB - Le Mans Ultimate Force Feedback
========================================
Version: 0.3.7

See README.md for full documentation with images and links.

PREREQUISITES
-------------
1. vJoy Driver (Version 2.1.9.1 recommended)
   Download: https://github.com/jshafer817/vJoy/releases
   
   Why vJoy? LMU needs a "dummy" device to bind steering to, so it doesn't 
   try to send its own FFB to your real wheel while lmuFFB is controlling it.
   
   Configuration:
   - Open "Configure vJoy" tool
   - Set up Device 1 with at least X Axis enabled
   - Disable all vJoy FFB Effects EXCEPT "Constant Force"
   - Click Apply

2. rF2 Shared Memory Plugin (rFactor2SharedMemoryMapPlugin64.dll)
   Download from one of these sources:
   - https://github.com/TheIronWolfModding/rF2SharedMemoryMapPlugin#download
   - Bundled with CrewChief: https://thecrewchief.org/
   
   Installation:
   - Copy rFactor2SharedMemoryMapPlugin64.dll to "Le Mans Ultimate/Plugins/" folder
   - Note: LMU does not have an in-game menu to enable plugins. It is enabled 
     automatically if the file is present.

3. vJoyInterface.dll (Required for lmuFFB to run)
   This DLL MUST be in the same folder as LMUFFB.exe
   
   Download from:
   - C:\Program Files\vJoy\SDK\lib\amd64\ (if you installed vJoy SDK)
   - https://github.com/shauleiz/vJoy/tree/master/SDK/lib/amd64/vJoyInterface.dll


STEP-BY-STEP SETUP
------------------

A. Configure Le Mans Ultimate (LMU)
   1. Start LMU
   2. Go to Settings > Graphics:
      - Set Display Mode to "Borderless" (Prevents crashes/minimizing)
   
   3. Go to Controls / Bindings
   4. Steering Axis - Choose ONE method:
      
      METHOD A (Direct - Recommended):
      Step-by-Step Setup:
      1. Clean Slate: Close Joystick Gremlin and vJoy Feeder if running
      2. Game Setup:
         - Start Le Mans Ultimate
         - Bind Steering directly to your Physical Wheel 
           (e.g., Simucube, Fanatec, Moza, Logitech)
         - IMPORTANT: Set In-Game FFB Strength to 0% (or "Off")
      3. App Setup:
         - Start LMUFFB
         - CRITICAL STEP: In the LMUFFB window, find the "FFB Device" dropdown
         - Click it and select your Physical Wheel from the list
         - Note: You MUST do this manually. If it says "Select Device...", 
           the app is calculating forces but sending them nowhere
      4. Verify:
         - Check the console for errors. If you select your wheel and do NOT 
           see a red error like "[DI] Failed to acquire", then it is connected!
         - Drive the car. You should feel the physics-based FFB
      
      Troubleshooting - No FFB:
      - Check Console Messages: While driving, look for "[DI Warning] Device 
        unavailable" repeating in the console
        * YES, I see the warning: The game has 'Locked' your wheel in Exclusive 
          Mode. You cannot use the Direct Method. You must use Method B (vJoy Bridge)
        * NO, console is clean: The game might be overwriting the signal. Try 
          Alt-Tabbing out of the game. If FFB suddenly kicks in when the game 
          is in the background, it confirms the game is interfering. Use Method B
      - Try Adjusting Settings: If you feel no FFB, try tweaking these values:
        * Master Gain: Increase from 0.5 to 1.0 or higher
        * SOP (Seat of Pants): Increase from 0.0 to 0.3 (feel lateral forces)
        * Understeer Effect: Ensure it's at 1.0 (default)
      
      Pros: Simplest setup. No vJoy required
      Cons: If wheel has no FFB, try Method B
      
      METHOD B (vJoy Bridge - Compatibility):
      - Bind to "vJoy Device (Axis Y)"
      - IMPORTANT: You MUST use "Joystick Gremlin" to map your wheel to vJoy Axis Y.
      - Why? Guarantees separation of FFB and Input.
   
   5. Force Feedback Settings:
      - Type: Set to "None" (if available) or reduce FFB Strength to 0% / Off
      - Note: LMU may not have a "None" option; reducing strength to 0 is 
        the workaround.
      - Effects: Set "Force Feedback Effects" to Off
      - Smoothing: Set to 0 (Raw) for fastest telemetry updates
      - Advanced: Set Collision Strength and Torque Sensitivity to 0%
      - Tweaks: Disable "Use Constant Steering Force Effect"

B. Configure lmuFFB (The App)
   1. Run LMUFFB.exe
   2. FFB Device: Select your Physical Wheel (e.g., "Simucube 2 Pro", "Fanatec DD1")
      - Do NOT select the vJoy device here
   3. Master Gain: Start low (0.5) and increase gradually
   4. SOP Effect: Start at 0.0
      - This effect injects lateral G-force
      - On High-Torque wheels (DD), this can cause violent oscillation if too high
   5. Drive! You should feel force feedback generated by the app


TROUBLESHOOTING
---------------

Wheel Jerking / Fighting:
  - You likely have a "Double FFB" conflict
  - Ensure LMU Steering is bound to vJoy, NOT your real wheel
  - Ensure LMU FFB is sending to vJoy (or disabled/strength at 0)
  - If the wheel oscillates on straights, reduce SOP Effect to 0.0

No Steering (Car won't turn):
  - If using Method B (vJoy), you need Joystick Gremlin running
  - Ensure it maps your wheel to vJoy Axis Y
  - The "vJoy Demo Feeder" is for testing only, not driving

No FFB:
  - Ensure "FFB Device" in lmuFFB is your real wheel
  - Check if Shared Memory Plugin is working (console should show "Connected")
  - Verify the plugin DLL is in Le Mans Ultimate/Plugins/ folder

"vJoyInterface.dll not found":
  - Copy vJoyInterface.dll to the same folder as LMUFFB.exe
  - Get it from: C:\Program Files\vJoy\SDK\lib\amd64\
  - Or download: https://github.com/shauleiz/vJoy/tree/master/SDK/lib/amd64/vJoyInterface.dll

"Could not open file mapping object":
  - Start LMU and load a track first
  - The Shared Memory Plugin only activates when driving


KNOWN ISSUES (v0.3.7)
---------------------
- Telemetry Gaps: Some users report missing telemetry for Dashboard apps 
  (ERS, Temps). lmuFFB has robust fallbacks, but if Tire Temperature data 
  is broken, the Understeer effect may be static.


FEEDBACK & SUPPORT
------------------
For feedback, questions, or support:
- LMU Forum Thread: 
  https://community.lemansultimate.com/index.php?threads/irffb-for-lmu-lmuffb.10440/
- GitHub Issues: 
  https://github.com/coasting-nc/LMUFFB/issues


For full documentation, advanced settings, and developer information, 
see README.md or visit: https://github.com/coasting-nc/LMUFFB

```

# File: rF2Data.h
```cpp
#ifndef RF2DATA_H
#define RF2DATA_H

#include <cstdint>

// rFactor 2 Telemetry Data Structures
// Based on The Iron Wolf's rF2SharedMemoryMapPlugin and rFactor 2 SDK

// Ensure strict alignment if necessary, but standard rF2 SDK usually works with default packing.
// However, the Shared Memory Plugin might align things specifically.
// We will use standard alignment matching the Python definition (which used native).
// Usually in C++ on Windows x64, doubles are 8-byte aligned.

#pragma pack(push, 4) // rFactor 2 often uses 4-byte packing for some legacy reasons, or default. 
// We will assume default packing for now, but if offsets are off, we might need #pragma pack(push, 1) or 4.
// Looking at the Python struct, we didn't specify _pack_, so it used native.
// Let's use standard layout.

struct rF2Vec3 {
    double x;
    double y;
    double z;
};

struct rF2Wheel {
    double mSuspensionDeflection;
    double mRideHeight;
    double mSuspForce;
    double mBrakeTemp;
    double mBrakePressure;
    double mRotation;
    double mLateralPatchVel;
    double mLongitudinalPatchVel;
    double mLateralGroundVel;
    double mLongitudinalGroundVel;
    double mCamber;
    double mLateralForce;
    double mLongitudinalForce;
    double mTireLoad;
    double mGripFract;
    double mPressure;
    double mTemperature[3]; // Inner, Middle, Outer
    double mWear;
    char mTerrainName[16];
    unsigned char mSurfaceType;
    unsigned char mFlat;
    unsigned char mDetached;
    unsigned char mPadding[5]; // Align next double? Python handled this automatically.
                               // In Python ctypes: c_byte, c_byte, c_byte follow each other.
                               // Then c_double starts. on x64, double aligns to 8.
                               // 16 + 1 + 1 + 1 = 19. Next double at 24. Padding = 5.
    
    double mStaticCamber;
    double mToeIn;
    double mTireRadius;
    double mVerticalTireDeflection;
    double mWheelYLocation;
    double mToe;
    double mCaster;
    double mHAngle;
    double mVAngle;
    double mSlipAngle;
    double mSlipRatio;
    double mMaxSlipAngle;
    double mMaxLatGrip;
};

struct rF2Telemetry {
    double mTime;
    double mDeltaTime;
    double mElapsedTime;
    int mLapNumber;
    double mLapStartET;
    char mVehicleName[64];
    char mTrackName[64];
    rF2Vec3 mPos;
    rF2Vec3 mLocalVel;
    rF2Vec3 mLocalAccel;
    rF2Vec3 mOri[3]; // [3][3] rotation matrix rows/cols? Usually 3 vectors.
    rF2Vec3 mLocalRot;
    rF2Vec3 mLocalRotAccel;
    double mSpeed;
    double mEngineRPM;
    double mEngineWaterTemp;
    double mEngineOilTemp;
    double mClutchRPM;
    double mUnfilteredThrottle;
    double mUnfilteredBrake;
    double mUnfilteredSteering;
    double mUnfilteredClutch;
    double mSteeringArmForce;
    double mFuel;
    double mEngineMaxRPM;
    unsigned char mScheduledStops;
    unsigned char mOverheating;
    unsigned char mDetached;
    unsigned char mHeadlights;
    // Padding to align int?
    // 4 chars = 4 bytes. Next is int (4 bytes). Aligned.
    int mGear;
    int mNumGears;
    // Next is rF2Wheel which starts with double (8 bytes).
    // Current pos: int(4) + int(4) = 8. Aligned.
    
    rF2Wheel mWheels[4]; // FL, FR, RL, RR
};

#pragma pack(pop)

#endif // RF2DATA_H

```

# File: .pytest_cache\README.md
```markdown
# pytest cache directory #

This directory contains data from the pytest's cache plugin,
which provides the `--lf` and `--ff` options, as well as the `cache` fixture.

**Do not** commit this to version control.

See [the docs](https://docs.pytest.org/en/stable/how-to/cache.html) for more information.

```

# File: docs\architecture.md
```markdown
# Architecture

LMUFFB uses a multi-threaded architecture implemented in C++ to ensure minimal latency for the Force Feedback signal while allowing for future GUI expansion.

## Deign choices

The app follows best practices for real-time signal generation. A recent update transitioned some remaining "canned" effects to physics-based audio/haptics (Phase Integration).

## High-Level Pipeline

```
[ Simulator (LMU) ]
       |
       v (Shared Memory)
       |
[ Telemetry Reader ]  <-- (Memory Mapped File)
       |
       v (Structs: rF2Telemetry)
       |
[ FFB Engine ]  <-- (FFBThread - 400Hz)
       |
       v (Calculated Force)
       |
[ vJoy Interface ]
       |
       v
[ Physical Wheel ]
```

## Components

### 1. Telemetry Interface (Shared Memory)
The application connects to the **rFactor 2 Shared Memory Map Plugin** (by The Iron Wolf).

*   **Implementation**: Windows API `OpenFileMappingA` and `MapViewOfFile`.
*   **Structs**: Defined in `rF2Data.h`, these mirror the C++ structs used by the plugin, allowing direct access to physics data without parsing overhead.

### 2. Threading Model
The application is split into two primary threads:

*   **FFB Thread (High Priority)**:
    *   Runs at **400Hz** (approx 2.5ms interval) to match the physics update rate of the simulator.
    *   Sole responsibility: Read telemetry -> Calculate Force -> Update vJoy axis.
    *   This isolation ensures that GUI rendering or OS background tasks do not introduce jitter into the FFB signal.
*   **Main/GUI Thread (Low Priority)**:
    *   Runs at **60Hz** (or lower if inactive).
    *   **GuiLayer (`src/GuiLayer.h`)**:
        *   Manages the Win32 Window and DirectX 11 device.
        *   Initializes the Dear ImGui context.
        *   Renders the settings window (`DrawTuningWindow`).
    *   Implements "Lazy Rendering": If `GuiLayer::Render()` reports no activity and the window is not focused, the update rate drops to ~10Hz to save CPU cycles.

### 3. FFB Engine (`FFBEngine.h`)
The core logic is encapsulated in a header-only class to facilitate unit testing.

*   **Inputs**: `SteeringArmForce`, `GripFract` (FL/FR), `LocalAccel` (Lateral G), `VerticalTireDeflection` (Suspension), `SlipAngle`.
*   **Features**:
    *   **Grip Modulation**: Scales force by grip fraction (Understeer feel).
    *   **SoP**: Adds lateral G-force (Oversteer feel).
    *   **Slide Texture**: Injects high-frequency noise when slip angle > threshold.
    *   **Road Texture**: Injects force based on high-frequency suspension velocity changes.
    *   **Min Force**: Boosts small signals to overcome wheel friction.

### 4. Output Driver
*   **vJoy**: The C++ application links against `vJoyInterface.lib` to communicate with the vJoy driver.
*   **Mechanism**: The calculated force (float -1.0 to 1.0) is scaled to the vJoy axis range (1 to 32768) and sent via `SetAxis`.

```

# File: docs\comparisons.md
```markdown
# Comparisons with Other FFB Apps

LMUFFB draws inspiration from established tools in the sim racing community. Here is how it compares.

## vs iRFFB (iRacing)

**iRFFB** is the benchmark for external FFB apps.

*   **Telemetry Source**: iRFFB reads from the iRacing API. LMUFFB reads from rF2/LMU Shared Memory.
*   **Philosophy**:
    *   **iRFFB**: "Reconstruction" (smoothing 60Hz) or "360Hz" (raw physics).
    *   **LMUFFB**: Similar to "360Hz" mode. It relies on the rFactor 2 engine's already high-fidelity `SteeringArmForce` (updated at 400Hz). Its primary value add is **modulating** this force based on `GripFract` (which rF2 provides directly) rather than estimating grip loss from slip angles.
*   **Understeer/Oversteer**: iRFFB estimates understeer via slip angle vs. torque curves. LMUFFB uses the explicit "Grip Fraction" variable exposed by the rF2 engine.

## vs Marvin's AIRA (iRacing)

**Marvin's AIRA** is known for its tactile enhancement features.

*   **FFB Philosophy**:
    *   **Marvin's AIRA**: "Detail Augmentation". Splits effects (Road, Curb, Slip) and boosts specific frequencies.
    *   **LMUFFB**: Now implements similar **Texture** features. The C++ engine includes:
        *   **Slide Texture**: Synthetic noise injection during high slip angles.
        *   **Road Texture**: High-pass filtering of suspension deflection to boost road noise.
*   **Architecture**: Marvin's AIRA is a complex WPF/.NET application. LMUFFB is a native C++ application optimized for minimal latency.

## vs TinyPedal (rFactor 2 / LMU)

**TinyPedal** is a popular overlay tool for rF2/LMU.

*   **Similarities**: Both use the **rFactor 2 Shared Memory Map Plugin**.
*   **Differences**: TinyPedal is a passive **Overlay** (Read-Only -> Display). LMUFFB is an active **Control** app (Read-Write -> vJoy).

## Comparison Matrix

| Feature | iRFFB | Marvin's AIRA | LMUFFB (C++) |
| :--- | :--- | :--- | :--- |
| **Telemetry Rate** | 60Hz / 360Hz | 60Hz / 360Hz | **400Hz** |
| **FFB Output Method** | **DirectInput** (Direct Mode) | **DirectInput** | **DirectInput** (Constant Force) |
| **vJoy Usage** | Optional (for Input Upsampling) | No | Optional (for Input Mapping) |
| **Latency** | Low (API Overhead) | Low (.NET Overhead) | **Ultra-Low (Native C++ / Shared Mem)** |
| **Grip Loss Feel** | Calc. from Slip | Calc. from Slip | **Direct Grip Telemetry** |
| **SoP Effect** | Yes | Yes | **Yes (Lateral G)** |
| **Road Texture** | No (Pass-thru) | **Yes (Boosted)** | **Yes (Suspension Delta)** |
| **Braking Lockup** | No | **Pedal Haptics** | **Yes (Wheel Rumble)** |
| **Wheel Spin** | No | **Pedal Haptics** | **Yes (Wheel Rumble)** |
| **GUI** | Full GUI | Advanced GUI | **Yes (Dear ImGui)** |

## DirectInput Implementation Deep Dive

A key differentiator for FFB apps is how they talk to the hardware.

### 1. iRFFB (The Pioneer)
*   **Method**: Uses DirectInput `Constant Force` effects.
*   **Latency Management**: Uses a "Direct Mode" that bypasses vJoy to talk to the wheel. It reads telemetry via the iRacing SDK (memory mapped file, updated at 360Hz or 60Hz depending on mode).
*   **Signal Processing**: Heavily relies on **Interpolation/Extrapolation**. Since iRacing's disk telemetry is 60Hz, iRFFB's "Reconstruction" filter predicts the signal to smooth out steps. In 360Hz mode, it uses the raw physics tick.
*   **Challenge**: The iRacing SDK update timing can jitter, so iRFFB has complex logic to sync the FFB thread.

### 2. Marvin's AIRA (The Specialist)
*   **Method**: Uses DirectInput, but often layers multiple effects.
*   **Architecture**: Built on .NET/WPF. While powerful, the managed runtime (C#) introduces potential Garbage Collection (GC) pauses, though usually micro-optimized to be imperceptible.
*   **Effect Layering**: Marvin's strength is splitting the signal. It might send a base `Constant Force` for torque, but overlay `Periodic` (Sine/Triangle) effects for road texture or engine vibration. This "Composition" approach allows for richer detail but higher complexity in managing effect slots on the wheel driver.

### 3. LMUFFB (This Project)
*   **Method**: Native C++ DirectInput `Constant Force` effect.
*   **Update Rate**: **Native 400Hz**. The rFactor 2 engine (LMU) updates physics at 400Hz and writes to Shared Memory at the same rate. LMUFFB reads this directly (Zero Copy via `MapViewOfFile`) and updates the DI effect immediately.
*   **Advantage**: **No Interpolation Needed**. Unlike iRFFB (in 60Hz mode), LMUFFB gets the raw high-frequency signal. The latency is purely the time to read memory + compute float math + call `SetParameters`.
*   **Simplicity**: LMUFFB uses a single "Master" Constant Force effect. All internal effects (SoP, Texture, Grip) are mathematically mixed into this single signal *before* sending it to the driver. This ensures we never run out of "Effect Slots" on cheaper wheels (some Logitech wheels only support 4 concurrent effects).

```

# File: docs\directinput_implementation.md
```markdown
# DirectInput FFB Implementation Guide

This document outlines the technical steps required to transition LMUFFB from a vJoy-based "Axis Mapping" architecture to a native **DirectInput Force Feedback** architecture. This change allows the application to send specific "Constant Force" packets directly to the steering wheel driver, bypassing the need for a virtual joystick and allowing the application to coexist seamlessly with the game's input system.

## 1. Overview

**Priority: CRITICAL / REQUIRED**

Currently, LMUFFB acts as a virtual joystick (`vJoy`) and maps the calculated force to the **Axis Position**. This visualizes the force but does **not** drive the physical motors of a user's steering wheel.
To function as a true Force Feedback application (like iRFFB or Marvin's AIRA), LMUFFB **must** implement a DirectInput client that opens the physical wheel and sends `Constant Force` packets.

*Hypothetical Feature Note: Implementing DirectInput correctly is complex due to device enumeration, exclusive locking (cooperative levels), and handling lost devices. It effectively turns the app into a specialized driver client.*

## 2. Technical Requirements

*   **API**: DirectInput8 (via `dinput8.lib` / `dinput8.dll`).
*   **Language**: C++ (Native COM interfaces).
*   **Privileges**: Exclusive access to the FFB device is often required (`DISCL_EXCLUSIVE | DISCL_BACKGROUND`).

## 3. Implementation Steps

### Phase 1: Device Enumeration & Initialization
Instead of connecting to vJoy ID 1, we must scan connected hardware.

```cpp
IDirectInput8* g_pDI = NULL;
IDirectInputDevice8* g_pDevice = NULL;

// 1. Create DirectInput Object
DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pDI, NULL);

// 2. Enumerate Devices (Filter for Wheels/FFB)
g_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK);

// 3. Callback Logic
BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) {
    // Check if device supports FFB
    // Create Device
    g_pDI->CreateDevice(pdidInstance->guidInstance, &g_pDevice, NULL);
    return DIENUM_STOP; // Stop after finding first FFB wheel
}
```

### Phase 2: Setting Cooperative Level
This is critical. FFB usually requires Exclusive/Background access so forces continue when the app is minimized (running alongside the game).

```cpp
g_pDevice->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND);
g_pDevice->SetDataFormat(&c_dfDIJoystick);
g_pDevice->Acquire();
```

### Phase 3: Creating the Effect
We need a **Constant Force** effect.

```cpp
DIEFFECT diEffect;
DICONSTANTFORCE diConstantForce;
LPDIRECTINPUTEFFECT g_pEffect = NULL;

// Initialize parameters
diConstantForce.lMagnitude = 0;

diEffect.dwSize = sizeof(DIEFFECT);
diEffect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
diEffect.dwDuration = INFINITE;
diEffect.dwGain = DI_FFNOMINALMAX;
diEffect.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
diEffect.lpvTypeSpecificParams = &diConstantForce;
// ... (Set Axes/Directions) ...

// Create
g_pDevice->CreateEffect(GUID_ConstantForce, &diEffect, &g_pEffect, NULL);
g_pEffect->Start(1, 0);
```

### Phase 4: Updating the Force (The Loop)
Inside the `FFBThread` (400Hz loop), instead of calling `SetAxis` (vJoy), we update the effect.

```cpp
void UpdateDirectInputForce(double normalizedForce) {
    if (!g_pEffect) return;

    // Map -1.0..1.0 to -10000..10000
    LONG magnitude = (LONG)(normalizedForce * 10000.0);
    
    // Clamp
    if (magnitude > 10000) magnitude = 10000;
    if (magnitude < -10000) magnitude = -10000;

    DICONSTANTFORCE cf;
    cf.lMagnitude = magnitude;

    DIEFFECT eff;
    ZeroMemory(&eff, sizeof(eff));
    eff.dwSize = sizeof(DIEFFECT);
    eff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
    eff.lpvTypeSpecificParams = &cf;

    // Send to driver (Low latency call)
    g_pEffect->SetParameters(&eff, DIEP_TYPESPECIFICPARAMS | DIEP_START);
}
```

## 4. Challenges & Solutions

1.  **Device Selection**: Users may have multiple controllers (Handbrake, Shifter, Wheel). The GUI must allow selecting the specific FFB device from a list.
2.  **Spring/Damper Effects**: Some wheels default to a heavy centering spring. The app should explicitly create specific Spring/Damper effects with 0 magnitude to "clear" the driver's default behavior.
3.  **Loss of Focus**: Even with `DISCL_BACKGROUND`, some games (or drivers) steal exclusive access. The app must handle `DIERR_NOTACQUIRED` errors and attempt to `Acquire()` periodically.

## 5. Benefits
*   **Latency**: Bypasses the vJoy -> Driver bridge.
*   **Usability**: User does not need to configure vJoy. They just select their wheel in LMUFFB.
*   **Compatibility**: Works with games that don't support multiple controllers well (though LMU is generally good with this).

```

# File: docs\ffb_customization.md
```markdown
# Customization of Tire Grip FFB

One of the primary advantages of external FFB applications like LMUFFB is the ability to tailor the force feedback sensation to the driver's preference.

The C++ version of LMUFFB implements the following customizable effects:

## 1. Understeer Effect (Grip Modulation)
*   **Description**: Modulates the strength of the force feedback based on the tire's available grip.
*   **Implementation**: `OutputForce = GameForce * (1.0 - (1.0 - Grip) * SliderValue)`
*   **Tuning**:
    *   **100% (1.0)**: Maximal lightness when understeering.
    *   **0% (0.0)**: Force follows game physics purely (heavy even when sliding).

## 2. Seat of Pants (SoP) / Oversteer
*   **Description**: Simulates the lateral G-forces acting on the driver's body by injecting lateral force into the steering wheel.
*   **Implementation**: `TotalForce += (LateralAccel / 9.81) * ScalingFactor * SliderValue`
*   **Tuning**: Higher values help catch oversteer earlier by feeling the "weight" of the car shifting.

## 3. Slide Texture
*   **Description**: Adds a synthetic vibration or "scrubbing" texture when the tires are sliding laterally.
*   **Implementation**: Injects high-frequency noise (Sine wave or Random) when `SlipAngle > Threshold` or `GripFract < Threshold`.
*   **Tuning**:
    *   **Gain**: Amplitude of the vibration.
    *   **Toggle**: On/Off.

## 4. Road Texture
*   **Description**: Amplifies high-frequency vertical suspension movements to enhance the feeling of curbs, bumps, and road surface details.
*   **Implementation**: Uses a high-pass filter (delta of `mVerticalTireDeflection`) to detect bumps.
*   **Tuning**:
    *   **Gain**: Strength of the amplification. Useful for damping-heavy wheels.

## 5. Min Force
*   **Description**: Boosts small force signals to overcome the internal friction/deadzone of mechanical wheels (Gears/Belts).
*   **Implementation**: If force is non-zero but below threshold, set it to threshold (preserving sign).

```

# File: docs\ffb_effects.md
```markdown
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
*   **Current Dynamic Implementation (v0.2.2+)**:
    *   **Aligning Torque Integration**: Calculates a synthetic "Aligning Torque" for the rear axle using `Rear Lateral Force`.
    *   **Mechanism**: This force is injected into the steering signal. If the rear tires generate large lateral forces (resisting a slide), the steering wheel will naturally counter-steer, providing a physical cue to catch the slide. This is modulated by the `Oversteer Boost` slider.
    *   **SoP (Seat of Pants)**: Also injects Lateral G-force into the wheel torque to provide "weight" cues.

## 3. Braking Lockup (Progressive Scrub)
*   **Goal**: To signal when tires have stopped rotating during braking (flat-spotting risk), allowing the driver to find the threshold.
*   **Current Dynamic Implementation (v0.2.2+)**:
    *   **Progressive Vibration**: Signal is derived from `SlipRatio` deviation.
    *   **Range**: -0.1 (Peak Grip) to -0.5 (Locking).
    *   **Frequency**: Transitions from High Pitch (60Hz) at the limit to Low Pitch (10Hz) at full lock.
    *   **Amplitude**: Scales linearly with severity.
*   **Customization**:
    *   **Lockup Rumble (Toggle)**: Enable/Disable.
    *   **Lockup Gain (Slider)**: Intensity of the vibration.

## 4. Wheel Spin (Traction Loss)
*   **Goal**: To signal when the driven wheels are spinning under power.
*   **Current Dynamic Implementation (v0.2.2+)**:
    *   **Torque Reduction**: As rear wheel slip increases, the total FFB force is reduced (simulating "floating" rear end).
    *   **Vibration**: Frequency scales with wheel speed difference (Slip Ratio), giving a "revving up" sensation through the rim.
*   **Customization**:
    *   **Spin Traction Loss (Toggle)**: Enable/Disable.
    *   **Spin Gain (Slider)**: Intensity.

## 5. Road & Slide Texture
*   **Slide Texture**: Adds "scrubbing" vibration when any tire is sliding laterally (high Slip Angle).
*   **Road Texture**: Adds "bumps" based on suspension velocity changes (High-Pass Filter).

---

## Signal Interference & Clarity

A critical challenge in FFB design is managing the "Noise Floor". When multiple effects are active simultaneously, they can interfere with each other or mask the underlying physics.

### 1. Signal Masking
*   **The Issue**: High-frequency vibrations (like **Lockup Rumble** or **Road Texture**) can physically overpower subtle torque changes (like **Understeer Lightness** or **SoP**). If the wheel is vibrating violently due to a lockup, the driver might miss the feeling of the rear end stepping out (SoP).
*   **Mitigation**:
    *   **Priority System**: Future versions should implement "Side-chaining" or "Ducking". For example, if a severe Lockup event occurs, reduce Road Texture gain to ensure the Lockup signal is clear.
    *   **Frequency Separation**: Ideally, "Information" (Grip/SoP) should be low-frequency (< 20Hz), while "Texture" (Lockup/Spin/Road) should be high-frequency (> 50Hz). This helps the human hand distinguish them.

### 2. Clipping
*   **The Issue**: Summing multiple effects (Game Torque + SoP + Rumble) can easily exceed the 100% force capability of the motor.
*   **Result**: The signal "clips" (flattens at max force). Information is lost. E.g., if you are cornering at 90% torque and a 20% SoP effect is added, you hit 100% and lose the detail of the SoP ramp-up.
*   **Mitigation**:
    *   **Master Limiter**: A soft-clip algorithm that compresses dynamic range rather than hard-clipping.
    *   **Tuning**: Users are advised to set "Master Gain" such that peak cornering forces hover around 70-80%, leaving headroom for dynamic effects.

### 3. Ambiguity (Texture Confusion)
*   **The Issue**: **Lockup** and **Wheel Spin** often use similar "Synthetic Rumble" effects. In the heat of battle, a driver might confuse one for the other if relying solely on the tactile cue without context (pedal position).
*   **Mitigation**:
    *   **Distinct Frequencies**: Future updates will tune Lockup to be "Sharper/Higher Pitch" (square wave) and Wheel Spin to be "Rougher/Lower Pitch" (sawtooth or randomized).
    *   **Context**: Since the driver knows if they are braking or accelerating, this ambiguity is usually resolved by context, but distinct tactile signatures help subconscious reaction times.


### 4. Interaction of Spin vs SoP Effects

*   The **Spin** effect reduces `total_force` (Torque Drop).
*   The **SoP** effect boosts force during oversteer.
*   **Result**: These two will fight slightly during a power slide. This is actually a good "natural" balance—the wheel tries to self-align (SoP), but the loss of traction makes it feel lighter/vaguer (Spin Drop). This should feel intuitive to the driver.


---

## Comparison of Implementation with iRFFB and Marvin's AIRA

| Effect | iRFFB (iRacing) | Marvin's AIRA (iRacing) | LMUFFB (LMU/rF2) |
| :--- | :--- | :--- | :--- |
| **Oversteer** | **SoP (Lateral G)** + Yaw logic | **Layered Effect**: Separate "Slip" channel. | **Rear Aligning Torque + SoP**: Synthetic rear-axle torque integration. |
| **Lockup** | Not explicit (part of "Understeer" feel in iRacing logic) | **Pedal Haptics** (often sent to pedals, but can be on wheel) | **Progressive Wheel Scrub**: Dynamic frequency/amplitude based on slip ratio. |
| **Wheel Spin** | Not explicit | **Pedal Haptics** / Wheel Rumble | **Torque Drop + Vibration**: Simulates traction loss + progressive rumble. |

---

## Legacy Implementation Notes (Pre-v0.2.2)

*   **Old Oversteer**: Relied solely on Grip Delta between Front/Rear to boost SoP.
*   **Old Lockup**: Binary rumble triggered when `SlipRatio < -0.2`.
*   **Old Wheel Spin**: Binary rumble triggered when `SlipRatio > 0.2`.


```

# File: docs\implementation_report_v0.3.md
```markdown
# Implementation Report v0.3.0

This document outlines the implementation details for the version 0.3.0 update, focusing on the transition from synthetic "canned" effects to dynamic physics-based signals with correct phase integration.

## 1. Analysis of Requirements

The user requested improvements based on `docs/dev_docs/report_on_ffb_improvements.md`. Key findings from the analysis:

*   **The Math Problem**: Using absolute time (`mElapsedTime`) with dynamic frequencies causes phase discontinuity ("clicks/pops").
    *   **Solution**: Adopt **Phase Accumulation** (`phase += freq * dt`).
*   **Wheel Spin**: The initial proposal to link spin to Engine RPM was rejected due to "noise" concerns on gear/belt wheels.
    *   **Solution**: Link vibration frequency to **Tire Slip Speed** (Car Speed * Slip Ratio). This ensures silence during grip and progressive feedback during slip.
*   **Lockup**: Link vibration to **Car Speed** (scrubbing pitch) and amplitude to **Tire Load**.



## 2. Implementation Details

### A. Phase Integration
Added member variables to `FFBEngine` to track the current phase of each oscillator:
*   `m_lockup_phase`
*   `m_spin_phase`
*   `m_slide_phase`

These accumulate `frequency * delta_time` each frame, ensuring smooth waveform continuity even when frequency modulates rapidly.

### B. Progressive Lockup
*   **Trigger**: Brake input + Slip Ratio < -0.1.
*   **Frequency**: `10Hz + (CarSpeed_ms * 1.5)`. Transitions from low judder at low speed to high-pitch scrub at high speed.
*   **Amplitude**: Scaled by `severity` (slip depth) AND `lockup_gain`.
*   **Waveform**: Sine wave (smoothed).

### C. Traction Loss (Wheel Spin)
*   **Trigger**: Throttle input + Slip Ratio > 0.2.
*   **Torque Drop**: Reduced total FFB gain by up to 60% based on slip severity. This provides the "floating" feeling of a powered slide.
*   **Vibration**:
    *   **Frequency**: Derived from **Slip Speed** (`CarSpeed * SlipRatio`).
    *   Mapping: Low slip speed = Low Hz (Grip fighting). High slip speed = High Hz (Free spinning).
    *   This replaces the "RPM" proposal to ensure the effect is purely tire-dynamics based.

### D. Slide Texture
*   **Trigger**: Lateral Slip Angle > 0.15 rad.
*   **Frequency**: Derived from `LateralGroundVel` (sideways speed).
*   **Waveform**: Changed from Sine to **Sawtooth** approximation (`(phase / 2PI) * 2 - 1`). This provides a sharper "stick-slip" texture more characteristic of rubber sliding on asphalt.
*   **Amplitude**: Modulated by `TireLoad`. Heavily loaded tires vibrate more violently.

## 3. Deviations from Initial Suggestions

*   **RPM Link**: Rejected as per "Follow up questions" analysis. Implemented **Slip Speed** logic instead.
*   **Load Factor**: Simplified load factor normalization to avoid extreme values.

## 4. Verification
*   **Tests**: Updated `tests/test_ffb_engine.cpp` to mock `mDeltaTime` and verify phase accumulation logic does not reset (except wrap-around) or jump discontinuously.

# Implementation Report v0.3.1

This document outlines the implementation details for version 0.3.0, focusing on advanced telemetry integration (`mLateralPatchVel`, `mTireLoad`) and refined signal processing.

## 1. Requirements Analysis

The user request (based on `docs/report_on_ffb_improvements.md`) asked for:
*   **Physics-based Frequencies**: Using Phase Integration to avoid clicks.
*   **Tire Load Scaling**: Modulating effects based on vertical load.
*   **Patch Velocity**: Using actual sliding speed for texture frequency.
*   **Code Review Fixes**: Safety clamps for load factors.

## 2. Implementation Details

### A. Phase Integration (Solved "Math Problem")
Implemented `m_phase += freq * dt` logic for all oscillators (Lockup, Spin, Slide).
*   **Benefit**: Eliminates audio artifacts/clicks when frequency modulates rapidly.
*   **State**: Added `m_lockup_phase`, `m_spin_phase`, `m_slide_phase`.

### B. Global Load Factor
Calculated `load_factor = avg_load / 4000.0` at the start of `calculate_force`.
*   **Clamp**: Added `std::min(1.5, ...)` safety clamp to prevent violence during high-compression events (Eau Rouge).
*   **Usage**: Applied to **Lockup**, **Slide**, and **Road Texture**. This makes the FFB feel "heavy" under load and "light" over crests.

### C. Advanced Slide Texture
*   **Frequency**: Switched from static or slip-angle based to `mLateralPatchVel` (Lateral Patch Velocity).
    *   Mapping: 1 m/s -> 40Hz, 10 m/s -> 200Hz.
    *   Result: Accurate "scrubbing" pitch that rises with slide speed.
*   **Amplitude**: Scaled by `load_factor`.

### D. Refined Lockup & Spin
*   **Lockup**: Frequency linked to `CarSpeed` (10Hz -> 80Hz). Amplitude linked to `LoadFactor`.
*   **Spin**: Frequency linked to `SlipSpeed` (CarSpeed * SlipRatio). Torque drop-off implemented.

### E. Road Texture Refinement
*   **Logic**: Applied `load_factor` to the existing suspension-delta logic. Bumps now feel harder when the suspension is loaded.

## 3. Deviations

*   **RPM Usage**: Explicitly rejected Engine RPM for Traction Loss (as per user request). Used `SlipSpeed` instead.
*   **Bottoming Out**: Did not implement `mSuspensionDeflection` logic yet as car-specific limits are unknown/variable.
*   **Terrain FX**: String parsing `mTerrainName` left for future optimization (string compare in 400Hz loop needs care).

## 4. Verification
*   **Tests**: Updated `tests/test_ffb_engine.cpp` to verify phase accumulation and load factor logic.

```

# File: docs\introduction.md
```markdown
# Introduction to LMUFFB

**LMUFFB** (Le Mans Ultimate Force Feedback) is a specialized high-performance application designed to enhance the driving experience in the *Le Mans Ultimate* simulator. Its primary goal is to provide **Force Feedback (FFB)** signals that communicate tire physics—specifically tire grip and loss of traction—more effectively than the game's native output.

This project is a response to the community's need for tools similar to **iRFFB** and **Marvin's AIRA**, which have transformed the FFB landscape for simulators like iRacing.

## Scope & Goal

The core scope of LMUFFB is:
1.  **Telemetry Acquisition**: Reading high-fidelity vehicle physics data from the simulator in real-time.
2.  **FFB Processing**: Applying algorithms to this data to calculate a "synthetic" steering force that emphasizes the "Seat of Pants" (SoP) feel and pneumatic trail effects.
3.  **Signal Output**: Sending this calculated force to the player's steering wheel via a virtual joystick driver (vJoy).

By doing so, LMUFFB allows players to feel when the car is understeering or oversteering through the steering wheel's resistance, a critical feedback loop for driving at the limit.

**Current Status**: The project has migrated from a Python prototype to a **native C++ application** to ensure sub-millisecond latency and consistent 400Hz update rates.

```

# File: docs\licensing.md
```markdown
# Licensing Analysis for LMUFFB

This document analyzes the licensing requirements for distributing LMUFFB, specifically concerning its dependencies: Dear ImGui, vJoy, and the rFactor 2 Shared Memory Plugin interface.

## 1. Components

### Dear ImGui
*   **License**: MIT License.
*   **Terms**: The MIT License allows for the use, copying, modification, merger, publication, distribution, sublicense, and/or sale of copies of the Software.
*   **Restriction**: The substantial portion of the Software (the license text) must be included in all copies or substantial portions of the Software.
*   **Implication**: You **can** distribute a compiled version of LMUFFB linked with Dear ImGui. You must include the Dear ImGui license text in your distribution (e.g., in an `About` box or a `LICENSE-IMGUI.txt` file).

### vJoy SDK
*   **License**: MIT License (Source: [shauleiz/vJoy on GitHub](https://github.com/shauleiz/vJoy)).
*   **Terms**: Standard MIT permissions.
*   **Implication**: You can distribute the application linked against `vJoyInterface.lib` / `vJoyInterface.dll` provided you respect the MIT attribution.

### rFactor 2 Shared Memory Map Plugin
*   **Component**: We rely on the header definitions (`struct` layout) and the concept of the plugin.
*   **License**: MIT License (Source: [TheIronWolfModding/rF2SharedMemoryMapPlugin](https://github.com/TheIronWolfModding/rF2SharedMemoryMapPlugin)).
*   **Implication**: Using the struct definitions to interoperate with the plugin is permitted.

## 2. Conclusion & Selection

The project has selected the **GNU General Public License v3.0 (GPL-3.0)** for the LMUFFB source code.

This choice ensures that:
1.  The project remains free software.
2.  Any improvements or modifications (forks) must also be released under the GPL-3.0 (Copyleft).

### Compatibility with Dependencies (MIT)
This combination is **fully compatible**.
*   **The Scenario**: A GPL-3.0 application (LMUFFB) linking against MIT-licensed libraries (Dear ImGui, vJoy).
*   **Legal Mechanic**: The MIT license is permissive and GPL-compatible. It grants the right to sublicense the library code. When compiled together, the resulting binary is distributed under the terms of the GPL-3.0.
*   **Redistribution**: You can legally distribute the `LMUFFB.exe` binary.

## 3. Redistribution Requirements

When you release the binary (`LMUFFB.exe`) or the installer, you must adhere to the following:

1.  **GPL Obligations**:
    *   You must provide the source code of LMUFFB (or a written offer to provide it) to anyone who receives the binary. Hosting this GitHub repository fulfills this.
    *   The binary itself is covered by the GPL-3.0.

2.  **MIT Obligations (Attribution)**:
    *   You must preserve the copyright notices of the MIT components.
    *   **Action**: Include a file named `THIRD-PARTY-NOTICES.txt` in the distribution containing the MIT license texts for **Dear ImGui** and **vJoy**.

### Summary for End Users
*   **LMUFFB**: Free software (GPL-3.0). You have the right to modify and share it.
*   **Included Libraries**: Free software (MIT). They are used by LMUFFB to provide GUI and Joystick functionality.

```

# File: docs\the_physics_of__feel_-_driver_guide.md
```markdown
# The Physics of Feel: LMUFFB Driver's Guide

This guide explains how LMUFFB translates raw telemetry data into tactile sensations. It details what you should feel in specific driving situations and provides a blueprint for visualizing these relationships through telemetry graphs.

## Part 1: Individual Driving Phenomena

### 1. Understeer (Front Grip Loss)

<img src="telemetry_visualizations/understeer.jpg" alt="Plot A (FFB Output): Final FFB Signal showing a drop in amplitude despite increased steering. Plot B (Input): Steering Angle increasing. Plot C (Physics): Front Tire Grip Fraction dropping from 1.0 to < 0.8. Plot D (Physics): Lateral G-Force plateauing or dropping." width="50%">

<img src="telemetry_visualizations/understeer2.jpg" alt="Additional understeer telemetry visualization showing the relationship between steering input, grip fraction, and FFB output." align="right" width="40%">

**The Situation:** You turn the wheel, but the car continues straight. The front tires have exceeded their slip angle limit and are scrubbing across the asphalt.

**The FFB Sensation:**
As you turn the wheel further, instead of the resistance increasing (as it would with a spring), the wheel suddenly goes **light**. The weight falls out of the steering, signaling that adding more steering angle is futile.

**How it Works:**
LMUFFB monitors the `GripFraction` of the front tires. As this value drops below 1.0 (100%), the application reduces the global steering force.

<br clear="right">

---

### 2. Oversteer (Rear Grip Loss / SoP)

<img src="telemetry_visualizations/oversteer.jpg" alt="Plot A (FFB Output): SoP/Oversteer Force Component spiking. Plot B (Physics): Lateral G-Force high amplitude. Plot C (Physics): Grip Delta (Front Grip minus Rear Grip). Plot D (Input): Steering Angle showing the driver counter-steering in response." width="50%">

<img src="telemetry_visualizations/oversteer2.jpg" alt="Additional oversteer telemetry visualization showing SoP effect and rear grip loss dynamics." align="right" width="40%">

**The Situation:** The rear tires lose grip, and the back of the car rotates (yaws) faster than the front. The car is sliding sideways.

**The FFB Sensation:**
You feel a distinct **pull** in the direction of the slide, urging you to counter-steer. Simultaneously, you feel the "weight" of the car shifting sideways through the rim, giving you an early warning before your eyes even detect the rotation.

**How it Works:**
This is the "Seat of Pants" (SoP) effect. LMUFFB injects Lateral G-force (`mLocalAccel.x`) into the steering signal. Additionally, it calculates a synthetic "Aligning Torque" based on the rear axle's lateral forces, boosting the signal when the rear grip drops below the front grip.

<br clear="right">

---

### 3. Braking Lockup (Threshold Braking)

<img src="telemetry_visualizations/brake_lockup.jpg" alt="Plot A (FFB Output): Lockup Rumble Signal showing frequency change. Plot B (Input): Brake Pedal Position at 100%. Plot C (Physics): Wheel Slip Ratio dropping below -0.1. Plot D (Physics): Car Speed decreasing, correlating with the changing frequency in Plot A." width="50%">

<img src="telemetry_visualizations/brake_lockup2.jpg" alt="Additional brake lockup telemetry visualization showing the relationship between speed, slip ratio, and vibration frequency." align="right" width="40%">

**The Situation:** You stomp on the brakes. One or more tires stop rotating while the car is still moving. The rubber is dragging along the road surface.

**The FFB Sensation:**
You feel a **vibration** that changes pitch based on your speed.
*   **High Speed:** A high-frequency "screeching" buzz (approx 60-80Hz).
*   **Low Speed:** A low-frequency "judder" or "grinding" (approx 10-20Hz).
*   *Note:* The vibration is stronger when the tire is heavily loaded (downforce/weight transfer) and fades if the tire is unloaded.

**How it Works:**
LMUFFB detects when `SlipRatio` is less than -0.1. It generates a sine wave where the **Frequency** is linked to Car Speed (`mLocalVel.z`) and the **Amplitude** is linked to Vertical Tire Load (`mTireLoad`).

<br clear="right">

---

### 4. Traction Loss (Power Wheel Spin)

<img src="telemetry_visualizations/traction_loss_power_wheelspin.jpg" alt="Plot A (FFB Output): Total Force showing a sudden drop/notch plus Vibration overlay. Plot B (Input): Throttle Position at 100%. Plot C (Physics): Rear Wheel Slip Ratio spiking > 0.2. Plot D (Physics): Slip Speed (m/s) correlating with vibration pitch." width="50%">

<img src="telemetry_visualizations/traction_loss_power_wheelspin2.jpg" alt="Additional traction loss telemetry visualization showing torque drop and vibration dynamics during wheel spin." align="right" width="40%">

**The Situation:** You apply full throttle in a low gear. The rear tires break traction and spin significantly faster than the road speed.

**The FFB Sensation:**
The steering wheel feels **vague and floating**, as if the rear of the car has detached from the road. Overlaid on this lightness is a smooth, high-frequency **hum** that rises in pitch as the wheels spin up.

**How it Works:**
LMUFFB detects positive `SlipRatio`.
1.  **Torque Drop:** It multiplies the total force by a reduction factor (e.g., 0.6x), creating the "floating" sensation.
2.  **Vibration:** It generates a vibration based on **Slip Speed** (the difference in m/s between tire surface and road).

<br clear="right">

---

### 5. Slide Texture (Lateral Scrubbing)

<img src="telemetry_visualizations/slide_lateral_scrubbing.jpg" alt="Plot A (FFB Output): Slide Texture Signal with Sawtooth waveform. Plot B (Physics): Lateral Patch Velocity (m/s). Plot C (Physics): Tire Slip Angle (rad). Plot D (Physics): Vertical Tire Load modulating the amplitude." width="50%">

<img src="telemetry_visualizations/slide_lateral_scrubbing2.jpg" alt="Additional slide texture telemetry visualization showing lateral scrubbing dynamics and tire load effects." align="right" width="40%">

**The Situation:** You are cornering hard. The car isn't spinning, but the tires are operating at a high slip angle, "crabbing" or scrubbing sideways across the asphalt.

**The FFB Sensation:**
A granular, **sandpaper-like texture** through the rim. It feels like the tire is "tearing" at the road surface.

**How it Works:**
When `LateralPatchVel` (the speed at which the contact patch slides sideways) is high, LMUFFB injects a **Sawtooth** wave. The sawtooth shape mimics the "stick-slip" physics of rubber friction better than a smooth sine wave.

<br clear="right">

---

## Part 2: Complex Interactions & Dynamics

This section details how LMUFFB handles conflicting signals to create a cohesive, natural driving feel.

### 1. The Power Slide (Spin vs. SoP)

<img src="telemetry_visualizations/power_slide_spin_vs_sop.jpg" alt="Plot A (FFB Output): Total Force showing vector direction (SoP) but chopped amplitude (Spin). Plot B (Physics): Lateral G High, driving the SoP. Plot C (Physics): Rear Slip Ratio High, driving the Torque Drop. Plot D (Physics): Yaw Rate showing the car rotation." width="50%">

<img src="telemetry_visualizations/power_slide_spin_vs_sop2.jpg" alt="Additional power slide telemetry visualization showing the interaction between Spin effect and SoP effect." align="right" width="40%">

**The Scenario:** Exiting a corner, you mash the throttle. The rear end steps out violently (Oversteer), but the wheels are also spinning wildly (Traction Loss).

**The Interaction:**
*   **The Conflict:**
    *   **SoP Effect** wants to *increase* force to tell you the car is rotating and urge a counter-steer.
    *   **Spin Effect** wants to *decrease* force to simulate the loss of rear friction and the "floating" rear axle.
*   **The Result:** A **"Light Counter-Steer"**. The wheel pulls in the direction of the correction (SoP), but the resistance is lower than normal (Spin Drop).
*   **Why it feels natural:** This mimics reality. When rear tires are spinning, they have very little lateral grip. The steering should guide you into the slide, but it shouldn't feel heavy or "locked in" because the rear of the car is effectively floating on a layer of molten rubber.

<br clear="right">

---

### 2. The "Dive" (Load Transfer vs. Understeer)

<img src="telemetry_visualizations/trail_braking_load_transfer_vs_understeer.jpg" alt="Plot A (FFB Output): Total Force showing rising peak, then sudden drop-off. Plot B (Physics): Front Tire Load spiking due to weight transfer. Plot C (Physics): Front Grip Fraction dropping as tires saturate. Plot D (Input): Brake Pressure vs Steering Angle." width="50%">

<img src="telemetry_visualizations/trail_braking_load_transfer_vs_understeer2.jpg" alt="Additional trail braking telemetry visualization showing the interaction between load transfer and understeer effect." align="right" width="40%">

**The Scenario:** You brake hard while turning into a corner (Trail Braking). The weight of the car transfers to the front tires.

**The Interaction:**
*   **The Conflict:**
    *   **Load Sensitivity** sees massive weight on the front tires (3000N -> 6000N). This *increases* the amplitude of road textures and mechanical trail.
    *   **Understeer Effect** watches the grip limit. If you brake too hard, you exceed the grip circle, and the effect tries to *reduce* force.
*   **The Result:** **"Heavy to Light Transition"**. Initially, the wheel feels incredibly heavy and detailed (due to Load Sensitivity) as the nose dives. As you exceed the limit, the weight suddenly vanishes (Understeer Effect), giving you a clear tactile cliff edge: "You pushed too hard."

<br clear="right">

---

### 3. The "Tank Slapper" (Snap Oversteer Recovery)

<img src="telemetry_visualizations/tank_slapper_snap_oversteer.jpg" alt="Plot A (FFB Output): Total Force showing the violent reversal/spike. Plot B (Physics): Rear Slip Ratio dropping from High to Zero. Plot C (Physics): Lateral G crossing zero rapidly. Plot D (Physics): Steering Arm Force showing the raw game physics aligning torque returning." width="50%">

<img src="telemetry_visualizations/tank_slapper_snap_oversteer2.jpg" alt="Additional tank slapper telemetry visualization showing snap oversteer recovery dynamics." align="right" width="40%">

**The Scenario:** You are counter-steering to catch a slide. Suddenly, the rear tires regain grip.

**The Interaction:**
*   **The Sequence:**
    1.  **Slide Phase:** SoP is high (pushing wheel correction), Spin Drop might be active.
    2.  **Grip Regain:** Rear Slip Ratio drops to 0. Lateral G snaps the other way.
*   **The Result:** **"The Snap"**.
    *   The "Spin Torque Drop" vanishes instantly (Force returns to 100%).
    *   The SoP vector reverses direction violently as the car snaps back.
    *   The FFB delivers a high-torque "whip" motion, simulating the stored energy in the chassis releasing. This cues the driver to center the wheel immediately to avoid over-correcting into a wall.

<br clear="right">
```

# File: docs\vjoy_compatibility.md
```markdown
# vJoy Version Compatibility

This document outlines the vJoy version requirements for LMUFFB. Compatibility issues between vJoy versions, Windows versions (10 vs 11), and Force Feedback apps are common.

## Recommendation

**We recommend using vJoy 2.1.9.1 (by jshafer817) for Windows 10 and 11.**

*   **Download**: [vJoy 2.1.9.1 Releases](https://github.com/jshafer817/vJoy/releases)
*   **Reasoning**: This version is signed for Windows 10/11 and includes fixes for Force Feedback (FFB) that were broken in some 2.1.8 releases on newer OS updates.

## Historical Context (iRFFB)

Users coming from iRFFB might be familiar with specific version requirements:
*   **Legacy (Windows 10 older builds)**: vJoy 2.1.8.39 (by shauleiz) was the standard.
*   **Modern (Windows 10 20H2+ / Windows 11)**: The original driver signature expired or was rejected by newer Windows security features. The fork by **jshafer817** (2.1.9.1) resolved this and is now the standard for sim racing tools on modern Windows.

## Compatibility Table

| OS Version | Recommended vJoy Version | Notes |
| :--- | :--- | :--- |
| **Windows 11** | **2.1.9.1** | Must use the jshafer817 fork. Original 2.1.9 may fail to install. |
| **Windows 10 (20H2+)** | **2.1.9.1** | Preferred for FFB stability. |
| **Windows 10 (Old)** | 2.1.8.39 | Legacy standard, acceptable if already installed. |

## Important Note on "Existing" Installations

If you already have vJoy installed for iRFFB:
1.  **Check Version**: Open `vJoyConf` or `Monitor vJoy` to see the version number.
2.  **Keep it**: If it works for iRFFB, it will work for LMUFFB. LMUFFB uses the standard `vJoyInterface.dll` API which is backward compatible.
3.  **Upgrade**: Only upgrade if you are experiencing "Failed to acquire device" errors or missing FFB. **Uninstall the old version completely before installing the new one.**

```

# File: docs\archived\bug_report_bottoming_logic.md
```markdown
# Bug Report: Suspension Bottoming Directional Logic

**Date:** 2025-05-23
**Status:** Resolved (v0.3.3)
**Component:** `FFBEngine.h` (Suspension Bottoming Effect)

## Description
The "Suspension Bottoming" effect, introduced in v0.3.2, is intended to provide a heavy haptic impulse when the suspension travels to its limit (detected via high Tire Load). 

However, the current implementation applies this force as a **DC Offset** (constant addition) whose direction is determined by the *current* total force. 

```cpp
double sign = (total_force > 0) ? 1.0 : -1.0; 
total_force += bump_force * sign;
```

## The Issue
When driving on a straight line, `total_force` is near zero (e.g., oscillating between -0.001 and 0.001 due to noise or minor steering adjustments). 
If the car hits a bump or compression (e.g., Eau Rouge compression) while the wheel is centered:
1.  `total_force` might be slightly negative (e.g., -0.0001).
2.  The logic calculates `sign` as `-1.0`.
3.  The `bump_force` (which can be large, e.g., 20% of max force) is added in the negative direction.
4.  **Result:** The wheel violently jerks to the LEFT (or RIGHT) depending on the micro-state of the FFB at that instant.

This feels unnatural. A vertical compression should result in a "heavy" feeling or a vibration/jolt that doesn't necessarily pull the wheel to one side unless the geometry dictates it (bump steer). In a straight line, it should probably be a high-frequency vibration (crunch) or simply increase the resistance to movement (damping), not an active turning force.

## Proposed Fix
Instead of a DC offset, use a synthesized waveform that feels like a mechanical limit hit, regardless of steering direction.

**Option A: High-Frequency Pulse (Crunch)**
Inject a 50-60Hz sine wave burst. This feels like a "thud" or "shudder" rather than a pull.
```cpp
// 50Hz Crunch
double crunch = std::sin(m_bottoming_phase) * bump_force;
total_force += crunch;
```

**Option B: Resistance (Friction/Damping)**
Increase the damping coefficient momentarily, making the wheel feel "heavy/stuck". (Harder to implement in the current "Force-only" architecture without Damping support).

**Recommendation:** Implement Option A (Pulse/Crunch) in the next release.


## Corrected Code Implementation

You need to modify `FFBEngine.h` to add the phase state variable and update the calculation logic.

**Step 1: Add the state variable**
Add `m_bottoming_phase` to the class member variables (alongside `m_lockup_phase`, etc.).

**Step 2: Update the logic**
Replace the DC offset logic with the oscillator logic.

Here is the corrected file segment for `FFBEngine.h`:

```cpp
class FFBEngine {
public:
    // ... [Existing Variables] ...

    // Phase Accumulators
    double m_lockup_phase = 0.0;
    double m_spin_phase = 0.0;
    double m_slide_phase = 0.0;
    double m_bottoming_phase = 0.0; // <--- ADD THIS

    double calculate_force(const rF2Telemetry* data) {
        // ... [Existing Code] ...

        // --- 5. Suspension Bottoming (High Load Impulse) ---
        if (m_bottoming_enabled) {
            // Detect sudden high load spikes which indicate bottoming out
            double max_load = (std::max)(fl.mTireLoad, fr.mTireLoad);
            
            // Threshold: 8000N is a heavy hit
            const double BOTTOM_THRESHOLD = 8000.0;
            
            if (max_load > BOTTOM_THRESHOLD) {
                double excess = max_load - BOTTOM_THRESHOLD;
                
                // Non-linear response (Square root softens the initial onset)
                double bump_magnitude = std::sqrt(excess) * m_bottoming_gain * 0.5;
                
                // FIX: Use a 50Hz "Crunch" oscillation instead of directional DC offset
                double freq = 50.0; 
                
                // Phase Integration
                m_bottoming_phase += freq * dt * TWO_PI;
                if (m_bottoming_phase > TWO_PI) m_bottoming_phase -= TWO_PI;

                // Generate vibration (Sine wave)
                // This creates a heavy shudder regardless of steering direction
                double crunch = std::sin(m_bottoming_phase) * bump_magnitude;
                
                total_force += crunch;
            }
        }

        // ... [Rest of Code] ...
    }
};
```

## Recommendation
Apply this fix immediately. The current implementation causes erratic behavior that could be misinterpreted by users as a hardware fault or "clipping" issue. The fix aligns with the "Phase Integration" architecture introduced in v0.3.0.
```

# File: docs\archived\gui_framework_options.md
```markdown
# GUI Framework Options for LMUFFB (C++ Version)

To transform LMUFFB from a console application to a user-friendly tool, a Graphical User Interface (GUI) is required. This document evaluates the options for implementing the GUI in C++.

## Priorities
1.  **Maintainability**: Code should be easy to read and update.
2.  **Robustness**: Stability is critical for a real-time FFB app.
3.  **Ease of Implementation**: Fast iteration time.

## Option 1: Dear ImGui (Recommended)
**Dear ImGui** is an immediate-mode GUI library designed for real-time applications (game engines, tools).

*   **Pros**:
    *   **Extremely Fast Development**: UI is defined in code logic. Adding a slider is one line of code: `ImGui::SliderFloat("Gain", &gain, 0.0f, 1.0f);`.
    *   **Performance**: Designed for high frame rates (ideal for visualizing telemetry graphs).
    *   **Lightweight**: Tiny footprint, no DLL hell, compiles into the executable.
    *   **Modern Look**: Can be styled easily (Docking, Dark Mode).
*   **Cons**:
    *   **Non-Standard Look**: Does not look like a native Windows app (looks like a game tool).
    *   **CPU Usage**: Redraws every frame (though can be optimized with "lazy" rendering).
*   **Suitability**: **High**. Perfect for a "tuner" app where sliders and graphs are the main focus.

## Option 2: Qt (Widgets or Quick)
**Qt** is a comprehensive cross-platform application framework.

*   **Pros**:
    *   **Professional Look**: Native styling or custom skins.
    *   **Rich Features**: Extensive library (networking, threading, persistence).
    *   **Visual Designer**: Qt Designer allows drag-and-drop UI creation.
*   **Cons**:
    *   **Bloat**: Requires shipping large DLLs (Qt5Core.dll, Qt5Gui.dll, etc.).
    *   **Licensing**: LGPL/Commercial constraints.
    *   **Complexity**: Meta-Object Compiler (MOC), signals/slots paradigm adds build complexity.
*   **Suitability**: **Medium**. Overkill for a simple FFB tuner, but good if the app grows into a complex suite.

## Option 3: wxWidgets
**wxWidgets** is a C++ library that lets developers create applications for Windows, macOS, Linux using native widgets.

*   **Pros**:
    *   **Native Look**: Uses Win32 API under the hood on Windows.
    *   **Stable**: Mature and widely used.
*   **Cons**:
    *   **Old-School API**: Event tables and macros can feel dated compared to modern C++.
    *   **Verbose**: Defining layouts in code is verbose.
*   **Suitability**: **Low**. Harder to make "cool" custom widgets (like force bars) compared to ImGui.

## Option 4: Native Win32 API / MFC
Directly using `CreateWindow` or Microsoft Foundation Classes.

*   **Pros**:
    *   **Zero Dependencies**: No external libraries needed.
    *   **Tiny Size**.
*   **Cons**:
    *   **Painful Development**: Creating layouts, handling resizing, and custom drawing graphs is extremely tedious.
    *   **Unmaintainable**: Boilerplate code explodes quickly.
*   **Suitability**: **Very Low**. Not recommended for rapid development.

## Recommendation: Dear ImGui

**Reasoning**:
*   The primary use case is **tuning parameters** (sliders) and **visualizing data** (telemetry graphs). ImGui excels at exactly this.
*   It introduces minimal build complexity (just add the .cpp files to the project).
*   It is the standard for sim racing tools (e.g., used in many overlays, SimHub plugins, etc.).
*   Integration with the existing `main.cpp` loop is straightforward:
    ```cpp
    // In Main Loop
    ImGui_ImplDX11_NewFrame();
    ImGui::Begin("LMUFFB Settings");
    ImGui::SliderFloat("Gain", &engine.m_gain, 0.0f, 2.0f);
    ImGui::End();
    ImGui::Render();
    ```

```

# File: docs\bug_reports\User bug report 001 Wheel Jerking with SOP Slider.md
```markdown
Based on the user report and the provided codebase, here is an analysis of the issue and the troubleshooting steps to assist the user.

### Analysis of the Issue

The user is reporting two distinct symptoms:
1.  **"Couldn't feel any effect":** The app is running, but the wheel isn't reacting to the app's commands.
2.  **"Jerking about randomly... if SOP slider > 0":** When a specific effect is enabled, the signal becomes unstable.

**Root Cause Diagnosis:**
The most probable cause is a **"Double FFB Conflict"**.

In the `README.md`, the architecture requires the game (Le Mans Ultimate) to send its signals to a **vJoy** (Virtual) device, while the **LMUFFB App** reads telemetry and sends the new FFB to the **Physical** wheel.

If the user has bound their **Moza R9 directly in the game settings** (instead of binding the vJoy device), two things happen:
1.  **The Game** is sending FFB commands to the Moza R9.
2.  **The App** is trying to send FFB commands to the Moza R9 (via the new DirectInput mode).

This results in a "Race Condition" at the driver level. The wheel receives a command from the Game, then milliseconds later a command from the App. This causes the "Jerking" (fighting signals) and the "No Effect" (the game overwriting the app's output).

The **SOP (Seat of Pants)** effect exacerbates this because it injects a constant lateral force during cornering. If the game is trying to center the wheel (aligning torque) and the App is trying to push it sideways (SOP), the rapid switching between these two contradictory commands causes violent oscillation.

---

### Troubleshooting Response for the User

Here is a draft you can send to the user.

***

**Subject:** Re: Moza R9 Setup / Jerking Issues

Hi, thanks for the report!

Based on your description ("No effect" initially, followed by "Jerking" when SOP is added), it sounds like you might be experiencing a **Double FFB Conflict**.

This happens when both **Le Mans Ultimate (The Game)** and **LMUFFB (The App)** are trying to control your steering wheel motor at the same time. They end up fighting each other, causing the wheel to go limp or jerk violently.

Please try this exact setup sequence to ensure the "Bridge" is working correctly:

**1. The "Dummy" Setup (In-Game)**
*   Ensure **vJoy** is installed.
*   In Le Mans Ultimate **Controls** menu, you must bind your **Steering Axis** to the **vJoy Device**, NOT your Moza R9.
    *   *Tip:* You may need to use the "vJoy Feeder" app (comes with vJoy) to move the virtual axis so the game detects it.
*   This ensures the game sends its Force Feedback to the "Dummy" (vJoy) and leaves your real wheel free for the App to control.

**2. The "Real" Setup (In App)**
*   Open LMUFFB.
*   In the **FFB Device** dropdown, select your **Moza R9**.
*   Now, the App will read the physics from the game and send the force to your Moza.

**3. Verify SOP Settings**
*   The **SOP (Seat of Pants)** effect adds Lateral G-force to the wheel. On a Direct Drive wheel like the R9, this can be very strong.
*   Once you have the vJoy binding sorted, start with **SOP Effect** at **0.0** and **Master Gain** at **0.5**.
*   Gradually increase SOP. If it still jerks on straights, let me know—it might indicate noise in the telemetry data which we can filter out in the next update.

**Summary Checklist:**
1.  [ ] Game Steering Axis = vJoy Device (Axis X)
2.  [ ] App FFB Device = Moza R9
3.  [ ] Game FFB Strength = 0 (or default, doesn't matter as long as it's sent to vJoy)

Let me know if that resolves the fighting sensation!

***

### Developer Notes (Internal)

If the user confirms they set it up correctly and the issue persists, there are two code-side areas to investigate in `FFBEngine.h` and `DirectInputFFB.cpp`:

1.  **SOP Noise Filtering (`FFBEngine.h`):**
    The current SOP implementation takes raw lateral acceleration:
    ```cpp
    double lat_g = data->mLocalAccel.x / 9.81;
    double sop_force = lat_g * m_sop_effect * 1000.0;
    ```
    If `mLocalAccel.x` is noisy (jittering around 0 on straights), this will translate directly to torque jitter.
    *Fix:* Implement a simple low-pass filter or a deadzone for `lat_g` in the next build.

2.  **DirectInput Update Safety (`DirectInputFFB.cpp`):**
    In `UpdateForce`, we check `if (magnitude == m_last_force) return;`.
    However, on high-torque DD wheels, rapid updates (400Hz) of the `ConstantForce` parameter without a `DIEP_START` flag is usually correct, but some drivers handle this poorly.
    *Verification:* If the user is still having issues, we might need to throttle the DirectInput update rate to 100Hz (while keeping physics at 400Hz) to see if the Moza driver prefers less frequent updates.
```

# File: docs\bug_reports\User bug report 002 VJoy Setup Troubleshooting and Bug.md
```markdown
Based on the user's report and the provided codebase, here is the analysis of the problem and the troubleshooting steps to assist the user.

### Analysis of the Issue

The user is facing two distinct problems:
1.  **"I have no steering in game now":** The user expects the app to pass their steering input to the game, but it is not happening.
2.  **"The wheel is now randomly and intermittently spinning about":** The wheel is moving on its own, likely due to a signal conflict.

#### Root Cause 1: LMUFFB is not an Input Bridge (Missing "Feeder")
The user states: *"It's as if LMUFFB isn't returning the steering angle to vJoy."*

Looking at `main.cpp` and `DirectInputFFB.cpp`, the application **does not read the steering axis** of the physical wheel. It only calculates Force Feedback.
*   **The Code:** The `FFBThread` calculates force and sends it to the wheel. It does *not* poll the wheel's position to update vJoy.
*   **The Docs:** `README.md` (Installation Step 4A) explicitly states: *"You MUST use a tool like **vJoy Feeder**, **Joystick Gremlin**, or **Mojo** to map your physical wheel axis to the vJoy axis. LMUFFB does not yet bridge steering input."*

**Conclusion:** The user stopped the "vJoy Feeder" app after testing it, or assumes LMUFFB replaces it. They need a separate app running in the background to actually steer the car.

#### Root Cause 2: The "Spinning" is caused by an Axis Conflict (Critical Bug/Config Trap)
The user states: *"The wheel is now randomly and intermittently spinning about... turning the wheel doesn't show anything in vJoy monitor."*

There is a conflict between the Code and the Documentation/User Setup:
1.  **The Code (`main.cpp` line 81):**
    ```cpp
    // Update vJoy Axis (for monitoring) if active
    if (vJoyActive) {
        long axis_val = (long)((force + 1.0) * 0.5 * (axis_max - axis_min) + axis_min);
        DynamicVJoy::Get().SetAxis(axis_val, VJOY_DEVICE_ID, 0x30); // 0x30 = HID_USAGE_X
    }
    ```
    The app writes the **Calculated Force** to **vJoy Axis X** at 400Hz. This is intended for debugging/monitoring FFB visually.

2.  **The User Setup:**
    The user has likely bound **Game Steering** to **vJoy Axis X** (as per the README instructions).

3.  **The Result:**
    *   The App calculates FFB Force.
    *   The App writes this Force to vJoy Axis X.
    *   The Game reads vJoy Axis X as **Steering Input**.
    *   **Feedback Loop:** The FFB Force is literally steering the car. If the FFB pushes left, the car steers left, which changes the physics, which changes the FFB. This causes the "random spinning" and "jerking."

---

### Troubleshooting Response for the User

Here is the suggested response to guide the user through fixing both issues.

***

**Subject:** Re: No steering and wheel spinning issues

Hi, thanks for the detailed follow-up. Moving the EXE to the vJoy folder was a good move to fix the DLL issue.

I have analyzed your report against the code, and I believe I know exactly what is happening. There are two separate things we need to fix.

### 1. Fix "No Steering" (The Bridge)
**The Issue:** LMUFFB calculates Force Feedback, but it **does not** read your steering angle. It cannot "bridge" your Moza steering to vJoy on its own.
**The Fix:** You must keep a "Feeder" application running in the background while you play.
1.  Open **vJoy Feeder** (or Joystick Gremlin).
2.  Select your Moza R9.
3.  Ensure that turning your Moza moves the vJoy Axis bars.
4.  **Leave this app open** while playing the game.

### 2. Fix "Wheel Spinning" (The Axis Conflict)
**The Issue:** This is a conflict between the App and your Game bindings.
Currently, LMUFFB outputs the **FFB Force** signal to **vJoy Axis X** (so you can see the force visually in vJoy Monitor).
However, if you bind the **Game Steering** to **vJoy Axis X**, the App is effectively "steering the car" with the Force Feedback signal. This creates a feedback loop where the wheel spins and jerks on its own.

**The Fix:** You need to use a different axis for steering to avoid the FFB signal.

**Please follow these exact steps:**

1.  **Configure vJoy:**
    *   Open **Configure vJoy**.
    *   Ensure **Device 1** has at least **X** and **Y** axes enabled.
2.  **Configure the Feeder (e.g., vJoy Feeder / Gremlin):**
    *   Map your **Moza Steering** to **vJoy Axis Y** (instead of X).
3.  **Configure the Game:**
    *   Go to LMU Controls.
    *   Bind **Steering** to **vJoy Axis Y**.
4.  **Run LMUFFB:**
    *   Now, LMUFFB will write FFB info to Axis X (which the game ignores).
    *   Your Feeder will write Steering to Axis Y (which the game uses).

**Summary Checklist:**
*   [ ] **Feeder App** is running in the background.
*   [ ] **Feeder** maps Moza Wheel -> vJoy **Axis Y**.
*   [ ] **Game** Steering is bound to vJoy **Axis Y**.
*   [ ] **LMUFFB** FFB Device is set to **Moza R9**.

Let me know if this stabilizes the wheel!

***

### Recommendation for Developers (Code Fix)

The current behavior in `main.cpp` is dangerous because it defaults to overwriting the most common steering axis (X) with FFB data.

**Recommended Changes for v0.3.9:**
1.  **Disable vJoy Output by Default:** In `Config.h` / `Config.cpp`, add a boolean `m_output_ffb_to_vjoy` defaulting to `false`.
2.  **Modify `main.cpp`:**
    ```cpp
    // Only write to vJoy if explicitly enabled for debugging
    if (vJoyActive && Config::m_output_ffb_to_vjoy) {
        long axis_val = ...;
        DynamicVJoy::Get().SetAxis(axis_val, VJOY_DEVICE_ID, 0x30);
    }
    ```
3.  **Update Documentation:** Clearly state that vJoy Axis X is used for FFB monitoring *only* if enabled, and users should use Axis X for steering (if the feature is disabled) or Axis Y (if enabled).



# Follow up question 1

An user replied to the suggested steps as following:

> Open vJoy Feeder (or Joystick Gremlin).
> Select your Moza R9.

Where in vJoy feeder do I select my Moza wheel?

[see image] docs\bug_reports\vJoy Feeder.jpeg

# Answer 1

That is my mistake, and I apologize for the confusion. Thank you for the screenshot—it clarifies exactly what is happening.

The application in your screenshot is the **vJoy Demo Feeder**. It is only designed to test if vJoy is working by moving those sliders with your mouse. **It cannot read your physical Moza wheel.**

To bridge your Moza wheel to vJoy, you need a specific mapping software. The most reliable (and free) tool for this is **Joystick Gremlin**.

Here is the corrected step-by-step guide to get your steering working without the "spinning" issue:

### 1. Download Joystick Gremlin
*   Download it here: [https://whitemagic.github.io/JoystickGremlin/](https://whitemagic.github.io/JoystickGremlin/)
*   Install and run it.

### 2. Map Your Wheel (The Bridge)
1.  In Joystick Gremlin, you will see tabs at the top for your connected devices. Click the tab for your **Moza R9**.
2.  Turn your physical wheel. You should see one of the axes in the list highlight/move (usually "X Axis" or "Rotation X").
3.  Click on that axis in the list.
4.  On the right side, under "Action", choose **"Map to vJoy"**.
5.  **Crucial Step (To fix the spinning bug):**
    *   Select vJoy Device: **1**
    *   Select Axis: **Y** (Do **NOT** select X, as LMUFFB is currently overwriting X for its display, causing the spinning).

### 3. Activate the Bridge
*   In the top menu of Joystick Gremlin, click the **"Activate"** button (the Gamepad icon turns green).
*   *Test:* Now, when you turn your physical Moza wheel, the **Y bar** in the vJoy Monitor (or the Feeder you screenshotted) should move.

### 4. Bind in Game
*   Go into Le Mans Ultimate.
*   Bind **Steering** to that new **vJoy Axis Y**.

Once you do this, the "Bridge" is complete:
**Moza** -> **Joystick Gremlin** -> **vJoy Axis Y** -> **Game**.

This leaves **vJoy Axis X** free for LMUFFB to use for its display without interfering with your steering.

# Follow up question 2

I added this alternative suggestion to an user:

"Try binding the wheel in game as the steering device, but set all FFB strength settings to 0 and disable all the FFB toggles in the in-game FFB configuration, to prevent receiving FFB from the game."

Do you think this is a feasible solution for "Fix "No Steering" (The Bridge)​" and "Fix "Wheel Spinning" (The Axis Conflict)​" ?

It seems strange that we need this bridge thing.. no other app like irFBB or Marvin's app needed Joystick Gremlin, as far as I am aware.


# Answer 2


This is a **very good suggestion**, and yes, it is absolutely a feasible solution. In fact, if it works on the user's specific system, it is the **preferred solution** because it eliminates the complexity of Joystick Gremlin entirely.

Here is the breakdown of why we suggested the "Bridge" method initially, and why your suggestion might work better.

### 1. Is your suggestion feasible?
**Yes.**
This is referred to as **"Method B"** in your `README.txt`.

*   **How it works:**
    1.  **Game Input:** The game reads the **Physical Wheel** directly for steering. (Solves "No Steering").
    2.  **Game Output:** You set Game FFB to **0 / None**. This stops the game from sending forces.
    3.  **App Output:** LMUFFB sends forces to the **Physical Wheel**.
    4.  **vJoy:** Not used at all. (Solves "Wheel Spinning" because the game isn't listening to the vJoy axis where LMUFFB writes data).

*   **The Risk (Why we hesitated):**
    The only risk is **"Device Locking" (Exclusive Mode)**.
    Some games, when you bind a wheel to them, tell Windows: *"I own this device now. No other app can touch it."*
    *   **If LMU does this:** LMUFFB will try to send FFB, get an "Access Denied" error, and the wheel will be dead.
    *   **If LMU is "polite" (Shared Mode):** It will read the steering, ignore the FFB, and let LMUFFB write the FFB. **This is the ideal scenario.**

**Recommendation:**
Tell the user to try this immediately. If LMUFFB shows "Device Acquired" in the console while the game is running, **this is the best way to play.**

---

### 2. Why did other apps (iRFFB, Marvin's) not need a bridge?

You are correct that needing a third-party "Feeder" is annoying. The difference lies in how those apps were architected compared to the current state of LMUFFB.

**A. iRFFB (Direct Mode)**
iRFFB has a "Direct" mode where it does exactly what you suggested:
*   The Game reads the steering.
*   The App sends the FFB.
*   *Note:* iRacing is very good at allowing "Shared" access to devices. rFactor 2 (LMU) is older technology and sometimes more aggressive about locking devices.

**B. Input Passthrough (Internal Bridge)**
Some advanced FFB apps act as the bridge internally.
1.  The App reads the physical wheel axis (using DirectInput).
2.  The App writes that value to vJoy.
3.  The Game reads vJoy.
*   **Current LMUFFB Status:** Your app **does not yet have the code** to read the physical wheel axis. It only calculates physics. That is why we currently need an external tool (Gremlin) to do that reading/writing.

### Summary for the User

You can send them this:

> "That is actually the best solution if it works on your system!
>
> The 'Bridge' (Joystick Gremlin) is a fallback method we use when the game refuses to share the wheel with the app.
>
> **Please try the Direct Method:**
> 1.  **Uninstall/Disable vJoy** (or just unbind it in-game).
> 2.  **In Game:** Bind your **Moza R9** directly to Steering.
> 3.  **In Game:** Set FFB Strength to **0%** (or 'None' if available).
> 4.  **In App:** Select **Moza R9**.
>
> If the wheel has force feedback and steers the car, you are good to go! You don't need vJoy or Gremlin.
>
> If the wheel has **NO force feedback** (is limp) while the app is running, it means the game has 'Locked' the device. In that case, you will need to go back to the Joystick Gremlin method."


```

# File: docs\dev_docs\directinput_implementation.md
```markdown
# DirectInput FFB Implementation Guide

This document outlines the technical steps required to transition LMUFFB from a vJoy-based "Axis Mapping" architecture to a native **DirectInput Force Feedback** architecture. This change allows the application to send specific "Constant Force" packets directly to the steering wheel driver, bypassing the need for a virtual joystick and allowing the application to coexist seamlessly with the game's input system.

## 1. Overview

**Priority: CRITICAL / REQUIRED**

Currently, LMUFFB acts as a virtual joystick (`vJoy`) and maps the calculated force to the **Axis Position**. This visualizes the force but does **not** drive the physical motors of a user's steering wheel.
To function as a true Force Feedback application (like iRFFB or Marvin's AIRA), LMUFFB **must** implement a DirectInput client that opens the physical wheel and sends `Constant Force` packets.

*Hypothetical Feature Note: Implementing DirectInput correctly is complex due to device enumeration, exclusive locking (cooperative levels), and handling lost devices. It effectively turns the app into a specialized driver client.*

## 2. Technical Requirements

*   **API**: DirectInput8 (via `dinput8.lib` / `dinput8.dll`).
*   **Language**: C++ (Native COM interfaces).
*   **Privileges**: Exclusive access to the FFB device is often required (`DISCL_EXCLUSIVE | DISCL_BACKGROUND`).

## 3. Implementation Steps

### Phase 1: Device Enumeration & Initialization
Instead of connecting to vJoy ID 1, we must scan connected hardware.

```cpp
IDirectInput8* g_pDI = NULL;
IDirectInputDevice8* g_pDevice = NULL;

// 1. Create DirectInput Object
DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pDI, NULL);

// 2. Enumerate Devices (Filter for Wheels/FFB)
g_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK);

// 3. Callback Logic
BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) {
    // Check if device supports FFB
    // Create Device
    g_pDI->CreateDevice(pdidInstance->guidInstance, &g_pDevice, NULL);
    return DIENUM_STOP; // Stop after finding first FFB wheel
}
```

### Phase 2: Setting Cooperative Level
This is critical. FFB usually requires Exclusive/Background access so forces continue when the app is minimized (running alongside the game).

```cpp
g_pDevice->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND);
g_pDevice->SetDataFormat(&c_dfDIJoystick);
g_pDevice->Acquire();
```

### Phase 3: Creating the Effect
We need a **Constant Force** effect.

```cpp
DIEFFECT diEffect;
DICONSTANTFORCE diConstantForce;
LPDIRECTINPUTEFFECT g_pEffect = NULL;

// Initialize parameters
diConstantForce.lMagnitude = 0;

diEffect.dwSize = sizeof(DIEFFECT);
diEffect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
diEffect.dwDuration = INFINITE;
diEffect.dwGain = DI_FFNOMINALMAX;
diEffect.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
diEffect.lpvTypeSpecificParams = &diConstantForce;
// ... (Set Axes/Directions) ...

// Create
g_pDevice->CreateEffect(GUID_ConstantForce, &diEffect, &g_pEffect, NULL);
g_pEffect->Start(1, 0);
```

### Phase 4: Updating the Force (The Loop)
Inside the `FFBThread` (400Hz loop), instead of calling `SetAxis` (vJoy), we update the effect.

```cpp
void UpdateDirectInputForce(double normalizedForce) {
    if (!g_pEffect) return;

    // Map -1.0..1.0 to -10000..10000
    LONG magnitude = (LONG)(normalizedForce * 10000.0);
    
    // Clamp
    if (magnitude > 10000) magnitude = 10000;
    if (magnitude < -10000) magnitude = -10000;

    DICONSTANTFORCE cf;
    cf.lMagnitude = magnitude;

    DIEFFECT eff;
    ZeroMemory(&eff, sizeof(eff));
    eff.dwSize = sizeof(DIEFFECT);
    eff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
    eff.lpvTypeSpecificParams = &cf;

    // Send to driver (Low latency call)
    g_pEffect->SetParameters(&eff, DIEP_TYPESPECIFICPARAMS | DIEP_START);
}
```

## 4. Challenges & Solutions

1.  **Device Selection**: Users may have multiple controllers (Handbrake, Shifter, Wheel). The GUI must allow selecting the specific FFB device from a list.
2.  **Spring/Damper Effects**: Some wheels default to a heavy centering spring. The app should explicitly create specific Spring/Damper effects with 0 magnitude to "clear" the driver's default behavior.
3.  **Loss of Focus**: Even with `DISCL_BACKGROUND`, some games (or drivers) steal exclusive access. The app must handle `DIERR_NOTACQUIRED` errors and attempt to `Acquire()` periodically.

## 5. Benefits
*   **Latency**: Bypasses the vJoy -> Driver bridge.
*   **Usability**: User does not need to configure vJoy. They just select their wheel in LMUFFB.
*   **Compatibility**: Works with games that don't support multiple controllers well (though LMU is generally good with this).

```

# File: docs\dev_docs\FFB_formulas.md
```markdown
Based on the `FFBEngine.h` file provided in the source code, here is the complete mathematical breakdown of the Force Feedback calculation.

The final output sent to the DirectInput driver is a normalized value between **-1.0** and **1.0**.

### 1. The Master Formula

$$ F_{final} = \text{Clamp}\left( \left( \frac{F_{total}}{4000} \times K_{gain} \right), -1.0, 1.0 \right) $$

Where **$F_{total}$** is the sum of all physics components:

$$ F_{total} = (F_{base} + F_{sop} + F_{vib\_lock} + F_{vib\_spin} + F_{vib\_slide} + F_{vib\_road} + F_{vib\_bottom}) \times M_{spin\_drop} $$

*(Note: $M_{spin\_drop}$ is a reduction multiplier active only during traction loss).*

---

### 2. Component Breakdown

#### A. Global Factors (Pre-calculated)
**Load Factor ($L_{factor}$)**: Scales texture effects based on how much weight is on the front tires.
$$ L_{factor} = \text{Clamp}\left( \frac{\text{Load}_{FL} + \text{Load}_{FR}}{2 \times 4000}, 0.0, 1.5 \right) $$

#### B. Base Force (Understeer / Grip Modulation)
This modulates the raw steering rack force from the game based on front tire grip.
$$ F_{base} = F_{steering\_arm} \times \left( 1.0 - \left( (1.0 - \text{Grip}_{avg}) \times K_{understeer} \right) \right) $$
*   $\text{Grip}_{avg}$: Average of Front Left and Front Right `mGripFract`.

#### C. Seat of Pants (SoP) & Oversteer
This injects lateral G-force and rear-axle aligning torque to simulate the car body's rotation.

1.  **Smoothed Lateral G ($G_{lat}$)**: Calculated via Low Pass Filter (Exponential Moving Average).
    $$ G_{smooth} = G_{prev} + \alpha \times \left( \frac{\text{AccelX}_{local}}{9.81} - G_{prev} \right) $$
    *   $\alpha$: User setting `m_sop_smoothing_factor`.

2.  **Base SoP**:
    $$ F_{sop\_base} = G_{smooth} \times K_{sop} \times 1000.0 $$

3.  **Oversteer Boost**:
    If Front Grip > Rear Grip:
    $$ F_{sop\_boosted} = F_{sop\_base} \times \left( 1.0 + (\text{Grip}_{delta} \times K_{oversteer} \times 2.0) \right) $$

4.  **Rear Aligning Torque**:
    $$ T_{rear} = \frac{\text{LatForce}_{RL} + \text{LatForce}_{RR}}{2} \times 0.05 \times K_{oversteer} $$

$$ F_{sop} = F_{sop\_boosted} + T_{rear} $$

#### D. Dynamic Textures (Vibrations)

**1. Progressive Lockup ($F_{vib\_lock}$)**
Active if Brake > 5% and Slip Ratio < -0.1.
*   **Frequency**: $10 + (|\text{Vel}_{car}| \times 1.5)$ Hz
*   **Amplitude**: $A = \text{Severity} \times K_{lockup} \times 800.0 \times L_{factor}$
*   **Force**: $A \times \sin(\text{phase})$

**2. Wheel Spin / Traction Loss ($F_{vib\_spin}$ & $M_{spin\_drop}$)**
Active if Throttle > 5% and Rear Slip Ratio > 0.2.
*   **Torque Drop Multiplier**: $M_{spin\_drop} = 1.0 - (\text{Severity} \times K_{spin} \times 0.6)$
*   **Frequency**: $10 + (\text{SlipSpeed} \times 2.5)$ Hz (Capped at 80Hz)
*   **Amplitude**: $A = \text{Severity} \times K_{spin} \times 500.0$
*   **Force**: $A \times \sin(\text{phase})$

**3. Slide Texture ($F_{vib\_slide}$)**
Active if Slip Angle > 0.15 rad (~8.5°).
*   **Frequency**: $30 + (\text{LateralGroundVel} \times 20.0)$ Hz
*   **Waveform**: Sawtooth
*   **Amplitude**: $A = K_{slide} \times 300.0 \times L_{factor}$
*   **Force**: $A \times \text{Sawtooth}(\text{phase})$

**4. Road Texture ($F_{vib\_road}$)**
High-pass filter on suspension movement.
*   $\Delta_{vert} = (\text{Deflection}_{current} - \text{Deflection}_{prev})$
*   **Force**: $(\Delta_{vert\_L} + \Delta_{vert\_R}) \times 5000.0 \times K_{road} \times L_{factor}$

**5. Suspension Bottoming ($F_{vib\_bottom}$)**
Active if Max Tire Load > 8000N.
*   **Magnitude**: $\sqrt{\text{Load}_{max} - 8000} \times K_{bottom} \times 0.5$
*   **Frequency**: Fixed 50Hz sine wave pulse.

---

### 3. Post-Processing (Min Force)

After calculating the normalized force ($F_{norm}$), the Min Force logic is applied to overcome wheel friction:

If $|F_{norm}| > 0.0001$ AND $|F_{norm}| < K_{min\_force}$:
$$ F_{final} = \text{sign}(F_{norm}) \times K_{min\_force} $$

---

### 4. Legend: Variables & Coefficients

**Telemetry Inputs (from Shared Memory):**
*   $F_{steering\_arm}$: `mSteeringArmForce`
*   $\text{Load}$: `mTireLoad` (N)
*   $\text{GripFract}$: `mGripFract` (0.0 to 1.0)
*   $\text{AccelX}_{local}$: `mLocalAccel.x` ($m/s^2$)
*   $\text{LatForce}$: `mLateralForce` (N)
*   $\text{Vel}_{car}$: `mLocalVel.z` (m/s)
*   $\text{LateralGroundVel}$: `mLateralGroundVel` (m/s)
*   $\text{Deflection}$: `mVerticalTireDeflection` (m)

**User Settings (Coefficients from GUI):**
*   $K_{gain}$: Master Gain (0.0 - 2.0)
*   $K_{understeer}$: Understeer Effect (0.0 - 1.0)
*   $K_{sop}$: SoP Effect (0.0 - 2.0)
*   $K_{oversteer}$: Oversteer Boost (0.0 - 1.0)
*   $K_{lockup}, K_{spin}, K_{slide}, K_{road}$: Texture Gains
*   $K_{min\_force}$: Min Force (0.0 - 0.20)

**Hardcoded Constants (Tuning):**
*   **4000.0**: Reference Max Force (Newtons) for normalization.
*   **4000.0**: Reference Tire Load (Newtons) for Load Factor.
*   **1000.0**: SoP Scaling factor.
*   **5000.0**: Road Texture stiffness.
*   **8000.0**: Bottoming threshold (Newtons).
```

# File: docs\dev_docs\investigation_vjoyless_implementation.md
```markdown
# Investigation: Removing vJoy Dependency (vJoy-less Architecture)

**Date:** 2025-05-23
**Status:** Investigation

## Goal
Determine if LMUFFB can function without requiring the user to install vJoy and bind it in-game, similar to how "Marvin's AIRA" or other FFB tools might operate.

## Current Architecture (vJoy Dependent)
1.  **Game Output:** Le Mans Ultimate (rF2 Engine) requires a DirectInput device to "drive". It sends physics forces to this device.
2.  **The Trick:** We bind the game to a **vJoy Device** (Virtual Joystick). The game thinks it's driving a wheel.
3.  **Telemetry:** LMUFFB reads telemetry via Shared Memory.
4.  **App Output:** LMUFFB calculates forces and sends them to the **Physical Wheel** via DirectInput.

## Alternative: "Marvin's Method" (Hypothesis)

Marvin's AIRA (for iRacing) and irFFB operate differently because iRacing has a specific "FFB API" or allows disabling FFB output while keeping input active.

### rFactor 2 / LMU Constraints
rFactor 2 is an older engine. It typically couples Input (Steering Axis) with Output (FFB).
- If you unbind FFB, you might lose Steering Input? No, you can usually set FFB Type to "None".
- **Problem:** If you bind your **Physical Wheel** as the Steering Axis in-game, the game *automatically* attempts to acquire it for FFB (Exclusive Mode).
- **Conflict:** LMUFFB also needs to acquire the Physical Wheel (Exclusive Mode) to send forces.
- **Result:** Race condition. "Double FFB".

### How to solve this without vJoy?

#### Method 1: The "None" FFB Hack
If LMU allows binding an axis *without* acquiring the device for FFB:
1.  Bind Physical Wheel Axis X to Steering in-game.
2.  Set "FFB Type" to **"None"** in game settings.
3.  LMUFFB acquires the wheel via DirectInput.
    *   **Risk:** If the game opens the device in "Exclusive" mode just to read the Axis, LMUFFB might be blocked from writing FFB (or vice versa). DirectInput usually allows "Background" "Non-Exclusive" reads, but FFB writing often requires "Exclusive".
    *   **Test Required:** Can LMUFFB send FFB to a device that LMU is actively reading steering from?
        - If yes: vJoy is NOT needed.
        - If no (Access Denied): We need vJoy.
    *   **LMU Specific Note:** Users report LMU lacks a "None" FFB option. The workaround is setting FFB strength to 0 or disabling effects, which *might* not release the exclusive lock.

#### Method 2: Proxy Driver (HID Guardian)
Some apps use a filter driver (like HidGuardian) to hide the physical wheel from the game, presenting a virtual one instead. This effectively *is* vJoy but hidden. Complex to install.

#### Method 3: UDP / API Injection
If LMU had an API to inject steering input (like Assetto Corsa), we wouldn't need to bind a physical axis. It does not.

## Conclusion & Next Steps
The dependency on vJoy exists primarily to **prevent the game from locking the physical wheel**.

**Experiment for v0.4.0:**
Try "Method 1":
1.  User binds real wheel in game.
2.  User disables FFB in game (Type: None).
3.  LMUFFB attempts to acquire wheel in `DISCL_BACKGROUND | DISCL_EXCLUSIVE` mode.
4.  If it works, vJoy is obsolete.
5.  If it fails (Device Busy), we need vJoy.

**Verdict:** For now, vJoy is the most reliable method to ensure signal separation. Marvin's app likely benefits from iRacing's more modern non-exclusive input handling.

```

# File: docs\dev_docs\investigation_vjoy_bundling.md
```markdown
# Investigation: vJoy Bundling and Licensing

**Date:** 2025-05-23
**Status:** Recommendations Ready

## 1. Technical Feasibility of Bundling `vJoyInterface.dll`

### Can we bundle it?
**Yes.**
`vJoyInterface.dll` is a standard Windows DLL. It serves as a bridge between the application and the vJoy Driver (kernel mode).
*   **Location:** If placed in the same directory as `LMUFFB.exe`, `LoadLibrary` (or static linking) will find it automatically.
*   **Architecture:** We must bundle the **x64 (amd64)** version of the DLL since LMUFFB is built as a 64-bit application.
*   **Dependency:** The DLL *requires* the vJoy Driver to be installed on the system to function. It cannot install the driver itself. If the driver is missing, `vJoyInterface.dll` might load, but initialization functions (like `AcquireVJD`) will fail or return "Driver Not Enabled".

### Conclusion
We **can and should** bundle `vJoyInterface.dll` with our release zip/installer. This eliminates the "DLL Not Found" error users face even if they have vJoy installed (sometimes the DLL isn't in PATH).

## 2. Licensing and Copyright

### vJoy License
vJoy (by Shaul Eizikovich) is open source.
*   **Source:** [GitHub - shauleiz/vJoy](https://github.com/shauleiz/vJoy)
*   **License:** **MIT License** (and some parts Public Domain / zlib in older versions, but generally permissive).
*   **SDK License:** The SDK (which includes the DLL) is intended for developers to distribute with their apps.

### Attribution Requirements
The MIT License requires including the copyright notice and permission notice in the software distribution.

### Recommendation
1.  **Bundle the DLL:** Include `vJoyInterface.dll` in the distribution folder.
2.  **Include License:** Add a file `licenses/vJoy_LICENSE.txt` containing the vJoy MIT license text.
3.  **Update Main License:** Mention in our `LICENSE` or `README` that the package contains third-party software (vJoy, Dear ImGui) and point to their licenses.

## 3. Version Checking Strategy

Since the DLL talks to the Driver, version mismatches can cause bugs.
*   **Target:** vJoy 2.1.9.1 (current standard).
*   **Check:** On startup, call `GetvJoyVersion()` (from the DLL) and `GetvJoyManufacturerString()`.
*   **Mismatch:** If the driver version differs significantly (e.g., < 2.1.8), warn the user. 2.1.8 and 2.1.9 are mostly compatible.

## Action Plan
1.  **Distribute:** Add `vJoyInterface.dll` to the repo (in `vendor/vJoy/lib/amd64`) or build script copy step.
2.  **Code:** Implement version check popup.
3.  **Docs:** Add vJoy License text.

```

# File: docs\dev_docs\Missing Mutex Lock (Race Condition).md
```markdown
Please address these 3 issues:

###  Critical Issues Detected

Please verify if this issue exists and in case fix it:

I found a significant bug in `DrawDebugWindow` regarding thread safety and data sampling.

#### A. Missing Mutex Lock (Race Condition)
In `DrawTuningWindow`, you correctly lock the mutex:
```cpp
std::lock_guard<std::mutex> lock(g_engine_mutex); // Correct
```
However, in `DrawDebugWindow`, **there is no lock**.
```cpp
void GuiLayer::DrawDebugWindow(FFBEngine& engine) {
    ImGui::Begin("FFB Analysis", &m_show_debug_window);
    
    // READING SHARED DATA WITHOUT LOCK
    plot_total.Add(engine.m_last_debug.total_output); 
    // ...
```
**The Risk:** The FFB Thread (400Hz) is writing to `engine.m_last_debug` at the same time the GUI Thread (60Hz) is reading it. This causes "Tearing". You might read the X acceleration from Frame 100 and the Steering Force from Frame 101. While usually subtle, it can cause weird spikes in the graphs.

**Fix:** Add the lock at the start of `DrawDebugWindow`:
```cpp
void GuiLayer::DrawDebugWindow(FFBEngine& engine) {
    std::lock_guard<std::mutex> lock(g_engine_mutex); // <--- ADD THIS
    ImGui::Begin("FFB Analysis", &m_show_debug_window);
    // ...
```

#### B. Sampling Aliasing (The "Binary" Look)
This explains why the user sees "Binary" or "Square Wave" data.
*   **Physics:** Updates 400 times per second.
*   **GUI:** Updates 60 times per second.
*   **The Problem:** The GUI simply grabs the *current* value when it renders. It **ignores** the 6 or 7 physics updates that happened in between frames.
    *   If the FFB is vibrating at 50Hz (a common frequency for "Crunch" or "Road Texture"), the 60Hz GUI will sample it at random points in the wave.
    *   Sometimes it catches the peak (+1.0), sometimes the trough (-1.0).
    *   Visually, this looks like the value is teleporting between +1 and -1 (Binary), rather than a smooth sine wave.

**Fix (Future):** To fix the graphs, you would need a thread-safe Queue where the FFB thread pushes *every* data point, and the GUI thread consumes them all. For now, just be aware that the graphs are "lossy snapshots."

#### C. Auto-Scaling Confusion
In `DrawDebugWindow`, you provide min/max values for some plots but not others, or the ranges might be too wide/narrow.
*   **Example:** `plot_input_steer` is set to -5000 to +5000. If the game outputs 0 (because FFB is off), it looks like a perfect flat line.
*   **Example:** `plot_clipping` is 0.0 to 1.1.
*   **Issue:** `ImGui::PlotLines` does not draw a "zero line" in the middle. If the data is 0.0, it draws a line at the bottom (or middle depending on range).
*   **Recommendation:** Ensure users know that a flat line in the *middle* is 0, but a flat line at the *bottom* might be -1 (or min range). Adding a tooltip or a value readout (e.g., `ImGui::Text("Value: %.2f", current_val)`) next to the graph helps immensely.
```

# File: docs\dev_docs\plan_guided_configurator.md
```markdown
# Plan: Guided Configurator (In-App)

**Date:** 2025-05-23
**Status:** Proposal

## Goal
Add a "Wizard" button inside the main LMUFFB GUI to help users fix configuration issues (Double FFB, wrong device) without restarting the app or reading external docs.

## Wizard Workflow

### Step 1: Device Check
*   **Prompt:** "Please rotate your physical steering wheel."
*   **Action:** App scans DirectInput devices for axis movement.
*   **Result:** Auto-selects the moving device in the "FFB Device" dropdown.
    *   *Check:* If the moving device is "vJoy", warn the user: "You seem to be rotating a virtual device? Please rotate your REAL wheel."

### Step 2: vJoy / Game Binding Check
*   **Prompt:** "We need to ensure the game is sending FFB to vJoy, not your wheel."
*   **Action:**
    1.  User is asked to wiggle vJoy Axis (via button "Test vJoy" which calls `SetAxis`).
    2.  User confirms if Game detects it.
    *   *Complex Part:* Checking if Game is WRONGLY bound to Real Wheel. We can't read Game Config directly easily.
    *   *Alternative:* Ask user: "Does your wheel resist turning when in the game menu?" (If yes, Game has FFB lock).

### Step 3: Input Feeder Warning
*   **Crucial Step:** Explain the "Steering Bridge".
*   **Text:** "If you bound Game Steering to vJoy, you must use a 'Feeder' app (like vJoy Feeder, Joystick Gremlin, or Mojo) to map your Real Wheel to vJoy. LMUFFB does **not** currently bridge steering input."
*   *Future:* If LMUFFB implements bridging (reading real axis, writing vJoy axis), enable it here.

## Implementation Priorities
1.  **Device Auto-Detect:** High priority. Reduces "I picked the wrong device" support tickets.
2.  **vJoy Test Button:** Easy to implement. Helps user bind game.

```

# File: docs\dev_docs\plan_guided_installer.md
```markdown
# Plan: Fully Guided Installer

**Date:** 2025-05-23
**Status:** Proposal

## Objective
Replace the raw executable distribution with a "Wizard" that handles prerequisites and configuration, reducing user error ("Double FFB", "vJoy missing").

## Installer Steps (Inno Setup / Custom Launcher)

### Phase 1: Prerequisite Check (Pre-Install)
1.  **Detect vJoy:** Check Registry or `C:\Program Files\vJoy`.
    *   *If missing:* Prompt to download/install (button to launch vJoy installer).
    *   *If present:* Check version (2.1.9.1 recommended).
2.  **Detect Shared Memory Plugin:** Check `[GamePath]/Plugins/rFactor2SharedMemoryMapPlugin64.dll`.
    *   Try to autodetect LMU game path (eg. from registry or common game paths). If it fails, offer to browse for Game Path. In any case, display the detected path in the installer, so that the user can confirm it.
    *   *If missing:* Offer to copy the DLL (bundled with installer) to the game folder. (User must browse for Game Path).
3.  **Visual C++ Redistributable:** Check and install if missing.

### Phase 2: Configuration Wizard (First Run)
When `LMUFFB.exe` is run for the first time (no config.ini):

**Step 1: vJoy Configuration**
*   Launch `vJoyConf.exe` (if possible) or show specific instructions: "Please enable 1 Device with X Axis."
*   Verify vJoy is active (try to acquire it).

**Step 2: Game Binding Helper**
*   *Innovative Idea:* Use a "vJoy Feeder" loop.
    *   "Open LMU Controls Menu. Click 'Detect' on Steering Axis."
    *   User clicks "Wiggle vJoy" button in Wizard.
    *   Wizard oscillates vJoy Axis X. Game detects it.
    *   User binds it.
    *   Instruction: "Now set In-Game FFB to 'None' or 'Smoothing 0'." [TODO: update all references to  In-Game FFB to 'None' or 'Smoothing 0' to reflect what LMU actually allows; replace "Game" with "LMU" in all references]

**Step 3: Wheel Selection**
*   Dropdown list of devices.
*   "Rotate your wheel". Bar moves.
*   "Select this device".

### Phase 3: Launch
*   Tell the user everything should be ready and LMU can be launched.

## Technical Requirements
1.  **Bundled Resources:**
    *   `vJoyInterface.dll` (x64/x86)
    *   `rFactor2SharedMemoryMapPlugin64.dll`
2.  **Logic:**
    *   Registry reading (Game path, vJoy path).
    *   Feeder logic (already in `DynamicVJoy.h`).

## Roadmap
*   **v0.4.0:** Implement Phase 1 (Installer checks).
*   **v0.5.0:** Implement Phase 2 (First Run Wizard).

```

# File: docs\dev_docs\plan_troubleshooting_FFB_visualizations.md
```markdown
# Plan: FFB & Telemetry Troubleshooting Visualizations

**Date:** 2025-05-23
**Status:** Proposal

## Goal
Implement comprehensive real-time visualizations and logging to assist users and developers in diagnosing FFB issues (jerking, dead signals, clipping) and telemetry gaps.

## 1. GUI Visualizations (Real-time)

### A. FFB Component Graph ("The Stack")
A real-time stacked area chart or multi-line graph showing the contribution of each effect to the total output.
*   **X-Axis:** Time (last 5-10 seconds sliding window).
*   **Y-Axis:** Force (-1.0 to 1.0).
*   **Lines/Layers (Individual Traces):**
    *   `Base Force` (Game Physics: `mSteeringArmForce` scaled)
    *   `SoP Force` (Smoothed Lateral G)
    *   `Understeer Drop` (Reduction factor)
    *   `Oversteer Boost` (Rear Aligning Torque)
    *   `Road Texture` (Vertical Deflection impulse)
    *   `Slide Texture` (Scrubbing vibration)
    *   `Lockup Vibration` (Brake slip impulse)
    *   `Wheel Spin Vibration` (Throttle slip impulse)
    *   `Bottoming Impulse` (High load limiter)
    *   `Total Output` (Thick white line)
    *   `Clipping` (Red indicator when Total > 1.0)

*Visual Style:* All graphs should be **Rolling Trace Plots** (Oscilloscope style), showing the last 5-10 seconds of data. This helps spot patterns like oscillation or spikes.

**UI Controls:**
*   Checkbox: "Show Debug Graphs" (to save CPU when not needed).
*   Toggles: Enable/Disable individual trace lines to isolate visual noise.

### B. Telemetry Inspector
A set of **Rolling Trace Plots** for each raw input value from Shared Memory. 
*   **Critical Values (Traced):**
    *   `mSteeringArmForce` (Is the game sending physics?)
    *   `mLocalAccel.x` (Is SoP working? Is it noisy?)
    *   `mTireLoad` (Are we airborne? Is bottoming logic valid?)
    *   `mGripFract` (Is it stuck at 0 or 1? Indicates tire model issues).
    *   `mSlipRatio` (Brake/Accel slip)
    *   `mSlipAngle` (Cornering slip)
    *   `mLateralPatchVel` (Slide speed)
    *   `mVerticalTireDeflection` (Road surface)
*   **Indicators:**
    *   Green dot: Value updating.
    *   Red dot: Value static/zero (Dead signal warning).

## 2. Troubleshooting Session Mode (Logger)

A dedicated "Calibration / Diagnostic" mode.

### Workflow
1.  User clicks "Start Diagnostic Session".
2.  App asks user to perform specific maneuvers:
    *   "Center Wheel and Drive Straight (5s)" -> Checks noise floor.
    *   "Turn 90 deg Left and Hold" -> Checks polarity.
    *   "Drive over a curb" -> Checks texture impulse.
3.  **Logging:** App records CSV file (`diagnostic_log_timestamp.csv`) at 400Hz.
    *   Columns: Time, Inputs (Steering, Pedals), Physics (G, Vel, Load), FFB Components, Output.
4.  **Analysis:**
    *   App generates a summary report:
        *   "Max Noise on Straight: 0.05 (Good)"
        *   "SoP Jitter: High (Suggest Smoothing)"
        *   "Telemetry Gaps: Tire Temps missing."

## 3. Implementation Plan

### Phase 1: Basic Graphs (v0.4.0)
*   Integrate `ImGui::PlotLines` or `ImPlot` (extension).
*   Visualize Total Output vs. Game Input.

### Phase 2: Detailed Inspection (v0.4.1)
*   Add Telemetry Value table with "Stuck Value" detection.

### Phase 3: Diagnostic Logger (v0.5.0)
*   Implement CSV writer and Wizard UI.

## 4. Implementation Status (v0.3.13)

**Phase 1 & 2 Completed.**
The GUI now includes a "Show Troubleshooting Graphs" toggle which opens an "FFB Analysis" window containing:
1.  **FFB Components Stack:** Real-time rolling plots for all 11 signal components (Base, SoP, Understeer, Oversteer, Road, Slide, Lockup, Spin, Bottoming, Clipping).
2.  **Telemetry Inspector:** Real-time rolling plots for 8 critical telemetry inputs (Steering Force, Lat G, Load, Grip, Slip Ratio/Angle, Patch Vel, Deflection).

**Technical Implementation:**
- `FFBEngine` populates a debug struct `m_last_debug` and `m_last_telemetry` every frame.
- `GuiLayer` maintains static `RollingBuffer` vectors (1000 samples) for each channel.
- `ImGui::PlotLines` renders the data at 60Hz.

```

# File: docs\dev_docs\PROJECT_CONTEXT.md
```markdown
# Project Context: LMUFFB (Le Mans Ultimate Force Feedback)

## 1. Goal
The objective of this project is to build a high-performance **Force Feedback (FFB) Application** for the racing simulator *Le Mans Ultimate (LMU)*. 
This tool is inspired by **iRFFB** (for iRacing) and **Marvin's AIRA**. It aims to solve the problem of "numb" or "generic" force feedback by calculating synthetic forces derived directly from physics telemetry (tire grip, suspension load, lateral Gs) rather than relying solely on the game's steering rack output.

## 2. Architecture Overview
The project has evolved from a Python prototype to a **native C++ application** to ensure ultra-low latency (critical for FFB).

### Pipeline
1.  **Telemetry Source**: The app reads telemetry from the **rFactor 2 Engine** (which powers LMU) via a memory-mapped file created by the *rFactor 2 Shared Memory Map Plugin*.
2.  **Processing (The Engine)**: A high-priority thread (400Hz) calculates forces based on:
    *   **Grip Modulation**: Reduces force as front grip is lost (Understeer).
    *   **SoP (Seat of Pants)**: Adds lateral G-forces to simulate chassis yaw (Oversteer).
    *   **Texture Synthesis**: Injects high-frequency vibrations for sliding and road bumps.
3.  **Output**: The app sends the calculated force signal to a **vJoy** (Virtual Joystick) device. The user binds the game controls to this virtual device, allowing the app to control the physical wheel via feedback loops or bridge software.

## 3. Current State
*   **Version**: C++ Port (Main).
*   **GUI**: Implemented using **Dear ImGui** (DX11). Allows real-time tuning of Gain, Smoothing, SoP, etc.
*   **Persistence**: Saves settings to `config.ini`.
*   **Installer**: Inno Setup script provided.
*   **Status**: Alpha. Functional loop, but vJoy dependency makes setup complex for users.

## 4. Key Resources
*   **iRFFB**: [GitHub Repository](https://github.com/nlp80/irFFB) - The primary inspiration.
*   **Marvin's AIRA**: [GitHub Repository](https://github.com/mherbold/MarvinsAIRA) - Inspiration for "Texture" and "Detail" effects.
*   **rF2 Shared Memory Plugin**: [TheIronWolfModding](https://github.com/TheIronWolfModding/rF2SharedMemoryMapPlugin) - The interface used to get data.
*   **vJoy**: [jshafer817 Fork](https://github.com/jshafer817/vJoy) - The required driver for Windows 10/11.

## 5. Technology Stack
*   **Language**: C++ (C++17 standard).
*   **Build System**: CMake (3.10+).
*   **GUI Library**: Dear ImGui (MIT License).
*   **OS**: Windows 10 / 11.
*   **Dependencies**: vJoy SDK (`vJoyInterface.lib`).

## 6. Roadmap / Next Steps
The immediate focus for future sessions is improving the **Output Stage**.
1.  **DirectInput Support**: Move away from vJoy. Implement `IDirectInputDevice8` to send "Constant Force" packets directly to the user's physical wheel. This drastically simplifies setup (no virtual driver needed).
2.  **Telemetry Analysis**: Implement more advanced filtering (High-Pass / Low-Pass) to isolate specific suspension frequencies for better "Road Texture".
3.  **Wheel Profiles**: Save/Load settings per car or per wheel base.

```

# File: docs\dev_docs\proposed_changes_to_disable_vJoy.md
```markdown
So should we add an option in the GUI to "disable" vJoy (and / or to release it) so that it leaves vJoy free for Gremlin?
If so, give me the code for that.


Yes, this is the correct engineering solution. Instead of asking users to hack DLL files, we should add a checkbox to **Enable/Disable vJoy Output**.

When disabled, the app will call `RelinquishVJD`, immediately freeing the device so Joystick Gremlin can grab it without restarting LMUFFB.

Here are the code changes required.

### 1. Update `src/Config.h`
Add a new boolean setting.

```cpp
// src/Config.h

class Config {
public:
    // ... existing methods ...

    // Global App Settings
    static bool m_ignore_vjoy_version_warning;
    
    // NEW: Toggle for vJoy output
    static bool m_enable_vjoy_output; 
};
```

### 2. Update `src/Config.cpp`
Initialize, Save, and Load the new setting. **I recommend defaulting it to `false`** to prevent this conflict for new users, but you can set it to `true` if you want to preserve legacy behavior.

```cpp
// src/Config.cpp

// Initialize (Default to FALSE to prevent Gremlin conflicts)
bool Config::m_enable_vjoy_output = false; 

void Config::Save(const FFBEngine& engine, const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        // ... existing saves ...
        file << "enable_vjoy_output=" << m_enable_vjoy_output << "\n"; // NEW
        file.close();
        // ...
    }
}

void Config::Load(FFBEngine& engine, const std::string& filename) {
    // ... existing load logic ...
    while (std::getline(file, line)) {
        // ... inside parsing loop ...
        if (key == "ignore_vjoy_version_warning") m_ignore_vjoy_version_warning = std::stoi(value);
        else if (key == "enable_vjoy_output") m_enable_vjoy_output = std::stoi(value); // NEW
        // ... existing keys ...
    }
}
```

### 3. Update `src/GuiLayer.cpp`
Add the checkbox to the GUI.

```cpp
// src/GuiLayer.cpp inside DrawTuningWindow

    ImGui::Separator();
    ImGui::Text("Output");
    
    // NEW: Checkbox to toggle vJoy
    // Using the Config variable directly
    if (ImGui::Checkbox("Output FFB to vJoy (Debug)", &Config::m_enable_vjoy_output)) {
        // Optional: You could trigger an immediate save here if desired
    }
    
    // Existing visualization placeholder
    ImGui::Text("Clipping Visualization Placeholder");
```

### 4. Update `main.cpp` (The Logic)
Refactor the `FFBThread` to dynamically Acquire/Relinquish the device based on the config.

```cpp
// main.cpp

void FFBThread() {
    long axis_min = 1;
    long axis_max = 32768;
    
    // 1. Load the DLL (We still load the library, but we don't acquire yet)
    bool vJoyDllLoaded = false;
    if (DynamicVJoy::Get().Load()) {
        vJoyDllLoaded = true;
        // ... [Keep existing version check logic here] ...
    }

    // Track acquisition state locally
    bool vJoyAcquired = false;

    std::cout << "[FFB] Loop Started." << std::endl;

    while (g_running) {
        if (g_ffb_active && g_pTelemetry) {
            double force = 0.0;
            {
                std::lock_guard<std::mutex> lock(g_engine_mutex);
                force = g_engine.calculate_force(g_pTelemetry);
            }

            // --- NEW vJoy LOGIC ---
            if (vJoyDllLoaded) {
                // Case A: User wants vJoy, but we don't have it -> ACQUIRE
                if (Config::m_enable_vjoy_output && !vJoyAcquired) {
                    VjdStat status = DynamicVJoy::Get().GetStatus(VJOY_DEVICE_ID);
                    if ((status == VJD_STAT_OWN) || ((status == VJD_STAT_FREE) && DynamicVJoy::Get().Acquire(VJOY_DEVICE_ID))) {
                        vJoyAcquired = true;
                        std::cout << "[vJoy] Device " << VJOY_DEVICE_ID << " acquired for debug output." << std::endl;
                    }
                }
                // Case B: User disabled vJoy, but we still have it -> RELEASE
                else if (!Config::m_enable_vjoy_output && vJoyAcquired) {
                    DynamicVJoy::Get().Relinquish(VJOY_DEVICE_ID);
                    vJoyAcquired = false;
                    std::cout << "[vJoy] Device " << VJOY_DEVICE_ID << " relinquished." << std::endl;
                }

                // Case C: We have it, update axis
                if (vJoyAcquired) {
                    long axis_val = (long)((force + 1.0) * 0.5 * (axis_max - axis_min) + axis_min);
                    DynamicVJoy::Get().SetAxis(axis_val, VJOY_DEVICE_ID, 0x30); 
                }
            }
            // ----------------------
            
            // Update DirectInput (Physical Wheel)
            DirectInputFFB::Get().UpdateForce(force);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    // Cleanup on exit
    if (vJoyAcquired) {
        DynamicVJoy::Get().Relinquish(VJOY_DEVICE_ID);
    }
    std::cout << "[FFB] Loop Stopped." << std::endl;
}
```

### How this solves the user's problem:
1.  The user starts LMUFFB.
2.  They uncheck **"Output FFB to vJoy"**.
3.  The app immediately calls `Relinquish()`.
4.  Joystick Gremlin (running in background) can now instantly grab the device without errors.
5.  The user drives happily.


## Follow up:
Here is the correct, complete implementation for main.cpp that handles the releasing.


1.  **The Problem:** Joystick Gremlin fails because LMUFFB has **Acquired** (locked) the vJoy device.
3.  **The Required Fix:** We need logic that actually calls `RelinquishVJD(ID)` when the checkbox is unticked. This frees the device so Gremlin can grab it.

You need the **Dynamic Acquire/Release Logic** (State Machine) inside the loop.

Here is the correct, complete implementation for `main.cpp` that handles the releasing:

```cpp
// main.cpp : FFBThread

void FFBThread() {
    long axis_min = 1;
    long axis_max = 32768;
    
    // 1. Load the DLL (We load the library, but we DO NOT acquire the device yet)
    bool vJoyDllLoaded = false;
    if (DynamicVJoy::Get().Load()) {
        vJoyDllLoaded = true;
        // ... [Keep existing version check logic here] ...
    }

    // Track acquisition state locally
    // We start FALSE. We only acquire if the config says so.
    bool vJoyAcquired = false; 

    std::cout << "[FFB] Loop Started." << std::endl;

    while (g_running) {
        if (g_ffb_active && g_pTelemetry) {
            double force = 0.0;
            {
                std::lock_guard<std::mutex> lock(g_engine_mutex);
                force = g_engine.calculate_force(g_pTelemetry);
            }

            // --- DYNAMIC vJoy LOGIC ---
            if (vJoyDllLoaded) {
                // STATE 1: User wants vJoy, but we don't have it yet -> TRY TO ACQUIRE
                if (Config::m_enable_vjoy_output && !vJoyAcquired) {
                    VjdStat status = DynamicVJoy::Get().GetStatus(VJOY_DEVICE_ID);
                    if ((status == VJD_STAT_OWN) || ((status == VJD_STAT_FREE) && DynamicVJoy::Get().Acquire(VJOY_DEVICE_ID))) {
                        vJoyAcquired = true;
                        std::cout << "[vJoy] Device " << VJOY_DEVICE_ID << " acquired for debug output." << std::endl;
                    }
                }
                // STATE 2: User disabled vJoy, but we are holding it -> RELEASE IT
                else if (!Config::m_enable_vjoy_output && vJoyAcquired) {
                    DynamicVJoy::Get().Relinquish(VJOY_DEVICE_ID);
                    vJoyAcquired = false;
                    std::cout << "[vJoy] Device " << VJOY_DEVICE_ID << " relinquished (freed for Gremlin)." << std::endl;
                }

                // STATE 3: If we own it, update the axis
                if (vJoyAcquired) {
                    long axis_val = (long)((force + 1.0) * 0.5 * (axis_max - axis_min) + axis_min);
                    DynamicVJoy::Get().SetAxis(axis_val, VJOY_DEVICE_ID, 0x30); 
                }
            }
            // --------------------------
            
            // Update DirectInput (Physical Wheel)
            DirectInputFFB::Get().UpdateForce(force);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    // Cleanup on exit
    if (vJoyAcquired) {
        DynamicVJoy::Get().Relinquish(VJOY_DEVICE_ID);
    }
    std::cout << "[FFB] Loop Stopped." << std::endl;
}
```

### Key Changes from Original:
1.  Removed `bool vJoyActive` initialization at the top. We now use `vJoyDllLoaded` to know if the DLL exists, and `vJoyAcquired` to track if we hold the device.
2.  Moved the `Acquire` logic **inside the loop**. This allows the user to toggle the checkbox in the GUI, and the app will instantly grab or release the device without restarting.
```

# File: docs\dev_docs\report_on_ffb_improvements.md
```markdown
Some of the FFB effects (described in docs/ffb_effects.md ) are based on forces taken from the car physics telemetry, which I think is the ideal scenario for telemetry and physics based FFB effects. However, other effects are currently based on "vibration" effects, that although are scaled gradually with what it is happening on the car, have vibration "frequencies" that I think might not be actually linked to physic forces. This is the case of these effects: 
* Braking Lockup (Progressive Scrub) effect: it has a frequency that "Transitions from High Pitch (60Hz) at the limit to Low Pitch (10Hz) at full lock", and an Amplitude that "Scales linearly with severity".
* Wheel Spin (Traction Loss) effect: it has a vibration component with a frequency that scales with wheel speed difference (Slip Ratio), giving a "revving up" sensation through the rim.
* Slide Texture: Adds "scrubbing" vibration when any tire is sliding laterally (high Slip Angle).
* Road Texture: Adds "bumps" based on suspension velocity changes (High-Pass Filter).

The problem is the following: is it possible for some or all of these effects, to be have "frequencies" and / or "amplitudes" that change dynamically with the car physics (data from telemetry)? For instance, if we have "scalar" values, can we use them as coefficients (or other types of multipliers or factors) of other components of the FFB signals? These components could be either telemetry values we already use, or that we could use in the feature (eg. data on forces, speed, acceleration, movements, etc.).


# Response

Yes, it is absolutely possible—and highly recommended—to link these "vibration" effects to physics telemetry. This moves your FFB from "canned effects" (pre-baked sine waves) to "procedural physics-based audio/haptics."

To do this correctly, you need to solve one specific math problem: **Phase Integration**.

### The Math Problem: Dynamic Frequency
Currently, your code uses: `sin(time * frequency)`.
If `frequency` changes dynamically (e.g., based on RPM or Speed), the result of `time * frequency` will jump discontinuously between frames, causing "clicks" and "pops" in the FFB motor.

**The Solution:**
You must track a `phase` variable and increment it per frame:
`phase += frequency * delta_time`.
Then calculate `sin(phase)`.

---

### Proposed Physics Implementations

Here is how you can rewrite the effects in `FFBEngine.h` to use the telemetry data found in `rF2Data.h`.

#### 1. Wheel Spin (Traction Loss) -> Linked to Engine RPM
Instead of a generic "revving" vibration, link the frequency to the **Engine RPM**. When the wheels spin, the drivetrain vibration (engine harmonics) often transmits through the steering column.

*   **Telemetry**: `data->mEngineRPM`
*   **Logic**: Map RPM to Hz (RPM / 60).

#### 2. Braking Lockup -> Linked to Wheel Speed / Car Speed
A tire scrubbing on asphalt creates a vibration frequency proportional to the speed the rubber is skipping over the road surface.
*   **Telemetry**: `data->mLocalVel.z` (Car Speed) or `wheel.mRotation`.
*   **Logic**: High speed = High frequency screech. Low speed = Low frequency judder.

#### 3. Slide Texture -> Linked to Tire Load
A tire sliding with 5000N of load on it vibrates much more violently than a tire sliding with 500N of load.
*   **Telemetry**: `wheel.mTireLoad`.
*   **Logic**: Use `mTireLoad` as a multiplier for the Amplitude.

---

### Code Implementation

Modify your `FFBEngine.h` to include state variables for phase tracking, and update the `calculate_force` method.

```cpp
// FFBEngine.h

class FFBEngine {
public:
    // ... existing settings ...

    // Internal state for Phase Integration (Required for dynamic frequencies)
    double m_lockup_phase = 0.0;
    double m_spin_phase = 0.0;
    double m_slide_phase = 0.0;

    double calculate_force(const rF2Telemetry* data) {
        if (!data) return 0.0;

        // ... [Existing Grip/SoP Logic] ... 
        
        double dt = data->mDeltaTime; // Time passed since last frame
        
        // --- 2b. Physics-Based Progressive Lockup ---
        if (m_lockup_enabled && data->mUnfilteredBrake > 0.05) {
            double slip_fl = data->mWheels[0].mSlipRatio;
            double slip_fr = data->mWheels[1].mSlipRatio;
            double max_slip = (std::min)(slip_fl, slip_fr); // Negative for braking
            
            if (max_slip < -0.1) {
                double severity = (std::abs(max_slip) - 0.1) / 0.4;
                severity = (std::min)(1.0, severity);

                // DYNAMIC FREQUENCY: Linked to Car Speed
                // As the car slows down, the "scrubbing" pitch drops.
                // Speed is in m/s. 
                // Example: 300kmh (83m/s) -> ~80Hz. 50kmh (13m/s) -> ~20Hz.
                double car_speed = std::abs(data->mLocalVel.z); // Longitudinal Velocity
                double freq = 10.0 + (car_speed * 0.8); 

                // Phase Integration
                m_lockup_phase += freq * dt * 6.28; // 2*PI
                if (m_lockup_phase > 6.28) m_lockup_phase -= 6.28;

                // DYNAMIC AMPLITUDE: Linked to Tire Load
                // A heavily loaded tire vibrates harder when locking.
                double load = (data->mWheels[0].mTireLoad + data->mWheels[1].mTireLoad) / 2.0;
                double load_factor = load / 3000.0; // Normalize (approx 3000N reference)
                load_factor = (std::min)(2.0, load_factor); // Cap it

                double amp = severity * m_lockup_gain * 800.0 * load_factor;
                
                // Use the integrated phase
                double rumble = std::sin(m_lockup_phase) * amp;
                total_force += rumble;
            }
        }

        // --- 2c. Physics-Based Wheel Spin ---
        if (m_spin_enabled && data->mUnfilteredThrottle > 0.05) {
            double slip_rl = data->mWheels[2].mSlipRatio;
            double slip_rr = data->mWheels[3].mSlipRatio;
            double max_slip = (std::max)(slip_rl, slip_rr);
            
            if (max_slip > 0.2) {
                double severity = (max_slip - 0.2) / 0.5;
                severity = (std::min)(1.0, severity);
                
                // Torque Drop (Floating feel)
                total_force *= (1.0 - (severity * m_spin_gain * 0.5));

                // DYNAMIC FREQUENCY: Linked to Engine RPM
                // Feel the engine hitting the limiter or revving up through the spin
                double rpm = data->mEngineRPM;
                // 4-stroke engine vibration is usually RPM / 60 * 2 (2nd order harmonic)
                double freq = (rpm / 60.0) * 2.0; 
                
                // Clamp freq to avoid ultra-low rumble at idle
                if (freq < 20.0) freq = 20.0;

                // Phase Integration
                m_spin_phase += freq * dt * 6.28;
                if (m_spin_phase > 6.28) m_spin_phase -= 6.28;

                double amp = severity * m_spin_gain * 600.0;
                double rumble = std::sin(m_spin_phase) * amp;
                
                total_force += rumble;
            }
        }

        // --- 3. Physics-Based Slide Texture ---
        if (m_slide_texture_enabled) {
            double avg_slip = (std::abs(fl.mSlipAngle) + std::abs(fr.mSlipAngle)) / 2.0;
            
            if (avg_slip > 0.15) { // 0.15 rad is roughly 8 degrees
                // DYNAMIC FREQUENCY: Scrubbing speed
                // How fast is the tire moving sideways?
                // Lateral Ground Velocity is perfect for this.
                double lat_vel = (std::abs(fl.mLateralGroundVel) + std::abs(fr.mLateralGroundVel)) / 2.0;
                
                // Scrubbing noise is usually high freq. 
                // Map 1 m/s -> 50Hz, 10 m/s -> 200Hz
                double freq = 30.0 + (lat_vel * 20.0);

                m_slide_phase += freq * dt * 6.28;
                if (m_slide_phase > 6.28) m_slide_phase -= 6.28;

                // Use a Sawtooth wave for "stick-slip" texture (more aggressive than sine)
                // Sawtooth: (phase / 2PI) * 2 - 1
                double sawtooth = (m_slide_phase / 6.28) * 2.0 - 1.0;

                // DYNAMIC AMPLITUDE: Tire Load
                double load = (fl.mTireLoad + fr.mTireLoad) / 2.0;
                double load_factor = load / 4000.0; // Heavier load = more vibration

                double noise = sawtooth * m_slide_texture_gain * 300.0 * load_factor;
                total_force += noise;
            }
        }
        
        // ... [Rest of code] ...
    }
};
```

### Summary of Changes

1.  **Phase Integration**: Added `m_phase` variables and `phase += freq * dt` logic. This allows frequency to change smoothly without audio glitches.
2.  **Lockup**: Frequency is now driven by `mLocalVel.z` (Car Speed). Amplitude is driven by `mTireLoad`.
3.  **Wheel Spin**: Frequency is now driven by `mEngineRPM`. This creates a very immersive connection between the engine sound and the wheel rim vibration.
4.  **Slide**: Waveform changed to **Sawtooth** (better for scrubbing feel), frequency driven by `mLateralGroundVel`, and amplitude driven by `mTireLoad`.

This approach utilizes the `rF2Telemetry` data you already have access to, making the FFB feel "alive" rather than "scripted."


# Follow up questions


Does the "Math Problem: Dynamic Frequency" (causing "clicks" and "pops" in the FFB motor) also affects the current implementation and effects? Should mitigations be adopted? If so, what are standard solutions to this?

I don't like the proposal to link Wheel Spin (Traction Loss) to Engine RPM. I am concerned about the use of engine RPM (especially for lower end wheels, like belt or gear based wheel like Logitec G29 and Thrustmaster T300, rather than direct drive wheels), because I think it might be "noisy" as always present even when we are not spinning (because the engine is always on and vibrating), and this could mask other FFB effects. Could we map this to forces not related to the engine, but only the tires and suspensions?

### 1. The "Math Problem" (Phase Discontinuity)

**Yes, the current implementation is definitely affected by this problem.**

In your current code:
```cpp
double rumble = std::sin(data->mElapsedTime * freq * 6.28) * amp;
```
`mElapsedTime` is the time since the game started. It grows indefinitely (e.g., 1000.0 seconds).

**The Issue:**
If `freq` changes from `60Hz` to `61Hz` between two frames (a 1.6% change), the value inside the sine function changes from `1000 * 60 = 60,000` to `1000 * 61 = 61,000`.
That is a jump of 1,000 full cycles in 0.002 seconds. The sine wave will instantly "teleport" to a random point in its cycle.

**The Symptom:**
On a Direct Drive wheel, this feels like "sand" or "static" in the signal. On a Gear/Belt wheel (G29/T300), it feels like a mechanical "knock" or "click" every time the grip changes slightly. It makes the FFB feel "digital" rather than organic.

**The Standard Solution: Phase Accumulation**
You must stop using absolute time (`mElapsedTime`) inside the sine wave. Instead, you must store a `phase` variable that remembers where the wave left off in the previous frame.

**Implementation Pattern:**
```cpp
// 1. Calculate how much the wave advances this specific frame
double phase_step = frequency_hz * delta_time_seconds * 2.0 * PI;

// 2. Add to stored phase
m_stored_phase += phase_step;

// 3. Wrap around to keep numbers small (optional but good practice)
if (m_stored_phase > 2.0 * PI) m_stored_phase -= 2.0 * PI;

// 4. Calculate Sine
double output = std::sin(m_stored_phase) * amplitude;
```

---

### 2. Alternative Wheel Spin (Traction Loss) Implementation

You are absolutely right to be concerned about Engine RPM on gear/belt wheels. Constant vibration creates "white noise" that masks important details (like curb texture or understeer).

**The Solution: Tire Slip Speed (Differential Velocity)**

Instead of the engine, we can link the frequency to the **physical speed difference** between the tire and the road. This is the "scrubbing speed."

*   **Physics:**
    *   **Low Slip Speed (e.g., 5 km/h difference):** The tire is fighting for grip. This creates a low-frequency "shudder" or "judder" (stick-slip phenomenon).
    *   **High Slip Speed (e.g., 50 km/h difference):** The tire is spinning freely. The rubber is skimming over the surface. This creates a higher frequency, smoother "hum" or "whine."
*   **Benefit:** If the car is moving but not spinning (0 slip), the frequency and amplitude are 0. It is **silent** unless traction is actually lost.

**Telemetry Calculation:**
We can derive Slip Speed using: `Car Speed * Slip Ratio`.

### Updated Code Implementation

Here is the corrected `FFBEngine.h` logic. This fixes the "Clicking" math error **and** implements the Tire-only Wheel Spin logic.

```cpp
// FFBEngine.h

class FFBEngine {
public:
    // ... existing settings ...

    // --- STATE VARIABLES FOR PHASE ACCUMULATION ---
    // These must persist between frames to solve the "Clicking" issue
    double m_lockup_phase = 0.0;
    double m_spin_phase = 0.0;
    double m_slide_phase = 0.0;

    double calculate_force(const rF2Telemetry* data) {
        if (!data) return 0.0;

        // [Standard Grip/SoP Logic here...]
        
        double dt = data->mDeltaTime; // Time passed since last frame (e.g., 0.0025s)
        const double TWO_PI = 6.28318530718;

        // --- 2b. Progressive Lockup (Corrected Math) ---
        if (m_lockup_enabled && data->mUnfilteredBrake > 0.05) {
            double slip_fl = data->mWheels[0].mSlipRatio;
            double slip_fr = data->mWheels[1].mSlipRatio;
            double max_slip = (std::min)(slip_fl, slip_fr); // Negative for braking
            
            if (max_slip < -0.1) {
                double severity = (std::abs(max_slip) - 0.1) / 0.4;
                severity = (std::min)(1.0, severity);

                // Frequency: Linked to Car Speed (Slower car = Lower pitch grinding)
                double car_speed_ms = std::abs(data->mLocalVel.z); 
                double freq = 10.0 + (car_speed_ms * 1.5); // 10Hz at stop, ~50Hz at 100kph

                // PHASE ACCUMULATION (Fixes Clicks)
                m_lockup_phase += freq * dt * TWO_PI;
                if (m_lockup_phase > TWO_PI) m_lockup_phase -= TWO_PI;

                double amp = severity * m_lockup_gain * 800.0;
                double rumble = std::sin(m_lockup_phase) * amp;
                total_force += rumble;
            }
        }

        // --- 2c. Wheel Spin / Traction Loss (Tire Physics Based) ---
        if (m_spin_enabled && data->mUnfilteredThrottle > 0.05) {
            double slip_rl = data->mWheels[2].mSlipRatio;
            double slip_rr = data->mWheels[3].mSlipRatio;
            double max_slip = (std::max)(slip_rl, slip_rr);
            
            // Threshold: 0.2 (20% slip)
            if (max_slip > 0.2) {
                double severity = (max_slip - 0.2) / 0.5;
                severity = (std::min)(1.0, severity);
                
                // 1. Torque Drop (The "Floating" sensation)
                // This is crucial for G29/T300 to feel "loose" without noise
                total_force *= (1.0 - (severity * m_spin_gain * 0.6)); 

                // 2. Vibration Frequency: Based on SLIP SPEED (Not RPM)
                // Calculate how fast the tire surface is moving relative to the road.
                // Slip Speed (m/s) approx = Car Speed (m/s) * Slip Ratio
                double car_speed_ms = std::abs(data->mLocalVel.z);
                double slip_speed_ms = car_speed_ms * max_slip;

                // Mapping:
                // 2 m/s (~7kph) slip -> 15Hz (Judder/Grip fighting)
                // 20 m/s (~72kph) slip -> 60Hz (Smooth spin)
                double freq = 10.0 + (slip_speed_ms * 2.5);
                
                // Cap frequency to prevent ultrasonic feeling on high speed burnouts
                if (freq > 80.0) freq = 80.0;

                // PHASE ACCUMULATION (Fixes Clicks)
                m_spin_phase += freq * dt * TWO_PI;
                if (m_spin_phase > TWO_PI) m_spin_phase -= TWO_PI;

                // Amplitude
                double amp = severity * m_spin_gain * 500.0;
                double rumble = std::sin(m_spin_phase) * amp;
                
                total_force += rumble;
            }
        }

        // ... [Rest of code] ...
    }
};
```

### Why this is better for G29 / T300:

1.  **No "Always On" Noise:** Unlike RPM, `Slip Speed` is zero when you have grip. The effect is purely reactive to the loss of traction.
2.  **Information vs. Noise:**
    *   **Low Frequency Judder (10-20Hz):** Tells the user "You are *just* breaking traction, ease off throttle." This is distinct and informative.
    *   **High Frequency Hum (50Hz+):** Tells the user "You are completely spinning the tires."
3.  **Smooth Transitions:** The Phase Accumulation ensures that as you accelerate and the slip speed increases, the vibration pitch ramps up smoothly (like a "sheuuu" sound) rather than clicking or popping.


# Code review of current implementation

This is a very solid implementation. You have successfully integrated the Phase Accumulation logic and the Physics-based frequency mapping.

Regarding your specific question about the **Simplified Load Factor**:

### Is the simplified Load Factor a good idea?
**Yes, but with one safety warning.**
Using a simple linear division (`load / 4000.0`) is computationally efficient and cleaner to read. However, in FFB, **unclamped physics values are dangerous**.

If the car hits a massive compression (e.g., the bottom of Eau Rouge) or a high curb, `mTireLoad` can spike to 10,000N+.
*   `10000 / 4000 = 2.5x` gain.
*   If your user already has the gain set high, this 2.5x spike will cause immediate **hard clipping** (signal flatlining at 100%) or a violent jolt that could rattle a gear-driven wheel damagingly.

**Recommendation:** Keep the simple calculation, but add a `std::min` cap (e.g., 1.5 or 2.0) just to protect the hardware and signal integrity.

---

### Code Review & Refinements

I have reviewed your `FFBEngine.h`. It is 95% perfect. I have made three small adjustments below to finalize it:

1.  **Global Load Calculation:** You calculated `load_factor` inside the *Slide* effect, but you forgot to apply it to the *Lockup* and *Road* effects (which benefit greatly from it). I moved the calculation to the top so all effects can use it.
2.  **Safety Clamp:** Added the `std::min` cap discussed above.
3.  **Road Texture Fix:** Applied the load factor to the Road Texture (bumps feel harder when suspension is compressed).

Here is the polished file:

```cpp
#ifndef FFBENGINE_H
#define FFBENGINE_H

#define NOMINMAX

#include <cmath>
#include <algorithm>
#include "rF2Data.h"

// FFB Engine Class
class FFBEngine {
public:
    // Settings (GUI Sliders)
    float m_gain = 1.0f;          
    float m_smoothing = 0.5f;     
    float m_understeer_effect = 1.0f; 
    float m_sop_effect = 0.5f;    
    float m_min_force = 0.0f;     
    
    // New Effects (v0.2)
    float m_oversteer_boost = 0.0f; 
    
    bool m_lockup_enabled = false;
    float m_lockup_gain = 0.5f;
    
    bool m_spin_enabled = false;
    float m_spin_gain = 0.5f;

    // Texture toggles
    bool m_slide_texture_enabled = true;
    float m_slide_texture_gain = 0.5f; 
    
    bool m_road_texture_enabled = false;
    float m_road_texture_gain = 0.5f; 

    // Internal state
    double m_prev_vert_deflection[2] = {0.0, 0.0}; // FL, FR
    
    // Phase Accumulators for Dynamic Oscillators
    double m_lockup_phase = 0.0;
    double m_spin_phase = 0.0;
    double m_slide_phase = 0.0;

    double calculate_force(const rF2Telemetry* data) {
        if (!data) return 0.0;
        
        double dt = data->mDeltaTime;
        const double TWO_PI = 6.28318530718;

        // Front Left and Front Right
        const rF2Wheel& fl = data->mWheels[0];
        const rF2Wheel& fr = data->mWheels[1];

        double game_force = data->mSteeringArmForce;

        // --- PRE-CALCULATION: TIRE LOAD FACTOR ---
        // Calculate this once to use across multiple effects.
        // Heavier load = stronger vibration transfer.
        double avg_load = (fl.mTireLoad + fr.mTireLoad) / 2.0;
        
        // Normalize: 4000N is a reference "loaded" GT tire.
        double load_factor = avg_load / 4000.0;
        
        // SAFETY CLAMP: Cap at 1.5x to prevent violent jolts during high-compression
        // or hard clipping when the user already has high gain.
        load_factor = (std::min)(1.5, (std::max)(0.0, load_factor));

        // --- 1. Understeer Effect (Grip Modulation) ---
        double grip_l = fl.mGripFract;
        double grip_r = fr.mGripFract;
        double avg_grip = (grip_l + grip_r) / 2.0;
        avg_grip = (std::max)(0.0, (std::min)(1.0, avg_grip));
        
        double grip_factor = 1.0 - ((1.0 - avg_grip) * m_understeer_effect);
        double output_force = game_force * grip_factor;

        // --- 2. Seat of Pants (SoP) / Oversteer ---
        double lat_g = data->mLocalAccel.x / 9.81;
        double sop_force = lat_g * m_sop_effect * 1000.0; 
        
        double grip_rl = data->mWheels[2].mGripFract;
        double grip_rr = data->mWheels[3].mGripFract;
        double avg_rear_grip = (grip_rl + grip_rr) / 2.0;
        
        double grip_delta = avg_grip - avg_rear_grip;
        if (grip_delta > 0.0) {
            sop_force *= (1.0 + (grip_delta * m_oversteer_boost * 2.0));
        }
        
        // Rear Aligning Torque Integration
        double rear_lat_force = (data->mWheels[2].mLateralForce + data->mWheels[3].mLateralForce) / 2.0;
        double rear_torque = rear_lat_force * 0.05 * m_oversteer_boost; 
        sop_force += rear_torque;

        double total_force = output_force + sop_force;
        
        // --- 2b. Progressive Lockup (Dynamic) ---
        if (m_lockup_enabled && data->mUnfilteredBrake > 0.05) {
            double slip_fl = data->mWheels[0].mSlipRatio;
            double slip_fr = data->mWheels[1].mSlipRatio;
            double max_slip = (std::min)(slip_fl, slip_fr); 
            
            if (max_slip < -0.1) {
                double severity = (std::abs(max_slip) - 0.1) / 0.4; 
                severity = (std::min)(1.0, severity);
                
                // Frequency: Car Speed
                double car_speed_ms = std::abs(data->mLocalVel.z); 
                double freq = 10.0 + (car_speed_ms * 1.5); 

                m_lockup_phase += freq * dt * TWO_PI;
                if (m_lockup_phase > TWO_PI) m_lockup_phase -= TWO_PI;

                // Amplitude: Severity * Gain * LOAD FACTOR
                // A locked tire with no weight on it (skipping) vibrates less than a loaded one.
                double amp = severity * m_lockup_gain * 800.0 * load_factor;
                
                double rumble = std::sin(m_lockup_phase) * amp;
                total_force += rumble;
            }
        }

        // --- 2c. Wheel Spin (Tire Physics Based) ---
        if (m_spin_enabled && data->mUnfilteredThrottle > 0.05) {
            double slip_rl = data->mWheels[2].mSlipRatio;
            double slip_rr = data->mWheels[3].mSlipRatio;
            double max_slip = (std::max)(slip_rl, slip_rr);
            
            if (max_slip > 0.2) {
                double severity = (max_slip - 0.2) / 0.5;
                severity = (std::min)(1.0, severity);
                
                // Torque Drop
                total_force *= (1.0 - (severity * m_spin_gain * 0.6)); 

                // Frequency: Slip Speed
                double car_speed_ms = std::abs(data->mLocalVel.z);
                double slip_speed_ms = car_speed_ms * max_slip;

                double freq = 10.0 + (slip_speed_ms * 2.5);
                if (freq > 80.0) freq = 80.0;

                m_spin_phase += freq * dt * TWO_PI;
                if (m_spin_phase > TWO_PI) m_spin_phase -= TWO_PI;

                double amp = severity * m_spin_gain * 500.0;
                double rumble = std::sin(m_spin_phase) * amp;
                
                total_force += rumble;
            }
        }

        // --- 3. Slide Texture (Dynamic Sawtooth) ---
        if (m_slide_texture_enabled) {
            double avg_slip = (std::abs(fl.mSlipAngle) + std::abs(fr.mSlipAngle)) / 2.0;
            
            if (avg_slip > 0.15) { 
                // Frequency: Lateral Velocity
                double lat_vel = (std::abs(fl.mLateralGroundVel) + std::abs(fr.mLateralGroundVel)) / 2.0;
                double freq = 30.0 + (lat_vel * 20.0);

                m_slide_phase += freq * dt * TWO_PI;
                if (m_slide_phase > TWO_PI) m_slide_phase -= TWO_PI;

                double sawtooth = (m_slide_phase / TWO_PI) * 2.0 - 1.0;

                // Amplitude: Gain * LOAD FACTOR
                double noise = sawtooth * m_slide_texture_gain * 300.0 * load_factor;
                total_force += noise;
            }
        }
        
        // --- 4. Road Texture (High Pass Filter) ---
        if (m_road_texture_enabled) {
            double vert_l = fl.mVerticalTireDeflection;
            double vert_r = fr.mVerticalTireDeflection;
            
            double delta_l = vert_l - m_prev_vert_deflection[0];
            double delta_r = vert_r - m_prev_vert_deflection[1];
            
            m_prev_vert_deflection[0] = vert_l;
            m_prev_vert_deflection[1] = vert_r;
            
            // Amplify sudden changes
            double road_noise = (delta_l + delta_r) * 5000.0 * m_road_texture_gain; 
            
            // Apply LOAD FACTOR: Bumps feel harder under compression
            road_noise *= load_factor;

            total_force += road_noise;
        }

        // --- 5. Min Force & Output ---
        double max_force_ref = 4000.0; 
        double norm_force = total_force / max_force_ref;
        
        norm_force *= m_gain;
        
        if (std::abs(norm_force) > 0.0001 && std::abs(norm_force) < m_min_force) {
            double sign = (norm_force > 0.0) ? 1.0 : -1.0;
            norm_force = sign * m_min_force;
        }

        return (std::max)(-1.0, (std::min)(1.0, norm_force));
    }
};

#endif // FFBENGINE_H
```

# Additional improvements

Based on the telemetry documentation you provided ( docs\telemetry_data_reference.md ) , there are **three specific data points** that are currently underutilized but would significantly improve the "physics" feel of your FFB.

Here is the analysis of what to use and how to implement it.

### 1. `mLateralPatchVel` (Lateral Patch Velocity)
**Use for:** Improving **Slide Texture**.

*   **Current Logic:** You are using `mSlipAngle`.
*   **The Problem:** Slip Angle is just an angle (geometry). It doesn't tell you how *fast* the rubber is scrubbing across the asphalt. A high slip angle at 5 km/h produces a low-frequency "groan," while the same angle at 150 km/h produces a high-frequency "screech."
*   **The Solution:** `mLateralPatchVel` is the actual speed (m/s) of the contact patch sliding sideways. This is the **direct physical source** of tire scrubbing vibration frequency.

**Implementation:**
```cpp
// Inside calculate_force()
// Map 1.0 m/s -> 40Hz, 10.0 m/s -> 200Hz
double slide_speed = std::abs(wheel.mLateralPatchVel);
double freq = 30.0 + (slide_speed * 17.0); 
// Use Phase Integration with this freq
```

### 2. `mTireLoad` (Vertical Tire Load)
**Use for:** Modulating **All Texture Effects** (Road, Slide, Lockup).

*   **Current Logic:** Effects have a fixed amplitude based on gain sliders.
*   **The Problem:** If a car crests a hill and the tires get light (unloaded), the FFB shouldn't be vibrating violently. Conversely, in the compression of Eau Rouge, the road texture should feel heavier.
*   **The Solution:** Use `mTireLoad` as a scalar multiplier.
    *   **Load Sensitivity:** `Amplitude = BaseGain * (mTireLoad / ReferenceLoad)`.
    *   *Note:* A standard GT3 car tire load is roughly 3000N-4000N static.

**Implementation:**
```cpp
// Normalize load (e.g., 4000N is 1.0)
double load_factor = wheel.mTireLoad / 4000.0;
// Clamp to avoid infinite force on huge compressions
load_factor = (std::min)(2.0, (std::max)(0.0, load_factor));

// Apply to effects
double road_noise = calculated_road_noise * load_factor;
double slide_noise = calculated_slide_noise * load_factor;
```

### 3. `mSuspensionDeflection`
**Use for:** New **"Bottoming Out"** Effect.

*   **The Concept:** When the suspension runs out of travel (hits the bump stops), the chassis receives a harsh vertical jolt. This is a critical cue for setup (ride height is too low) or driving (hitting a kerb too hard).
*   **Logic:** Monitor if deflection approaches the limit (you might need to log max values to find the limit for specific cars, or assume a generic range like 0.15m).

**Implementation:**
```cpp
// Simple derivative-based impact
// If deflection changes rapidly near the limit, add a pulse.
// Or simpler: If deflection > 0.95 * MaxTravel, add a "Thud" force.
```

---

### 4. `mTerrainName` (Surface FX)
**Use for:** **Kerb/Rumble Strip Enhancement**.

*   **The Concept:** rFactor 2 physics are great, but sometimes flat kerbs don't transmit enough detail through the steering rack alone.
*   **Logic:** Check the string `mTerrainName`.
    *   If it contains "RUMBLE", "KERB", or "STONE", boost the **Road Texture** gain.
    *   If it contains "GRASS" or "GRAVEL", inject a lower-frequency "wobble" to simulate uneven terrain.

---

### Updated Code Proposal

Here is how I would integrate `mLateralPatchVel` and `mTireLoad` into your existing engine to make the effects dynamic.

```cpp
// FFBEngine.h

class FFBEngine {
public:
    // ... state variables ...
    double m_slide_phase = 0.0;

    double calculate_force(const rF2Telemetry* data) {
        // ... [Setup] ...
        double dt = data->mDeltaTime;
        const double TWO_PI = 6.28318530718;

        // Calculate Average Front Tire Load for scaling
        double fl_load = data->mWheels[0].mTireLoad;
        double fr_load = data->mWheels[1].mTireLoad;
        double avg_load = (fl_load + fr_load) / 2.0;
        
        // Normalize Load: 4000N is a "standard" loaded GT tire. 
        // If load is 0 (airborne), factor is 0.
        double load_factor = avg_load / 4000.0;
        load_factor = (std::min)(2.0, (std::max)(0.0, load_factor));

        // --- IMPROVED SLIDE TEXTURE ---
        if (m_slide_texture_enabled) {
            // 1. Trigger: Still use Slip Angle or GripFract to decide IF we are sliding
            double avg_slip_angle = (std::abs(data->mWheels[0].mSlipAngle) + std::abs(data->mWheels[1].mSlipAngle)) / 2.0;
            
            if (avg_slip_angle > 0.15) { // ~8 degrees
                
                // 2. Frequency: Driven by Lateral Patch Velocity (The speed of the scrub)
                double fl_slide_vel = std::abs(data->mWheels[0].mLateralPatchVel);
                double fr_slide_vel = std::abs(data->mWheels[1].mLateralPatchVel);
                double avg_slide_vel = (fl_slide_vel + fr_slide_vel) / 2.0;

                // Map: 1 m/s -> 40Hz (Low groan), 15 m/s -> 200Hz (High screech)
                double freq = 40.0 + (avg_slide_vel * 10.0);
                if (freq > 250.0) freq = 250.0;

                // 3. Phase Integration (No clicks)
                m_slide_phase += freq * dt * TWO_PI;
                if (m_slide_phase > TWO_PI) m_slide_phase -= TWO_PI;

                // 4. Waveform: Sawtooth is best for "Scrubbing" texture
                double sawtooth = (m_slide_phase / TWO_PI) * 2.0 - 1.0;

                // 5. Amplitude: Modulated by TIRE LOAD
                // If the tire is sliding but has no weight on it, it shouldn't shake the wheel.
                double amp = m_slide_texture_gain * 400.0 * load_factor;

                total_force += sawtooth * amp;
            }
        }

        // --- IMPROVED ROAD TEXTURE ---
        if (m_road_texture_enabled) {
            // ... [Existing High Pass Filter Logic] ...
            
            // Apply Load Factor here too!
            // Bumps feel harder when the car is under compression.
            road_noise *= load_factor; 
            
            total_force += road_noise;
        }

        // ... [Rest of code] ...
    }
};
```

### Summary regarding your concerns:

1.  **Engine RPM:** I agree with you. Do not use it for traction loss. However, you *could* add a separate checkbox "Engine Idle Vibration" that simply adds a tiny sine wave at `RPM/60` Hz when the car is stationary, just for immersion. But keep it separate from physics.
2.  **Telemetry Usage:** Switching from `SlipAngle` to `LateralPatchVel` for the *frequency* of the slide effect is the single biggest physics upgrade you can make to that specific effect. It aligns the audio/tactile feedback with the actual velocity of the rubber.


```

# File: docs\dev_docs\roadmap.md
```markdown
# Roadmap & Future Development

To evolve LMUFFB from a prototype to a daily-driver application, the following steps are recommended:

## Completed Features (C++ Port)
*   [x] **Native C++ Port**: Migrated from Python to C++ for performance.
*   [x] **FFB Engine**: Implemented Grip Modulation, SoP, Min Force.
*   [x] **Texture Effects**: Implemented Slide Texture (noise) and Road Texture (suspension delta).
*   [x] **Architecture**: Threaded design (FFB 400Hz / Main 60Hz).
*   [x] **Testing**: Comprehensive C++ Unit Test suite.

## Short Term
*   [x] **GUI Implementation**: Added support for **Dear ImGui**.
    *   Logic for Sliders and Toggles implemented in `src/GuiLayer.cpp`.
    *   Developer instructions in `vendor/imgui/README.txt`.
*   [x] **Installer Support**: Added Inno Setup script (`installer/lmuffb.iss`) handling vJoy checks and Plugin installation.
*   [ ] **Config Persistence**: Save/Load user settings to an `.ini` or `.json` file.

## Medium Term
*   **DirectInput FFB Support**: Documentation guide created (`docs/dev_docs/directinput_implementation.md`). Implementation pending.

## Long Term (Performance)
*   **Wheel-Specific Modes**: Add specific protocols for popular bases (Fanatec, Simucube, Logitech) to display data on wheel screens (RPM LEDs) using the telemetry data.

```

# File: docs\dev_docs\telemetry_availability_report.md
```markdown
# Le Mans Ultimate Telemetry Availability Report

**Date:** 2025-05-23
**Version:** 1.0
**Target:** LMUFFB v0.3.2

## Executive Summary
Recent investigations into the Le Mans Ultimate (LMU) community forums indicate that while the game uses the rFactor 2 Shared Memory Plugin (DAMPlugin), certain telemetry data points are inconsistent, hidden, or intermittently broken across updates. 

While most complaints focus on Dashboard data (ERS, Temperatures, Fuel), there is a risk that physics-related fields used by LMUFFB could be affected, particularly those related to tire state (Grip, Load).

## Critical Telemetry Dependencies
LMUFFB relies on the following fields from `rF2Telemetry`. If these are zero or static, specific effects will fail.

| Field | Effect | Risk Level | Notes |
| :--- | :--- | :--- | :--- |
| `mSteeringArmForce` | **Master FFB** | Low | Core game FFB. If missing, no force at all. |
| `mTireLoad` | Slide Texture, Bottoming | Medium | Used for amplitude scaling. If 0, effects are silent. |
| `mVerticalTireDeflection` | Road Texture | Medium | Used for bump detection. |
| `mSlipAngle` | Slide Texture | Low | Essential for physics; unlikely to be hidden. |
| `mSlipRatio` | Lockup, Spin | Low | Essential for physics. |
| `mLateralPatchVel` | Slide Texture (Freq) | Medium | Advanced physics field; new in v0.3.2. |
| `mLocalAccel` | SoP (Lateral G) | Low | Required for Motion Rigs; likely present. |
| `mLocalVel` | Freq Scaling | Low | Basic vector. |
| `mGripFract` | Understeer (Grip Loss) | **High** | Derived from Tire Temp/Wear/Surface. **Temps reported broken.** |
| `mLateralForce` | Oversteer Boost | Low | Core physics. |

## Findings from Community Research
1.  **Dashboard Data Issues:** Users report missing ERS State (SOC), TC Level, ABS Level, and Motor Maps. This confirms LMU does not expose the full rFactor 2 telemetry set.
2.  **Tire Data Instability:** Reports from Feb 2024 indicate `mTemperature` and `mPressure` were broken in a specific build, then partially fixed.
    *   **Impact:** If Tire Temperature is not simulated or exposed, `mGripFract` (which usually depends on temp) might be calculated incorrectly or return a static value.
3.  **Plugin Compatibility:** The standard rF2 DAMPlugin works but causes "poor performance & pit menu flicker" for some users. 
    *   **Mitigation:** LMUFFB only *reads* the memory mapped file; it does not inject a plugin DLL into the game process (unlike the official DAMPlugin which *writes* the memory). **However**, the user *must* have the DAMPlugin installed and running for the memory map to exist. If the plugin causes instability, users might disable it, breaking LMUFFB.

## Recommendations for LMUFFB

### 1. Robustness Checks (Immediate)
We should implement "Sanity Checks" in `FFBEngine`:
*   If `mTireLoad` is consistently 0.0, fallback to a static load factor (1.0).
*   If `mGripFract` is consistently 0.0 or 1.0 (despite cornering), warn the user or disable Understeer effect to prevent sudden cutouts.

### 2. Telemetry Inspector (Future)
Implement the "Telemetry Logging" or a "Debug View" in the GUI (Planned v0.4.0) to allow users to verify if their install is outputting valid data.
*   *Action:* Prioritize the "Input Graphs" feature in the GUI.

### 3. Fallback Logic
If `mLateralPatchVel` (newly used) proves unreliable, we should fallback to `mLateralGroundVel` (which is standard kinematic data).

## Conclusion
The core physics vectors (Velocity, Acceleration, Force) are likely safe as they are essential for motion platforms which are supported. The highest risk is with **Tire State** data (`mGripFract`, `mTireLoad`) which might be simplified or hidden in LMU compared to rFactor 2.

We will proceed with the current v0.3.2 release but advise users to report "Dead FFB" issues which may indicate missing telemetry.

```

# File: docs\dev_docs\telemetry_data_reference.md
```markdown
# Telemetry Data Reference

This document lists the physics data available from the **rFactor 2 Shared Memory Map Plugin** (struct `rF2Telemetry` and `rF2Wheel`). It documents which values LMUFFB currently uses and explores potential future uses for enhanced Force Feedback.

## 1. Global Vehicle Telemetry (`rF2Telemetry`)

These values describe the state of the vehicle chassis and engine.

| Variable | Description | Current Usage | Future Potential |
| :--- | :--- | :--- | :--- |
| `mTime`, `mDeltaTime` | Simulation time/step. | **Used**: Driving oscillators (sine waves) for effects. | Timestamping for logging. |
| `mElapsedTime` | Session time. | **Used**: Phase for texture generation. | |
| `mSteeringArmForce` | Torque on the steering rack (Game Physics). | **Used**: Primary FFB source. | |
| `mLocalAccel` | Acceleration in car-local space (X=Lat, Y=Vert, Z=Long). | **Used**: `x` for SoP (Seat of Pants) effect. | `z` for braking dive/acceleration squat cues. |
| `mLocalRot`, `mLocalRotAccel` | Rotation rate/accel (Yaw/Pitch/Roll). | Unused. | **High Priority**: Use Yaw Rate vs Steering Angle to detect oversteer more accurately than Grip Delta. |
| `mSpeed` | Vehicle speed (m/s). | Unused. | Speed-sensitive damping (reduce FFB oscillations at high speed). |
| `mEngineRPM` | Engine rotation speed. | Unused. | **Engine Vibration**: Inject RPM-matched vibration into the wheel (common in fanatec pedals/wheels). |
| `mUnfilteredThrottle` | Raw throttle input. | **Used**: Trigger for Wheel Spin effects. | |
| `mUnfilteredBrake` | Raw brake input. | **Used**: Trigger for Lockup effects. | |
| `mPos`, `mLocalVel`, `mOri` | World position/velocity/orientation. | Unused. | Motion platform integration? (Out of scope for FFB). |
| `mFuel`, `mEngineWaterTemp`, etc. | Vehicle health/status. | Unused. | Dash display data. |

---

## 2. Wheel & Tire Telemetry (`rF2Wheel`)

Available for each of the 4 wheels (`mWheels[0]`=FL, `[1]`=FR, `[2]`=RL, `[3]`=RR).

### Forces & Grip
| Variable | Description | Current Usage | Future Potential |
| :--- | :--- | :--- | :--- |
| `mSteeringArmForce` (Global) | **Note**: This is global, but derived from FL/FR tie rods. | **Used**. | |
| `mLateralForce` | Force acting sideways on the tire contact patch. | **Used**: Rear Oversteer calculation (Aligning Torque). | Front pneumatic trail calculation refinement. |
| `mLongitudinalForce` | Force acting forward/back (Accel/Brake). | Unused. | ABS pulse simulation (modulate brake force). |
| `mTireLoad` | Vertical load (N) on the tire. | Unused. | **Load Sensitivity**: Reduce FFB gain if front tires are unloaded (cresting a hill). |
| `mGripFract` | Grip usage fraction (0.0=No Grip used, 1.0=Limit). | **Used**: Understeer lightness & Oversteer logic. | |
| `mMaxLatGrip` | Theoretical max lateral grip. | Unused. | Normalizing force values across different cars. |

### Motion & Slip
| Variable | Description | Current Usage | Future Potential |
| :--- | :--- | :--- | :--- |
| `mSlipAngle` | Angle between tire direction and velocity vector. | **Used**: Slide Texture trigger. | Pneumatic trail calculation (Slip * Trail Curve). |
| `mSlipRatio` | Difference between wheel rotation and road speed. | **Used**: Lockup & Spin progressive effects. | |
| `mLateralPatchVel` | Velocity of the contact patch sliding sideways. | Unused. | More accurate "scrub" sound/feel than Slip Angle alone. |
| `mRotation` | Wheel rotation speed (rad/s). | Unused. | |

### Suspension & Surface
| Variable | Description | Current Usage | Future Potential |
| :--- | :--- | :--- | :--- |
| `mVerticalTireDeflection` | Compression of the tire rubber. | **Used**: Road Texture (High-pass filter). | |
| `mSuspensionDeflection` | Compression of the spring/damper. | Unused. | **Bottoming Out**: Add a harsh "thud" if deflection hits max travel. |
| `mRideHeight` | Chassis height. | Unused. | Scraping effects. |
| `mTerrainName` | Name of surface (e.g., "ROAD", "GRASS"). | Unused. | **Surface FX**: Different rumble patterns for Kerbs vs Grass vs Gravel. |
| `mSurfaceType` | ID of surface. | Unused. | Faster lookup for Surface FX. |
| `mCamber`, `mToe` | Static/Dynamic alignment. | Unused. | Setup analysis (not FFB). |

### Condition
| Variable | Description | Current Usage | Future Potential |
| :--- | :--- | :--- | :--- |
| `mTemperature[3]` | Inner/Middle/Outer tire temps. | Unused. | **Cold Tire Feel**: Reduce grip/force when tires are below optimal temp range. |
| `mWear` | Tire wear fraction. | Unused. | **Wear Feel**: Reduce overall gain as tires wear out (long stint simulation). |
| `mPressure` | Tire pressure. | Unused. | |
| `mFlat`, `mDetached` | Damage flags. | Unused. | **Damage FX**: Add wobble if tire is flat or wheel detached. |

---

## 3. Summary of "Low Hanging Fruit"

These are features that would provide high value with relatively low implementation effort:

1.  **Surface Effects**: Reading `mTerrainName` to detect "Rumble Strips" or "Kerbs" and injecting a specific vibration pattern. rFactor 2 FFB is usually good at this, but enhancing it (like iRFFB's "Kerb Effect") is popular.
2.  **Engine Vibration**: Adding a subtle RPM-based hum (`mEngineRPM`) adds immersion, especially for idle/revving.
3.  **Suspension Bottoming**: Triggering a heavy jolt when `mSuspensionDeflection` limits are reached.

```

# File: docs\dev_docs\telemetry_logging_investigation.md
```markdown
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

```

# File: docs\dev_docs\test_coverage_analysis.md
```markdown
# Test Coverage Analysis & Recommendations

**Date:** 2025-12-07  
**Version:** v0.3.3  
**Status:** Analysis Complete

## Executive Summary

Current test suite covers approximately **60% of critical FFB logic**. This document identifies gaps and recommends additional tests to achieve **85% coverage** of safety-critical and user-facing functionality.

---

## Current Test Coverage ✅

The existing test suite (`tests/test_ffb_engine.cpp`) validates:

1. **Zero Input Handling** - Ensures no force with null telemetry
2. **Grip Modulation (Understeer)** - Front tire grip loss detection
3. **SoP Effect** - Lateral G-force injection
4. **Min Force** - Deadzone removal
5. **Progressive Lockup** - Phase integration, dynamic frequency
6. **Slide Texture** - Lateral scrubbing vibration
7. **Dynamic Tuning** - GUI parameter changes
8. **Suspension Bottoming** - New v0.3.2 effect validation

**Test Results:** 14/14 passing ✅

---

## Critical Gaps in Coverage ❌

### 1. **Oversteer/Rear Aligning Torque** (HIGH PRIORITY)

**Code Location:** `FFBEngine.h` lines 88-112  
**Current Coverage:** 0%

**What's Missing:**
- Grip delta calculation (`avg_grip - avg_rear_grip`)
- Rear lateral force integration
- Oversteer boost multiplier effect
- Rear aligning torque (`rear_lat_force * 0.05 * m_oversteer_boost`)

**Why Critical:**
This is a **key driver feedback mechanism** for catching slides. If broken, users won't feel oversteer until it's too late.

**Recommended Test:**
```cpp
void test_oversteer_boost() {
    std::cout << "\nTest: Oversteer Boost (Rear Grip Loss)" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));
    
    engine.m_sop_effect = 1.0;
    engine.m_oversteer_boost = 1.0;
    
    // Scenario: Front has grip, rear is sliding
    data.mWheels[0].mGripFract = 1.0; // FL
    data.mWheels[1].mGripFract = 1.0; // FR
    data.mWheels[2].mGripFract = 0.5; // RL (sliding)
    data.mWheels[3].mGripFract = 0.5; // RR (sliding)
    
    // Lateral G (cornering)
    data.mLocalAccel.x = 9.81; // 1G lateral
    
    // Rear lateral force (resisting slide)
    data.mWheels[2].mLateralForce = 2000.0;
    data.mWheels[3].mLateralForce = 2000.0;
    
    double force = engine.calculate_force(&data);
    
    // Expected: SoP boosted by grip delta (0.5) + rear torque
    // Base SoP = 1.0 * 1.0 * 1000 = 1000
    // Boost = 1.0 + (0.5 * 1.0 * 2.0) = 2.0x
    // SoP = 1000 * 2.0 = 2000
    // Rear Torque = 2000 * 0.05 * 1.0 = 100
    // Total SoP = 2100 / 4000 = 0.525
    
    ASSERT_TRUE(std::abs(force - 0.525) < 0.05);
    std::cout << "[PASS] Oversteer boost verified." << std::endl;
}
```

---

### 2. **Phase Integration Edge Cases** (HIGH PRIORITY)

**Code Location:** Lines 138-139, 177-178, 201-202, 255-256  
**Current Coverage:** Partial (basic phase update tested, edge cases not)

**What's Missing:**
- **Phase wraparound** - Does phase correctly wrap at 2π?
- **Phase continuity** - When effects turn on/off, does phase reset cause clicks?
- **Multiple oscillators simultaneously** - Do all 4 phases advance independently?
- **Extreme delta times** - What happens with dt = 0 or dt = 1.0?

**Why Critical:**
Phase discontinuities cause **audible clicks and pops** in the FFB motor, which is the exact problem phase integration was designed to solve.

**Recommended Test:**
```cpp
void test_phase_wraparound() {
    std::cout << "\nTest: Phase Wraparound (Anti-Click)" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));
    
    engine.m_lockup_enabled = true;
    engine.m_lockup_gain = 1.0;
    
    data.mUnfilteredBrake = 1.0;
    data.mWheels[0].mSlipRatio = -0.3;
    data.mWheels[1].mSlipRatio = -0.3;
    data.mLocalVel.z = 20.0; // 20 m/s
    data.mDeltaTime = 0.01;
    
    // Run for 100 frames (should wrap phase multiple times)
    double prev_phase = 0.0;
    int wrap_count = 0;
    
    for (int i = 0; i < 100; i++) {
        engine.calculate_force(&data);
        
        // Check for wraparound
        if (engine.m_lockup_phase < prev_phase) {
            wrap_count++;
            // Verify wrap happened near 2π
            ASSERT_TRUE(prev_phase > 6.0); // Close to 2π
        }
        prev_phase = engine.m_lockup_phase;
    }
    
    // Should have wrapped at least once
    ASSERT_TRUE(wrap_count > 0);
    std::cout << "[PASS] Phase wrapped " << wrap_count << " times without discontinuity." << std::endl;
}
```

---

### 3. **Road Texture State Persistence** (MEDIUM PRIORITY)

**Code Location:** Lines 214-234  
**Current Coverage:** 0%

**What's Missing:**
- High-pass filter (delta calculation)
- State persistence (`m_prev_vert_deflection`)
- Load factor application to road noise

**Why Important:**
Road texture is a **stateful effect**. If state isn't preserved correctly, bumps won't be detected.

**Recommended Test:**
```cpp
void test_road_texture_state_persistence() {
    std::cout << "\nTest: Road Texture State Persistence" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));
    
    engine.m_road_texture_enabled = true;
    engine.m_road_texture_gain = 1.0;
    
    // Frame 1: Initial deflection
    data.mWheels[0].mVerticalTireDeflection = 0.01;
    data.mWheels[1].mVerticalTireDeflection = 0.01;
    data.mWheels[0].mTireLoad = 4000.0;
    data.mWheels[1].mTireLoad = 4000.0;
    
    double force1 = engine.calculate_force(&data);
    // First frame: delta = 0.01 - 0.0 = 0.01
    // Expected force = (0.01 + 0.01) * 5000 * 1.0 * 1.0 = 100
    // Normalized = 100 / 4000 = 0.025
    
    // Frame 2: Bump (sudden increase)
    data.mWheels[0].mVerticalTireDeflection = 0.02;
    data.mWheels[1].mVerticalTireDeflection = 0.02;
    
    double force2 = engine.calculate_force(&data);
    // Delta = 0.02 - 0.01 = 0.01
    // Force should be same as frame 1
    
    ASSERT_TRUE(std::abs(force2 - force1) < 0.001);
    
    // Frame 3: No change (flat road)
    double force3 = engine.calculate_force(&data);
    // Delta = 0.0, force should be near zero
    ASSERT_TRUE(std::abs(force3) < 0.01);
    
    std::cout << "[PASS] Road texture state preserved correctly." << std::endl;
}
```

---

### 4. **Multi-Effect Interaction** (MEDIUM PRIORITY)

**Code Location:** Entire `calculate_force` method  
**Current Coverage:** Partial (individual effects tested, not combinations)

**What's Missing:**
- Lockup + Spin simultaneously
- Slide + Road texture
- All effects enabled at once
- Effect interference/masking

**Why Important:**
In real driving, **multiple effects trigger simultaneously** (e.g., trail braking = lockup + slide + road). We need to verify they don't interfere.

**Recommended Test:**
```cpp
void test_multi_effect_interaction() {
    std::cout << "\nTest: Multi-Effect Interaction (Lockup + Spin)" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));
    
    // Enable both lockup and spin
    engine.m_lockup_enabled = true;
    engine.m_lockup_gain = 1.0;
    engine.m_spin_enabled = true;
    engine.m_spin_gain = 1.0;
    
    // Scenario: Braking AND spinning (e.g., locked front, spinning rear)
    data.mUnfilteredBrake = 1.0;
    data.mUnfilteredThrottle = 0.5; // Partial throttle
    data.mWheels[0].mSlipRatio = -0.3; // Front locked
    data.mWheels[1].mSlipRatio = -0.3;
    data.mWheels[2].mSlipRatio = 0.5;  // Rear spinning
    data.mWheels[3].mSlipRatio = 0.5;
    data.mLocalVel.z = 20.0;
    data.mDeltaTime = 0.01;
    
    // Run multiple frames
    for (int i = 0; i < 10; i++) {
        engine.calculate_force(&data);
    }
    
    // Verify both phases advanced
    ASSERT_TRUE(engine.m_lockup_phase > 0.0);
    ASSERT_TRUE(engine.m_spin_phase > 0.0);
    
    // Verify phases are different (independent oscillators)
    ASSERT_TRUE(std::abs(engine.m_lockup_phase - engine.m_spin_phase) > 0.1);
    
    std::cout << "[PASS] Multiple effects coexist without interference." << std::endl;
}
```

---

### 5. **Load Factor Edge Cases** (MEDIUM PRIORITY)

**Code Location:** Lines 59-69  
**Current Coverage:** Implicit (used in bottoming test, not explicitly tested)

**What's Missing:**
- Zero load (airborne)
- Extreme load (20000N compression)
- Negative load (invalid data)
- Clamp verification (1.5x cap)

**Why Important:**
Load factor is a **safety-critical multiplier**. Unclamped values could cause violent jolts or motor damage.

**Recommended Test:**
```cpp
void test_load_factor_edge_cases() {
    std::cout << "\nTest: Load Factor Edge Cases" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));
    
    engine.m_slide_texture_enabled = true;
    engine.m_slide_texture_gain = 1.0;
    
    // Setup slide condition
    data.mWheels[0].mSlipAngle = 0.2;
    data.mWheels[1].mSlipAngle = 0.2;
    data.mWheels[0].mLateralPatchVel = 5.0;
    data.mWheels[1].mLateralPatchVel = 5.0;
    data.mDeltaTime = 0.01;
    
    // Case 1: Zero load (airborne)
    data.mWheels[0].mTireLoad = 0.0;
    data.mWheels[1].mTireLoad = 0.0;
    
    double force_airborne = engine.calculate_force(&data);
    // Load factor = 0, slide texture should be silent
    ASSERT_TRUE(std::abs(force_airborne) < 0.001);
    
    // Case 2: Extreme load (20000N)
    data.mWheels[0].mTireLoad = 20000.0;
    data.mWheels[1].mTireLoad = 20000.0;
    
    engine.calculate_force(&data); // Advance phase
    double force_extreme = engine.calculate_force(&data);
    
    // Load factor should be clamped at 1.5
    // Max expected: sawtooth * 300 * 1.5 = 450
    // Normalized: 450 / 4000 = 0.1125
    ASSERT_TRUE(std::abs(force_extreme) < 0.15); // Allow margin
    
    std::cout << "[PASS] Load factor clamped correctly." << std::endl;
}
```

---

### 6. **Wheel Spin Torque Drop Interaction** (MEDIUM PRIORITY)

**Code Location:** Line 160  
**Current Coverage:** 0%

**What's Missing:**
- Torque drop applied to combined force (game + SoP)
- Interaction with other additive effects
- Extreme slip values (100% slip)

**Why Important:**
Torque drop is **multiplicative**, not additive. It modifies the total force, which could cause unexpected behavior if other effects are active.

**Recommended Test:**
```cpp
void test_spin_torque_drop_interaction() {
    std::cout << "\nTest: Spin Torque Drop with SoP" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));
    
    engine.m_spin_enabled = true;
    engine.m_spin_gain = 1.0;
    engine.m_sop_effect = 1.0;
    
    // High SoP force
    data.mLocalAccel.x = 9.81; // 1G lateral
    data.mSteeringArmForce = 2000.0;
    
    // No spin initially
    data.mUnfilteredThrottle = 0.0;
    double force_no_spin = engine.calculate_force(&data);
    
    // Now trigger spin
    data.mUnfilteredThrottle = 1.0;
    data.mWheels[2].mSlipRatio = 0.7; // 70% slip (severe = 1.0)
    data.mWheels[3].mSlipRatio = 0.7;
    data.mLocalVel.z = 20.0;
    data.mDeltaTime = 0.01;
    
    double force_with_spin = engine.calculate_force(&data);
    
    // Torque drop: 1.0 - (1.0 * 1.0 * 0.6) = 0.4 (60% reduction)
    // Force should be significantly lower
    ASSERT_TRUE(force_with_spin < force_no_spin * 0.5);
    
    std::cout << "[PASS] Spin torque drop reduces total force." << std::endl;
}
```

---

## Lower Priority Gaps

### 7. **Config Save/Load** (Integration Test)
**File:** `src/Config.cpp`  
**Coverage:** 0%

Not critical for unit tests (requires file I/O), but should have integration tests.

### 8. **DirectInput FFB** (Hardware Mock Required)
**File:** `src/DirectInputFFB.cpp`  
**Coverage:** 0%

Requires hardware mocking. Consider manual testing or hardware-in-the-loop tests.

### 9. **Dynamic vJoy** (Runtime Dependency)
**File:** `src/DynamicVJoy.h`  
**Coverage:** 0%

Graceful degradation tested manually. Low priority for automated tests.

---

## Recommended Test Implementation Plan

### Phase 1: High Priority (Target: 1-2 hours)
1. ✅ Implement `test_oversteer_boost()`
2. ✅ Implement `test_phase_wraparound()`
3. ✅ Implement `test_multi_effect_interaction()`

### Phase 2: Medium Priority (Target: 1 hour)
4. ✅ Implement `test_road_texture_state_persistence()`
5. ✅ Implement `test_load_factor_edge_cases()`
6. ✅ Implement `test_spin_torque_drop_interaction()`

### Phase 3: Polish (Target: 30 minutes)
7. Add frequency capping tests (80Hz spin, 250Hz slide)
8. Add severity scaling edge cases (negative values, > 1.0)

---

## Expected Outcome

**Current Coverage:** ~60%  
**After Phase 1:** ~75%  
**After Phase 2:** ~85%  
**After Phase 3:** ~90%

**85% coverage is excellent** for a real-time physics application and covers all safety-critical paths.

---

## Test Execution

**Build Command:**
```powershell
& 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1' -Arch amd64 -SkipAutomaticLocation; cl /EHsc /std:c++17 /I.. test_ffb_engine.cpp /Fe:test_ffb_engine.exe
```

**Run Command:**
```powershell
.\test_ffb_engine.exe
```

**Current Results:** 14/14 passing ✅

---

## Conclusion

The current test suite provides a **solid foundation** but misses critical edge cases and interaction scenarios. Implementing the recommended tests will:

1. **Prevent regressions** in oversteer detection (driver safety)
2. **Eliminate audio glitches** from phase discontinuities (user experience)
3. **Validate safety clamps** on load factor (hardware protection)
4. **Ensure multi-effect stability** (real-world driving scenarios)

**Recommendation:** Implement Phase 1 tests immediately before v0.3.4 release.

```

# File: docs\dev_docs\TODO.md
```markdown
# LMUFFB To-Do List

## Completed Features
- [x] **Core FFB Engine**: C++ implementation of Grip Modulation, SoP, and Texture effects.
- [x] **GUI**: Dear ImGui integration with sliders and toggles.
- [x] **Config Persistence**: Save/Load settings to `config.ini`.
- [x] **Installer**: Inno Setup script for automated installation.
- [x] **Documentation**: Comprehensive guides for Architecture, Customization, and Licensing.

## Immediate Tasks
- [x] **DirectInput Support**: Replace vJoy with native DirectInput "Constant Force" packets.
- [] Guided installer as in docs\plan_guided_installer.md
- [] Add in app guided configurator to as described in Guided configurator in the app
- [] If possible, completely remove vJoy dependency as described in docs\dev_docs\investigation_vjoyless_implementation.md
- [] Troubleshooting visualization of FFB and telemetry values as in (future doc) docs\plan_troubleshooting_FFB_visualizations.md
- [ ] **Advanced Filtering**: Implement Bi-Quad filters (Low Pass / High Pass) for better road texture isolation.
- [ ] **Telemetry Analysis**: Add visual graphs to the GUI showing the raw vs. filtered signal.
- [ ] **Telemetry Logging**: Investigate and implement CSV logging. See `docs/dev_docs/telemetry_logging_investigation.md`.

## Backlog / Wishlist
- [ ] **Wheel Profiles**: Save/Load settings per car or wheel base.
- [ ] **Auto-Launch**: Option to start automatically with LMU.
- [ ] **Minimize to Tray**: Keep app running in background without taskbar clutter.


## Bug fixes, polish, throubleshooting, and improvements

Make install instruction more clear. Make them consistent between readme.txt and readme.md (eg. which shared memory plugin to install).
Shared memory plugin: this should be correct (please confirm/verify):
* https://github.com/TheIronWolfModding/rF2SharedMemoryMapPlugin#download
* https://www.mediafire.com/file/s6ojcr9zrs6q9ls/rf2_sm_tools_3.7.15.1.zip/file
Make better instructions on installing and enabling the shared memory plugin. Maybe suggest installing CrewChief since it might do that automatically. Note: there is no ingame option in LMU to enable the shared memory plugin (this was maybe in rFactor 2).

Add link to download vJoy (from github). vJoy SDK dll (64 bit) : https://github.com/shauleiz/vJoy/tree/master/SDK/lib/amd64/vJoyInterface.dll

In the instructions for user (in readme) include some of the tips from docs\bug_reports\Wheel Jerking with SOP Slider.md : **Summary Checklist**,  Tip to bind wheel ("You may need to use the "vJoy Feeder" app (comes with vJoy) to move the virtual axis so the game detects it").
Also need to include in the instructions for user (in readme) that LMU must be run in "Borderless" window mode, otherwise crashes happen.

Guided install: give the tip on how to bind the wheel to vJoy, "You may need to use the "vJoy Feeder" app (comes with vJoy) to move the virtual axis so the game detects it".

Address the bug report in docs\bug_reports\Wheel Jerking with SOP Slider.md
Set default settings to the safest possible, eg.: **SOP Effect** at **0.0** and **Master Gain** at **0.5**.

Another user got a crash (it was probably because LMU was not run in borderless window mode).

Another user was having a very strong wheel if gain was above 0 (even at 0.01).It was like having 100% FFB, and he was using a DD wheel with 9 newton meters (Nm) of force.

Todo: we need a markdown document with a plan on how to make a fully guided install (instead of releasing the exe). The install should guide the user through the process of installing vJoy, enabling the shared memory plugin, and binding the wheel to vJoy. Should also display all other instructions that are on the readme.

Consider this: Would it help if we suggest, in the readme, to disable,  in vjoy config,   all effects except constant?

In the pop up that appears if vJoy is not found, put a more descriptive message (eg. version of vJoy needed, put the dll in the same dir as the exe (naming in full the dll), and say if the dll is from vJoy SDK or basic installation). But anyway the real solution to this is the fully guided install.

Do an investigation on this, and save a report to a markdown file: Apparently Marvin's app managed an implementation that does not need vJoy. We currently need vJoy as a "dummy" device so that the game sends its FFB there. Investigate if we can do something similar to Marvin's app, getting rid of vJoy need.

## Throubleshooting 2

### FFB Visualizations

Add a new, comprehensive markdown document (docs\plan_troubleshooting_FFB_visualizations.md) discussing and planning how can we make the app more easy to debug and troubleshoot. In particular, for cases in which the FFB is not working as intended (eg. forces too strong, jerking, etc.), for be able to more easily identify the cause of the problem. Consider also adding display widgets that show in the GUI more info and visuals on what the FFB is doing, including having separated visual for each effect or formula, so we can see which one is "exploding" with high values, or oscillating, or having other strange behavior.
This could also be in the form of a "trail" visualization, in that we show the trails of multiple values over time (eg. over the last 5-10 seconds), so we can see if there are any patterns or anomalies.

In the GUI there should be an option (eg. checkbox or button) to display these additional visualizations for troubleshooting FFB. Each component in our FFB formula should have its own visualization, so that it can be independently visualized and checked.

Consider also adding additional trouble shooting visuals, like status of the shared memory (eg. values we read). We should definitely include also a visual for each telemetry value that we read. It should be visualized like the other trails for the formulas. Also, each visualization must have a checkbox to enable or disable it, to avoid visual clutter.

We should mention in the readme that there are these visualization options, and that they can be enabled or disabled in the GUI. We should also include a screenshot of the GUI with these visualization options enabled.
From these visualizations, we will be able also to spot telemetry values that are absent (eg. always constant or zero).
We should also log more info to the console every time something is not as expected. Eg. if we detect that some telemetry values are always constant or zero, we should log a warning to the console (be carefull not to clutter it and repeat the same warning multiple times, or too often).

Additionally, we could have an app mode to start a "troubleshooting" sesssion (like a "calibration" or something smilar), in that we ask the user to perform a series of actions to help us identify the cause of the problem. These actions could be things like: 
* drive the car in a straight line, 
* do a braking in a straight line,
* drive the car in a corner, 
* do some train braking (braking and turning into a corner)
* accelerate flat out to try indice wheel sping,
* etc.
During this "troubleshooting session", we should log all the telemetry and FFB values to a file (or at least a summary of them). The FFB values should include the individual values of each effect or formula in the FFB. We should also log the FFB values that we are receiving from the game.
We could either have a mechanism for allowing the user to send this data (logged values and / or summary) to us, or have some code that automatically analyzes that data and spots issue, so that the user can copy and paste the issues report / summary into a bug report.

### Clarify how it works the communication of steering inputs among real wheel, vJoy and game

Solve this contradiction:
the readme instructions say that we must bind the ingame control to vJoy,  so that it is a dummy wheel to send the FFB to instead of the real wheel, but if we do that, we can't control the car from the game right? Or would vJoy take the position information from the real wheel and send it to the game? 
In any case, if possible we need to perform additional checks on this (possibly at regular intervals during runtime, as a throubleshooting feature), eg. if vJoy is receiving control inputs from the real wheel.
Should we add an instruction in the readme that says we should bind the real wheel to vJoy?

### Guided configurator in the app
In addition to the guided installer, from the main GUI of the app, should we have a button to run a guided configurator? This is both for initial configuration, and if things mess up and need to be reconfigured.

### Checks for FFB outputs from the game (not the app): "Race condition Double FFB"

See if it is possible to check if the game is sending FFB to the wheel instead of vJoy: eg. from the shared memory, maybe there is info about it. If we detect it, we should print a warning to the console.

# Update README.txt and or README.md

Update README.txt and README.md. The two had conflicting information about the installation process. Make sure they are consistent.

## Throubleshooting 3

### Verify readme, instructions and other docs apply to LMU

This app is primarily for LMU. All information in readme, instructions and other docs must be verified to be valid for LMU. We can mention rF2 only secondarily.
Eg. there seems to be no "None" FFB option in LMU, verify this and update the readme and any other relevant docs.
Add a screenshot of the LMU in-game settings, so we can reference it during development  to confirm what LMU allows.

TODO: update all references to  In-Game FFB to 'None' or 'Smoothing 0' to reflect what LMU actually allows.

TODO: replace "Game" with "LMU" in all references.

### Test Required for removing vJoy dependency
Note: The dependency on vJoy exists primarily to **prevent the game from locking the physical wheel**.
If LMU allows binding an axis *without* acquiring the device for FFB:
1.  Bind Physical Wheel Axis X to Steering in-game.
2.  Set "FFB Type" to **"None"** in game settings.
3.  LMUFFB acquires the wheel via DirectInput.
    *   **Risk:** If the game opens the device in "Exclusive" mode just to read the Axis, LMUFFB might be blocked from writing FFB (or vice versa). DirectInput usually allows "Background" "Non-Exclusive" reads, but FFB writing often requires "Exclusive".
Can LMUFFB send FFB to a device that LMU is actively reading steering from?
    - If yes: vJoy is NOT needed.
    - If no (Access Denied): We need vJoy.

**Experiment for v0.4.0:**
Try "Method 1":
1.  User binds real wheel in game.
2.  User disables FFB in game (Type: None).
3.  LMUFFB attempts to acquire wheel in `DISCL_BACKGROUND | DISCL_EXCLUSIVE` mode.
4.  If it works, vJoy is obsolete.
5.  If it fails (Device Busy), we need vJoy.

### Check and command line log for FFB acquired exclusiverly by LMU

During the loop (or early startup) add a check if the FFB is acquired exclusiverly by LMU. If it is, print a warning to the console.
We could also add a button in the GUI to perform this check: the user would first start LMU and start driving, ensuring that the FFB is working from LMU. Then the user would start LMUFFB and click the button to perform the check. If the FFB is acquired exclusiverly by LMU, print a warning to the console.

### Review docs\plan_troubleshooting_FFB_visualizations.md

Review docs\plan_troubleshooting_FFB_visualizations.md and make sure that in "### A. FFB Component Graph ("The Stack")" we have listed one graph for each component of the FFB, that is, each individual formula or effect that can independently not work as expected and cause FFB issues.

Do a similar review for section "### B. Telemetry Inspector", making sure that we have an invidual visualization for each value we read from game shared memory / telemetry.

Update docs\plan_troubleshooting_FFB_visualizations.md stating clearly that we want the visualization of each value (FFB or telemetry) to be a "trailing" or "trace" live plot visualization that shows the last 5-10 seconds of data, with a sliding window. The plot should be similar to the "telemetry" trace often shown in driving sims with the driver inputs (accelerator pedal, breaking, steering wheel position, etc.).

### DONE:Update README.txt again and remove INSTALL.txt

README.txt install instructions must be as detailed as the ones in README.md. Note for example that in README.txt we don't indicate a path to download the vJoy SDK dll.

Review INSTALL.txt: does it contain any additional information that is not in README.txt? It seems it is a redundant file, that should be deleted once we include all the information in README.txt.

### DONE: Telemetry trail visualization example

See image docs\bug_reports\telemetry_trail_example.png , which display a typical plot visualization with a telemetry trail of user inputs (accelerator pedal and breaking pedal), with a trailing window of a few seconds. On the right side, there are also histograms of the current (istant) values of the same telemetry values. In any case, this current note from docs\plan_troubleshooting_FFB_visualizations.md is already sufficient to precisely identify the style of these plots: "*Visual Style:* All graphs should be **Rolling Trace Plots** (Oscilloscope style), showing the last 5-10 seconds of data. This helps spot patterns like oscillation or spikes."

## Throubleshooting 4

### More about vJoy

- [x] **Bundling Investigation**: Yes, we can bundle `vJoyInterface.dll` (x64). See `docs/dev_docs/investigation_vjoy_bundling.md`.
- [x] **Version Check**: Implemented startup check for vJoy driver version (warns if < 2.1.8).
- [x] **Licensing**: vJoy is MIT licensed. Added `licenses/vJoy_LICENSE.txt`.


## Throubleshooting 5

### LMU  in-game settings: controls and FFB

See these screenshots to see the available options in LMU:
* docs\bug_reports\LMU_settings_controls.png
* docs\bug_reports\LMU_settings_FFB.png

In particular the relevant setting seems to be "Force Feedback Strenght", that we should set to 0% , so that LMU does not send FFB to the wheel. Update README.md and README.txt to reflect this (instructing on setting this value to 0% in the in-game settings).
We should also probably instruct the user to disable the first toggle, "Force Feedback Effects". Also minimum steering torque should be set to 0%. Pheraphs also "Force Feedback Smoothing" should be set to 0. Also the values in "Advanced Force Feedback" should be set to 0: "Collision Strenght" and "Steering Torque Sensitivity". And also disable the toggle in the final section "Tweaks": "Use Constant Steering Force Effect".

### Smoothing options

For each case in which apply smoothing or other corrections to the FFB formula (eg. cap to values), we should add support to disable/enable each (individually) and, when possible, to configure them within a range of values (eg. from a minimum to a maximum value). For the cap to values, we might allow to increase or decrease the cap value, so that the user can fine tune the FFB to their liking. We might still enforce some hard limits on the ranges of configuration values allowed (to prevent values that will cause clear issues or bugs).


### vJoy bundling

Note that we can consider separately the vJoy installer and the vJoy sdk dll. We could only bundle the sdk dll, and let the user install vJoy through the installer. But we must also check that the installed vJoy version is the expected one, otherwise show a popup with a warning (with a "don't show again" checkbox) and a warning message to the console (this one always active since it is less intrusive). Confirm this is already fully implemented or not.

### vJoy links

Keep this reference for the link to download the vJoy SDk (although we can bundle it with our app): 
https://sourceforge.net/projects/vjoystick/files/Beta%202.x/2.1.9.1-160719/

Keep this reference for the version of vJoy to install:
https://github.com/jshafer817/vJoy/releases/tag/v2.1.9.1


## Throubleshooting 6

### vJoy dll location issue
Address this issue:
"I have moved LMUFFB.exe to the vJoy installation directory (C:\Program Files\vJoy\x64), because the vJoy config/monitor/feeder was complaining if I moved the 'vJoyInterface.dll' elsewhere. LMUFFB seems fine running in here."
We might not want to bundle the vJoyInterface.dll with our app, because of the reported issue. We could either bundle the full vJoy SDK with our app, or if the user has it already installed (with the SDK, note that there are versions of vJoy without the SDK) we might ask the user, during the guided installer (and the guided configurator) to set the path in which the vJoyInterface.dll is located. 
To address this issue, update docs\plan_guided_installer.md and docs\dev_docs\plan_guided_configurator.md .

### wheel binding issues

Address this user reported bug: docs\bug_reports\User bug report 002 VJoy Setup Troubleshooting and Bug.md

Implement "### Recommendation for Developers (Code Fix)". Also add an option in the Gui to enable / disable m_output_ffb_to_vjoy (disabled by default).

Update accordingly this section of the README.md : "4. Steering Axis - Choose ONE method:", and any other relevant section.


### Readme updates (txt and md)

In the install and configuration instructions, consider (if relevant according to latest code changes and findings) adding instruction to the user to disable the LMU in-game FFB forces (so no forces are transmitted from the game to the wheel), using these setting:  "Force Feedback Strenght", that we should set to 0% , so that LMU does not send FFB to the wheel. Update README.md and README.txt to reflect this (instructing on setting this value to 0% in the in-game settings).
We should also probably instruct the user to disable the first toggle, "Force Feedback Effects". Also minimum steering torque should be set to 0%. Pheraphs also "Force Feedback Smoothing" should be set to 0. Also the values in "Advanced Force Feedback" should be set to 0: "Collision Strenght" and "Steering Torque Sensitivity". And also disable the toggle in the final section "Tweaks": "Use Constant Steering Force Effect".

If these suggestions make sense, consider if making them the main instructions, or as secondary suggestions of things to try or for troubleshooting.

## Throubleshooting 7

See docs\bug_reports\User bug report 002 VJoy Setup Troubleshooting and Bug.md , there is an update section from "# Follow up question 1" onward. Please address that and update the README.md and README.txt, and any other relevant document accordingly.

## Throubleshooting 8

1. Review the FFB code again, to check for all the smoothing we apply. I remember there are supposed to be at least 2 different smoothings applied in different parts of the formula / different effects. For each one of such smoothings, there must be a configuration option (also in the GUI) to adjust it (including a neutral value that would effectively disable the smoothing). Currently, the GUI (in Advanced Tuning) allows to to adjust "SoP Smoothing". 

2. Implement this major new feature:
- [] Troubleshooting visualization of FFB and telemetry values as in (future doc) docs\plan_troubleshooting_FFB_visualizations.md

## Throubleshooting 9

In the readmes, for the "bridge" solution, include more detailed instructions as described here: docs\bug_reports\User bug report 002 VJoy Setup Troubleshooting and Bug.md , "# Answer 1" section. I copy them here:

### 1. Download Joystick Gremlin
*   Download it here: [https://whitemagic.github.io/JoystickGremlin/](https://whitemagic.github.io/JoystickGremlin/)
*   Install and run it.

### 2. Map Your Wheel (The Bridge)
1.  In Joystick Gremlin, you will see tabs at the top for your connected devices. Click the tab for your **Moza R9**.
2.  Turn your physical wheel. You should see one of the axes in the list highlight/move (usually "X Axis" or "Rotation X").
3.  Click on that axis in the list.
4.  On the right side, under "Action", choose **"Map to vJoy"**.
5.  **Crucial Step (To fix the spinning bug):**
    *   Select vJoy Device: **1**
    *   Select Axis: **Y** (Do **NOT** select X, as LMUFFB is currently overwriting X for its display, causing the spinning).

### 3. Activate the Bridge
*   In the top menu of Joystick Gremlin, click the **"Activate"** button (the Gamepad icon turns green).
*   *Test:* Now, when you turn your physical Moza wheel, the **Y bar** in the vJoy Monitor (or the Feeder you screenshotted) should move.

### 4. Bind in Game
*   Go into Le Mans Ultimate.
*   Bind **Steering** to that new **vJoy Axis Y**.

Once you do this, the "Bridge" is complete:
**Moza** -> **Joystick Gremlin** -> **vJoy Axis Y** -> **Game**.

This leaves **vJoy Axis X** free for LMUFFB to use for its display without interfering with your steering.



## Throubleshooting 10

Implement the changes described in this doc:
docs\dev_docs\proposed_changes_to_disable_vJoy.md

## Throubleshooting 11
1. I repea what I requested above:
I have done a change to your latest code in main.pp, please review if this is correct: Line 71 (my version): if (vJoyDllLoaded && DynamicVJoy::Get().Enabled()) { // TODO: I have re-added " && DynamicVJoy::Get().Enabled()" make sure this is correct (your version): if (vJoyDllLoaded) {

Verify if this change is correct. Note that I have not pushed the changes, so you must compare your local code with the line I have pasted here.


2.  Also, in your latest changes, I don't see any addition of a new GUI checkbox to "release" vJoy ("Output FFB to vJoy").
Have you merged / confused it with "Monitor FFB on vJoy" checkbox? I think they should remain two separate checkboxes.
Also, every time you enable / disable it, and we acquire or release vJoy, we should print that info on the console.
Please review again docs\dev_docs\proposed_changes_to_disable_vJoy.md


3. In our FFB formula, we have an hardcoded scaling factor of 1000 for the SoP. Instead, expose the scaling factor in the GUI so that it can be adjusted by the user within a range of values.

## Throubleshooting 12

### More on enabling vJoy checkboxes
The difference between vJoy enabled and vJoy monitoring (the two checkboxes) it that monitoring additionally reads the vJoy values and displays them in the telemetry, for debugging purposes. In theory, we could have vJoy enabled without monitoring (but not the other way around I think).

### Casting to int for Plotting
Note that to avoid a compile warning to precision loss I added some casting in src\GuiLayer.cpp, eg:
        ImGui::PlotLines("##Total", plot_total.data.data(), (int)plot_total.data.size(), plot_total.offset, "Total", -1.0f, 1.0f, ImVec2(0, 60));
These int casting are present for all PlotLines calls. Make sure you edit your code with this change, so I don't get any merge issue later.

### Mutex bug

Verify the issues described here and in case fix it: docs\dev_docs\Missing Mutex Lock (Race Condition).md
```

# File: docs\python_version\performance_analysis.md
```markdown
# Performance Analysis: Python vs C++

A common question for real-time applications, especially in sim racing where latency is critical, is the choice of language.

## Python (Legacy Prototype)

*   **Pros**: Rapid development, readability, easy memory mapping (`mmap`).
*   **Cons**:
    *   **Garbage Collection (GC)**: Python's GC can introduce "stop-the-world" pauses. In a loop running at 400Hz (2.5ms per frame), a GC pause of 1-2ms is significant.
    *   **Jitter**: The wake-up time of `time.sleep()` in Python is less consistent on Windows.

## C++ (Current Implementation)

*   **Pros**:
    *   **Deterministic Latency**: No GC. Manual memory management ensures consistent loop times.
    *   **Raw Speed**: Math operations are negligible in cost.
    *   **Multithreading**: We now use a dedicated `FFBThread` running at 400Hz, completely decoupled from the GUI thread.
    *   **Direct API Access**: Interfacing with Windows APIs (`OpenFileMapping`, `vJoyInterface`) is native.

## Technology Choice: C++ vs Rust

While Rust offers memory safety guarantees, **C++** was chosen for this project because:
1.  **Industry Standard**: The rFactor 2 plugin SDK is written in C++. Integration with `ctypes` structs (which mirror C structs) is trivial.
2.  **Examples**: Abundant sample code exists for rFactor 2 plugins in C++.
3.  **Dependencies**: The vJoy SDK is provided as a C/C++ library (`.lib`). Linking this in Rust requires FFI bindings which adds complexity.

## Benchmark Expectations

| Metric | Python | C++ |
| :--- | :--- | :--- |
| **Loop Rate** | ~350-400Hz (Jittery) | **400Hz (Stable)** |
| **Input Lag** | ~5-10ms added | **<1ms added** |
| **CPU Usage** | Moderate (Interpreter) | **Low (Compiled)** |

```

# File: docs\python_version\porting_guide_cpp.md
```markdown
# Porting Guide: Python to C++ (Legacy)

> **Note**: This guide served as the blueprint for the C++ port. The port is now complete and located in the root of the repository. This document is preserved for reference regarding the transition logic.

This guide outlines the steps required to port LMUFFB from Python to C++ for improved performance and lower latency.

## 1. Development Environment

*   **IDE**: Visual Studio 2022 (Community Edition or newer).
*   **Workload**: Desktop development with C++.
*   **SDKs**: Windows SDK (included with VS).

## 2. Shared Memory Access

In C++, you access the rFactor 2 Shared Memory directly using Windows APIs.

**Key APIs**:
*   `OpenFileMappingA`: Opens the named shared memory object.
*   `MapViewOfFile`: Maps the memory into your process's address space.

**Snippet**:
```cpp
#include <windows.h>
#include <iostream>
#include "rF2Telemetry.h" // You need the header with struct definitions

// Constants
const char* SHARED_MEMORY_NAME = "$rFactor2SMMP_Telemetry$";

int main() {
    HANDLE hMapFile = OpenFileMappingA(FILE_MAP_READ, FALSE, SHARED_MEMORY_NAME);
    if (hMapFile == NULL) {
        std::cerr << "Could not open file mapping object (" << GetLastError() << ")." << std::endl;
        return 1;
    }

    auto* pTelemetry = (rF2Telemetry*)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, sizeof(rF2Telemetry));
    if (pTelemetry == NULL) {
        std::cerr << "Could not map view of file (" << GetLastError() << ")." << std::endl;
        CloseHandle(hMapFile);
        return 1;
    }

    // Main Loop
    while (true) {
        // Access data directly
        double engineRPM = pTelemetry->mEngineRPM;
        // ... calculation logic ...
        Sleep(2); // ~500Hz
    }

    UnmapViewOfFile(pTelemetry);
    CloseHandle(hMapFile);
    return 0;
}
```

## 3. Data Structures

You must define the `rF2Telemetry`, `rF2Wheel`, etc., structs exactly as they are in the Python code (which mirrors the C++ original).

*   **Alignment**: Ensure strict packing if the plugin uses it. Usually, standard alignment works, but `#pragma pack(push, 1)` might be needed if the plugin uses 1-byte packing. The standard rFactor 2 SDK usually relies on default alignment (4 or 8 bytes).
*   **Headers**: Refer to [The Iron Wolf's Plugin Source](https://github.com/TheIronWolfModding/rF2SharedMemoryMapPlugin) for the canonical header files (`rF2State.h`, etc.).

## 4. FFB Output (vJoy)

To replace `pyvjoy`, you will use the **vJoyInterface.dll** C SDK.

1.  **Download**: vJoy SDK from the vJoy website or GitHub.
2.  **Include**: `public.h` and `vjoyinterface.h`.
3.  **Link**: `vJoyInterface.lib`.

**Snippet**:
```cpp
#include "public.h"
#include "vjoyinterface.h"

// Initialize
UINT iInterface = 1;
AcquireVJD(iInterface);

// Update Axis (X Axis)
long min_val = 1;
long max_val = 32768;
// Calculate force (normalized -1.0 to 1.0)
double force = ...; 
long axis_val = (long)((force + 1.0) * 0.5 * (max_val - min_val) + min_val);

SetAxis(axis_val, iInterface, HID_USAGE_X);

// Cleanup
RelinquishVJD(iInterface);
```

## 5. FFB Engine Logic

Porting `ffb_engine.py` is straightforward.

*   `self.smoothing`: Implement using a simple exponential moving average or a low-pass filter class.
*   `mGripFract`: Directly accessible from the struct.
*   **Math**: Use `std::min`, `std::max`, `std::abs` from `<algorithm>` and `<cmath>`.

## 6. Optimization

*   **High Resolution Timer**: Use `QueryPerformanceCounter` for timing loop execution to ensure steady 400Hz updates, replacing `Sleep(2)` which can be imprecise.
*   **DirectInput**: For the long-term roadmap, replace vJoy with direct DirectInput FFB commands (`IDirectInputDevice8::SendForceFeedbackCommand`), which allows sending constant force packets without a virtual driver middleware.

```

# File: docs\python_version\porting_guide_rust.md
```markdown
# Porting Guide: Python to Rust (Alternative)

> **Note**: The project has currently standardized on C++ for the native implementation. This guide is preserved as a reference for any future experimental Rust port or fork.

This guide outlines the steps required to port LMUFFB from Python to Rust, prioritizing memory safety and modern tooling.

## 1. Development Environment

*   **Installer**: `rustup` (Windows).
*   **Toolchain**: `stable-x86_64-pc-windows-msvc`.
*   **IDE**: VS Code with `rust-analyzer` extension.

## 2. Dependencies (Crates)

Add these to your `Cargo.toml`:

```toml
[dependencies]
windows = { version = "0.52", features = ["Win32_System_Memory", "Win32_Foundation", "Win32_System_Threading"] }
libc = "0.2"
# Optional: 'vjoy' wrapper crate if available, or bindgen
```

## 3. Shared Memory Access

Rust uses the `windows` crate to call Win32 APIs. Accessing raw memory requires `unsafe` blocks.

**Snippet**:
```rust
use windows::Win32::System::Memory::{OpenFileMappingA, MapViewOfFile, FILE_MAP_READ};
use windows::Win32::Foundation::{CloseHandle, HANDLE};
use windows::core::PCSTR;
use std::ffi::CString;

#[repr(C)]
struct Rf2Telemetry {
    // Define fields matching C++ struct
    // Use types like f64, i32, [u8; 64]
    m_time: f64,
    // ...
}

fn main() -> windows::core::Result<()> {
    let map_name = CString::new("$rFactor2SMMP_Telemetry$").unwrap();
    
    unsafe {
        let handle = OpenFileMappingA(
            FILE_MAP_READ, 
            false, 
            PCSTR(map_name.as_ptr() as *const u8)
        )?;

        if handle.is_invalid() {
            panic!("Could not open file mapping");
        }

        let ptr = MapViewOfFile(handle, FILE_MAP_READ, 0, 0, std::mem::size_of::<Rf2Telemetry>());
        if ptr.is_null() {
            panic!("Could not map view of file");
        }

        let telemetry = &*(ptr as *const Rf2Telemetry);

        loop {
            // Read data safely (once cast)
            let rpm = telemetry.m_engine_rpm; // Hypothetical field
            
            // ... Logic ...
            
            std::thread::sleep(std::time::Duration::from_millis(2));
        }
        
        // Cleanup happens when handle is dropped if wrapped properly, 
        // but raw handles need CloseHandle(handle);
    }
    
    Ok(())
}
```

## 4. Data Structures

You must define the structs with `#[repr(C)]` to ensure they match the C layout in memory.

```rust
#[repr(C)]
pub struct Rf2Vec3 {
    pub x: f64,
    pub y: f64,
    pub z: f64,
}

#[repr(C)]
pub struct Rf2Wheel {
    pub m_suspension_deflection: f64,
    // ... all other fields
}

#[repr(C)]
pub struct Rf2Telemetry {
    pub m_time: f64,
    // ...
    pub m_wheels: [Rf2Wheel; 4],
}
```

## 5. FFB Output (vJoy)

There are a few Rust crates for vJoy (e.g., `vjoy-rs`), but they might be unmaintained. The most robust method is to link against `vJoyInterface.dll` using `libloading` or `bindgen`.

**Using `libloading` (Dynamic Loading)**:
1.  Load `vJoyInterface.dll`.
2.  Get symbols for `AcquireVJD`, `SetAxis`, `RelinquishVJD`.
3.  Call them inside `unsafe` blocks.

## 6. FFB Engine Logic

Porting the Python logic to Rust is ideal for safety.

*   **Structs**: Create a `FfbEngine` struct holding state (smoothing buffers).
*   **Traits**: Implement traits like `Default` for initialization.
*   **Math**: Rust's `f64` methods (`.min()`, `.max()`, `.abs()`) map directly to Python's.

## 7. Performance Notes

*   Rust's release builds (`cargo build --release`) are comparable to C++ in speed.
*   **Safety**: Rust prevents buffer overflows when accessing arrays (like `m_wheels`), but since the raw pointer comes from Shared Memory, the initial dereference is `unsafe`. Once wrapped in a safe abstraction, the rest of the app is protected.

```

# File: installer\lmuffb.iss
```
; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "LMUFFB"
#define MyAppVersion "1.0"
#define MyAppPublisher "LMUFFB Community"
#define MyAppURL "https://github.com/your-repo/LMUFFB"
#define MyAppExeName "LMUFFB.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{A1B2C3D4-E5F6-7890-ABCD-EF1234567890}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
; Remove the following line to run in administrative install mode (install for all users.)
PrivilegesRequired=lowest
OutputDir=Output
OutputBaseFilename=LMUFFB_Setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
; The Main Executable (Must be built first)
Source: "..\build\Release\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
; vJoy Interface DLL (Must be present in build folder)
Source: "..\build\Release\vJoyInterface.dll"; DestDir: "{app}"; Flags: ignoreversion
; The rFactor 2 Plugin (Included in repo or build) - Optional
Source: "..\\plugins\\rFactor2SharedMemoryMapPlugin64.dll"; DestDir: "{app}\\PluginBackup"; Flags: ignoreversion external skipifsourcedoesntexist

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Code]
// Helper to check if vJoy is installed
function IsVJoyInstalled: Boolean;
begin
  // Check typical registry key for vJoy (Registry check logic varies by version)
  // vJoy 2.x usually resides in HKLM
  Result := RegKeyExists(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\vJoy');
end;

// Function to find LMU Installation
function FindLMUInstallPath(var Path: String): Boolean;
begin
  // Look for Steam install path in Registry
  if RegQueryStringValue(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Steam App 2399420', 'InstallLocation', Path) then
  begin
    Result := True;
  end
  else
  begin
    Result := False;
  end;
end;

procedure CurStepChanged(CurStep: TSetupStep);
var
  LMUPath: String;
  TargetPluginPath: String;
  SourcePluginPath: String;
  ResultCode: Integer;
begin
  if CurStep = ssPostInstall then
  begin
    // 1. Check vJoy
    if not IsVJoyInstalled then
    begin
      if MsgBox('vJoy does not appear to be installed. LMUFFB requires vJoy to function. Do you want to open the download page?', mbConfirmation, MB_YESNO) = IDYES then
      begin
        ShellExec('open', 'https://github.com/shauleiz/vJoy/releases', '', '', SW_SHOWNORMAL, ewNoWait, ResultCode);
      end;
    end;

    // 2. Auto-Install Plugin
    if FindLMUInstallPath(LMUPath) then
    begin
       TargetPluginPath := LMUPath + '\Plugins\rFactor2SharedMemoryMapPlugin64.dll';
       SourcePluginPath := ExpandConstant('{app}\PluginBackup\rFactor2SharedMemoryMapPlugin64.dll');
       
       if not FileExists(TargetPluginPath) then
       begin
         if MsgBox('Do you want to automatically install the Shared Memory Plugin to Le Mans Ultimate?' + #13#10 + 'Found path: ' + LMUPath, mbConfirmation, MB_YESNO) = IDYES then
         begin
           if FileCopy(SourcePluginPath, TargetPluginPath, False) then
             MsgBox('Plugin installed successfully.', mbInformation, MB_OK)
           else
             MsgBox('Failed to copy plugin. You may need to copy it manually from the installation folder.', mbError, MB_OK);
         end;
       end;
    end;
  end;
end;

```

# File: licenses\vJoy_LICENSE.txt
```
MIT License

Copyright (c) 2017 Shaul Eizikovich

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

# File: rust_port\README.md
```markdown
# LMUFFB Rust Port

This directory is reserved for the future Rust implementation of the application.

Please refer to `docs/porting_guide_rust.md` in the root directory for instructions on how to proceed with this port.

```

# File: scripts\create_context.py
```python
import os

OUTPUT_FILE = "FULL_PROJECT_CONTEXT.md"

# Extensions to include
EXTENSIONS = {
    '.cpp', '.h', '.c', '.hpp', 
    '.md', '.txt', 
    '.iss', '.cmake', 'CMakeLists.txt',
    '.py'
}

# Directories to exclude
EXCLUDE_DIRS = {
    'build', '.git', 'python_prototype', 'vendor', '__pycache__', '.vscode', '.specstory'
}

# Files to exclude
EXCLUDE_FILES = {
    OUTPUT_FILE, 'LICENSE', 'VERSION', 'compile_commands.json'
}

def is_text_file(filename):
    return any(filename.endswith(ext) or filename == ext for ext in EXTENSIONS)

def main():
    root_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    output_dir = os.path.join(root_dir, 'docs', 'dev_docs')
    os.makedirs(output_dir, exist_ok=True)
    output_path = os.path.join(output_dir, OUTPUT_FILE)

    with open(output_path, 'w', encoding='utf-8') as outfile:
        outfile.write("# LMUFFB Project Context\n\n")
        outfile.write("This file contains the full source code and documentation of the project.\n\n")

        for dirpath, dirnames, filenames in os.walk(root_dir):
            # Modify dirnames in-place to filter directories
            dirnames[:] = [d for d in dirnames if d not in EXCLUDE_DIRS]

            for filename in filenames:
                if filename in EXCLUDE_FILES:
                    continue
                
                if not is_text_file(filename):
                    continue

                filepath = os.path.join(dirpath, filename)
                relpath = os.path.relpath(filepath, root_dir)

                print(f"Adding {relpath}...")

                outfile.write(f"\n# File: {relpath}\n")
                
                # Determine language for fencing
                ext = os.path.splitext(filename)[1].lower()
                lang = ""
                if ext in ['.cpp', '.h', '.c']: lang = "cpp"
                elif ext == '.py': lang = "python"
                elif ext == '.md': lang = "markdown"
                elif ext == '.cmake' or filename == 'CMakeLists.txt': lang = "cmake"
                
                outfile.write(f"```{lang}\n")
                
                try:
                    with open(filepath, 'r', encoding='utf-8', errors='ignore') as infile:
                        outfile.write(infile.read())
                except Exception as e:
                    outfile.write(f"Error reading file: {e}")
                
                outfile.write("\n```\n")

    print(f"\nContext file generated: {output_path}")

if __name__ == "__main__":
    main()

```

# File: src\Config.cpp
```cpp
#include "Config.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool Config::m_ignore_vjoy_version_warning = false;
bool Config::m_output_ffb_to_vjoy = false; // Disabled by default (Safety)

void Config::Save(const FFBEngine& engine, const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "ignore_vjoy_version_warning=" << m_ignore_vjoy_version_warning << "\n";
        file << "output_ffb_to_vjoy=" << m_output_ffb_to_vjoy << "\n";
        file << "gain=" << engine.m_gain << "\n";
        file << "sop_smoothing_factor=" << engine.m_sop_smoothing_factor << "\n";
        file << "sop_scale=" << engine.m_sop_scale << "\n";
        file << "max_load_factor=" << engine.m_max_load_factor << "\n";
        file << "understeer=" << engine.m_understeer_effect << "\n";
        file << "sop=" << engine.m_sop_effect << "\n";
        file << "min_force=" << engine.m_min_force << "\n";
        file << "oversteer_boost=" << engine.m_oversteer_boost << "\n";
        file << "lockup_enabled=" << engine.m_lockup_enabled << "\n";
        file << "lockup_gain=" << engine.m_lockup_gain << "\n";
        file << "spin_enabled=" << engine.m_spin_enabled << "\n";
        file << "spin_gain=" << engine.m_spin_gain << "\n";
        file << "slide_enabled=" << engine.m_slide_texture_enabled << "\n";
        file << "slide_gain=" << engine.m_slide_texture_gain << "\n";
        file << "road_enabled=" << engine.m_road_texture_enabled << "\n";
        file << "road_gain=" << engine.m_road_texture_gain << "\n";
        file.close();
        std::cout << "[Config] Saved to " << filename << std::endl;
    } else {
        std::cerr << "[Config] Failed to save to " << filename << std::endl;
    }
}

void Config::Load(FFBEngine& engine, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "[Config] No config found, using defaults." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, '=')) {
            std::string value;
            if (std::getline(is_line, value)) {
                try {
                    if (key == "ignore_vjoy_version_warning") m_ignore_vjoy_version_warning = std::stoi(value);
                    else if (key == "output_ffb_to_vjoy") m_output_ffb_to_vjoy = std::stoi(value);
                    else if (key == "gain") engine.m_gain = std::stof(value);
                    else if (key == "sop_smoothing_factor") engine.m_sop_smoothing_factor = std::stof(value);
                    else if (key == "sop_scale") engine.m_sop_scale = std::stof(value);
                    else if (key == "max_load_factor") engine.m_max_load_factor = std::stof(value);
                    else if (key == "smoothing") engine.m_sop_smoothing_factor = std::stof(value); // Legacy support
                    else if (key == "understeer") engine.m_understeer_effect = std::stof(value);
                    else if (key == "sop") engine.m_sop_effect = std::stof(value);
                    else if (key == "min_force") engine.m_min_force = std::stof(value);
                    else if (key == "oversteer_boost") engine.m_oversteer_boost = std::stof(value);
                    else if (key == "lockup_enabled") engine.m_lockup_enabled = std::stoi(value);
                    else if (key == "lockup_gain") engine.m_lockup_gain = std::stof(value);
                    else if (key == "spin_enabled") engine.m_spin_enabled = std::stoi(value);
                    else if (key == "spin_gain") engine.m_spin_gain = std::stof(value);
                    else if (key == "oversteer_boost") engine.m_oversteer_boost = std::stof(value);
                    else if (key == "lockup_enabled") engine.m_lockup_enabled = std::stoi(value);
                    else if (key == "lockup_gain") engine.m_lockup_gain = std::stof(value);
                    else if (key == "spin_enabled") engine.m_spin_enabled = std::stoi(value);
                    else if (key == "spin_gain") engine.m_spin_gain = std::stof(value);
                    else if (key == "slide_enabled") engine.m_slide_texture_enabled = std::stoi(value);
                    else if (key == "slide_gain") engine.m_slide_texture_gain = std::stof(value);
                    else if (key == "road_enabled") engine.m_road_texture_enabled = std::stoi(value);
                    else if (key == "road_gain") engine.m_road_texture_gain = std::stof(value);
                } catch (...) {
                    std::cerr << "[Config] Error parsing line: " << line << std::endl;
                }
            }
        }
    }
    std::cout << "[Config] Loaded from " << filename << std::endl;
}

```

# File: src\Config.h
```cpp
#ifndef CONFIG_H
#define CONFIG_H

#include "../FFBEngine.h"
#include <string>

class Config {
public:
    static void Save(const FFBEngine& engine, const std::string& filename = "config.ini");
    static void Load(FFBEngine& engine, const std::string& filename = "config.ini");

    // Global App Settings (not part of FFB Physics)
    static bool m_ignore_vjoy_version_warning;
    static bool m_output_ffb_to_vjoy; // Output FFB signal to vJoy Axis X (Monitor)
};

#endif

```

# File: src\DirectInputFFB.cpp
```cpp
#include "DirectInputFFB.h"
#include <iostream>
#include <cmath>

#ifdef _WIN32
#include <dinput.h>
#endif

DirectInputFFB& DirectInputFFB::Get() {
    static DirectInputFFB instance;
    return instance;
}

DirectInputFFB::DirectInputFFB() {}

DirectInputFFB::~DirectInputFFB() {
    Shutdown();
}

bool DirectInputFFB::Initialize(HWND hwnd) {
    m_hwnd = hwnd;
#ifdef _WIN32
    if (FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDI, NULL))) {
        std::cerr << "[DI] Failed to create DirectInput8 interface." << std::endl;
        return false;
    }
    std::cout << "[DI] Initialized." << std::endl;
    return true;
#else
    std::cout << "[DI] Mock Initialized (Non-Windows)." << std::endl;
    return true;
#endif
}

void DirectInputFFB::Shutdown() {
#ifdef _WIN32
    if (m_pEffect) {
        m_pEffect->Unload();
        m_pEffect->Release();
        m_pEffect = nullptr;
    }
    if (m_pDevice) {
        m_pDevice->Unacquire();
        m_pDevice->Release();
        m_pDevice = nullptr;
    }
    if (m_pDI) {
        m_pDI->Release();
        m_pDI = nullptr;
    }
#endif
    m_active = false;
}

// Callback for enumeration
#ifdef _WIN32
BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) {
    auto* devices = (std::vector<DeviceInfo>*)pContext;
    DeviceInfo info;
    info.guid = pdidInstance->guidInstance;
    // Convert WCHAR name to string (simplified)
    char name[260];
    WideCharToMultiByte(CP_ACP, 0, pdidInstance->tszProductName, -1, name, 260, NULL, NULL);
    info.name = std::string(name);
    devices->push_back(info);
    return DIENUM_CONTINUE;
}
#endif

std::vector<DeviceInfo> DirectInputFFB::EnumerateDevices() {
    std::vector<DeviceInfo> devices;
#ifdef _WIN32
    if (!m_pDI) return devices;
    m_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, &devices, DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK);
#else
    // Mock devices
    DeviceInfo d1; d1.name = "Simucube 2 Pro (Mock)";
    DeviceInfo d2; d2.name = "Logitech G29 (Mock)";
    devices.push_back(d1);
    devices.push_back(d2);
#endif
    return devices;
}

bool DirectInputFFB::SelectDevice(const GUID& guid) {
#ifdef _WIN32
    if (!m_pDI) return false;

    // Cleanup old
    if (m_pDevice) {
        m_pDevice->Unacquire();
        m_pDevice->Release();
        m_pDevice = nullptr;
    }

    if (FAILED(m_pDI->CreateDevice(guid, &m_pDevice, NULL))) {
        std::cerr << "[DI] Failed to create device." << std::endl;
        return false;
    }

    if (FAILED(m_pDevice->SetDataFormat(&c_dfDIJoystick))) {
        std::cerr << "[DI] Failed to set data format." << std::endl;
        return false;
    }

    // Exclusive/Background is typically required for FFB
    if (FAILED(m_pDevice->SetCooperativeLevel(m_hwnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND))) {
        std::cerr << "[DI] Failed to set cooperative level." << std::endl;
        return false;
    }

    if (FAILED(m_pDevice->Acquire())) {
        std::cerr << "[DI] Failed to acquire device." << std::endl;
        // Don't return false yet, might just need focus/retry
    }

    // Create Effect
    if (CreateEffect()) {
        m_active = true;

        // Find the name again for the log (optional, or store it earlier)
        std::cout << "[DI] SUCCESS: Physical Device acquired and FFB Effect created." << std::endl; 
        // ------------------------------

        // Need to store name? We had it in enumeration.
        return true;
    }
    return false;
#else
    m_active = true;
    m_deviceName = "Mock Device Selected";
    return true;
#endif
}

bool DirectInputFFB::CreateEffect() {
#ifdef _WIN32
    if (!m_pDevice) return false;

    DWORD rgdwAxes[1] = { DIJOFS_X };
    LONG rglDirection[1] = { 0 };
    DICONSTANTFORCE cf;
    cf.lMagnitude = 0;

    DIEFFECT eff;
    ZeroMemory(&eff, sizeof(eff));
    eff.dwSize = sizeof(DIEFFECT);
    eff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
    eff.dwDuration = INFINITE;
    eff.dwSamplePeriod = 0;
    eff.dwGain = DI_FFNOMINALMAX;
    eff.dwTriggerButton = DIEB_NOTRIGGER;
    eff.dwTriggerRepeatInterval = 0;
    eff.cAxes = 1;
    eff.rgdwAxes = rgdwAxes;
    eff.rglDirection = rglDirection;
    eff.lpEnvelope = NULL;
    eff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
    eff.lpvTypeSpecificParams = &cf;
    eff.dwStartDelay = 0;

    if (FAILED(m_pDevice->CreateEffect(GUID_ConstantForce, &eff, &m_pEffect, NULL))) {
        std::cerr << "[DI] Failed to create Constant Force effect." << std::endl;
        return false;
    }
    
    // Start immediately
    m_pEffect->Start(1, 0);
    return true;
#endif
    return true;
}

void DirectInputFFB::UpdateForce(double normalizedForce) {
    if (!m_active) return;

    // Clamp
    normalizedForce = (std::max)(-1.0, (std::min)(1.0, normalizedForce));

    // Scale to -10000..10000
    long magnitude = static_cast<long>(normalizedForce * 10000.0);

    // Optimization: Don't call driver if value hasn't changed
    if (magnitude == m_last_force) return;
    m_last_force = magnitude;

#ifdef _WIN32
    if (m_pEffect) {
        DICONSTANTFORCE cf;
        cf.lMagnitude = magnitude;
        DIEFFECT eff;
        ZeroMemory(&eff, sizeof(eff));
        eff.dwSize = sizeof(DIEFFECT);
        eff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
        eff.lpvTypeSpecificParams = &cf;
        
        // Only update parameters. 
        // DO NOT pass DIEP_START here as it restarts the envelope and can cause clicks/latency.
        // Using DIEP_START in SetParameters effectively restarts the effect.
        HRESULT hr = m_pEffect->SetParameters(&eff, DIEP_TYPESPECIFICPARAMS);
        if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
            // Try to re-acquire once
            HRESULT hrAcq = m_pDevice->Acquire();
            if (SUCCEEDED(hrAcq)) {
                m_pEffect->SetParameters(&eff, DIEP_TYPESPECIFICPARAMS);
            } else if (hrAcq == DIERR_OTHERAPPHASPRIO) {
                static int log_limit = 0;
                if (log_limit++ % 400 == 0) { // Log once per sec approx
                    std::cerr << "[DI Warning] Device unavailable. LMU (or another app) has Exclusive Priority. " 
                              << "You may have a 'Double FFB' conflict." << std::endl;
                }
            }
        }
    }
#else
    // std::cout << "[DI Mock] Force: " << magnitude << std::endl;
#endif
}

```

# File: src\DirectInputFFB.h
```cpp
#ifndef DIRECTINPUTFFB_H
#define DIRECTINPUTFFB_H

#include <vector>
#include <string>
#include <atomic>

#ifdef _WIN32
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#else
// Mock types for non-Windows build/test
typedef void* HWND;
typedef void* LPDIRECTINPUT8;
typedef void* LPDIRECTINPUTDEVICE8;
typedef void* LPDIRECTINPUTEFFECT;
struct GUID { unsigned long Data1; unsigned short Data2; unsigned short Data3; unsigned char Data4[8]; };
#endif

struct DeviceInfo {
    GUID guid;
    std::string name;
};

class DirectInputFFB {
public:
    static DirectInputFFB& Get();

    bool Initialize(HWND hwnd);
    void Shutdown();

    // Returns a list of FFB-capable devices
    std::vector<DeviceInfo> EnumerateDevices();

    // Select and Acquire a device
    bool SelectDevice(const GUID& guid);
    
    // Update the Constant Force effect (-1.0 to 1.0)
    void UpdateForce(double normalizedForce);

    bool IsActive() const { return m_active; }
    std::string GetCurrentDeviceName() const { return m_deviceName; }

private:
    DirectInputFFB();
    ~DirectInputFFB();

    LPDIRECTINPUT8 m_pDI = nullptr;
    LPDIRECTINPUTDEVICE8 m_pDevice = nullptr;
    LPDIRECTINPUTEFFECT m_pEffect = nullptr;
    HWND m_hwnd = nullptr;
    
    bool m_active = false;
    std::string m_deviceName = "None";
    
    // Internal helper to create the Constant Force effect
    bool CreateEffect();

    long m_last_force = -999999; 
};

#endif // DIRECTINPUTFFB_H

```

# File: src\DynamicVJoy.h
```cpp
#ifndef DYNAMICVJOY_H
#define DYNAMICVJOY_H

#include <windows.h>
#include <iostream>

// vJoy Status Enum (from vJoy SDK, defined here to avoid dependency)
enum VjdStat {
    VJD_STAT_OWN,   // The vJoy Device is owned by this application
    VJD_STAT_FREE,  // The vJoy Device is free
    VJD_STAT_BUSY,  // The vJoy Device is owned by another application
    VJD_STAT_MISS,  // The vJoy Device is missing
    VJD_STAT_UNKN   // Unknown
};

// Typedefs for vJoy functions
typedef BOOL (WINAPI *vJoyEnabled_t)();
typedef BOOL (WINAPI *AcquireVJD_t)(UINT);
typedef VOID (WINAPI *RelinquishVJD_t)(UINT);
typedef BOOL (WINAPI *SetAxis_t)(LONG, UINT, UINT);
typedef enum VjdStat (WINAPI *GetVJDStatus_t)(UINT);
typedef SHORT (WINAPI *GetvJoyVersion_t)();
typedef PVOID (WINAPI *GetvJoyProductString_t)();
typedef PVOID (WINAPI *GetvJoyManufacturerString_t)();
typedef PVOID (WINAPI *GetvJoySerialNumberString_t)();

class DynamicVJoy {
public:
    static DynamicVJoy& Get() {
        static DynamicVJoy instance;
        return instance;
    }

    bool Load() {
        if (m_hModule) return true; // Already loaded

        m_hModule = LoadLibraryA("vJoyInterface.dll");
        if (!m_hModule) {
            std::cout << "[vJoy] Library not found. vJoy support disabled." << std::endl;
            return false;
        }

        m_vJoyEnabled = (vJoyEnabled_t)GetProcAddress(m_hModule, "vJoyEnabled");
        m_AcquireVJD = (AcquireVJD_t)GetProcAddress(m_hModule, "AcquireVJD");
        m_RelinquishVJD = (RelinquishVJD_t)GetProcAddress(m_hModule, "RelinquishVJD");
        m_SetAxis = (SetAxis_t)GetProcAddress(m_hModule, "SetAxis");
        m_GetVJDStatus = (GetVJDStatus_t)GetProcAddress(m_hModule, "GetVJDStatus");
        m_GetvJoyVersion = (GetvJoyVersion_t)GetProcAddress(m_hModule, "GetvJoyVersion");
        m_GetvJoyProductString = (GetvJoyProductString_t)GetProcAddress(m_hModule, "GetvJoyProductString");
        m_GetvJoyManufacturerString = (GetvJoyManufacturerString_t)GetProcAddress(m_hModule, "GetvJoyManufacturerString");
        m_GetvJoySerialNumberString = (GetvJoySerialNumberString_t)GetProcAddress(m_hModule, "GetvJoySerialNumberString");

        if (!m_vJoyEnabled || !m_AcquireVJD || !m_RelinquishVJD || !m_SetAxis || !m_GetVJDStatus) {
            std::cerr << "[vJoy] Library loaded but functions missing." << std::endl;
            FreeLibrary(m_hModule);
            m_hModule = NULL;
            return false;
        }

        std::cout << "[vJoy] Library loaded successfully." << std::endl;
        return true;
    }

    bool Enabled() { return (m_hModule && m_vJoyEnabled) ? m_vJoyEnabled() : false; }
    BOOL Acquire(UINT id) { return (m_hModule && m_AcquireVJD) ? m_AcquireVJD(id) : FALSE; }
    VOID Relinquish(UINT id) { if (m_hModule && m_RelinquishVJD) m_RelinquishVJD(id); }
    BOOL SetAxis(LONG value, UINT id, UINT axis) { return (m_hModule && m_SetAxis) ? m_SetAxis(value, id, axis) : FALSE; }
    VjdStat GetStatus(UINT id) { return (m_hModule && m_GetVJDStatus) ? m_GetVJDStatus(id) : VJD_STAT_MISS; }
    
    SHORT GetVersion() { return (m_hModule && m_GetvJoyVersion) ? m_GetvJoyVersion() : 0; }
    const char* GetManufacturerString() { return (m_hModule && m_GetvJoyManufacturerString) ? (const char*)m_GetvJoyManufacturerString() : ""; }
    const char* GetProductString() { return (m_hModule && m_GetvJoyProductString) ? (const char*)m_GetvJoyProductString() : ""; }
    const char* GetSerialNumberString() { return (m_hModule && m_GetvJoySerialNumberString) ? (const char*)m_GetvJoySerialNumberString() : ""; }

    bool IsLoaded() const { return m_hModule != NULL; }

private:
    DynamicVJoy() {}
    ~DynamicVJoy() {
        if (m_hModule) FreeLibrary(m_hModule);
    }

    HMODULE m_hModule = NULL;
    vJoyEnabled_t m_vJoyEnabled = NULL;
    AcquireVJD_t m_AcquireVJD = NULL;
    RelinquishVJD_t m_RelinquishVJD = NULL;
    SetAxis_t m_SetAxis = NULL;
    GetVJDStatus_t m_GetVJDStatus = NULL;
    GetvJoyVersion_t m_GetvJoyVersion = NULL;
    GetvJoyProductString_t m_GetvJoyProductString = NULL;
    GetvJoyManufacturerString_t m_GetvJoyManufacturerString = NULL;
    GetvJoySerialNumberString_t m_GetvJoySerialNumberString = NULL;
};

#endif // DYNAMICVJOY_H

```

# File: src\GuiLayer.cpp
```cpp
#include "GuiLayer.h"
#include "Config.h"
#include "DirectInputFFB.h"
#include <iostream>
#include <vector>
#include <mutex>

#ifdef ENABLE_IMGUI
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>

// Global DirectX variables (Simplified for brevity, usually managed in a separate backend class)
static ID3D11Device*            g_pd3dDevice = NULL;
static ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D11RenderTargetView*  g_mainRenderTargetView = NULL;
static HWND                     g_hwnd = NULL;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// External linkage to FFB loop status
extern std::atomic<bool> g_running;
extern std::mutex g_engine_mutex;

// Macro stringification helper
#define XSTR(x) STR(x)
#define STR(x) #x

// If VERSION is not defined via CMake, default
#ifndef LMUFFB_VERSION
#define LMUFFB_VERSION "Dev"
#endif

bool GuiLayer::Init() {
    // Create Application Window
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"LMUFFB", NULL };
    ::RegisterClassExW(&wc);
    
    // Construct Title with Version
    // We need wide string for CreateWindowW. 
    // Simplified conversion for version string (assumes ASCII version)
    std::string ver = LMUFFB_VERSION;
    std::wstring wver(ver.begin(), ver.end());
    std::wstring title = L"LMUFFB v" + wver;

    g_hwnd = ::CreateWindowW(wc.lpszClassName, title.c_str(), WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(g_hwnd)) {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return false;
    }

    // Show the window
    ::ShowWindow(g_hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(g_hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(g_hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    return true;
}

void GuiLayer::Shutdown() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(g_hwnd);
    ::UnregisterClassW(L"LMUFFB", GetModuleHandle(NULL));
}

void* GuiLayer::GetWindowHandle() {
    return (void*)g_hwnd;
}

bool GuiLayer::Render(FFBEngine& engine) {
    // Handle Windows messages
    MSG msg;
    while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
        if (msg.message == WM_QUIT) {
            g_running = false;
            return false;
        }
    }
    
    // If minimized, sleep to save CPU (Lazy Rendering)
    // Note: In a real app we'd check IsIconic(g_hwnd) outside this logic or return a 'should_sleep' flag
    if (g_running == false) return false;

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Draw Tuning Window
    DrawTuningWindow(engine);
    
    // Draw Debug Window (if enabled)
    if (m_show_debug_window) {
        DrawDebugWindow(engine);
    }

    // Rendering
    ImGui::Render();
    const float clear_color_with_alpha[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    g_pSwapChain->Present(1, 0); // Present with vsync

    // Return focus state for lazy rendering optimization
    // Typically, if ImGui::IsAnyItemActive() is true, we want fast updates
    return ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow) || ImGui::IsAnyItemActive();
}

void GuiLayer::DrawTuningWindow(FFBEngine& engine) {
    // LOCK MUTEX to prevent race condition with FFB Thread
    std::lock_guard<std::mutex> lock(g_engine_mutex);

    // Show Version in title bar or top text
    std::string title = std::string("LMUFFB v") + LMUFFB_VERSION + " - FFB Configuration";
    ImGui::Begin(title.c_str());

    ImGui::Text("Core Settings");
    
    // Device Selection
    static std::vector<DeviceInfo> devices;
    static int selected_device_idx = -1;
    
    // Scan button (or auto scan once)
    if (devices.empty()) {
        devices = DirectInputFFB::Get().EnumerateDevices();
    }

    if (ImGui::BeginCombo("FFB Device", selected_device_idx >= 0 ? devices[selected_device_idx].name.c_str() : "Select Device...")) {
        for (int i = 0; i < devices.size(); i++) {
            bool is_selected = (selected_device_idx == i);
            if (ImGui::Selectable(devices[i].name.c_str(), is_selected)) {
                selected_device_idx = i;
                DirectInputFFB::Get().SelectDevice(devices[i].guid);
            }
            if (is_selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    if (ImGui::Button("Rescan Devices")) {
        devices = DirectInputFFB::Get().EnumerateDevices();
        selected_device_idx = -1;
    }

    ImGui::Separator();
    
    ImGui::SliderFloat("Master Gain", &engine.m_gain, 0.0f, 2.0f, "%.2f");
    ImGui::SliderFloat("Min Force", &engine.m_min_force, 0.0f, 0.20f, "%.3f");

    if (ImGui::TreeNode("Advanced Tuning")) {
        ImGui::SliderFloat("SoP Smoothing", &engine.m_sop_smoothing_factor, 0.0f, 1.0f, "%.2f (1=Raw)");
        ImGui::SliderFloat("SoP Scale", &engine.m_sop_scale, 100.0f, 5000.0f, "%.0f");
        ImGui::SliderFloat("Load Cap", &engine.m_max_load_factor, 1.0f, 3.0f, "%.1fx");
        ImGui::TreePop();
    }

    ImGui::Separator();
    ImGui::Text("Effects");
    ImGui::SliderFloat("Understeer (Grip)", &engine.m_understeer_effect, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("SoP (Lateral G)", &engine.m_sop_effect, 0.0f, 2.0f, "%.2f");
    ImGui::SliderFloat("Oversteer Boost", &engine.m_oversteer_boost, 0.0f, 1.0f, "%.2f");

    ImGui::Separator();
    ImGui::Text("Haptics (Dynamic)");
    ImGui::Checkbox("Progressive Lockup", &engine.m_lockup_enabled);
    if (engine.m_lockup_enabled) {
        ImGui::SameLine(); ImGui::SliderFloat("##Lockup", &engine.m_lockup_gain, 0.0f, 1.0f, "Gain: %.2f");
    }
    
    ImGui::Checkbox("Spin Traction Loss", &engine.m_spin_enabled);
    if (engine.m_spin_enabled) {
        ImGui::SameLine(); ImGui::SliderFloat("##Spin", &engine.m_spin_gain, 0.0f, 1.0f, "Gain: %.2f");
    }

    ImGui::Separator();
    ImGui::Text("Textures");
    ImGui::Checkbox("Slide Rumble", &engine.m_slide_texture_enabled);
    if (engine.m_slide_texture_enabled) {
        ImGui::Indent();
        ImGui::SliderFloat("Slide Gain", &engine.m_slide_texture_gain, 0.0f, 2.0f);
        ImGui::Unindent();
    }
    ImGui::Checkbox("Road Details", &engine.m_road_texture_enabled);
    if (engine.m_road_texture_enabled) {
        ImGui::Indent();
        ImGui::SliderFloat("Road Gain", &engine.m_road_texture_gain, 0.0f, 5.0f);
        ImGui::Unindent();
    }

    ImGui::Separator();
    ImGui::Text("Output");
    
    // vJoy Monitoring (Safety critical)
    if (ImGui::Checkbox("Enable vJoy Output (Monitor)", &Config::m_output_ffb_to_vjoy)) {
        // Warn user if enabling
        if (Config::m_output_ffb_to_vjoy) {
            MessageBoxA(NULL, "WARNING: Enabling this will output the FFB signal to vJoy Axis X.\n\n"
                              "If you have bound Game Steering to vJoy Axis X, this will cause a Feedback Loop (Wheel Spinning).\n"
                              "Only enable this if you are NOT using vJoy Axis X for steering.", 
                              "Safety Warning", MB_ICONWARNING | MB_OK);
        }
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Acquires the vJoy device and outputs FFB force to Axis X.\nREQUIRED if you want to see forces in vJoy Monitor.\nDISABLE if you use Joystick Gremlin (Gremlin needs exclusive access).");

    // Visualize Clipping (this requires the calculated force from the engine passed back, 
    // or we just show the static gain for now. A real app needs a shared state for 'last_output_force')
    // ImGui::ProgressBar((float)engine.last_force, ImVec2(0.0f, 0.0f)); 
    ImGui::Text("Clipping Visualization Placeholder");

    ImGui::Separator();
    if (ImGui::Button("Save Configuration")) {
        Config::Save(engine);
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("Show Troubleshooting Graphs", &m_show_debug_window)) {
        // Just toggles window
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Reset Defaults")) {
        // Reset Logic (Updated v0.3.13)
        engine.m_gain = 0.5f;
        engine.m_understeer_effect = 1.0f;
        engine.m_sop_effect = 0.15f;
        engine.m_min_force = 0.0f;
        engine.m_sop_smoothing_factor = 0.05f;
        engine.m_max_load_factor = 1.5f;
        engine.m_oversteer_boost = 0.0f;
        engine.m_lockup_enabled = false;
        engine.m_lockup_gain = 0.5f;
        engine.m_spin_enabled = false;
        engine.m_spin_gain = 0.5f;
        engine.m_slide_texture_enabled = true;
        engine.m_slide_texture_gain = 0.5f;
        engine.m_road_texture_enabled = false;
        engine.m_road_texture_gain = 0.5f;
    }

    ImGui::End();
}

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

// Helper functions for D3D (boilerplate)
bool CreateDeviceD3D(HWND hWnd) {
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D() {
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget() {
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

#else
// Stub Implementation for Headless Builds
bool GuiLayer::Init() { 
    std::cout << "[GUI] Disabled (Headless Mode)" << std::endl; 
    return true; 
}
void GuiLayer::Shutdown() {}
bool GuiLayer::Render(FFBEngine& engine) { return false; } // Always lazy
#endif

// --- Helper: Ring Buffer for PlotLines ---
struct RollingBuffer {
    std::vector<float> data;
    int offset = 0;
    
    RollingBuffer(int size = 2000) {
        data.resize(size, 0.0f);
    }
    
    void Add(float val) {
        data[offset] = val;
        offset = (offset + 1) % data.size();
    }
};

// Static buffers for debug plots (FFB)
static RollingBuffer plot_total(1000);
static RollingBuffer plot_base(1000);
static RollingBuffer plot_sop(1000);
static RollingBuffer plot_understeer(1000);
static RollingBuffer plot_oversteer(1000);
static RollingBuffer plot_road(1000);
static RollingBuffer plot_slide(1000);
static RollingBuffer plot_lockup(1000);
static RollingBuffer plot_spin(1000);
static RollingBuffer plot_bottoming(1000);
static RollingBuffer plot_clipping(1000);

// Static buffers for debug plots (Telemetry)
static RollingBuffer plot_input_steer(1000); // SteeringArmForce
static RollingBuffer plot_input_accel(1000); // LocalAccel.x
static RollingBuffer plot_input_load(1000); // TireLoad
static RollingBuffer plot_input_grip(1000); // GripFract
static RollingBuffer plot_input_slip_ratio(1000); // SlipRatio
static RollingBuffer plot_input_slip_angle(1000); // SlipAngle
static RollingBuffer plot_input_patch_vel(1000); // PatchVel
static RollingBuffer plot_input_vert_deflection(1000); // Deflection

// Toggle State
bool GuiLayer::m_show_debug_window = false;

void GuiLayer::DrawDebugWindow(FFBEngine& engine) {
    ImGui::Begin("FFB Analysis", &m_show_debug_window);
    
    // Update Buffers (Ideally this should be done in the FFB thread and passed via a queue,
    // but sampling at 60Hz from the latest state is "good enough" for visual inspection)
    // Note: This misses high-freq spikes between frames, but shows trends.
    // For Phase 3 (Logging), we need proper buffering.
    
    // --- Update Buffers ---
    // FFB
    plot_total.Add(engine.m_last_debug.total_output);
    plot_base.Add(engine.m_last_debug.base_force);
    plot_sop.Add(engine.m_last_debug.sop_force);
    plot_understeer.Add(engine.m_last_debug.understeer_drop);
    plot_oversteer.Add(engine.m_last_debug.oversteer_boost);
    plot_road.Add(engine.m_last_debug.texture_road);
    plot_slide.Add(engine.m_last_debug.texture_slide);
    plot_lockup.Add(engine.m_last_debug.texture_lockup);
    plot_spin.Add(engine.m_last_debug.texture_spin);
    plot_bottoming.Add(engine.m_last_debug.texture_bottoming);
    plot_clipping.Add(engine.m_last_debug.clipping);

    // Telemetry
    plot_input_steer.Add((float)engine.m_last_telemetry.mSteeringArmForce);
    plot_input_accel.Add((float)engine.m_last_telemetry.mLocalAccel.x);
    // Use MAX of FL/FR or Avg? Usually max is interesting for load, avg for others.
    // Let's use AVG for simplicity in single line.
    const rF2Wheel& fl = engine.m_last_telemetry.mWheels[0];
    const rF2Wheel& fr = engine.m_last_telemetry.mWheels[1];
    
    plot_input_load.Add((float)((fl.mTireLoad + fr.mTireLoad) / 2.0));
    plot_input_grip.Add((float)((fl.mGripFract + fr.mGripFract) / 2.0));
    plot_input_slip_ratio.Add((float)((fl.mSlipRatio + fr.mSlipRatio) / 2.0));
    plot_input_slip_angle.Add((float)((std::abs(fl.mSlipAngle) + std::abs(fr.mSlipAngle)) / 2.0));
    plot_input_patch_vel.Add((float)((std::abs(fl.mLateralPatchVel) + std::abs(fr.mLateralPatchVel)) / 2.0));
    plot_input_vert_deflection.Add((float)((fl.mVerticalTireDeflection + fr.mVerticalTireDeflection) / 2.0));
    
    // --- Draw UI ---
    if (ImGui::CollapsingHeader("FFB Components (Stack)", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Total Output");
        ImGui::PlotLines("##Total", plot_total.data.data(), (int)plot_total.data.size(), plot_total.offset, "Total", -1.0f, 1.0f, ImVec2(0, 60));
        
        ImGui::Columns(2, "FFBCols", false);
        ImGui::Text("Base Force"); ImGui::PlotLines("##Base", plot_base.data.data(), (int)plot_base.data.size(), plot_base.offset, NULL, -4000.0f, 4000.0f, ImVec2(0, 40));
        ImGui::NextColumn();
        ImGui::Text("SoP (Lat G)"); ImGui::PlotLines("##SoP", plot_sop.data.data(), (int)plot_sop.data.size(), plot_sop.offset, NULL, -1000.0f, 1000.0f, ImVec2(0, 40));
        ImGui::NextColumn();
        ImGui::Text("Understeer Cut"); ImGui::PlotLines("##Under", plot_understeer.data.data(), (int)plot_understeer.data.size(), plot_understeer.offset, NULL, -2000.0f, 2000.0f, ImVec2(0, 40));
        ImGui::NextColumn();
        ImGui::Text("Oversteer Boost"); ImGui::PlotLines("##Over", plot_oversteer.data.data(), (int)plot_oversteer.data.size(), plot_oversteer.offset, NULL, -500.0f, 500.0f, ImVec2(0, 40));
        ImGui::NextColumn();
        ImGui::Text("Road Texture"); ImGui::PlotLines("##Road", plot_road.data.data(), (int)plot_road.data.size(), plot_road.offset, NULL, -1000.0f, 1000.0f, ImVec2(0, 40));
        ImGui::NextColumn();
        ImGui::Text("Slide Texture"); ImGui::PlotLines("##Slide", plot_slide.data.data(), (int)plot_slide.data.size(), plot_slide.offset, NULL, -500.0f, 500.0f, ImVec2(0, 40));
        ImGui::NextColumn();
        ImGui::Text("Lockup Vib"); ImGui::PlotLines("##Lock", plot_lockup.data.data(), (int)plot_lockup.data.size(), plot_lockup.offset, NULL, -500.0f, 500.0f, ImVec2(0, 40));
        ImGui::NextColumn();
        ImGui::Text("Spin Vib"); ImGui::PlotLines("##Spin", plot_spin.data.data(), (int)plot_spin.data.size(), plot_spin.offset, NULL, -500.0f, 500.0f, ImVec2(0, 40));
        ImGui::NextColumn();
        ImGui::Text("Bottoming"); ImGui::PlotLines("##Bot", plot_bottoming.data.data(), (int)plot_bottoming.data.size(), plot_bottoming.offset, NULL, -1000.0f, 1000.0f, ImVec2(0, 40));
        ImGui::NextColumn();
        ImGui::Text("Clipping"); ImGui::PlotLines("##Clip", plot_clipping.data.data(), (int)plot_clipping.data.size(), plot_clipping.offset, NULL, 0.0f, 1.1f, ImVec2(0, 40));
        ImGui::Columns(1);
    }

    if (ImGui::CollapsingHeader("Telemetry Inspector (Raw)", ImGuiTreeNodeFlags_None)) {
        ImGui::Columns(2, "TelCols", false);
        ImGui::Text("Steering Force"); ImGui::PlotLines("##StForce", plot_input_steer.data.data(), (int)plot_input_steer.data.size(), plot_input_steer.offset, NULL, -5000.0f, 5000.0f, ImVec2(0, 40));
        ImGui::NextColumn();
        ImGui::Text("Local Accel X"); ImGui::PlotLines("##LatG", plot_input_accel.data.data(), (int)plot_input_accel.data.size(), plot_input_accel.offset, NULL, -20.0f, 20.0f, ImVec2(0, 40));
        ImGui::NextColumn();
        ImGui::Text("Avg Tire Load"); ImGui::PlotLines("##Load", plot_input_load.data.data(), (int)plot_input_load.data.size(), plot_input_load.offset, NULL, 0.0f, 10000.0f, ImVec2(0, 40));
        ImGui::NextColumn();
        ImGui::Text("Avg Grip Fract"); ImGui::PlotLines("##Grip", plot_input_grip.data.data(), (int)plot_input_grip.data.size(), plot_input_grip.offset, NULL, 0.0f, 1.2f, ImVec2(0, 40));
        ImGui::NextColumn();
        ImGui::Text("Avg Slip Ratio"); ImGui::PlotLines("##SlipR", plot_input_slip_ratio.data.data(), (int)plot_input_slip_ratio.data.size(), plot_input_slip_ratio.offset, NULL, -1.0f, 1.0f, ImVec2(0, 40));
        ImGui::NextColumn();
        ImGui::Text("Avg Slip Angle"); ImGui::PlotLines("##SlipA", plot_input_slip_angle.data.data(), (int)plot_input_slip_angle.data.size(), plot_input_slip_angle.offset, NULL, 0.0f, 1.0f, ImVec2(0, 40));
        ImGui::NextColumn();
        ImGui::Text("Avg Lat PatchVel"); ImGui::PlotLines("##PatchV", plot_input_patch_vel.data.data(), (int)plot_input_patch_vel.data.size(), plot_input_patch_vel.offset, NULL, 0.0f, 20.0f, ImVec2(0, 40));
        ImGui::NextColumn();
        ImGui::Text("Avg Deflection"); ImGui::PlotLines("##Defl", plot_input_vert_deflection.data.data(), (int)plot_input_vert_deflection.data.size(), plot_input_vert_deflection.offset, NULL, 0.0f, 0.1f, ImVec2(0, 40));
        ImGui::Columns(1);
    }

    ImGui::End();
}

```

# File: src\GuiLayer.h
```cpp
#ifndef GUILAYER_H
#define GUILAYER_H

#include "../FFBEngine.h"

// Forward declaration to avoid pulling in ImGui headers here if not needed
struct GuiContext; 

class GuiLayer {
public:
    static bool Init();
    static void Shutdown();
    
    static void* GetWindowHandle(); // Returns HWND

    // Returns true if the GUI is active/focused (affects lazy rendering)
    static bool Render(FFBEngine& engine);

private:
    static void DrawTuningWindow(FFBEngine& engine);
    static void DrawDebugWindow(FFBEngine& engine);
    
    // UI State
    static bool m_show_debug_window;
};

#endif // GUILAYER_H

```

# File: tests\CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.10)
project(LMUFFB_Tests)

set(CMAKE_CXX_STANDARD 17)

# Include main source dir for headers
include_directories(..)

# Test Executable
add_executable(run_tests test_ffb_engine.cpp)

# Enable testing
enable_testing()
add_test(NAME CoreLogicTest COMMAND run_tests)

```

# File: tests\test_ffb_engine.cpp
```cpp
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cstring>
#include "../FFBEngine.h"
#include "../rF2Data.h"

// --- Simple Test Framework ---
int g_tests_passed = 0;
int g_tests_failed = 0;

#define ASSERT_TRUE(condition) \
    if (condition) { \
        std::cout << "[PASS] " << #condition << std::endl; \
        g_tests_passed++; \
    } else { \
        std::cout << "[FAIL] " << #condition << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
        g_tests_failed++; \
    }

#define ASSERT_NEAR(a, b, epsilon) \
    if (std::abs((a) - (b)) < (epsilon)) { \
        std::cout << "[PASS] " << #a << " approx " << #b << std::endl; \
        g_tests_passed++; \
    } else { \
        std::cout << "[FAIL] " << #a << " (" << (a) << ") != " << #b << " (" << (b) << ")" << std::endl; \
        g_tests_failed++; \
    }

// --- Tests ---

void test_zero_input() {
    std::cout << "\nTest: Zero Input" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));
    
    // Set minimal grip to avoid divide by zero if any
    data.mWheels[0].mGripFract = 1.0;
    data.mWheels[1].mGripFract = 1.0;

    double force = engine.calculate_force(&data);
    ASSERT_NEAR(force, 0.0, 0.001);
}

void test_grip_modulation() {
    std::cout << "\nTest: Grip Modulation (Understeer)" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));
    
    // Set Gain to 1.0 for testing logic (default is now 0.5)
    engine.m_gain = 1.0; 

    data.mSteeringArmForce = 2000.0; // Half of max ~4000
    // Disable SoP and Texture to isolate
    engine.m_sop_effect = 0.0;
    engine.m_slide_texture_enabled = false;
    engine.m_road_texture_enabled = false;

    // Case 1: Full Grip (1.0) -> Output should be 2000 / 4000 = 0.5
    data.mWheels[0].mGripFract = 1.0;
    data.mWheels[1].mGripFract = 1.0;
    double force_full = engine.calculate_force(&data);
    ASSERT_NEAR(force_full, 0.5, 0.001);

    // Case 2: Half Grip (0.5) -> Output should be 2000 * 0.5 = 1000 / 4000 = 0.25
    data.mWheels[0].mGripFract = 0.5;
    data.mWheels[1].mGripFract = 0.5;
    double force_half = engine.calculate_force(&data);
    ASSERT_NEAR(force_half, 0.25, 0.001);
}

void test_sop_effect() {
    std::cout << "\nTest: SoP Effect" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));

    // Disable Game Force
    data.mSteeringArmForce = 0.0;
    engine.m_sop_effect = 0.5; 
    engine.m_gain = 1.0; // Ensure gain is 1.0
    
    // 0.5 G lateral (4.905 m/s2)
    data.mLocalAccel.x = 4.905;
    
    // Calculation: 
    // LatG = 4.905 / 9.81 = 0.5
    // SoP Force = 0.5 * 0.5 * 1000 = 250
    // Norm Force = 250 / 4000 = 0.0625
    
    // Run for multiple frames to let smoothing settle (alpha=0.1)
    double force = 0.0;
    for (int i=0; i<60; i++) {
        force = engine.calculate_force(&data);
    }

    ASSERT_NEAR(force, 0.0625, 0.001);
}

void test_min_force() {
    std::cout << "\nTest: Min Force" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));

    // Ensure we have minimal grip so calculation doesn't zero out somewhere else
    data.mWheels[0].mGripFract = 1.0;
    data.mWheels[1].mGripFract = 1.0;

    // Disable Noise/Textures to ensure they don't add random values
    engine.m_slide_texture_enabled = false;
    engine.m_road_texture_enabled = false;
    engine.m_sop_effect = 0.0;

    data.mSteeringArmForce = 10.0; // Very small force
    engine.m_min_force = 0.10; // 10% min force

    double force = engine.calculate_force(&data);
    // 10 / 4000 = 0.0025. 
    // This is > 0.0001 (deadzone check) but < 0.10.
    // Should be boosted to 0.10.
    
    // Debug print
    if (std::abs(force - 0.10) > 0.001) {
        std::cout << "Debug Min Force: Calculated " << force << " Expected 0.10" << std::endl;
    }
    
    ASSERT_NEAR(force, 0.10, 0.001);
}

void test_progressive_lockup() {
    std::cout << "\nTest: Progressive Lockup" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));
    
    engine.m_lockup_enabled = true;
    engine.m_lockup_gain = 1.0;
    engine.m_sop_effect = 0.0;
    engine.m_slide_texture_enabled = false;
    
    data.mSteeringArmForce = 0.0;
    data.mUnfilteredBrake = 1.0;
    
    // Set DeltaTime for phase integration
    data.mDeltaTime = 0.01;
    data.mLocalVel.z = 20.0; // 20 m/s
    
    // Case 1: Low Slip (-0.15). Severity = (0.15 - 0.1) / 0.4 = 0.125
    data.mWheels[0].mSlipRatio = -0.15;
    data.mWheels[1].mSlipRatio = -0.15;
    
    // Ensure data.mDeltaTime is set! 
    data.mDeltaTime = 0.01;
    
    // DEBUG: Manually verify phase logic in test
    // freq = 10 + (20 * 1.5) = 40.0
    // dt = 0.01
    // step = 40 * 0.01 * 6.28 = 2.512
    
    engine.calculate_force(&data); // Frame 1
    // engine.m_lockup_phase should be approx 2.512
    
    double force_low = engine.calculate_force(&data); // Frame 2
    // engine.m_lockup_phase should be approx 5.024
    // sin(5.024) is roughly -0.95.
    // Amp should be non-zero.
    
    // Debug
    // std::cout << "Force Low: " << force_low << " Phase: " << engine.m_lockup_phase << std::endl;

    if (engine.m_lockup_phase == 0.0) {
         // Maybe frequency calculation is zero?
         // Freq = 10 + (20 * 1.5) = 40.
         // Dt = 0.01.
         // Accumulator += 40 * 0.01 * 6.28 = 2.5.
         std::cout << "[FAIL] Phase stuck at 0. Check data inputs." << std::endl;
    }

    ASSERT_TRUE(std::abs(force_low) > 0.00001);
    ASSERT_TRUE(engine.m_lockup_phase != 0.0);
    
    std::cout << "[PASS] Progressive Lockup calculated." << std::endl;
    g_tests_passed++;
}

void test_slide_texture() {
    std::cout << "\nTest: Slide Texture" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));
    
    engine.m_slide_texture_enabled = true;
    engine.m_slide_texture_gain = 1.0;
    
    data.mSteeringArmForce = 0.0;
    data.mWheels[0].mSlipAngle = 0.2; // Slipping > 0.15
    data.mWheels[1].mSlipAngle = 0.2;
    data.mDeltaTime = 0.013; // Avoid 0.01 which lands exactly on zero-crossing for 125Hz
    data.mWheels[0].mLateralPatchVel = 5.0; // Use PatchVel as updated in engine
    data.mWheels[1].mLateralPatchVel = 5.0;
    data.mWheels[0].mTireLoad = 1000.0; // Some load
    data.mWheels[1].mTireLoad = 1000.0;
    
    // Run two frames to advance phase
    engine.calculate_force(&data);
    double force = engine.calculate_force(&data);
    
    // We just assert it's non-zero
    if (std::abs(force) > 0.00001) {
        std::cout << "[PASS] Slide texture generated non-zero force: " << force << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Slide texture force is zero" << std::endl;
        g_tests_failed++;
    }
}

void test_dynamic_tuning() {
    std::cout << "\nTest: Dynamic Tuning (GUI Simulation)" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));
    
    // Default State: Full Game Force
    data.mSteeringArmForce = 2000.0;
    data.mWheels[0].mGripFract = 1.0;
    data.mWheels[1].mGripFract = 1.0;
    engine.m_understeer_effect = 0.0; // Disabled effect initially
    engine.m_sop_effect = 0.0;
    engine.m_slide_texture_enabled = false;
    engine.m_road_texture_enabled = false;
    
    // Explicitly set gain 1.0 for this baseline
    engine.m_gain = 1.0;

    double force_initial = engine.calculate_force(&data);
    // Should pass through 2000.0 (normalized: 0.5)
    ASSERT_NEAR(force_initial, 0.5, 0.001);
    
    // --- User drags Master Gain Slider to 2.0 ---
    engine.m_gain = 2.0;
    double force_boosted = engine.calculate_force(&data);
    // Should be 0.5 * 2.0 = 1.0
    ASSERT_NEAR(force_boosted, 1.0, 0.001);
    
    // --- User enables Understeer Effect ---
    // And grip drops
    engine.m_gain = 1.0; // Reset gain
    engine.m_understeer_effect = 1.0;
    data.mWheels[0].mGripFract = 0.5;
    data.mWheels[1].mGripFract = 0.5;
    
    double force_grip_loss = engine.calculate_force(&data);
    // 2000 * 0.5 = 1000 -> 0.25 normalized
    ASSERT_NEAR(force_grip_loss, 0.25, 0.001);
    
    std::cout << "[PASS] Dynamic Tuning verified." << std::endl;
    g_tests_passed++;
}

void test_suspension_bottoming() {
    std::cout << "\nTest: Suspension Bottoming (Fix Verification)" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));

    // Enable Bottoming
    engine.m_bottoming_enabled = true;
    engine.m_bottoming_gain = 1.0;
    
    // Disable others
    engine.m_sop_effect = 0.0;
    engine.m_slide_texture_enabled = false;
    
    // Straight line condition: Zero steering force
    data.mSteeringArmForce = 0.0;
    
    // Massive Load Spike (10000N > 8000N threshold)
    data.mWheels[0].mTireLoad = 10000.0;
    data.mWheels[1].mTireLoad = 10000.0;
    data.mDeltaTime = 0.01;
    
    // Run multiple frames to check oscillation
    // Phase calculation: Freq=50. 50 * 0.01 * 2PI = 0.5 * 2PI = PI.
    // Frame 1: Phase = PI. Sin(PI) = 0. Force = 0.
    // Frame 2: Phase = 2PI (0). Sin(0) = 0. Force = 0.
    // Bad luck with 50Hz and 100Hz (0.01s).
    // Let's use dt = 0.005 (200Hz)
    data.mDeltaTime = 0.005; 
    
    // Frame 1: Phase += 50 * 0.005 * 2PI = 0.25 * 2PI = PI/2.
    // Sin(PI/2) = 1.0. 
    // Excess = 2000. Sqrt(2000) ~ 44.7. * 0.5 = 22.35.
    // Force should be approx +22.35 (normalized later by /4000)
    
    engine.calculate_force(&data); // Frame 1
    double force = engine.calculate_force(&data); // Frame 2 (Phase PI, sin 0?)
    
    // Let's check frame 1 explicitly by resetting
    FFBEngine engine2;
    engine2.m_bottoming_enabled = true;
    engine2.m_bottoming_gain = 1.0;
    engine2.m_sop_effect = 0.0;
    engine2.m_slide_texture_enabled = false;
    data.mDeltaTime = 0.005;
    
    double force_f1 = engine2.calculate_force(&data); 
    // Expect ~ 22.35 / 4000 = 0.005
    
    if (std::abs(force_f1) > 0.0001) {
        std::cout << "[PASS] Bottoming effect active. Force: " << force_f1 << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Bottoming effect zero. Phase alignment?" << std::endl;
        g_tests_failed++;
    }
}

void test_oversteer_boost() {
    std::cout << "\nTest: Oversteer Boost (Rear Grip Loss)" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));
    
    engine.m_sop_effect = 1.0;
    engine.m_oversteer_boost = 1.0;
    engine.m_gain = 1.0;
    
    // Scenario: Front has grip, rear is sliding
    data.mWheels[0].mGripFract = 1.0; // FL
    data.mWheels[1].mGripFract = 1.0; // FR
    data.mWheels[2].mGripFract = 0.5; // RL (sliding)
    data.mWheels[3].mGripFract = 0.5; // RR (sliding)
    
    // Lateral G (cornering)
    data.mLocalAccel.x = 9.81; // 1G lateral
    
    // Rear lateral force (resisting slide)
    data.mWheels[2].mLateralForce = 2000.0;
    data.mWheels[3].mLateralForce = 2000.0;
    
    // Run for multiple frames to let smoothing settle
    double force = 0.0;
    for (int i=0; i<60; i++) {
        force = engine.calculate_force(&data);
    }
    
    // Expected: SoP boosted by grip delta (0.5) + rear torque
    // Base SoP = 1.0 * 1.0 * 1000 = 1000
    // Boost = 1.0 + (0.5 * 1.0 * 2.0) = 2.0x
    // SoP = 1000 * 2.0 = 2000
    // Rear Torque = 2000 * 0.05 * 1.0 = 100
    // Total SoP = 2100 / 4000 = 0.525
    
    ASSERT_NEAR(force, 0.525, 0.05);
}

void test_phase_wraparound() {
    std::cout << "\nTest: Phase Wraparound (Anti-Click)" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));
    
    engine.m_lockup_enabled = true;
    engine.m_lockup_gain = 1.0;
    
    data.mUnfilteredBrake = 1.0;
    data.mWheels[0].mSlipRatio = -0.3;
    data.mWheels[1].mSlipRatio = -0.3;
    data.mLocalVel.z = 20.0; // 20 m/s
    data.mDeltaTime = 0.01;
    
    // Run for 100 frames (should wrap phase multiple times)
    double prev_phase = 0.0;
    int wrap_count = 0;
    
    for (int i = 0; i < 100; i++) {
        engine.calculate_force(&data);
        
        // Check for wraparound
        if (engine.m_lockup_phase < prev_phase) {
            wrap_count++;
            // Verify wrap happened near 2π
            // With freq=40Hz, dt=0.01, step is ~2.5 rad.
            // So prev_phase could be as low as 6.28 - 2.5 = 3.78.
            // We check it's at least > 3.0 to ensure it's not resetting randomly at 0.
            if (!(prev_phase > 3.0)) {
                 std::cout << "[FAIL] Wrapped phase too early: " << prev_phase << std::endl;
                 g_tests_failed++;
            }
        }
        prev_phase = engine.m_lockup_phase;
    }
    
    // Should have wrapped at least once
    if (wrap_count > 0) {
        std::cout << "[PASS] Phase wrapped " << wrap_count << " times without discontinuity." << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Phase did not wrap" << std::endl;
        g_tests_failed++;
    }
}

void test_road_texture_state_persistence() {
    std::cout << "\nTest: Road Texture State Persistence" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));
    
    engine.m_road_texture_enabled = true;
    engine.m_road_texture_gain = 1.0;
    
    // Frame 1: Initial deflection
    data.mWheels[0].mVerticalTireDeflection = 0.01;
    data.mWheels[1].mVerticalTireDeflection = 0.01;
    data.mWheels[0].mTireLoad = 4000.0;
    data.mWheels[1].mTireLoad = 4000.0;
    
    double force1 = engine.calculate_force(&data);
    // First frame: delta = 0.01 - 0.0 = 0.01
    // Expected force = (0.01 + 0.01) * 5000 * 1.0 * 1.0 = 100
    // Normalized = 100 / 4000 = 0.025
    
    // Frame 2: Bump (sudden increase)
    data.mWheels[0].mVerticalTireDeflection = 0.02;
    data.mWheels[1].mVerticalTireDeflection = 0.02;
    
    double force2 = engine.calculate_force(&data);
    // Delta = 0.02 - 0.01 = 0.01
    // Force should be same as frame 1
    
    ASSERT_NEAR(force2, force1, 0.001);
    
    // Frame 3: No change (flat road)
    double force3 = engine.calculate_force(&data);
    // Delta = 0.0, force should be near zero
    if (std::abs(force3) < 0.01) {
        std::cout << "[PASS] Road texture state preserved correctly." << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Road texture state issue" << std::endl;
        g_tests_failed++;
    }
}

void test_multi_effect_interaction() {
    std::cout << "\nTest: Multi-Effect Interaction (Lockup + Spin)" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));
    
    // Enable both lockup and spin
    engine.m_lockup_enabled = true;
    engine.m_lockup_gain = 1.0;
    engine.m_spin_enabled = true;
    engine.m_spin_gain = 1.0;
    
    // Scenario: Braking AND spinning (e.g., locked front, spinning rear)
    data.mUnfilteredBrake = 1.0;
    data.mUnfilteredThrottle = 0.5; // Partial throttle
    data.mWheels[0].mSlipRatio = -0.3; // Front locked
    data.mWheels[1].mSlipRatio = -0.3;
    data.mWheels[2].mSlipRatio = 0.5;  // Rear spinning
    data.mWheels[3].mSlipRatio = 0.5;
    data.mLocalVel.z = 20.0;
    data.mDeltaTime = 0.01;
    
    // Run multiple frames
    for (int i = 0; i < 10; i++) {
        engine.calculate_force(&data);
    }
    
    // Verify both phases advanced
    bool lockup_ok = engine.m_lockup_phase > 0.0;
    bool spin_ok = engine.m_spin_phase > 0.0;
    
    if (lockup_ok && spin_ok) {
         // Verify phases are different (independent oscillators)
        if (std::abs(engine.m_lockup_phase - engine.m_spin_phase) > 0.1) {
             std::cout << "[PASS] Multiple effects coexist without interference." << std::endl;
             g_tests_passed++;
        } else {
             std::cout << "[FAIL] Phases are identical?" << std::endl;
             g_tests_failed++;
        }
    } else {
        std::cout << "[FAIL] Effects did not trigger." << std::endl;
        g_tests_failed++;
    }
}

void test_load_factor_edge_cases() {
    std::cout << "\nTest: Load Factor Edge Cases" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));
    
    engine.m_slide_texture_enabled = true;
    engine.m_slide_texture_gain = 1.0;
    
    // Setup slide condition
    data.mWheels[0].mSlipAngle = 0.2;
    data.mWheels[1].mSlipAngle = 0.2;
    data.mWheels[0].mLateralPatchVel = 5.0;
    data.mWheels[1].mLateralPatchVel = 5.0;
    data.mDeltaTime = 0.01;
    
    // Case 1: Zero load (airborne)
    data.mWheels[0].mTireLoad = 0.0;
    data.mWheels[1].mTireLoad = 0.0;
    
    double force_airborne = engine.calculate_force(&data);
    // Load factor = 0, slide texture should be silent
    ASSERT_NEAR(force_airborne, 0.0, 0.001);
    
    // Case 2: Extreme load (20000N)
    data.mWheels[0].mTireLoad = 20000.0;
    data.mWheels[1].mTireLoad = 20000.0;
    
    engine.calculate_force(&data); // Advance phase
    double force_extreme = engine.calculate_force(&data);
    
    // Load factor should be clamped at 1.5
    // Max expected: sawtooth * 300 * 1.5 = 450
    // Normalized: 450 / 4000 = 0.1125
    if (std::abs(force_extreme) < 0.15) {
        std::cout << "[PASS] Load factor clamped correctly." << std::endl;
        g_tests_passed++;
    } else {
         std::cout << "[FAIL] Load factor not clamped? Force: " << force_extreme << std::endl;
         g_tests_failed++;
    }
}

void test_spin_torque_drop_interaction() {
    std::cout << "\nTest: Spin Torque Drop with SoP" << std::endl;
    FFBEngine engine;
    rF2Telemetry data;
    std::memset(&data, 0, sizeof(data));
    
    engine.m_spin_enabled = true;
    engine.m_spin_gain = 1.0;
    engine.m_sop_effect = 1.0;
    engine.m_gain = 1.0;
    
    // High SoP force
    data.mLocalAccel.x = 9.81; // 1G lateral
    data.mSteeringArmForce = 2000.0;
    
    // Set Grip to 1.0 so Game Force isn't killed by Understeer Effect
    data.mWheels[0].mGripFract = 1.0;
    data.mWheels[1].mGripFract = 1.0;
    data.mWheels[2].mGripFract = 1.0;
    data.mWheels[3].mGripFract = 1.0;
    
    // No spin initially
    data.mUnfilteredThrottle = 0.0;
    
    // Run multiple frames to settle SoP
    double force_no_spin = 0.0;
    for (int i=0; i<60; i++) {
        force_no_spin = engine.calculate_force(&data);
    }
    
    // Now trigger spin
    data.mUnfilteredThrottle = 1.0;
    data.mWheels[2].mSlipRatio = 0.7; // 70% slip (severe = 1.0)
    data.mWheels[3].mSlipRatio = 0.7;
    data.mLocalVel.z = 20.0;
    data.mDeltaTime = 0.01;
    
    double force_with_spin = engine.calculate_force(&data);
    
    // Torque drop: 1.0 - (1.0 * 1.0 * 0.6) = 0.4 (60% reduction)
    // Force should be significantly lower
    // Note: Spin also adds a rumble, which might increase the force slightly.
    // Base 0.25 -> Dropped 0.10. Rumble adds +/- 0.125.
    // So force could be up to 0.225 or down to -0.025.
    // We check that it is NOT equal to original (0.25) and somewhat reduced/modified.
    if (std::abs(force_with_spin - force_no_spin) > 0.05) {
        std::cout << "[PASS] Spin torque drop modifies total force." << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Torque drop ineffective. Spin: " << force_with_spin << " NoSpin: " << force_no_spin << std::endl;
        g_tests_failed++;
    }
}

int main() {
    test_zero_input();
    test_suspension_bottoming();
    test_grip_modulation();
    test_sop_effect();
    test_min_force();
    test_progressive_lockup();
    test_slide_texture();
    test_dynamic_tuning();
    
    test_oversteer_boost();
    test_phase_wraparound();
    test_road_texture_state_persistence();
    test_multi_effect_interaction();
    test_load_factor_edge_cases();
    test_spin_torque_drop_interaction();

    std::cout << "\n----------------" << std::endl;
    std::cout << "Tests Passed: " << g_tests_passed << std::endl;
    std::cout << "Tests Failed: " << g_tests_failed << std::endl;
    
    return g_tests_failed > 0 ? 1 : 0;
}

```

# File: tests\build-tests\CMakeCache.txt
```
# This is the CMakeCache file.
# For build in directory: c:/dev/personal/LMUFFB_public/LMUFFB/tests/build-tests
# It was generated by CMake: C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe
# You can edit this file to change values found and used by cmake.
# If you do not want to change any of the values, simply exit the editor.
# If you do want to change a value, simply edit, save, and exit the editor.
# The syntax for the file is as follows:
# KEY:TYPE=VALUE
# KEY is the name of a variable in the cache.
# TYPE is a hint to GUIs for the type of VALUE, DO NOT EDIT TYPE!.
# VALUE is the current value for the KEY.

########################
# EXTERNAL cache entries
########################

//Path to a program.
CMAKE_AR:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.36.32532/bin/Hostx64/x64/lib.exe

//No help, variable specified on the command line.
CMAKE_BUILD_TYPE:UNINITIALIZED=Debug

//Semicolon separated list of supported configuration types, only
// supports Debug, Release, MinSizeRel, and RelWithDebInfo, anything
// else will be ignored.
CMAKE_CONFIGURATION_TYPES:STRING=Debug;Release;MinSizeRel;RelWithDebInfo

//Flags used by the CXX compiler during all build types.
CMAKE_CXX_FLAGS:STRING=/DWIN32 /D_WINDOWS /W3 /GR /EHsc

//Flags used by the CXX compiler during DEBUG builds.
CMAKE_CXX_FLAGS_DEBUG:STRING=/MDd /Zi /Ob0 /Od /RTC1

//Flags used by the CXX compiler during MINSIZEREL builds.
CMAKE_CXX_FLAGS_MINSIZEREL:STRING=/MD /O1 /Ob1 /DNDEBUG

//Flags used by the CXX compiler during RELEASE builds.
CMAKE_CXX_FLAGS_RELEASE:STRING=/MD /O2 /Ob2 /DNDEBUG

//Flags used by the CXX compiler during RELWITHDEBINFO builds.
CMAKE_CXX_FLAGS_RELWITHDEBINFO:STRING=/MD /Zi /O2 /Ob1 /DNDEBUG

//Libraries linked by default with all C++ applications.
CMAKE_CXX_STANDARD_LIBRARIES:STRING=kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib

//Flags used by the C compiler during all build types.
CMAKE_C_FLAGS:STRING=/DWIN32 /D_WINDOWS /W3

//Flags used by the C compiler during DEBUG builds.
CMAKE_C_FLAGS_DEBUG:STRING=/MDd /Zi /Ob0 /Od /RTC1

//Flags used by the C compiler during MINSIZEREL builds.
CMAKE_C_FLAGS_MINSIZEREL:STRING=/MD /O1 /Ob1 /DNDEBUG

//Flags used by the C compiler during RELEASE builds.
CMAKE_C_FLAGS_RELEASE:STRING=/MD /O2 /Ob2 /DNDEBUG

//Flags used by the C compiler during RELWITHDEBINFO builds.
CMAKE_C_FLAGS_RELWITHDEBINFO:STRING=/MD /Zi /O2 /Ob1 /DNDEBUG

//Libraries linked by default with all C applications.
CMAKE_C_STANDARD_LIBRARIES:STRING=kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib

//Flags used by the linker during all build types.
CMAKE_EXE_LINKER_FLAGS:STRING=/machine:x64

//Flags used by the linker during DEBUG builds.
CMAKE_EXE_LINKER_FLAGS_DEBUG:STRING=/debug /INCREMENTAL

//Flags used by the linker during MINSIZEREL builds.
CMAKE_EXE_LINKER_FLAGS_MINSIZEREL:STRING=/INCREMENTAL:NO

//Flags used by the linker during RELEASE builds.
CMAKE_EXE_LINKER_FLAGS_RELEASE:STRING=/INCREMENTAL:NO

//Flags used by the linker during RELWITHDEBINFO builds.
CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO:STRING=/debug /INCREMENTAL

//Value Computed by CMake.
CMAKE_FIND_PACKAGE_REDIRECTS_DIR:STATIC=C:/dev/personal/LMUFFB_public/LMUFFB/tests/build-tests/CMakeFiles/pkgRedirects

//Install path prefix, prepended onto install directories.
CMAKE_INSTALL_PREFIX:PATH=C:/Program Files (x86)/LMUFFB_Tests

//Path to a program.
CMAKE_LINKER:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.36.32532/bin/Hostx64/x64/link.exe

//Flags used by the linker during the creation of modules during
// all build types.
CMAKE_MODULE_LINKER_FLAGS:STRING=/machine:x64

//Flags used by the linker during the creation of modules during
// DEBUG builds.
CMAKE_MODULE_LINKER_FLAGS_DEBUG:STRING=/debug /INCREMENTAL

//Flags used by the linker during the creation of modules during
// MINSIZEREL builds.
CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL:STRING=/INCREMENTAL:NO

//Flags used by the linker during the creation of modules during
// RELEASE builds.
CMAKE_MODULE_LINKER_FLAGS_RELEASE:STRING=/INCREMENTAL:NO

//Flags used by the linker during the creation of modules during
// RELWITHDEBINFO builds.
CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO:STRING=/debug /INCREMENTAL

//Path to a program.
CMAKE_MT:FILEPATH=C:/Program Files (x86)/Windows Kits/10/bin/10.0.22621.0/x64/mt.exe

//Value Computed by CMake
CMAKE_PROJECT_DESCRIPTION:STATIC=

//Value Computed by CMake
CMAKE_PROJECT_HOMEPAGE_URL:STATIC=

//Value Computed by CMake
CMAKE_PROJECT_NAME:STATIC=LMUFFB_Tests

//RC compiler
CMAKE_RC_COMPILER:FILEPATH=C:/Program Files (x86)/Windows Kits/10/bin/10.0.22621.0/x64/rc.exe

//Flags for Windows Resource Compiler during all build types.
CMAKE_RC_FLAGS:STRING=-DWIN32

//Flags for Windows Resource Compiler during DEBUG builds.
CMAKE_RC_FLAGS_DEBUG:STRING=-D_DEBUG

//Flags for Windows Resource Compiler during MINSIZEREL builds.
CMAKE_RC_FLAGS_MINSIZEREL:STRING=

//Flags for Windows Resource Compiler during RELEASE builds.
CMAKE_RC_FLAGS_RELEASE:STRING=

//Flags for Windows Resource Compiler during RELWITHDEBINFO builds.
CMAKE_RC_FLAGS_RELWITHDEBINFO:STRING=

//Flags used by the linker during the creation of shared libraries
// during all build types.
CMAKE_SHARED_LINKER_FLAGS:STRING=/machine:x64

//Flags used by the linker during the creation of shared libraries
// during DEBUG builds.
CMAKE_SHARED_LINKER_FLAGS_DEBUG:STRING=/debug /INCREMENTAL

//Flags used by the linker during the creation of shared libraries
// during MINSIZEREL builds.
CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL:STRING=/INCREMENTAL:NO

//Flags used by the linker during the creation of shared libraries
// during RELEASE builds.
CMAKE_SHARED_LINKER_FLAGS_RELEASE:STRING=/INCREMENTAL:NO

//Flags used by the linker during the creation of shared libraries
// during RELWITHDEBINFO builds.
CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO:STRING=/debug /INCREMENTAL

//If set, runtime paths are not added when installing shared libraries,
// but are added when building.
CMAKE_SKIP_INSTALL_RPATH:BOOL=NO

//If set, runtime paths are not added when using shared libraries.
CMAKE_SKIP_RPATH:BOOL=NO

//Flags used by the linker during the creation of static libraries
// during all build types.
CMAKE_STATIC_LINKER_FLAGS:STRING=/machine:x64

//Flags used by the linker during the creation of static libraries
// during DEBUG builds.
CMAKE_STATIC_LINKER_FLAGS_DEBUG:STRING=

//Flags used by the linker during the creation of static libraries
// during MINSIZEREL builds.
CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL:STRING=

//Flags used by the linker during the creation of static libraries
// during RELEASE builds.
CMAKE_STATIC_LINKER_FLAGS_RELEASE:STRING=

//Flags used by the linker during the creation of static libraries
// during RELWITHDEBINFO builds.
CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO:STRING=

//If this value is on, makefiles will be generated without the
// .SILENT directive, and all commands will be echoed to the console
// during the make.  This is useful for debugging only. With Visual
// Studio IDE projects all commands are done without /nologo.
CMAKE_VERBOSE_MAKEFILE:BOOL=FALSE

//Value Computed by CMake
LMUFFB_Tests_BINARY_DIR:STATIC=C:/dev/personal/LMUFFB_public/LMUFFB/tests/build-tests

//Value Computed by CMake
LMUFFB_Tests_IS_TOP_LEVEL:STATIC=ON

//Value Computed by CMake
LMUFFB_Tests_SOURCE_DIR:STATIC=C:/dev/personal/LMUFFB_public/LMUFFB/tests


########################
# INTERNAL cache entries
########################

//ADVANCED property for variable: CMAKE_AR
CMAKE_AR-ADVANCED:INTERNAL=1
//This is the directory where this CMakeCache.txt was created
CMAKE_CACHEFILE_DIR:INTERNAL=c:/dev/personal/LMUFFB_public/LMUFFB/tests/build-tests
//Major version of cmake used to create the current loaded cache
CMAKE_CACHE_MAJOR_VERSION:INTERNAL=3
//Minor version of cmake used to create the current loaded cache
CMAKE_CACHE_MINOR_VERSION:INTERNAL=26
//Patch version of cmake used to create the current loaded cache
CMAKE_CACHE_PATCH_VERSION:INTERNAL=0
//Path to CMake executable.
CMAKE_COMMAND:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe
//Path to cpack program executable.
CMAKE_CPACK_COMMAND:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cpack.exe
//Path to ctest program executable.
CMAKE_CTEST_COMMAND:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/ctest.exe
//ADVANCED property for variable: CMAKE_CXX_FLAGS
CMAKE_CXX_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_CXX_FLAGS_DEBUG
CMAKE_CXX_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_CXX_FLAGS_MINSIZEREL
CMAKE_CXX_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_CXX_FLAGS_RELEASE
CMAKE_CXX_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_CXX_FLAGS_RELWITHDEBINFO
CMAKE_CXX_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_CXX_STANDARD_LIBRARIES
CMAKE_CXX_STANDARD_LIBRARIES-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS
CMAKE_C_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_DEBUG
CMAKE_C_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_MINSIZEREL
CMAKE_C_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_RELEASE
CMAKE_C_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_RELWITHDEBINFO
CMAKE_C_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_STANDARD_LIBRARIES
CMAKE_C_STANDARD_LIBRARIES-ADVANCED:INTERNAL=1
//Executable file format
CMAKE_EXECUTABLE_FORMAT:INTERNAL=Unknown
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS
CMAKE_EXE_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_DEBUG
CMAKE_EXE_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_MINSIZEREL
CMAKE_EXE_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_RELEASE
CMAKE_EXE_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//Name of external makefile project generator.
CMAKE_EXTRA_GENERATOR:INTERNAL=
//Name of generator.
CMAKE_GENERATOR:INTERNAL=Visual Studio 17 2022
//Generator instance identifier.
CMAKE_GENERATOR_INSTANCE:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community
//Name of generator platform.
CMAKE_GENERATOR_PLATFORM:INTERNAL=
//Name of generator toolset.
CMAKE_GENERATOR_TOOLSET:INTERNAL=
//Source directory with the top level CMakeLists.txt file for this
// project
CMAKE_HOME_DIRECTORY:INTERNAL=C:/dev/personal/LMUFFB_public/LMUFFB/tests
//ADVANCED property for variable: CMAKE_LINKER
CMAKE_LINKER-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS
CMAKE_MODULE_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_DEBUG
CMAKE_MODULE_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL
CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_RELEASE
CMAKE_MODULE_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MT
CMAKE_MT-ADVANCED:INTERNAL=1
//number of local generators
CMAKE_NUMBER_OF_MAKEFILES:INTERNAL=1
//Platform information initialized
CMAKE_PLATFORM_INFO_INITIALIZED:INTERNAL=1
//noop for ranlib
CMAKE_RANLIB:INTERNAL=:
//ADVANCED property for variable: CMAKE_RC_COMPILER
CMAKE_RC_COMPILER-ADVANCED:INTERNAL=1
CMAKE_RC_COMPILER_WORKS:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS
CMAKE_RC_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_DEBUG
CMAKE_RC_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_MINSIZEREL
CMAKE_RC_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_RELEASE
CMAKE_RC_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_RELWITHDEBINFO
CMAKE_RC_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//Path to CMake installation.
CMAKE_ROOT:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.26
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS
CMAKE_SHARED_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_DEBUG
CMAKE_SHARED_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL
CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_RELEASE
CMAKE_SHARED_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SKIP_INSTALL_RPATH
CMAKE_SKIP_INSTALL_RPATH-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SKIP_RPATH
CMAKE_SKIP_RPATH-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS
CMAKE_STATIC_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_DEBUG
CMAKE_STATIC_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL
CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_RELEASE
CMAKE_STATIC_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_VERBOSE_MAKEFILE
CMAKE_VERBOSE_MAKEFILE-ADVANCED:INTERNAL=1


```

# File: tests\build-tests\cmake_install.cmake
```cmake
# Install script for directory: C:/dev/personal/LMUFFB_public/LMUFFB/tests

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/LMUFFB_Tests")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/dev/personal/LMUFFB_public/LMUFFB/tests/build-tests/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")

```

# File: tests\build-tests\CTestTestfile.cmake
```cmake
# CMake generated Testfile for 
# Source directory: C:/dev/personal/LMUFFB_public/LMUFFB/tests
# Build directory: C:/dev/personal/LMUFFB_public/LMUFFB/tests/build-tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(CoreLogicTest "C:/dev/personal/LMUFFB_public/LMUFFB/tests/build-tests/Debug/run_tests.exe")
  set_tests_properties(CoreLogicTest PROPERTIES  _BACKTRACE_TRIPLES "C:/dev/personal/LMUFFB_public/LMUFFB/tests/CMakeLists.txt;14;add_test;C:/dev/personal/LMUFFB_public/LMUFFB/tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(CoreLogicTest "C:/dev/personal/LMUFFB_public/LMUFFB/tests/build-tests/Release/run_tests.exe")
  set_tests_properties(CoreLogicTest PROPERTIES  _BACKTRACE_TRIPLES "C:/dev/personal/LMUFFB_public/LMUFFB/tests/CMakeLists.txt;14;add_test;C:/dev/personal/LMUFFB_public/LMUFFB/tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(CoreLogicTest "C:/dev/personal/LMUFFB_public/LMUFFB/tests/build-tests/MinSizeRel/run_tests.exe")
  set_tests_properties(CoreLogicTest PROPERTIES  _BACKTRACE_TRIPLES "C:/dev/personal/LMUFFB_public/LMUFFB/tests/CMakeLists.txt;14;add_test;C:/dev/personal/LMUFFB_public/LMUFFB/tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(CoreLogicTest "C:/dev/personal/LMUFFB_public/LMUFFB/tests/build-tests/RelWithDebInfo/run_tests.exe")
  set_tests_properties(CoreLogicTest PROPERTIES  _BACKTRACE_TRIPLES "C:/dev/personal/LMUFFB_public/LMUFFB/tests/CMakeLists.txt;14;add_test;C:/dev/personal/LMUFFB_public/LMUFFB/tests/CMakeLists.txt;0;")
else()
  add_test(CoreLogicTest NOT_AVAILABLE)
endif()

```

# File: tests\build-tests\CMakeFiles\TargetDirectories.txt
```
C:/dev/personal/LMUFFB_public/LMUFFB/tests/build-tests/CMakeFiles/run_tests.dir
C:/dev/personal/LMUFFB_public/LMUFFB/tests/build-tests/CMakeFiles/RUN_TESTS.dir
C:/dev/personal/LMUFFB_public/LMUFFB/tests/build-tests/CMakeFiles/ALL_BUILD.dir
C:/dev/personal/LMUFFB_public/LMUFFB/tests/build-tests/CMakeFiles/ZERO_CHECK.dir

```

# File: tests\build-tests\CMakeFiles\3.26.0-msvc3\CMakeCCompiler.cmake
```cmake
set(CMAKE_C_COMPILER "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.36.32532/bin/Hostx64/x64/cl.exe")
set(CMAKE_C_COMPILER_ARG1 "")
set(CMAKE_C_COMPILER_ID "MSVC")
set(CMAKE_C_COMPILER_VERSION "19.36.32537.0")
set(CMAKE_C_COMPILER_VERSION_INTERNAL "")
set(CMAKE_C_COMPILER_WRAPPER "")
set(CMAKE_C_STANDARD_COMPUTED_DEFAULT "90")
set(CMAKE_C_EXTENSIONS_COMPUTED_DEFAULT "OFF")
set(CMAKE_C_COMPILE_FEATURES "c_std_90;c_function_prototypes;c_std_99;c_restrict;c_variadic_macros;c_std_11;c_static_assert;c_std_17")
set(CMAKE_C90_COMPILE_FEATURES "c_std_90;c_function_prototypes")
set(CMAKE_C99_COMPILE_FEATURES "c_std_99;c_restrict;c_variadic_macros")
set(CMAKE_C11_COMPILE_FEATURES "c_std_11;c_static_assert")
set(CMAKE_C17_COMPILE_FEATURES "c_std_17")
set(CMAKE_C23_COMPILE_FEATURES "")

set(CMAKE_C_PLATFORM_ID "Windows")
set(CMAKE_C_SIMULATE_ID "")
set(CMAKE_C_COMPILER_FRONTEND_VARIANT "MSVC")
set(CMAKE_C_SIMULATE_VERSION "")
set(CMAKE_C_COMPILER_ARCHITECTURE_ID x64)

set(MSVC_C_ARCHITECTURE_ID x64)

set(CMAKE_AR "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.36.32532/bin/Hostx64/x64/lib.exe")
set(CMAKE_C_COMPILER_AR "")
set(CMAKE_RANLIB ":")
set(CMAKE_C_COMPILER_RANLIB "")
set(CMAKE_LINKER "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.36.32532/bin/Hostx64/x64/link.exe")
set(CMAKE_MT "C:/Program Files (x86)/Windows Kits/10/bin/10.0.22621.0/x64/mt.exe")
set(CMAKE_COMPILER_IS_GNUCC )
set(CMAKE_C_COMPILER_LOADED 1)
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_C_ABI_COMPILED TRUE)

set(CMAKE_C_COMPILER_ENV_VAR "CC")

set(CMAKE_C_COMPILER_ID_RUN 1)
set(CMAKE_C_SOURCE_FILE_EXTENSIONS c;m)
set(CMAKE_C_IGNORE_EXTENSIONS h;H;o;O;obj;OBJ;def;DEF;rc;RC)
set(CMAKE_C_LINKER_PREFERENCE 10)

# Save compiler ABI information.
set(CMAKE_C_SIZEOF_DATA_PTR "8")
set(CMAKE_C_COMPILER_ABI "")
set(CMAKE_C_BYTE_ORDER "LITTLE_ENDIAN")
set(CMAKE_C_LIBRARY_ARCHITECTURE "")

if(CMAKE_C_SIZEOF_DATA_PTR)
  set(CMAKE_SIZEOF_VOID_P "${CMAKE_C_SIZEOF_DATA_PTR}")
endif()

if(CMAKE_C_COMPILER_ABI)
  set(CMAKE_INTERNAL_PLATFORM_ABI "${CMAKE_C_COMPILER_ABI}")
endif()

if(CMAKE_C_LIBRARY_ARCHITECTURE)
  set(CMAKE_LIBRARY_ARCHITECTURE "")
endif()

set(CMAKE_C_CL_SHOWINCLUDES_PREFIX "")
if(CMAKE_C_CL_SHOWINCLUDES_PREFIX)
  set(CMAKE_CL_SHOWINCLUDES_PREFIX "${CMAKE_C_CL_SHOWINCLUDES_PREFIX}")
endif()





set(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES "")
set(CMAKE_C_IMPLICIT_LINK_LIBRARIES "")
set(CMAKE_C_IMPLICIT_LINK_DIRECTORIES "")
set(CMAKE_C_IMPLICIT_LINK_FRAMEWORK_DIRECTORIES "")

```

# File: tests\build-tests\CMakeFiles\3.26.0-msvc3\CMakeCXXCompiler.cmake
```cmake
set(CMAKE_CXX_COMPILER "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.36.32532/bin/Hostx64/x64/cl.exe")
set(CMAKE_CXX_COMPILER_ARG1 "")
set(CMAKE_CXX_COMPILER_ID "MSVC")
set(CMAKE_CXX_COMPILER_VERSION "19.36.32537.0")
set(CMAKE_CXX_COMPILER_VERSION_INTERNAL "")
set(CMAKE_CXX_COMPILER_WRAPPER "")
set(CMAKE_CXX_STANDARD_COMPUTED_DEFAULT "14")
set(CMAKE_CXX_EXTENSIONS_COMPUTED_DEFAULT "OFF")
set(CMAKE_CXX_COMPILE_FEATURES "cxx_std_98;cxx_template_template_parameters;cxx_std_11;cxx_alias_templates;cxx_alignas;cxx_alignof;cxx_attributes;cxx_auto_type;cxx_constexpr;cxx_decltype;cxx_decltype_incomplete_return_types;cxx_default_function_template_args;cxx_defaulted_functions;cxx_defaulted_move_initializers;cxx_delegating_constructors;cxx_deleted_functions;cxx_enum_forward_declarations;cxx_explicit_conversions;cxx_extended_friend_declarations;cxx_extern_templates;cxx_final;cxx_func_identifier;cxx_generalized_initializers;cxx_inheriting_constructors;cxx_inline_namespaces;cxx_lambdas;cxx_local_type_template_args;cxx_long_long_type;cxx_noexcept;cxx_nonstatic_member_init;cxx_nullptr;cxx_override;cxx_range_for;cxx_raw_string_literals;cxx_reference_qualified_functions;cxx_right_angle_brackets;cxx_rvalue_references;cxx_sizeof_member;cxx_static_assert;cxx_strong_enums;cxx_thread_local;cxx_trailing_return_types;cxx_unicode_literals;cxx_uniform_initialization;cxx_unrestricted_unions;cxx_user_literals;cxx_variadic_macros;cxx_variadic_templates;cxx_std_14;cxx_aggregate_default_initializers;cxx_attribute_deprecated;cxx_binary_literals;cxx_contextual_conversions;cxx_decltype_auto;cxx_digit_separators;cxx_generic_lambdas;cxx_lambda_init_captures;cxx_relaxed_constexpr;cxx_return_type_deduction;cxx_variable_templates;cxx_std_17;cxx_std_20;cxx_std_23")
set(CMAKE_CXX98_COMPILE_FEATURES "cxx_std_98;cxx_template_template_parameters")
set(CMAKE_CXX11_COMPILE_FEATURES "cxx_std_11;cxx_alias_templates;cxx_alignas;cxx_alignof;cxx_attributes;cxx_auto_type;cxx_constexpr;cxx_decltype;cxx_decltype_incomplete_return_types;cxx_default_function_template_args;cxx_defaulted_functions;cxx_defaulted_move_initializers;cxx_delegating_constructors;cxx_deleted_functions;cxx_enum_forward_declarations;cxx_explicit_conversions;cxx_extended_friend_declarations;cxx_extern_templates;cxx_final;cxx_func_identifier;cxx_generalized_initializers;cxx_inheriting_constructors;cxx_inline_namespaces;cxx_lambdas;cxx_local_type_template_args;cxx_long_long_type;cxx_noexcept;cxx_nonstatic_member_init;cxx_nullptr;cxx_override;cxx_range_for;cxx_raw_string_literals;cxx_reference_qualified_functions;cxx_right_angle_brackets;cxx_rvalue_references;cxx_sizeof_member;cxx_static_assert;cxx_strong_enums;cxx_thread_local;cxx_trailing_return_types;cxx_unicode_literals;cxx_uniform_initialization;cxx_unrestricted_unions;cxx_user_literals;cxx_variadic_macros;cxx_variadic_templates")
set(CMAKE_CXX14_COMPILE_FEATURES "cxx_std_14;cxx_aggregate_default_initializers;cxx_attribute_deprecated;cxx_binary_literals;cxx_contextual_conversions;cxx_decltype_auto;cxx_digit_separators;cxx_generic_lambdas;cxx_lambda_init_captures;cxx_relaxed_constexpr;cxx_return_type_deduction;cxx_variable_templates")
set(CMAKE_CXX17_COMPILE_FEATURES "cxx_std_17")
set(CMAKE_CXX20_COMPILE_FEATURES "cxx_std_20")
set(CMAKE_CXX23_COMPILE_FEATURES "cxx_std_23")

set(CMAKE_CXX_PLATFORM_ID "Windows")
set(CMAKE_CXX_SIMULATE_ID "")
set(CMAKE_CXX_COMPILER_FRONTEND_VARIANT "MSVC")
set(CMAKE_CXX_SIMULATE_VERSION "")
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID x64)

set(MSVC_CXX_ARCHITECTURE_ID x64)

set(CMAKE_AR "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.36.32532/bin/Hostx64/x64/lib.exe")
set(CMAKE_CXX_COMPILER_AR "")
set(CMAKE_RANLIB ":")
set(CMAKE_CXX_COMPILER_RANLIB "")
set(CMAKE_LINKER "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.36.32532/bin/Hostx64/x64/link.exe")
set(CMAKE_MT "C:/Program Files (x86)/Windows Kits/10/bin/10.0.22621.0/x64/mt.exe")
set(CMAKE_COMPILER_IS_GNUCXX )
set(CMAKE_CXX_COMPILER_LOADED 1)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_CXX_ABI_COMPILED TRUE)

set(CMAKE_CXX_COMPILER_ENV_VAR "CXX")

set(CMAKE_CXX_COMPILER_ID_RUN 1)
set(CMAKE_CXX_SOURCE_FILE_EXTENSIONS C;M;c++;cc;cpp;cxx;m;mm;mpp;CPP;ixx;cppm)
set(CMAKE_CXX_IGNORE_EXTENSIONS inl;h;hpp;HPP;H;o;O;obj;OBJ;def;DEF;rc;RC)

foreach (lang C OBJC OBJCXX)
  if (CMAKE_${lang}_COMPILER_ID_RUN)
    foreach(extension IN LISTS CMAKE_${lang}_SOURCE_FILE_EXTENSIONS)
      list(REMOVE_ITEM CMAKE_CXX_SOURCE_FILE_EXTENSIONS ${extension})
    endforeach()
  endif()
endforeach()

set(CMAKE_CXX_LINKER_PREFERENCE 30)
set(CMAKE_CXX_LINKER_PREFERENCE_PROPAGATES 1)

# Save compiler ABI information.
set(CMAKE_CXX_SIZEOF_DATA_PTR "8")
set(CMAKE_CXX_COMPILER_ABI "")
set(CMAKE_CXX_BYTE_ORDER "LITTLE_ENDIAN")
set(CMAKE_CXX_LIBRARY_ARCHITECTURE "")

if(CMAKE_CXX_SIZEOF_DATA_PTR)
  set(CMAKE_SIZEOF_VOID_P "${CMAKE_CXX_SIZEOF_DATA_PTR}")
endif()

if(CMAKE_CXX_COMPILER_ABI)
  set(CMAKE_INTERNAL_PLATFORM_ABI "${CMAKE_CXX_COMPILER_ABI}")
endif()

if(CMAKE_CXX_LIBRARY_ARCHITECTURE)
  set(CMAKE_LIBRARY_ARCHITECTURE "")
endif()

set(CMAKE_CXX_CL_SHOWINCLUDES_PREFIX "")
if(CMAKE_CXX_CL_SHOWINCLUDES_PREFIX)
  set(CMAKE_CL_SHOWINCLUDES_PREFIX "${CMAKE_CXX_CL_SHOWINCLUDES_PREFIX}")
endif()





set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES "")
set(CMAKE_CXX_IMPLICIT_LINK_LIBRARIES "")
set(CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "")
set(CMAKE_CXX_IMPLICIT_LINK_FRAMEWORK_DIRECTORIES "")

```

# File: tests\build-tests\CMakeFiles\3.26.0-msvc3\CMakeRCCompiler.cmake
```cmake
set(CMAKE_RC_COMPILER "C:/Program Files (x86)/Windows Kits/10/bin/10.0.22621.0/x64/rc.exe")
set(CMAKE_RC_COMPILER_ARG1 "")
set(CMAKE_RC_COMPILER_LOADED 1)
set(CMAKE_RC_SOURCE_FILE_EXTENSIONS rc;RC)
set(CMAKE_RC_OUTPUT_EXTENSION .res)
set(CMAKE_RC_COMPILER_ENV_VAR "RC")

```

# File: tests\build-tests\CMakeFiles\3.26.0-msvc3\CMakeSystem.cmake
```cmake
set(CMAKE_HOST_SYSTEM "Windows-10.0.26100")
set(CMAKE_HOST_SYSTEM_NAME "Windows")
set(CMAKE_HOST_SYSTEM_VERSION "10.0.26100")
set(CMAKE_HOST_SYSTEM_PROCESSOR "AMD64")



set(CMAKE_SYSTEM "Windows-10.0.26100")
set(CMAKE_SYSTEM_NAME "Windows")
set(CMAKE_SYSTEM_VERSION "10.0.26100")
set(CMAKE_SYSTEM_PROCESSOR "AMD64")

set(CMAKE_CROSSCOMPILING "FALSE")

set(CMAKE_SYSTEM_LOADED 1)

```

# File: tests\build-tests\CMakeFiles\3.26.0-msvc3\VCTargetsPath.txt
```
C:/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Microsoft/VC/v170

```

# File: tests\build-tests\CMakeFiles\3.26.0-msvc3\CompilerIdC\CMakeCCompilerId.c
```cpp
#ifdef __cplusplus
# error "A C++ compiler has been selected for C."
#endif

#if defined(__18CXX)
# define ID_VOID_MAIN
#endif
#if defined(__CLASSIC_C__)
/* cv-qualifiers did not exist in K&R C */
# define const
# define volatile
#endif

#if !defined(__has_include)
/* If the compiler does not have __has_include, pretend the answer is
   always no.  */
#  define __has_include(x) 0
#endif


/* Version number components: V=Version, R=Revision, P=Patch
   Version date components:   YYYY=Year, MM=Month,   DD=Day  */

#if defined(__INTEL_COMPILER) || defined(__ICC)
# define COMPILER_ID "Intel"
# if defined(_MSC_VER)
#  define SIMULATE_ID "MSVC"
# endif
# if defined(__GNUC__)
#  define SIMULATE_ID "GNU"
# endif
  /* __INTEL_COMPILER = VRP prior to 2021, and then VVVV for 2021 and later,
     except that a few beta releases use the old format with V=2021.  */
# if __INTEL_COMPILER < 2021 || __INTEL_COMPILER == 202110 || __INTEL_COMPILER == 202111
#  define COMPILER_VERSION_MAJOR DEC(__INTEL_COMPILER/100)
#  define COMPILER_VERSION_MINOR DEC(__INTEL_COMPILER/10 % 10)
#  if defined(__INTEL_COMPILER_UPDATE)
#   define COMPILER_VERSION_PATCH DEC(__INTEL_COMPILER_UPDATE)
#  else
#   define COMPILER_VERSION_PATCH DEC(__INTEL_COMPILER   % 10)
#  endif
# else
#  define COMPILER_VERSION_MAJOR DEC(__INTEL_COMPILER)
#  define COMPILER_VERSION_MINOR DEC(__INTEL_COMPILER_UPDATE)
   /* The third version component from --version is an update index,
      but no macro is provided for it.  */
#  define COMPILER_VERSION_PATCH DEC(0)
# endif
# if defined(__INTEL_COMPILER_BUILD_DATE)
   /* __INTEL_COMPILER_BUILD_DATE = YYYYMMDD */
#  define COMPILER_VERSION_TWEAK DEC(__INTEL_COMPILER_BUILD_DATE)
# endif
# if defined(_MSC_VER)
   /* _MSC_VER = VVRR */
#  define SIMULATE_VERSION_MAJOR DEC(_MSC_VER / 100)
#  define SIMULATE_VERSION_MINOR DEC(_MSC_VER % 100)
# endif
# if defined(__GNUC__)
#  define SIMULATE_VERSION_MAJOR DEC(__GNUC__)
# elif defined(__GNUG__)
#  define SIMULATE_VERSION_MAJOR DEC(__GNUG__)
# endif
# if defined(__GNUC_MINOR__)
#  define SIMULATE_VERSION_MINOR DEC(__GNUC_MINOR__)
# endif
# if defined(__GNUC_PATCHLEVEL__)
#  define SIMULATE_VERSION_PATCH DEC(__GNUC_PATCHLEVEL__)
# endif

#elif (defined(__clang__) && defined(__INTEL_CLANG_COMPILER)) || defined(__INTEL_LLVM_COMPILER)
# define COMPILER_ID "IntelLLVM"
#if defined(_MSC_VER)
# define SIMULATE_ID "MSVC"
#endif
#if defined(__GNUC__)
# define SIMULATE_ID "GNU"
#endif
/* __INTEL_LLVM_COMPILER = VVVVRP prior to 2021.2.0, VVVVRRPP for 2021.2.0 and
 * later.  Look for 6 digit vs. 8 digit version number to decide encoding.
 * VVVV is no smaller than the current year when a version is released.
 */
#if __INTEL_LLVM_COMPILER < 1000000L
# define COMPILER_VERSION_MAJOR DEC(__INTEL_LLVM_COMPILER/100)
# define COMPILER_VERSION_MINOR DEC(__INTEL_LLVM_COMPILER/10 % 10)
# define COMPILER_VERSION_PATCH DEC(__INTEL_LLVM_COMPILER    % 10)
#else
# define COMPILER_VERSION_MAJOR DEC(__INTEL_LLVM_COMPILER/10000)
# define COMPILER_VERSION_MINOR DEC(__INTEL_LLVM_COMPILER/100 % 100)
# define COMPILER_VERSION_PATCH DEC(__INTEL_LLVM_COMPILER     % 100)
#endif
#if defined(_MSC_VER)
  /* _MSC_VER = VVRR */
# define SIMULATE_VERSION_MAJOR DEC(_MSC_VER / 100)
# define SIMULATE_VERSION_MINOR DEC(_MSC_VER % 100)
#endif
#if defined(__GNUC__)
# define SIMULATE_VERSION_MAJOR DEC(__GNUC__)
#elif defined(__GNUG__)
# define SIMULATE_VERSION_MAJOR DEC(__GNUG__)
#endif
#if defined(__GNUC_MINOR__)
# define SIMULATE_VERSION_MINOR DEC(__GNUC_MINOR__)
#endif
#if defined(__GNUC_PATCHLEVEL__)
# define SIMULATE_VERSION_PATCH DEC(__GNUC_PATCHLEVEL__)
#endif

#elif defined(__PATHCC__)
# define COMPILER_ID "PathScale"
# define COMPILER_VERSION_MAJOR DEC(__PATHCC__)
# define COMPILER_VERSION_MINOR DEC(__PATHCC_MINOR__)
# if defined(__PATHCC_PATCHLEVEL__)
#  define COMPILER_VERSION_PATCH DEC(__PATHCC_PATCHLEVEL__)
# endif

#elif defined(__BORLANDC__) && defined(__CODEGEARC_VERSION__)
# define COMPILER_ID "Embarcadero"
# define COMPILER_VERSION_MAJOR HEX(__CODEGEARC_VERSION__>>24 & 0x00FF)
# define COMPILER_VERSION_MINOR HEX(__CODEGEARC_VERSION__>>16 & 0x00FF)
# define COMPILER_VERSION_PATCH DEC(__CODEGEARC_VERSION__     & 0xFFFF)

#elif defined(__BORLANDC__)
# define COMPILER_ID "Borland"
  /* __BORLANDC__ = 0xVRR */
# define COMPILER_VERSION_MAJOR HEX(__BORLANDC__>>8)
# define COMPILER_VERSION_MINOR HEX(__BORLANDC__ & 0xFF)

#elif defined(__WATCOMC__) && __WATCOMC__ < 1200
# define COMPILER_ID "Watcom"
   /* __WATCOMC__ = VVRR */
# define COMPILER_VERSION_MAJOR DEC(__WATCOMC__ / 100)
# define COMPILER_VERSION_MINOR DEC((__WATCOMC__ / 10) % 10)
# if (__WATCOMC__ % 10) > 0
#  define COMPILER_VERSION_PATCH DEC(__WATCOMC__ % 10)
# endif

#elif defined(__WATCOMC__)
# define COMPILER_ID "OpenWatcom"
   /* __WATCOMC__ = VVRP + 1100 */
# define COMPILER_VERSION_MAJOR DEC((__WATCOMC__ - 1100) / 100)
# define COMPILER_VERSION_MINOR DEC((__WATCOMC__ / 10) % 10)
# if (__WATCOMC__ % 10) > 0
#  define COMPILER_VERSION_PATCH DEC(__WATCOMC__ % 10)
# endif

#elif defined(__SUNPRO_C)
# define COMPILER_ID "SunPro"
# if __SUNPRO_C >= 0x5100
   /* __SUNPRO_C = 0xVRRP */
#  define COMPILER_VERSION_MAJOR HEX(__SUNPRO_C>>12)
#  define COMPILER_VERSION_MINOR HEX(__SUNPRO_C>>4 & 0xFF)
#  define COMPILER_VERSION_PATCH HEX(__SUNPRO_C    & 0xF)
# else
   /* __SUNPRO_CC = 0xVRP */
#  define COMPILER_VERSION_MAJOR HEX(__SUNPRO_C>>8)
#  define COMPILER_VERSION_MINOR HEX(__SUNPRO_C>>4 & 0xF)
#  define COMPILER_VERSION_PATCH HEX(__SUNPRO_C    & 0xF)
# endif

#elif defined(__HP_cc)
# define COMPILER_ID "HP"
  /* __HP_cc = VVRRPP */
# define COMPILER_VERSION_MAJOR DEC(__HP_cc/10000)
# define COMPILER_VERSION_MINOR DEC(__HP_cc/100 % 100)
# define COMPILER_VERSION_PATCH DEC(__HP_cc     % 100)

#elif defined(__DECC)
# define COMPILER_ID "Compaq"
  /* __DECC_VER = VVRRTPPPP */
# define COMPILER_VERSION_MAJOR DEC(__DECC_VER/10000000)
# define COMPILER_VERSION_MINOR DEC(__DECC_VER/100000  % 100)
# define COMPILER_VERSION_PATCH DEC(__DECC_VER         % 10000)

#elif defined(__IBMC__) && defined(__COMPILER_VER__)
# define COMPILER_ID "zOS"
  /* __IBMC__ = VRP */
# define COMPILER_VERSION_MAJOR DEC(__IBMC__/100)
# define COMPILER_VERSION_MINOR DEC(__IBMC__/10 % 10)
# define COMPILER_VERSION_PATCH DEC(__IBMC__    % 10)

#elif defined(__open_xl__) && defined(__clang__)
# define COMPILER_ID "IBMClang"
# define COMPILER_VERSION_MAJOR DEC(__open_xl_version__)
# define COMPILER_VERSION_MINOR DEC(__open_xl_release__)
# define COMPILER_VERSION_PATCH DEC(__open_xl_modification__)
# define COMPILER_VERSION_TWEAK DEC(__open_xl_ptf_fix_level__)


#elif defined(__ibmxl__) && defined(__clang__)
# define COMPILER_ID "XLClang"
# define COMPILER_VERSION_MAJOR DEC(__ibmxl_version__)
# define COMPILER_VERSION_MINOR DEC(__ibmxl_release__)
# define COMPILER_VERSION_PATCH DEC(__ibmxl_modification__)
# define COMPILER_VERSION_TWEAK DEC(__ibmxl_ptf_fix_level__)


#elif defined(__IBMC__) && !defined(__COMPILER_VER__) && __IBMC__ >= 800
# define COMPILER_ID "XL"
  /* __IBMC__ = VRP */
# define COMPILER_VERSION_MAJOR DEC(__IBMC__/100)
# define COMPILER_VERSION_MINOR DEC(__IBMC__/10 % 10)
# define COMPILER_VERSION_PATCH DEC(__IBMC__    % 10)

#elif defined(__IBMC__) && !defined(__COMPILER_VER__) && __IBMC__ < 800
# define COMPILER_ID "VisualAge"
  /* __IBMC__ = VRP */
# define COMPILER_VERSION_MAJOR DEC(__IBMC__/100)
# define COMPILER_VERSION_MINOR DEC(__IBMC__/10 % 10)
# define COMPILER_VERSION_PATCH DEC(__IBMC__    % 10)

#elif defined(__NVCOMPILER)
# define COMPILER_ID "NVHPC"
# define COMPILER_VERSION_MAJOR DEC(__NVCOMPILER_MAJOR__)
# define COMPILER_VERSION_MINOR DEC(__NVCOMPILER_MINOR__)
# if defined(__NVCOMPILER_PATCHLEVEL__)
#  define COMPILER_VERSION_PATCH DEC(__NVCOMPILER_PATCHLEVEL__)
# endif

#elif defined(__PGI)
# define COMPILER_ID "PGI"
# define COMPILER_VERSION_MAJOR DEC(__PGIC__)
# define COMPILER_VERSION_MINOR DEC(__PGIC_MINOR__)
# if defined(__PGIC_PATCHLEVEL__)
#  define COMPILER_VERSION_PATCH DEC(__PGIC_PATCHLEVEL__)
# endif

#elif defined(_CRAYC)
# define COMPILER_ID "Cray"
# define COMPILER_VERSION_MAJOR DEC(_RELEASE_MAJOR)
# define COMPILER_VERSION_MINOR DEC(_RELEASE_MINOR)

#elif defined(__TI_COMPILER_VERSION__)
# define COMPILER_ID "TI"
  /* __TI_COMPILER_VERSION__ = VVVRRRPPP */
# define COMPILER_VERSION_MAJOR DEC(__TI_COMPILER_VERSION__/1000000)
# define COMPILER_VERSION_MINOR DEC(__TI_COMPILER_VERSION__/1000   % 1000)
# define COMPILER_VERSION_PATCH DEC(__TI_COMPILER_VERSION__        % 1000)

#elif defined(__CLANG_FUJITSU)
# define COMPILER_ID "FujitsuClang"
# define COMPILER_VERSION_MAJOR DEC(__FCC_major__)
# define COMPILER_VERSION_MINOR DEC(__FCC_minor__)
# define COMPILER_VERSION_PATCH DEC(__FCC_patchlevel__)
# define COMPILER_VERSION_INTERNAL_STR __clang_version__


#elif defined(__FUJITSU)
# define COMPILER_ID "Fujitsu"
# if defined(__FCC_version__)
#   define COMPILER_VERSION __FCC_version__
# elif defined(__FCC_major__)
#   define COMPILER_VERSION_MAJOR DEC(__FCC_major__)
#   define COMPILER_VERSION_MINOR DEC(__FCC_minor__)
#   define COMPILER_VERSION_PATCH DEC(__FCC_patchlevel__)
# endif
# if defined(__fcc_version)
#   define COMPILER_VERSION_INTERNAL DEC(__fcc_version)
# elif defined(__FCC_VERSION)
#   define COMPILER_VERSION_INTERNAL DEC(__FCC_VERSION)
# endif


#elif defined(__ghs__)
# define COMPILER_ID "GHS"
/* __GHS_VERSION_NUMBER = VVVVRP */
# ifdef __GHS_VERSION_NUMBER
# define COMPILER_VERSION_MAJOR DEC(__GHS_VERSION_NUMBER / 100)
# define COMPILER_VERSION_MINOR DEC(__GHS_VERSION_NUMBER / 10 % 10)
# define COMPILER_VERSION_PATCH DEC(__GHS_VERSION_NUMBER      % 10)
# endif

#elif defined(__TASKING__)
# define COMPILER_ID "Tasking"
  # define COMPILER_VERSION_MAJOR DEC(__VERSION__/1000)
  # define COMPILER_VERSION_MINOR DEC(__VERSION__ % 100)
# define COMPILER_VERSION_INTERNAL DEC(__VERSION__)

#elif defined(__TINYC__)
# define COMPILER_ID "TinyCC"

#elif defined(__BCC__)
# define COMPILER_ID "Bruce"

#elif defined(__SCO_VERSION__)
# define COMPILER_ID "SCO"

#elif defined(__ARMCC_VERSION) && !defined(__clang__)
# define COMPILER_ID "ARMCC"
#if __ARMCC_VERSION >= 1000000
  /* __ARMCC_VERSION = VRRPPPP */
  # define COMPILER_VERSION_MAJOR DEC(__ARMCC_VERSION/1000000)
  # define COMPILER_VERSION_MINOR DEC(__ARMCC_VERSION/10000 % 100)
  # define COMPILER_VERSION_PATCH DEC(__ARMCC_VERSION     % 10000)
#else
  /* __ARMCC_VERSION = VRPPPP */
  # define COMPILER_VERSION_MAJOR DEC(__ARMCC_VERSION/100000)
  # define COMPILER_VERSION_MINOR DEC(__ARMCC_VERSION/10000 % 10)
  # define COMPILER_VERSION_PATCH DEC(__ARMCC_VERSION    % 10000)
#endif


#elif defined(__clang__) && defined(__apple_build_version__)
# define COMPILER_ID "AppleClang"
# if defined(_MSC_VER)
#  define SIMULATE_ID "MSVC"
# endif
# define COMPILER_VERSION_MAJOR DEC(__clang_major__)
# define COMPILER_VERSION_MINOR DEC(__clang_minor__)
# define COMPILER_VERSION_PATCH DEC(__clang_patchlevel__)
# if defined(_MSC_VER)
   /* _MSC_VER = VVRR */
#  define SIMULATE_VERSION_MAJOR DEC(_MSC_VER / 100)
#  define SIMULATE_VERSION_MINOR DEC(_MSC_VER % 100)
# endif
# define COMPILER_VERSION_TWEAK DEC(__apple_build_version__)

#elif defined(__clang__) && defined(__ARMCOMPILER_VERSION)
# define COMPILER_ID "ARMClang"
  # define COMPILER_VERSION_MAJOR DEC(__ARMCOMPILER_VERSION/1000000)
  # define COMPILER_VERSION_MINOR DEC(__ARMCOMPILER_VERSION/10000 % 100)
  # define COMPILER_VERSION_PATCH DEC(__ARMCOMPILER_VERSION     % 10000)
# define COMPILER_VERSION_INTERNAL DEC(__ARMCOMPILER_VERSION)

#elif defined(__clang__)
# define COMPILER_ID "Clang"
# if defined(_MSC_VER)
#  define SIMULATE_ID "MSVC"
# endif
# define COMPILER_VERSION_MAJOR DEC(__clang_major__)
# define COMPILER_VERSION_MINOR DEC(__clang_minor__)
# define COMPILER_VERSION_PATCH DEC(__clang_patchlevel__)
# if defined(_MSC_VER)
   /* _MSC_VER = VVRR */
#  define SIMULATE_VERSION_MAJOR DEC(_MSC_VER / 100)
#  define SIMULATE_VERSION_MINOR DEC(_MSC_VER % 100)
# endif

#elif defined(__LCC__) && (defined(__GNUC__) || defined(__GNUG__) || defined(__MCST__))
# define COMPILER_ID "LCC"
# define COMPILER_VERSION_MAJOR DEC(__LCC__ / 100)
# define COMPILER_VERSION_MINOR DEC(__LCC__ % 100)
# if defined(__LCC_MINOR__)
#  define COMPILER_VERSION_PATCH DEC(__LCC_MINOR__)
# endif
# if defined(__GNUC__) && defined(__GNUC_MINOR__)
#  define SIMULATE_ID "GNU"
#  define SIMULATE_VERSION_MAJOR DEC(__GNUC__)
#  define SIMULATE_VERSION_MINOR DEC(__GNUC_MINOR__)
#  if defined(__GNUC_PATCHLEVEL__)
#   define SIMULATE_VERSION_PATCH DEC(__GNUC_PATCHLEVEL__)
#  endif
# endif

#elif defined(__GNUC__)
# define COMPILER_ID "GNU"
# define COMPILER_VERSION_MAJOR DEC(__GNUC__)
# if defined(__GNUC_MINOR__)
#  define COMPILER_VERSION_MINOR DEC(__GNUC_MINOR__)
# endif
# if defined(__GNUC_PATCHLEVEL__)
#  define COMPILER_VERSION_PATCH DEC(__GNUC_PATCHLEVEL__)
# endif

#elif defined(_MSC_VER)
# define COMPILER_ID "MSVC"
  /* _MSC_VER = VVRR */
# define COMPILER_VERSION_MAJOR DEC(_MSC_VER / 100)
# define COMPILER_VERSION_MINOR DEC(_MSC_VER % 100)
# if defined(_MSC_FULL_VER)
#  if _MSC_VER >= 1400
    /* _MSC_FULL_VER = VVRRPPPPP */
#   define COMPILER_VERSION_PATCH DEC(_MSC_FULL_VER % 100000)
#  else
    /* _MSC_FULL_VER = VVRRPPPP */
#   define COMPILER_VERSION_PATCH DEC(_MSC_FULL_VER % 10000)
#  endif
# endif
# if defined(_MSC_BUILD)
#  define COMPILER_VERSION_TWEAK DEC(_MSC_BUILD)
# endif

#elif defined(_ADI_COMPILER)
# define COMPILER_ID "ADSP"
#if defined(__VERSIONNUM__)
  /* __VERSIONNUM__ = 0xVVRRPPTT */
#  define COMPILER_VERSION_MAJOR DEC(__VERSIONNUM__ >> 24 & 0xFF)
#  define COMPILER_VERSION_MINOR DEC(__VERSIONNUM__ >> 16 & 0xFF)
#  define COMPILER_VERSION_PATCH DEC(__VERSIONNUM__ >> 8 & 0xFF)
#  define COMPILER_VERSION_TWEAK DEC(__VERSIONNUM__ & 0xFF)
#endif

#elif defined(__IAR_SYSTEMS_ICC__) || defined(__IAR_SYSTEMS_ICC)
# define COMPILER_ID "IAR"
# if defined(__VER__) && defined(__ICCARM__)
#  define COMPILER_VERSION_MAJOR DEC((__VER__) / 1000000)
#  define COMPILER_VERSION_MINOR DEC(((__VER__) / 1000) % 1000)
#  define COMPILER_VERSION_PATCH DEC((__VER__) % 1000)
#  define COMPILER_VERSION_INTERNAL DEC(__IAR_SYSTEMS_ICC__)
# elif defined(__VER__) && (defined(__ICCAVR__) || defined(__ICCRX__) || defined(__ICCRH850__) || defined(__ICCRL78__) || defined(__ICC430__) || defined(__ICCRISCV__) || defined(__ICCV850__) || defined(__ICC8051__) || defined(__ICCSTM8__))
#  define COMPILER_VERSION_MAJOR DEC((__VER__) / 100)
#  define COMPILER_VERSION_MINOR DEC((__VER__) - (((__VER__) / 100)*100))
#  define COMPILER_VERSION_PATCH DEC(__SUBVERSION__)
#  define COMPILER_VERSION_INTERNAL DEC(__IAR_SYSTEMS_ICC__)
# endif

#elif defined(__SDCC_VERSION_MAJOR) || defined(SDCC)
# define COMPILER_ID "SDCC"
# if defined(__SDCC_VERSION_MAJOR)
#  define COMPILER_VERSION_MAJOR DEC(__SDCC_VERSION_MAJOR)
#  define COMPILER_VERSION_MINOR DEC(__SDCC_VERSION_MINOR)
#  define COMPILER_VERSION_PATCH DEC(__SDCC_VERSION_PATCH)
# else
  /* SDCC = VRP */
#  define COMPILER_VERSION_MAJOR DEC(SDCC/100)
#  define COMPILER_VERSION_MINOR DEC(SDCC/10 % 10)
#  define COMPILER_VERSION_PATCH DEC(SDCC    % 10)
# endif


/* These compilers are either not known or too old to define an
  identification macro.  Try to identify the platform and guess that
  it is the native compiler.  */
#elif defined(__hpux) || defined(__hpua)
# define COMPILER_ID "HP"

#else /* unknown compiler */
# define COMPILER_ID ""
#endif

/* Construct the string literal in pieces to prevent the source from
   getting matched.  Store it in a pointer rather than an array
   because some compilers will just produce instructions to fill the
   array rather than assigning a pointer to a static array.  */
char const* info_compiler = "INFO" ":" "compiler[" COMPILER_ID "]";
#ifdef SIMULATE_ID
char const* info_simulate = "INFO" ":" "simulate[" SIMULATE_ID "]";
#endif

#ifdef __QNXNTO__
char const* qnxnto = "INFO" ":" "qnxnto[]";
#endif

#if defined(__CRAYXT_COMPUTE_LINUX_TARGET)
char const *info_cray = "INFO" ":" "compiler_wrapper[CrayPrgEnv]";
#endif

#define STRINGIFY_HELPER(X) #X
#define STRINGIFY(X) STRINGIFY_HELPER(X)

/* Identify known platforms by name.  */
#if defined(__linux) || defined(__linux__) || defined(linux)
# define PLATFORM_ID "Linux"

#elif defined(__MSYS__)
# define PLATFORM_ID "MSYS"

#elif defined(__CYGWIN__)
# define PLATFORM_ID "Cygwin"

#elif defined(__MINGW32__)
# define PLATFORM_ID "MinGW"

#elif defined(__APPLE__)
# define PLATFORM_ID "Darwin"

#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
# define PLATFORM_ID "Windows"

#elif defined(__FreeBSD__) || defined(__FreeBSD)
# define PLATFORM_ID "FreeBSD"

#elif defined(__NetBSD__) || defined(__NetBSD)
# define PLATFORM_ID "NetBSD"

#elif defined(__OpenBSD__) || defined(__OPENBSD)
# define PLATFORM_ID "OpenBSD"

#elif defined(__sun) || defined(sun)
# define PLATFORM_ID "SunOS"

#elif defined(_AIX) || defined(__AIX) || defined(__AIX__) || defined(__aix) || defined(__aix__)
# define PLATFORM_ID "AIX"

#elif defined(__hpux) || defined(__hpux__)
# define PLATFORM_ID "HP-UX"

#elif defined(__HAIKU__)
# define PLATFORM_ID "Haiku"

#elif defined(__BeOS) || defined(__BEOS__) || defined(_BEOS)
# define PLATFORM_ID "BeOS"

#elif defined(__QNX__) || defined(__QNXNTO__)
# define PLATFORM_ID "QNX"

#elif defined(__tru64) || defined(_tru64) || defined(__TRU64__)
# define PLATFORM_ID "Tru64"

#elif defined(__riscos) || defined(__riscos__)
# define PLATFORM_ID "RISCos"

#elif defined(__sinix) || defined(__sinix__) || defined(__SINIX__)
# define PLATFORM_ID "SINIX"

#elif defined(__UNIX_SV__)
# define PLATFORM_ID "UNIX_SV"

#elif defined(__bsdos__)
# define PLATFORM_ID "BSDOS"

#elif defined(_MPRAS) || defined(MPRAS)
# define PLATFORM_ID "MP-RAS"

#elif defined(__osf) || defined(__osf__)
# define PLATFORM_ID "OSF1"

#elif defined(_SCO_SV) || defined(SCO_SV) || defined(sco_sv)
# define PLATFORM_ID "SCO_SV"

#elif defined(__ultrix) || defined(__ultrix__) || defined(_ULTRIX)
# define PLATFORM_ID "ULTRIX"

#elif defined(__XENIX__) || defined(_XENIX) || defined(XENIX)
# define PLATFORM_ID "Xenix"

#elif defined(__WATCOMC__)
# if defined(__LINUX__)
#  define PLATFORM_ID "Linux"

# elif defined(__DOS__)
#  define PLATFORM_ID "DOS"

# elif defined(__OS2__)
#  define PLATFORM_ID "OS2"

# elif defined(__WINDOWS__)
#  define PLATFORM_ID "Windows3x"

# elif defined(__VXWORKS__)
#  define PLATFORM_ID "VxWorks"

# else /* unknown platform */
#  define PLATFORM_ID
# endif

#elif defined(__INTEGRITY)
# if defined(INT_178B)
#  define PLATFORM_ID "Integrity178"

# else /* regular Integrity */
#  define PLATFORM_ID "Integrity"
# endif

# elif defined(_ADI_COMPILER)
#  define PLATFORM_ID "ADSP"

#else /* unknown platform */
# define PLATFORM_ID

#endif

/* For windows compilers MSVC and Intel we can determine
   the architecture of the compiler being used.  This is because
   the compilers do not have flags that can change the architecture,
   but rather depend on which compiler is being used
*/
#if defined(_WIN32) && defined(_MSC_VER)
# if defined(_M_IA64)
#  define ARCHITECTURE_ID "IA64"

# elif defined(_M_ARM64EC)
#  define ARCHITECTURE_ID "ARM64EC"

# elif defined(_M_X64) || defined(_M_AMD64)
#  define ARCHITECTURE_ID "x64"

# elif defined(_M_IX86)
#  define ARCHITECTURE_ID "X86"

# elif defined(_M_ARM64)
#  define ARCHITECTURE_ID "ARM64"

# elif defined(_M_ARM)
#  if _M_ARM == 4
#   define ARCHITECTURE_ID "ARMV4I"
#  elif _M_ARM == 5
#   define ARCHITECTURE_ID "ARMV5I"
#  else
#   define ARCHITECTURE_ID "ARMV" STRINGIFY(_M_ARM)
#  endif

# elif defined(_M_MIPS)
#  define ARCHITECTURE_ID "MIPS"

# elif defined(_M_SH)
#  define ARCHITECTURE_ID "SHx"

# else /* unknown architecture */
#  define ARCHITECTURE_ID ""
# endif

#elif defined(__WATCOMC__)
# if defined(_M_I86)
#  define ARCHITECTURE_ID "I86"

# elif defined(_M_IX86)
#  define ARCHITECTURE_ID "X86"

# else /* unknown architecture */
#  define ARCHITECTURE_ID ""
# endif

#elif defined(__IAR_SYSTEMS_ICC__) || defined(__IAR_SYSTEMS_ICC)
# if defined(__ICCARM__)
#  define ARCHITECTURE_ID "ARM"

# elif defined(__ICCRX__)
#  define ARCHITECTURE_ID "RX"

# elif defined(__ICCRH850__)
#  define ARCHITECTURE_ID "RH850"

# elif defined(__ICCRL78__)
#  define ARCHITECTURE_ID "RL78"

# elif defined(__ICCRISCV__)
#  define ARCHITECTURE_ID "RISCV"

# elif defined(__ICCAVR__)
#  define ARCHITECTURE_ID "AVR"

# elif defined(__ICC430__)
#  define ARCHITECTURE_ID "MSP430"

# elif defined(__ICCV850__)
#  define ARCHITECTURE_ID "V850"

# elif defined(__ICC8051__)
#  define ARCHITECTURE_ID "8051"

# elif defined(__ICCSTM8__)
#  define ARCHITECTURE_ID "STM8"

# else /* unknown architecture */
#  define ARCHITECTURE_ID ""
# endif

#elif defined(__ghs__)
# if defined(__PPC64__)
#  define ARCHITECTURE_ID "PPC64"

# elif defined(__ppc__)
#  define ARCHITECTURE_ID "PPC"

# elif defined(__ARM__)
#  define ARCHITECTURE_ID "ARM"

# elif defined(__x86_64__)
#  define ARCHITECTURE_ID "x64"

# elif defined(__i386__)
#  define ARCHITECTURE_ID "X86"

# else /* unknown architecture */
#  define ARCHITECTURE_ID ""
# endif

#elif defined(__TI_COMPILER_VERSION__)
# if defined(__TI_ARM__)
#  define ARCHITECTURE_ID "ARM"

# elif defined(__MSP430__)
#  define ARCHITECTURE_ID "MSP430"

# elif defined(__TMS320C28XX__)
#  define ARCHITECTURE_ID "TMS320C28x"

# elif defined(__TMS320C6X__) || defined(_TMS320C6X)
#  define ARCHITECTURE_ID "TMS320C6x"

# else /* unknown architecture */
#  define ARCHITECTURE_ID ""
# endif

# elif defined(__ADSPSHARC__)
#  define ARCHITECTURE_ID "SHARC"

# elif defined(__ADSPBLACKFIN__)
#  define ARCHITECTURE_ID "Blackfin"

#elif defined(__TASKING__)

# if defined(__CTC__) || defined(__CPTC__)
#  define ARCHITECTURE_ID "TriCore"

# elif defined(__CMCS__)
#  define ARCHITECTURE_ID "MCS"

# elif defined(__CARM__)
#  define ARCHITECTURE_ID "ARM"

# elif defined(__CARC__)
#  define ARCHITECTURE_ID "ARC"

# elif defined(__C51__)
#  define ARCHITECTURE_ID "8051"

# elif defined(__CPCP__)
#  define ARCHITECTURE_ID "PCP"

# else
#  define ARCHITECTURE_ID ""
# endif

#else
#  define ARCHITECTURE_ID
#endif

/* Convert integer to decimal digit literals.  */
#define DEC(n)                   \
  ('0' + (((n) / 10000000)%10)), \
  ('0' + (((n) / 1000000)%10)),  \
  ('0' + (((n) / 100000)%10)),   \
  ('0' + (((n) / 10000)%10)),    \
  ('0' + (((n) / 1000)%10)),     \
  ('0' + (((n) / 100)%10)),      \
  ('0' + (((n) / 10)%10)),       \
  ('0' +  ((n) % 10))

/* Convert integer to hex digit literals.  */
#define HEX(n)             \
  ('0' + ((n)>>28 & 0xF)), \
  ('0' + ((n)>>24 & 0xF)), \
  ('0' + ((n)>>20 & 0xF)), \
  ('0' + ((n)>>16 & 0xF)), \
  ('0' + ((n)>>12 & 0xF)), \
  ('0' + ((n)>>8  & 0xF)), \
  ('0' + ((n)>>4  & 0xF)), \
  ('0' + ((n)     & 0xF))

/* Construct a string literal encoding the version number. */
#ifdef COMPILER_VERSION
char const* info_version = "INFO" ":" "compiler_version[" COMPILER_VERSION "]";

/* Construct a string literal encoding the version number components. */
#elif defined(COMPILER_VERSION_MAJOR)
char const info_version[] = {
  'I', 'N', 'F', 'O', ':',
  'c','o','m','p','i','l','e','r','_','v','e','r','s','i','o','n','[',
  COMPILER_VERSION_MAJOR,
# ifdef COMPILER_VERSION_MINOR
  '.', COMPILER_VERSION_MINOR,
#  ifdef COMPILER_VERSION_PATCH
   '.', COMPILER_VERSION_PATCH,
#   ifdef COMPILER_VERSION_TWEAK
    '.', COMPILER_VERSION_TWEAK,
#   endif
#  endif
# endif
  ']','\0'};
#endif

/* Construct a string literal encoding the internal version number. */
#ifdef COMPILER_VERSION_INTERNAL
char const info_version_internal[] = {
  'I', 'N', 'F', 'O', ':',
  'c','o','m','p','i','l','e','r','_','v','e','r','s','i','o','n','_',
  'i','n','t','e','r','n','a','l','[',
  COMPILER_VERSION_INTERNAL,']','\0'};
#elif defined(COMPILER_VERSION_INTERNAL_STR)
char const* info_version_internal = "INFO" ":" "compiler_version_internal[" COMPILER_VERSION_INTERNAL_STR "]";
#endif

/* Construct a string literal encoding the version number components. */
#ifdef SIMULATE_VERSION_MAJOR
char const info_simulate_version[] = {
  'I', 'N', 'F', 'O', ':',
  's','i','m','u','l','a','t','e','_','v','e','r','s','i','o','n','[',
  SIMULATE_VERSION_MAJOR,
# ifdef SIMULATE_VERSION_MINOR
  '.', SIMULATE_VERSION_MINOR,
#  ifdef SIMULATE_VERSION_PATCH
   '.', SIMULATE_VERSION_PATCH,
#   ifdef SIMULATE_VERSION_TWEAK
    '.', SIMULATE_VERSION_TWEAK,
#   endif
#  endif
# endif
  ']','\0'};
#endif

/* Construct the string literal in pieces to prevent the source from
   getting matched.  Store it in a pointer rather than an array
   because some compilers will just produce instructions to fill the
   array rather than assigning a pointer to a static array.  */
char const* info_platform = "INFO" ":" "platform[" PLATFORM_ID "]";
char const* info_arch = "INFO" ":" "arch[" ARCHITECTURE_ID "]";



#if !defined(__STDC__) && !defined(__clang__)
# if defined(_MSC_VER) || defined(__ibmxl__) || defined(__IBMC__)
#  define C_VERSION "90"
# else
#  define C_VERSION
# endif
#elif __STDC_VERSION__ > 201710L
# define C_VERSION "23"
#elif __STDC_VERSION__ >= 201710L
# define C_VERSION "17"
#elif __STDC_VERSION__ >= 201000L
# define C_VERSION "11"
#elif __STDC_VERSION__ >= 199901L
# define C_VERSION "99"
#else
# define C_VERSION "90"
#endif
const char* info_language_standard_default =
  "INFO" ":" "standard_default[" C_VERSION "]";

const char* info_language_extensions_default = "INFO" ":" "extensions_default["
#if (defined(__clang__) || defined(__GNUC__) || defined(__xlC__) ||           \
     defined(__TI_COMPILER_VERSION__)) &&                                     \
  !defined(__STRICT_ANSI__)
  "ON"
#else
  "OFF"
#endif
"]";

/*--------------------------------------------------------------------------*/

#ifdef ID_VOID_MAIN
void main() {}
#else
# if defined(__CLASSIC_C__)
int main(argc, argv) int argc; char *argv[];
# else
int main(int argc, char* argv[])
# endif
{
  int require = 0;
  require += info_compiler[argc];
  require += info_platform[argc];
  require += info_arch[argc];
#ifdef COMPILER_VERSION_MAJOR
  require += info_version[argc];
#endif
#ifdef COMPILER_VERSION_INTERNAL
  require += info_version_internal[argc];
#endif
#ifdef SIMULATE_ID
  require += info_simulate[argc];
#endif
#ifdef SIMULATE_VERSION_MAJOR
  require += info_simulate_version[argc];
#endif
#if defined(__CRAYXT_COMPUTE_LINUX_TARGET)
  require += info_cray[argc];
#endif
  require += info_language_standard_default[argc];
  require += info_language_extensions_default[argc];
  (void)argv;
  return require;
}
#endif

```

# File: tests\build-tests\CMakeFiles\3.26.0-msvc3\CompilerIdCXX\CMakeCXXCompilerId.cpp
```cpp
/* This source file must have a .cpp extension so that all C++ compilers
   recognize the extension without flags.  Borland does not know .cxx for
   example.  */
#ifndef __cplusplus
# error "A C compiler has been selected for C++."
#endif

#if !defined(__has_include)
/* If the compiler does not have __has_include, pretend the answer is
   always no.  */
#  define __has_include(x) 0
#endif


/* Version number components: V=Version, R=Revision, P=Patch
   Version date components:   YYYY=Year, MM=Month,   DD=Day  */

#if defined(__COMO__)
# define COMPILER_ID "Comeau"
  /* __COMO_VERSION__ = VRR */
# define COMPILER_VERSION_MAJOR DEC(__COMO_VERSION__ / 100)
# define COMPILER_VERSION_MINOR DEC(__COMO_VERSION__ % 100)

#elif defined(__INTEL_COMPILER) || defined(__ICC)
# define COMPILER_ID "Intel"
# if defined(_MSC_VER)
#  define SIMULATE_ID "MSVC"
# endif
# if defined(__GNUC__)
#  define SIMULATE_ID "GNU"
# endif
  /* __INTEL_COMPILER = VRP prior to 2021, and then VVVV for 2021 and later,
     except that a few beta releases use the old format with V=2021.  */
# if __INTEL_COMPILER < 2021 || __INTEL_COMPILER == 202110 || __INTEL_COMPILER == 202111
#  define COMPILER_VERSION_MAJOR DEC(__INTEL_COMPILER/100)
#  define COMPILER_VERSION_MINOR DEC(__INTEL_COMPILER/10 % 10)
#  if defined(__INTEL_COMPILER_UPDATE)
#   define COMPILER_VERSION_PATCH DEC(__INTEL_COMPILER_UPDATE)
#  else
#   define COMPILER_VERSION_PATCH DEC(__INTEL_COMPILER   % 10)
#  endif
# else
#  define COMPILER_VERSION_MAJOR DEC(__INTEL_COMPILER)
#  define COMPILER_VERSION_MINOR DEC(__INTEL_COMPILER_UPDATE)
   /* The third version component from --version is an update index,
      but no macro is provided for it.  */
#  define COMPILER_VERSION_PATCH DEC(0)
# endif
# if defined(__INTEL_COMPILER_BUILD_DATE)
   /* __INTEL_COMPILER_BUILD_DATE = YYYYMMDD */
#  define COMPILER_VERSION_TWEAK DEC(__INTEL_COMPILER_BUILD_DATE)
# endif
# if defined(_MSC_VER)
   /* _MSC_VER = VVRR */
#  define SIMULATE_VERSION_MAJOR DEC(_MSC_VER / 100)
#  define SIMULATE_VERSION_MINOR DEC(_MSC_VER % 100)
# endif
# if defined(__GNUC__)
#  define SIMULATE_VERSION_MAJOR DEC(__GNUC__)
# elif defined(__GNUG__)
#  define SIMULATE_VERSION_MAJOR DEC(__GNUG__)
# endif
# if defined(__GNUC_MINOR__)
#  define SIMULATE_VERSION_MINOR DEC(__GNUC_MINOR__)
# endif
# if defined(__GNUC_PATCHLEVEL__)
#  define SIMULATE_VERSION_PATCH DEC(__GNUC_PATCHLEVEL__)
# endif

#elif (defined(__clang__) && defined(__INTEL_CLANG_COMPILER)) || defined(__INTEL_LLVM_COMPILER)
# define COMPILER_ID "IntelLLVM"
#if defined(_MSC_VER)
# define SIMULATE_ID "MSVC"
#endif
#if defined(__GNUC__)
# define SIMULATE_ID "GNU"
#endif
/* __INTEL_LLVM_COMPILER = VVVVRP prior to 2021.2.0, VVVVRRPP for 2021.2.0 and
 * later.  Look for 6 digit vs. 8 digit version number to decide encoding.
 * VVVV is no smaller than the current year when a version is released.
 */
#if __INTEL_LLVM_COMPILER < 1000000L
# define COMPILER_VERSION_MAJOR DEC(__INTEL_LLVM_COMPILER/100)
# define COMPILER_VERSION_MINOR DEC(__INTEL_LLVM_COMPILER/10 % 10)
# define COMPILER_VERSION_PATCH DEC(__INTEL_LLVM_COMPILER    % 10)
#else
# define COMPILER_VERSION_MAJOR DEC(__INTEL_LLVM_COMPILER/10000)
# define COMPILER_VERSION_MINOR DEC(__INTEL_LLVM_COMPILER/100 % 100)
# define COMPILER_VERSION_PATCH DEC(__INTEL_LLVM_COMPILER     % 100)
#endif
#if defined(_MSC_VER)
  /* _MSC_VER = VVRR */
# define SIMULATE_VERSION_MAJOR DEC(_MSC_VER / 100)
# define SIMULATE_VERSION_MINOR DEC(_MSC_VER % 100)
#endif
#if defined(__GNUC__)
# define SIMULATE_VERSION_MAJOR DEC(__GNUC__)
#elif defined(__GNUG__)
# define SIMULATE_VERSION_MAJOR DEC(__GNUG__)
#endif
#if defined(__GNUC_MINOR__)
# define SIMULATE_VERSION_MINOR DEC(__GNUC_MINOR__)
#endif
#if defined(__GNUC_PATCHLEVEL__)
# define SIMULATE_VERSION_PATCH DEC(__GNUC_PATCHLEVEL__)
#endif

#elif defined(__PATHCC__)
# define COMPILER_ID "PathScale"
# define COMPILER_VERSION_MAJOR DEC(__PATHCC__)
# define COMPILER_VERSION_MINOR DEC(__PATHCC_MINOR__)
# if defined(__PATHCC_PATCHLEVEL__)
#  define COMPILER_VERSION_PATCH DEC(__PATHCC_PATCHLEVEL__)
# endif

#elif defined(__BORLANDC__) && defined(__CODEGEARC_VERSION__)
# define COMPILER_ID "Embarcadero"
# define COMPILER_VERSION_MAJOR HEX(__CODEGEARC_VERSION__>>24 & 0x00FF)
# define COMPILER_VERSION_MINOR HEX(__CODEGEARC_VERSION__>>16 & 0x00FF)
# define COMPILER_VERSION_PATCH DEC(__CODEGEARC_VERSION__     & 0xFFFF)

#elif defined(__BORLANDC__)
# define COMPILER_ID "Borland"
  /* __BORLANDC__ = 0xVRR */
# define COMPILER_VERSION_MAJOR HEX(__BORLANDC__>>8)
# define COMPILER_VERSION_MINOR HEX(__BORLANDC__ & 0xFF)

#elif defined(__WATCOMC__) && __WATCOMC__ < 1200
# define COMPILER_ID "Watcom"
   /* __WATCOMC__ = VVRR */
# define COMPILER_VERSION_MAJOR DEC(__WATCOMC__ / 100)
# define COMPILER_VERSION_MINOR DEC((__WATCOMC__ / 10) % 10)
# if (__WATCOMC__ % 10) > 0
#  define COMPILER_VERSION_PATCH DEC(__WATCOMC__ % 10)
# endif

#elif defined(__WATCOMC__)
# define COMPILER_ID "OpenWatcom"
   /* __WATCOMC__ = VVRP + 1100 */
# define COMPILER_VERSION_MAJOR DEC((__WATCOMC__ - 1100) / 100)
# define COMPILER_VERSION_MINOR DEC((__WATCOMC__ / 10) % 10)
# if (__WATCOMC__ % 10) > 0
#  define COMPILER_VERSION_PATCH DEC(__WATCOMC__ % 10)
# endif

#elif defined(__SUNPRO_CC)
# define COMPILER_ID "SunPro"
# if __SUNPRO_CC >= 0x5100
   /* __SUNPRO_CC = 0xVRRP */
#  define COMPILER_VERSION_MAJOR HEX(__SUNPRO_CC>>12)
#  define COMPILER_VERSION_MINOR HEX(__SUNPRO_CC>>4 & 0xFF)
#  define COMPILER_VERSION_PATCH HEX(__SUNPRO_CC    & 0xF)
# else
   /* __SUNPRO_CC = 0xVRP */
#  define COMPILER_VERSION_MAJOR HEX(__SUNPRO_CC>>8)
#  define COMPILER_VERSION_MINOR HEX(__SUNPRO_CC>>4 & 0xF)
#  define COMPILER_VERSION_PATCH HEX(__SUNPRO_CC    & 0xF)
# endif

#elif defined(__HP_aCC)
# define COMPILER_ID "HP"
  /* __HP_aCC = VVRRPP */
# define COMPILER_VERSION_MAJOR DEC(__HP_aCC/10000)
# define COMPILER_VERSION_MINOR DEC(__HP_aCC/100 % 100)
# define COMPILER_VERSION_PATCH DEC(__HP_aCC     % 100)

#elif defined(__DECCXX)
# define COMPILER_ID "Compaq"
  /* __DECCXX_VER = VVRRTPPPP */
# define COMPILER_VERSION_MAJOR DEC(__DECCXX_VER/10000000)
# define COMPILER_VERSION_MINOR DEC(__DECCXX_VER/100000  % 100)
# define COMPILER_VERSION_PATCH DEC(__DECCXX_VER         % 10000)

#elif defined(__IBMCPP__) && defined(__COMPILER_VER__)
# define COMPILER_ID "zOS"
  /* __IBMCPP__ = VRP */
# define COMPILER_VERSION_MAJOR DEC(__IBMCPP__/100)
# define COMPILER_VERSION_MINOR DEC(__IBMCPP__/10 % 10)
# define COMPILER_VERSION_PATCH DEC(__IBMCPP__    % 10)

#elif defined(__open_xl__) && defined(__clang__)
# define COMPILER_ID "IBMClang"
# define COMPILER_VERSION_MAJOR DEC(__open_xl_version__)
# define COMPILER_VERSION_MINOR DEC(__open_xl_release__)
# define COMPILER_VERSION_PATCH DEC(__open_xl_modification__)
# define COMPILER_VERSION_TWEAK DEC(__open_xl_ptf_fix_level__)


#elif defined(__ibmxl__) && defined(__clang__)
# define COMPILER_ID "XLClang"
# define COMPILER_VERSION_MAJOR DEC(__ibmxl_version__)
# define COMPILER_VERSION_MINOR DEC(__ibmxl_release__)
# define COMPILER_VERSION_PATCH DEC(__ibmxl_modification__)
# define COMPILER_VERSION_TWEAK DEC(__ibmxl_ptf_fix_level__)


#elif defined(__IBMCPP__) && !defined(__COMPILER_VER__) && __IBMCPP__ >= 800
# define COMPILER_ID "XL"
  /* __IBMCPP__ = VRP */
# define COMPILER_VERSION_MAJOR DEC(__IBMCPP__/100)
# define COMPILER_VERSION_MINOR DEC(__IBMCPP__/10 % 10)
# define COMPILER_VERSION_PATCH DEC(__IBMCPP__    % 10)

#elif defined(__IBMCPP__) && !defined(__COMPILER_VER__) && __IBMCPP__ < 800
# define COMPILER_ID "VisualAge"
  /* __IBMCPP__ = VRP */
# define COMPILER_VERSION_MAJOR DEC(__IBMCPP__/100)
# define COMPILER_VERSION_MINOR DEC(__IBMCPP__/10 % 10)
# define COMPILER_VERSION_PATCH DEC(__IBMCPP__    % 10)

#elif defined(__NVCOMPILER)
# define COMPILER_ID "NVHPC"
# define COMPILER_VERSION_MAJOR DEC(__NVCOMPILER_MAJOR__)
# define COMPILER_VERSION_MINOR DEC(__NVCOMPILER_MINOR__)
# if defined(__NVCOMPILER_PATCHLEVEL__)
#  define COMPILER_VERSION_PATCH DEC(__NVCOMPILER_PATCHLEVEL__)
# endif

#elif defined(__PGI)
# define COMPILER_ID "PGI"
# define COMPILER_VERSION_MAJOR DEC(__PGIC__)
# define COMPILER_VERSION_MINOR DEC(__PGIC_MINOR__)
# if defined(__PGIC_PATCHLEVEL__)
#  define COMPILER_VERSION_PATCH DEC(__PGIC_PATCHLEVEL__)
# endif

#elif defined(_CRAYC)
# define COMPILER_ID "Cray"
# define COMPILER_VERSION_MAJOR DEC(_RELEASE_MAJOR)
# define COMPILER_VERSION_MINOR DEC(_RELEASE_MINOR)

#elif defined(__TI_COMPILER_VERSION__)
# define COMPILER_ID "TI"
  /* __TI_COMPILER_VERSION__ = VVVRRRPPP */
# define COMPILER_VERSION_MAJOR DEC(__TI_COMPILER_VERSION__/1000000)
# define COMPILER_VERSION_MINOR DEC(__TI_COMPILER_VERSION__/1000   % 1000)
# define COMPILER_VERSION_PATCH DEC(__TI_COMPILER_VERSION__        % 1000)

#elif defined(__CLANG_FUJITSU)
# define COMPILER_ID "FujitsuClang"
# define COMPILER_VERSION_MAJOR DEC(__FCC_major__)
# define COMPILER_VERSION_MINOR DEC(__FCC_minor__)
# define COMPILER_VERSION_PATCH DEC(__FCC_patchlevel__)
# define COMPILER_VERSION_INTERNAL_STR __clang_version__


#elif defined(__FUJITSU)
# define COMPILER_ID "Fujitsu"
# if defined(__FCC_version__)
#   define COMPILER_VERSION __FCC_version__
# elif defined(__FCC_major__)
#   define COMPILER_VERSION_MAJOR DEC(__FCC_major__)
#   define COMPILER_VERSION_MINOR DEC(__FCC_minor__)
#   define COMPILER_VERSION_PATCH DEC(__FCC_patchlevel__)
# endif
# if defined(__fcc_version)
#   define COMPILER_VERSION_INTERNAL DEC(__fcc_version)
# elif defined(__FCC_VERSION)
#   define COMPILER_VERSION_INTERNAL DEC(__FCC_VERSION)
# endif


#elif defined(__ghs__)
# define COMPILER_ID "GHS"
/* __GHS_VERSION_NUMBER = VVVVRP */
# ifdef __GHS_VERSION_NUMBER
# define COMPILER_VERSION_MAJOR DEC(__GHS_VERSION_NUMBER / 100)
# define COMPILER_VERSION_MINOR DEC(__GHS_VERSION_NUMBER / 10 % 10)
# define COMPILER_VERSION_PATCH DEC(__GHS_VERSION_NUMBER      % 10)
# endif

#elif defined(__TASKING__)
# define COMPILER_ID "Tasking"
  # define COMPILER_VERSION_MAJOR DEC(__VERSION__/1000)
  # define COMPILER_VERSION_MINOR DEC(__VERSION__ % 100)
# define COMPILER_VERSION_INTERNAL DEC(__VERSION__)

#elif defined(__SCO_VERSION__)
# define COMPILER_ID "SCO"

#elif defined(__ARMCC_VERSION) && !defined(__clang__)
# define COMPILER_ID "ARMCC"
#if __ARMCC_VERSION >= 1000000
  /* __ARMCC_VERSION = VRRPPPP */
  # define COMPILER_VERSION_MAJOR DEC(__ARMCC_VERSION/1000000)
  # define COMPILER_VERSION_MINOR DEC(__ARMCC_VERSION/10000 % 100)
  # define COMPILER_VERSION_PATCH DEC(__ARMCC_VERSION     % 10000)
#else
  /* __ARMCC_VERSION = VRPPPP */
  # define COMPILER_VERSION_MAJOR DEC(__ARMCC_VERSION/100000)
  # define COMPILER_VERSION_MINOR DEC(__ARMCC_VERSION/10000 % 10)
  # define COMPILER_VERSION_PATCH DEC(__ARMCC_VERSION    % 10000)
#endif


#elif defined(__clang__) && defined(__apple_build_version__)
# define COMPILER_ID "AppleClang"
# if defined(_MSC_VER)
#  define SIMULATE_ID "MSVC"
# endif
# define COMPILER_VERSION_MAJOR DEC(__clang_major__)
# define COMPILER_VERSION_MINOR DEC(__clang_minor__)
# define COMPILER_VERSION_PATCH DEC(__clang_patchlevel__)
# if defined(_MSC_VER)
   /* _MSC_VER = VVRR */
#  define SIMULATE_VERSION_MAJOR DEC(_MSC_VER / 100)
#  define SIMULATE_VERSION_MINOR DEC(_MSC_VER % 100)
# endif
# define COMPILER_VERSION_TWEAK DEC(__apple_build_version__)

#elif defined(__clang__) && defined(__ARMCOMPILER_VERSION)
# define COMPILER_ID "ARMClang"
  # define COMPILER_VERSION_MAJOR DEC(__ARMCOMPILER_VERSION/1000000)
  # define COMPILER_VERSION_MINOR DEC(__ARMCOMPILER_VERSION/10000 % 100)
  # define COMPILER_VERSION_PATCH DEC(__ARMCOMPILER_VERSION     % 10000)
# define COMPILER_VERSION_INTERNAL DEC(__ARMCOMPILER_VERSION)

#elif defined(__clang__)
# define COMPILER_ID "Clang"
# if defined(_MSC_VER)
#  define SIMULATE_ID "MSVC"
# endif
# define COMPILER_VERSION_MAJOR DEC(__clang_major__)
# define COMPILER_VERSION_MINOR DEC(__clang_minor__)
# define COMPILER_VERSION_PATCH DEC(__clang_patchlevel__)
# if defined(_MSC_VER)
   /* _MSC_VER = VVRR */
#  define SIMULATE_VERSION_MAJOR DEC(_MSC_VER / 100)
#  define SIMULATE_VERSION_MINOR DEC(_MSC_VER % 100)
# endif

#elif defined(__LCC__) && (defined(__GNUC__) || defined(__GNUG__) || defined(__MCST__))
# define COMPILER_ID "LCC"
# define COMPILER_VERSION_MAJOR DEC(__LCC__ / 100)
# define COMPILER_VERSION_MINOR DEC(__LCC__ % 100)
# if defined(__LCC_MINOR__)
#  define COMPILER_VERSION_PATCH DEC(__LCC_MINOR__)
# endif
# if defined(__GNUC__) && defined(__GNUC_MINOR__)
#  define SIMULATE_ID "GNU"
#  define SIMULATE_VERSION_MAJOR DEC(__GNUC__)
#  define SIMULATE_VERSION_MINOR DEC(__GNUC_MINOR__)
#  if defined(__GNUC_PATCHLEVEL__)
#   define SIMULATE_VERSION_PATCH DEC(__GNUC_PATCHLEVEL__)
#  endif
# endif

#elif defined(__GNUC__) || defined(__GNUG__)
# define COMPILER_ID "GNU"
# if defined(__GNUC__)
#  define COMPILER_VERSION_MAJOR DEC(__GNUC__)
# else
#  define COMPILER_VERSION_MAJOR DEC(__GNUG__)
# endif
# if defined(__GNUC_MINOR__)
#  define COMPILER_VERSION_MINOR DEC(__GNUC_MINOR__)
# endif
# if defined(__GNUC_PATCHLEVEL__)
#  define COMPILER_VERSION_PATCH DEC(__GNUC_PATCHLEVEL__)
# endif

#elif defined(_MSC_VER)
# define COMPILER_ID "MSVC"
  /* _MSC_VER = VVRR */
# define COMPILER_VERSION_MAJOR DEC(_MSC_VER / 100)
# define COMPILER_VERSION_MINOR DEC(_MSC_VER % 100)
# if defined(_MSC_FULL_VER)
#  if _MSC_VER >= 1400
    /* _MSC_FULL_VER = VVRRPPPPP */
#   define COMPILER_VERSION_PATCH DEC(_MSC_FULL_VER % 100000)
#  else
    /* _MSC_FULL_VER = VVRRPPPP */
#   define COMPILER_VERSION_PATCH DEC(_MSC_FULL_VER % 10000)
#  endif
# endif
# if defined(_MSC_BUILD)
#  define COMPILER_VERSION_TWEAK DEC(_MSC_BUILD)
# endif

#elif defined(_ADI_COMPILER)
# define COMPILER_ID "ADSP"
#if defined(__VERSIONNUM__)
  /* __VERSIONNUM__ = 0xVVRRPPTT */
#  define COMPILER_VERSION_MAJOR DEC(__VERSIONNUM__ >> 24 & 0xFF)
#  define COMPILER_VERSION_MINOR DEC(__VERSIONNUM__ >> 16 & 0xFF)
#  define COMPILER_VERSION_PATCH DEC(__VERSIONNUM__ >> 8 & 0xFF)
#  define COMPILER_VERSION_TWEAK DEC(__VERSIONNUM__ & 0xFF)
#endif

#elif defined(__IAR_SYSTEMS_ICC__) || defined(__IAR_SYSTEMS_ICC)
# define COMPILER_ID "IAR"
# if defined(__VER__) && defined(__ICCARM__)
#  define COMPILER_VERSION_MAJOR DEC((__VER__) / 1000000)
#  define COMPILER_VERSION_MINOR DEC(((__VER__) / 1000) % 1000)
#  define COMPILER_VERSION_PATCH DEC((__VER__) % 1000)
#  define COMPILER_VERSION_INTERNAL DEC(__IAR_SYSTEMS_ICC__)
# elif defined(__VER__) && (defined(__ICCAVR__) || defined(__ICCRX__) || defined(__ICCRH850__) || defined(__ICCRL78__) || defined(__ICC430__) || defined(__ICCRISCV__) || defined(__ICCV850__) || defined(__ICC8051__) || defined(__ICCSTM8__))
#  define COMPILER_VERSION_MAJOR DEC((__VER__) / 100)
#  define COMPILER_VERSION_MINOR DEC((__VER__) - (((__VER__) / 100)*100))
#  define COMPILER_VERSION_PATCH DEC(__SUBVERSION__)
#  define COMPILER_VERSION_INTERNAL DEC(__IAR_SYSTEMS_ICC__)
# endif


/* These compilers are either not known or too old to define an
  identification macro.  Try to identify the platform and guess that
  it is the native compiler.  */
#elif defined(__hpux) || defined(__hpua)
# define COMPILER_ID "HP"

#else /* unknown compiler */
# define COMPILER_ID ""
#endif

/* Construct the string literal in pieces to prevent the source from
   getting matched.  Store it in a pointer rather than an array
   because some compilers will just produce instructions to fill the
   array rather than assigning a pointer to a static array.  */
char const* info_compiler = "INFO" ":" "compiler[" COMPILER_ID "]";
#ifdef SIMULATE_ID
char const* info_simulate = "INFO" ":" "simulate[" SIMULATE_ID "]";
#endif

#ifdef __QNXNTO__
char const* qnxnto = "INFO" ":" "qnxnto[]";
#endif

#if defined(__CRAYXT_COMPUTE_LINUX_TARGET)
char const *info_cray = "INFO" ":" "compiler_wrapper[CrayPrgEnv]";
#endif

#define STRINGIFY_HELPER(X) #X
#define STRINGIFY(X) STRINGIFY_HELPER(X)

/* Identify known platforms by name.  */
#if defined(__linux) || defined(__linux__) || defined(linux)
# define PLATFORM_ID "Linux"

#elif defined(__MSYS__)
# define PLATFORM_ID "MSYS"

#elif defined(__CYGWIN__)
# define PLATFORM_ID "Cygwin"

#elif defined(__MINGW32__)
# define PLATFORM_ID "MinGW"

#elif defined(__APPLE__)
# define PLATFORM_ID "Darwin"

#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
# define PLATFORM_ID "Windows"

#elif defined(__FreeBSD__) || defined(__FreeBSD)
# define PLATFORM_ID "FreeBSD"

#elif defined(__NetBSD__) || defined(__NetBSD)
# define PLATFORM_ID "NetBSD"

#elif defined(__OpenBSD__) || defined(__OPENBSD)
# define PLATFORM_ID "OpenBSD"

#elif defined(__sun) || defined(sun)
# define PLATFORM_ID "SunOS"

#elif defined(_AIX) || defined(__AIX) || defined(__AIX__) || defined(__aix) || defined(__aix__)
# define PLATFORM_ID "AIX"

#elif defined(__hpux) || defined(__hpux__)
# define PLATFORM_ID "HP-UX"

#elif defined(__HAIKU__)
# define PLATFORM_ID "Haiku"

#elif defined(__BeOS) || defined(__BEOS__) || defined(_BEOS)
# define PLATFORM_ID "BeOS"

#elif defined(__QNX__) || defined(__QNXNTO__)
# define PLATFORM_ID "QNX"

#elif defined(__tru64) || defined(_tru64) || defined(__TRU64__)
# define PLATFORM_ID "Tru64"

#elif defined(__riscos) || defined(__riscos__)
# define PLATFORM_ID "RISCos"

#elif defined(__sinix) || defined(__sinix__) || defined(__SINIX__)
# define PLATFORM_ID "SINIX"

#elif defined(__UNIX_SV__)
# define PLATFORM_ID "UNIX_SV"

#elif defined(__bsdos__)
# define PLATFORM_ID "BSDOS"

#elif defined(_MPRAS) || defined(MPRAS)
# define PLATFORM_ID "MP-RAS"

#elif defined(__osf) || defined(__osf__)
# define PLATFORM_ID "OSF1"

#elif defined(_SCO_SV) || defined(SCO_SV) || defined(sco_sv)
# define PLATFORM_ID "SCO_SV"

#elif defined(__ultrix) || defined(__ultrix__) || defined(_ULTRIX)
# define PLATFORM_ID "ULTRIX"

#elif defined(__XENIX__) || defined(_XENIX) || defined(XENIX)
# define PLATFORM_ID "Xenix"

#elif defined(__WATCOMC__)
# if defined(__LINUX__)
#  define PLATFORM_ID "Linux"

# elif defined(__DOS__)
#  define PLATFORM_ID "DOS"

# elif defined(__OS2__)
#  define PLATFORM_ID "OS2"

# elif defined(__WINDOWS__)
#  define PLATFORM_ID "Windows3x"

# elif defined(__VXWORKS__)
#  define PLATFORM_ID "VxWorks"

# else /* unknown platform */
#  define PLATFORM_ID
# endif

#elif defined(__INTEGRITY)
# if defined(INT_178B)
#  define PLATFORM_ID "Integrity178"

# else /* regular Integrity */
#  define PLATFORM_ID "Integrity"
# endif

# elif defined(_ADI_COMPILER)
#  define PLATFORM_ID "ADSP"

#else /* unknown platform */
# define PLATFORM_ID

#endif

/* For windows compilers MSVC and Intel we can determine
   the architecture of the compiler being used.  This is because
   the compilers do not have flags that can change the architecture,
   but rather depend on which compiler is being used
*/
#if defined(_WIN32) && defined(_MSC_VER)
# if defined(_M_IA64)
#  define ARCHITECTURE_ID "IA64"

# elif defined(_M_ARM64EC)
#  define ARCHITECTURE_ID "ARM64EC"

# elif defined(_M_X64) || defined(_M_AMD64)
#  define ARCHITECTURE_ID "x64"

# elif defined(_M_IX86)
#  define ARCHITECTURE_ID "X86"

# elif defined(_M_ARM64)
#  define ARCHITECTURE_ID "ARM64"

# elif defined(_M_ARM)
#  if _M_ARM == 4
#   define ARCHITECTURE_ID "ARMV4I"
#  elif _M_ARM == 5
#   define ARCHITECTURE_ID "ARMV5I"
#  else
#   define ARCHITECTURE_ID "ARMV" STRINGIFY(_M_ARM)
#  endif

# elif defined(_M_MIPS)
#  define ARCHITECTURE_ID "MIPS"

# elif defined(_M_SH)
#  define ARCHITECTURE_ID "SHx"

# else /* unknown architecture */
#  define ARCHITECTURE_ID ""
# endif

#elif defined(__WATCOMC__)
# if defined(_M_I86)
#  define ARCHITECTURE_ID "I86"

# elif defined(_M_IX86)
#  define ARCHITECTURE_ID "X86"

# else /* unknown architecture */
#  define ARCHITECTURE_ID ""
# endif

#elif defined(__IAR_SYSTEMS_ICC__) || defined(__IAR_SYSTEMS_ICC)
# if defined(__ICCARM__)
#  define ARCHITECTURE_ID "ARM"

# elif defined(__ICCRX__)
#  define ARCHITECTURE_ID "RX"

# elif defined(__ICCRH850__)
#  define ARCHITECTURE_ID "RH850"

# elif defined(__ICCRL78__)
#  define ARCHITECTURE_ID "RL78"

# elif defined(__ICCRISCV__)
#  define ARCHITECTURE_ID "RISCV"

# elif defined(__ICCAVR__)
#  define ARCHITECTURE_ID "AVR"

# elif defined(__ICC430__)
#  define ARCHITECTURE_ID "MSP430"

# elif defined(__ICCV850__)
#  define ARCHITECTURE_ID "V850"

# elif defined(__ICC8051__)
#  define ARCHITECTURE_ID "8051"

# elif defined(__ICCSTM8__)
#  define ARCHITECTURE_ID "STM8"

# else /* unknown architecture */
#  define ARCHITECTURE_ID ""
# endif

#elif defined(__ghs__)
# if defined(__PPC64__)
#  define ARCHITECTURE_ID "PPC64"

# elif defined(__ppc__)
#  define ARCHITECTURE_ID "PPC"

# elif defined(__ARM__)
#  define ARCHITECTURE_ID "ARM"

# elif defined(__x86_64__)
#  define ARCHITECTURE_ID "x64"

# elif defined(__i386__)
#  define ARCHITECTURE_ID "X86"

# else /* unknown architecture */
#  define ARCHITECTURE_ID ""
# endif

#elif defined(__TI_COMPILER_VERSION__)
# if defined(__TI_ARM__)
#  define ARCHITECTURE_ID "ARM"

# elif defined(__MSP430__)
#  define ARCHITECTURE_ID "MSP430"

# elif defined(__TMS320C28XX__)
#  define ARCHITECTURE_ID "TMS320C28x"

# elif defined(__TMS320C6X__) || defined(_TMS320C6X)
#  define ARCHITECTURE_ID "TMS320C6x"

# else /* unknown architecture */
#  define ARCHITECTURE_ID ""
# endif

# elif defined(__ADSPSHARC__)
#  define ARCHITECTURE_ID "SHARC"

# elif defined(__ADSPBLACKFIN__)
#  define ARCHITECTURE_ID "Blackfin"

#elif defined(__TASKING__)

# if defined(__CTC__) || defined(__CPTC__)
#  define ARCHITECTURE_ID "TriCore"

# elif defined(__CMCS__)
#  define ARCHITECTURE_ID "MCS"

# elif defined(__CARM__)
#  define ARCHITECTURE_ID "ARM"

# elif defined(__CARC__)
#  define ARCHITECTURE_ID "ARC"

# elif defined(__C51__)
#  define ARCHITECTURE_ID "8051"

# elif defined(__CPCP__)
#  define ARCHITECTURE_ID "PCP"

# else
#  define ARCHITECTURE_ID ""
# endif

#else
#  define ARCHITECTURE_ID
#endif

/* Convert integer to decimal digit literals.  */
#define DEC(n)                   \
  ('0' + (((n) / 10000000)%10)), \
  ('0' + (((n) / 1000000)%10)),  \
  ('0' + (((n) / 100000)%10)),   \
  ('0' + (((n) / 10000)%10)),    \
  ('0' + (((n) / 1000)%10)),     \
  ('0' + (((n) / 100)%10)),      \
  ('0' + (((n) / 10)%10)),       \
  ('0' +  ((n) % 10))

/* Convert integer to hex digit literals.  */
#define HEX(n)             \
  ('0' + ((n)>>28 & 0xF)), \
  ('0' + ((n)>>24 & 0xF)), \
  ('0' + ((n)>>20 & 0xF)), \
  ('0' + ((n)>>16 & 0xF)), \
  ('0' + ((n)>>12 & 0xF)), \
  ('0' + ((n)>>8  & 0xF)), \
  ('0' + ((n)>>4  & 0xF)), \
  ('0' + ((n)     & 0xF))

/* Construct a string literal encoding the version number. */
#ifdef COMPILER_VERSION
char const* info_version = "INFO" ":" "compiler_version[" COMPILER_VERSION "]";

/* Construct a string literal encoding the version number components. */
#elif defined(COMPILER_VERSION_MAJOR)
char const info_version[] = {
  'I', 'N', 'F', 'O', ':',
  'c','o','m','p','i','l','e','r','_','v','e','r','s','i','o','n','[',
  COMPILER_VERSION_MAJOR,
# ifdef COMPILER_VERSION_MINOR
  '.', COMPILER_VERSION_MINOR,
#  ifdef COMPILER_VERSION_PATCH
   '.', COMPILER_VERSION_PATCH,
#   ifdef COMPILER_VERSION_TWEAK
    '.', COMPILER_VERSION_TWEAK,
#   endif
#  endif
# endif
  ']','\0'};
#endif

/* Construct a string literal encoding the internal version number. */
#ifdef COMPILER_VERSION_INTERNAL
char const info_version_internal[] = {
  'I', 'N', 'F', 'O', ':',
  'c','o','m','p','i','l','e','r','_','v','e','r','s','i','o','n','_',
  'i','n','t','e','r','n','a','l','[',
  COMPILER_VERSION_INTERNAL,']','\0'};
#elif defined(COMPILER_VERSION_INTERNAL_STR)
char const* info_version_internal = "INFO" ":" "compiler_version_internal[" COMPILER_VERSION_INTERNAL_STR "]";
#endif

/* Construct a string literal encoding the version number components. */
#ifdef SIMULATE_VERSION_MAJOR
char const info_simulate_version[] = {
  'I', 'N', 'F', 'O', ':',
  's','i','m','u','l','a','t','e','_','v','e','r','s','i','o','n','[',
  SIMULATE_VERSION_MAJOR,
# ifdef SIMULATE_VERSION_MINOR
  '.', SIMULATE_VERSION_MINOR,
#  ifdef SIMULATE_VERSION_PATCH
   '.', SIMULATE_VERSION_PATCH,
#   ifdef SIMULATE_VERSION_TWEAK
    '.', SIMULATE_VERSION_TWEAK,
#   endif
#  endif
# endif
  ']','\0'};
#endif

/* Construct the string literal in pieces to prevent the source from
   getting matched.  Store it in a pointer rather than an array
   because some compilers will just produce instructions to fill the
   array rather than assigning a pointer to a static array.  */
char const* info_platform = "INFO" ":" "platform[" PLATFORM_ID "]";
char const* info_arch = "INFO" ":" "arch[" ARCHITECTURE_ID "]";



#if defined(__INTEL_COMPILER) && defined(_MSVC_LANG) && _MSVC_LANG < 201403L
#  if defined(__INTEL_CXX11_MODE__)
#    if defined(__cpp_aggregate_nsdmi)
#      define CXX_STD 201402L
#    else
#      define CXX_STD 201103L
#    endif
#  else
#    define CXX_STD 199711L
#  endif
#elif defined(_MSC_VER) && defined(_MSVC_LANG)
#  define CXX_STD _MSVC_LANG
#else
#  define CXX_STD __cplusplus
#endif

const char* info_language_standard_default = "INFO" ":" "standard_default["
#if CXX_STD > 202002L
  "23"
#elif CXX_STD > 201703L
  "20"
#elif CXX_STD >= 201703L
  "17"
#elif CXX_STD >= 201402L
  "14"
#elif CXX_STD >= 201103L
  "11"
#else
  "98"
#endif
"]";

const char* info_language_extensions_default = "INFO" ":" "extensions_default["
#if (defined(__clang__) || defined(__GNUC__) || defined(__xlC__) ||           \
     defined(__TI_COMPILER_VERSION__)) &&                                     \
  !defined(__STRICT_ANSI__)
  "ON"
#else
  "OFF"
#endif
"]";

/*--------------------------------------------------------------------------*/

int main(int argc, char* argv[])
{
  int require = 0;
  require += info_compiler[argc];
  require += info_platform[argc];
  require += info_arch[argc];
#ifdef COMPILER_VERSION_MAJOR
  require += info_version[argc];
#endif
#ifdef COMPILER_VERSION_INTERNAL
  require += info_version_internal[argc];
#endif
#ifdef SIMULATE_ID
  require += info_simulate[argc];
#endif
#ifdef SIMULATE_VERSION_MAJOR
  require += info_simulate_version[argc];
#endif
#if defined(__CRAYXT_COMPUTE_LINUX_TARGET)
  require += info_cray[argc];
#endif
  require += info_language_standard_default[argc];
  require += info_language_extensions_default[argc];
  (void)argv;
  return require;
}

```

# File: tests\build-tests\Testing\Temporary\CTestCostData.txt
```
CoreLogicTest 0 0
---
CoreLogicTest

```
