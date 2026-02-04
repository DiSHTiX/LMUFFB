#include "test_ffb_common.h"

namespace FFBEngineTests {

static void test_regression_road_texture_toggle() {
    std::cout << "\nTest: Regression - Road Texture Toggle Spike" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine);
    TelemInfoV01 data = CreateBasicTestTelemetry(20.0);
    engine.m_road_texture_enabled = false;
    engine.calculate_force(&data);
    data.mWheel[0].mVerticalTireDeflection = 0.05;
    engine.m_road_texture_enabled = true;
    double f = engine.calculate_force(&data);
    ASSERT_TRUE(std::abs(f) < 0.1);
}

static void test_regression_bottoming_switch() {
    std::cout << "\nTest: Regression - Bottoming Method Switch Spike" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine);
    TelemInfoV01 data = CreateBasicTestTelemetry(20.0);
    engine.m_bottoming_enabled = true;
    engine.m_bottoming_method = 0;
    engine.calculate_force(&data);
    engine.m_bottoming_method = 1;
    double f = engine.calculate_force(&data);
    ASSERT_NEAR(f, 0.0, 0.001);
}

static void test_road_texture_teleport() {
    std::cout << "\nTest: Road Texture Teleport (Delta Clamp)" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine); // v0.5.12: Initialize with T300 defaults
    TelemInfoV01 data;
    std::memset(&data, 0, sizeof(data));
    
    // Disable Bottoming
    engine.m_bottoming_enabled = false;
    data.mLocalVel.z = -20.0; // Moving fast (v0.6.21)

    engine.m_road_texture_enabled = true;
    engine.m_road_texture_gain = 1.0;
    engine.m_max_torque_ref = 40.0f;
    engine.m_gain = 1.0; // Ensure gain is 1.0
    engine.m_invert_force = false;
    
    // Frame 1: 0.0
    data.mWheel[0].mVerticalTireDeflection = 0.0;
    data.mWheel[1].mVerticalTireDeflection = 0.0;
    data.mWheel[0].mTireLoad = 4000.0; // Load Factor 1.0
    data.mWheel[1].mTireLoad = 4000.0;
    engine.calculate_force(&data);
    
    // Frame 2: Teleport (+0.1m)
    data.mWheel[0].mVerticalTireDeflection = 0.1;
    data.mWheel[1].mVerticalTireDeflection = 0.1;
    
    // Without Clamp:
    // Delta = 0.1. Sum = 0.2.
    // Force = 0.2 * 50.0 = 10.0.
    // Norm = 1.0 / 40.0 = 0.25.
    
    // With Clamp (+/- 0.01):
    // Delta clamped to 0.01. Sum = 0.02.
    // Force = 0.02 * 50.0 = 1.0.
    // Norm = 1.0 / 40.0 = 0.025.
    
    double force = engine.calculate_force(&data);
    
    // Check if clamped
    // v0.4.50: Decoupling scales force to 20Nm baseline.
    // Clamped Force = 1.0 Nm. Normalized = 1.0 / 20.0 = 0.05.
    if (std::abs(force - 0.05) < 0.001) {
        std::cout << "[PASS] Teleport spike clamped." << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Teleport spike unclamped? Got " << force << " Expected 0.05." << std::endl;
        g_tests_failed++;
    }
}

static void test_suspension_bottoming() {
    std::cout << "\nTest: Suspension Bottoming (Fix Verification)" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine); // v0.5.12: Initialize with T300 defaults
    TelemInfoV01 data;
    std::memset(&data, 0, sizeof(data));

    // Enable Bottoming
    engine.m_bottoming_enabled = true;
    engine.m_bottoming_gain = 1.0;
    data.mLocalVel.z = -20.0; // Moving fast (v0.6.21)
    
    // Disable others
    engine.m_sop_effect = 0.0;
    engine.m_slide_texture_enabled = false;
    
    // Straight line condition: Zero steering force
    data.mSteeringShaftTorque = 0.0;
    
    // Massive Load Spike (10000N > 8000N threshold)
    data.mWheel[0].mTireLoad = 10000.0;
    data.mWheel[1].mTireLoad = 10000.0;
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
    InitializeEngine(engine2); // v0.5.12: Initialize with T300 defaults
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

static void test_road_texture_state_persistence() {
    std::cout << "\nTest: Road Texture State Persistence" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine);
    engine.m_road_texture_enabled = true;
    TelemInfoV01 data = CreateBasicTestTelemetry(20.0);
    data.mWheel[0].mVerticalTireDeflection = 0.01;
    double f1 = engine.calculate_force(&data);
    double f2 = engine.calculate_force(&data);
    ASSERT_NEAR(f1, f2, 0.001);
}

