# TINYPEDAL Hivemind Memory Corpus

> **Document Generated:** 2026-02-02
> **Project:** TinyPedal - Racing Simulation Overlay
> **Source:** `docs/tidypedal/FULL_PROJECT_CONTEXT.md`
> **Total Chunks:** 21 | **Total Memories:** 49

---

## Overview

This document catalogs the complete hivemind memory corpus for the TINYPEDAL project. The memories contain comprehensive documentation of TinyPedal's codebase, including widgets, modules, UI editors, configuration systems, and architecture patterns.

**Project Details:**
- **Language:** Python 3.8-3.10
- **GUI Framework:** PySide2/PySide6 (Qt)
- **Simulators Supported:** rFactor 2, Le Mans Ultimate
- **License:** GPL-3.0
- **Files:** 90+ Python source files
- **Widgets:** 40+ overlay components
- **Modules:** 10+ data processing modules

---

## Memory Index

### Research & Setup Chunks

| ID | Memory ID | Content Summary | Key Topics |
|----|-----------|-----------------|------------|
| Initial | `mem-94f6a48e7132e2c4` | AGENTS.md SOP, create_context.py script, environment discovery | SOP, hivemind, skills, CLI tools |
| Summary | `mem-f1968b970f446d99` | Complete research summary with project overview, architecture, findings | Project overview, architecture, ready-for-development |

### Configuration & Settings Chunks

| ID | Memory ID | Content Summary | Key Topics |
|----|-----------|-----------------|------------|
| Config 1 | `mem-d705ee22d8330ced` | Config sections: Update/UI preferences, compatibility, user paths, overlay options, units, spectate mode, pace notes playback, tools | Config, UI preferences, compatibility, units, spectate |
| Config 2 | `mem-ac0e1ba19abd1328` | Widget/module blocks: Brake temp/wear, Cruise, Brake/Track info editors, Tyre compounds, Heatmap editor, Track map, Pace notes, Pedal, Radar, Deltabest, DRS, Electric motor, Engine, Flag, Fuel, Gear | Widgets, modules, editors, brake, tyre, compounds |
| Config 3 | `mem-0aa6560231a3ea24` | Extended widget options: Relative, Brake, Cruise, Track notes, Tyre, Tyre temperature | Extended widgets, relative, track notes |
| Settings 1 | `mem-16651162340d9767` | Widget settings part 2: flag, force, friction_circle, fuel, fuel_energy_saver, gear, heading, instrument, laps_and_position, lap_time_history, navigation, p2p, pace_notes, pedal, pit_stop_estimate, radar, rake_angle | Widget settings, flag, force, fuel, gear, heading |
| Settings 2 | `mem-bc6f5b879814c185` | Widget settings part 3 + UI module: relative, relative_finish_order, ride_height, rivals, roll_angle, rpm_led, sectors, session, slip_ratio, speedometer, time_delta, tyre widgets, virtual_energy, weather, weight_distribution, wheel_camber/toe + UI palette/style classes | Widget settings, UI, palette, relative, weather |
| Templates | `mem-da56a890f4839367` | Wheels module generators, process functions (vehicle, weather), template settings for API, brakes, classes, common, compounds, filelock, global, heatmap, module, tracks, widgets | Templates, settings, process functions, wheels |

### Widget Configurations

| ID | Memory ID | Content Summary | Key Topics |
|----|-----------|-----------------|------------|
| Widgets | `mem-e45646c604f2acc3` | Changelog-like entries and widget updates (lines 3600-5317) | Changelog, widget updates |
| Widgets 2 | `mem-795754e2de8583e8` | More changelog entries across 2.61.0, 2.60.x versions and widget updates | Version history, 2.61.0, 2.60.x |
| Widgets 3 | `mem-9a9941da1dd7a60a` | Versioned history continuing into 2.2x+ | Version history, 2.2x |
| Widgets 4 | `mem-081e325325408d5d` | Changelog 2.0.0 through 1.x history | Legacy history, 1.x |

### Reader & Adapter Chunks

| ID | Memory ID | Content Summary | Key Topics |
|----|-----------|-----------------|------------|
| Readers 1 | `mem-d60a5a13894a228d` | Receiver classes for various readers and interfaces (chunk 11) | Receivers, interfaces |
| Readers 2 | `mem-b501f087fe0dd228` | Receiver classes continued (chunk 12) | Receivers, interfaces |
| RF2 Readers | `mem-5ff2685367644812` | RF2 Reader Classes: telemetry data parsing, vehicle mapping, API conversion, singleplayer/multiplayer support, error handling | RF2, telemetry, reader, API |

### Module Chunks

