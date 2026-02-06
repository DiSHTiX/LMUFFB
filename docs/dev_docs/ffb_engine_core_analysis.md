# FFB Engine Core Analysis

## Overview

The LMUFFB FFB Engine is a sophisticated force feedback calculation system designed to provide realistic steering feel for Le Mans Ultimate. It processes telemetry data at 400Hz to generate torque commands for steering wheels, emphasizing tire physics and road feedback.

## Architecture

### Core Components

The engine consists of several interconnected calculation modules:

```
Telemetry Input → Sanitization → Force Calculation → Output Scaling → Hardware
     ↓              ↓              ↓                    ↓            ↓
   Raw Data → Validation → Base + Effects → Normalization → Clipping → Wheel
```

### Key Data Structures

- **TelemInfoV01**: Raw telemetry from LMU shared memory
- **ProcessedTelemetryData**: Sanitized data from TelemetryProcessor
- **FFBSnapshot**: Debug/telemetry data for GUI visualization
- **FFBEngine**: Main calculation class with 50+ configurable parameters

## Force Calculation Pipeline

### 1. Input Processing & Sanitization

```cpp
// Delta time validation (400Hz target)
if (dt <= 0.000001) dt = 0.0025;

// Telemetry processing with fallback
auto processedData = m_telemetryProcessor.process(vehicleData);

// Load calculation with suspension fallback
double avg_load = processedData.totalTireLoad;
```

**Key Features:**
- Hysteresis-based missing data detection (20-frame threshold)
- Automatic fallback to suspension forces when tire loads unavailable
- NaN/infinity sanitization

### 2. Base Force Calculation

```cpp
// Primary FFB source: Steering shaft torque
double base_force = data->mSteeringShaftTorque;

// Grip modulation (understeer/oversteer feel)
double grip_factor = calculate_grip_factor(processedData);
base_force *= grip_factor;

// Load scaling (tire pressure simulation)
base_force *= calculate_load_factor(avg_load);
```

**Grip Factor Calculation:**
- Uses tire load ratios to detect weight transfer
- Front/rear grip imbalance creates understeer feel
- Hysteresis prevents oscillation during transitions

### 3. Seat of Pants (SoP) Effects

```cpp
// Lateral G-force simulation
double lateral_g = data->mLocalAccel.x;
double sop_force = lateral_g * m_sop_effect * m_sop_scale;

// Yaw kick (rotational acceleration feedback)
double yaw_accel = data->mLocalRotAccel.y;
double yaw_force = yaw_accel * m_sop_yaw_gain * BASE_NM_YAW_KICK;

// Speed gating eliminates idle noise
sop_total *= speed_gate;
```

**SoP Components:**
- **Lateral G**: Simulates cornering forces (oversteer feel)
- **Yaw Kick**: Provides counter-steering torque during slides
- **Speed Gating**: Eliminates vibration at low speeds

### 4. Texture Effects System

The engine generates multiple frequency-modulated vibration effects:

#### Road Texture
```cpp
// Base frequency from tire contact
double road_freq = calculate_road_frequency(car_speed, tire_radius);

// Amplitude from suspension deflection
double road_amp = suspension_deflection * m_road_texture_gain;

// Phase integration prevents clicks
m_road_phase += road_freq * dt * TWO_PI;
road_noise = sin(m_road_phase) * road_amp;
```

#### Slide Texture
```cpp
// Triggered by grip loss
if (grip_factor < 0.8) {
    double slide_freq = 30.0 + (grip_loss * 20.0);
    m_slide_phase += slide_freq * dt * TWO_PI;
    slide_noise = sin(m_slide_phase) * m_slide_texture_gain;
}
```

#### Lockup Effects
```cpp
// ABS pulses during braking
if (m_abs_pulse_enabled && brake_pressure > threshold) {
    m_abs_phase += ABS_FREQ * dt * TWO_PI;
    abs_pulse = sin(m_abs_phase) * m_abs_gain;
}

// Lockup rumble during traction loss
if (wheel_slip > lockup_threshold) {
    lockup_rumble = generate_lockup_pattern(wheel_slip);
}
```

### 5. Gyroscopic Damping

```cpp
// Calculate steering velocity
double steer_vel = (steer_angle - m_prev_steering_angle) / dt;

// Low-pass filter to smooth
double alpha = dt / (m_gyro_smoothing + dt);
m_steering_velocity_smoothed += alpha * (steer_vel - m_steering_velocity_smoothed);

// Damping force opposes steering movement
double gyro_force = -m_steering_velocity_smoothed * m_gyro_gain * (car_speed / SCALE);
```

**Purpose:** Provides mechanical steering feel, especially valuable for wheel setups without built-in dampers.

### 6. Output Processing

```cpp
// Normalize to [-1, 1] range
double norm_force = total_force / m_max_torque_ref;

// Apply master gain
norm_force *= m_gain;

// Minimum force boost (overcomes wheel friction)
if (abs(norm_force) > 0.0001 && abs(norm_force) < m_min_force) {
    norm_force = sign(norm_force) * m_min_force;
}

// Force inversion option
if (m_invert_force) norm_force *= -1.0;

// Final clipping
return max(-1.0, min(1.0, norm_force));
```

