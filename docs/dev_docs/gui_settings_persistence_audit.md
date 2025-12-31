# Audit Report: GUI Settings Persistence & Consistency

## 1. Introduction
This report documents the findings of an audit comparing the interactive settings available in `src/GuiLayer.cpp` against the persistence logic in `src/Config.cpp` and the `Preset` structure in `src/Config.h`. The goal was to identify settings that are adjusted by the user but fail to persist in the `config.ini` file or custom presets.

## 2. Identified Persistence Issues

### 2.1. Settings Missing from Active Configuration (Main Section)
The following settings can be adjusted in the "Advanced Settings" section of the GUI but are **never saved** to the disk. Consequently, they reset to defaults every time the application is restarted.

| GUI Setting | Internal Variable | Status |
| :--- | :--- | :--- |
| **Mute Below (Speed Gate)** | `engine.m_speed_gate_lower` | **Missing from Save/Load loop** |
| **Full Above (Speed Gate)** | `engine.m_speed_gate_upper` | **Missing from Save/Load loop** |

### 2.2. Settings Missing from User Presets
Custom presets (Saved by the user) fail to capture several important settings because these parameters are either missing from the `Preset` structure or the associated serialization loops.

| Setting | Preserved in Presets? | Reason |
| :--- | :--- | :--- |
| **Texture Load Cap** | **No** | Missing from the `Preset` struct in `Config.h`. |
| **Mute Below (Speed Gate)** | **No** | Missing from `Preset::Save` loop and `LoadPresets` parser. |
| **Full Above (Speed Gate)** | **No** | Missing from `Preset::Save` loop and `LoadPresets` parser. |
| **Road Fallback Scale** | **No** | Missing from `Preset::Save` loop and `LoadPresets` parser. |
| **Understeer Affects SoP**| **No** | Missing from `Preset::Save` loop and `LoadPresets` parser. |

### 2.3. Data Validation & Clamping Bugs
During the audit, inconsistencies were found in how values are "clamped" or restricted when loading from the `ini` file compared to their allowed ranges in the GUI.

*   **Brake Load Cap**: The GUI allows up to **10.0x**, but `Config::LoadPresets` incorrectly clamps the value to **3.0x** (`(std::min)(3.0f, std::stof(value))`).
*   **Lockup Strength**: The GUI allows up to **3.0x**, but `Config::LoadPresets` incorrectly clamps the value to **2.0x** (`(std::min)(2.0f, std::stof(value))`).

## 3. Workflow & UX Issues

### 3.1. Lack of Auto-Save
As noted in `TODO.md` and `report_ui_ux_overhaul.md`, the application currently relies on manual saving for most parameters.
*   **Problem**: Changes made to sliders are held in memory only. If the app crashes or is closed without clicking "Save Current Config", the changes are lost.
*   **Exception**: Only Toggling Graphs or Selecting a Device triggers an immediate `Config::Save()`.

### 3.2. Unsaved State on Shutdown
The `GuiLayer::Shutdown()` process captures the final window position and size but **fails to call `Config::Save()`**.
*   **Result**: Window position changes are often "lost" unless the user happened to trigger a save by other means during the session.

## 4. Recommendations for Implementation Fixes
1.  **Update `Preset` Struct**: Add `m_texture_load_cap` and ensuring all `speed_gate` variables are included.
2.  **Synchronize Loops**: Update the `for` loops in `Config::Save` (Preset section) and the parser in `Config::LoadPresets` to include the missing fields.
3.  **Fix Clamping**: Adjust the `std::min` limits in `LoadPresets` to match the maximum values allowed in the GUI.
4.  **Implement Auto-Save**: Introduce a mechanism to trigger `Config::Save()` when a GUI slider is released or when the window is closed.
