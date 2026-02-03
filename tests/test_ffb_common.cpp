#include "test_ffb_common.h"

namespace FFBEngineTests {

// --- Global Test Counters ---
int g_tests_passed = 0;
int g_tests_failed = 0;

// --- Helper: Create Basic Test Telemetry ---
TelemInfoV01 CreateBasicTestTelemetry(double speed, double slip_angle) {
    TelemInfoV01 data;
    std::memset(&data, 0, sizeof(data));
    
    // Time
    data.mDeltaTime = 0.01; // 100Hz
    
    // Velocity
    data.mLocalVel.z = -speed; // Game uses -Z for forward
    
    // Wheel setup (all 4 wheels)
    for (int i = 0; i < 4; i++) {
        data.mWheel[i].mGripFract = 0.0; // Trigger approximation mode
        data.mWheel[i].mTireLoad = 4000.0; // Realistic load
        data.mWheel[i].mStaticUndeflectedRadius = 30; // 0.3m radius
        data.mWheel[i].mRotation = speed * 3.33f; // Match speed (rad/s)
        data.mWheel[i].mLongitudinalGroundVel = speed;
        data.mWheel[i].mLateralPatchVel = slip_angle * speed; // Convert to m/s
        data.mWheel[i].mBrakePressure = 1.0; // Default for tests (v0.6.0)
        data.mWheel[i].mSuspForce = 4000.0; // Grounded (v0.6.0)
        data.mWheel[i].mVerticalTireDeflection = 0.001; // Avoid "missing data" warning (v0.6.21)
    }
    
    return data;
}

// --- Helper: Initialize Engine with Test Defaults ---
void InitializeEngine(FFBEngine& engine) {
    Preset::ApplyDefaultsToEngine(engine);
    // v0.5.12: Force consistent baseline for legacy tests
    engine.m_max_torque_ref = 20.0f;
    engine.m_invert_force = false;
    
    // v0.6.31: Zero out all auxiliary effects for clean physics testing by default.
    // Individual tests can re-enable what they need.
    engine.m_steering_shaft_smoothing = 0.0f; 
    engine.m_slip_angle_smoothing = 0.0f;
    engine.m_sop_smoothing_factor = 1.0f; // 1.0 = Instant/No smoothing
    engine.m_yaw_accel_smoothing = 0.0f;
    engine.m_gyro_smoothing = 0.0f;
    engine.m_chassis_inertia_smoothing = 0.0f;
    
    engine.m_sop_effect = 0.0f;
    engine.m_sop_yaw_gain = 0.0f;
    engine.m_oversteer_boost = 0.0f;
    engine.m_rear_align_effect = 0.0f;
    engine.m_gyro_gain = 0.0f;
    
    engine.m_slide_texture_enabled = false;
    engine.m_road_texture_enabled = false;
    engine.m_lockup_enabled = false;
    engine.m_spin_enabled = false;
    engine.m_abs_pulse_enabled = false;
    engine.m_scrub_drag_gain = 0.0f;
    engine.m_min_force = 0.0f;
    
    // v0.6.25: Disable speed gate by default for legacy tests (avoids muting physics at 0 speed)
    engine.m_speed_gate_lower = -10.0f;
    engine.m_speed_gate_upper = -5.0f;
}

void Run() {
    std::cout << "\n--- FFTEngine Regression Suite ---" << std::endl;
    
    // Categorized Runners
    Run_CorePhysics();
    Run_SlopeDetection();
    Run_Understeer();
    Run_SpeedGate();
    Run_YawGyro();
    Run_Coordinates();
    Run_Texture(); // Features
    Run_Config();
    Run_SlipGrip();
    Run_Internal();

    std::cout << "\n--- Physics Engine Test Summary ---" << std::endl;
    std::cout << "Tests Passed: " << g_tests_passed << std::endl;
    std::cout << "Tests Failed: " << g_tests_failed << std::endl;
}

} // namespace FFBEngineTests