| ID | Memory ID | Content Summary | Key Topics |
|----|-----------|-----------------|------------|
| Modules 1 | `mem-1ddcc377ed81805f` | RF2 Vehicle & Wheel Reader Classes, REST API (RestAPIData, ResRawOutput, ResParOutput), Data modules: delta, energy, force | Vehicle, Wheel, REST API, modules, delta, energy, force |
| Modules 2 | `mem-64b6662169071208` | Map recording (MapCoords, MapRecorder), Notes (pace/track notes), Relative (standings, position calc), Sectors (sector times), Stats (driver stats), Vehicles (vehicle info) | Map, Notes, Relative, Sectors, Stats, Vehicles |
| Core Modules | `mem-220d95d6ef4edd9a` | Core modules: async_request, API connectors, Tinypedal main (chunk 9) | Core, async, API, main |
| Core 2 | `mem-1876969f999d5365` | main.py and api_connector completion (chunk 9b) | Main, API connector |
| Validators | `mem-5f72ac66bb719094` | validator.py and code completion (chunk 10) | Validators |

### UI Editor Chunks

| ID | Memory ID | Content Summary | Key Topics |
|----|-----------|-----------------|------------|
| UI Editors | `mem-5300b3c9e80c9f70` | Brake Editor, Fuel Calculator (comprehensive race strategy), Heatmap Editor | Brake Editor, Fuel Calculator, Heatmap Editor |
| UI Menus | `mem-3d6f6dd7f41fcb02` | UI Menus (File, API, Tools, Window, Help), ModuleList, NotifyBar, PaceNotesPlayer, SpectateList, TrackInfoEditor | UI, Menus, ModuleList, NotifyBar, PaceNotes |
| Final Widgets | `mem-09d352965052e117` | Weather widget (temperature, rain, wetness trends), Weather Forecast, Weight Distribution, Wheel Camber/Toe | Weather, Weight Distribution, Wheel Camber, Wheel Toe |

---

## Memory Statistics

```
Total Chunks:          21
Total Memories:        49
Source Document:       docs/tidypedal/FULL_PROJECT_CONTEXT.md (2.2MB)
Lines Covered:         ~57,360 lines
```

### Coverage by Area

| Area | Chunks | Coverage |
|------|--------|----------|
| Widgets | 5 | 40+ overlay widgets with full configuration |
| Modules | 5 | 10+ data processing modules |
| UI Editors | 3 | Brake, compound, vehicle, track, heatmap, pace notes |
| Configuration | 5 | Global settings, user paths, units, overlay, presets |
| Adapters | 2 | RF2 reader classes, REST API integration |
| Research | 2 | Project summary, AGENTS.md SOP |

---

## Usage Guide

### Searching the Corpus

```bash
# Search for widget configuration
hivemind_find(query="widget settings fuel configuration")

# Search for module information
hivemind_find(query="module delta energy force")

# Search for UI editor details
hivemind_find(query="brake editor fuel calculator heatmap")

# Search for API/REST integration
hivemind_find(query="REST API RF2 reader")
```

### Retrieving Specific Memory

```bash
# Get complete memory by ID
hivemind_get(id="mem-16651162340d9767")
```

### Memory Structure

Each memory contains:
- **Content:** Detailed documentation of the code/feature
- **Metadata:** Tags, confidence score, creation timestamp
- **Tags:** Topic classification for filtering

---

## Key Findings

### Architecture Overview

TinyPedal is structured as:

```
tinypedal/
├── widget/          # 40+ overlay widgets
├── module/          # 10+ data processing modules
├── ui/              # UI editors and dialogs
├── userfile/        # User data handlers
├── adapter/         # Sim API adapters (rf1, rf2, lmu)
├── setting/         # Configuration management
└── ...
```

### Core Components

**Adapters (Sim API Integration):**
- `rf1` - rFactor 1 legacy support
- `rf2` - rFactor 2 via Shared Memory Map Plugin
- `lmu` - Le Mans Ultimate via Shared Memory Map Plugin

**Data Processing Modules:**
- `delta` - Delta time calculation with EMA smoothing
- `energy` - Energy consumption for hybrid vehicles
- `force` - G-force tracking (lateral, longitudinal)
- `mapping` - Track map recording and SVG generation
- `notes` - Pace notes and track notes
- `relative` - Relative times between vehicles
- `sectors` - Sector times and delta best
- `stats` - Driver statistics with persistence
- `vehicles` - Vehicle info and lap tracking
- `wheels` - Wheel-specific calculations

**Widgets (40+ Overlay Components):**
- **Telemetry:** brake_temp, brake_wear, force, suspension, tyre_pressure/temperature/wear
- **Race Info:** deltabest, lap_time_history, sectors, timing, standings
- **Comparison:** relative, relative_finish_order, rivals, radar, trailing
- **Analysis:** driver_stats, fuel_calculator, pace_notes, track_map, weather_forecast
- **Navigation:** brake_bias, fuel, gear, heading, rpm_led, speedometer, steering

