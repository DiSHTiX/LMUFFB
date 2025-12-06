# Changelog

All notable changes to this project will be documented in this file.

## [0.3.0] - Unreleased

### Added
- **Dynamic Physics Effects (v0.3)**:
    - **Phase Integration**: Implemented phase accumulation for all oscillators to eliminate audio/FFB clicks.
    - **Progressive Lockup**: Frequency linked to Car Speed, Amplitude linked to Tire Load.
    - **Traction Loss**: Frequency linked to Slip Speed (Car Speed * Slip Ratio), Amplitude linked to Slip Severity.
    - **Slide Texture**: Frequency linked to Lateral Patch Velocity, Waveform changed to Sawtooth.
    - **Load Sensitivity**: Effects now scale with vertical tire load (heavier load = stronger effect).
- **DirectInput Support**: Implemented native FFB via DirectInput. Users can now select their physical wheel in the GUI to receive force feedback directly.
- **Device Selection**: Added a dropdown in the GUI to enumerate and select FFB devices.
- **GUI Error Popup**: Displays a MessageBox error if the Shared Memory Map cannot be opened (e.g., game not running), improving user feedback.
- **Configuration Persistence**: Added `Config.h/cpp` to save and load user settings to `config.ini`.
- **GUI Controls**: Added "Save Configuration" and "Reset Defaults" buttons to the main window.
- **Changelog**: Started tracking changes in `CHANGELOG.md`.

### Fixed
- Fixed compilation errors in `FFBEngine.h` due to Windows `min`/`max` macro conflicts by adding `#define NOMINMAX`.
- Fixed type mismatch in `GuiLayer.cpp` (`double` vs `float`) by updating `FFBEngine` member types to `float`.
- Fixed vJoy DLL linking issue by updating `CMakeLists.txt` to conditionally link x86/x64 libs.
