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

This is how frameworks like **Google Test** and **Catch2** work. Instead of manually calling functions in `main()`, you create a system where defining a test *automatically* adds it to a list.

You can implement this in about 20 lines of code in your `test_ffb_engine.cpp`.

**1. Add the Infrastructure (Top of file):**

```cpp
#include <vector>
#include <functional>

// A list to hold all registered tests
struct TestRegistry {
    using TestFunc = std::function<void()>;
    struct TestEntry {
        std::string name;
        TestFunc func;
    };
    
    static std::vector<TestEntry>& GetTests() {
        static std::vector<TestEntry> tests;
        return tests;
    }

    // Helper struct to register tests at startup
    struct Registrar {
        Registrar(const std::string& name, TestFunc func) {
            TestRegistry::GetTests().push_back({name, func});
        }
    };
};

// The Macro that makes it magic
#define TEST_CASE(name) \
    void name(); \
    static TestRegistry::Registrar reg_##name(#name, name); \
    void name()
```

**2. Define your tests using the Macro:**

Instead of `void test_name() { ... }`, you write:

```cpp
TEST_CASE(test_sanity_checks) {
    // ... your test code ...
    ASSERT_TRUE(true);
}

TEST_CASE(test_rear_force_workaround) {
    // ... your test code ...
}
```

**3. Update `main()` to run the list:**

You no longer need to manually call functions. `main` becomes generic:

```cpp
int main() {
    std::cout << "Running " << TestRegistry::GetTests().size() << " tests...\n";

    for (const auto& test : TestRegistry::GetTests()) {
        std::cout << "Running: " << test.name << "..." << std::endl;
        try {
            test.func();
        } catch (const std::exception& e) {
            std::cout << "[FAIL] Exception in " << test.name << ": " << e.what() << std::endl;
            g_tests_failed++;
        }
    }

    std::cout << "\n----------------" << std::endl;
    std::cout << "Tests Passed: " << g_tests_passed << std::endl;
    std::cout << "Tests Failed: " << g_tests_failed << std::endl;

    return g_tests_failed > 0 ? 1 : 0;
}
```

**Why this works:**
The macro creates a global `static` variable (`reg_##name`). In C++, global variables are initialized *before* `main()` starts. The constructor of that variable pushes the function pointer into the vector. By the time `main()` runs, the vector is already full of all your tests.

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
