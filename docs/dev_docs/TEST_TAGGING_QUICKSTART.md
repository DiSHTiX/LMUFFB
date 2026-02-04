# Test Tagging System - Quick Start

## Running Tests with Tags

### Run All Tests (Default)
```powershell
.\build\tests\Release\run_combined_tests.exe
```

### Run Specific Tags
```powershell
# Run only physics tests
.\build\tests\Release\run_combined_tests.exe --tag=Physics

# Run physics and regression tests
.\build\tests\Release\run_combined_tests.exe --tag=Physics,Regression

# Run only slope detection tests
.\build\tests\Release\run_combined_tests.exe --tag=Slope
```

### Exclude Tags
```powershell
# Run all tests except performance tests
.\build\tests\Release\run_combined_tests.exe --exclude=Performance
```

### Run by Category
```powershell
# Run only core physics category
.\build\tests\Release\run_combined_tests.exe --category=CorePhysics

# Run multiple categories
.\build\tests\Release\run_combined_tests.exe --category=CorePhysics,SlipGrip
```

### Get Help
```powershell
.\build\tests\Release\run_combined_tests.exe --help
```

## Available Tags

### Functional Tags
- `Physics` - Core physics calculations
- `Math` - Mathematical helpers
- `Integration` - Multi-component tests
- `Config` - Configuration and persistence
- `Regression` - Bug fix regression tests
- `Edge` - Edge cases and boundaries
- `Performance` - Stress and stability tests

### Component Tags
- `SoP` - Self-Aligning Torque
- `Slope` - Slope detection
- `Texture` - Haptic textures
- `Grip` - Grip calculation
- `Coordinates` - Coordinate systems
- `Smoothing` - Filtering algorithms

## Available Categories
- `CorePhysics`
- `SlipGrip`
- `Understeer`
- `SlopeDetection`
- `Texture`
- `YawGyro`
- `Coordinates`
- `Config`
- `SpeedGate`
- `Internal`

## Common Use Cases

### Pre-Commit Quick Check
```powershell
# Fast smoke test (~30% of tests)
.\build\tests\Release\run_combined_tests.exe --tag=Physics,Regression
```

### Working on Slope Detection
```powershell
# Run only slope-related tests
.\build\tests\Release\run_combined_tests.exe --tag=Slope
```

### Configuration Changes
```powershell
# Test config and integration
.\build\tests\Release\run_combined_tests.exe --tag=Config,Integration
```

### Performance Validation
```powershell
# Run stress tests only
.\build\tests\Release\run_combined_tests.exe --tag=Performance
```

## Full Documentation

See `docs/dev_docs/test_tagging_system.md` for complete documentation.

## Implementation Status

**Current Status:** Infrastructure ready, tags being added incrementally

The tagging infrastructure is complete and functional. Tags are being added to test functions progressively. Tests without tags will still run when no filters are specified.

To add tags to a test, update the test output message:
```cpp
std::cout << "\nTest: SoP Effect [Physics][SoP]" << std::endl;
```

Then register the tags in the runner function using `ShouldRunTest()`.
