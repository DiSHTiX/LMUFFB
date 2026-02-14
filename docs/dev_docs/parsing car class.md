The car class information is retrieved from the Shared Memory Interface (which LMU inherits from rFactor 2).

Specifically, the code pulls from:

Buffer: g_localData.scoring
Struct: 
VehicleScoringInfoV01
 (retrieved using the player's index playerVehicleIdx).
Field: mVehicleClass (a char[32] array).

This field is generally reliable as it is defined in the .veh file of the car. However, LMU has several ways it might name things depending on the series (WEC vs. ELMS).


2. Analysis of the Current Implementation

Potential Issues Identified:

* Strict Strings for LMP2: The current code looks for "LMP2 (WEC)" or "LMP2 (ELMS)". If the game simply returns "LMP2", it will fail all checks and default to 4000.0 (GT3 levels), causing prototyping cars to feel overshot and "clippy" at high speeds.

* Case Sensitivity: std::string::find is case-sensitive. If the game returns "hypercar" (lowercase) or "LMGT3", it might fail some checks.

* Default Fallback: The default of 4000.0 is a bit low for modern prototypes. If identification fails, we might want a more "middle-ground" safety value.

3. Suggestions for Robustness


I recommend the following changes to make the identification more resilient:

A. Make Strings Case-Insensitive and Robust
Convert the input string to uppercase before matching and use more hierarchical matching for the LMP2 variants.

B. Hierarchical Matching
Check for the broad category first (LMP2), then try to specialize if descriptors like ELMS are present.

C. Secondary Fallback (Vehicle Name)
As a safety net, if the class comes back as "Unknown" or empty, we can perform a quick check on the mVehicleName (e.g., searching for "499P" or "963" to identify a Hypercar).

Use additional names:

### **Vehicle Name to Class Mapping**

| If `mVehicleName` contains... | Infer Class | Seed Load (`m_auto_peak_load`) |
| :---- | :---- | :---- |
| `499P`, `GR010`, `963`, `9X8`, `V-SERIES.R`, `SCG 007`, `GLICKENHAUS`, `VANWALL`, `A424`, `SC63`, `VALKYRIE`, `M HYBRID`, `TIPO 6`, `680` | Hypercar | 9500.0 N |
| `ORECA`,  07 | LMP2 | 7500.0 N (WEC/Default) |
| `LIGIER`, `GINETTA`, `DUQUEINE`, `P320`, `P325`, `G61`, `D09` | LMP3 | 5800.0 N |
| `RSR-19`, `488 GTE`, `C8.R`, `VANTAGE AMR` | GTE | 5500.0 N |
| `LMGT3`, `296 GT3`, `M4 GT3`, `Z06 GT3`, `HURACAN`, `RC F`, `720S`, `MUSTANG` | GT3 | 4800.0 N  |



### **Le Mans Ultimate Car List​**

| Car | Class | Championship | Seasons | Base game or DLC? |
| ----- | ----- | ----- | ----- | ----- |
| Alpine A424 | Hypercar (LMDh) | WEC | 2024-2026 | 2024 Pack 2 |
| Aston Martin Valkyrie AMR-LMH | Hypercar (LMH) | WEC | 2025 | Base game |
| Aston Martin Vantage AMR | GTE | WEC | 2023 | Base game |
| Aston Martin Vantage AMR LMGT3 Evo | LMGT3 | WEC, ELMS | 2024-2026 | 2024 Pack 4 |
| BMW M Hybrid V8 | Hypercar (LMDh) | WEC | 2024-2026 | Base game |
| BMW M4 LMGT3 Evo | LMGT3 | WEC | 2024-2026 | 2024 Pack 3 |
| Cadillac V-Series.R | Hypercar (LMDh) | WEC | 2023-2026 | Base game |
| Chevrolet Corvette C8.R | GTE | WEC | 2023 | Base game |
| Chevrolet Corvette Z06 LMGT3.R | LMGT3 | WEC, ELMS | 2024-2026 | 2024 Pack 3 |
| Duqueine D09\* | LMP3 | ELMS | 2025-2026 | ELMS Pack 3 |
| Ferrari 296 LMGT3 | LMGT3 | WEC, ELMS | 2024-2026 | 2024 Pack 3 |
| Ferrari 488 GTE Evo | GTE | WEC | 2023 | Base game |
| Ferrari 499P | Hypercar (LMH) | WEC | 2023-2026 | Base game |
| Ford Mustang LMGT3 | LMGT3 | WEC | 2024-2026 | Base game |
| Ginetta G61-LT-P325 Evo | LMP3 | ELMS | 2025 | ELMS Pack 2 |
| Glickenhaus SCG 007 | Hypercar (LMH) | WEC | 2023 | Base game |
| Isotta Fraschini Tipo 6 | Hypercar (LMH) | WEC | 2024 | 2024 Pack 2 |
| Lamborghini Huracán LMGT3 Evo II | LMGT3 | WEC, ELMS | 2024-2025 | 2024 Pack 5 |
| Lamborghini SC63 | Hypercar (LMDh) | WEC | 2024 | 2024 Pack 1 |
| Lexus RC F LMGT3 | LMGT3 | WEC | 2024-2026 | 2024 Pack 5 |
| Ligier JS P325 | LMP3 | ELMS | 2025-2026 | ELMS Pack 1 |
| McLaren 720S LMGT3 Evo | LMGT3 | WEC, ELMS | 2024-2026 | Base game |
| Mercedes-AMG LMGT3 Evo | LMGT3 | WEC, ELMS | 2025-2026 | Base game |
| Oreca 07 | LMP2 | WEC | 2023-2026 | Base game |
| Oreca 07 (derestricted) | LMP2 | ELMS | 2025-2026 | Base game |
| Peugeot 9X8 | Hypercar (LMH) | WEC | 2023 | Base game |
| Peugeot 9X8 2024 | Hypercar (LMH) | WEC | 2024-2026 | 2024 Pack 1 |
| Porsche 911 LMGT3 R (992) | LMGT3 | WEC, ELMS | 2024-2026 | 2024 Pack 1 |
| Porsche 911 RSR-19 | GTE | WEC | 2023 | Base game |
| Porsche 963 | Hypercar (LMDh) | WEC | 2023-2025 | Base game |
| Toyota GR010 Hybrid | Hypercar (LMH) | WEC | 2023-2025 | Base game |
| Vanwall-Vandervell 680 | Hypercar (LMH) | WEC | 2023 | Base game |

\*not released yet

Note that while some cars have the 2026 listed, the season is not actually in *LMU* yet. These cars are on the entry lists in WEC and/or ELMS in 2026, however, so expect their liveries and potential updates to arrive in *Le Mans Ultimate* eventually.