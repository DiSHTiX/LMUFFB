# Changelog

All notable changes to this project will be documented in this file.

## [0.7.22] - 2026-02-10
### Added
- **Linux Port (GLFW + OpenGL)**:
  - **Cross-Platform GUI**: Refactored the GUI layer to support both Windows (DirectX 11) and Linux (GLFW + OpenGL 3) backends.
  - **Maximized Testability**: Enabled GUI widget logic and configuration handling tests on Linux CI by splitting ImGui into Core and Backend components.
  - **Hardware Mocking**: Implemented Linux-specific mocks for DirectInput FFB, Game Connector (Shared Memory), and vJoy, enabling the application to run and be tested on Linux environments.
  - **Shared Memory Compatibility**: Updated shared memory interface wrappers and vendor headers to compile on Linux by guarding Windows-specific headers and providing dummy typedefs.
  - **Build System**: Enhanced `CMakeLists.txt` with conditional logic for platform-specific dependencies and ImGui backends. Added `HEADLESS_GUI` option for non-graphical environments.
  - **Headless Mode Support**: Improved headless mode robustness by providing stubs for all GUI-related platform calls.

## [0.7.21] - 2026-02-10
### Changed
- **Slope Detection Refinement (Full v0.7.17 Plan Compliance)**:
  - **Improved Mathematical Stability**: Replaced the binary derivative gate with a continuous `smoothstep` confidence ramp for grip loss scaling.
  - **Denominator Protection**: Implemented robust protection against division-by-small-number artifacts using a minimum epsilon for `dAlpha/dt`.
  - **Physically Constrained Slope**: Enforced a hard `std::clamp` on calculated slope values to the physically possible range [-20.0, 20.0].
  - **Smooth Transitions**: The `smoothstep` ramp ensures zero-derivative endpoints for seamless haptic transitions between cornering and straights.

## [0.7.20] - 2026-02-10
### Added
- **Slope Detection Hardening**:
  - **Numerical Stability Fixes**: Implemented a hard clamp on calculated raw slope values within [-20.0, 20.0] to prevent mathematical explosions near thresholds.
  - **Improved Confidence Ramp**: Updated `calculate_slope_confidence` to use a dedicated 0.01 to 0.10 rad/s `inverse_lerp` ramp, providing smoother transitions and rejecting singularity artifacts at low steering speeds.
### Testing
- **Comprehensive Hardening Suite**: Added 6 new test cases to `tests/test_ffb_slope_detection.cpp` covering:
  - `TestSlope_NearThreshold_Singularity`: Verifies clamping under extreme dG/dAlpha conditions.
  - `TestSlope_ZeroCrossing`: Ensures stability during direction changes.
  - `TestSlope_SmallSignals`: Validates noise rejection below active thresholds.
  - `TestSlope_ImpulseRejection`: Confirms smoothing of single-frame physics spikes.
  - `TestSlope_NoiseImmunity`: Statistically verifies slope stability under high-noise conditions.
  - `TestConfidenceRamp_Progressive`: Validates the smooth blending of the grip effect.
- **Verification**: All tests passing, bringing the total validated assertions to 962.

## [0.7.18] - 2026-02-09
### Added
- **Batch Processing for Log Analyzer**:
  - Implemented a new `batch` command in the `lmuffb_log_analyzer` tool.
  - Automatically processes all `.csv` logs in a directory, running `info`, `analyze`, `plots`, and `report` for each.
  - Generates a centralized results directory containing reports and diagnostic plots for all captured sessions.
### Testing
- **New Test Suite**: `tools/lmuffb_log_analyzer/tests/test_batch.py`
  - Validates end-to-end directory processing and file generation logic.

## [0.7.17] - 2026-02-07
### Changed
- **Test Suite Refactoring (Completion)**:
  - **Full Auto-Registration Migration**: Completed the transition of the entire test suite to the self-registering `TEST_CASE` macro (it was started with version 0.7.8). Legacy `static void` tests in `test_screenshot.cpp`, `test_windows_platform.cpp`, and persistence files have been fully migrated.
  - **Infrastructure Consolidation**: Centralized assertion macros (`ASSERT_EQ`, `ASSERT_EQ_STR`, etc.) and global test counters into `test_ffb_common.h` and `test_ffb_common.cpp`.
  - **Unified Test Runner**: Streamlined `main_test_runner.cpp` to rely solely on the automated registry. Removed redundant manual `Run()` calls and namespace forward declarations, ensuring all 867+ assertions contribute to a single, unified report.
  - **Code Cleanup**: Removed duplicated logic and redundant headers across the test suite, improving compilation overhead and maintainability.

