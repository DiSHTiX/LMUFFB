#include "test_ffb_common.h"
#include <thread>
#include <chrono>

namespace FFBEngineTests {

TEST_CASE(test_coverage_slope_torque, "Coverage") {
    FFBEngine engine;
    TelemInfoV01 data = CreateBasicTestTelemetry();
    FFBEngineTestAccess::SetSlopeUseTorque(engine, true);
    
    // Setup torque buffer to produce a negative derivative if possible, 
    // or just set m_slope_torque_current directly if we can (we can't yet).
    // Let's use the buffer. m_slope_torque_current is calculated in calculate_slope_grip.
    // Wait, m_slope_torque_current is calculated at line 1209.
    
    // Fill buffers to allow SG derivative calculation
    std::array<double, 41> torque_buf = {};
    for(int i=0; i<41; i++) torque_buf[i] = 100.0 - (i * 5.0); // Negative ramp
    FFBEngineTestAccess::SetSlopeTorqueBuffer(engine, torque_buf);
    FFBEngineTestAccess::SetSlopeBufferIndex(engine, 0);
    FFBEngineTestAccess::SetSlopeBufferCount(engine, 41);
    
    // Call calculation - this should update m_slope_torque_current and use it
    double output = FFBEngineTestAccess::CallCalculateSlopeGrip(engine, 1.0, 0.1, 0.01, &data);
    ASSERT_TRUE(std::isfinite(output));
}

TEST_CASE(test_coverage_stats_latching, "Coverage") {
    FFBEngine engine;
    TelemInfoV01 data = CreateBasicTestTelemetry();
    
    // Initialize stats with some data
    FFBEngineTestAccess::GetTorqueStats(engine).Update(50.0);
    data.mSteeringShaftTorque = 50.0f;
    
    // Set last_log_time to 2 seconds ago
    auto two_secs_ago = std::chrono::steady_clock::now() - std::chrono::seconds(2);
    FFBEngineTestAccess::SetLastLogTime(engine, two_secs_ago);
    
    // Call calculate_force (which calls the stats latching logic)
    engine.calculate_force(&data);
    
    // Verify that interval_count was reset to 0 (after latching into l_avg)
    ASSERT_EQ(FFBEngineTestAccess::GetTorqueStats(engine).interval_count, 0);
    ASSERT_NEAR(FFBEngineTestAccess::GetTorqueStats(engine).l_avg, 50.0, 0.001);
}

TEST_CASE(test_coverage_flatspot, "Coverage") {
    FFBEngine engine;
    TelemInfoV01 data = CreateBasicTestTelemetry(10.0); // 10 m/s
    FFBCalculationContext ctx;
    ctx.dt = 0.0025;
    ctx.car_speed = 10.0;
    
    // wheel freq = speed / circumference. 10 / (2*PI*0.33) approx 4.8 Hz > 1.0
    FFBEngineTestAccess::SetFlatspotSuppression(engine, true);
    FFBEngineTestAccess::SetFlatspotStrength(engine, 0.5f);
    
    float out = FFBEngineTestAccess::CallApplySignalConditioning(engine, 1.0f, &data, ctx);
    // Should be filtered.
    ASSERT_NEAR(out, 1.0f, 1.0f); // Just ensure it runs and returns something finite
    ASSERT_TRUE(std::isfinite(out));
}

TEST_CASE(test_coverage_gyro_damping, "Coverage") {
    FFBEngine engine;
    TelemInfoV01 data = CreateBasicTestTelemetry(20.0);
    FFBCalculationContext ctx;
    ctx.dt = 0.01;
    ctx.car_speed = 20.0;
    ctx.decoupling_scale = 1.0;
    
    data.mPhysicalSteeringWheelRange = 10.0f; // 10 rad
    data.mUnfilteredSteering = 0.5f; // 0.5 * 5 = 2.5 rad
    
    FFBEngineTestAccess::CallCalculateGyroDamping(engine, &data, ctx);
    // ctx.gyro_force should be populated
    ASSERT_TRUE(std::isfinite(ctx.gyro_force));
}

TEST_CASE(test_coverage_abs_pulse, "Coverage") {
    FFBEngine engine;
    TelemInfoV01 data = CreateBasicTestTelemetry(10.0);
    FFBCalculationContext ctx;
    ctx.dt = 0.01;
    ctx.decoupling_scale = 1.0;
    ctx.speed_gate = 1.0;
    
    FFBEngineTestAccess::SetABSPulseEnabled(engine, true);
    data.mUnfilteredBrake = 0.8f; // Above threshold 0.5
    
    // Need pressure delta > threshold 2.0
    // prev pressure is 0 by default. set current pressure to 1.0. 
    // delta = (1.0 - 0) / 0.01 = 100.0 > 2.0
    for(int i=0; i<4; i++) data.mWheel[i].mBrakePressure = 1.0f;
    
    FFBEngineTestAccess::CallCalculateABSPulse(engine, &data, ctx);
    ASSERT_TRUE(std::isfinite(ctx.abs_pulse_force));
    // Since freq > 0, sin(phase) might be non-zero.
}

} // namespace FFBEngineTests
