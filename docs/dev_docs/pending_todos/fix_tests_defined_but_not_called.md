In standard C++, there is **no built-in reflection mechanism** that allows a program to inspect itself and say "give me a list of all functions defined in this file."

However, there are three standard ways to solve this problem, ranging from "Compiler Checks" to "Architectural Patterns."

### Option 1: The Compiler Warning Method (Easiest)

If your tests are all in the same file as `main()` (which `tests/test_ffb_engine.cpp` is), you can rely on the compiler to tell you if a function is defined but not used.

**The Trick:** You must declare the test functions as `static`.
In C++, `static` on a global function means "this function is only visible in this file." If the compiler sees a `static` function that is never called, it knows it's dead code and will issue a warning.

**How to apply it:**

1.  Change your test definitions:
    ```cpp
    // Old
    void test_my_feature() { ... }

    // New
    static void test_my_feature() { ... }
    ```
2.  Compile.
    *   **MSVC (Windows):** Warning **C4505** ("unreferenced local function has been removed").
    *   **GCC/Clang (Linux):** Warning **-Wunused-function**.

**Pros:** Zero infrastructure code.
**Cons:** You have to manually add `static` to every test.

---

### Option 2: The Auto-Registration Pattern (Recommended)

This is how frameworks like **Google Test** and **Catch2** work. Instead of manually calling functions in `Run()`, you create a system where defining a test *automatically* adds it to a global registry.

Given the new **multi-file structure** of the test suite (v0.7.6+), this pattern requires a **Cross-Unit Singleton** to ensure all tests register to the same list regardless of which `.cpp` file they are in.

#### 1. Architecture

**In `tests/test_ffb_common.h` (The Interface):**
```cpp
#include <vector>
#include <functional>
#include <string>

namespace FFBEngineTests {

struct TestEntry {
    std::string name;
    std::string category; // Derived from filename or tag
    std::function<void()> func;
};

class TestRegistry {
public:
    static TestRegistry& Instance();
    void Register(const std::string& name, const std::string& category, std::function<void()> func);
    const std::vector<TestEntry>& GetTests() const;

private:
    std::vector<TestEntry> m_tests;
};

// Helper class for static registration
struct AutoRegister {
    AutoRegister(const std::string& name, const std::string& category, std::function<void()> func) {
        TestRegistry::Instance().Register(name, category, func);
    }
};

}

// Macro to define tests
// Usage: TEST_CASE(test_my_feature)
#define TEST_CASE(test_name) \
    void test_name(); \
    static FFBEngineTests::AutoRegister reg_##test_name(#test_name, __FILE__, test_name); \
    void test_name()
```

**In `tests/test_ffb_common.cpp` (The Implementation):**
```cpp
namespace FFBEngineTests {

TestRegistry& TestRegistry::Instance() {
    static TestRegistry instance; // Thread-safe singleton
    return instance;
}

void TestRegistry::Register(const std::string& name, const std::string& category, std::function<void()> func) {
    m_tests.push_back({name, category, func});
}

const std::vector<TestEntry>& TestRegistry::GetTests() const {
    return m_tests;
}

// Updated Run() Loop - replaces manual calls
void Run() {
    auto& tests = TestRegistry::Instance().GetTests();
    std::cout << "Running " << tests.size() << " registered tests..." << std::endl;

    for (const auto& test : tests) {
        // Integrate with existing Tag Filtering
        // Example: if (!ShouldRunTest(test.name, g_active_tags)) continue;

        std::cout << "Running: " << test.name << "..." << std::endl;
        try {
            test.func();
        } catch (const std::exception& e) {
            std::cout << "[FAIL] Exception: " << e.what() << std::endl;
            g_tests_failed++;
        }
    }
}

}
```

#### 2. Migration Steps

1.  **Define Infrastructure:** Add the code above to `common.h` and `common.cpp`.
2.  **Migrate Tests:** Replace `static void test_name() { ... }` with `TEST_CASE(test_name) { ... }` in all `test_ffb_*.cpp` files.
    *   *Tip:* Use Regex Replace: `static void (test_\w+)\(\)` → `TEST_CASE($1)`
3.  **Clean Up:** Delete all individual `Run_Category()` functions and their calls in `Run()`.

---

### Implementation Analysis (v0.7.x Split Suite)

Implementing this solution for the current codebase (13+ files, ~590 tests) involves the following considerations:

**Complexity: Low-Medium**
*   **Logic:** The registry code itself is minimal (< 50 lines).
*   **Integration:** Requires ensuring correct linking. Since all test files are explicitly listed in `CMakeLists.txt`, the linker will include them, and the auto-registration macros will execute automatically at startup.

**Time Consumption: 2-4 Hours**
*   **Infrastructure:** ~30 minutes to update `common` files.
*   **Migration:** ~2 hours to mechanically find-and-replace `TEST_CASE` across 590 tests.
*   **Verification:** ~1 hour to ensure all 590 tests still run and tags work.

**Risks: Low**
*   **Build Issues:** Minor risk of linker optimizations stripping "unused" objects. (Low risk with current CMake configuration).
*   **Dependency:** Creates a harder dependency on `common.h` macros, but simplifies test writing significantly.

---

### Option 3: External Script (The "Linter" Way)

If you don't want to change your C++ code structure, you can use a simple Python script to scan the file. This is often used in CI/CD pipelines.

**`scripts/check_tests.py`**:
```python
import re

with open("tests/test_ffb_engine.cpp", "r") as f:
    content = f.read()

# Find all functions starting with "void test_"
defined_tests = set(re.findall(r'void (test_\w+)\(\)', content))

# Find all calls inside main()
# This is a naive regex, but usually works for simple test files
called_tests = set(re.findall(r'(test_\w+)\(\);', content))

missing = defined_tests - called_tests

if missing:
    print("ERROR: The following tests are defined but NOT called in main:")
    for t in missing:
        print(f"  - {t}")
    exit(1)
else:
    print("All tests are called.")
    exit(0)
```

### Recommendation for LMUFFB

Since you are already refactoring `tests/test_ffb_engine.cpp` to add new tests:

1.  **Short Term:** Use **Option 1 (Static)**. Just add `static` to your test functions. It's the fastest way to spot the issue right now without rewriting the file structure.
2.  **Long Term:** Adopt **Option 2 (Auto-Registration)**. It prevents this bug from ever happening again and makes adding new tests cleaner (you just write the test and forget about it).