## [0.7.16] - 2026-02-06
### Added
- **Stability and Safety Improvements**:
  - **FFB Heartbeat Watchdog**: Implemented telemetry staleness detection. If the game crashes or freezes (telemetry stops updating for > 100ms), FFB is automatically muted to prevent the wheel from being "stuck" with high forces.
  - **Comprehensive Parameter Validation**: Added rigorous safety clamping for all physical parameters in `Preset::Apply` and `Config::Load`. This prevents application crashes or `NaN` outputs caused by manually editing `config.ini` with invalid/negative values (e.g., negative `lockup_gamma`, zero `max_torque_ref`).
### Testing
- **New Stability Test Suite**: `tests/test_ffb_stability.cpp`
  - `test_negative_parameter_safety`: Verifies that invalid inputs are correctly clamped and do not cause crashes.
  - `test_config_load_validation`: Validates robust loading from malformed `.ini` files.
  - `test_engine_robustness_to_static_telemetry`: Ensures the engine handles frozen telemetry gracefully.

## [0.7.15] - 2026-02-06
### Fixed
- **Build Error**: Resolved compilation error C2513 in `Config.cpp` by renaming the local lambda `near` to `is_near` to avoid collision with legacy MSVC macros.

## [0.7.14] - 2026-02-05
### Added
- **Preset Handling Improvements**:
  - **Last Preset Persistence**: The app now remembers and automatically selects the last used preset on startup.
  - **Dirty State Indication**: Added a `*` suffix to the preset name in the GUI when settings have been modified but not saved.
  - **Improved "Save Current Config"**: Now updates the currently selected user preset instead of just saving to the top-level config.
  - **Preset Management**: Added "Delete" and "Duplicate" buttons to the GUI for easier preset management.
  - **Safety**: Built-in presets are protected from deletion.

## [0.7.13] - 2026-02-05
### Added
- **Preset Import/Export**:
  - Implemented standalone preset file I/O (.ini) for easy sharing of FFB configurations.
  - Added "Import Preset..." and "Export Selected..." buttons to the GUI with native Win32 file dialogs.
  - Robust name collision handling for imported presets (automatic renaming).
### Testing
- **New Test Suite**: `tests/test_ffb_import_export.cpp`
  - `test_preset_export_import`: Verifies single-preset round-trip integrity.
  - `test_import_name_collision`: Validates automatic renaming logic.

## [0.7.12] - 2026-02-05
### Added
- **Preset & Telemetry Versioning**:
  - **Preset Version Tracking**: Added `app_version` field to user presets to track compatibility.
  - **Legacy Migration**: Automated migration of presets missing version information to the current `0.7.12` version.
  - **Telemetry Version Header**: Injected application version into telemetry log headers for improved offline analysis.
- **Improved Config Persistence**:
  - Fixed a long-standing bug where `gain` was not being correctly loaded from user presets.
  - Implemented auto-save trigger when legacy presets are migrated during load.
### Testing
- **New Test Suite**: `tests/test_versioned_presets.cpp`
  - `test_preset_version_persistence`: Verifies version round-trip.
  - `test_legacy_preset_migration`: Validates automatic version injection for old presets.
- **Updated Test**: `tests/test_async_logger.cpp`
  - `test_logger_header_version_check`: Confirms the CSV header contains the correct version string.

## [0.7.11] - 2026-02-05
### Added
- **Slope Detection Min/Max Threshold Mapping**:
  - Implemented a more intuitive and powerful threshold system for slope-driven understeer.
  - **Min Threshold**: The dG/dAlpha slope value where the effect begins (dead zone edge). Defaults to -0.3.
  - **Max Threshold**: The slope value where the effect reaches maximum saturation. Defaults to -2.0.
  - **Linear Mapping**: Replaced the previous sensitivity-based exponential scaling with a predictable linear mapping between thresholds.
- **Improved InverseLerp Helper**: Added `inverse_lerp` utility with robust floating-point safety and support for both positive and negative ranges, optimized for tire slope physics.
- **MSVC Build Warning Mitigation**: Implemented explicit divide-by-zero protection in `inverse_lerp` and `smoothstep` to suppress compiler warning C4723.

