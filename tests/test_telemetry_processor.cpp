#include <iostream>
#include <cmath>
#include <cstring>
#include <algorithm>

#if defined(_WIN32)
#include "../src/FFBEngine.h"
#include "../src/lmu_sm_interface/InternalsPlugin.hpp"
#else
#include "../include/TelemetryProcessor.h"
#endif

namespace TelemetryProcessorTests {
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

#define ASSERT_LE(a, b) \
    if ((a) <= (b)) { \
        std::cout << "[PASS] " << #a << " <= " << #b << std::endl; \
        g_tests_passed++; \
    } else { \
        std::cout << "[FAIL] " << #a << " (" << (a) << ") > " << #b << " (" << (b) << ")" << std::endl; \
        g_tests_failed++; \
    }

static void test_ema_filter() {
    std::cout << "\n=== Test: EMA Filter ===" << std::endl;

    TelemetryProcessor::EMAFilter ema(0.1, 1.0);

    double result = ema.Update(10.0, 0.0025);
    ASSERT_TRUE(result > 1.0 && result < 10.0);

    result = ema.Update(10.0, 0.0025);
    ASSERT_TRUE(result > 1.0 && result < 10.0);

    ASSERT_NEAR(ema.GetState(), result, 0.001);
}

static void test_clamp() {
    std::cout << "\n=== Test: Clamp Function ===" << std::endl;

    double result = TelemetryProcessor::Clamp(5.0, 0.0, 10.0);
    ASSERT_NEAR(result, 5.0, 0.001);

    result = TelemetryProcessor::Clamp(-5.0, 0.0, 10.0);
    ASSERT_NEAR(result, 0.0, 0.001);

    result = TelemetryProcessor::Clamp(15.0, 0.0, 10.0);
    ASSERT_NEAR(result, 10.0, 0.001);
}

static void test_is_finite() {
    std::cout << "\n=== Test: IsFinite ===" << std::endl;

    ASSERT_TRUE(TelemetryProcessor::IsFinite(1.0));
    ASSERT_TRUE(TelemetryProcessor::IsFinite(-1.0));
    ASSERT_TRUE(TelemetryProcessor::IsFinite(0.0));
    ASSERT_TRUE(!TelemetryProcessor::IsFinite(std::nan("")));
    ASSERT_TRUE(!TelemetryProcessor::IsFinite(std::numeric_limits<double>::infinity()));
}

static void test_is_in_range() {
    std::cout << "\n=== Test: IsInRange ===" << std::endl;

    ASSERT_TRUE(TelemetryProcessor::IsInRange(5.0, 0.0, 10.0));
    ASSERT_TRUE(!TelemetryProcessor::IsInRange(-1.0, 0.0, 10.0));
    ASSERT_TRUE(!TelemetryProcessor::IsInRange(11.0, 0.0, 10.0));
    ASSERT_TRUE(TelemetryProcessor::IsInRange(0.0, 0.0, 10.0));
    ASSERT_TRUE(TelemetryProcessor::IsInRange(10.0, 0.0, 10.0));
}

static void test_grip_from_slip() {
    std::cout << "\n=== Test: EstimateGripFromSlip ===" << std::endl;

    double grip = TelemetryProcessor::EstimateGripFromSlip(0.05, 1000.0);
    ASSERT_NEAR(grip, 1.0, 0.1);

    grip = TelemetryProcessor::EstimateGripFromSlip(0.2, 1000.0);
    ASSERT_TRUE(grip < 1.0);
    ASSERT_TRUE(grip > 0.0);

    grip = TelemetryProcessor::EstimateGripFromSlip(0.3, 1000.0);
    ASSERT_TRUE(grip < 0.5);
}

static void test_kinematic_load_params() {
    std::cout << "\n=== Test: Kinematic Load with Custom Params ===" << std::endl;

    TelemetryProcessor::KinematicParams params = {1100.0, 2.0, 0.55, 0.6};

    TelemInfoV01 data = {};
    data.mLocalVel.z = 0.0;
    data.mLocalAccel.x = 0.0;
    data.mLocalAccel.z = 0.0;

    double load = TelemetryProcessor::EstimateKinematicLoad(&data, 0, params);
    ASSERT_NEAR(load, 0.0, 10.0);
}

void run_all_tests() {
    std::cout << "========================================" << std::endl;
    std::cout << "Running TelemetryProcessor Tests (v0.7.0)" << std::endl;
    std::cout << "========================================" << std::endl;

    test_ema_filter();
    test_clamp();
    test_is_finite();
    test_is_in_range();
    test_grip_from_slip();
    test_kinematic_load_params();

    std::cout << "\n========================================" << std::endl;
    std::cout << "Results: " << g_tests_passed << " passed, " << g_tests_failed << " failed" << std::endl;
    std::cout << "========================================" << std::endl;

    if (g_tests_failed > 0) {
        std::exit(1);
    }
}

}

int main() {
#if !defined(_WIN32)
    std::cout << "Note: Running limited tests (Windows headers not available)" << std::endl;
#endif
    TelemetryProcessorTests::run_all_tests();
    return 0;
}
