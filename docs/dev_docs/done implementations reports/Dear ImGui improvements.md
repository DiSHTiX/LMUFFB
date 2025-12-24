# Technical Report: Dynamic Window Resizing & Layout Management

## 1. Introduction & Context

### The Architecture: Frame vs. Canvas
To solve the resizing issue, it is crucial to understand how **Dear ImGui** interacts with the Windows Operating System.

1.  **The OS Window (The Frame):** This is the native Windows container created via the Win32 API (`CreateWindowW`). It has a title bar, minimize/close buttons, and a specific resolution (e.g., 800x600).
2.  **The ImGui Window (The Content):** This is a purely graphical element drawn by the GPU *inside* the Frame. In the standard ImGui branch, it cannot leave the Frame.

### The Problem
Currently, your application starts with a fixed Frame size. When the user enables "Graphs", the content becomes too wide for the Frame. The user is forced to manually resize the OS window to see the new content, and then resize it back when they are done. This is a poor User Experience (UX).

### The Solution: "Smart Container"
We will implement logic to make the OS Window **reactive**.
*   **State A (Config Mode):** The window is narrow (e.g., 500px).
*   **State B (Analysis Mode):** The window automatically expands (e.g., 1200px) to accommodate the graphs side-by-side.
*   **Persistence:** We will save the dimensions of *both* states independently. If a user resizes the "Analysis" view to fit their specific monitor, the app will remember that specific size next time they open the graphs.

---

## 2. Future Roadmap: The Docking Branch

It is worth noting that Dear ImGui has a separate development branch called **"Docking"** (currently in beta but widely used).

*   **Feature:** `ImGuiConfigFlags_ViewportsEnable`.
*   **Behavior:** This allows you to drag an ImGui tab *outside* of the main OS window. ImGui automatically creates a new, separate OS window for that tab on the fly.
*   **Benefit:** This is the ultimate solution for multi-monitor setups (e.g., Config on Screen 1, Telemetry on Screen 2).
*   **Why not now?** Switching branches requires updating your `vendor/imgui` files and potentially adjusting the backend initialization code. For the current codebase, the **Smart Container** solution below is the most robust and immediate fix without external dependency changes.

---

## 3. Implementation Guide

### Step 1: Configuration (Persistence)

We need to store the geometry for both states.

**File:** `src/Config.h`
```cpp
class Config {
public:
    // ... existing members ...
    
    // Window Geometry Persistence
    static int win_pos_x, win_pos_y;
    static int win_w_small, win_h_small; // Dimensions for Config Only
    static int win_w_large, win_h_large; // Dimensions for Config + Graphs
    static bool show_graphs;             // Remember if graphs were open
};
```

**File:** `src/Config.cpp`
Initialize with sensible defaults.
```cpp
// Defaults
int Config::win_pos_x = 100;
int Config::win_pos_y = 100;
int Config::win_w_small = 500;   // Narrow
int Config::win_h_small = 800;
int Config::win_w_large = 1400;  // Wide
int Config::win_h_large = 800;
bool Config::show_graphs = false;

// Update Config::Save and Config::Load to read/write these values
// (Standard boilerplate code)
```

### Step 2: Window Management Helpers

We need logic to resize the window and capture its current size before switching states.

**File:** `src/GuiLayer.cpp`

Add these helper functions (either as static functions or private members of `GuiLayer`):

```cpp
// Resize the native OS window
void ResizeWindow(HWND hwnd, int x, int y, int w, int h) {
    // SWP_NOZORDER: Don't change if it's on top or bottom
    // SWP_NOACTIVATE: Don't steal focus aggressively
    ::SetWindowPos(hwnd, NULL, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
}

// Save the current window size to the appropriate Config variable
void SaveCurrentWindowGeometry(bool is_graph_mode) {
    RECT rect;
    if (::GetWindowRect(g_hwnd, &rect)) {
        Config::win_pos_x = rect.left;
        Config::win_pos_y = rect.top;
        int w = rect.right - rect.left;
        int h = rect.bottom - rect.top;

        if (is_graph_mode) {
            Config::win_w_large = w;
            Config::win_h_large = h;
        } else {
            Config::win_w_small = w;
            Config::win_h_small = h;
        }
    }
}
```