### Fixed
- **Legacy Migration**: Implemented automatic migration logic that converts old `slope_sensitivity` values to the new min/max threshold model, preserving the user's intended feel on first launch.
- **Slope Logic Robustness**: Fixed edge cases where zero-range thresholds could cause FFB spikes or invalid calculations.

### Testing
- **New Test Cases**: Added comprehensive verification for the new threshold system.
  - `test_inverse_lerp_helper`: Validates math across normal, reversed, and tiny ranges.
  - `test_slope_minmax_dead_zone`: Confirms zero force reduction before the min threshold.
  - `test_slope_minmax_linear_response`: Verifies proportional force mapping.
  - `test_slope_minmax_saturation`: Ensures the effect saturates correctly.
  - `test_slope_sensitivity_migration`: Validates that legacy configs are correctly translated.
- **Regression**: All tests passing.

## [0.7.10] - 2026-02-04
### Added
- **Log Analyzer Tool**:
  - **Standalone Python Tool**: Created a comprehensive diagnostic tool in `tools/lmuffb_log_analyzer/` for processing telemetry logs.
  - **Advanced Diagnostics**: Analyzes slope stability, detects oscillation events, and identifies potential FFB issues (e.g., stuck understeer, frequent floor hits).
  - **Visualizations**: Generates multi-panel time-series plots, tire curves (Slip Angle vs Lat G), and dAlpha/dt histograms using Matplotlib.
  - **Automated Reporting**: Produces formatted text reports with session metadata, physics statistics, and prioritized issue lists.
  - **CLI Interface**: Includes `info`, `analyze`, `plots`, and `report` commands for flexible workflow integration.
- **Documentation**: 
  - Created detailed `README.md` for the Log Analyzer tool.
  - Linked the Log Analyzer in the main project `README.md`.
  - Updated `plan_log_analyzer.md` with implementation notes and final status.
### Testing
- **Unit Tests**: Implemented 6 new test suites in `tools/lmuffb_log_analyzer/tests/` covering parsing, analysis math, and plot generation.
- **Verification**: All 6 Log Analyzer tests passing; manual CLI end-to-end verification completed.

## [0.7.9] - 2026-02-04
### Added
- **Telemetry Logger Integration**:
  - **High-Performance Asynchronous Logging**: Implemented a double-buffered 100Hz CSV logger that captures 40+ physics channels without impacting FFB loop performance.
  - **FFB Engine Integration**: Captures raw telemetry, intermediate slope detection derivatives (dG/dt, dAlpha/dt), and final force components.
  - **GUI Controls**: Added manual Start/Stop/Marker buttons and a recording status indicator ("REC").
  - **Auto-Start Support**: Optional setting to automatically begin logging when entering a driving session.
  - **Persistence**: New settings for `log_path` and `auto_start_logging` saved to `config.ini`.
  - **Context-Aware Filenames**: Log files are automatically named using timestamp, car name, and track name.

## [0.7.8] - 2026-02-04
### Changed
- **Test Suite Refactoring**:
  - **Auto-Registration Pattern**: Migrated the entire test suite (591 tests across 12 files) to use the self-registering `TEST_CASE` macro.
  - **Eliminated "Fixed but not Called" Bug**: Tests are now automatically registered at startup, removing the need to manually add them to `Run_Category()` functions.
  - **Code Cleanup**: Removed all `Run_*` legacy runner functions from `test_ffb_common.cpp` and `test_ffb_common.h`.
  - **Verification**: All 591 tests passing.

## [0.7.7] - 2026-02-04
### Changed
- **Test Suite Refactoring**:
  - Split `test_ffb_features.cpp` further by extracting road texture tests.
  - **New File**: `test_ffb_road_texture.cpp` containing 8 tests:
    - Road texture regression tests (Toggle, Teleport, Persistence)
    - Suspension bottoming tests (Scrape, Spike, Universal)
    - Scrub drag fade tests
  - **Verification**: All 590 tests passing using the new `Run_RoadTexture()` runner.
  - `test_ffb_features.cpp` now focuses on Integration, Speed Gate, and Notch Filter tests.

