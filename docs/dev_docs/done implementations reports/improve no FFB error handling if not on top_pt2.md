Here is the specific plan to add the **Smart Recovery Cool-down** logic to the existing implementation.

## Plan: Implement Recovery Throttling

**Objective:** Prevent the application from spamming the DirectInput driver with `Acquire()` calls 400 times a second when the device is locked by another application (e.g., the game in Exclusive Mode).

**Logic:**
1.  Define a cooldown constant (e.g., 2000ms).
2.  Track the timestamp of the last recovery attempt.
3.  Inside the error handling block, only attempt `Acquire()` if the cooldown period has elapsed.

---


**The Solution: "Smart Back-off" (Cool-down Timer)**

We need to implement a **Recovery Cool-down**.
1.  If an error occurs, we try to recover **once**.
2.  If that recovery fails (or if the error persists), we **wait** (e.g., 2 seconds) before trying again.
3.  This turns a 400Hz "Tug of War" into a gentle "Are you free yet?" check every few seconds.

This solves the background issue (it will reconnect when you tab back in) without spamming the driver while you are tabbed out.

Here is the updated implementation plan.

### Modified `src/DirectInputFFB.cpp`

We will add a static timer to the recovery block.

```cpp
// src/DirectInputFFB.cpp

// ... inside UpdateForce ...

    // Constants
    const DWORD RECOVERY_COOLDOWN_MS = 2000; // Wait 2 seconds between retry attempts

#ifdef _WIN32
    if (m_pEffect) {
        // ... [Setup structs] ...
        
        HRESULT hr = m_pEffect->SetParameters(&eff, DIEP_TYPESPECIFICPARAMS);
        
        if (FAILED(hr)) {
            // ... [Error Logging Logic (keep existing)] ...

            // --- SMART RECOVERY LOGIC ---
            static DWORD lastRecoveryAttempt = 0;
            DWORD now = GetTickCount();

            // Only attempt recovery if enough time has passed
            if (now - lastRecoveryAttempt > RECOVERY_COOLDOWN_MS) {
                
                // Mark this attempt
                lastRecoveryAttempt = now;

                // Try to get it back
                HRESULT hrAcq = m_pDevice->Acquire();
                
                if (SUCCEEDED(hrAcq)) {
                    std::cout << "[DI RECOVERY] Device Re-Acquired. Restarting Motor." << std::endl;
                    m_pEffect->Start(1, 0); 
                    m_pEffect->SetParameters(&eff, DIEP_TYPESPECIFICPARAMS);
                } else {
                    // If we failed to acquire, we are likely in the background or fighting the game.
                    // The cooldown ensures we don't spam this check 400 times/sec.
                }
            }
        }
    }
#endif
```
