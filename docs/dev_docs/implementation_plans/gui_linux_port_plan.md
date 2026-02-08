```markdown
# Implementation Plan - Linux Port (GLFW + OpenGL)

## Context
The goal is to make the LMUFFB application compatible with Linux (specifically Ubuntu) to enable Continuous Integration (CI) and improve code quality via cross-platform compilation. While the core Force Feedback (DirectInput) and Telemetry (Shared Memory) features are Windows-specific, the GUI and Physics Engine can run on Linux. This plan details replacing the DirectX 11 backend with GLFW/OpenGL for Linux builds and mocking the Windows-specific hardware layers.

## Reference Documents
*   **User Request:** Port GUI to Linux using GLFW + OpenGL.
*   **Analysis:** Existing codebase relies heavily on `windows.h`, `dinput.h`, and DirectX 11.

## Codebase Analysis Summary

### Current Architecture
*   **Entry Point (`src/main.cpp`):** Initializes `DirectInputFFB`, `GameConnector`, and `GuiLayer`. Contains the main application loop.
*   **GUI Layer (`src/GuiLayer.cpp`):** Tightly couples Dear ImGui logic with DirectX 11 initialization and Win32 message handling (`WndProc`).
*   **Hardware Layer (`src/DirectInputFFB.cpp`):** Uses DirectInput API for FFB.
*   **Telemetry Layer (`src/GameConnector.cpp`):** Uses Windows Shared Memory APIs (`OpenFileMapping`).
*   **vJoy Layer (`src/DynamicVJoy.h`):** Loads `vJoyInterface.dll`.

### Impacted Functionalities
1.  **Build System:** `CMakeLists.txt` needs to handle platform-specific dependencies (GLFW/OpenGL vs DX11/DInput).
2.  **GUI Initialization:** `GuiLayer::Init`, `Render`, and `Shutdown` must be split into platform-specific implementations.
3.  **Hardware Interfaces:** `DirectInputFFB`, `GameConnector`, and `DynamicVJoy` must be mocked on Linux to prevent compilation errors and runtime crashes.
4.  **Main Loop:** `main.cpp` needs to handle the lack of `HWND` on Linux and potential differences in thread sleeping/timing.

## FFB Effect Impact Analysis
*   **No Impact on Logic:** The core physics logic in `FFBEngine.cpp` is platform-agnostic and will remain identical.
*   **User Perspective (Linux):** Users can launch the app, adjust sliders, and view the "Tuning" and "Debug" windows. However, **no FFB will be generated** and **no telemetry will be received**. The app will effectively run in a "Disconnected" state, which is sufficient for development and testing.

## Proposed Changes

### 1. Build System (`CMakeLists.txt`)
*   Add logic to detect the operating system.
*   **Linux:**
    *   Find and link `glfw3` and `OpenGL`.
    *   Add ImGui backends: `imgui_impl_glfw.cpp`, `imgui_impl_opengl3.cpp`.
    *   Exclude Windows-specific sources (`GuiLayer_Win32.cpp`).
*   **Windows:**
    *   Keep existing links (`d3d11`, `dinput8`, `dxguid`).
    *   Add ImGui backends: `imgui_impl_win32.cpp`, `imgui_impl_dx11.cpp`.
    *   Exclude Linux-specific sources (`GuiLayer_Linux.cpp`).

### 2. GUI Layer Refactoring
Split `src/GuiLayer.cpp` into three files to separate backend logic from UI definition.

*   **`src/GuiLayer_Common.cpp`:**
    *   Contains `DrawTuningWindow`, `DrawDebugWindow`, `SetupGUIStyle`.
    *   Contains `SaveCompositeScreenshot` (logic adapted to use backend-agnostic pixel reading if possible, or `#ifdef`ed).
*   **`src/GuiLayer_Win32.cpp`:**
    *   Implements `GuiLayer::Init`, `GuiLayer::Render`, `GuiLayer::Shutdown` using **DirectX 11** and **Win32 API**.
    *   Contains `WndProc` and D3D globals.
*   **`src/GuiLayer_Linux.cpp`:**
    *   Implements `GuiLayer::Init`, `GuiLayer::Render`, `GuiLayer::Shutdown` using **GLFW** and **OpenGL 3**.
    *   Handles GLFW window creation and event polling.

### 3. Hardware Mocking (Linux Support)
Wrap Windows-specific code in `#ifdef _WIN32` blocks and provide dummy implementations for `#else`.

*   **`src/DirectInputFFB.cpp`:**
    *   **Linux:** `Initialize` returns true (mock success). `SelectDevice` returns true. `UpdateForce` does nothing.
*   **`src/GameConnector.cpp`:**
    *   **Linux:** `TryConnect` returns false. `CopyTelemetry` returns false.
*   **`src/DynamicVJoy.h`:**
    *   **Linux:** `Load` returns false. All other methods return safe defaults.
*   **`src/main.cpp`:**
    *   Remove/Guard `timeBeginPeriod(1)` (Windows only).
    *   Ensure `DirectInputFFB::Get().Initialize()` receives `nullptr` or appropriate handle on Linux.

### 4. Version Increment
*   Increment version in `src/Version.h` (e.g., `0.7.17` -> `0.7.18`).

## Parameter Synchronization Checklist
*   N/A - No new user settings are being added.

## Test Plan (TDD-Ready)

### 1. Compilation Verification (Linux)
*   **Test:** `Build Linux`
*   **Command:** `cmake -S . -B build && cmake --build build`
*   **Expectation:** Build succeeds without errors regarding missing `<windows.h>`, `<dinput.h>`, or DirectX headers.

### 2. GUI Launch Verification (Linux)
*   **Test:** `Launch App`
*   **Action:** Run `./LMUFFB` (or executable name).
*   **Expectation:**
    *   Window opens with title "lmuFFB v0.7.18".
    *   ImGui interface renders correctly (Dark theme).
    *   "Connecting to LMU..." status is visible (yellow).
    *   Sliders can be moved.
    *   Console logs "[DI] Mock Initialized (Non-Windows)".

### 3. Unit Test Verification
*   **Test:** `Run Unit Tests`
*   **Command:** `./build/tests/run_combined_tests`
*   **Expectation:** All physics and logic tests pass on Linux.

## Deliverables
*   [ ] Updated `CMakeLists.txt` with Linux/Windows conditional logic.
*   [ ] `src/GuiLayer_Common.cpp` (Refactored UI logic).
*   [ ] `src/GuiLayer_Win32.cpp` (DirectX 11 Backend).
*   [ ] `src/GuiLayer_Linux.cpp` (GLFW/OpenGL Backend).
*   [ ] Updated `src/DirectInputFFB.cpp` with Linux mocks.
*   [ ] Updated `src/GameConnector.cpp` with Linux mocks.
*   [ ] Updated `src/DynamicVJoy.h` with Linux mocks.
*   [ ] Updated `src/main.cpp` for cross-platform entry.
*   [ ] Updated `src/Version.h`.
*   [ ] **Implementation Notes:** Update plan with any library version conflicts or specific GLFW setup hurdles encountered.
```

```json
{
  "status": "success",
  "plan_path": "docs/dev_docs/plans/plan_linux_port_01.md",
  "backlog_items": []
}
```