### Step 3: Initialization

Ensure the app starts with the correct size based on the last saved state.

**File:** `src/GuiLayer.cpp` -> `GuiLayer::Init()`

```cpp
bool GuiLayer::Init() {
    // ... [WNDCLASSEX setup] ...

    // 1. Determine startup size
    int start_w = Config::show_graphs ? Config::win_w_large : Config::win_w_small;
    int start_h = Config::show_graphs ? Config::win_h_large : Config::win_h_small;

    // 2. Create Window with saved position and size
    g_hwnd = ::CreateWindowW(wc.lpszClassName, title.c_str(), WS_OVERLAPPEDWINDOW, 
        Config::win_pos_x, Config::win_pos_y, 
        start_w, start_h, 
        NULL, NULL, wc.hInstance, NULL);

    // ... [Rest of Init] ...
}
```

### Step 4: The Toggle Logic (The Core Fix)

This handles the transition when the user clicks the checkbox.

**File:** `src/GuiLayer.cpp` -> `DrawTuningWindow()`

```cpp
void GuiLayer::DrawTuningWindow(FFBEngine& engine) {
    std::lock_guard<std::mutex> lock(g_engine_mutex);

    // --- A. LAYOUT CALCULATION ---
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    
    // Define the fixed width for the configuration panel content
    const float CONFIG_PANEL_WIDTH = 500.0f; 

    // Calculate width: Full viewport if graphs off, fixed width if graphs on
    float current_width = Config::show_graphs ? CONFIG_PANEL_WIDTH : viewport->Size.x;

    // Lock the ImGui window to the left side of the OS window
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(ImVec2(current_width, viewport->Size.y));
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    ImGui::Begin("MainUI", nullptr, flags);

    // ... [Draw UI Elements] ...

    // --- B. THE CHECKBOX LOGIC ---
    // Use a temporary bool to detect the click event
    bool toggled = Config::show_graphs;
    
    if (ImGui::Checkbox("Graphs", &toggled)) {
        // 1. Save the geometry of the OLD state before switching
        SaveCurrentWindowGeometry(Config::show_graphs);
        
        // 2. Update state
        Config::show_graphs = toggled;
        
        // 3. Apply geometry of the NEW state
        int target_w = Config::show_graphs ? Config::win_w_large : Config::win_w_small;
        int target_h = Config::show_graphs ? Config::win_h_large : Config::win_h_small;
        
        // Resize the OS window immediately
        ResizeWindow(g_hwnd, Config::win_pos_x, Config::win_pos_y, target_w, target_h);
    }

    // ... [Rest of UI] ...
    ImGui::End();
}
```

### Step 5: The Graphs Layout

Ensure the graphs fill the remaining space on the right.

**File:** `src/GuiLayer.cpp` -> `DrawDebugWindow()`

```cpp
void GuiLayer::DrawDebugWindow(FFBEngine& engine) {
    // Only draw if enabled
    if (!Config::show_graphs) return;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    const float CONFIG_PANEL_WIDTH = 500.0f; 

    // Position: Start after the config panel
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + CONFIG_PANEL_WIDTH, viewport->Pos.y));
    
    // Size: Fill the rest of the width
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x - CONFIG_PANEL_WIDTH, viewport->Size.y));
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    ImGui::Begin("FFB Analysis", nullptr, flags);
    
    // ... [Graph Drawing Code] ...
    
    ImGui::End();
}
```

### Step 6: Shutdown Safety

Ensure we save the final window state when the app closes.

**File:** `src/GuiLayer.cpp` -> `Shutdown()`

```cpp
void GuiLayer::Shutdown() {
    // Capture the final position/size before destroying the window
    SaveCurrentWindowGeometry(Config::show_graphs);
    
    // ... [Rest of Shutdown] ...
}
```