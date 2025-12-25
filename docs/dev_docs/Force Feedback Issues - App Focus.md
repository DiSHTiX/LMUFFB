## Introduction 

The issue is caused by a conflict over **Exclusive Access** to the DirectInput device.
*   **The Root Cause:** DirectInput requires **Exclusive Access** to send Force Feedback commands. If Le Mans Ultimate (the game) acquires the wheel in Exclusive Mode (which it does by default when focused), your app is kicked out or demoted to "Shared Mode" (Non-Exclusive). In Shared Mode, you can read buttons/steering, but **you cannot send FFB**, resulting in the `DIERR_NOTEXCLUSIVEACQUIRED` (0x80040205) error.
*   **The Symptom:** When the user Alt-Tabs to LMUFFB, LMU loses focus and releases the wheel. LMUFFB grabs it (Exclusive) and FFB works. When the user clicks back into the game, LMU grabs the wheel (Exclusive), LMUFFB loses it, and FFB stops.
*   **The Fix:**
    1.  **Code:** Implement "Dynamic Promotion" in `UpdateForce`. If we detect we are in Shared Mode (Error 0x80040205), we must aggressively try to re-acquire Exclusive access every frame until we get it.
    2.  **User:** The user **MUST** disable FFB in LMU (set "FFB Strength" to "0" or "Off") to prevent the game from demanding Exclusive access.



Here is the detailed explanation of what **"Dynamic Promotion"** means in this context, why it is needed, and how it interacts with the "Every 2 Seconds" rule.

### 1. The Context: DirectInput Permissions

In Windows DirectInput, there are two main ways an application can connect to a joystick/wheel:

1.  **Non-Exclusive (Shared) Mode:**
    *   **Capabilities:** Can **Read** inputs (Steering, Pedals, Buttons).
    *   **Limitations:** Cannot **Write** Force Feedback (FFB) effects.
    *   **Behavior:** Multiple apps can be in Shared Mode at the same time.

2.  **Exclusive Mode:**
    *   **Capabilities:** Can **Read** inputs AND **Write** FFB effects.
    *   **Limitations:** **Only ONE application** can hold Exclusive access at a time.
    *   **Behavior:** If App A has Exclusive access, and App B requests Exclusive access, App A usually loses it (is "kicked out").

### 2. The Problem: The "Demotion"

1.  **Start:** You launch LMUFFB. It successfully grabs **Exclusive** access. FFB works.
2.  **Conflict:** You launch Le Mans Ultimate (LMU). The game window becomes active. The game tries to initialize its own FFB.
3.  **Demotion:** Windows/DirectInput sees the Game is the "Foreground" window and gives it priority. LMUFFB is silently **demoted** to **Shared Mode** (or loses the device entirely).
4.  **The Error:** LMUFFB tries to send an FFB command (`SetParameters`). DirectInput rejects it with error `0x80040205` (`DIERR_NOTEXCLUSIVEACQUIRED`), which translates to: *"You are connected, but you don't have permission to send forces."*

### 3. The Solution: "Dynamic Promotion"

"Dynamic Promotion" is the logic implemented in `DirectInputFFB.cpp` to handle this specific error. Instead of just logging "Error" and stopping, the code actively tries to upgrade its status back to Exclusive.

**The Logic Flow:**

1.  **Detect:** The app catches error `0x80040205` inside `UpdateForce`.
2.  **Reset:** It calls `Unacquire()` to fully let go of the "Shared" connection.
3.  **Promote:** It calls `SetCooperativeLevel` with `DISCL_EXCLUSIVE | DISCL_BACKGROUND`. This is the request to Windows: *"I want to be the Boss (Exclusive) again."*
4.  **Re-Acquire:** It calls `Acquire()`.

If successful, LMUFFB steals the Exclusive lock back from the game (or grabs it if the game released it).

### 4. Clarification: "Every Frame" vs. "Every 2 Seconds"

In my initial analysis, I said "aggressively try... every frame." **This was a theoretical description of the urgency, but practically, we implemented the 2-second throttle.**

Here is why the **2-second throttle** is the correct implementation of this concept:

