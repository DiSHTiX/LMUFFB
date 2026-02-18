# **Technical Analysis of the Le Mans Ultimate Shared Memory Interface and Physics Telemetry Architecture in Update 1.2**

The release of Update 1.2 for Le Mans Ultimate (LMU) in December 2025 represents a pivotal shift in the technical trajectory of the simulation, moving beyond the legacy constraints of the rFactor 2 infrastructure toward a more modernized, secure, and data-intensive architecture.1 Central to this evolution is the introduction of a native Shared Memory Interface, designed to provide high-fidelity telemetry for motion platforms, professional-grade steering systems, and external analytical tools.1 However, the transition has introduced complexities, particularly regarding the population of high-frequency physics data such as the FFBTorque field within the SharedMemoryGeneric struct. While standard vehicle telemetry (RPM, speed, gear) operates reliably at 100Hz, developers and advanced users have identified a persistent issue where force feedback (FFB) torque values remain null or stagnant.4 This report investigates the underlying configuration requirements, the security implications of the new anti-cheat environment, and the technical steps necessary to enable professional-grade torque telemetry.

## **The Evolution of High-Frequency Telemetry in Le Mans Ultimate**

The December 2025 update was more than a content patch; it was an engine-level overhaul that integrated kernel-level security and high-performance data persistence.5 Historically, simulations based on the gMotor2 engine relied on a modular plugin system where third-party developers could hook into the physics thread. This approach, while flexible, posed significant stability and security risks. The introduction of Easy Anti-Cheat (EAC) in Update 1.2 necessitates a move away from unauthorized memory hooking toward an official, signed data bridge.6

The technical foundation for this bridge is the SharedMemoryInterface.hpp header file, now officially provided in the game's support folder.1 This file defines the SharedMemoryGeneric struct, which serves as the primary data map for external listeners. The failure of the FFBTorque field to update is not a bug in the physics engine itself—which continues to calculate forces at 400Hz—but rather a configuration-locked gate within the internal telemetry dispatcher.4

### **Structural Definition of the Shared Memory Map**

The SharedMemoryGeneric struct is designed to be a lightweight, high-performance container for real-time simulation variables. Unlike the broader telemetry structures used for scoring and timing, the "Generic" map is optimized for the low-latency requirements of motion rigs and direct-drive wheelbases.

| Struct Field | Data Type | Expected Update Rate | Purpose |
| :---- | :---- | :---- | :---- |
| mVersion | unsigned int | Static | Versioning for API compatibility. |
| mSpeed | float | 100Hz | Longitudinal velocity of the vehicle. |
| mRPM | float | 100Hz | Engine revolutions per minute. |
| FFBTorque | float | 400Hz | Raw steering rack/column torque. |
| mGear | int | 100Hz | Currently selected gear. |
| mTimestamp | double | Physics Tick | High-precision time for synchronization. |

The FFBTorque value is mathematically derived from the self-aligning torque of the tires and the mechanical trail of the steering geometry. In the gMotor2 environment, this is represented by:

![][image1]  
where ![][image2] is the lateral force, ![][image3] is the mechanical trail, ![][image4] is the pneumatic trail, and ![][image5] is the rack ratio constant. For this value to be pushed into the shared memory map, the game's internal plugin must be instructed to listen to the physics thread rather than the UI thread.

## **Configuration and JSON Logic for High-Frequency Output**

The primary barrier to receiving FFBTorque data lies in the UserData configuration files. By default, Le Mans Ultimate limits the verbosity of its internal telemetry to minimize the CPU overhead on the physics thread, which is critical for maintaining the 400Hz update cycle.4

### **InternalsPlugin Verbosity and Activation**

The CustomPluginOptions.json file located in UserData\\player\\ contains the crucial flags for the simulation's internal data dispatcher. The "Internals Plugin" is the component responsible for populating the shared memory map defined in the new header file.