## [0.7.6] - 2026-02-04
### Changed
- **Test Suite Refactoring**:
  - Split `test_ffb_features.cpp` into two files:
    - `test_ffb_lockup_braking.cpp`: Contains 8 tests related to lockup, ABS, and braking forces.
    - `test_ffb_features.cpp`: Retains remaining road texture and effect integration tests.
  - **Verification**: All 590 tests passing using the new `Run_LockupBraking()` runner.
  - **Organization**: Improved test navigability by grouping braking physics into a dedicated file.

## [0.7.5] - 2026-02-03
### Changed
- **Test Suite Refactoring**: 
  - Completed the modularization of the test suite. The `test_ffb_engine.cpp` file has been fully split into 9 modular files.
  - **Verification**: All tests are passing. The total test count is **591**, successfully restoring the 5 tests that were temporarily missing in v0.7.4.
  - **Restored Tests**:
    - `Rear Force Workaround active` assertion
    - `Rear slip angle` assertions (POSITIVE/NEGATIVE)
    - `Config Safety Validation` assertions for optimal slip ratio and small value resets
  - This completes the infrastructure refactoring with zero physics changes.

## [0.7.4] - 2026-02-03
### Changed
- **Test Suite Refactoring**: 
  - Split the monolithic `test_ffb_engine.cpp` (7,263 lines) into 9 modular category files (`test_ffb_core_physics`, `test_ffb_slip_grip`, etc.) for improved maintainability.
  - Introduced `test_ffb_common.h` and `test_ffb_common.cpp` for shared test infrastructure and helpers.
  - Incomplete implementation was pushed to git due to quota limits. Only 586 tests are currently present (5 less than the original 591).
  - Re-verified some of the regression tests (586?) to ensure physics integrity during the migration.

## [0.7.3] - 2026-02-03
### Fixed
- **Slope Detection Stability Fixes**:
  - **Decay on Straight**: Implemented a decay mechanism that returns the slope to zero when steering movement is below a threshold. This eliminates the "sticky" understeer feel where the wheel would remain light after exiting a corner.
  - **Configurability**: Added `slope_alpha_threshold` and `slope_decay_rate` parameters to allow user fine-tuning of stability vs. response time.
  - **Confidence Gate**: Introduced an optional confidence-based grip scaling. The grip loss effect is now scaled by the magnitude of `dAlpha/dt`, automatically smoothing out random FFB jolts during low-intensity maneuvers.
- **Physics Calculation Robustness**:
  - Added protection against large telemetry discontinuities (e.g., jumps from high slip to zero) which previously could cause garbage derivatives and "FFB explosions."
  - Improved Savitzky-Golay derivative calculations by enforcing a configurable minimum activity threshold (`m_slope_alpha_threshold`).

### Added
- **v0.7.3 Stability Controls**:
  - **Alpha Threshold**: Minimum `dAlpha/dt` (rad/s) required to update the slope calculation.
  - **Decay Rate**: Time constant for how fast the slope "forgets" the previous cornering state.
  - **Confidence Gate Toggle**: Enable/Disable the steering-speed based confidence scaling.
- **Verification Suite (v0.7.3)**:
  - `test_slope_decay_on_straight`: Verifies that `m_slope_current` returns to zero after cornering.
  - `test_slope_alpha_threshold_configurable`: Confirms the calculation only updates when steering movement is sufficient.
  - `test_slope_confidence_gate`: Validates the mathematical scaling of the grip effect based on `dAlpha/dt`.
  - `test_slope_stability_config_persistence`: Ensures new parameters are correctly saved to `config.ini` and presets.
  - `test_slope_no_understeer_on_straight_v073`: End-to-end physics test confirming zero force reduction when driving straight after a slide.
  - `test_slope_decay_rate_boundaries`: Verifies the impact of different decay rates on recovery speed.
  - `test_slope_alpha_threshold_validation`: Conforms that out-of-range parameters are clamped to safe defaults during loading.

### Changed
- **Config Persistence Logic**: Updated `Config::Save`, `Config::Load`, and `Preset` structures to include the new stability parameters, maintaining full backward compatibility.
- **GUI Organization**: Grouped the new stability controls under a dedicated "Stability Fixes (v0.7.3)" sub-header within the Physics section for better discoverability.

## [0.7.2] - 2026-02-03

