# Implementation Plan - Remove vJoy Support and Runtime Library Loading

This plan outlines the steps to remove vJoy support from the LMUFFB application. This change is motivated by the desire to eliminate `LoadLibraryA` calls, which are flagged by some antivirus software as suspicious behavior (runtime library loading), and to simplify the codebase since vJoy functionality is no longer required.

## User Review Required

> [!IMPORTANT]
> This change permanently removes the ability to output FFB signals to a vJoy virtual device. Ensure that no critical workflows depend on this feature.

## Proposed Changes

### 1. Remove Source Files

Delete the following files which are exclusively used for vJoy integration:
- `src/DynamicVJoy.h`

### 2. Codebase Modifications

#### `src/main.cpp`
- Remove `#include "DynamicVJoy.h"`.
- Remove `VJOY_DEVICE_ID` constant.
- Remove vJoy initialization logic (attempt to load DLL).
- Remove vJoy acquisition loop inside the main application loop.
- Remove logic that feeds FFB data to the vJoy device (`DynamicVJoy::Get().SetAxis(...)`).
- Remove vJoy relinquishment logic on exit.

#### `src/Config.h` & `src/Config.cpp`
- Remove static member variables:
    - `m_enable_vjoy`
    - `m_output_ffb_to_vjoy`
    - `m_ignore_vjoy_version_warning`
- Remove initialization of these variables in `Config.cpp`.
- Remove loading and saving logic for these keys in `Config::Load` and `Config::Save`.

#### `CMakeLists.txt`
- Remove `src/DynamicVJoy.h` from the `APP_SOURCES` list.

### 3. Documentation Updates

#### `README.md`
- Remove sections describing vJoy setup, usage, and "vJoy not found" messages.
- Clean up any troubleshooting steps related to vJoy.

## Verification Plan

### Automated Tests
- Run `run_combined_tests.exe`.
- Use the `test_windows_platform` suite to ensure no regressions in basic Windows functionality.
- Since vJoy was an optional runtime component, existing tests should pass without modification (unless they explicitly checked for vJoy config flags, which is unlikely based on analysis).

### Manual Verification
- Build the application in `Release` mode.
- Verify that the application starts successfully without `vJoyInterface.dll` present.
- Confirm that the console no longer prints `[vJoy]` messages.
- Verify that standard FFB output to the physical wheel still works correctly.