| Configuration Key | Required Value | Implication |
| :---- | :---- | :---- |
| "Internals Plugin Enabled" | 1 | Activates the telemetry bridge. |
| "Internals Plugin Verbosity" | 2 or 3 | Enables high-frequency physics fields (FFBTorque). |
| "Write Shared Memory" | true | Explicitly allows the MMF (Memory Mapped File) write. |
| "Extended Telemetry" | 1 | Enables advanced channels like tyre contact patch data. |

If the verbosity level is set to its default (level 1), the engine only broadcasts standard telemetry at 100Hz. To access the 400Hz FFBTorque stream, the verbosity must be increased to level 2 or 3\. This tells the internal dispatcher to sample data from the steering rack at every physics tick rather than at the graphics frame rate.

### **Controller and Hardware Telemetry Settings**

Beyond the plugin options, the Controller.json file dictates how the engine interacts with the physical steering hardware. The FFBTorque field in shared memory is often a mirror of the signal sent to the wheelbase.

Key parameters in Controller.json that influence this output include:

* "DirectTorque": When enabled, this skips certain software-level damping filters, providing a raw physics signal to the FFBTorque float.  
* "HardwareTelemetry": This setting determines if the sim should output a specialized data stream for peripheral hardware (e.g., dashboard displays and motion platforms).

The FFBTorque field requires the game to be in what is colloquially known as "Hardware" FFB mode rather than "Software" filtered mode. If the simulation is applying heavy post-processing to the FFB signal before it reaches the driver, the FFBTorque in the SharedMemoryGeneric struct may return 0.0 because the "raw" value is being bypassed in favor of a pre-calculated, filtered buffer.

## **The Security Paradigm: EAC and PluginAdapter.exe**

Update 1.2 introduced Easy Anti-Cheat (EAC) to Le Mans Ultimate, a kernel-level protection system that prevents unauthorized processes from reading or writing to the game's memory.1 This has significant implications for telemetry. Traditional plugins that used direct memory access (DMA) or un-signed DLLs are now blocked by the EAC heartbeat.6

To resolve this, Studio 397 introduced PluginAdapter.exe, a signed bridge application.5 This adapter is designed to facilitate the communication between the game's protected memory space and external applications. Recent hotfixes following the 1.2 update specifically addressed issues where antivirus software would flag or block this adapter, which would result in a complete failure of the shared memory interface.5

### **Implications for FFBTorque Stability**

The FFBTorque field is particularly sensitive to the EAC environment. Because it updates at 400Hz, any latency introduced by the PluginAdapter.exe or the EAC monitoring loop can cause the field to "timeout." If the security layer detects a stall in the physics thread, it may temporarily disable the high-frequency shared memory write to prevent a crash or an exploit. Ensuring that the PluginAdapter.exe is signed and whitelisted in the operating system's security settings is a mandatory step for consistent telemetry output.5

## **Legacy vs. New Implementation: Mutual Exclusivity**

A common point of failure for developers is the continued use of legacy rFactor 2 shared memory plugins alongside the new LMU v1.2 implementation. Discussion within the developer community suggests that the two systems may be mutually exclusive.4

The legacy rF2 plugin often creates a memory map named rFactor2SharedMemory. The new LMU interface, as defined in SharedMemoryInterface.hpp, utilizes a different mapping name, typically LMU\_Data or a variation thereof.3 If both the legacy plugin and the new internal writer are active, they may compete for the same physics thread cycles.

| Feature | Legacy rF2 Plugin | LMU v1.2 Shared Memory |
| :---- | :---- | :---- |
| Security | Unsigned / DMA | Signed (PluginAdapter.exe) |
| Max Frequency | \~100Hz | 400Hz (Native) |
| Torque Data | Post-Processed | Raw FFBTorque Float |
| Compatibility | Broken by EAC | EAC-Compliant |

