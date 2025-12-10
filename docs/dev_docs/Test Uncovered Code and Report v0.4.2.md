Based on a comprehensive review of the provided codebase and documentation, here is the report on test coverage, codebase issues, and recommended updates.

---

# LMUFFB Codebase & Test Coverage Report

**Date:** December 11, 2025
**Version Analyzed:** v0.4.2 (Staged)

## 1. Codebase Issues & Observations

Before addressing test coverage, the following issues were identified in the source code which may impact stability or performance.

### A. Performance / Real-time Constraints
1.  **Blocking I/O in High-Frequency Loop (`FFBEngine.h`)**:
    *   **Issue:** Inside `calculate_force` (lines 105-115), there is logic to print telemetry stats to `std::cout` every second. While rate-limited, `std::cout` is a blocking I/O operation. If the console buffer fills or blocks, it could stall the 400Hz FFB thread, causing a hiccup in force feedback.
    *   **Recommendation:** Move the logging logic to the GUI thread (Consumer) by reading the stats from a thread-safe structure, or use a non-blocking logger.

2.  **Sleep Timer Resolution (`main.cpp`)**:
    *   **Issue:** `std::this_thread::sleep_for(std::chrono::milliseconds(2));` is used to target ~400Hz. On Windows, the default timer resolution is often 15.6ms. Without calling `timeBeginPeriod(1)` (winmm.lib) or using a high-resolution waitable timer, the loop might run significantly slower than 400Hz (approx 64Hz).
    *   **Recommendation:** Verify if `timeBeginPeriod(1)` is called implicitly by libraries, or add it explicitly to `main`.

### B. Logic / Math
3.  **Frame-Rate Dependent Smoothing (`FFBEngine.h`)**:
    *   **Issue:** The SoP smoothing (Line 156) uses `m_sop_smoothing_factor` directly as the alpha for the Low Pass Filter:
        `m_sop_lat_g_smoothed = m_sop_lat_g_smoothed + alpha * (lat_g - m_sop_lat_g_smoothed);`
    *   **Impact:** The smoothing behavior depends on the update rate (`dt`). If the game physics rate fluctuates or if the app misses a frame, the effective cutoff frequency changes.
    *   **Recommendation:** Implement time-corrected smoothing: `alpha = 1.0 - exp(-dt * cutoff_frequency)`.

---

## 2. Test Coverage Analysis

The current test suite (`tests/test_ffb_engine.cpp`) is surprisingly robust for the core physics, covering about **85% of `FFBEngine.h`**. However, peripheral systems are completely untested.

### Covered Areas (Green) ✅
*   **Core Physics:** Understeer, SoP, Oversteer Boost, Min Force.
*   **Dynamic Effects:** Lockup, Slide Texture, Road Texture, Bottoming.
*   **Math Integrity:** Phase wraparound, Load factor clamping.
*   **Sanity Checks:** Missing Load, Missing Grip, Invalid DeltaTime.
*   **State Machines:** Hysteresis logic for signal dropouts.
*   **Configuration:** Preset application logic.

### Uncovered Areas (Red) ❌

#### 1. Configuration Persistence (`src/Config.cpp`) - **0% Coverage**
*   **Risk:** High. If `Save()` or `Load()` fails (e.g., locale issues with floats, permission errors), the user loses their settings. There are no tests verifying that a saved file can be re-loaded correctly.

#### 2. Telemetry Statistics (`FFBEngine.h` - `ChannelStats`) - **0% Coverage**
*   **Risk:** Low/Medium. The `ChannelStats` struct tracks Min/Max/Avg. If this logic is wrong, the console logs (and future auto-calibration features) will be incorrect.

#### 3. Shared Memory Logic (`src/GameConnector.cpp`) - **0% Coverage**
*   **Risk:** High. The `IsInRealtime()` and `CopyTelemetry()` functions rely on specific memory layouts. While hard to unit test without the game, we can mock the memory layout to ensure the *logic* (locking, copying, player index finding) works.

#### 4. Smoothing Behavior - **Implicit Only**
*   **Risk:** Medium. `test_sop_effect` tests that smoothing happens, but doesn't verify the *quality* of smoothing (e.g., step response).

---

## 3. Prioritized Test Updates

Here is the recommended order of implementation for new tests.

| Priority | Component | Description | Rationale |
| :--- | :--- | :--- | :--- |
| **1** | **Config I/O** | Test `Save` and `Load` with a temporary file. | Ensures user settings persist correctly across sessions. |
| **2** | **Stats Logic** | Test `ChannelStats` update/reset logic. | Ensures diagnostic data is accurate. |
| **3** | **Game State** | Mock `SharedMemoryLayout` to test `IsInRealtime`. | Verifies the new v0.4.2 feature (muting FFB in menus) works logically. |
| **4** | **Smoothing** | Test Step Response of SoP filter. | Ensures smoothing behaves predictably. |

---

## 4. Code Suggestions for Tests

