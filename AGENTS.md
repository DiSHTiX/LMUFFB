# LMUFFB - AI Developer Guide

This file provides SOP, build commands, code style, and patterns for AI agents working on LMUFFB (C++ Force Feedback Driver for Le Mans Ultimate).

---

## ✅ Standard Task Workflow (SOP)

**Perform these steps for EVERY task to ensure quality and consistency.**

### 1. 🧠 Consult Memory
*   **Action**: Query the hivemind memories.
*   **Why**: It contains workarounds (like Git fixes) and architectural lessons learned from previous sessions.
*   **When**: ALWAYS query memory BEFORE:
*      * Starting complex debugging
*      * Making architectural decisions
*      * Using unfamiliar tools/libraries
*      * Implementing cross-cutting features
---

### 2. 🔄 Context
*   **Context**: If you need to refresh your understanding of the full codebase, run `python scripts/create_context.py`.
*   **Read Updated Docs**: For each changed documentation file, read its current content to understand the updates.
*   **Why**: Documentation changes often reflect new features, API changes, architecture updates, or critical fixes. You must stay current with the project's evolving knowledge base.
*   **Priority Files**: Pay special attention to changes in:
*   `README.md` - User-facing features and setup
*   `CHANGELOG.md` - Recent changes and version history
*   `docs/dev_docs/telemetry_data_reference.md` - API source of truth
*   `docs/dev_docs/FFB_formulas.md` - Physics and scaling constants
*   `docs/architecture.md` - System design and components
*   `hivemind` memory tool - Catch up on learnings of Previous sessions

---

### 3. 🧪 Test-Driven Development
*   **Requirement**: You **must** add or update C++ unit tests for every logic change or new feature.
*   **Location**: Add test cases to files under `tests/`
*   **Verification**: You **must** compile and run the tests to prove your code works.
*   **Constraint**: Do not submit code if `run_tests` fails.
*   **Command (Linux)**:
        ```bash
        mkdir -p build_tests && cd build_tests
        cmake ../tests
        cmake --build .
        ./run_tests
        ```
*   **Command (Windows - PowerShell)**:
        ```powershell
        & 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1' -Arch amd64 -SkipAutomaticLocation; cl /EHsc /std:c++17 /I.. tests\test_ffb_engine.cpp src\Config.cpp /Fe:tests\test_ffb_engine.exe
        .\tests\test_ffb_engine.exe
        ```
---

### 4. 📚 Documentation Updates
*   **Requirement**: You **must** scan and update ALL relevant documentation to reflect your changes.
*   **Process**:
    1.  **Scan Documentation**: Use `find_by_name` to list all `.md` files in the project.
    2.  **Read Relevant Docs**: Review the documentation files that are likely affected by your changes.
    3.  **Determine Relevance**: Identify which documents need updates based on your changes.
    4.  **Update Documents**: Modify all relevant documentation to maintain consistency.
*   **Common Documentation Targets**:
    *   **Math/Physics Changes** → Update `docs/dev_docs/FFB_formulas.md`
    *   **New FFB Effects** → Update `docs/ffb_effects.md` AND `docs/the_physics_of__feel_-_driver_guide.md`
    *   **Telemetry Usage** → Update `docs/dev_docs/telemetry_data_reference.md`
    *   **GUI Changes** → Update `README.md` (text descriptions)
    *   **Architecture Changes** → Update `docs/architecture.md`
    *   **New Features** → Update `README.md`, `docs/introduction.md`, and relevant feature docs
    *   **Bug Fixes** → Consider updating `docs/dev_docs/TODO.md` to mark items as complete
    *   **LMU 1.2 Features** → Update `docs/dev_docs/new_ffb_features_enabled_by_lmu_1.2.md`
    *   **Configuration Changes** → Update `docs/ffb_customization.md`
*   **Documentation Directories**:
    *   `docs/` - User-facing documentation
    *   `docs/dev_docs/` - Developer and technical documentation
    *   `docs/bug_reports/` - Bug reports and troubleshooting
    *   Root `.md` files - `README.md`, `CHANGELOG.md`, `AGENTS.md`
*   **Critical**: Do NOT assume only one document needs updating. Your changes may affect multiple documents.

### 5. 📦 Versioning & Changelog
*   **Update Version**: Increment the number in the `VERSION` file (root directory).
    *   *Patch (0.0.X)*: Bug fixes, tweaks, refactoring.
    *   *Minor (0.X.0)*: New features, new effects.
    *   You must also update `src\Version.h`.
*   **Update Changelog**: Add a concise entry to `CHANGELOG.md` under the new version number.
*   **Constraint**: Do not submit code if `run_tests` fails.

### 6. 🧠 Update Memory (Critical)
*   **Action**: If you encountered a build error, a command failure, or learned something new about the code structure, append it to `AGENTS_MEMORY.md` and the `hivemind` tool.
*   **Goal**: Help the *next* AI session avoid the same problem.

