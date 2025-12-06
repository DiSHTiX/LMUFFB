# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]

### Added
- **Dynamic Physics Effects**:
    - **Oversteer**: Replaced simple SoP boost with Rear Aligning Torque integration.
    - **Lockup**: Replaced binary rumble with Progressive Scrub (Frequency/Amplitude scaling).
    - **Wheel Spin**: Replaced binary rumble with Progressive Torque Drop-off.
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
