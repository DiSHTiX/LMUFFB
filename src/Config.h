#ifndef CONFIG_H
#define CONFIG_H

#include "../FFBEngine.h"
#include <string>
#include <vector>

struct Preset {
    std::string name;
    float gain;
    float understeer;
    float sop;
    float sop_scale;
    float sop_smoothing;
    float min_force;
    float oversteer_boost;
    bool lockup_enabled;
    float lockup_gain;
    bool spin_enabled;
    float spin_gain;
    bool slide_enabled;
    float slide_gain;
    bool road_enabled;
    float road_gain;
    
    // Apply this preset to an engine instance
    void Apply(FFBEngine& engine) const {
        engine.m_gain = gain;
        engine.m_understeer_effect = understeer;
        engine.m_sop_effect = sop;
        engine.m_sop_scale = sop_scale;
        engine.m_sop_smoothing_factor = sop_smoothing;
        engine.m_min_force = min_force;
        engine.m_oversteer_boost = oversteer_boost;
        engine.m_lockup_enabled = lockup_enabled;
        engine.m_lockup_gain = lockup_gain;
        engine.m_spin_enabled = spin_enabled;
        engine.m_spin_gain = spin_gain;
        engine.m_slide_texture_enabled = slide_enabled;
        engine.m_slide_texture_gain = slide_gain;
        engine.m_road_texture_enabled = road_enabled;
        engine.m_road_texture_gain = road_gain;
    }
};

class Config {
public:
    static void Save(const FFBEngine& engine, const std::string& filename = "config.ini");
    static void Load(FFBEngine& engine, const std::string& filename = "config.ini");
    
    // Preset Management
    static std::vector<Preset> presets;
    static void LoadPresets(); // Populates presets vector
    static void ApplyPreset(int index, FFBEngine& engine);

    // Global App Settings (not part of FFB Physics)
    static bool m_ignore_vjoy_version_warning;
    static bool m_enable_vjoy;        // Acquire vJoy device (Driver Enabled)
    static bool m_output_ffb_to_vjoy; // Output FFB signal to vJoy Axis X (Monitor)
};

#endif