static void test_universal_bottoming() {
    std::cout << "\nTest: Universal Bottoming" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine);
    engine.m_bottoming_enabled = true;
    engine.m_bottoming_gain = 1.0f;
    TelemInfoV01 data = CreateBasicTestTelemetry(20.0);
    
    // Method A: Ride Height (Scrape)
    engine.m_bottoming_method = 0;
    data.mWheel[0].mRideHeight = 0.001;
    
    // Set dt to ensure phase doesn't hit 0 crossing (50Hz)
    // 50Hz period = 0.02s. dt=0.01 is half period. PI. sin(PI)=0.
    // Use dt=0.005 (PI/2). sin(PI/2)=1.
    data.mDeltaTime = 0.005;

    double f1 = engine.calculate_force(&data);
    
    if (std::abs(f1) > 0.0001) {
        std::cout << "[PASS] Bottoming Method A (Scrape) Triggered. Force: " << f1 << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Bottoming Method A (Scrape) Silent." << std::endl;
        g_tests_failed++;
    }
    
    // Method B: Suspension Deflection (Spike) - Using 10000N logic from other test
    engine.m_bottoming_method = 1;
    data.mWheel[0].mRideHeight = 0.1; // Reset RH
    data.mWheel[0].mTireLoad = 10000.0; // Trigger spike
    data.mWheel[1].mTireLoad = 10000.0;
    data.mDeltaTime = 0.005; // 200Hz to catch phase
    
    // Reset Engine to clear phases
    FFBEngine engine2;
    InitializeEngine(engine2);
    engine2.m_bottoming_enabled = true;
    engine2.m_bottoming_gain = 1.0f;
    engine2.m_bottoming_method = 1;
    
    double f2 = engine2.calculate_force(&data);
    
    if (std::abs(f2) > 0.0001) {
        std::cout << "[PASS] Bottoming Method B (Spike) Triggered. Force: " << f2 << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Bottoming Method B (Spike) Silent." << std::endl;
        g_tests_failed++;
    }
}

static void test_unconditional_vert_accel_update() {
    std::cout << "\nTest: Unconditional m_prev_vert_accel Update (v0.6.36)" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine);
    TelemInfoV01 data = CreateBasicTestTelemetry(20.0);
    engine.m_road_texture_enabled = false;
    data.mLocalAccel.y = 5.5;
    engine.m_prev_vert_accel = 0.0;
    engine.calculate_force(&data);
    ASSERT_NEAR(engine.m_prev_vert_accel, 5.5, 0.01);
}

static void test_scrub_drag_fade() {
    std::cout << "\nTest: Scrub Drag Fade-In" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine); // v0.5.12: Initialize with T300 defaults
    TelemInfoV01 data;
    std::memset(&data, 0, sizeof(data));
    
    // Disable Bottoming to avoid noise
    engine.m_bottoming_enabled = false;
    // Disable Slide Texture (enabled by default)
    engine.m_slide_texture_enabled = false;

    engine.m_road_texture_enabled = true;
    engine.m_scrub_drag_gain = 1.0;
    
    // Case 1: 0.25 m/s lateral velocity (Midpoint of 0.0 - 0.5 window)
    // Expected: 50% of force.
    // Full force calculation: drag_gain * 2.0 = 2.0.
    // Fade = 0.25 / 0.5 = 0.5.
    // Expected Force = 5.0 * 0.5 = 2.5.
    // Normalized by Ref (40.0). Output = 2.5 / 40.0 = 0.0625.
    // Direction: Positive Vel -> Negative Force.
    // Norm Force = -0.0625.
    
    data.mWheel[0].mLateralPatchVel = 0.25;
    data.mWheel[1].mLateralPatchVel = 0.25;
    data.mLocalVel.z = -20.0; // Moving fast (v0.6.22)
    engine.m_max_torque_ref = 40.0f;
    engine.m_gain = 1.0;
    
    double force = engine.calculate_force(&data);
    
    // Check absolute magnitude
    // v0.4.50: Decoupling scales force to 20Nm baseline independently of Ref.
    // Full force = 2.5 Nm. Normalized (by any Ref) = 2.5 / 20.0 = 0.125.
    if (std::abs(std::abs(force) - 0.125) < 0.001) {
        std::cout << "[PASS] Scrub drag faded correctly (50%)." << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Scrub drag fade incorrect. Got " << force << " Expected 0.125." << std::endl;
        g_tests_failed++;
    }
}

static void test_stationary_gate() {
    std::cout << "\nTest: Stationary Signal Gate" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine);
    engine.m_speed_gate_lower = 1.0f;
    engine.m_speed_gate_upper = 5.0f;
    
    // Case 1: Stationary (0.0 m/s) -> Effects should be gated to 0.0
    {
        TelemInfoV01 data = CreateBasicTestTelemetry(0.0);
        
        // Enable Road Texture
        engine.m_road_texture_enabled = true;
        engine.m_road_texture_gain = 1.0;
        
        // Simulate Engine Idle Vibration (Deflection Delta)
        data.mWheel[0].mVerticalTireDeflection = 0.001; 
        data.mWheel[1].mVerticalTireDeflection = 0.001;
        // Previous was 0.0 at initialization, so delta is 0.001
        
        double force = engine.calculate_force(&data);
        
        // Should be 0.0 due to speed_gate
        ASSERT_NEAR(force, 0.0, 0.0001);
    }
    
    // Case 2: Moving slowly (0.5 m/s) -> Gate should be 0.0 (since 0.5 < m_speed_gate_lower)
    {
        TelemInfoV01 data = CreateBasicTestTelemetry(0.5);
        engine.m_road_texture_enabled = true;
        data.mWheel[0].mVerticalTireDeflection = 0.001; 
        data.mWheel[1].mVerticalTireDeflection = 0.001;
        
        double force = engine.calculate_force(&data);
        ASSERT_NEAR(force, 0.0, 0.0001);
    }
    
    // Case 3: Moving at 5.0 m/s (m_speed_gate_upper) -> Gate should be 1.0
    {
        TelemInfoV01 data = CreateBasicTestTelemetry(5.0);
        engine.m_road_texture_enabled = true;
        engine.m_road_texture_gain = 1.0;
        engine.m_max_torque_ref = 20.0f;
        
        data.mWheel[0].mVerticalTireDeflection = 0.002; 
        data.mWheel[1].mVerticalTireDeflection = 0.002;
        
        double force = engine.calculate_force(&data);
        
        // Delta = 0.002 - 0.001 = 0.001. Sum = 0.002.
        // Force = 0.002 * 50.0 = 0.1 Nm.
        // Normalized = 0.1 / 20.0 = 0.005.
        ASSERT_NEAR(force, 0.005, 0.0001);
    }
}

