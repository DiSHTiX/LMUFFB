# Windows Compilation Issue: std::max Macro Conflict

## Problem

When compiling C++ code on Windows with MSVC, calls to `std::max` and `std::min` can fail with cryptic errors like:

```
error C2589: '(': illegal token on right side of '::'
error C2062: type 'unknown-type' unexpected
error C2059: syntax error: ')'
```

## Root Cause

Windows headers (particularly `<windows.h>` and `<windef.h>`) define `max` and `min` as **preprocessor macros**:

```cpp
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
```

When you write `std::max(a, b)`, the preprocessor expands it to:
```cpp
std::(((a) > (b)) ? (a) : (b))(a, b)  // Invalid syntax!
```

This is a well-known Windows SDK issue that has existed for decades.

## Solution

Wrap `std::max` and `std::min` calls in **extra parentheses** to prevent macro expansion:

### ❌ Wrong (will fail on Windows):
```cpp
double result = std::max(a, b);
double result = std::min(a, b);
```

### ✅ Correct (works on all platforms):
```cpp
double result = (std::max)(a, b);
double result = (std::min)(a, b);
```

The extra parentheses prevent the preprocessor from recognizing the macro pattern.

## Alternative Solutions

1. **Define NOMINMAX** (before including Windows headers):
   ```cpp
   #define NOMINMAX
   #include <windows.h>
   ```
   This prevents the macros from being defined, but may break code that relies on them.

2. **Undefine the macros** (after including Windows headers):
   ```cpp
   #include <windows.h>
   #undef max
   #undef min
   ```

3. **Use the parentheses approach** (recommended):
   - Works everywhere
   - No side effects
   - No need to modify includes
   - Already used throughout `FFBEngine.h`

## Project Convention

**This project uses the parentheses approach** for consistency with existing code in `FFBEngine.h`. All calls to `std::max` and `std::min` should use:

```cpp
(std::max)(a, b)
(std::min)(a, b)
```

## References

- Microsoft Docs: [Avoiding Name Collisions](https://docs.microsoft.com/en-us/windows/win32/winprog/using-the-windows-headers#faster-builds-with-smaller-header-files)
- Stack Overflow: [Why does std::min/max not work?](https://stackoverflow.com/questions/5004356/why-does-stdmin-max-not-work-on-windows)

## History

- **v0.4.18**: Encountered in test code when adding yaw acceleration smoothing tests
- **Earlier versions**: Already resolved in main codebase (`FFBEngine.h`)
- **Lesson**: Always use `(std::max)` and `(std::min)` in this project
