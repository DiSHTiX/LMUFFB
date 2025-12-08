# LMUFFB - AI Developer Guide

This document provides the Standard Operating Procedures (SOP), context, and constraints for AI assistants (Jules) working on the LMUFFB C++ Force Feedback Driver.

---

## ✅ Standard Task Workflow (SOP)

**Perform these steps for EVERY task to ensure quality and consistency.**

### 1. 🔄 Sync (Direct to Main)
*   **Sync**: Run `git checkout main` and `git pull` to ensure you have the latest code.
*   **Policy**: You are authorized to work directly on the `main` branch.

### 2. 🧠 Consult Memory
*   **Action**: Read `AGENTS_MEMORY.md`.
*   **Why**: It contains build workarounds (Linux vs Windows) and architectural lessons.

### 3. 🧪 Test-Driven Development
*   **Requirement**: You **must** add or update C++ unit tests for every logic change.
*   **Verification**: You **must** compile and run the tests in the Linux VM.
    *   *Command*:
        ```bash
        mkdir -p build_tests && cd build_tests
        cmake ../tests
        cmake --build .
        ./run_tests
        ```
    *   **CRITICAL**: Do not commit or push code if `run_tests` fails. Since you are pushing to `main`, breaking the build is forbidden.

### 4. 📚 Documentation Updates
*   **Requirement**: Update relevant Markdown files (`FFB_formulas.md`, `README.md`, etc.) to reflect your changes.

### 5. 📦 Versioning & Changelog
*   **Update Version**: Increment the number in the `VERSION` file.
*   **Update Changelog**: Add a concise entry to `CHANGELOG.md`.

### 6. 📤 Delivery (Commit & Push)
*   **Commit**: Stage your changes (`git add .`) and commit with a descriptive message.
*   **Memory**: If you learned something new (e.g., a new build quirk), update `AGENTS_MEMORY.md` *before* committing.
*   **Push**: Push directly to the remote.
    *   Command: `git push origin main`

---

## 🌍 Environment & Constraints

*   **Target OS**: Windows 10/11.
*   **Jules Environment**: Ubuntu Linux.
*   **Build Limitation**: You **cannot** build the main application (`LMUFFB.exe`) in this environment.
    *   ❌ **DirectX 11** (`d3d11.h`) is missing on Linux.
    *   ❌ **DirectInput 8** (`dinput.h`) is missing on Linux.
    *   ❌ **Win32 API** (`windows.h`) is missing on Linux.
*   **Strategy**: You **can** build and run the **Unit Tests** (`tests/`).
    *   ✅ The Physics Engine (`FFBEngine.h`) is pure C++17 and platform-agnostic.
    *   ✅ The Test Suite mocks the Windows telemetry inputs.

---

## 🏗️ Architecture & Patterns

### 1. The Core Loop (400Hz)
*   **Component**: `FFBEngine` (Header-only: `FFBEngine.h`).
*   **Constraint**: Runs on a high-priority thread. **No memory allocation** (heap) allowed inside `calculate_force`.
*   **Math Rule (Critical)**: Use **Phase Accumulation** for vibrations.
    *   ❌ *Wrong*: `sin(time * frequency)` (Causes clicks when freq changes).
    *   ✅ *Right*: `phase += frequency * dt; output = sin(phase);`
*   **Safety**: All physics inputs involving `mTireLoad` must be clamped (e.g., `std::min(1.5, load_factor)`) to prevent hardware damage.

### 2. The GUI Loop (60Hz)
*   **Component**: `src/GuiLayer.cpp` (ImGui).
*   **Pattern**: **Producer-Consumer**.
    *   *Producer (FFB Thread)*: Pushes `FFBSnapshot` structs into `m_debug_buffer` every tick.
    *   *Consumer (GUI Thread)*: Calls `GetDebugBatch()` to swap the buffer and render *all* ticks since the last frame.
    *   *Constraint*: Never read `FFBEngine` state directly for plots; always use the snapshot batch to avoid aliasing.

### 3. Hardware Interface
*   **Component**: `src/DirectInputFFB.cpp`.
*   **Pattern**: Sends "Constant Force" updates.
*   **Optimization**: Includes a check `if (magnitude == m_last_force) return;` to minimize driver overhead.

---

## 📂 Key Documentation References

*   **Formulas**: `docs/dev_docs/FFB_formulas.md` (The math behind the code).
*   **Telemetry**: `docs/dev_docs/telemetry_data_reference.md` (Available inputs).
*   **Structs**: `rF2Data.h` (Memory layout - **Must match rFactor 2 plugin exactly**).

---

## 📝 Code Generation Guidelines

1.  **Adding New Effects**:
    *   Add a boolean toggle and gain float to `FFBEngine` class.
    *   Add a phase accumulator variable (`double m_effect_phase`) if it oscillates.
    *   Implement logic in `calculate_force`.
    *   Add UI controls in `GuiLayer::DrawTuningWindow`.
    *   Add visualization data to `FFBSnapshot` struct.

2.  **Modifying Config**:
    *   Update `src/Config.h` (declaration).
    *   Update `src/Config.cpp` (Save/Load logic).
    *   **Default to Safe**: New features should default to `false` or `0.0`.

3.  **Thread Safety**:
    *   Access to `FFBEngine` settings from the GUI thread must be protected by `std::lock_guard<std::mutex> lock(g_engine_mutex);`.

## 🚫 Common Pitfalls
*   **Do not** use `mElapsedTime` for sine waves (see Math Rule).
*   **Do not** remove the `vJoyInterface.dll` dynamic loading logic (the app must run even if vJoy is missing).
*   **Do not** change the struct packing in `rF2Data.h` (it breaks shared memory reading).