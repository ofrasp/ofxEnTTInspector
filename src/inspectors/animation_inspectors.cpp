// ============================================================================
// Animation Component Inspectors - Implementation
// ============================================================================

#include "animation_inspectors.h"

namespace inspector {

// ============================================================================
// Tween Component Inspector
// ============================================================================

void registerProperties(ecs::tween_component& comp, ComponentInspector& inspector) {
    inspector.addProperty("From", &comp.from);
    inspector.addProperty("To", &comp.to);
    inspector.addProperty("Current", &comp.current);
    inspector.addProperty("Duration", &comp.duration, 0.1f, 60.0f);
    inspector.addProperty("Elapsed", &comp.elapsed);
    
    std::vector<std::string> easingTypes = {
        "Linear",
        "Ease In Quad", "Ease Out Quad", "Ease In Out Quad",
        "Ease In Cubic", "Ease Out Cubic", "Ease In Out Cubic",
        "Ease In Quart", "Ease Out Quart", "Ease In Out Quart",
        "Ease In Quint", "Ease Out Quint", "Ease In Out Quint",
        "Ease In Sine", "Ease Out Sine", "Ease In Out Sine",
        "Ease In Expo", "Ease Out Expo", "Ease In Out Expo",
        "Ease In Circ", "Ease Out Circ", "Ease In Out Circ",
        "Ease In Elastic", "Ease Out Elastic", "Ease In Out Elastic",
        "Ease In Back", "Ease Out Back", "Ease In Out Back",
        "Ease In Bounce", "Ease Out Bounce", "Ease In Out Bounce"
    };
    inspector.addEnumProperty("Easing", (int*)&comp.easing, easingTypes);
    
    inspector.addProperty("Playing", &comp.playing);
    inspector.addProperty("Loop", &comp.loop);
    inspector.addProperty("Ping Pong", &comp.pingpong);
    inspector.addProperty("Yoyo", &comp.yoyo);
    
    inspector.addCustomProperty("Controls", [&]() {
        if (ImGui::Button("Play")) comp.play();
        ImGui::SameLine();
        if (ImGui::Button("Pause")) comp.pause();
        ImGui::SameLine();
        if (ImGui::Button("Stop")) comp.stop();
        ImGui::SameLine();
        if (ImGui::Button("Reset")) comp.reset();
    });
    
    inspector.addCustomProperty("Preview", [&]() {
        if (comp.duration > 0) {
            float t = comp.elapsed / comp.duration;
            float eased = ecs::tween_component::ease(t, comp.easing);
            
            ImGui::Text("Progress: %.2f%%", t * 100.0f);
            ImGui::ProgressBar(t);
            
            ImGui::Text("Eased: %.2f%%", eased * 100.0f);
            ImGui::ProgressBar(eased);
        }
    });
}

// ============================================================================
// Particle Emitter Component Inspector
// ============================================================================

void registerProperties(ecs::particle_emitter_component& comp, ComponentInspector& inspector) {
    inspector.addProperty("Max Particles", &comp.maxParticles, 1, 10000);
    inspector.addProperty("Emit Rate", &comp.emitRate, 0.0f, 1000.0f);
    inspector.addProperty("Playing", &comp.playing);
    inspector.addProperty("Loop", &comp.loop);
    
    if (ImGui::CollapsingHeader("Emission", ImGuiTreeNodeFlags_DefaultOpen)) {
        inspector.addProperty("Position", &comp.emitPosition);
        inspector.addProperty("Direction", &comp.emitDirection);
        inspector.addProperty("Spread", &comp.emitSpread, 0.0f, 180.0f);
    }
    
    if (ImGui::CollapsingHeader("Particle Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
        inspector.addProperty("Life Min", &comp.lifeMin, 0.1f, 60.0f);
        inspector.addProperty("Life Max", &comp.lifeMax, 0.1f, 60.0f);
        inspector.addProperty("Size Min", &comp.sizeMin, 0.1f, 100.0f);
        inspector.addProperty("Size Max", &comp.sizeMax, 0.1f, 100.0f);
        inspector.addProperty("Speed Min", &comp.speedMin, 0.0f, 1000.0f);
        inspector.addProperty("Speed Max", &comp.speedMax, 0.0f, 1000.0f);
        inspector.addProperty("Color Start", &comp.colorStart);
        inspector.addProperty("Color End", &comp.colorEnd);
    }
    
    if (ImGui::CollapsingHeader("Forces")) {
        inspector.addProperty("Gravity", &comp.gravity);
        inspector.addProperty("Damping", &comp.damping, 0.0f, 1.0f);
    }
    
    inspector.addCustomProperty("Controls", [&]() {
        if (ImGui::Button("Play")) comp.play();
        ImGui::SameLine();
        if (ImGui::Button("Pause")) comp.pause();
        ImGui::SameLine();
        if (ImGui::Button("Stop")) comp.stop();
        
        static int emitCount = 10;
        if (ImGui::InputInt("Count", &emitCount, 1, 10)) {
            emitCount = ofClamp(emitCount, 1, 1000);
        }
        if (ImGui::Button("Emit")) {
            comp.emit(emitCount);
        }
    });
    
    inspector.addCustomProperty("Stats", [&]() {
        ImGui::Text("Active Particles: %zu / %d", comp.particles.size(), comp.maxParticles);
        
        float fillPercent = (float)comp.particles.size() / comp.maxParticles * 100.0f;
        ImGui::ProgressBar((float)comp.particles.size() / comp.maxParticles);
        ImGui::SameLine();
        ImGui::Text("%.1f%%", fillPercent);
    });
}

// ============================================================================
// PostFX Component Inspector
// ============================================================================

void registerProperties(ecs::postfx_component& comp, ComponentInspector& inspector) {
    inspector.addProperty("Enabled", &comp.enabled);
    
    if (ImGui::CollapsingHeader("Effect Chain", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Active Effects: %zu", comp.effectChain.size());
        
        for (size_t i = 0; i < comp.effectChain.size(); i++) {
            auto& effect = comp.effectChain[i];
            
            ImGui::PushID(static_cast<int>(i));
            ImGui::Checkbox("##enabled", &effect.enabled);
            ImGui::SameLine();
            
            const char* effectName = "";
            switch (effect.type) {
                case ecs::postfx_component::BLOOM: effectName = "Bloom"; break;
                case ecs::postfx_component::BLUR: effectName = "Blur"; break;
                case ecs::postfx_component::CHROMATIC_ABERRATION: effectName = "Chromatic Aberration"; break;
                case ecs::postfx_component::VIGNETTE: effectName = "Vignette"; break;
                case ecs::postfx_component::GRAIN: effectName = "Grain"; break;
                case ecs::postfx_component::SCANLINES: effectName = "Scanlines"; break;
                case ecs::postfx_component::CRT: effectName = "CRT"; break;
                case ecs::postfx_component::GLITCH: effectName = "Glitch"; break;
                case ecs::postfx_component::COLOR_GRADING: effectName = "Color Grading"; break;
                case ecs::postfx_component::FXAA: effectName = "FXAA"; break;
                case ecs::postfx_component::DOF: effectName = "Depth of Field"; break;
                case ecs::postfx_component::SSAO: effectName = "SSAO"; break;
            }
            
            if (ImGui::TreeNode(effectName)) {
                for (auto& [paramName, paramValue] : effect.params) {
                    if (ImGui::DragFloat(paramName.c_str(), &paramValue, 0.01f, 0.0f, 10.0f)) {
                        comp.setParam(effect.type, paramName, paramValue);
                    }
                }
                
                if (ImGui::Button("Remove Effect")) {
                    comp.removeEffect(effect.type);
                }
                
                ImGui::TreePop();
            }
            
            ImGui::PopID();
        }
    }
    
    inspector.addCustomProperty("Add Effects", [&]() {
        if (ImGui::BeginCombo("Add Effect", "Select...")) {
            if (ImGui::Selectable("Bloom")) comp.addEffect(ecs::postfx_component::BLOOM);
            if (ImGui::Selectable("Blur")) comp.addEffect(ecs::postfx_component::BLUR);
            if (ImGui::Selectable("Chromatic Aberration")) comp.addEffect(ecs::postfx_component::CHROMATIC_ABERRATION);
            if (ImGui::Selectable("Vignette")) comp.addEffect(ecs::postfx_component::VIGNETTE);
            if (ImGui::Selectable("Grain")) comp.addEffect(ecs::postfx_component::GRAIN);
            if (ImGui::Selectable("Scanlines")) comp.addEffect(ecs::postfx_component::SCANLINES);
            if (ImGui::Selectable("CRT")) comp.addEffect(ecs::postfx_component::CRT);
            if (ImGui::Selectable("Glitch")) comp.addEffect(ecs::postfx_component::GLITCH);
            if (ImGui::Selectable("Color Grading")) comp.addEffect(ecs::postfx_component::COLOR_GRADING);
            if (ImGui::Selectable("FXAA")) comp.addEffect(ecs::postfx_component::FXAA);
            if (ImGui::Selectable("Depth of Field")) comp.addEffect(ecs::postfx_component::DOF);
            if (ImGui::Selectable("SSAO")) comp.addEffect(ecs::postfx_component::SSAO);
            ImGui::EndCombo();
        }
    });
}

} // namespace inspector
