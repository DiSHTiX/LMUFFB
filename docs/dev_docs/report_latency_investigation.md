# Report: Perceived Latency Investigation

## 1. Context and Problem Statement
**Perceived Latency**: Users report a "delay" and "disconnect from game physics" even when smoothing is disabled. We need to investigate if this is inherent to the specific game/wheel combination or if the app's processing loop introduces avoidable lag.

Note: this will be addressed / implemented later; we will also first need confirmation from testing by other users with DDs that the issue is still present before proceeding with this.

## 2. Proposed Solution: Latency Investigation & Monitoring
*   **Timestamp Logging**: To investigate latency, we will add high-precision timestamps to the console output when `DirectInput` packets are sent vs. when Game Telemetry is received.
*   **Processing Loop Check**: Verify that the main loop sleep times are not causing jitter.

## 3. Testing Plan

### 3.1. Latency Check
*   **Setup**: Enable the new timestamp logging.
*   **Action**: Correlate game physics update time (from `mElapsedTime`) with the wall-clock time of the FFB packet submission.
*   **Verification**: Calculate the delta. If > 10ms, investigate thread scheduling or VSync settings.