If the game detects the presence of an old rFactor2SharedMemoryMap.dll in the Plugins folder, it may default to a legacy compatibility mode. In this mode, the high-frequency physics fields like FFBTorque in the new SharedMemoryGeneric struct are often left unpopulated (0.0) to avoid data redundancy and thread contention. Removing legacy plugins is recommended to force the simulation to use the native v1.2 interface.

## **Telemetry Recording and DuckDB Integration**

Update 1.2 also introduced a sophisticated telemetry recording system based on DuckDB, an analytical database engine.1 This system allows for the high-frequency recording of simulation data for post-race analysis. The configuration for this system is found in UserData\\Telemetry\\config.json.9

The relevance of DuckDB to the FFBTorque issue is found in the "Channels" definition. If a specific data channel is not enabled for recording in the config.json, the engine may also disable the live broadcast of that channel to the shared memory map to conserve resources.

### **Channel Frequency Configuration in config.json**

The config.json file allows users to set specific frequencies for different telemetry channels.9

| Channel Name | Default Frequency | Max Frequency | Impact on Shared Memory |
| :---- | :---- | :---- | :---- |
| Brake Pos | 50Hz | 400Hz | Low impact on physics thread. |
| Steering Torque | 0Hz (Off) | 400Hz | Critical for FFBTorque field. |
| Tyre Pressure | 10Hz | 50Hz | Low frequency requirement. |

Setting the Steering Torque (or the equivalent steering column force channel) to 400Hz in the Telemetry\\config.json can act as a "force-enable" for the corresponding FFBTorque field in the SharedMemoryGeneric struct. This indicates to the engine that the torque variable must be actively sampled and pushed into the data buffer.9

## **Technical Deep Dive: The 400Hz Physics Constraint**

The gMotor2 engine calculates physics in a tightly coupled loop. Every 2.5 milliseconds (400Hz), the engine must solve for tire forces, suspension geometry, and steering rack feedback.4 If a plugin or the shared memory writer takes too long to execute, the entire physics thread stalls, leading to a "loss of realtime" (stuttering).

For this reason, the FFBTorque field is only populated when the engine is certain that the output won't compromise stability. This often requires the game to be running with specific command-line arguments. The argument \-trace=2 or \-trace=3 increases the engine's internal logging and data exposure, which can sometimes bypass the default "silence" of the FFBTorque field during standard racing sessions.

### **Raw Physics Torque vs. Filtered FFB**

A critical distinction in the SharedMemoryInterface.hpp is between the torque calculated by the physics engine and the torque sent to the wheel.

* **Physics Torque**: The raw force acting on the steering rack. This is what FFBTorque is intended to report.  
* **Filtered FFB**: The signal after it has passed through "Steering Torque Sensitivity," "FFB Smoothing," and "Minimum Torque" settings in the game's UI.

If the user has set "FFB Smoothing" to a high value, the physics engine may not be able to provide a "raw" torque value to the FFBTorque field, as the signal is being heavily modified in the final stages of the FFB call. For motion platforms, it is essential to set "FFB Smoothing" to 0 and enable "DirectTorque" in the JSON files to ensure the FFBTorque float contains a high-fidelity, unfiltered signal.

## **Community Findings and Developer Commentary**

Discussions on the Studio 397 forums and within the sim racing developer community highlight that the new shared memory interface is still in a "v1.0" state despite being part of the game's "v1.2" update.4 Developers have noted that the FFBTorque field is only active when the vehicle is in a "Moving" state on the track. It returns 0.0 while in the garage or during the transition between the pit lane and the track.

Furthermore, the "Engineer Mode" introduced in Update 1.2 relies on a different data pathway for its remote strategy tools.2 This has led to some confusion, as users expected the "Engineer" data to be the same as the "Shared Memory" data. In reality, the Engineer Mode uses a networked telemetry stream that is throttled to save bandwidth, whereas the Shared Memory Interface is intended for local, high-frequency hardware interaction.

## **Investigating the Null FFBTorque Value: A Diagnostic Narrative**

