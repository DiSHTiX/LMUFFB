# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]

### Added
- **GUI Error Popup**: Displays a MessageBox error if the Shared Memory Map cannot be opened (e.g., game not running), improving user feedback.
- **Configuration Persistence**: Added `Config.h/cpp` to save and load user settings to `config.ini`.
- **GUI Controls**: Added "Save Configuration" and "Reset Defaults" buttons to the main window.
- **Changelog**: Started tracking changes in `CHANGELOG.md`.

### Fixed
- Fixed compilation errors in `FFBEngine.h` due to Windows `min`/`max` macro conflicts by adding `#define NOMINMAX`.
- Fixed type mismatch in `GuiLayer.cpp` (`double` vs `float`) by updating `FFBEngine` member types to `float`.
- Fixed vJoy DLL linking issue by updating `CMakeLists.txt` to conditionally link x86/x64 libs.
