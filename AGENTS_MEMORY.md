# AI Memory & Knowledge Base

**To the AI (Jules):**
This file represents your "Long-Term Memory". It contains lessons learned from previous sessions.
*   **READ** this before starting any task to avoid repeating past mistakes.
*   **WRITE** to this if you discover a new workaround, a build quirk, or an architectural insight that will help future instances of yourself.

---

## 🔧 Environment & Workflow

### Git Protocol
*   **Status**: Write access is enabled.
*   **Workflow**: Work directly on the `main` branch.
*   **Requirement**: Always `git pull` before starting. Always `git push origin main` after finishing.

### CMake & Build (Linux VM)
*   **Constraint**: This environment (Ubuntu Linux) **cannot** build the main `LMUFFB.exe` target because it lacks Windows headers (`d3d11.h`, `dinput.h`, `windows.h`).
*   **Solution**: Do not attempt to build the root `CMakeLists.txt`. Instead, **always** target the tests directory explicitly to verify logic:
    ```bash
    mkdir -p build_tests && cd build_tests
    cmake ../tests
    cmake --build .
    ./run_tests
    ```

---

## 🧠 Architectural Insights

### Telemetry Data
*   **Observation**: LMU telemetry is sometimes inconsistent.
*   **Strategy**: When adding new effects based on telemetry, always check if the value is non-zero in `tests/test_ffb_engine.cpp` before assuming it works in the game.

### Thread Safety
*   **Critical**: The GUI thread (Consumer) and FFB thread (Producer) share data.
*   **Rule**: Never access `FFBEngine` members directly from `GuiLayer` without locking `g_engine_mutex`, OR use the `GetDebugBatch()` snapshot system which handles locking internally.