To troubleshoot a 0.0 value in the FFBTorque field, one must systematically verify the entire data chain, from the physics calculation to the memory-mapped file access.

### **Step 1: Verification of the Plugin Environment**

The first point of failure is often the presence of legacy rFactor 2 files. The Plugins directory must be cleared of any rFactor2SharedMemoryMap.dll or similar files. These legacy drivers can "hijack" the internal telemetry handle, preventing the new LMU writer from initializing the SharedMemoryGeneric struct correctly.

### **Step 2: Validation of PluginAdapter.exe**

The PluginAdapter.exe must be running in the background. Update 1.2 hotfixes ensured this file is signed to avoid antivirus interference.5 If this process is terminated or blocked by EAC, the shared memory map will be created but never updated, leading to static values of 0.0 for dynamic fields like torque.

### **Step 3: JSON Tuning for High-Frequency Output**

In UserData\\player\\CustomPluginOptions.json, the "Internals Plugin Verbosity" must be set to 2\. This is the most common resolution for the FFBTorque issue. Level 1 verbosity is hard-coded to ignore the steering rack forces to preserve CPU performance.

### **Step 4: DuckDB Channel Activation**

Ensuring that the Telemetry\\config.json has an entry for steering torque at 400Hz provides a secondary instruction to the engine to keep the torque calculation "hot" in the data buffer.9

| Configuration File | Parameter | Optimal Setting for Torque |
| :---- | :---- | :---- |
| CustomPluginOptions.json | Internals Plugin Verbosity | 2 |
| Controller.json | Use Direct Input FFB | true |
| Telemetry\\config.json | Steering Torque Frequency | 400 |
| CustomPluginOptions.json | Write Shared Memory | true |

## **The Impact of New Physics Features on Telemetry**

Update 1.2 introduced significant changes to the simulation of tires and tracks, including "rim thermals" and updated "RealRoad" surface physics.3 Rim thermals allow for heat transfer between the brakes, the wheel rims, and the tire carcasses.3 This complexity affects the tire's grip levels and, consequently, the torque feedback felt through the steering column.

As the rim heats up, the internal tire pressure rises, changing the pneumatic trail (![][image4]). Because FFBTorque is a product of this trail, the value becomes more dynamic as the race progresses. If a user is seeing 0.0, they may be testing in a session where the tires are not yet under load or the thermal model has not fully initialized. The FFBTorque field requires active lateral and longitudinal loads on the tire contact patch to generate a non-zero value.

## **Future Outlook: Scalability and Professional Use**

The transition to the native Shared Memory Interface and the inclusion of the SharedMemoryInterface.hpp SDK suggests that Motorsport Games and Studio 397 are preparing Le Mans Ultimate for a wider ecosystem of professional hardware.1 This is essential not only for the current PC version but also for the anticipated console release mentioned in the Update 1.2 technical notes.6

Consoles have much stricter requirements for memory management and background processes. The move toward a signed PluginAdapter.exe and a standardized shared memory map is a precursor to a unified telemetry API that can function across both PC and console architectures. For the professional sim racing community, this means that the reliability of the FFBTorque field will be a priority for future hotfixes, as it is the foundation for the high-end motion and haptic markets.

## **Summary of Findings**

The investigation into the Le Mans Ultimate Shared Memory Interface released with Update 1.2 reveals a sophisticated but highly-configured system. The failure of the FFBTorque field to update while standard 100Hz telemetry remains active is primarily a result of default "low-verbosity" settings intended to protect the 400Hz physics thread.

To resolve the 0.0 value issue, the following technical conditions must be met:

1. The Internals Plugin in CustomPluginOptions.json must be set to a verbosity level of 2 or higher.  
2. The PluginAdapter.exe must be running and authorized by the Easy Anti-Cheat system.  
3. Legacy rFactor 2 plugins must be removed to avoid mutual exclusivity conflicts.  
4. The vehicle must be in an active racing state with loads being applied to the steering rack.  
5. Advanced telemetry channels in the DuckDB config.json should be set to 400Hz to ensure the engine samples torque at every physics tick.