### Added
- **Smoothstep Speed Gating**:
  - Replaced linear interpolation for speed-based FFB gating with a smooth Hermite S-curve (Smoothstep).
  - Implemented `smoothstep(edge0, edge1, x)` helper function in `FFBEngine` using polynomial `t² * (3 - 2t)`.
  - Provides zero-derivative endpoints for a more natural "fade-in" of FFB forces at low speeds.
  - Improves the pit-lane and stationary-to-moving transition by eliminating the "angular" feel of linear scaling.
- **v0.7.2 Test Suite**:
  - `test_smoothstep_helper_function`: Verifies mathematical correctness of the Hermite polynomial.
  - `test_smoothstep_vs_linear`: Confirms the S-curve characteristic (above/below linear at different points).
  - `test_smoothstep_edge_cases`: Validates boundary safety (below/above thresholds) and zero-range handling.
  - `test_speed_gate_uses_smoothstep`: End-to-end physics test verifying the non-linear force ratio at intermediate speeds.
  - `test_smoothstep_stationary_silence_preserved`: Ensures no regression in stationary noise suppression.

### Changed
- **FFB Speed Gating Logic**: Refactored `calculate_force` to use the new `smoothstep` helper, simplifying the code while enhancing physical fidelity.

## [0.7.1] - 2026-02-02

### Fixed
- **Slope Detection Stability**:
  - Automatically disable **Lateral G Boost (Slide)** when Slope Detection is enabled. This prevents artificial grip-delta oscillations caused by asymmetric calculation methods between axles.
  - Updated default parameters for a more conservative and smoother experience on initial activation (Sensitivity: 0.5, Smoothing Tau: 0.04s).
- **GUI & UX Improvements**:
  - Added a dedicated **Slope (dG/dAlpha) Graph** in the Internal Physics section for real-time monitoring of the tire curve derivative.
  - Implemented detailed **Tuning Tooltips** for the SG Filter Window, providing hardware-specific recommendations for Direct Drive, Belt, and Gear-driven wheels.
  - Added a dashboard warning when both Slope Detection and Lateral G Boost are conceptually enabled, informing the user that the boost has been auto-suppressed.
- **Diagnostic Coverage**:
  - Populated the `slope_current` field in `FFBSnapshot` to ensure slope detection state is visible in telemetry logs and debugging tools.

### Added
- **v0.7.1 Regression Tests**:
  - `test_slope_detection_defaults_v071`: Verifies the new, less aggressive default parameters.
  - `test_slope_detection_diag_v071`: Confirms the slope diagnostic is correctly populated in snapshots.
  - `test_slope_detection_boost_disable_v071`: Ensures Lateral G Boost is properly suppressed when slope detection is active.
  - `test_slope_detection_less_aggressive_v071`: End-to-end physics test verifying the reduced sensitivity compared to v0.7.0.

## [0.7.0] - 2026-02-01

### Added
- **Dynamic Slope Detection (Tire Peak Grip Monitoring)**:
  - Implemented real-time dynamic grip estimation using Lateral G vs. Slip Angle slope monitoring.
  - Replaces static "Grip Loss" thresholds with an adaptive system that detects the tire's saturation point regardless of track conditions or car setups.
  - Added Savitzky-Golay (SG) filtering (window size 15-31) to calculate high-fidelity derivatives of noisy telemetry data.
  - **Front-Axle Specific**: Slope detection is applied exclusively to the front axle for precise understeer communication, while maintaining stability for rear grip effects.

### Fixed
- **Preset-Engine Synchronization**:
  - Fixed a critical regression where newly added parameters (optimal_slip_angle, slope_detection, etc.) were missing from `Preset::Apply()` and `Preset::UpdateFromEngine()`.
  - Resolved "Invalid optimal_slip_angle (0)" warnings that occurred when applying presets.
- **FFBEngine Initialization Order**:
  - Fixed circular dependency between `FFBEngine.h` and `Config.h` by moving the constructor to `Config.h`.

### Improved
- **Understeer Effect Fidelity**:
  - The Understeer Effect now utilizes the dynamic slope to determine force reduction, providing a much more organic and informative "light wheel" feel during front-end slides.
  - Added safety clamping (0.2 floor) and smoothing (configurable tau) to ensure stable feedback during rapid transitions.

### Added
- **Regression Test Suite**:
  - Added 11+ new tests covering slope detection math, noise rejection, SG filter coefficients, and Preset-Engine synchronization.
  - Added `test_preset_engine_sync_regression` to prevent future parameter synchronization omissions.