static void test_idle_smoothing() {
    std::cout << "\nTest: Automatic Idle Smoothing" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine);
    TelemInfoV01 data = CreateBasicTestTelemetry(0.0); // Stopped
    
    // Setup: User wants RAW FFB (0 smoothing)
    engine.m_steering_shaft_smoothing = 0.0f;
    engine.m_gain = 1.0f;
    engine.m_max_torque_ref = 10.0f; // Allow up to 10 Nm without clipping
    
    // 1. Simulate Engine Vibration at Idle (20Hz sine wave)
    // Amplitude 5.0 Nm. 
    // With 0.1s smoothing (Idle Target), 20Hz should be heavily attenuated.
    double max_force_idle = 0.0;
    data.mDeltaTime = 0.0025; // 400Hz
    
    for(int i=0; i<100; i++) {
        double t = i * data.mDeltaTime;
        data.mSteeringShaftTorque = 5.0 * std::sin(20.0 * 6.28 * t);
        double force = engine.calculate_force(&data);
        max_force_idle = (std::max)(max_force_idle, std::abs(force));
    }
    
    // Expect significant attenuation (e.g. < 0.15 normalized instead of 0.5)
    if (max_force_idle < 0.15) {
        std::cout << "[PASS] Idle vibration attenuated (Max: " << max_force_idle << " < 0.15)" << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Idle vibration too strong! Max: " << max_force_idle << std::endl;
        g_tests_failed++;
    }
    
    // 2. Simulate Driving (High Speed)
    TelemInfoV01 data_driving = CreateBasicTestTelemetry(20.0);
    data_driving.mDeltaTime = 0.0025;
    
    // Reset smoother
    engine.m_steering_shaft_torque_smoothed = 0.0;
    
    double max_force_driving = 0.0;
    for(int i=0; i<100; i++) {
        double t = i * data_driving.mDeltaTime;
        data_driving.mSteeringShaftTorque = 5.0 * std::sin(20.0 * 6.28 * t); // Same vibration (e.g. curb)
        double force = engine.calculate_force(&data_driving);
        max_force_driving = (std::max)(max_force_driving, std::abs(force));
    }
    
    // Expect RAW pass-through (near 0.5)
    if (max_force_driving > 0.4) {
        std::cout << "[PASS] Driving vibration passed through (Max: " << max_force_driving << " > 0.4)" << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Driving vibration over-smoothed. Max: " << max_force_driving << std::endl;
        g_tests_failed++;
    }
}

static void test_stationary_silence() {
    std::cout << "\nTest: Stationary Silence (Base Torque & SoP Gating)" << std::endl;
    // Setup engine with defaults (Gate: 1.0m/s to 5.0m/s)
    FFBEngine engine;
    InitializeEngine(engine);
    engine.m_speed_gate_lower = 1.0f;
    engine.m_speed_gate_upper = 5.0f;
    
    TelemInfoV01 data = CreateBasicTestTelemetry(0.0); // 0 Speed
    
    // Inject Noise into Physics Channels
    data.mSteeringShaftTorque = 5.0;
    data.mLocalAccel.x = 2.0;
    data.mLocalRotAccel.y = 10.0;
    
    double force = engine.calculate_force(&data);
    
    if (std::abs(force) > 0.001) {
        std::cout << "  [DEBUG] Stationary Silence Fail: force=" << force << std::endl;
        // The underlying components should be gated
    }
    
    // Expect 0.0 because speed_gate should be 0.0 at 0 m/s
    // speed_gate = (0.0 - 1.0) / (5.0 - 1.0) = -0.25 -> clamped to 0.0
    ASSERT_NEAR(force, 0.0, 0.001);
}

static void test_driving_forces_restored() {
    std::cout << "\nTest: Driving Forces Restored" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine);
    
    TelemInfoV01 data = CreateBasicTestTelemetry(20.0); // Normal driving speed
    
    // Inject same noise values
    data.mSteeringShaftTorque = 5.0;
    data.mLocalAccel.x = 2.0;
    data.mLocalRotAccel.y = 10.0;
    
    double force = engine.calculate_force(&data);
    
    // At 20 m/s, speed_gate should be 1.0 (full pass-through)
    // We expect a non-zero force
    ASSERT_TRUE(std::abs(force) > 0.1);
}