By aligning these configuration steps with the requirements of the new SharedMemoryInterface.hpp header, developers can unlock the full potential of the Update 1.2 physics engine and achieve the high-frequency torque output necessary for professional-grade simulation feedback.

#### **Works cited**

1. V1.2 (Update 2\) \- Le Mans Ultimate, accessed February 18, 2026, [https://guide.lemansultimate.com/hc/en-gb/articles/14556121957775-V1-2-Update-2](https://guide.lemansultimate.com/hc/en-gb/articles/14556121957775-V1-2-Update-2)  
2. Le Mans Ultimate's v1.2 December update: All you need to know : r/simracing \- Reddit, accessed February 18, 2026, [https://www.reddit.com/r/simracing/comments/1pdye7d/le\_mans\_ultimates\_v12\_december\_update\_all\_you/](https://www.reddit.com/r/simracing/comments/1pdye7d/le_mans_ultimates_v12_december_update_all_you/)  
3. V1.2 Update \- patch notes · Le Mans Ultimate update for 9 December 2025 \- SteamDB, accessed February 18, 2026, [https://steamdb.info/patchnotes/21098088/](https://steamdb.info/patchnotes/21098088/)  
4. Add missing parameters to telemetry for plugins | Page 18 | Le Mans Ultimate Community, accessed February 18, 2026, [https://community.lemansultimate.com/index.php?threads/add-missing-parameters-to-telemetry-for-plugins.66/page-18](https://community.lemansultimate.com/index.php?threads/add-missing-parameters-to-telemetry-for-plugins.66/page-18)  
5. Le Mans Ultimate \- Steam Community, accessed February 18, 2026, [https://steamcommunity.com/app/2399420/announcements/?l=english](https://steamcommunity.com/app/2399420/announcements/?l=english)  
6. Motorsport Games Inc. Releases Major Version 1.2 Update for Le Mans Ultimate, Introducing New Content and Enhanced Online Features \- Quiver Quantitative, accessed February 18, 2026, [https://www.quiverquant.com/news/Motorsport+Games+Inc.+Releases+Major+Version+1.2+Update+for+Le+Mans+Ultimate,+Introducing+New+Content+and+Enhanced+Online+Features](https://www.quiverquant.com/news/Motorsport+Games+Inc.+Releases+Major+Version+1.2+Update+for+Le+Mans+Ultimate,+Introducing+New+Content+and+Enhanced+Online+Features)  
7. Le Mans Ultimate's v1.2 December update: All you need to know \- Traxion.GG, accessed February 18, 2026, [https://traxion.gg/le-mans-ultimates-v1-2-december-update-all-you-need-to-know/](https://traxion.gg/le-mans-ultimates-v1-2-december-update-all-you-need-to-know/)  
8. V1.2.1.0 \- Update 2, Patch 1 \- 16th December · Le Mans Ultimate update for 16 December 2025 \- SteamDB, accessed February 18, 2026, [https://steamdb.info/patchnotes/21168182/](https://steamdb.info/patchnotes/21168182/)  
9. Telemetry Recording – Le Mans Ultimate, accessed February 18, 2026, [https://guide.lemansultimate.com/hc/en-gb/articles/14524956311695-Telemetry-Recording](https://guide.lemansultimate.com/hc/en-gb/articles/14524956311695-Telemetry-Recording)

[image1]: <data:image/png;base64,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>

[image2]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABUAAAAZCAYAAADe1WXtAAACNElEQVR4AdSUyUtVYRiHj80TUVHQCBENENWuIFoEQUG0LFpFRMugPyCIqF20atW+IEpw5wAiKOJCxQEcEBUV5wlFXTiPz3P0XA7XA94rulDe577v957v+/l+475gF/72ruh+VuMoHN8C+xygT6LFp298lV7v4Bv8gK/wHl7BG/gE3+EzPIBEUyj6sEowBd1wC17DLDRBCzRCNfTBI7gAiZYuOkavdpiDXiiAcqiBSsiHf2BuAp9ocdGowxWCa9AKI+AMcClbIBqHjEUP0VnBc/g2mATNtf5DcAz8Jy7TKHGipVd6gl43wFPgUti+RNuNcllmiBX7jR+AREsXPUMvRXHBQ34+wi/4AlWQkcVFja3qOiPr4T/kQS6481ZKuLUpFPWK1tNq60hWQC14jIyHiTOyuKib4NSXGNkB06C5038J3HVcaN6mI0Tx8TTXLZ48ReouDEEnLIPmCWg22OAs3tv1Af8YnOFb/FM4DEEkqr9IwkrdVaG5yQ6Scc0X8S6TY04T3werX8GHoi8IvI5l+JvwEqy0FH8H4ubSNJDoh5PgDC7jFfSi+D0ULSRp2VZhxZ5R4yfkHYRLmQdfvPfzZH0HrNwl8lz7LRTlW1aWQ2+n761S3KkPklMYF2xL1A30eHk6nqFyG2z7CBFmL5rDKNfwHr4YfCY9xz6PVk8q2Falnk9Fn6NwHkpA8XA9ibMWdWAXA3+CYkV439rU1GlnLeoYBXwWfQ96SKSmTRyaRygMdvJnDQAA//9qPgMHAAAABklEQVQDABLFdTM2jkx0AAAAAElFTkSuQmCC>

[image3]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAZCAYAAAAxFw7TAAABzklEQVR4AeyTOyhHURzHr/ervB/JI69B2BSTIouYDWZWg8WibDY7ZoOSQaJEMVhlEYNSEkpCeUTen8/9RwaFy2Bw+33O93fuved7z+/cc+KDX77+DQOXIJFljYNIocHrwBSSDhiESogU7w2zcNCwEf3xDDUoxKgO9uAEIoUzTGCk5ZaiznIXvQM/gnwvNBxgyBZMQT24hgtoK/gx5Ouh4SivN8E0rIJG7aj5I/qt0NDSchhVCzsQef0YG+47DSvo5IGl36CRwxmKa6eJhs8k/iQ3uM/S6at+OIM8Gcx9J43cZ0gs7Pigme4RuGWK0C6ohjbogyFQvT9M3gPd0A+doDEShCWnkhXA63bxp2TTN3JpnqAFlmAd/KAskp+B1WWiYTjDC7JZ8M+OozXgtvED8+Qewzl0H1wGJ7BGfg354JrfomFoeE82Bh65XnQEjuEBLKUK3YQ4KAb1EC0BKzN3v+oVlsz9wLIuSZytP4U0jHJan7mdHODsD7h3CmXgu5bbQO6/eDOk/2EkcXcFzsG4olkGj6ZVuCss1+VRPzXcYPAE+LJLMEPuCUKCbZpJ8IRZtrP91NCjpxHjwrDvEoQdGp95jzQWrkss+6X27xu+AAAA///ugo1iAAAABklEQVQDAOGwVDN8+EplAAAAAElFTkSuQmCC>

[image4]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA8AAAAaCAYAAABozQZiAAABvElEQVR4AdyTyyutURiH9z6nzjmdW5QkFMm1SEQZKDNDGRsoQ2bkH6AMZCDlHzAykolkyIQkA1Iit1yi5J57Ls+ztdli70+Z2b3P/q31rff37Xe9a+1voU98vog5TAu+w4cids8/cFRCHfyDwIg1/ye7ARrhDwRG1Gy5SWTnwSacQmBEzWoG2WmwBFcQGJqayBqDXiiAZuiHMkgYmjV2kLUC89AKfeAciR+ad1leBxu2gE6CpZ+hCUOzzUonKxsW4QI+FFFzLtm/YBYShT/0vK7ZG1XOkz2w/J9oCnjWv9FMsDKfqanM9YQ0m1DCg1U4Bjtej3p0VWgbdEML1EIP5EBY8wMDm3ONFkMpeFGOUF+2jcogOgLehSI0Yj5n4IIXo4axDZtAD8Djchtu6ZB5MridWzRS9h2DYWgHz3cItYp79C94bfdRX1qB7sAyPFg2GrJ0Fy+dxOC+xaZV8zwfBmALns2M34QdtTH+STZY9cWj6DjcQKRs9T08hSwWbJ6GKcYzYIVIKK7Zplimf9FCMiWMuj3kKaJ7fpq9fNv5aaZd0AlzYGORl4hnNtGjca9rpJ/Aq19lHrds1wJ5BAAA//9wFBYhAAAABklEQVQDAFkmVjWG9S/lAAAAAElFTkSuQmCC>

[image5]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACkAAAAZCAYAAACsGgdbAAADi0lEQVR4AeyW6atNURTAzzPP86xMyewDUoYMnwxJQiSZMoskkSi+4B+QUIQimZLMEpmVOQop81RkiszT73fvfbfrvPNeXveeV+rd1u+utYez99prr73PKRP8B79SJ3O1SUVFsjqTdIcxMB76QS0oB62hDuRB7BLlZFVmHQbLYRK0hYbQDXR4JHoCWIeKX8JONmLKeTAHXsI2WA1rYA9Ug4XQHD7Cb4hdMp2sx2yzYBTsAh07j34Nn+ARWP6KfgYfoEQk38nKzDYCJsJe0Ml36MxIab+g7ibcBh1HxS86afK3YKpp8AB08i06Sn5RaRQfo79DiYhOeloHM1t7uAz3oDAxTzfQeBGMLCp+0clKTNMXzL3raA8EKlI+U+uWqzFzIo0ZpR3UhUjRSe++NrS6xffRmRGqSHkgGOleaBczCO09WRadC3GsBQzUByJFJ91uo+mpDZ9Y21rxZBdYBEuhP9QGcxmVtZxjhPJQAyJFJ3XMLdS2c2ZHt3UzFQegGejYDvQ1+AE+4+BG3BvCMtWBi66JYR0qLT5vX5+pkKr1MDpWqlhQOag5eJYm70m33TqKCXGAL1j1U9xFewM4qAsaTdnbwAivxB4HvWEGdAC3cShasb/pMpvCEFgB9tFxzMD06YmxCZaBN06iTYfc5i1UPoSp4Osv0YitmLOTMdz6O2gjjwp09DSG0XqP9o3k/akzTujuXKB+LjiGk3oXu8ir1H0D+6ISYmQN1BlKG+E5JM6HTmrcosJVWz6G7fauQ2+FU3AEXLlX1E9sxed0zki7/eaW7e7KcTqMhcXQElyIud0U24W6I0brBmXHMQX8HvBD5iR1T8FFoIJApzTc1isYnuIe6FWwH3TMQ2OkfYf7WqS6gDiRGJkptE6H3TAfjLxRst157KPt3Np0CXwxHMW4BL6WzVnMpNgxaSX/9d6V+sBBqrTzI0fxL8mj1AD8amqCdkuroL33vPR9zg8RozSAetPDK64rthE1GB2x/ZrSKV+zpkEn6oZD+gYJO0nbP4t551vKPPTku61GzXTxzu3MSC7Ag4UZ+KLYGQSBDnnn+j1qfuroK+q9Ecz7J9g+a3o4R3q7qS+2GJlDPOVHiSfbvDR65uZa6g/DdlgC6+ENGCm/rvZhm/d+xJzAngmmljtof8+H6eccWTnJuIWKOeatYe6FOzmxbeZnuC2ynM12Rw4YR2Wpk8WIapFd/wAAAP//GZZ/xAAAAAZJREFUAwCiKrozxSBe8gAAAABJRU5ErkJggg==>