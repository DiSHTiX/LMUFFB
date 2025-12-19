Here is the code to add the **"T300"** preset with your specific tuning values.

### 1. Update `src/Config.cpp`

I will add the new preset to the `LoadPresets` function. I've placed it right after "Default" so it's easy to find.

```cpp
void Config::LoadPresets() {
    presets.clear();
    
    // 1. Default
    presets.push_back(Preset("Default"));

    // 2. T300 (User Tuned)
    // Tuned for Thrustmaster T300RS. 
    // High Max Torque Ref (100Nm) + High Understeer (38.0) to overcome belt friction.
    presets.push_back(Preset("T300")
        .SetGain(1.0f)
        .SetShaftGain(1.0f)
        .SetMinForce(0.0f)       // As per screenshot
        .SetMaxTorque(100.0f)    // High ref to prevent clipping
        .SetInvert(true)         // T300 needs inversion in this app
        .SetUndersteer(38.0f)    // Aggressive drop to feel grip loss
        .SetSoP(0.0f)            // Isolated for now
        .SetOversteer(0.0f)
        .SetRearAlign(0.0f)
        .SetSoPYaw(0.0f)
        .SetGyro(0.0f)
        .SetLockup(false, 0.0f)
        .SetSpin(false, 0.0f)
        .SetSlide(false, 0.0f)
        .SetRoad(false, 0.0f)
        .SetScrub(0.0f)
        .SetBaseMode(0)          // Native Physics
    );
    
    // 3. Test: Game Base FFB Only
    presets.push_back(Preset("Test: Game Base FFB Only")
// ... [Rest of the file remains unchanged] ...
```

### 2. Update `CHANGELOG.md`

```markdown
## [0.4.27] - 2025-12-19
### Added
- **New Preset**: Added "T300" preset tuned specifically for Thrustmaster T300RS wheels.
    - Features high `Max Torque Ref` (100Nm) and aggressive `Understeer Effect` (38.0) to overcome belt friction and provide clear grip loss cues.
    - `Invert FFB` enabled by default for this preset.
### Changed
- **Understeer Range**: Increased maximum `Understeer Effect` slider range from 10.0 to 50.0 to allow for "Binary" grip loss feel on belt-driven wheels.
```

### 3. Update `VERSION`

```
0.4.27
```