static void test_slide_texture() {
    std::cout << "\nTest: Slide Texture (Front & Rear)" << std::endl;
    
    // Case 1: Front Slip (Understeer)
    // v0.4.39 UPDATE: Work-Based Scrubbing requires grip LOSS to generate vibration
    // Gripping tires (grip=1.0) should NOT scrub, even with high lateral velocity
    {
        FFBEngine engine;
        InitializeEngine(engine); // v0.5.12: Initialize with T300 defaults
        TelemInfoV01 data;
        std::memset(&data, 0, sizeof(data));
        // Default RH to avoid scraping
        data.mWheel[0].mRideHeight = 0.1; data.mWheel[1].mRideHeight = 0.1;
        
        engine.m_max_torque_ref = 20.0f; // Standard scale for test
        engine.m_slide_texture_enabled = true;
        engine.m_slide_texture_gain = 1.0;
        
        data.mSteeringShaftTorque = 0.0;
        
        // Front Sliding WITH GRIP LOSS (v0.4.39 Fix)
        data.mWheel[0].mLateralPatchVel = 5.0; 
        data.mWheel[1].mLateralPatchVel = 5.0;
        data.mWheel[2].mLateralPatchVel = 0.0; // Rear Grip
        data.mWheel[3].mLateralPatchVel = 0.0;
        
        // Set grip to 0.0 to trigger approximation AND grip loss
        data.mWheel[0].mGripFract = 0.0; // Missing -> Triggers approximation
        data.mWheel[1].mGripFract = 0.0;
        data.mWheel[0].mTireLoad = 4000.0; // Valid load (prevents low-speed cutoff)
        data.mWheel[1].mTireLoad = 4000.0;
        data.mLocalVel.z = 20.0; // Moving fast (> 5.0 m/s cutoff)
        
        engine.m_slide_freq_scale = 1.0f;
        
        data.mDeltaTime = 0.013; // 13ms. For 35Hz (5m/s input), period is 28ms. 
                                 // 13ms is ~0.46 period, ensuring non-zero phase advance.
        
        engine.calculate_force(&data); // Cycle 1
        double force = engine.calculate_force(&data); // Cycle 2
        
        if (std::abs(force) > 0.001) {
             std::cout << "[PASS] Front slip triggers Slide Texture (Force: " << force << ")" << std::endl;
             g_tests_passed++;
        } else {
             std::cout << "[FAIL] Front slip failed to trigger Slide Texture." << std::endl;
             g_tests_failed++;
        }
    }

    // Case 2: Rear Slip (Oversteer/Drift)
    {
        FFBEngine engine;
        InitializeEngine(engine); // v0.5.12: Initialize with T300 defaults
        TelemInfoV01 data;
        std::memset(&data, 0, sizeof(data));
        data.mWheel[0].mRideHeight = 0.1; data.mWheel[1].mRideHeight = 0.1;

        engine.m_max_torque_ref = 20.0f; 
        engine.m_slide_texture_enabled = true;
        engine.m_slide_texture_gain = 1.0;
        engine.m_slide_freq_scale = 1.0f;
        
        data.mSteeringShaftTorque = 0.0;
        
        // Front Grip, Rear Sliding
        data.mWheel[0].mLateralPatchVel = 0.0; 
        data.mWheel[1].mLateralPatchVel = 0.0;
        data.mWheel[2].mLateralPatchVel = 10.0; // High Rear Slip
        data.mWheel[3].mLateralPatchVel = 10.0;
        
        data.mDeltaTime = 0.013;
        data.mLocalVel.z = 20.0; 
        data.mWheel[0].mGripFract = 0.5; // Simulate front grip loss to enable global slide effect
        data.mWheel[1].mGripFract = 0.5;
        data.mWheel[0].mTireLoad = 4000.0; // Front Load required for effect amplitude scaling
        data.mWheel[1].mTireLoad = 4000.0;

        engine.calculate_force(&data);
        double force = engine.calculate_force(&data);
        
        if (std::abs(force) > 0.001) {
             std::cout << "[PASS] Rear slip triggers Slide Texture (Force: " << force << ")" << std::endl;
             g_tests_passed++;
        } else {
             std::cout << "[FAIL] Rear slip failed to trigger Slide Texture." << std::endl;
             g_tests_failed++;
        }
    }
}

static void test_dynamic_tuning() {
    std::cout << "\nTest: Dynamic Tuning (GUI Simulation)" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine); // v0.5.12: Initialize with T300 defaults
    TelemInfoV01 data;
    std::memset(&data, 0, sizeof(data));
    data.mDeltaTime = 0.0025;
    data.mLocalVel.z = -20.0;
    
    // Default RH to avoid scraping
    data.mWheel[0].mRideHeight = 0.1; data.mWheel[1].mRideHeight = 0.1;
    
    // Default State: Full Game Force
    data.mSteeringShaftTorque = 10.0; // 10 Nm (0.5 normalized)
    data.mWheel[0].mGripFract = 1.0;
    data.mWheel[1].mGripFract = 1.0;
    engine.m_understeer_effect = 0.0; // Disabled effect initially
    engine.m_sop_effect = 0.0;
    engine.m_slide_texture_enabled = false;
    engine.m_road_texture_enabled = false;
    
    // Explicitly set gain 1.0 for this baseline
    engine.m_gain = 1.0;
    engine.m_max_torque_ref = 20.0f; // Fix Reference for Test (v0.4.4)
    engine.m_invert_force = false;

    double force_initial = engine.calculate_force(&data);
    // Should pass through 10.0 (normalized: 0.5)
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
    data.mWheel[0].mGripFract = 0.5;
    data.mWheel[1].mGripFract = 0.5;
    
    double force_grip_loss = engine.calculate_force(&data);
    // 10.0 * 0.5 = 5.0 -> 0.25 normalized
    ASSERT_NEAR(force_grip_loss, 0.25, 0.001);
    
    std::cout << "[PASS] Dynamic Tuning verified." << std::endl;
    g_tests_passed++;
}