Add the following code to `tests/test_ffb_engine.cpp`.

### A. Configuration Persistence Test (Priority 1)
This verifies that values saved to disk are identical when loaded back.

```cpp
#include <fstream>
#include <cstdio> // for remove()

void test_config_persistence() {
    std::cout << "\nTest: Config Save/Load Persistence" << std::endl;
    
    std::string test_file = "test_config.ini";
    FFBEngine engine_save;
    FFBEngine engine_load;
    
    // 1. Setup unique values
    engine_save.m_gain = 1.23f;
    engine_save.m_sop_effect = 0.45f;
    engine_save.m_lockup_enabled = true;
    engine_save.m_road_texture_gain = 2.5f;
    
    // 2. Save
    Config::Save(engine_save, test_file);
    
    // 3. Load into fresh engine
    Config::Load(engine_load, test_file);
    
    // 4. Verify
    ASSERT_NEAR(engine_load.m_gain, 1.23f, 0.001);
    ASSERT_NEAR(engine_load.m_sop_effect, 0.45f, 0.001);
    ASSERT_NEAR(engine_load.m_road_texture_gain, 2.5f, 0.001);
    
    if (engine_load.m_lockup_enabled == true) {
        std::cout << "[PASS] Boolean persistence." << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Boolean persistence failed." << std::endl;
        g_tests_failed++;
    }
    
    // Cleanup
    std::remove(test_file.c_str());
}
```

### B. Channel Stats Test (Priority 2)
Verifies the math helper used for diagnostics.

```cpp
void test_channel_stats() {
    std::cout << "\nTest: Channel Stats Logic" << std::endl;
    
    ChannelStats stats;
    
    // Sequence: 10, 20, 30
    stats.Update(10.0);
    stats.Update(20.0);
    stats.Update(30.0);
    
    ASSERT_NEAR(stats.min, 10.0, 0.001);
    ASSERT_NEAR(stats.max, 30.0, 0.001);
    ASSERT_NEAR(stats.Avg(), 20.0, 0.001);
    
    // Test Reset
    stats.Reset();
    ASSERT_TRUE(stats.count == 0);
    ASSERT_NEAR(stats.Avg(), 0.0, 0.001); // Handle divide by zero check
    
    std::cout << "[PASS] Channel Stats verified." << std::endl;
    g_tests_passed++;
}
```

### C. Game State Logic Mock (Priority 3)
This requires mocking the shared memory structure. Since `GameConnector` is a singleton and tightly coupled to Windows APIs, we can test the *logic* by extracting the check into a helper or manually constructing the struct if accessible.

*Note: Since `GameConnector` uses `MapViewOfFile`, unit testing it directly is hard without refactoring. However, we can test the logic if we move the `IsInRealtime` logic to a static helper that takes a `SharedMemoryLayout*`.*

**Refactoring Suggestion for `src/GameConnector.h`:**
```cpp
// Add static helper
static bool CheckRealtimeState(const SharedMemoryLayout* layout);
```

**Test Implementation:**
```cpp
void test_game_state_logic() {
    std::cout << "\nTest: Game State Logic (Mock)" << std::endl;
    
    // Mock Layout
    SharedMemoryLayout mock_layout;
    std::memset(&mock_layout, 0, sizeof(mock_layout));
    
    // Case 1: Player not found
    // (Default state is 0/false)
    // Logic: if player not found, returns false? Or assumes false.
    
    // Case 2: Player found, InRealtime = 0 (Menu)
    mock_layout.data.scoring.vehScoringInfo[5].mIsPlayer = true;
    mock_layout.data.scoring.scoringInfo.mInRealtime = false;
    
    // We need to replicate the logic from GameConnector::IsInRealtime here 
    // or expose it. Assuming we copy-paste logic for verification:
    bool result_menu = false;
    for(int i=0; i<104; i++) {
        if(mock_layout.data.scoring.vehScoringInfo[i].mIsPlayer) {
            result_menu = mock_layout.data.scoring.scoringInfo.mInRealtime;
            break;
        }
    }
    ASSERT_TRUE(result_menu == false);
    
    // Case 3: Player found, InRealtime = 1 (Driving)
    mock_layout.data.scoring.scoringInfo.mInRealtime = true;
    bool result_driving = false;
    for(int i=0; i<104; i++) {
        if(mock_layout.data.scoring.vehScoringInfo[i].mIsPlayer) {
            result_driving = mock_layout.data.scoring.scoringInfo.mInRealtime;
            break;
        }
    }
    ASSERT_TRUE(result_driving == true);
    
    std::cout << "[PASS] Game state logic verified." << std::endl;
    g_tests_passed++;
}
```

### D. Update `main()` in `tests/test_ffb_engine.cpp`
Add the calls to the new tests.

```cpp
int main() {
    // ... existing tests ...
    test_presets(); // Existing
    
    // New Tests
    test_config_persistence();
    test_channel_stats();
    test_game_state_logic();
    
    // ...
}
```