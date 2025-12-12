This is a critical feature for stabilizing the new physics workarounds. Since we are now *calculating* physics rather than just reading them, we need to see the math in action over time to tune it.

Here is the design proposal for a **High-Performance Asynchronous Telemetry Logger**.

### 1. Architectural Constraints
*   **The Golden Rule:** You **cannot** write to disk inside the `FFBThread` (400Hz). Disk I/O is blocking and unpredictable (can take 1ms or 100ms). Doing so will cause the FFB to stutter.
*   **The Solution:** **Double-Buffered Asynchronous Logging**.
    1.  **Producer (FFB Thread):** Writes data to a fast in-memory buffer (RAM).
    2.  **Consumer (Worker Thread):** Wakes up periodically, swaps the buffer, and writes the data to disk (CSV).

### 2. Data Format: CSV (Comma Separated Values)
While binary is faster, **CSV** is the right choice here because:
1.  **Universal:** Opens in Excel, Google Sheets.
2.  **MegaLogViewer:** Can be imported directly into tools like MegaLogViewer (used by tuners) or Motec i2 (via converters).
3.  **Human Readable:** You can open it in Notepad to check if a value is exactly `0.000`.

### 3. Implementation Design

#### A. The Data Structure (`LogFrame`)
We need a struct that captures the exact state of a physics tick.

```cpp
struct LogFrame {
    double timestamp;      // Time since session start
    
    // Inputs
    float steering_torque;
    float throttle;
    float brake;
    
    // Raw Telemetry (The "Truth")
    float raw_load_fl;
    float raw_grip_fl;
    float raw_susp_force_fl;
    float raw_ride_height_fl;
    float raw_lat_vel;
    
    // Calculated Physics (The "Workaround")
    float calc_load_fl;
    float calc_grip_fl;
    float calc_slip_ratio_fl;
    float calc_slip_angle_fl;
    
    // FFB Outputs (The Result)
    float ffb_total;
    float ffb_sop;
    float ffb_road;
    float ffb_scrub;
    bool  clipping;
    
    // Markers
    bool  user_marker; // Did user press "Mark" button?
};
```

#### B. The Logger Class (`AsyncLogger`)

```cpp
class AsyncLogger {
public:
    void Start(std::string filename);
    void Stop();
    
    // Called from FFBThread (400Hz) - Must be lock-free or extremely fast
    void Log(const LogFrame& frame);

private:
    void WorkerThread(); // The background writer

    std::vector<LogFrame> m_buffer_active;
    std::vector<LogFrame> m_buffer_writing;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_running;
    std::thread m_thread;
};
```

#### C. The Logic (Double Buffering)

1.  **FFB Loop:** Calls `Log(frame)`.
    *   Acquires lock (very brief).
    *   `m_buffer_active.push_back(frame)`.
    *   If `m_buffer_active.size() > 1000` (approx 2.5 seconds of data), notify the worker thread.
2.  **Worker Thread:**
    *   Wakes up.
    *   Acquires lock.
    *   **Swaps** `m_buffer_active` with `m_buffer_writing`. (This is instant).
    *   Releases lock. (FFB thread can keep writing to the new empty active buffer).
    *   Writes `m_buffer_writing` to disk.
    *   Clears `m_buffer_writing`.

### 4. Making it "Informative" (Analysis Features)

To make these logs actually useful for diagnosing the "Zero Load" or "Instability" issues, we should add:

1.  **Session Header:**
    At the top of the CSV, write the **Config Settings** used for that session.
    ```csv
    # LMUFFB Log v1.0
    # Date: 2025-12-12
    # Settings: Gain=0.8, SoP=0.15, LoadMethod=SuspForce, ManualSlip=True
    Time,Steer,Throttle,...
    ```
    *Why:* When you analyze a log 3 days later, you need to know *which* settings caused that oscillation.

2.  **The "Marker" Button:**
    Add a button in the GUI (or a keyboard shortcut like `Spacebar`) that sets a flag in the log.
    *   *Scenario:* You are driving and feel a weird "clunk". You hit Space.
    *   *Analysis:* Open Excel, filter for `Marker == 1`. You instantly find the exact timestamp of the issue.

3.  **Derived Deltas (Optional but helpful):**
    Log `DeltaTime` (dt).
    *   *Why:* To detect if the FFB loop is stuttering or missing frames. If `dt` spikes from 0.0025 to 0.020, you have a performance problem.

### 5. Analysis Workflow (How to use it)

1.  **MegaLogViewer (Recommended):**
    *   It handles large CSVs easily.
    *   You can plot `Calc Grip` vs `Steering Torque` to see if the understeer effect is triggering correctly.
2.  **Excel / Google Sheets:**
    *   Good for short captures (< 30 seconds).
    *   Use Conditional Formatting on the `Raw Load` column to highlight exactly when it drops to 0.

### 6. Implementation Plan

1.  **Phase 1 (Core):** Implement `AsyncLogger` class with double buffering.
2.  **Phase 2 (Integration):** Add `m_logger` to `FFBEngine`. Call `Log()` at the end of `calculate_force`.
3.  **Phase 3 (GUI):** Add "Start/Stop Logging" toggle in the Troubleshooting window.

**Would you like me to generate the full C++ code for this `AsyncLogger` class?**