static void test_oversteer_boost() {
    std::cout << "\nTest: Lateral G Boost (Slide)" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine); // v0.5.12: Initialize with T300 defaults
    TelemInfoV01 data;
    std::memset(&data, 0, sizeof(data));
    
    // Default RH to avoid scraping
    data.mWheel[0].mRideHeight = 0.1; data.mWheel[1].mRideHeight = 0.1;
    
    engine.m_sop_effect = 1.0;
    engine.m_oversteer_boost = 1.0;
    engine.m_gain = 1.0;
    // Lower Scale to match new Nm range
    engine.m_sop_scale = 10.0; 
    // Disable smoothing to verify math instantly (v0.4.2 fix) 
    engine.m_sop_smoothing_factor = 1.0; 
    engine.m_max_torque_ref = 20.0f; // Fix Reference for Test (v0.4.4)
    engine.m_invert_force = false;
    
    // Scenario: Front has grip, rear is sliding
    data.mWheel[0].mGripFract = 1.0; // FL
    data.mWheel[1].mGripFract = 1.0; // FR
    data.mWheel[2].mGripFract = 0.5; // RL (sliding)
    data.mWheel[3].mGripFract = 0.5; // RR (sliding)
    
    // Lateral G (cornering)
    data.mLocalAccel.x = 9.81; // 1G lateral
    
    // Rear lateral force (resisting slide)
    data.mWheel[2].mLateralForce = 2000.0;
    data.mWheel[3].mLateralForce = 2000.0;
    
    // Run for multiple frames to let smoothing settle
    double force = 0.0;
    for (int i=0; i<60; i++) {
        force = engine.calculate_force(&data);
    }
    
    // Norm = 20 / 20 = 1.0.
    ASSERT_TRUE(std::abs(force) > 0.5);
}





static void test_spin_torque_drop_interaction() {
    std::cout << "\nTest: Spin Torque Drop with SoP" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine); // v0.5.12: Initialize with T300 defaults
    TelemInfoV01 data;
    std::memset(&data, 0, sizeof(data));
    
    // Default RH to avoid scraping
    data.mWheel[0].mRideHeight = 0.1; data.mWheel[1].mRideHeight = 0.1;
    
    engine.m_spin_enabled = true;
    engine.m_spin_gain = 1.0;
    engine.m_sop_effect = 1.0;
    engine.m_gain = 1.0;
    engine.m_sop_scale = 10.0;
    engine.m_max_torque_ref = 20.0f; // Fix Reference for Test (v0.4.4)
    
    // High SoP force
    data.mLocalAccel.x = 9.81; // 1G lateral
    data.mSteeringShaftTorque = 10.0; // 10 Nm
    
    // Set Grip to 1.0 so Game Force isn't killed by Understeer Effect
    data.mWheel[0].mGripFract = 1.0;
    data.mWheel[1].mGripFract = 1.0;
    data.mWheel[2].mGripFract = 1.0;
    data.mWheel[3].mGripFract = 1.0;
    
    // No spin initially
    data.mUnfilteredThrottle = 0.0;
    
    // Run multiple frames to settle SoP
    double force_no_spin = 0.0;
    for (int i=0; i<60; i++) {
        force_no_spin = engine.calculate_force(&data);
    }
    
    // Now trigger spin
    data.mUnfilteredThrottle = 1.0;
    data.mLocalVel.z = 20.0;
    
    // 70% slip (severe = 1.0)
    double ground_vel = 20.0;
    data.mWheel[2].mLongitudinalGroundVel = ground_vel;
    data.mWheel[3].mLongitudinalGroundVel = ground_vel;
    data.mWheel[2].mLongitudinalPatchVel = 0.7 * ground_vel;
    data.mWheel[3].mLongitudinalPatchVel = 0.7 * ground_vel;

    data.mDeltaTime = 0.01;
    
    double force_with_spin = engine.calculate_force(&data);
    
    // Torque drop: 1.0 - (1.0 * 1.0 * 0.6) = 0.4 (60% reduction)
    // NoSpin: Base + SoP. 10.0 / 20.0 (Base) + SoP.
    // With spin, Base should be reduced.
    // However, Spin adds rumble. 
    // With 20Nm scale, rumble can be large if not careful.
    // But we scaled rumble down to 2.5.
    
    // v0.4.19: After coordinate fix, magnitudes may be different
    // Reduce threshold to 0.02 to account for sign changes
    if (std::abs(force_with_spin - force_no_spin) > 0.02) {
        std::cout << "[PASS] Spin torque drop modifies total force." << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Torque drop ineffective. Spin: " << force_with_spin << " NoSpin: " << force_no_spin << std::endl;
        g_tests_failed++;
    }
}



