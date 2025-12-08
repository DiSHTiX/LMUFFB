
This project is a C++ Force Feedback driver. 

## 🛠️ Developer Tools

These are the executable tools and scripts within the repository that Jules (or a developer) can use to build, test, and maintain the project.

### 1. Context Generator (`scripts/create_context.py`)
*   **Description**: A Python script that aggregates all source code (`.cpp`, `.h`), documentation (`.md`), and build files into a single Markdown file.
*   **Purpose**: Use this to refresh the AI's context window with the latest codebase state.
*   **Input**: The repository root directory.
*   **Output**: `docs/dev_docs/FULL_PROJECT_CONTEXT.md`.
*   **Command**: `python scripts/create_context.py`

### 2. Test Suite (`tests/test_ffb_engine.cpp`)
*   **Description**: A standalone C++ unit test suite for the `FFBEngine`.
*   **Purpose**: Verifies physics calculations (Phase integration, Oversteer logic, Safety clamps) without running the full GUI or game.
*   **Build & Run**:
    ```powershell
    # Compile
    cl /EHsc /std:c++17 /I.. tests\test_ffb_engine.cpp /Fe:test_ffb_engine.exe
    # Run
    tests\test_ffb_engine.exe
    ```
*   **Convention**: When modifying `FFBEngine.h`, always run this tool to ensure no regressions in physics logic.

### 3. Build System (CMake / MSVC)
*   **Description**: The project uses CMake to generate MSVC solution files.
*   **Purpose**: Compiles the main application `LMUFFB.exe`.
*   **Key Commands**:
    *   *Configure*: `cmake -B build`
    *   *Build*: `cmake --build build --config Release`
*   **Dependencies**: Requires `vJoyInterface.lib` (linked) and `DirectX` headers.

---

## 🏗️ Architectural Components (Context for Code Generation)

These are the primary C++ modules. Jules should treat these as the **Subject** of code generation, adhering to their specific constraints.

### 1. Physics Core (`FFBEngine`)
*   **Type**: Header-only Class (`FFBEngine.h`).
*   **Constraint**: Must run inside a 400Hz loop. **Zero memory allocation** allowed in `calculate_force`.
*   **Pattern**: Uses **Phase Accumulation** (`phase += freq * dt`) for all vibration effects. Do not use absolute time.
*   **Data Flow**: Reads `rF2Telemetry` -> Calculates `double` Force -> Pushes `FFBSnapshot` to GUI.

### 2. Hardware Interface (`DirectInputFFB`)
*   **Type**: Singleton Class (`src/DirectInputFFB.cpp`).
*   **Constraint**: Manages `IDirectInputDevice8`.
*   **Pattern**: Sends "Constant Force" updates. Includes logic to skip updates if the force value hasn't changed (optimization).

### 3. Telemetry Interface (`rF2Data.h`)
*   **Type**: Data Structure Definition.
*   **Constraint**: Must strictly match the memory layout of the *rFactor 2 Shared Memory Map Plugin*.
*   **Key Fields**:
    *   `mSteeringArmForce`: Primary FFB input.
    *   `mGripFract`: Used for Understeer effect.
    *   `mLateralPatchVel`: Used for Slide Texture frequency.

---

## 📝 Conventions & Guidelines

### Threading Model
*   **FFB Thread**: High priority, 400Hz. Handles Physics (`FFBEngine`) and Hardware Output (`DirectInput`). **No GUI calls allowed.**
*   **Main Thread**: Low priority, 60Hz. Handles GUI (`ImGui`) and Input processing.
*   **Synchronization**: Access to shared data (like `m_gain` or `m_debug_buffer`) must be protected by `g_engine_mutex`.

### Safety Mechanisms
*   **vJoy Guard**: The app includes a "Safety Switch" (`Config::m_output_ffb_to_vjoy`). By default, writing to vJoy is **disabled** to prevent feedback loops if the user misconfigures the game.
*   **Load Clamping**: Physics calculations involving `mTireLoad` must be clamped (e.g., `std::min(1.5, load_factor)`) to prevent hardware damage during physics glitches.