**UI Editors:**
- Brake Editor
- Tyre Compound Editor
- Vehicle Brand/Class Editors
- Track Info Editor
- Heatmap Editor
- Track Notes Editor
- Pace Notes Editor
- Preset Transfer

### Configuration System

- JSON-based presets in `settings/`
- Global config in OS-specific locations
- Heatmap presets for visualization
- Template-based widget settings
- User data files: JSON/CSV/SVG formats

---

## Development Workflow

### AGENTS.md SOP

1. **Consult Memory** - Use hivemind before work
2. **Update Documentation** - ALL relevant docs
3. **Store Learnings** - Use hivemind_store for findings
4. **Delivery** - No git pushes, just documentation

### Quick Start Commands

```bash
# Search memories
hivemind_find(query="widget fuel configuration")

# List all memories
hivemind_stats()
```

---

## Memory ID Reference Table

| Chunk | Memory ID | Type | Tags |
|-------|-----------|------|------|
| Initial | `mem-94f6a48e7132e2c4` | Research | tinypedal, research, agents, docs |
| Config 1 | `mem-d705ee22d8330ced` | Config | FULL_PROJECT_CONTEXT, chunk2, config |
| Config 2 | `mem-ac0e1ba19abd1328` | Widgets | FULL_PROJECT_CONTEXT, chunk3, widgets, modules |
| Config 3 | `mem-0aa6560231a3ea24` | Widgets | FULL_PROJECT_CONTEXT, chunk4, widgets, config |
| Widgets | `mem-e45646c604f2acc3` | Changelog | FULL_PROJECT_CONTEXT, chunk5 |
| Widgets 2 | `mem-795754e2de8583e8` | Changelog | FULL_PROJECT_CONTEXT, chunk6 |
| Widgets 3 | `mem-9a9941da1dd7a60a` | Changelog | FULL_PROJECT_CONTEXT, chunk7 |
| Widgets 4 | `mem-081e325325408d5d` | Changelog | FULL_PROJECT_CONTEXT, chunk8 |
| Core Modules | `mem-220d95d6ef4edd9a` | Modules | FULL_PROJECT_CONTEXT, chunk9 |
| Core 2 | `mem-1876969f999d5365` | Core | FULL_PROJECT_CONTEXT, chunk9b |
| Validators | `mem-5f72ac66bb719094` | Validators | FULL_PROJECT_CONTEXT, chunk10 |
| Readers 1 | `mem-d60a5a13894a228d` | Adapters | FULL_PROJECT_CONTEXT, chunk11 |
| Readers 2 | `mem-b501f087fe0dd228` | Adapters | FULL_PROJECT_CONTEXT, chunk12 |
| RF2 Readers | `mem-5ff2685367644812` | RF2 | FULL_PROJECT_CONTEXT, chunk13, RF2, reader-classes |
| Modules 1 | `mem-1ddcc377ed81805f` | Modules | FULL_PROJECT_CONTEXT, chunk14, Vehicle, Wheel, RESTAPI |
| Modules 2 | `mem-64b6662169071208` | Modules | FULL_PROJECT_CONTEXT, chunk15, MapRecorder, notes, relative |
| Templates | `mem-da56a890f4839367` | Settings | FULL_PROJECT_CONTEXT, chunk16, wheels, process, template |
| Settings 1 | `mem-16651162340d9767` | Widgets | FULL_PROJECT_CONTEXT, chunk17, widgets, flag, force, fuel |
| Settings 2 | `mem-bc6f5b879814c185` | Widgets | FULL_PROJECT_CONTEXT, chunk18, widgets, relative, ui |
| UI Editors | `mem-5300b3c9e80c9f70` | UI | FULL_PROJECT_CONTEXT, chunk19, ui, brake_editor, fuel_calculator |
| UI Menus | `mem-3d6f6dd7f41fcb02` | UI | FULL_PROJECT_CONTEXT, chunk20, ui, menus, PaceNotes |
| Final Widgets | `mem-09d352965052e117` | Widgets | FULL_PROJECT_CONTEXT, chunk21, widgets, weather, weight_distribution |
| Summary | `mem-f1968b970f446d99` | Summary | TinyPedal, research, complete, project, analysis |

---

## Related Files

| File | Path | Purpose |
|------|------|---------|
| FULL_PROJECT_CONTEXT.md | `docs/tidypedal/FULL_PROJECT_CONTEXT.md` | Source document (2.2MB) |
| customization.md | `docs/tidypedal/customization.md` | User customization guide |
| TIDYPEDAL_hivemind.md | `docs/tidypedal/TIDYPEDAL_hivemind.md` | Tidypedal hivemind memory map |

---

*Document auto-generated from hivemind memory corpus on 2026-02-02*
