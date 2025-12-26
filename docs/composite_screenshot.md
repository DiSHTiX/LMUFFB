# Composite Screenshot Feature

**Version:** 0.6.5+  
**Date:** 2025-12-26  
**Feature:** Composite GUI + Console Screenshot

## Overview

The composite screenshot feature captures both the lmuFFB GUI window and the console window in a single image, making it easier to share debugging information and application state with the community.

## Usage

1. **Open lmuFFB:** Launch the application normally
2. **Position Windows:** Arrange the GUI and console windows as desired (they will be captured regardless of position)
3. **Click "Save Screenshot":** Press the "Save Screenshot" button in the GUI
4. **Find Your Screenshot:** The image will be saved in the application directory with a timestamp

### Screenshot Filename Format
```
screenshot_YYYY-MM-DD_HH-MM-SS.png
```

Example: `screenshot_2025-12-26_11-30-45.png`

## Features

### Automatic Window Detection
- **GUI Window:** Always captured (the main lmuFFB interface)
- **Console Window:** Automatically detected and captured if present
- **Fallback:** If console is not available, captures GUI only

### Composite Layout
The screenshot creates a side-by-side layout:
```
┌─────────────┬───┬──────────────┐
│             │   │              │
│  GUI Window │ G │   Console    │
│             │ A │   Window     │
│             │ P │              │
└─────────────┴───┴──────────────┘
```

- **Gap:** 10 pixels of dark gray background between windows
- **Alignment:** Both windows aligned at the top
- **Background:** Dark gray (#1E1E1E) fills any empty space

### Console Information Captured
The console window typically shows:
- FFB loop status messages
- Connection status to LMU
- Device acquisition messages
- Warning and error messages
- Telemetry data warnings
- Performance information

## Technical Details

### Implementation
The feature uses two different capture methods:

1. **GUI Window:** Captured using Windows GDI (BitBlt)
   - Works for any window, including DirectX-rendered content
   - Captures the actual window as displayed on screen

2. **Console Window:** Captured using Windows GDI (BitBlt)
   - Detects console using `GetConsoleWindow()`
   - Captures text and background colors

### Image Format
- **Format:** PNG (lossless compression)
- **Color Depth:** 32-bit RGBA
- **Channels:** 4 (Red, Green, Blue, Alpha)
- **Alpha:** Forced to 255 (fully opaque)

### Memory Management
- Temporary buffers allocated for each window
- Composite buffer created for final image
- All buffers automatically freed after save
- No memory leaks

## Troubleshooting

### Console Not Captured
**Problem:** Screenshot only shows GUI window

**Possible Causes:**
1. Application compiled as Windows GUI application (no console)
2. Console window was closed manually
3. Running in headless mode (`--headless` flag)

**Solution:**
- Ensure you're running the standard build (not headless)
- Don't close the console window before taking screenshot
- Check console output for "[GUI] Screenshot saved (GUI only)" message

### Screenshot Failed
**Problem:** Error message "Screenshot failed: No windows to capture"

**Possible Causes:**
1. GUI window handle is invalid
2. Application in unusual state

**Solution:**
- Restart the application
- Check console for error messages
- Report issue on GitHub with console output

### Large File Size
**Problem:** Screenshot files are very large

**Explanation:**
- PNG format is lossless, so large windows = large files
- Typical size: 500KB - 2MB depending on window sizes

**Solution:**
- This is normal and expected
- PNG provides best quality for UI screenshots
- If needed, compress with external tools after capture

## Sharing Screenshots

### Best Practices for Forum Posts
1. **Take screenshot during issue:** Capture the exact moment the problem occurs
2. **Include console output:** The console often shows relevant error messages
3. **Show relevant settings:** Ensure the GUI shows the settings you're discussing
4. **Enable graphs if relevant:** Turn on "Graphs" checkbox before screenshot

### Recommended Workflow
```
1. Reproduce the issue
2. Adjust window sizes for readability
3. Click "Save Screenshot"
4. Upload to forum/GitHub issue
5. Reference filename in your post
```

### Privacy Considerations
- Screenshots may contain system information in console
- Window titles may show your username
- File paths may be visible in console output
- Review screenshot before sharing if privacy is a concern

## Code Reference

### Main Functions
- `CaptureWindowToBuffer()` - Captures any window using GDI
- `SaveCompositeScreenshot()` - Creates composite image
- `SaveScreenshot()` - Legacy DirectX-only capture (still available)

### Source Files
- `src/GuiLayer.cpp` - Lines 285-455 (screenshot implementation)
- Button handler: Line 639

### Dependencies
- Windows GDI (GetDC, BitBlt, GetDIBits)
- stb_image_write.h (PNG encoding)
- Standard C++ (vector, iostream)

## Future Enhancements

See `docs/dev_docs/console_to_gui_integration.md` for planned improvements:
- Integrated console panel within GUI
- Single-window screenshot
- Enhanced console features (filtering, search, export)

## Version History

### v0.6.4 (2025-12-26)
- **Added:** Composite screenshot feature
- **Added:** Automatic console window detection
- **Added:** Side-by-side layout with gap
- **Added:** Fallback to GUI-only if console unavailable
- **Improved:** Console output includes dimensions

### Previous Versions
- v0.6.3 and earlier: DirectX-only screenshot (GUI window only)

## Related Documentation

- [Console to GUI Integration](console_to_gui_integration.md) - Future enhancement plan
- [FFB Tuning Recommendations](../FFB%20Tuning%20Recommendations.md) - How to use screenshots for tuning
- [Driver's Guide to Testing](../Driver's%20Guide%20to%20Testing%20LMUFFB.md) - Testing scenarios

## Support

If you encounter issues with the screenshot feature:
1. Check console output for error messages
2. Try restarting the application
3. Report on [GitHub Issues](https://github.com/coasting-nc/LMUFFB/issues)
4. Include console output in your report
