Please address these 3 issues:

###  Critical Issues Detected

Please verify if this issue exists and in case fix it:

I found a significant bug in `DrawDebugWindow` regarding thread safety and data sampling.

#### A. Missing Mutex Lock (Race Condition)
In `DrawTuningWindow`, you correctly lock the mutex:
```cpp
std::lock_guard<std::mutex> lock(g_engine_mutex); // Correct
```
However, in `DrawDebugWindow`, **there is no lock**.
```cpp
void GuiLayer::DrawDebugWindow(FFBEngine& engine) {
    ImGui::Begin("FFB Analysis", &m_show_debug_window);
    
    // READING SHARED DATA WITHOUT LOCK
    plot_total.Add(engine.m_last_debug.total_output); 
    // ...
```
**The Risk:** The FFB Thread (400Hz) is writing to `engine.m_last_debug` at the same time the GUI Thread (60Hz) is reading it. This causes "Tearing". You might read the X acceleration from Frame 100 and the Steering Force from Frame 101. While usually subtle, it can cause weird spikes in the graphs.

**Fix:** Add the lock at the start of `DrawDebugWindow`:
```cpp
void GuiLayer::DrawDebugWindow(FFBEngine& engine) {
    std::lock_guard<std::mutex> lock(g_engine_mutex); // <--- ADD THIS
    ImGui::Begin("FFB Analysis", &m_show_debug_window);
    // ...
```

#### B. Sampling Aliasing (The "Binary" Look)
This explains why the user sees "Binary" or "Square Wave" data.
*   **Physics:** Updates 400 times per second.
*   **GUI:** Updates 60 times per second.
*   **The Problem:** The GUI simply grabs the *current* value when it renders. It **ignores** the 6 or 7 physics updates that happened in between frames.
    *   If the FFB is vibrating at 50Hz (a common frequency for "Crunch" or "Road Texture"), the 60Hz GUI will sample it at random points in the wave.
    *   Sometimes it catches the peak (+1.0), sometimes the trough (-1.0).
    *   Visually, this looks like the value is teleporting between +1 and -1 (Binary), rather than a smooth sine wave.

**Fix (Future):** To fix the graphs, you would need a thread-safe Queue where the FFB thread pushes *every* data point, and the GUI thread consumes them all. For now, just be aware that the graphs are "lossy snapshots."

#### C. Auto-Scaling Confusion
In `DrawDebugWindow`, you provide min/max values for some plots but not others, or the ranges might be too wide/narrow.
*   **Example:** `plot_input_steer` is set to -5000 to +5000. If the game outputs 0 (because FFB is off), it looks like a perfect flat line.
*   **Example:** `plot_clipping` is 0.0 to 1.1.
*   **Issue:** `ImGui::PlotLines` does not draw a "zero line" in the middle. If the data is 0.0, it draws a line at the bottom (or middle depending on range).
*   **Recommendation:** Ensure users know that a flat line in the *middle* is 0, but a flat line at the *bottom* might be -1 (or min range). Adding a tooltip or a value readout (e.g., `ImGui::Text("Value: %.2f", current_val)`) next to the graph helps immensely.