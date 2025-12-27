# Testing Instructions for Composite Screenshot Fix

## Current Status
The code has been updated with diagnostic logging to help identify why the console window capture is failing.

## What to Test

### Step 1: Close the Running Application
The application is currently running and preventing the rebuild. Please:
1. Close the lmuFFB application (the GUI window)
2. This will allow the rebuild to proceed

### Step 2: Rebuild the Application
Run this command:
```powershell
& 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1' -Arch amd64 -SkipAutomaticLocation
cmake --build build --config Release --target LMUFFB
```

### Step 3: Run the Application
```
.\build\Release\LMUFFB.exe
```

### Step 4: Take a Screenshot
1. Click the "Save Screenshot" button
2. **Look at the console output** - you should now see diagnostic messages like:
   ```
   [GUI] GUI window capture: SUCCESS
   [GUI] Console window found, attempting capture...
   [GUI] Console window capture: SUCCESS (or FAILED)
   [GUI] Console dimensions: WIDTHxHEIGHT
   ```

### Step 5: Report the Console Output
Please share:
1. The exact console output when you click "Save Screenshot"
2. Whether the screenshot now includes both windows or just the GUI

## What the Diagnostic Logging Will Tell Us

The new logging will show:
- ✅ **"Console window found"** - GetConsoleWindow() is working
- ✅ **"Console window capture: SUCCESS"** - PrintWindow is working
- ❌ **"Console window capture: FAILED"** - PrintWindow is failing (this is the likely issue)
- ❌ **"No console window found"** - GetConsoleWindow() returned NULL (unlikely)

## Possible Issues and Solutions

### If "Console window capture: FAILED"
This means `PrintWindow` is failing for the console window. Possible causes:
1. **Console window is minimized** - PrintWindow may not work on minimized windows
2. **Console window has special properties** - Some console windows can't be captured with PrintWindow
3. **Permissions issue** - The application may not have permission to capture the console

**Solution**: We may need to use a different capture method for console windows, such as:
- Using `BitBlt` with desktop DC
- Using Windows Desktop Duplication API
- Capturing the screen region where the console is located

### If "No console window found"
This would mean `GetConsoleWindow()` is returning NULL, which is unlikely since you can see the console in your screenshot.

## Next Steps

Once you provide the console output, I'll know exactly what's failing and can implement the appropriate fix.
