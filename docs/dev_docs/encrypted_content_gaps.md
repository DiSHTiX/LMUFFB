# Encrypted Content Gaps Documentation (v0.4.39)

**Date**: 2025-12-20  
**Status**: Documented (Not Yet Implemented)  
**Priority**: Low (Monitor for user feedback)

---

## Overview

This document tracks two potential gaps in the FFB implementation when running on **encrypted Le Mans Ultimate content** (DLC, Hypercars). These gaps were identified in the follow-up analysis after implementing the Kinematic Load Model (v0.4.39).

**Source**: `docs/dev_docs/Improving FFB App Tyres.md` - "Follow ups after first implementation"

---

## Background: Suspension Physics Packet

The LMU shared memory interface provides several suspension-related telemetry fields:
- `mSuspensionDeflection` (Spring compression)
- `mVerticalTireDeflection` (Tire compression)
- `mRideHeight` (Chassis height above ground)

**Key Risk**: These fields are part of the same **suspension physics packet**. If the game engine blocks `mSuspensionDeflection` on encrypted content, it is highly likely that `mVerticalTireDeflection` and `mRideHeight` are **also blocked** (all return 0.0).

---

## Gap A: Road Texture (`mVerticalTireDeflection`)

### Current Implementation

**File**: `FFBEngine.h` lines 1025-1044  
**Effect**: Road Texture (bumps, curbs)  
**Method**: High-pass filter on `mVerticalTireDeflection` delta

```cpp
double vert_l = fl.mVerticalTireDeflection;
double vert_r = fr.mVerticalTireDeflection;

double delta_l = vert_l - m_prev_vert_deflection[0];
double delta_r = vert_r - m_prev_vert_deflection[1];

double road_noise = (delta_l + delta_r) * 50.0 * m_road_texture_gain;
```

### Potential Issue

**If `mVerticalTireDeflection` is blocked (0.0)**:
- Delta = `0.0 - 0.0 = 0.0`
- Road noise = `0.0 * 50.0 = 0.0`
- **Result**: Road Texture will be **silent** - no bumps or curbs felt

### Impact

**Severity**: Medium  
**User Experience**: Loss of tactile feedback for road surface details

### Potential Solution

**Fallback to Vertical G-Force**:
```cpp
// Detect if deflection is blocked
bool deflection_blocked = (vert_l == 0.0 && vert_r == 0.0 && car_speed > 5.0);

if (deflection_blocked) {
    // Use Vertical G-Force (mLocalAccel.y) through high-pass filter
    double vert_accel = data->mLocalAccel.y;
    double delta_accel = vert_accel - m_prev_vert_accel;
    road_noise = delta_accel * ACCEL_TO_ROAD_SCALE * m_road_texture_gain;
}
```

### Status

- ✅ **Documented** in code (TODO comment added)
- ❌ **Not Implemented** (awaiting user feedback)
- 📊 **Monitoring**: No user reports of silent road texture on encrypted content

---

## Gap B: Bottoming Effect (`mRideHeight`)

### Current Implementation

**File**: `FFBEngine.h` lines 1046-1059  
**Effect**: Bottoming (scraping, suspension bottoming out)  
**Method A**: Scraping - triggers when `mRideHeight < 0.002m`

```cpp
if (m_bottoming_method == 0) {
    double min_rh = (std::min)(fl.mRideHeight, fr.mRideHeight);
    if (min_rh < 0.002 && min_rh > -1.0) {
        triggered = true;
        intensity = (0.002 - min_rh) / 0.002;
    }
}
```

### Potential Issue

**If `mRideHeight` is blocked (0.0)**:
- Check: `0.0 < 0.002` → **Always TRUE**
- Intensity: `(0.002 - 0.0) / 0.002 = 1.0` → **Maximum**
- **Result**: **Constant false positive** - permanent scraping vibration

### Impact

**Severity**: High (if it occurs)  
**User Experience**: Annoying permanent vibration, unusable

### Potential Solution

