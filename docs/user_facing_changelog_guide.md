# User-Facing Changelog Guide

## Purpose

This document provides guidelines for creating entries in the **User-Facing Changelog** (currently `Version Releases (user facing changelog).md`). This changelog is designed for end users and emphasizes understandable, benefit-focused release notes, while the technical `CHANGELOG.md` serves developers and power users.

## Core Principles

### 1. User-First Language
- **Avoid**: Technical jargon, implementation details, class names, file paths
- **Use**: Clear descriptions of what changed from the user's perspective
- **Example**:
  - ❌ "Refactored `FFBEngine::calculate_force` with context-based processing"
  - ✅ "Improved performance and stability of force feedback calculations"

### 2. Focus on Benefits
Explain HOW the change helps the user, not just WHAT changed.

**Example**:
- ❌ "Added Auto-Connect to LMU"
- ✅ "**Auto-Connect to LMU**: The app now automatically connects every 2 seconds, eliminating the need to click 'Retry' manually. Status shows 'Connecting...' in yellow while searching and 'Connected' in green when active."

### 3user Relevance First
Include changes that users will:
- **Notice** during normal use
- **Configure** via UI controls
- **Benefit from** (fixes, improvements, new features)

**Exclude** or minimize:
- Internal refactoring (unless it improves performance/stability noticeably)
- Test suite additions
- Developer documentation updates
- Code quality improvements (unless they fix bugs users encountered)

### 4. Be Concise
- Keep entries brief but informative
- Use bullet points for multiple changes
- Group related changes together
- Limit to 2-4 sentences per feature

## Entry Structure

### Version Header
```markdown
## [Version] - Date
**[Optional: Special Thanks or Theme]**
```

**Example**:
```markdown
## Version 0.6.39 - January 31, 2026
**Special Thanks** to @AndersHogqvist for the Auto-connect feature!
```

### Change Categories

Use these categories in order of user impact:

1. **Added** - New features users can see/use
2. **Fixed** - Bug fixes that improve user experience
3. **Changed** - Modified behavior users will notice
4. **Improved** - Performance/quality enhancements

### Entry Format

```markdown
### [Category]
- **[Feature Name]**: [1-2 sentence description focusing on user benefit]
  - [Optional: Key detail or usage tip]
  - [Optional: Setting location or how to use]
```

## Writing Guidelines

### DO ✅

- **Start with the benefit**: "Fixed vibrations when stationary" instead of "Implemented speed gate"
- **Mention UI locations**: "Added new sliders in Advanced Settings"
- **Include defaults**: "Now defaults to 18 km/h (previously 10 km/h)"
- **Provide context**: "This fixes the shaking wheel in the pits"
- **Group related items**: Combine multiple slider additions into one entry
- **Use bold for names**: **Auto-Connect**, **Speed Gate**, **ABS Pulse**

### DON'T ❌

- **List every test added**: Users don't care about `test_speed_gate_custom_thresholds()`
- **Quote code**: Avoid `m_prev_vert_accel` or `FFBEngine.h`
- **Over-explain internals**: "Uses std::fmod for phase wrapping" → "Fixed stuttering during vibration effects"
- **Include file paths**: `src/lmu_sm_interface/SafeSharedMemoryLock.h`
- **List every config parameter**: Just mention the feature, not `speed_gate_lower` and `speed_gate_upper` separately

## Length Guidelines

- **Major version** (0.x.0): 4-8 bullet points
- **Minor version** (0.6.x): 2-6 bullet points
- **Patch version** (0.6.x, bug fix only): 1-3 bullet points

**Target**: 50-150 words per version entry

## Examples

### Good Entry ✅

```markdown
## Version 0.6.22 - December 28, 2025

### Fixed
- **Vibrations While Stationary**: Fixed the wheel shaking issue when sitting in the pits or at low speeds. Vibration effects now automatically fade out below 2 m/s and steering torque is smoothed below 3 m/s to eliminate engine rumble.
- **Road Texture on Encrypted Cars**: Added fallback detection for DLC cars where suspension data is blocked, now using vertical acceleration to ensure you can still feel bumps and curbs.
```