*   **The "Tug of War" Risk:** If LMUFFB tries to grab Exclusive access **every frame** (400 times a second), and the Game *also* tries to grab it (because it's in focus), the device will flip-flop between them milliseconds apart. This causes the FFB to stutter violently or the driver to crash.
*   **The Throttled Approach:**
    1.  LMUFFB detects it lost Exclusive access.
    2.  It waits **2 seconds**.
    3.  It tries **once** to Promote itself back to Exclusive.
    4.  If it fails (because the Game is fighting back), it waits another 2 seconds.

### Summary

**Dynamic Promotion** is the code's ability to realize: *"I am currently a second-class citizen (Shared), but I need to be the Boss (Exclusive) to do my job. I will attempt to upgrade my permissions right now."*

This is critical because without this logic, once LMUFFB is demoted to Shared mode (e.g., by Alt-Tabbing), it would stay in Shared mode forever, and FFB would never come back until you restarted the app.


## The Difference: "Acquire" vs. "Promote"

You are absolutely correct. If we stick to the **2-second throttle**, the "Auto Rebind" checkbox is likely unnecessary clutter. The 2-second delay prevents the "resource slowdown" and "stuttering" risks that an every-frame attempt would cause.

### The Difference: "Acquire" vs. "Promote"

You asked: *"I don't understand what is different from the current implementation."*

The difference is **HOW** we reconnect.

1.  **Current Implementation (The Loop of Death):**
    *   **Scenario:** You Alt-Tab. Windows demotes LMUFFB to **Shared Mode**.
    *   **Error:** `UpdateForce` fails with `DIERR_NOTEXCLUSIVEACQUIRED`.
    *   **Current Recovery:** We wait 2 seconds and call `m_pDevice->Acquire()`.
    *   **The Problem:** Since the device is already set to "Shared" (from the demotion), `Acquire()` succeeds immediately... **but it keeps you in Shared Mode**.
    *   **Result:** The next frame, `UpdateForce` fails *again* because you are still Shared. You are trapped in a loop: *Fail -> Wait -> Acquire (Shared) -> Fail*.

2.  **Proposed Fix (Dynamic Promotion):**
    *   **Scenario:** You Alt-Tab. LMUFFB is demoted to **Shared Mode**.
    *   **Error:** `UpdateForce` fails with `DIERR_NOTEXCLUSIVEACQUIRED`.
    *   **New Recovery:** We wait 2 seconds.
    *   **The Fix:** We explicitly say: *"I don't just want to acquire. I want to **change the rules**."*
        ```cpp
        m_pDevice->Unacquire(); // Let go completely
        m_pDevice->SetCooperativeLevel(..., DISCL_EXCLUSIVE | ...); // DEMAND Exclusive
        m_pDevice->Acquire(); // Grab it
        ```
    *   **Result:** This forces Windows to either give us Exclusive access (fixing the FFB) or fail hard (if the game blocks us). It breaks the "Shared Mode" loop.

### Implementation Plan (No Checkbox)

Since we agree the checkbox is not needed, here is the code to simply **fix the logic** within the existing 2-second timer.

**File:** `src/DirectInputFFB.cpp`
**Function:** `UpdateForce`

Find the `// 3. Attempt Recovery` block and replace it with this:

```cpp
            // 3. Attempt Recovery (with Smart Cool-down)
            if (recoverable) {
                // Throttle recovery attempts to prevent CPU spam when device is locked
                static DWORD lastRecoveryAttempt = 0;
                DWORD now = GetTickCount();
                
                // Only attempt recovery if cooldown period has elapsed (2 Seconds)
                if (now - lastRecoveryAttempt > RECOVERY_COOLDOWN_MS) {
                    lastRecoveryAttempt = now; 
                    
                    // --- DYNAMIC PROMOTION FIX ---
                    // If we are stuck in "Shared Mode" (0x80040205), standard Acquire() 
                    // just re-confirms Shared Mode. We must force a mode switch.
                    if (hr == DIERR_NOTEXCLUSIVEACQUIRED) {
                        std::cout << "[DI] Attempting to promote to Exclusive Mode..." << std::endl;
                        m_pDevice->Unacquire();
                        m_pDevice->SetCooperativeLevel(m_hwnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND);
                    }
                    // -----------------------------

                    HRESULT hrAcq = m_pDevice->Acquire();
                    
                    if (SUCCEEDED(hrAcq)) {
                        // Log recovery success
                        std::cout << "[DI RECOVERY] Device re-acquired successfully. FFB motor restarted." << std::endl;
                        
                        // Update our internal state if we fixed the exclusivity
                        if (hr == DIERR_NOTEXCLUSIVEACQUIRED) {
                             m_isExclusive = true; 
                        }

                        // CRITICAL: Restart the effect motor
                        m_pEffect->Start(1, 0); 
                        
                        // Retry the update immediately so the user feels it instantly
                        m_pEffect->SetParameters(&eff, DIEP_TYPESPECIFICPARAMS);
                    }
                }
            }
```

### Summary
*   **No GUI changes needed.**
*   **No Config changes needed.**
*   This simply makes the existing "Reconnect" logic smarter so it can actually recover from an Alt-Tab event instead of getting stuck in Shared Mode.

## Make the "Smart Recovery" logic optional via an "Auto Rebind" checkbox.

Here is the report and implementation plan to make the "Smart Recovery" logic optional via an "Auto Rebind" checkbox.

### Analysis
*   **Current Behavior:** The app attempts to re-acquire the DirectInput device every 2 seconds (`RECOVERY_COOLDOWN_MS`) if a "Lost" or "Priority" error occurs.
*   **User Concern:** Even with the 2-second throttle, repeated acquisition attempts can cause micro-stutters in some drivers or interfere with the game if the user intends to let the game handle FFB temporarily.
*   **Solution:** Wrap the recovery logic in a conditional check controlled by a new `m_auto_rebind` configuration variable.

### Implementation Plan

1.  **Configuration**: Add `m_auto_rebind` to the `Config` class to persist the setting.
2.  **GUI**: Add the checkbox to the "Core Settings" line in `GuiLayer.cpp`.
3.  **Logic**: Update `DirectInputFFB.cpp` to check this flag before attempting recovery.

---

### 1. Update `src/Config.h`
Add the static boolean variable.

```cpp
class Config {
public:
    // ... existing members ...
    
    // NEW: Toggle for automatic device recovery
    static bool m_auto_rebind; 
};
```

### 2. Update `src/Config.cpp`
Initialize, Save, and Load the new setting. **Default to true** (recommended behavior), but allow users to disable it.

```cpp
// Initialize
bool Config::m_auto_rebind = true; // Default to enabled

void Config::Save(const FFBEngine& engine, const std::string& filename) {
    // ... inside file open check ...
        file << "auto_rebind=" << m_auto_rebind << "\n"; // NEW
    // ...
}

void Config::Load(FFBEngine& engine, const std::string& filename) {
    // ... inside loop ...
        else if (key == "auto_rebind") m_auto_rebind = std::stoi(value); // NEW
    // ...
}
```

### 3. Update `src/GuiLayer.cpp`
Add the checkbox on the same line as the "Unbind" button.

```cpp
// Inside DrawTuningWindow...

    ImGui::SameLine();
    if (ImGui::Button("Unbind")) {
        DirectInputFFB::Get().ReleaseDevice();
        selected_device_idx = -1;
    }

    // NEW: Auto Rebind Checkbox
    ImGui::SameLine();
    // Use a slightly darker color for the text to make it distinct from the buttons
    ImGui::Checkbox("Auto Rebind", &Config::m_auto_rebind);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("If checked, the app will automatically try to reconnect\n"
                          "if the device is lost or if the game steals focus.\n"
                          "Disable this if you experience stuttering.");
    }

    // Acquisition Mode & Troubleshooting (Existing code follows...)
    if (DirectInputFFB::Get().IsActive()) {
        // ...
```

### 4. Update `src/DirectInputFFB.cpp`
Include `Config.h` and wrap the recovery logic.

```cpp
// Add include at the top
#include "Config.h" 

// ... inside UpdateForce ...

            // 3. Attempt Recovery (with Smart Cool-down)
            // NEW: Check Config::m_auto_rebind before proceeding
            if (recoverable && Config::m_auto_rebind) {
                
                // Throttle recovery attempts to prevent CPU spam when device is locked
                static DWORD lastRecoveryAttempt = 0;
                DWORD now = GetTickCount();
                
                // Only attempt recovery if cooldown period has elapsed
                if (now - lastRecoveryAttempt > RECOVERY_COOLDOWN_MS) {
                    // ... [Existing Recovery Logic] ...
                }
            }
```

## How often to reaquire?

**Recommendation: Every 2 Seconds (2000ms)**

You should **definitely NOT** try to reacquire every frame.

### Why "Every Frame" is dangerous:
1.  **Performance Cost:** The DirectInput `Acquire()` function is not a simple variable check. It involves communicating with the Windows hardware driver and the COM interface. Calling this 400 times per second (400Hz) when it is failing will consume significant CPU cycles, likely causing the game itself to stutter.
2.  **"Tug of War":** If the game is also trying to acquire the device (which it does), trying to grab it back 400 times a second can create a race condition where the device flips between the Game and the App rapidly. This results in the FFB pulsing on and off violently or the driver crashing.

### Why 2 Seconds is the "Sweet Spot":
*   **Safety:** It ensures that if a conflict occurs, the system has time to settle before we try again.
*   **Resource Usage:** Checking once every 2 seconds has effectively **zero impact** on CPU performance.
*   **User Experience:** If a user Alt-Tabs back into the game, waiting 2 seconds for the FFB to kick in is noticeable but acceptable. It prevents the "stutter" that would ruin the race.

### How to tune it:
If you find 2 seconds feels too sluggish (e.g., you want FFB back faster after Alt-Tabbing), you can lower it to **1000ms (1 second)**, but I would not recommend going lower than that.

In `src/DirectInputFFB.cpp`:

```cpp
namespace {
    constexpr DWORD DIAGNOSTIC_LOG_INTERVAL_MS = 1000; 
    constexpr DWORD RECOVERY_COOLDOWN_MS = 2000; // <--- Change this to 1000 if you want faster recovery
}
```


## Implementation plan
Here are the updated files to implement the fix and better diagnostics.

### 1. `src/DirectInputFFB.h`
Added `m_isExclusive` flag and `IsExclusive()` getter to track the state.

```cpp
#ifndef DIRECTINPUTFFB_H
#define DIRECTINPUTFFB_H

#include <vector>
#include <string>
#include <atomic>

#ifdef _WIN32
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#else
// Mock types for non-Windows build/test
typedef void* HWND;
typedef void* LPDIRECTINPUT8;
typedef void* LPDIRECTINPUTDEVICE8;
typedef void* LPDIRECTINPUTEFFECT;
struct GUID { unsigned long Data1; unsigned short Data2; unsigned short Data3; unsigned char Data4[8]; };
#endif

struct DeviceInfo {
    GUID guid;
    std::string name;
};

class DirectInputFFB {
public:
    static DirectInputFFB& Get();

    bool Initialize(HWND hwnd);
    void Shutdown();

    // Returns a list of FFB-capable devices
    std::vector<DeviceInfo> EnumerateDevices();

    // Select and Acquire a device
    bool SelectDevice(const GUID& guid);
    
    // Release the currently acquired device (User unbind)
    void ReleaseDevice();

    // Update the Constant Force effect (-1.0 to 1.0)
    void UpdateForce(double normalizedForce);

    // NEW: Helpers for Config persistence
    static std::string GuidToString(const GUID& guid);
    static GUID StringToGuid(const std::string& str);
    static std::string GetActiveWindowTitle();

    bool IsActive() const { return m_active; }
    std::string GetCurrentDeviceName() const { return m_deviceName; }
    
    // Check if device was acquired in exclusive mode
    bool IsExclusive() const { return m_isExclusive; }

private:
    DirectInputFFB();
    ~DirectInputFFB();

    LPDIRECTINPUT8 m_pDI = nullptr;
    LPDIRECTINPUTDEVICE8 m_pDevice = nullptr;
    LPDIRECTINPUTEFFECT m_pEffect = nullptr;
    HWND m_hwnd = nullptr;
    
    bool m_active = false;
    bool m_isExclusive = false; // Track acquisition mode
    std::string m_deviceName = "None";
    
    // Internal helper to create the Constant Force effect
    bool CreateEffect();

    long m_last_force = -999999; 
};

#endif // DIRECTINPUTFFB_H
```

### 2. `src/DirectInputFFB.cpp`
Implemented the **Smart Recovery Logic**. If `DIERR_NOTEXCLUSIVEACQUIRED` is hit, it attempts to upgrade the cooperative level to Exclusive on the fly.

```cpp
#include "DirectInputFFB.h"

// Standard Library Headers
#include <iostream>
#include <cmath>
#include <cstdio> // For sscanf, sprintf
#include <algorithm> // For std::max, std::min

// Platform-Specific Headers
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#include <dinput.h>
#include <iomanip> // For std::hex
#include <string>
#endif

// Constants
namespace {
    constexpr DWORD DIAGNOSTIC_LOG_INTERVAL_MS = 1000; // Rate limit diagnostic logging to 1 second
    constexpr DWORD RECOVERY_COOLDOWN_MS = 2000;       // Wait 2 seconds between recovery attempts
}

// Keep existing implementations
DirectInputFFB& DirectInputFFB::Get() {
    static DirectInputFFB instance;
    return instance;
}

DirectInputFFB::DirectInputFFB() {}

// NEW: Helper to get foreground window title for diagnostics
std::string DirectInputFFB::GetActiveWindowTitle() {
#ifdef _WIN32
    char wnd_title[256];
    HWND hwnd = GetForegroundWindow();
    if (hwnd) {
        GetWindowTextA(hwnd, wnd_title, sizeof(wnd_title));
        return std::string(wnd_title);
    }
#endif
    return "Unknown";
}

// NEW: Helper Implementations for GUID
std::string DirectInputFFB::GuidToString(const GUID& guid) {
    char buf[64];
    sprintf_s(buf, "{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
        guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    return std::string(buf);
}

GUID DirectInputFFB::StringToGuid(const std::string& str) {
    GUID guid = { 0 };
    if (str.empty()) return guid;
    unsigned long p0;
    unsigned short p1, p2;
    unsigned int p3, p4, p5, p6, p7, p8, p9, p10;
    int n = sscanf_s(str.c_str(), "{%08lX-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}",
        &p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10);
    if (n == 11) {
        guid.Data1 = p0;
        guid.Data2 = (unsigned short)p1;
        guid.Data3 = (unsigned short)p2;
        guid.Data4[0] = (unsigned char)p3; guid.Data4[1] = (unsigned char)p4;
        guid.Data4[2] = (unsigned char)p5; guid.Data4[3] = (unsigned char)p6;
        guid.Data4[4] = (unsigned char)p7; guid.Data4[5] = (unsigned char)p8;
        guid.Data4[6] = (unsigned char)p9; guid.Data4[7] = (unsigned char)p10;
    }
    return guid;
}



/**
 * @brief Returns the description for a DirectInput return code.
 * 
 * Parsed from: https://learn.microsoft.com/en-us/previous-versions/windows/desktop/ee416869(v=vs.85)#constants
 * 
 * @param hr The HRESULT returned by a DirectInput method.
 * @return const char* The description of the error or status code.
 */
const char* GetDirectInputErrorString(HRESULT hr) {
    switch (hr) {
        // Success Codes
        case S_OK: // Also DI_OK
            return "The operation completed successfully (S_OK).";
        case S_FALSE: // Also DI_BUFFEROVERFLOW, DI_NOEFFECT, DI_NOTATTACHED, DI_PROPNOEFFECT
            return "Operation technically succeeded but had no effect or hit a warning (S_FALSE). The device buffer overflowed and some input was lost. This value is equal to DI_BUFFEROVERFLOW, DI_NOEFFECT, DI_NOTATTACHED, DI_PROPNOEFFECT.";
        case DI_DOWNLOADSKIPPED:
            return "The parameters of the effect were successfully updated, but the effect could not be downloaded because the associated device was not acquired in exclusive mode.";
        case DI_EFFECTRESTARTED:
            return "The effect was stopped, the parameters were updated, and the effect was restarted.";
        case DI_POLLEDDEVICE:
            return "The device is a polled device.. As a result, device buffering does not collect any data and event notifications is not signaled until the IDirectInputDevice8 Interface method is called.";
        case DI_SETTINGSNOTSAVED:
            return "The action map was applied to the device, but the settings could not be saved.";
        case DI_TRUNCATED:
            return "The parameters of the effect were successfully updated, but some of them were beyond the capabilities of the device and were truncated to the nearest supported value.";
        case DI_TRUNCATEDANDRESTARTED:
            return "Equal to DI_EFFECTRESTARTED | DI_TRUNCATED.";
        case DI_WRITEPROTECT:
            return "A SUCCESS code indicating that settings cannot be modified.";

        // Error Codes
        case DIERR_ACQUIRED:
            return "The operation cannot be performed while the device is acquired.";
        case DIERR_ALREADYINITIALIZED:
            return "This object is already initialized.";
        case DIERR_BADDRIVERVER:
            return "The object could not be created due to an incompatible driver version or mismatched or incomplete driver components.";
        case DIERR_BETADIRECTINPUTVERSION:
            return "The application was written for an unsupported prerelease version of DirectInput.";
        case DIERR_DEVICEFULL:
            return "The device is full.";
        case DIERR_DEVICENOTREG: // Equal to REGDB_E_CLASSNOTREG
            return "The device or device instance is not registered with DirectInput.";
        case DIERR_EFFECTPLAYING:
            return "The parameters were updated in memory but were not downloaded to the device because the device does not support updating an effect while it is still playing.";
        case DIERR_GENERIC: // Equal to E_FAIL
            return "An undetermined error occurred inside the DirectInput subsystem.";
        case DIERR_HANDLEEXISTS: // Equal to E_ACCESSDENIED
            return "Access denied or handle already exists. Another application may have exclusive access.";
        case DIERR_HASEFFECTS:
            return "The device cannot be reinitialized because effects are attached to it.";
        case DIERR_INCOMPLETEEFFECT:
            return "The effect could not be downloaded because essential information is missing. For example, no axes have been associated with the effect, or no type-specific information has been supplied.";
        case DIERR_INPUTLOST:
            return "Access to the input device has been lost. It must be reacquired.";
        case DIERR_INVALIDPARAM: // Equal to E_INVALIDARG
            return "An invalid parameter was passed to the returning function, or the object was not in a state that permitted the function to be called.";
        case DIERR_MAPFILEFAIL:
            return "An error has occurred either reading the vendor-supplied action-mapping file for the device or reading or writing the user configuration mapping file for the device.";
        case DIERR_MOREDATA:
            return "Not all the requested information fit into the buffer.";
        case DIERR_NOAGGREGATION:
            return "This object does not support aggregation.";
        case DIERR_NOINTERFACE: // Equal to E_NOINTERFACE
            return "The object does not support the specified interface.";
        case DIERR_NOTACQUIRED:
            return "The operation cannot be performed unless the device is acquired.";
        case DIERR_NOTBUFFERED:
            return "The device is not buffered. Set the DIPROP_BUFFERSIZE property to enable buffering.";
        case DIERR_NOTDOWNLOADED:
            return "The effect is not downloaded.";
        case DIERR_NOTEXCLUSIVEACQUIRED:
            return "The operation cannot be performed unless the device is acquired in DISCL_EXCLUSIVE mode.";
        case DIERR_NOTFOUND:
            return "The requested object does not exist (DIERR_NOTFOUND).";
        // case DIERR_OBJECTNOTFOUND: // Duplicate of DIERR_NOTFOUND
        //    return "The requested object does not exist.";
        case DIERR_OLDDIRECTINPUTVERSION:
            return "The application requires a newer version of DirectInput.";
        // case DIERR_OTHERAPPHASPRIO: // Duplicate of DIERR_HANDLEEXISTS (E_ACCESSDENIED)
        //    return "Another application has a higher priority level, preventing this call from succeeding.";
        case DIERR_OUTOFMEMORY: // Equal to E_OUTOFMEMORY
            return "The DirectInput subsystem could not allocate sufficient memory to complete the call.";
        // case DIERR_READONLY: // Duplicate of DIERR_HANDLEEXISTS (E_ACCESSDENIED)
        //    return "The specified property cannot be changed.";
        case DIERR_REPORTFULL:
            return "More information was requested to be sent than can be sent to the device.";
        case DIERR_UNPLUGGED:
            return "The operation could not be completed because the device is not plugged in.";
        case DIERR_UNSUPPORTED: // Equal to E_NOTIMPL
            return "The function called is not supported at this time.";
        case E_HANDLE:
            return "The HWND parameter is not a valid top-level window that belongs to the process.";
        case E_PENDING:
            return "Data is not yet available.";
        case E_POINTER:
            return "An invalid pointer, usually NULL, was passed as a parameter.";
        
        default:
            return "Unknown DirectInput Error";
    }
}

DirectInputFFB::~DirectInputFFB() {
    Shutdown();
}

bool DirectInputFFB::Initialize(HWND hwnd) {
    m_hwnd = hwnd;
#ifdef _WIN32
    if (FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDI, NULL))) {
        std::cerr << "[DI] Failed to create DirectInput8 interface." << std::endl;
        return false;
    }
    std::cout << "[DI] Initialized." << std::endl;
    return true;
#else
    std::cout << "[DI] Mock Initialized (Non-Windows)." << std::endl;
    return true;
#endif
}

void DirectInputFFB::Shutdown() {
    ReleaseDevice(); // Reuse logic
    if (m_pDI) {
        #ifdef _WIN32
        m_pDI->Release();
        m_pDI = nullptr;
        #endif
    }
}

#ifdef _WIN32
BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) {
    auto* devices = (std::vector<DeviceInfo>*)pContext;
    DeviceInfo info;
    info.guid = pdidInstance->guidInstance;
    char name[260];
    WideCharToMultiByte(CP_ACP, 0, pdidInstance->tszProductName, -1, name, 260, NULL, NULL);
    info.name = std::string(name);
    devices->push_back(info);
    return DIENUM_CONTINUE;
}
#endif

std::vector<DeviceInfo> DirectInputFFB::EnumerateDevices() {
    std::vector<DeviceInfo> devices;
#ifdef _WIN32
    if (!m_pDI) return devices;
    m_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, &devices, DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK);
#else
    DeviceInfo d1; d1.name = "Simucube 2 Pro (Mock)";
    DeviceInfo d2; d2.name = "Logitech G29 (Mock)";
    devices.push_back(d1);
    devices.push_back(d2);
#endif
    return devices;
}

void DirectInputFFB::ReleaseDevice() {
#ifdef _WIN32
    if (m_pEffect) {
        m_pEffect->Stop();
        m_pEffect->Unload();
        m_pEffect->Release();
        m_pEffect = nullptr;
    }
    if (m_pDevice) {
        m_pDevice->Unacquire();
        m_pDevice->Release();
        m_pDevice = nullptr;
    }
    m_active = false;
    m_isExclusive = false;
    m_deviceName = "None";
    std::cout << "[DI] Device released by user." << std::endl;
#else
    m_active = false;
    m_isExclusive = false;
    m_deviceName = "None";
#endif
}

bool DirectInputFFB::SelectDevice(const GUID& guid) {
#ifdef _WIN32
    if (!m_pDI) return false;

    // Cleanup old using new method
    ReleaseDevice();

    std::cout << "[DI] Attempting to create device..." << std::endl;
    if (FAILED(m_pDI->CreateDevice(guid, &m_pDevice, NULL))) {
        std::cerr << "[DI] Failed to create device." << std::endl;
        return false;
    }

    std::cout << "[DI] Setting Data Format..." << std::endl;
    if (FAILED(m_pDevice->SetDataFormat(&c_dfDIJoystick))) {
        std::cerr << "[DI] Failed to set data format." << std::endl;
        return false;
    }

    // Reset state
    m_isExclusive = false;

    // Attempt 1: Exclusive/Background (Best for FFB)
    std::cout << "[DI] Attempting to set Cooperative Level (Exclusive | Background)..." << std::endl;
    HRESULT hr = m_pDevice->SetCooperativeLevel(m_hwnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND);
    
    if (SUCCEEDED(hr)) {
        m_isExclusive = true;
        std::cout << "[DI] Cooperative Level set to EXCLUSIVE." << std::endl;
    } else {
        // Fallback: Non-Exclusive
        std::cerr << "[DI] Exclusive mode failed (Error: " << std::hex << hr << std::dec << "). Retrying in Non-Exclusive mode..." << std::endl;
        hr = m_pDevice->SetCooperativeLevel(m_hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
        
        if (SUCCEEDED(hr)) {
            m_isExclusive = false;
            std::cout << "[DI] Cooperative Level set to NON-EXCLUSIVE." << std::endl;
        }
    }
    
    if (FAILED(hr)) {
        std::cerr << "[DI] Failed to set cooperative level (Non-Exclusive failed too)." << std::endl;
        return false;
    }

    std::cout << "[DI] Acquiring device..." << std::endl;
    if (FAILED(m_pDevice->Acquire())) {
        std::cerr << "[DI] Failed to acquire device." << std::endl;
        // Don't return false yet, might just need focus/retry
    } else {
        std::cout << "[DI] Device Acquired in " << (m_isExclusive ? "EXCLUSIVE" : "NON-EXCLUSIVE") << " mode." << std::endl;
    }

    // Create Effect
    if (CreateEffect()) {
       m_active = true;
        std::cout << "[DI] SUCCESS: Physical Device fully initialized and FFB Effect created." << std::endl;
 
        return true;
    }
    return false;
#else
    m_active = true;
    m_deviceName = "Mock Device Selected";
    return true;
#endif
}

bool DirectInputFFB::CreateEffect() {
#ifdef _WIN32
    if (!m_pDevice) return false;

    DWORD rgdwAxes[1] = { DIJOFS_X };
    LONG rglDirection[1] = { 0 };
    DICONSTANTFORCE cf;
    cf.lMagnitude = 0;

    DIEFFECT eff;
    ZeroMemory(&eff, sizeof(eff));
    eff.dwSize = sizeof(DIEFFECT);
    eff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
    eff.dwDuration = INFINITE;
    eff.dwSamplePeriod = 0;
    eff.dwGain = DI_FFNOMINALMAX;
    eff.dwTriggerButton = DIEB_NOTRIGGER;
    eff.dwTriggerRepeatInterval = 0;
    eff.cAxes = 1;
    eff.rgdwAxes = rgdwAxes;
    eff.rglDirection = rglDirection;
    eff.lpEnvelope = NULL;
    eff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
    eff.lpvTypeSpecificParams = &cf;
    eff.dwStartDelay = 0;

    if (FAILED(m_pDevice->CreateEffect(GUID_ConstantForce, &eff, &m_pEffect, NULL))) {
        std::cerr << "[DI] Failed to create Constant Force effect." << std::endl;
        return false;
    }
    
    // Start immediately
    m_pEffect->Start(1, 0);
    return true;
#endif
    return true;
}

void DirectInputFFB::UpdateForce(double normalizedForce) {
    if (!m_active) return;

    // Sanity Check: If 0.0, stop effect to prevent residual hum
    if (std::abs(normalizedForce) < 0.00001) normalizedForce = 0.0;

    // --- DECLUTTERING: REMOVED CLIPPING WARNING ---
    /*
    if (std::abs(normalizedForce) > 0.99) {
        static int clip_log = 0;
        if (clip_log++ % 400 == 0) { 
            std::cout << "[DI] WARNING: FFB Output Saturated..." << std::endl;
        }
    }
    */
    // ----------------------------------------------

    // Clamp
    normalizedForce = (std::max)(-1.0, (std::min)(1.0, normalizedForce));

    // Scale to -10000..10000
    long magnitude = static_cast<long>(normalizedForce * 10000.0);

    // Optimization: Don't call driver if value hasn't changed
    if (magnitude == m_last_force) return;
    m_last_force = magnitude;

#ifdef _WIN32
    if (m_pEffect) {
        DICONSTANTFORCE cf;
        cf.lMagnitude = magnitude;
        
        DIEFFECT eff;
        ZeroMemory(&eff, sizeof(eff));
        eff.dwSize = sizeof(DIEFFECT);
        eff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
        eff.lpvTypeSpecificParams = &cf;
        
        // Try to update parameters
        HRESULT hr = m_pEffect->SetParameters(&eff, DIEP_TYPESPECIFICPARAMS);
        
        // --- DIAGNOSTIC & RECOVERY LOGIC ---
        if (FAILED(hr)) {
            // 1. Identify the Error
            std::string errorType = GetDirectInputErrorString(hr);

            // Append Custom Advice for Priority/Exclusive Errors
            if (hr == DIERR_OTHERAPPHASPRIO || hr == DIERR_NOTEXCLUSIVEACQUIRED ) {
                errorType += " [CRITICAL: Game has stolen priority! DISABLE IN-GAME FFB]";
                
                // Update exclusivity state to reflect reality
                m_isExclusive = false;
            }

            // FIX: Default to TRUE. If update failed, we must try to reconnect.
            bool recoverable = true; 

            // 2. Log the Context (Rate limited)
            static DWORD lastLogTime = 0;
            if (GetTickCount() - lastLogTime > DIAGNOSTIC_LOG_INTERVAL_MS) {
                std::cerr << "[DI ERROR] Failed to update force. Error: " << errorType 
                          << " (0x" << std::hex << hr << std::dec << ")" << std::endl;
                std::cerr << "           Active Window: [" << GetActiveWindowTitle() << "]" << std::endl;
                lastLogTime = GetTickCount();
            }

            // 3. Attempt Recovery (with Smart Cool-down)
            if (recoverable) {
                // Throttle recovery attempts to prevent CPU spam when device is locked
                static DWORD lastRecoveryAttempt = 0;
                DWORD now = GetTickCount();
                
                // Only attempt recovery if cooldown period has elapsed
                if (now - lastRecoveryAttempt > RECOVERY_COOLDOWN_MS) {
                    lastRecoveryAttempt = now; // Mark this attempt
                    
                    // DYNAMIC PROMOTION: If we hit NOTEXCLUSIVEACQUIRED, we MUST try to get Exclusive
                    if (hr == DIERR_NOTEXCLUSIVEACQUIRED) {
                        m_pDevice->Unacquire();
                        m_pDevice->SetCooperativeLevel(m_hwnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND);
                    }

                    HRESULT hrAcq = m_pDevice->Acquire();
                    
                    if (SUCCEEDED(hrAcq)) {
                        // Log recovery success (rate-limited for diagnostics)
                        static DWORD lastSuccessLog = 0;
                        if (GetTickCount() - lastSuccessLog > 5000) { // 5 second cooldown
                            std::cout << "[DI RECOVERY] Device re-acquired successfully. FFB motor restarted." << std::endl;
                            lastSuccessLog = GetTickCount();
                        }
                        
                        // Check if we managed to get Exclusive
                        if (hr == DIERR_NOTEXCLUSIVEACQUIRED) {
                             m_isExclusive = true; // Assume success if Acquire worked after SetCoop
                        }

                        // Restart the effect to ensure motor is active
                        m_pEffect->Start(1, 0); 
                        
                        // Retry the update immediately
                        m_pEffect->SetParameters(&eff, DIEP_TYPESPECIFICPARAMS);
                    }
                }
            }
        }
    }
#endif
}
```

### 3. `src/GuiLayer.cpp`
Updated to show the acquisition mode clearly.

```cpp
// ... inside DrawTuningWindow ...

    // Acquisition Mode & Troubleshooting
    if (DirectInputFFB::Get().IsActive()) {
        if (DirectInputFFB::Get().IsExclusive()) {
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Mode: EXCLUSIVE (Game FFB Blocked)");
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("LMUFFB has exclusive control.\nThe game can read steering but cannot send FFB.\nThis prevents 'Double FFB' issues.");
        } else {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.4f, 1.0f), "Mode: SHARED (Potential Conflict)");
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("LMUFFB is sharing the device.\nEnsure In-Game FFB is set to 'None' or 0% strength\nto avoid two force signals fighting each other.");
        }
    }
```