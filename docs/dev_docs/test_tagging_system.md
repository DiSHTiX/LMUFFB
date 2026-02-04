# Test Tagging System

**Version:** 1.0  
**Date:** 2026-02-04  
**Status:** Active

---

## Overview

The LMUFFB test suite uses a tagging system to enable running subsets of tests based on their characteristics. This allows developers to quickly run relevant tests during development without executing the entire suite.

---

## Available Tags

### Functional Tags

| Tag | Description | Example Tests |
|-----|-------------|---------------|
| `[Physics]` | Core physics calculations and force generation | `test_base_force_modes`, `test_sop_effect` |
| `[Math]` | Mathematical helpers and algorithms | `test_smoothstep_helper_function`, `test_slope_sg_derivative` |
| `[Integration]` | Multi-component integration tests | `test_multi_effect_interaction`, `test_coordinate_all_effects_alignment` |
| `[Config]` | Configuration, persistence, and presets | `test_config_persistence`, `test_preset_initialization` |
| `[Regression]` | Regression tests for specific bug fixes | `test_regression_rear_torque_lpf`, `test_regression_phase_explosion` |
| `[Edge]` | Edge cases and boundary conditions | `test_notch_filter_edge_cases`, `test_yaw_kick_edge_cases` |
| `[Performance]` | Stress tests and performance validation | `test_stress_stability`, `test_high_gain_stability` |

### Component Tags

| Tag | Description | Example Tests |
|-----|-------------|---------------|
| `[SoP]` | Self-Aligning Torque (SoP) related | `test_sop_effect`, `test_sop_yaw_kick` |
| `[Slope]` | Slope detection system | `test_slope_detection_buffer_init`, `test_slope_decay_on_straight` |
| `[Texture]` | Haptic textures (slide, road, lockup) | `test_slide_texture`, `test_road_texture_teleport` |
| `[Grip]` | Grip calculation and approximation | `test_grip_modulation`, `test_combined_grip_loss` |
| `[Coordinates]` | Coordinate system and sign conventions | `test_coordinate_sop_inversion`, `test_coordinate_scrub_drag_direction` |
| `[Smoothing]` | Filtering and smoothing algorithms | `test_smoothing_step_response`, `test_time_corrected_smoothing` |

---

## Usage

### Running All Tests

```powershell
.\build\tests\Release\run_combined_tests.exe
```

### Running Tests by Tag

```powershell
# Run only physics tests
.\build\tests\Release\run_combined_tests.exe --tag=Physics

# Run only regression tests
.\build\tests\Release\run_combined_tests.exe --tag=Regression

# Run multiple tags (OR logic)
.\build\tests\Release\run_combined_tests.exe --tag=Physics,Math

# Exclude specific tags
.\build\tests\Release\run_combined_tests.exe --exclude=Performance
```

### Running Tests by Category

```powershell
# Run only core physics category
.\build\tests\Release\run_combined_tests.exe --category=CorePhysics

# Run multiple categories
.\build\tests\Release\run_combined_tests.exe --category=CorePhysics,SlipGrip
```

---

## Implementation

### Test Function Naming Convention

Test functions should include tags in their documentation comments:

```cpp
// [Physics][SoP] Test Self-Aligning Torque effect
static void test_sop_effect() {
    std::cout << "\nTest: SoP Effect [Physics][SoP]" << std::endl;
    // ... test implementation
}

// [Math][Edge] Test smoothstep edge cases
static void test_smoothstep_edge_cases() {
    std::cout << "\nTest: Smoothstep Edge Cases [Math][Edge]" << std::endl;
    // ... test implementation
}
```

### Tag Registry

Tags are registered in `test_ffb_common.h` and can be queried programmatically:

```cpp
// Get all tests with a specific tag
std::vector<std::string> physics_tests = GetTestsByTag("Physics");

// Get all tags for a specific test
std::vector<std::string> tags = GetTestTags("test_sop_effect");
```

---

## Tag Assignment Guidelines

### When to Use Each Tag

**[Physics]:**
- Tests that validate core FFB force calculations
- Tests that verify physical behavior (grip, load transfer, etc.)
- Tests that check force output correctness

