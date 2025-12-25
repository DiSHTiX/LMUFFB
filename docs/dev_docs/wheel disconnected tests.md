### 1. Refactoring for Testability

**Modify `src/DirectInputFFB.h`**:
Add a static public helper function.
```cpp
static std::string FormatErrorWithAdvice(HRESULT hr);
```

**Modify `src/DirectInputFFB.cpp`**:
Implement the logic here.
```cpp
std::string DirectInputFFB::FormatErrorWithAdvice(HRESULT hr) {
    std::string errorMsg = GetDirectInputErrorString(hr); // The helper you added
    
    // Check for Priority/Exclusive errors (including raw hex)
    if (hr == DIERR_OTHERAPPHASPRIO || 
        hr == DIERR_NOTEXCLUSIVEACQUIRED || 
        hr == 0x80040205) {
        errorMsg += " [CRITICAL: Game has stolen priority! DISABLE IN-GAME FFB]";
    }
    
    return errorMsg;
}
```

### 2. The Test Case

**Modify `tests/test_windows_platform.cpp`**:
Add a test that checks specific error codes.

```cpp
static void test_directinput_error_formatting() {
    std::cout << "\nTest: DirectInput Error Formatting & Advice" << std::endl;

    // Case 1: Generic Error (Input Lost)
    // Should have standard text, NO custom advice
    std::string err1 = DirectInputFFB::FormatErrorWithAdvice(DIERR_INPUTLOST);
    bool has_standard = (err1.find("Access to the input device has been lost") != std::string::npos);
    bool has_advice = (err1.find("DISABLE IN-GAME FFB") != std::string::npos);
    
    if (has_standard && !has_advice) {
        std::cout << "[PASS] Generic error formatted correctly." << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Generic error format wrong. Got: " << err1 << std::endl;
        g_tests_failed++;
    }

    // Case 2: Priority Error (DIERR_OTHERAPPHASPRIO)
    // Should have custom advice
    std::string err2 = DirectInputFFB::FormatErrorWithAdvice(DIERR_OTHERAPPHASPRIO);
    if (err2.find("DISABLE IN-GAME FFB") != std::string::npos) {
        std::cout << "[PASS] Priority error contains custom advice." << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Priority error missing advice. Got: " << err2 << std::endl;
        g_tests_failed++;
    }

    // Case 3: The "Unknown" Hex Code (0x80040205)
    // Should be recognized and have custom advice
    std::string err3 = DirectInputFFB::FormatErrorWithAdvice(0x80040205);
    if (err3.find("DISABLE IN-GAME FFB") != std::string::npos) {
        std::cout << "[PASS] Hex code 0x80040205 recognized with advice." << std::endl;
        g_tests_passed++;
    } else {
        std::cout << "[FAIL] Hex code 0x80040205 failed. Got: " << err3 << std::endl;
        g_tests_failed++;
    }
}
```

### 3. Prompt for the Agent

Here is the updated prompt that includes the refactoring and the test.

***

Please proceed with the following task:

**Task: Refactor DirectInput Error Logic & Add Tests**

**Reference Documents:**
*   `src\DirectInputFFB.h`
*   `src\DirectInputFFB.cpp`
*   `tests\test_windows_platform.cpp`

**Context:**
We need to ensure that specific DirectInput error codes (specifically `0x80040205` / `DIERR_OTHERAPPHASPRIO`) trigger a specific warning message to the user. To verify this reliably, we need to extract the error formatting logic into a testable static function and write a unit test for it.

**Implementation Requirements:**

1.  **Update `src/DirectInputFFB.h`**:
    *   Add a public static method: `static std::string FormatErrorWithAdvice(HRESULT hr);`

2.  **Update `src/DirectInputFFB.cpp`**:
    *   Implement `FormatErrorWithAdvice`:
        *   Call `GetDirectInputErrorString(hr)` to get the base description.
        *   Check if `hr` matches `DIERR_OTHERAPPHASPRIO`, `DIERR_NOTEXCLUSIVEACQUIRED`, or `0x80040205`.
        *   If it matches, append: `" [CRITICAL: Game has stolen priority! DISABLE IN-GAME FFB]"`.
        *   Return the combined string.
    *   **Refactor `UpdateForce`**:
        *   Replace the manual error string construction logic with a call to `FormatErrorWithAdvice(hr)`.
        *   Use the returned string in the `std::cerr` log.

3.  **Update `tests/test_windows_platform.cpp`**:
    *   Add a new test function `test_directinput_error_formatting()`.
    *   **Test Case 1:** Verify `DIERR_INPUTLOST` returns standard text *without* the custom advice.
    *   **Test Case 2:** Verify `DIERR_OTHERAPPHASPRIO` returns text *with* the custom advice.
    *   **Test Case 3:** Verify raw hex `0x80040205` returns text *with* the custom advice.
    *   Call this test from `main()`.

**Deliverables:**
*   Modified `src/DirectInputFFB.h`
*   Modified `src/DirectInputFFB.cpp`
*   Modified `tests/test_windows_platform.cpp`

**Check-list for completion:**
- [ ] `DirectInputFFB.h`: `FormatErrorWithAdvice` declared.
- [ ] `DirectInputFFB.cpp`: `FormatErrorWithAdvice` implemented with hex check.
- [ ] `DirectInputFFB.cpp`: `UpdateForce` uses the new helper.
- [ ] `test_windows_platform.cpp`: New test passes for all 3 cases.