**Sanity Check for Blocked Data**:
```cpp
if (m_bottoming_method == 0) {
    double min_rh = (std::min)(fl.mRideHeight, fr.mRideHeight);
    
    // Sanity check: If exactly 0.0 while moving, data is blocked
    bool rh_blocked = (min_rh == 0.0 && car_speed > 5.0);
    
    if (rh_blocked) {
        // Fallback to Method B (Suspension Force Spike) or disable
        // Skip Method A to prevent false positive
    } else if (min_rh < 0.002 && min_rh > -1.0) {
        triggered = true;
        intensity = (0.002 - min_rh) / 0.002;
    }
}
```

### Status

- ✅ **Documented** in code (TODO comment added)
- ❌ **Not Implemented** (awaiting user feedback)
- 📊 **Monitoring**: No user reports of constant scraping on encrypted content

---

## Why Not Implemented Yet?

### Empirical Evidence

Testing on encrypted LMU content (Hypercars, DLC) shows:
- `mTireLoad` = 0.0 (blocked) ❌
- `mSuspForce` = Valid (not blocked) ✓
- `mVerticalTireDeflection` = **Unknown** (needs testing)
- `mRideHeight` = **Unknown** (needs testing)

**Hypothesis**: The game engine may block tire sensors (`mTireLoad`) but leave suspension sensors active for visual suspension animation.

### Priority Assessment

**Low Priority** because:
1. **No User Reports**: No complaints of silent road texture or constant scraping on encrypted content
2. **Workarounds Exist**: Users can disable effects if they malfunction
3. **Method B Available**: Bottoming has alternative detection method (Suspension Force Spike)
4. **Critical Path Protected**: Front load (most important) already has Kinematic Model fallback

---

## Implementation Trigger

**Implement if**:
1. User reports silent road texture on encrypted cars
2. User reports constant scraping vibration on encrypted cars
3. Telemetry logs show `mVerticalTireDeflection = 0.0` or `mRideHeight = 0.0` while moving

**Target Version**: v0.4.40 or later

---

## Testing Strategy

### Manual Testing (If Implementing)

1. **Load Encrypted Content**: LMU Hypercar or DLC car
2. **Enable Road Texture**: Set gain to 1.0
3. **Drive Over Curbs**: Verify vibration is present
4. **Enable Bottoming (Method A)**: Set gain to 1.0
5. **Drive Normally**: Verify no constant vibration
6. **Hit Curb Hard**: Verify bottoming triggers correctly

### Telemetry Logging

Add diagnostic logging to detect blocked data:
```cpp
// In calculate_force()
if (car_speed > 5.0) {
    if (fl.mVerticalTireDeflection == 0.0 && fr.mVerticalTireDeflection == 0.0) {
        std::cout << "[WARNING] mVerticalTireDeflection appears blocked (encrypted content?)" << std::endl;
    }
    if (fl.mRideHeight == 0.0 && fr.mRideHeight == 0.0) {
        std::cout << "[WARNING] mRideHeight appears blocked (encrypted content?)" << std::endl;
    }
}
```

---

## Related Files

- **Implementation**: `FFBEngine.h` lines 1025-1073
- **TODO Comments**: Added in v0.4.39
- **Analysis**: `docs/dev_docs/Improving FFB App Tyres.md`
- **This Document**: `docs/dev_docs/code_reviews/encrypted_content_gaps.md`

---

## Recommendations

### For Users (If Issues Occur)

**If Road Texture is Silent**:
1. Verify `Road Texture` is enabled and gain > 0
2. Try increasing gain to maximum
3. Report issue with car/track details

**If Constant Scraping Vibration**:
1. Switch Bottoming Method from A (Scraping) to B (Suspension Spike)
2. Or disable Bottoming effect temporarily
3. Report issue with car/track details

### For Developers

1. **Monitor Discord/Forums**: Watch for user reports on encrypted content
2. **Add Telemetry Logging**: In next version, log when suspension data appears blocked
3. **Implement Fallbacks**: If confirmed, implement solutions outlined above

---

**Document Version**: 1.0  
**Last Updated**: 2025-12-20  
**Status**: Monitoring - No action required unless user reports received
