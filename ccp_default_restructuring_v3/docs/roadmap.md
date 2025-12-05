# Roadmap & Future Development

To evolve LMUFFB from a prototype to a daily-driver application, the following steps are recommended:

## Completed Features (C++ Port)
*   [x] **Native C++ Port**: Migrated from Python to C++ for performance.
*   [x] **FFB Engine**: Implemented Grip Modulation, SoP, Min Force.
*   [x] **Texture Effects**: Implemented Slide Texture (noise) and Road Texture (suspension delta).
*   [x] **Architecture**: Threaded design (FFB 400Hz / Main 60Hz).
*   [x] **Testing**: Comprehensive C++ Unit Test suite.

## Short Term
*   **GUI Implementation**: Add a Graphical User Interface using **Dear ImGui**.
    *   Sliders for Gain, Understeer, SoP, Smoothing.
    *   Toggles for Textures.
    *   Visual clipping bar.
*   **Config Persistence**: Save/Load user settings to an `.ini` or `.json` file.

## Medium Term
*   **Installer**: Create an installer (InnoSetup or similar) that:
    *   Installs the app.
    *   Checks/Installs vJoy.
    *   Auto-installs the `rFactor2SharedMemoryMapPlugin64.dll` to the game folder.
*   **DirectInput FFB Support**: Move beyond vJoy "Axis" mapping. Implement proper DirectInput "Constant Force" packet sending.

## Long Term (Performance)
*   **Wheel-Specific Modes**: Add specific protocols for popular bases (Fanatec, Simucube, Logitech) to display data on wheel screens (RPM LEDs) using the telemetry data.