**Why it's good**:
- Focuses on user-facing problem ("wheel shaking in pits")
- Explains the solution clearly
- No code or technical details
- Mentions what users will notice ("fade out", "smoothed")

### Bad Entry ❌

```markdown
## Version 0.6.22 - December 28, 2025

### Added
- Implemented a dynamic Low Pass Filter (LPF) for the steering shaft torque with automatic smoothing at car speed < 3.0 m/s
- Road texture fallback mechanism using mLocalAccel.y when mVerticalTireDeflection is missing
- test_stationary_gate() and updated test_missing_telemetry_warnings()

### Changed
- Updated warning to include "(Likely Encrypted/DLC Content)"
```

**Why it's bad**:
- Technical jargon ("LPF", "mLocalAccel.y")
- Lists test additions (irrelevant to users)
- Doesn't explain user benefit
- No mention of what problem it solves

## Special Cases

### Breaking Changes
Clearly mark and explain migration:

```markdown
### Changed
- **⚠️ BREAKING: Understeer Effect Range**: The slider now uses 0.0-2.0 instead of 0-200. Old values are automatically converted (50.0 → 0.5). See new scale guide in tooltip.
```

### Community Contributions
Always credit:

```markdown
**Special Thanks** to @DiSHTiX for the icon implementation!
```

### Critical Fixes
Use emphasis:

```markdown
### Fixed
- **CRITICAL**: Fixed wheel staying locked at last force when pausing the game, which could cause injury. The wheel now immediately releases when entering menus.
```

## Review Checklist

Before finalizing an entry, ask:

- [ ] Would a non-technical user understand this?
- [ ] Have I explained the benefit, not just the change?
- [ ] Is it concise (no walls of text)?
- [ ] Did I avoid code/file references?
- [ ] Did I group related changes?
- [ ] Is it something users will actually notice?

## Versioning Strategy

### When to Include in User-Facing Changelog

| Change Type | Include? | Rationale |
|-------------|----------|-----------|
| New GUI feature/slider | ✅ Yes | Users see and use it |
| Bug fix (user-reported) | ✅ Yes | Solves user pain point |
| Performance improvement (noticeable) | ✅ Yes | Users feel the difference |
| New preset | ✅ Yes | Users can select it |
| Refactoring (no behavior change) | ❌ No | Users won't notice |
| Test additions | ❌ No | Developer-only |
| Documentation (dev docs) | ❌ No | Not user-facing |
| Code review fixes | ❌ Maybe | Only if it fixes a user-visible bug |

## Version Entry Template

```markdown
## Version [X.YZ] - [Month Day, Year]
**Special Thanks** to [Contributors] for [Contribution]!

### Added
- **[Feature Name]**: [User benefit description]

### Fixed
- **[Issue Description]**: [What was wrong and how it's now better]

### Changed
- **[Feature Name]**: [What changed and why users care]

### Improved
- **[Area]**: [Performance/quality improvement users will feel]
```

---

## Conversion Example: Technical → User-Facing

**From CHANGELOG.md** (Technical):
```markdown
### Refactored
- **GameConnector Lifecycle**:
  - Introduced `Disconnect()` method to centralize resource cleanup
  - Fixed potential resource leaks in `TryConnect()` 
  - Updated `IsConnected()` with double-checked locking pattern
  - Process Handle Robustness: Connection succeeds even if window handle unavailable
  - Updated destructor to ensure handles properly closed
```

**To User-Facing Changelog**:
```markdown
### Improved
- **Connection Reliability**: Fixed connection issues and resource leaks that could cause the app to not detect the game properly. Connection is now more robust even if the game window isn't fully loaded yet.
```

**Changes made**:
- Removed method names and technical details
- Focused on user benefit: "more reliable connection"
- Combined multiple technical points into one user-facing benefit
- Explained in terms users understand: "game not detected properly"

