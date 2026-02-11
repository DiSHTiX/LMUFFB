# Security Audit & False Positive Analysis (v0.7.25)

## Overview
This report analyzes the `lmuFFB` codebase to identify features and patterns that may trigger antivirus heuristics or behavioral monitoring warnings. While direct access to the specific VirusTotal report was unavailable, a static analysis of the source code reveals several areas that are common sources of false positives in game utilities.

## Findings

### 1. Missing Executable Metadata (High Probability Trigger)
The current `src/res.rc` file contains only an icon definition. It lacks the standard `VERSIONINFO` resource block.
*   **Impact**: Antivirus heuristics often flag binaries without version information, company name, or product description as "generic" or "suspicious" (e.g., specific trojans often lack this).
*   **Behavior**: The file appears "anonymous" to the OS and security software.

### 2. Process Handle Usage (`GameConnector.cpp`)
The application uses `OpenProcess` to verify if the game is running.
```cpp
m_hProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
```
*   **Analysis**: The usage of `PROCESS_QUERY_LIMITED_INFORMATION` is good practice (least privilege). However, the act of opening a handle to another process is a core behavior of game hacks and injection tools.
*   **Risk**: Moderate. Some sensitive heuristics might flag this as "Process Access" or "Memory Inspection".

### 3. DirectInput Exclusive Mode (`DirectInputFFB.cpp`)
The application requests `DISCL_EXCLUSIVE` access to the input device.
```cpp
hr = ((IDirectInputDevice8*)m_pDevice)->SetCooperativeLevel(m_hwnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND);
```
*   **Analysis**: This is necessary for high-fidelity FFB. However, exclusive input access can be interpreted by behavioral analysis as potential input interception (keylogging/macro behavior), especially if combined with background processing.
*   **Status**: This is a known false-positive vector for input tools but is functionally required.

### 4. Code Signing (Missing)
*   **Observation**: The artifact is likely unsigned.
*   **Impact**: Lack of a digital signature is the single biggest factor in "User Reputation" scores on SmartScreen and many AV engines.

## Recommendations

### Short Term (Code Changes)

1.  **Implement `VERSIONINFO` in Resource File**:
    *   Modify `src/res.rc` to include a full `VS_VERSION_INFO` block.
    *   Include `CompanyName`, `FileDescription`, `FileVersion`, `InternalName`, `LegalCopyright`, `OriginalFilename`, `ProductName`, `ProductVersion`.
    *   **Why**: This legitimizes the binary and distinguishes it from generic malware templates.

2.  **Verify Build Security Flags**:
    *   Ensure the compiler uses `/GS` (Buffer Security Check), `/DYNAMICBASE` (ASLR), and `/NXCOMPAT` (DEP). (These are usually default in CMake/VS, but confirming them is good).

### Long Term

1.  **Code Signing**:
    *   Acquire a code signing certificate (e.g., EV or Standard) to sign the executable. This whitelist the app with Microsoft and many AV vendors instantly.

2.  **False Positive Submission**:
    *   Submit the clean file to vendors (Microsoft, Symantec, Kaspersky) via their "False Positive" web forms.

## Action Plan: `VERSIONINFO` Implementation

Use the following template for `src/res.rc` (to be implemented):

```rc
#include <windows.h>
#include "resource.h"

// ... existing ICON ...

VS_VERSION_INFO VERSIONINFO
FILEVERSION     0,7,25,0
PRODUCTVERSION  0,7,25,0
FILEFLAGSMASK   VS_FFI_FILEFLAGSMASK
FILEFLAGS       0
FILEOS          VOS_NT_WINDOWS32
FILETYPE        VFT_APP
FILESUBTYPE     VFT2_UNKNOWN
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "040904E4"
        BEGIN
            VALUE "CompanyName",      "Mmuffb Community"
            VALUE "FileDescription",  "Le Mans Ultimate FFB Bridge"
            VALUE "FileVersion",      "0.7.25.0"
            VALUE "InternalName",     "lmuFFB"
            VALUE "LegalCopyright",   "© 2026"
            VALUE "OriginalFilename", "LMUFFB.exe"
            VALUE "ProductName",      "lmuFFB"
            VALUE "ProductVersion",   "0.7.25.0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
        VALUE "Translation", 0x409, 1252
    END
END
```

## How to Export VirusTotal Report (vt-cli)

If you obtain an API key (free tier is sufficient), you can fetch the JSON report for deeper analysis:

1.  **Install vt-cli**: https://github.com/VirusTotal/vt-cli/releases
2.  **Configure API Key**: `vt init`
3.  **Fetch Report**:
    ```powershell
    vt file behaviors 9652deae0ca058c637a5890c198d7bec542ed9dbd94ea621845a6c209896d964 --format json > report.json
    ```
4.  Share the `report.json` with me for detailed parsing of "Sandbox reports" and "Mitre ATT&CK" matrices.