## Key Algorithms

### Grip Calculation

The engine uses a sophisticated grip approximation system:

```cpp
double calculate_grip_factor(const ProcessedTelemetryData& data) {
    // Use available grip data or fallback to approximation
    if (has_valid_grip_data()) {
        return (front_grip + rear_grip) / 2.0;
    } else {
        // Combined friction circle approximation
        double slip_lat = calculate_slip_angle();
        double slip_long = calculate_slip_ratio();
        return sqrt(pow(slip_lat/OPT_LAT, 2) + pow(slip_long/OPT_LONG, 2));
    }
}
```

### Phase Integration (Anti-Click Technology)

All oscillating effects use phase accumulation to prevent discontinuities:

```cpp
// Correct phase integration
m_phase += frequency * dt * TWO_PI;
m_phase = fmod(m_phase, TWO_PI);  // Proper wrapping

// Incorrect (causes clicks)
double phase = time * frequency * TWO_PI;
```

### Hysteresis Systems

Multiple hysteresis loops prevent oscillation:

- **Missing Data Detection**: 20-frame threshold
- **Grip Transitions**: Prevents rapid toggling between states
- **Speed Gating**: Smooth on/off transitions

## Performance Characteristics

### Timing Requirements
- **400Hz Update Rate**: 2.5ms maximum per frame
- **Real-time Constraints**: No memory allocation in hot path
- **Thread Safety**: GUI and FFB threads communicate via lock-free buffers

### Optimization Strategies
- **Early Returns**: Invalid data rejected quickly
- **Lookup Tables**: Pre-computed sine/cosine values where applicable
- **Minimal Branching**: Effects disabled via multiplication, not conditionals
- **State Preservation**: History variables updated every frame to prevent spikes

## Configuration System

### Parameter Categories

**Base Settings:**
- `m_gain`: Master output scaling (0.1-2.0)
- `m_max_torque_ref`: Reference torque for normalization (5-50 Nm)
- `m_min_force`: Minimum output to overcome friction (0.01-0.1)

**Grip Effects:**
- `m_understeer_effect`: Understeer feedback strength (0-2.0)
- `m_sop_effect`: Seat of pants lateral G scaling (0-2.0)
- `m_sop_yaw_gain`: Yaw kick intensity (0-5.0)

**Texture Effects:**
- `m_road_texture_gain`: Road vibration amplitude (0-1.0)
- `m_slide_texture_enabled`: Enable slide rumble (bool)
- `m_lockup_enabled`: Enable brake lockup effects (bool)

**Advanced Tuning:**
- `m_gyro_gain`: Steering damper strength (0-2.0)
- `m_speed_gate_lower/upper`: Speed-based effect gating (0-20 m/s)
- `m_invert_force`: Force direction inversion (bool)

### Preset System

16 built-in presets optimized for different car types:
- **GT3/GT4**: Balanced settings for touring cars
- **LMP/Hypercar**: Smooth settings for high-speed endurance
- **GM DD 21Nm**: "Steering Shaft Purist" - minimal processing
- **Wheel-specific**: Tuned for different wheel torque ratings

## Diagnostic Capabilities

### Real-time Monitoring
- **FFBSnapshot**: 50+ debug values updated every frame
- **Channel Stats**: Min/max/average tracking for all force components
- **Warning System**: Automatic detection of missing telemetry data

### GUI Integration
- **Rolling Buffer Plots**: Real-time visualization of force components
- **Weight Distribution**: Live front/rear axle load ratios
- **Effect Isolation**: Individual effect toggles for tuning

## Strengths & Limitations

### Strengths
- **Physics-Based**: Effects derived from actual vehicle dynamics
- **Highly Configurable**: 50+ parameters for fine-tuning
- **Robust Fallbacks**: Graceful handling of missing telemetry
- **Real-time Performance**: Optimized for 400Hz operation
- **Cross-Platform**: Core engine works on Linux/Windows

### Limitations
- **Complexity**: Large parameter space can be overwhelming
- **Telemetry Dependent**: Quality limited by game data availability
- **Learning Curve**: Requires understanding of vehicle dynamics
- **Debugging Difficulty**: Interacting effects make isolation challenging

## Future Enhancement Opportunities

### Algorithm Improvements
- **Machine Learning**: Adaptive parameter tuning based on driver style
- **Advanced Grip Models**: More sophisticated tire physics simulation
- **Haptic Libraries**: Standardized effect patterns for common scenarios

### Architecture Enhancements
- **Plugin System**: User-extensible effect modules
- **Multi-threaded Processing**: Parallel effect calculation
- **GPU Acceleration**: FFT-based frequency analysis

### User Experience
- **Auto-tuning**: Automated parameter optimization
- **Effect Libraries**: Preset combinations for different scenarios
- **Real-time Analysis**: AI-powered setup recommendations

## Conclusion

The LMUFFB FFB Engine represents a sophisticated approach to force feedback generation, combining physics-based calculations with extensive configurability. Its modular architecture allows for precise tuning while maintaining real-time performance requirements. The integration of telemetry processing with robust fallback mechanisms ensures reliable operation across different game conditions and content types.