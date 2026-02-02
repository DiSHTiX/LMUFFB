#include <iostream>
#include <cmath>
#include <cstring>
#include <algorithm>
#include "../src/FFBEngine.h"
#include "../src/lmu_sm_interface/InternalsPlugin.hpp"

namespace FFBEngineTests {
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

static void test_kinematic_load_zero_velocity() {
    std::cout << "\n=== Test: Kinematic Load at Zero Velocity (Bug B3) ===" << std::endl;

    FFBEngine engine;
    engine.m_approx_mass_kg = 1100.0f;
    engine.m_approx_weight_bias = 0.55f;

    TelemInfoV01 data = {};
    data.mLocalVel.z = 0.0;
    data.mLocalAccel.x = 0.0;
    data.mLocalAccel.z = 0.0;

    engine.m_accel_x_smoothed = 0.0;
    engine.m_accel_z_smoothed = 0.0;

    double load_fl = engine.calculate_kinematic_load(&data, 0);

    ASSERT_NEAR(load_fl, 0.0, 10.0);
}

static void test_kinematic_load_low_velocity() {
    std::cout << "\n=== Test: Kinematic Load at Low Velocity (Bug B3) ===" << std::endl;

    FFBEngine engine;
    engine.m_approx_mass_kg = 1100.0f;
    engine.m_approx_weight_bias = 0.55f;

    TelemInfoV01 data = {};
    data.mLocalVel.z = 5.0;
    data.mLocalAccel.x = 0.0;
    data.mLocalAccel.z = 0.0;

    engine.m_accel_x_smoothed = 0.0;
    engine.m_accel_z_smoothed = 0.0;

    double load_fl = engine.calculate_kinematic_load(&data, 0);

    double expected_factor = 5.0 / 10.0;
    double expected_load = (1100.0 * 9.81 * (1.0 - 0.55) * expected_factor) / 2.0;

    ASSERT_NEAR(load_fl, expected_load, 50.0);
}

static void test_kinematic_load_full_velocity() {
    std::cout << "\n=== Test: Kinematic Load at Full Velocity (Bug B3) ===" << std::endl;

    FFBEngine engine;
    engine.m_approx_mass_kg = 1100.0f;
    engine.m_approx_weight_bias = 0.55f;

    TelemInfoV01 data = {};
    data.mLocalVel.z = 80.0;
    data.mLocalAccel.x = 0.0;
    data.mLocalAccel.z = 0.0;

    engine.m_accel_x_smoothed = 0.0;
    engine.m_accel_z_smoothed = 0.0;

    double load_fl = engine.calculate_kinematic_load(&data, 0);

    double expected_factor = 1.0;
    double expected_static = (1100.0 * 9.81 * (1.0 - 0.55) * expected_factor) / 2.0;

    ASSERT_NEAR(load_fl, expected_static, 50.0);
}

static void test_kinematic_load_downforce() {
    std::cout << "\n=== Test: Kinematic Load Downforce (Bug B2) ===" << std::endl;

    FFBEngine engine;
    engine.m_approx_mass_kg = 1100.0f;
    engine.m_approx_weight_bias = 0.55f;
    engine.m_approx_aero_coeff = 2.0f;

    TelemInfoV01 data = {};
    data.mLocalVel.z = 50.0;
    data.mLocalAccel.x = 0.0;
    data.mLocalAccel.z = 0.0;

    engine.m_accel_x_smoothed = 0.0;
    engine.m_accel_z_smoothed = 0.0;

    double load_fl = engine.calculate_kinematic_load(&data, 0);

    double expected_static = (1100.0 * 9.81 * 0.45 * 1.0) / 2.0;
    double expected_downforce = 2.0 * (50.0 * 50.0) / 4.0;
    double expected_total = expected_static + expected_downforce;

    ASSERT_NEAR(load_fl, expected_total, 100.0);
    ASSERT_TRUE(load_fl > expected_static);
}

void run_all_tests() {
    std::cout << "========================================" << std::endl;
    std::cout << "Running FFBEngine Bug Fix Tests (v0.7.0)" << std::endl;
    std::cout << "========================================" << std::endl;

    test_kinematic_load_zero_velocity();
    test_kinematic_load_low_velocity();
    test_kinematic_load_full_velocity();
    test_kinematic_load_downforce();

    std::cout << "\n========================================" << std::endl;
    std::cout << "Results: " << g_tests_passed << " passed, " << g_tests_failed << " failed" << std::endl;
    std::cout << "========================================" << std::endl;

    if (g_tests_failed > 0) {
        std::exit(1);
    }
}

}

int main() {
    FFBEngineTests::run_all_tests();
    return 0;
}