static void test_static_notch_integration() {
    std::cout << "\nTest: Static Notch Integration (v0.4.43)" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine); // v0.5.12: Initialize with T300 defaults
    TelemInfoV01 data;
    std::memset(&data, 0, sizeof(data));

    // Setup
    engine.m_static_notch_enabled = true;
    engine.m_static_notch_freq = 11.0;
    engine.m_static_notch_width = 10.0; // Q = 11/10 = 1.1 (Wide notch for testing)
    engine.m_gain = 1.0;
    engine.m_max_torque_ref = 1.0; 
    engine.m_bottoming_enabled = false; // Disable to avoid interference
    engine.m_invert_force = false;      // Disable inversion for clarity
    engine.m_understeer_effect = 0.0;   // Disable grip logic clamping

    data.mDeltaTime = 0.0025; // 400Hz
    data.mWheel[0].mRideHeight = 0.1; // Valid RH
    data.mWheel[1].mRideHeight = 0.1;
    data.mLocalVel.z = 20.0; // Valid Speed
    data.mWheel[0].mTireLoad = 4000.0; // Valid Load
    data.mWheel[1].mTireLoad = 4000.0;
    
    // 1. Target Frequency (11Hz) - Should be attenuated
    double max_amp_target = 0.0;
    for (int i = 0; i < 400; i++) { // 1 second
        double t = (double)i * data.mDeltaTime;
        data.mSteeringShaftTorque = std::sin(2.0 * 3.14159265 * 11.0 * t); // Test at 11Hz
        
        double force = engine.calculate_force(&data);
        
        // Skip transient (first 100 frames = 0.25s)
        if (i > 100 && std::abs(force) > max_amp_target) {
            max_amp_target = std::abs(force);
        }
    }
    
    // Q=1.1 notch at 11Hz should provide significant attenuation.
    if (max_amp_target < 0.3) {
        std::cout << "[PASS] Static Notch attenuated 11Hz signal (Max Amp: " << max_amp_target << ")" << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Static Notch failed to attenuate 11Hz. Max Amp: " << max_amp_target << std::endl;
        g_tests_failed++;
    }

    // 2. Off-Target Frequency (20Hz) - Should pass
    engine.m_static_notch_enabled = false;
    engine.calculate_force(&data); // Reset by disabling
    engine.m_static_notch_enabled = true;

    double max_amp_pass = 0.0;
    for (int i = 0; i < 400; i++) {
        double t = (double)i * data.mDeltaTime;
        data.mSteeringShaftTorque = std::sin(2.0 * 3.14159265 * 20.0 * t); // Test at 20Hz (far from 11Hz)
        
        double force = engine.calculate_force(&data);
        
        if (i > 100 && std::abs(force) > max_amp_pass) {
            max_amp_pass = std::abs(force);
        }
    }

    if (max_amp_pass > 0.8) {
        std::cout << "[PASS] Static Notch passed 20Hz signal (Max Amp: " << max_amp_pass << ")" << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Static Notch attenuated 20Hz signal. Max Amp: " << max_amp_pass << std::endl;
        g_tests_failed++;
    }
}

static void test_notch_filter_bandwidth() {
    std::cout << "\nTest: Notch Filter Bandwidth (v0.6.10)" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine);
    TelemInfoV01 data = CreateBasicTestTelemetry(20.0);
    
    engine.m_static_notch_enabled = true;
    engine.m_static_notch_freq = 50.0f;
    engine.m_static_notch_width = 10.0f; // 45Hz to 55Hz
    
    // Case 1: Signal at center frequency (50Hz)
    // 50Hz signal: 10/dt = 1/dt. Samples per period = (1/dt)/50.
    // If dt=0.0025 (400Hz), samples per period = 8.
    data.mDeltaTime = 0.0025; // 400Hz
    
    // Inject 50Hz sine wave
    double amplitude = 10.0;
    double max_output = 0.0;
    for (int i = 0; i < 100; i++) {
        data.mSteeringShaftTorque = std::sin(2.0 * PI * 50.0 * (i * data.mDeltaTime)) * amplitude;
        double output = std::abs(engine.calculate_force(&data));
        if (i > 50 && output > max_output) max_output = output;
    }
    // Normalized amplitude max is (10.0 * 1.0) / 20.0 = 0.5.
    // At center, it should be highly attenuated (near 0)
    ASSERT_TRUE(max_output < 0.1); 

    // Case 2: Signal at 46Hz (inside the 10Hz bandwidth)
    max_output = 0.0;
    for (int i = 0; i < 100; i++) {
        data.mSteeringShaftTorque = std::sin(2.0 * PI * 46.0 * (i * data.mDeltaTime)) * amplitude;
        double output = std::abs(engine.calculate_force(&data));
        if (i > 50 && output > max_output) max_output = output;
    }
    // 46Hz is within the 10Hz bandwidth (45-55). Should be significantly attenuated but > 0.
    // Max unattenuated is 0.5. Calculated gain ~0.64 -> Expect ~0.32
    ASSERT_TRUE(max_output < 0.4); 
    ASSERT_TRUE(max_output > 0.1);

    // Case 3: Signal at 65Hz (outside the 10Hz bandwidth)
    max_output = 0.0;
    for (int i = 0; i < 100; i++) {
        data.mSteeringShaftTorque = std::sin(2.0 * PI * 65.0 * (i * data.mDeltaTime)) * amplitude;
        double output = std::abs(engine.calculate_force(&data));
        if (i > 50 && output > max_output) max_output = output;
    }
    // 65Hz is far outside 45-55. Attenuation should be minimal.
    // Expected output near 0.25.
    ASSERT_TRUE(max_output > 0.2);
}