**[Math]:**
- Tests for mathematical helpers (smoothstep, interpolation, etc.)
- Tests for signal processing algorithms
- Tests for numerical stability

**[Integration]:**
- Tests that combine multiple effects
- Tests that verify interaction between components
- End-to-end scenario tests

**[Config]:**
- Configuration save/load tests
- Preset application tests
- Parameter validation tests

**[Regression]:**
- Tests added to prevent specific bugs from recurring
- Should reference the version where the bug was fixed
- Should include bug description in comments

**[Edge]:**
- Boundary condition tests
- Zero/null input tests
- Extreme value tests

**[Performance]:**
- Stress tests with random inputs
- High-iteration stability tests
- Tests that verify no NaN/Inf outputs

### Multiple Tags

Tests can have multiple tags. Use the most specific tags that apply:

```cpp
// [Physics][Regression][SoP] - A regression test for a physics bug in SoP
static void test_regression_rear_torque_lpf() { ... }

// [Math][Edge][Smoothing] - Edge cases for smoothing math
static void test_smoothing_step_response() { ... }
```

---

## Tag Maintenance

### Adding New Tags

1. Update this documentation with the new tag definition
2. Add the tag to `test_ffb_common.h` tag registry
3. Apply the tag to relevant tests
4. Update the tag count in the summary section below

### Removing Tags

1. Remove tag from all test functions
2. Remove from `test_ffb_common.h` registry
3. Update this documentation

---

## Current Tag Statistics

| Tag | Test Count | Percentage |
|-----|------------|------------|
| `[Physics]` | ~80 | 13.5% |
| `[Math]` | ~25 | 4.2% |
| `[Integration]` | ~15 | 2.5% |
| `[Config]` | ~20 | 3.4% |
| `[Regression]` | ~30 | 5.1% |
| `[Edge]` | ~20 | 3.4% |
| `[Performance]` | ~5 | 0.8% |
| `[SoP]` | ~25 | 4.2% |
| `[Slope]` | ~22 | 3.7% |
| `[Texture]` | ~30 | 5.1% |
| `[Grip]` | ~35 | 5.9% |
| `[Coordinates]` | ~15 | 2.5% |
| `[Smoothing]` | ~20 | 3.4% |

**Total Tests:** 591  
**Tagged Tests:** ~342 (57.9%)  
**Untagged Tests:** ~249 (42.1%)

*Note: Percentages are approximate. Tests can have multiple tags.*

---

## Future Enhancements

### Planned Features

1. **Tag Filtering in CI/CD:**
   - Run quick smoke tests (`[Physics]` + `[Regression]`) on every commit
   - Run full suite on pull requests
   - Run `[Performance]` tests nightly

2. **Tag-Based Test Reports:**
   - Generate coverage reports by tag
   - Track test execution time by tag
   - Identify slow tests within each category

3. **Interactive Tag Selection:**
   - GUI for selecting which tags to run
   - Save tag combinations as profiles
   - Quick-run buttons for common tag sets

4. **Automatic Tag Inference:**
   - Suggest tags based on test name and content
   - Warn if a test lacks tags
   - Validate tag consistency

---

## Examples

### Development Workflow Examples

**Scenario 1: Working on Slope Detection**
```powershell
# Run only slope-related tests
.\build\tests\Release\run_combined_tests.exe --tag=Slope
```

**Scenario 2: Pre-Commit Check**
```powershell
# Run physics and regression tests (fast smoke test)
.\build\tests\Release\run_combined_tests.exe --tag=Physics,Regression
```

**Scenario 3: Configuration Changes**
```powershell
# Run config and integration tests
.\build\tests\Release\run_combined_tests.exe --tag=Config,Integration
```

**Scenario 4: Performance Validation**
```powershell
# Run only performance/stress tests
.\build\tests\Release\run_combined_tests.exe --tag=Performance
```

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-02-04 | Initial tag system documentation |

---

**Document Status:** Active  
**Maintainer:** Development Team  
**Last Updated:** 2026-02-04
