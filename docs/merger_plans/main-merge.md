# Merger Plan: main-merge off main

This document captures the current strategy to integrate TelemetryProcessor relocation and v0.7 features into the main branch, with conflict-aware adoption and a release-ready PR plan.

## 1) Context and Goals
- Move TelemetryProcessor.h from include/ to src/ and align all references
- Consolidate v0.7 environmental features (weather, terrain, compound, filters) into main
- Gate Windows/Linux differences (Windows dependencies present in tests; guard Linux CI)
- Produce a clean, well-documented PR with a clear diff and rationale

## 2) Current State (as of this plan)
- TelemetryProcessor relocated: src/TelemetryProcessor.h (canonical) with includes updated
- Folders touched: src/FFBEngine.h, src/Config.h, tests/test_telemetry_processor.cpp, docs, changelog
- Branch: main-merge created off main

## 3) Conflicts and Divergences (surface area to resolve)
- Include paths: ensure all callers reference src/TelemetryProcessor.h
- Env feature wiring: align weather/terrain/compound/filter changes so there is a single coherent implementation in main
- Test guards: Windows-only tests guarded for Linux CI

## 4) Adoption Strategy (how to resolve conflicts)
- Adopt canonical header: prefer src/TelemetryProcessor.h across all callers
- Merge v0.7.0 env features from TelemetryProcessor plan into main once a single coherent version exists
- If main already has overlapping changes, port the strongest/most complete implementation into main-merge
- De-duplicate by removing include/TelemetryProcessor.h remnants from the codebase

## 5) Branch Plan and PR plan
- Create a single PR from main-merge into main with a comprehensive diff summary
- Provide a short rationale: relocation + v0.7 features in one coherent update
- Gate Windows-only tests: keep guarded on Linux CI, but ensure Windows CI sees the full set

## 6) Risk and Mitigations
- Risk: conflicts across FFBEngine.h and Config.h
- Mitigation: prefer the most complete, centralized environment feature wiring; guard Windows-specific code behind #ifdef _WIN32
- Risk: Linux CI failing due to Windows headers
- Mitigation: mark Windows tests to be skipped on Linux CI; keep a dedicated Windows CI path

## 7) Bead Mapping (current tasks)
- Prepare merge branch from main
- Identify conflicts and divergences
- Adopt canonical TelemetryProcessor header location
- Consolidate v0.7.0 env features into main
- Guard Linux CI tests (Windows dependencies)
- Prepare PR plan and documentation

## 8) Diffs and Review Checklist
- Ensure TelemetryProcessor relocation is reflected in:
  - src/FFBEngine.h includes
  - tests/test_telemetry_processor.cpp includes
  - docs references and CHANGELOG
- Validate v0.7 env features are wired consistently in src/FFBEngine.h and src/Config.h
- Confirm Linux CI tests are guarded for Windows-only tests

## 9) Active Beads (current)
- cell--venk8-ml9xoreskb1: Prepare merge branch from main
- cell--venk8-ml9xorey976: Identify conflicts and divergences
- cell--venk8-ml9xorf4tyk: Adopt canonical TelemetryProcessor header location
- cell--venk8-ml9xorfkg2s: Prepare PR plan and documentation
- (Consolidate env features) planned bead
- (Guard Linux CI tests) planned bead

## 10) Resume Prompt

- Continue with the merge preparation by implementing the adoption of the canonical header location and resolving conflicts.
- Next steps: surface diffs, create a unified patch, and prepare the PR description.

---

End of merger plan draft.