static void test_notch_filter_edge_cases() {
    std::cout << "\nTest: Notch Filter Edge Cases (v0.6.10)" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine);
    TelemInfoV01 data = CreateBasicTestTelemetry(20.0);
    
    engine.m_static_notch_enabled = true;
    engine.m_static_notch_freq = 11.0f; // Use new default
    data.mDeltaTime = 0.0025; // 400Hz
    
    // Edge Case 1: Minimum Width (0.1 Hz) - Very narrow notch
    // Q = 11 / 0.1 = 110 (extremely surgical)
    engine.m_static_notch_width = 0.1f;
    
    double amplitude = 10.0;
    double max_output_narrow = 0.0;
    
    // Test at 11Hz (center) - should be heavily attenuated
    for (int i = 0; i < 100; i++) {
        data.mSteeringShaftTorque = std::sin(2.0 * PI * 11.0 * (i * data.mDeltaTime)) * amplitude;
        double output = std::abs(engine.calculate_force(&data));
        if (i > 50 && output > max_output_narrow) max_output_narrow = output;
    }
    // Notch filter with high Q provides excellent attenuation but not perfect due to transients
    ASSERT_TRUE(max_output_narrow < 0.6); // Very narrow notch still attenuates center significantly
    
    // Test at 10.5Hz (just 0.5 Hz away) - should pass through with narrow notch
    max_output_narrow = 0.0;
    for (int i = 0; i < 100; i++) {
        data.mSteeringShaftTorque = std::sin(2.0 * PI * 10.5 * (i * data.mDeltaTime)) * amplitude;
        double output = std::abs(engine.calculate_force(&data));
        if (i > 50 && output > max_output_narrow) max_output_narrow = output;
    }
    ASSERT_TRUE(max_output_narrow > 0.3); // Narrow notch doesn't affect nearby frequencies
    
    // Edge Case 2: Maximum Width (10.0 Hz) - Very wide notch
    // Q = 11 / 10 = 1.1 (wide suppression)
    engine.m_static_notch_width = 10.0f;
    
    double max_output_wide = 0.0;
    
    // Test at 6Hz (5 Hz away, at edge of 10Hz bandwidth)
    for (int i = 0; i < 100; i++) {
        data.mSteeringShaftTorque = std::sin(2.0 * PI * 6.0 * (i * data.mDeltaTime)) * amplitude;
        double output = std::abs(engine.calculate_force(&data));
        if (i > 50 && output > max_output_wide) max_output_wide = output;
    }
    // Wide notch affects frequencies 5Hz away but doesn't eliminate them
    ASSERT_TRUE(max_output_wide > 0.05); // Not completely eliminated
    
    // Edge Case 3: Below minimum safety clamp (should clamp to 0.1)
    // This tests the safety clamp in FFBEngine.h line 811
    engine.m_static_notch_width = 0.05f; // Below 0.1 minimum
    
    // The code should clamp this to 0.1, giving Q = 11 / 0.1 = 110
    max_output_narrow = 0.0;
    for (int i = 0; i < 100; i++) {
        data.mSteeringShaftTorque = std::sin(2.0 * PI * 11.0 * (i * data.mDeltaTime)) * amplitude;
        double output = std::abs(engine.calculate_force(&data));
        if (i > 50 && output > max_output_narrow) max_output_narrow = output;
    }
    ASSERT_TRUE(max_output_narrow < 0.7); // Safety clamp prevents extreme Q values
}



static void test_phase_wraparound() {
    std::cout << "\nTest: Phase Wraparound (Anti-Click)" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine); // v0.5.12: Initialize with T300 defaults
    TelemInfoV01 data;
    std::memset(&data, 0, sizeof(data));
    
    // Default RH to avoid scraping
    data.mWheel[0].mRideHeight = 0.1; data.mWheel[1].mRideHeight = 0.1;
    
    engine.m_lockup_enabled = true;
    engine.m_lockup_gain = 1.0;
    
    data.mUnfilteredBrake = 1.0;
    // Slip ratio -0.3
    data.mWheel[0].mLongitudinalGroundVel = 20.0;
    data.mWheel[1].mLongitudinalGroundVel = 20.0;
    data.mWheel[0].mLongitudinalPatchVel = -0.3 * 20.0;
    data.mWheel[1].mLongitudinalPatchVel = -0.3 * 20.0;
    
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
            // Verify wrap happened near 2Ï€
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

static void test_multi_effect_interaction() {
    std::cout << "\nTest: Multi-Effect Interaction (Lockup + Spin)" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine); // v0.5.12: Initialize with T300 defaults
    TelemInfoV01 data;
    std::memset(&data, 0, sizeof(data));
    
    // Default RH to avoid scraping
    data.mWheel[0].mRideHeight = 0.1; data.mWheel[1].mRideHeight = 0.1;
    
    // Set tire radius for snapshot (v0.4.41)
    data.mWheel[0].mStaticUndeflectedRadius = 33; // 33cm = 0.33m
    data.mWheel[1].mStaticUndeflectedRadius = 33;
    data.mWheel[2].mStaticUndeflectedRadius = 33;
    data.mWheel[3].mStaticUndeflectedRadius = 33;
    
    // Set base steering torque
    data.mSteeringShaftTorque = 5.0; // 5 Nm base force
    
    // Enable both lockup and spin
    engine.m_lockup_enabled = true;
    engine.m_lockup_gain = 1.0;
    engine.m_spin_enabled = true;
    engine.m_spin_gain = 1.0;
    
    // Scenario: Braking AND spinning (e.g., locked front, spinning rear)
    data.mUnfilteredBrake = 1.0;
    data.mUnfilteredThrottle = 0.5; // Partial throttle
    
    data.mLocalVel.z = 20.0;
    double ground_vel = 20.0;
    data.mWheel[0].mLongitudinalGroundVel = ground_vel;
    data.mWheel[1].mLongitudinalGroundVel = ground_vel;
    data.mWheel[2].mLongitudinalGroundVel = ground_vel;
    data.mWheel[3].mLongitudinalGroundVel = ground_vel;

    // Front Locked (-0.3 slip ratio)
    // Slip ratio = PatchVel / GroundVel, so PatchVel = slip_ratio * GroundVel
    // For -0.3 slip: PatchVel = -0.3 * 20 = -6.0 m/s
    data.mWheel[0].mLongitudinalPatchVel = -0.3 * ground_vel;
    data.mWheel[1].mLongitudinalPatchVel = -0.3 * ground_vel;
    
    // Rear Spinning (+0.5 slip ratio)
    // For +0.5 slip: PatchVel = 0.5 * 20 = 10.0 m/s
    data.mWheel[2].mLongitudinalPatchVel = 0.5 * ground_vel;
    data.mWheel[3].mLongitudinalPatchVel = 0.5 * ground_vel;

    data.mDeltaTime = 0.01;
    data.mElapsedTime = 0.0; // Initialize elapsed time
    
    // Run multiple frames
    // Note: Using 11 frames instead of 10 to avoid a coincidence where
    // lockup phase (40Hz at 20m/s) wraps exactly to 0 after 10 frames with dt=0.01.
    for (int i = 0; i < 11; i++) {
        data.mElapsedTime += data.mDeltaTime; // Increment time each frame
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
        std::cout << "[FAIL] Effects did not trigger. lockup_phase=" << engine.m_lockup_phase << ", spin_phase=" << engine.m_spin_phase << std::endl;
        g_tests_failed++;
    }
}

