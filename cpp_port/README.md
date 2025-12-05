# LMUFFB C++ Port Documentation

This directory contains the C++ implementation of the LMU Force Feedback App. This version is intended to be the high-performance, low-latency successor to the Python prototype.

## 1. Building the Application

### Prerequisites
*   **Compiler**: MSVC (Visual Studio 2022 Build Tools) or generic C++ compiler.
*   **Build System**: CMake (3.10+).
*   **SDKs**: 
    *   **vJoy SDK**: Download from [vJoy GitHub](https://github.com/shauleiz/vJoy). Extract it and note the path.

### Option A: Visual Studio 2022 (IDE)
1.  Open Visual Studio.
2.  Select "Open a local folder" and choose the `cpp_port` directory.
3.  Visual Studio will auto-detect `CMakeLists.txt`.
4.  Open `CMakeSettings.json` (or Project Settings) to define the variable `VJOY_SDK_DIR` pointing to your vJoy SDK location (e.g., `C:/Program Files/vJoy/SDK`).
5.  Select **Build > Build All**.

### Option B: Visual Studio Code
1.  Install **VS Code**.
2.  Install extensions: **C/C++** (Microsoft) and **CMake Tools** (Microsoft).
3.  Open the `cpp_port` folder in VS Code.
4.  When prompted to configure CMake, select your installed compiler kit (e.g., *Visual Studio Community 2022 Release - x86_amd64*).
5.  Open `.vscode/settings.json` (or create it) to set the vJoy path:
    ```json
    {
        "cmake.configureSettings": {
            "VJOY_SDK_DIR": "C:/Path/To/vJoy/SDK"
        }
    }
    ```
6.  Click **Build** in the bottom status bar.

### Option C: Command Line (Windows)
1.  Open the **Developer Command Prompt for VS 2022**.
2.  Navigate to the `cpp_port` directory.
3.  Run the following commands:
    ```cmd
    mkdir build
    cd build
    cmake -G "NMake Makefiles" -DVJOY_SDK_DIR="C:/Path/To/vJoy/SDK" ..
    nmake
    ```
    *Alternatively, use `cmake --build .` instead of `nmake`.*

---

## 2. Missing Features for Production Readiness

This C++ port is currently a **Console Application**. To be considered a polished consumer product, the following features must be implemented:

### A. Graphical User Interface (GUI)
*   **Current State**: None (Command Line Interface).
*   **Requirement**: A lightweight GUI to manage settings without editing files or code.
*   **Implementation Strategy**: 
    *   Use **Dear ImGui** (DX11/OpenGL backend) for a lightweight, high-performance overlay or window. 
    *   Alternatively, use **Qt** for a native look but higher footprint.
*   **Controls Needed**:
    *   Sliders for Gain, SoP, Smoothing.
    *   Dropdown for vJoy Device selection.
    *   "Start/Stop" toggle button.
    *   Visual clipping indicator (bar graph showing force output).

### B. Robust Error Handling
*   **Current State**: Prints errors to `std::cerr` and exits.
*   **Requirement**: User-friendly popups.
*   **Implementation**:
    *   Use `MessageBoxA(NULL, "Error Message", "LMUFFB Error", MB_ICONERROR)` for critical failures (e.g., Shared Memory not found).
    *   Status bar in the GUI for non-critical warnings.

### C. Installation & Plugin Management
*   **Current State**: Manual copy-paste of DLLs.
*   **Requirement**: An "Auto-Setup" feature.
*   **Implementation**:
    *   The app should detect the LMU installation path (via Registry or Steam library search).
    *   Check if `rFactor2SharedMemoryMapPlugin64.dll` is in `Plugins/`.
    *   Check `CustomPluginVariables.JSON` and automatically edit it to enable the plugin if disabled.

### D. DirectInput Integration
*   **Current State**: Relies on vJoy.
*   **Requirement**: Direct hardware access (optional but better).
*   **Implementation**: 
    *   Implement `IDirectInputDevice8` to send Constant Force packets directly to the wheel driver, bypassing vJoy. This mimics iRFFB's "Direct Mode" and reduces latency/complexity for end-users.

---

## 3. Future Enhancements

*   **Minimization to Tray**: App should run in the background with a System Tray icon.
*   **Auto-Launch**: Option to start automatically when LMU is launched (process detection).
*   **Profile System**: Save different settings for different cars (e.g., GTE vs Hypercar).
