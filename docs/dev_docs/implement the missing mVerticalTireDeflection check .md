Here is the plan to implement the missing `mVerticalTireDeflection` check and the corresponding test update.

### 1. Update `src/FFBEngine.h`

We need to add the state variables to track the missing frames and the warning flag, then implement the hysteresis logic inside `calculate_force`.

**Add to Class Member Variables (under `// Warning States` and `// Hysteresis`):**

```cpp
// In FFBEngine class

    // ... existing warning flags ...
    bool m_warned_susp_deflection = false;
    bool m_warned_vert_deflection = false; // <--- ADD THIS

    // ... existing hysteresis counters ...
    int m_missing_susp_deflection_frames = 0;
    int m_missing_vert_deflection_frames = 0; // <--- ADD THIS
```

**Add Logic inside `calculate_force` (near the other Sanity Checks):**

```cpp
        // ... [Existing checks for mSuspensionDeflection] ...

        // 5. Vertical Tire Deflection (mVerticalTireDeflection) - NEW
        // Check: If exactly 0.0 while moving fast. 
        // Real tires always deflect slightly due to load/road noise.
        // We use a higher speed threshold (10 m/s) to ensure dynamic forces exist.
        double avg_vert_def = (std::abs(fl.mVerticalTireDeflection) + std::abs(fr.mVerticalTireDeflection)) / 2.0;
        
        if (avg_vert_def < 0.000001 && std::abs(data->mLocalVel.z) > 10.0) {
            m_missing_vert_deflection_frames++;
        } else {
            m_missing_vert_deflection_frames = (std::max)(0, m_missing_vert_deflection_frames - 1);
        }
        
        if (m_missing_vert_deflection_frames > 50 && !m_warned_vert_deflection) {
            std::cout << "[WARNING] mVerticalTireDeflection is missing for car: " << data->mVehicleName 
                      << ". (Likely Encrypted/DLC Content). Road Texture fallback active." << std::endl;
            m_warned_vert_deflection = true;
        }
```

---

### 2. Update `tests/test_ffb_engine.cpp`

We will extend the existing `test_missing_telemetry_warnings` function to verify this new check.

**Update `test_missing_telemetry_warnings`:**

```cpp
static void test_missing_telemetry_warnings() {
    std::cout << "\nTest: Missing Telemetry Warnings (v0.6.3)" << std::endl;
    FFBEngine engine;
    InitializeEngine(engine);
    TelemInfoV01 data = CreateBasicTestTelemetry(20.0);
    
    // Set Vehicle Name
    strcpy_s(data.mVehicleName, "TestCar_GT3");

    // Capture stdout
    std::stringstream buffer;
    std::streambuf* prev_cout_buf = std::cout.rdbuf(buffer.rdbuf());

    // --- Case 1: Missing Grip ---
    // ... [Existing Grip Test Code] ...

    // --- Case 2: Missing Suspension Force ---
    // ... [Existing SuspForce Test Code] ...

    // --- Case 3: Missing Vertical Tire Deflection (NEW) ---
    // Reset output buffer
    buffer.str("");
    
    // Set Vertical Deflection to 0.0 (Missing)
    for(int i=0; i<4; i++) data.mWheel[i].mVerticalTireDeflection = 0.0;
    
    // Ensure speed is high enough to trigger check (> 10.0 m/s)
    data.mLocalVel.z = 20.0; 

    // Run for 60 frames to trigger hysteresis (> 50 frames)
    for(int i=0; i<60; i++) {
        engine.calculate_force(&data);
    }
    
    std::string output = buffer.str();
    bool vert_warn = output.find("[WARNING] mVerticalTireDeflection is missing") != std::string::npos;
    
    if (vert_warn) {
        std::cout.rdbuf(prev_cout_buf);
        std::cout << "[PASS] Vertical Deflection warning triggered." << std::endl;
        g_tests_passed++;
        std::cout.rdbuf(buffer.rdbuf());
    } else {
        std::cout.rdbuf(prev_cout_buf);
        std::cout << "[FAIL] Vertical Deflection warning missing." << std::endl;
        g_tests_failed++;
        std::cout.rdbuf(buffer.rdbuf());
    }

    // Restore cout
    std::cout.rdbuf(prev_cout_buf);
}
```