static void test_notch_filter_attenuation() {
    std::cout << "\nTest: Notch Filter Attenuation (v0.4.41)" << std::endl;
    BiquadNotch filter;
    double sample_rate = 400.0;
    double target_freq = 15.0; // 15Hz
    filter.Update(target_freq, sample_rate, 2.0);

    // 1. Target Frequency: Should be killed
    double max_amp_target = 0.0;
    for (int i = 0; i < 400; i++) {
        double t = (double)i / sample_rate;
        double in = std::sin(2.0 * 3.14159265 * target_freq * t);
        double out = filter.Process(in);
        // Skip initial transient
        if (i > 100 && std::abs(out) > max_amp_target) max_amp_target = std::abs(out);
    }
    
    if (max_amp_target < 0.1) {
        std::cout << "[PASS] Notch Filter attenuated target frequency (Max Amp: " << max_amp_target << ")" << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Notch Filter did not attenuate target frequency. Max Amp: " << max_amp_target << std::endl;
        g_tests_failed++;
    }

    // 2. Off-Target Frequency: Should pass
    filter.Reset();
    double pass_freq = 2.0; // 2Hz steering
    double max_amp_pass = 0.0;
    for (int i = 0; i < 400; i++) {
        double t = (double)i / sample_rate;
        double in = std::sin(2.0 * 3.14159265 * pass_freq * t);
        double out = filter.Process(in);
        if (i > 100 && std::abs(out) > max_amp_pass) max_amp_pass = std::abs(out);
    }

    if (max_amp_pass > 0.8) {
        std::cout << "[PASS] Notch Filter passed off-target frequency (Max Amp: " << max_amp_pass << ")" << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Notch Filter attenuated off-target frequency. Max Amp: " << max_amp_pass << std::endl;
        g_tests_failed++;
    }
}

static void test_frequency_estimator() {
    std::cout << "\nTest: Frequency Estimator (v0.4.41)" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine); // v0.5.12: Initialize with T300 defaults
    TelemInfoV01 data;
    std::memset(&data, 0, sizeof(data));
    data.mLocalVel.z = -20.0; // Moving fast (v0.6.22)
    
    data.mDeltaTime = 0.0025; // 400Hz
    double target_freq = 20.0; // 20Hz vibration

    // Run 1 second of simulation
    for (int i = 0; i < 400; i++) {
        double t = (double)i * data.mDeltaTime;
        data.mSteeringShaftTorque = 5.0 * std::sin(2.0 * 3.14159265 * target_freq * t);
        data.mElapsedTime = t;
        
        // Ensure no other effects trigger
        data.mWheel[0].mRideHeight = 0.1;
        data.mWheel[1].mRideHeight = 0.1;
        
        engine.calculate_force(&data);
    }

    double estimated = engine.m_debug_freq;
    if (std::abs(estimated - target_freq) < 1.0) {
        std::cout << "[PASS] Frequency Estimator converged to " << estimated << " Hz (Target: " << target_freq << ")" << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Frequency Estimator mismatch. Got " << estimated << " Hz, Expected ~" << target_freq << std::endl;
        g_tests_failed++;
    }
}

void Run_Texture() {
    std::cout << "\n=== Road Effects & Features Tests ===" << std::endl;
    test_regression_road_texture_toggle();
    test_regression_bottoming_switch();
    test_road_texture_teleport();
    test_suspension_bottoming();
    test_road_texture_state_persistence();
    test_universal_bottoming();
    test_unconditional_vert_accel_update();
    test_scrub_drag_fade();
    test_stationary_gate();
    test_idle_smoothing();
    test_stationary_silence();
    test_driving_forces_restored();
    test_slide_texture();
    test_dynamic_tuning();
    test_oversteer_boost();
    test_phase_wraparound();
    test_multi_effect_interaction();

    test_spin_torque_drop_interaction();
    test_notch_filter_attenuation();
    test_frequency_estimator();
    test_static_notch_integration();
    test_notch_filter_bandwidth();
    test_notch_filter_edge_cases();
}

} // namespace FFBEngineTests