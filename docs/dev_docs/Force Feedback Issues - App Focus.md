# Exclusive Device Acquisition & Recovery Report

## 1. Problem Analysis: The "Tug of War"
The reported issue where Force Feedback stops working after Alt-Tabbing or changing window focus is caused by a conflict over **DirectInput Exclusive Access**.

*   **The Mechanism:** To send Force Feedback commands, an application must hold **Exclusive Access** to the device.
*   **The Conflict:** When Le Mans Ultimate (LMU) gains focus, it automatically attempts to acquire the wheel in Exclusive Mode. Windows grants this priority to the foreground window, silently "demoting" LMUFFB to **Shared Mode** (Non-Exclusive).
*   **The Symptom:** In Shared Mode, LMUFFB can still read steering inputs (so the graphs might move), but any attempt to write FFB forces fails with error `0x80040205` (`DIERR_NOTEXCLUSIVEACQUIRED`).
*   **The Loop:** Standard recovery logic simply calls `Acquire()`. However, since the device handle is already in "Shared Mode" (due to the demotion), `Acquire()` succeeds immediately but **keeps the app in Shared Mode**. The app remains trapped in a state where it is "Connected" but "Muted."

## 2. The Solution: Dynamic Promotion
To break this loop, we implement **Dynamic Promotion** within the existing connection recovery logic.

*   **Logic:** When the app detects the specific error `DIERR_NOTEXCLUSIVEACQUIRED`, it does not just try to re-acquire. It explicitly:
    1.  **Unacquires** the device (releasing the Shared handle).
    2.  **Requests Promotion** by calling `SetCooperativeLevel` with `DISCL_EXCLUSIVE`.
    3.  **Re-acquires** the device.
*   **Safety Throttle:** This logic is placed inside the existing **2-second recovery cooldown**. This prevents a "resource race" or stuttering that would occur if we attempted this heavy operation every frame (400Hz).
*   **Motor Restart:** Upon successful re-acquisition, we explicitly call `m_pEffect->Start(1, 0)` to ensure the FFB motor is reactivated, fixing the "silent wheel" bug.

---

## 3. Implementation Plan

### Step 1: Track Acquisition State
We need to know if we currently hold Exclusive rights so we can display this status to the user.

**File:** `src/DirectInputFFB.h`

```cpp
class DirectInputFFB {
public:
    // ... existing methods ...
    
    // NEW: Check if device was acquired in exclusive mode
    bool IsExclusive() const { return m_isExclusive; }

private:
    // ... existing members ...
    
    bool m_isExclusive = false; // Track acquisition mode
};
```

### Step 2: Implement Dynamic Promotion Logic
We modify the `UpdateForce` loop to handle the specific error code and promote the cooperative level.

**File:** `src/DirectInputFFB.cpp`

```cpp
// Inside DirectInputFFB::UpdateForce(double normalizedForce)

    // ... [Calculation of magnitude] ...

#ifdef _WIN32
    if (m_pEffect) {
        // ... [Setup DICONSTANTFORCE cf and DIEFFECT eff] ...
        
        // Try to update parameters
        HRESULT hr = m_pEffect->SetParameters(&eff, DIEP_TYPESPECIFICPARAMS);
        
        // --- DIAGNOSTIC & RECOVERY LOGIC ---
        if (FAILED(hr)) {
            // 1. Identify if the error is recoverable
            bool recoverable = (hr == DIERR_INPUTLOST || 
                                hr == DIERR_NOTACQUIRED || 
                                hr == DIERR_OTHERAPPHASPRIO || 
                                hr == DIERR_NOTEXCLUSIVEACQUIRED);

            // 2. Log Error (Rate limited to 1s)
            // ... [Existing logging logic] ...

            // 3. Attempt Recovery (Throttled to every 2 seconds)
            static DWORD lastRecoveryAttempt = 0;
            DWORD now = GetTickCount();
            
            if (recoverable && (now - lastRecoveryAttempt > RECOVERY_COOLDOWN_MS)) {
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
                    std::cout << "[DI RECOVERY] Device re-acquired successfully." << std::endl;
                    
                    // Update our internal state if we fixed the exclusivity
                    if (hr == DIERR_NOTEXCLUSIVEACQUIRED) {
                            m_isExclusive = true; 
                    }

                    // CRITICAL FIX: Restart the effect motor
                    // Often, re-acquiring is not enough; the effect must be restarted.
                    m_pEffect->Start(1, 0); 
                    
                    // Retry the update immediately so the user feels it instantly
                    m_pEffect->SetParameters(&eff, DIEP_TYPESPECIFICPARAMS);
                }
            }
        }
    }
#endif
```

### Step 3: Initialize State Correctly
Ensure we reset the state when selecting a new device.

**File:** `src/DirectInputFFB.cpp`

```cpp
bool DirectInputFFB::SelectDevice(const GUID& guid) {
    // ... [Device Creation] ...

    // Reset state
    m_isExclusive = false;

    // Attempt 1: Exclusive/Background (Best for FFB)
    HRESULT hr = m_pDevice->SetCooperativeLevel(m_hwnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND);
    
    if (SUCCEEDED(hr)) {
        m_isExclusive = true;
        std::cout << "[DI] Cooperative Level set to EXCLUSIVE." << std::endl;
    } else {
        // Fallback: Non-Exclusive
        std::cerr << "[DI] Exclusive mode failed. Retrying in Non-Exclusive mode..." << std::endl;
        hr = m_pDevice->SetCooperativeLevel(m_hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
        
        if (SUCCEEDED(hr)) {
            m_isExclusive = false;
        }
    }
    
    // ... [Acquire and CreateEffect] ...
}
```

### Step 4: Visual Feedback in GUI
Display the current mode so the user knows if they are in a "Safe" (Exclusive) or "Conflict-Prone" (Shared) state.

**File:** `src/GuiLayer.cpp`

```cpp
// Inside DrawTuningWindow...

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