### 7. 📤 Delivery
*   **Do Not Push**: Do not run any command that change git history (e.g. `git add`, `git commit`, `git push`) or that reset the local staged changes (e.g. `git reset`).

*   **MANDATORY CHECKLIST**:
    *   [ ] **Documentation Scanned**: Did you scan all `.md` files and identify relevant docs?
    *   [ ] **Documentation Updated**: Did you update ALL relevant documentation (not just one file)?
    *   [ ] **Version Bumped**: Did you increment the number in `VERSION`?
    *   [ ] **Changelog Updated**: Did you add a section in `CHANGELOG.md`?
    *   [ ] **Tests Passed**: Did you verify with `run_tests`?

---

## 🌍 Environment & Constraints

*   **Target OS**: Windows 10/11.
*   **Current Environment**: Arch Linux.
*   **Build Limitation**: You **cannot** build the main application (`LMUFFB.exe`) in this environment.
    *   ❌ **DirectX 11** (`d3d11.h`) is missing on Linux.
    *   ❌ **DirectInput 8** (`dinput.h`) is missing on Linux.
    *   ❌ **Win32 API** (`windows.h`) is missing on Linux.
*   **Strategy**: You **can** build and run the **Unit Tests** (`tests/`).
    *   ✅ The Physics Engine (`FFBEngine.h`) is pure C++17 and platform-agnostic.
    *   ✅ The Test Suite mocks the Windows telemetry inputs.
*   **Windows Build Command** (Full Application):
    *   If you need to verify the full application builds (GUI + FFB), use:
        ```powershell
        & 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1' -Arch amd64 -SkipAutomaticLocation; cmake --build build --config Release --clean-first
        ```
    *   This builds the complete `LMUFFB.exe` with all dependencies (ImGui, DirectInput, DirectX 11).

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
*   **Structs**: `src/lmu_sm_interface/*.hpp` (Memory layout - **Must match LMU plugin exactly**).

---

## 📝 Code Generation Guidelines

1.  **Formatting**
    *   Single-line comments use `//` style (not block comments)
    *   Do not delete/replace existing code but add a Single-line `//` comment at beginning of line
    *   Insert new code on the next newline

2.  **Adding New Effects**:
    *   Add a boolean toggle and gain float to `FFBEngine` class.
    *   Add a phase accumulator variable (`double m_effect_phase`) if it oscillates.
    *   Implement logic in `calculate_force`.
    *   Add UI controls in `GuiLayer::DrawTuningWindow`.
    *   Add visualization data to `FFBSnapshot` struct.

3.  **Modifying Config**:
    *   Update `src/Config.h` (declaration).
    *   Update `src/Config.cpp` (Save/Load logic).
    *   **Default to Safe**: New features should default to `false` or `0.0`.

4.  **Thread Safety**:
    *   Access to `FFBEngine` settings from the GUI thread must be protected by `std::lock_guard<std::mutex> lock(g_engine_mutex);`.

## Common Pitfalls

- Do NOT use `mElapsedTime` for sine wave calculations
- Do NOT allocate memory in the real-time FFB thread
- Do NOT remove vJoyInterface.dll dynamic loading (app must work without vJoy)
- Do NOT change struct packing in shared memory headers

---

## 10. Hivemind - Unified Memory System
The hive remembers everything. Learnings, sessions, patterns—all searchable.

**What to Store: ✅ Good memories:**
   * Root causes of tricky bugs (with context)
   * Architectural decisions (with reasoning and tradeoffs)
   * Domain-specific patterns (with examples)
   * Tool/library gotchas (with workarounds)
   * Failed approaches (to avoid repeating)
**What NOT to Store: ❌ Bad memories:**
   * Generic knowledge (already in docs)
   * Implementation details that change frequently
   * Vague descriptions ("fixed the thing")
   * Duplicate information

### Tools
| Tool | Purpose |
|------|---------|
| `hivemind_store` | Store a memory (learnings, decisions, patterns) |
| `hivemind_find` | Search all memories (semantic + FTS fallback) |
| `hivemind_get` | Get specific memory by ID |
| `hivemind_remove` | Delete outdated/incorrect memory |
| `hivemind_validate` | Confirm memory still accurate (resets 90-day decay) |
| `hivemind_stats` | Memory statistics and health |
| `hivemind_index` | Index AI session directories |
| `hivemind_sync` | Sync to .hive/memories.jsonl |

### Usage

# Store a learning (include WHY, not just WHAT)
`hivemind_store(information="OAuth refresh tokens need 5min buffer before expiry to avoid race conditions",tags="auth,oauth,tokens")`

# Search memories (includes past sessions)
`hivemind_find(query="token refresh", limit=5)`

# Search only Claude sessions
`hivemind_find(query="authentication error", collection="claude")`

# Index new sessions
`hivemind_index()`

# Sync to git
`hivemind_sync()`

