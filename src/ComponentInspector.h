#pragma once

#include "ofMain.h"
#include "imgui.h"
#include "PropertyReflector.h"
#include <string>
#include <functional>
#include <vector>
#include <filesystem>
#include <glm/glm.hpp>

namespace inspector {

struct PropertyInfo {
    std::string          name;
    std::function<bool()> drawImGui;
};

class ComponentInspector {
public:
    ComponentInspector(const std::string& componentName)
        : m_componentName(componentName) {}

    // ── Typed addProperty overloads (UI + reflection) ────────────────────────

    void addProperty(const std::string& name, float* value,
                     float min = 0.f, float max = 100.f, float speed = 1.f) {
        m_properties.push_back({name, [=]() -> bool {
            return ImGui::DragFloat(name.c_str(), value, speed, min, max);
        }});
        m_reflected.emplace_back(name, PinDataType::Float, value, min, max);
    }

    void addProperty(const std::string& name, int* value,
                     int min = 0, int max = 100, int speed = 1) {
        m_properties.push_back({name, [=]() -> bool {
            return ImGui::DragInt(name.c_str(), value, (float)speed, min, max);
        }});
        m_reflected.emplace_back(name, PinDataType::Int, value, (float)min, (float)max);
    }

    void addProperty(const std::string& name, bool* value) {
        m_properties.push_back({name, [=]() -> bool {
            return ImGui::Checkbox(name.c_str(), value);
        }});
        m_reflected.emplace_back(name, PinDataType::Bool, value, 0.f, 1.f);
    }

    void addProperty(const std::string& name, std::string* value) {
        m_properties.push_back({name, [=]() -> bool {
            char buf[256];
            strncpy(buf, value->c_str(), sizeof(buf));
            if (ImGui::InputText(name.c_str(), buf, sizeof(buf))) {
                *value = buf;
                return true;
            }
            return false;
        }});
        m_reflected.emplace_back(name, PinDataType::String, value);
    }

    void addProperty(const std::string& name, std::filesystem::path* value) {
        m_properties.push_back({name, [=]() -> bool {
            char buf[512];
            std::string s = value->string();
            strncpy(buf, s.c_str(), sizeof(buf));
            buf[sizeof(buf) - 1] = '\0';
            if (ImGui::InputText(name.c_str(), buf, sizeof(buf))) {
                *value = buf;
                return true;
            }
            return false;
        }});
    }

    void addProperty(const std::string& name, glm::vec2* value,
                     float min = -FLT_MAX, float max = FLT_MAX, float speed = 1.f) {
        m_properties.push_back({name, [=]() -> bool {
            return ImGui::DragFloat2(name.c_str(), (float*)value, speed, min, max);
        }});
        m_reflected.emplace_back(name, PinDataType::Vec2, value, min, max);
    }

    void addProperty(const std::string& name, glm::vec3* value,
                     float min = -FLT_MAX, float max = FLT_MAX, float speed = 1.f) {
        m_properties.push_back({name, [=]() -> bool {
            return ImGui::DragFloat3(name.c_str(), (float*)value, speed, min, max);
        }});
        m_reflected.emplace_back(name, PinDataType::Vec3, value, min, max);
    }

    void addProperty(const std::string& name, glm::vec4* value,
                     float min = -FLT_MAX, float max = FLT_MAX, float speed = 1.f) {
        m_properties.push_back({name, [=]() -> bool {
            return ImGui::DragFloat4(name.c_str(), (float*)value, speed, min, max);
        }});
    }

    void addProperty(const std::string& name, ofColor* value) {
        m_properties.push_back({name, [value, name]() -> bool {
            ofFloatColor fc = *value;
            ImVec4 c = ImVec4(fc.r, fc.g, fc.b, fc.a);
            if (ImGui::ColorEdit4(name.c_str(), (float*)&c)) {
                value->set(c.x * 255.f, c.y * 255.f, c.z * 255.f, c.w * 255.f);
                return true;
            }
            return false;
        }});
        m_reflected.emplace_back(name, PinDataType::Color, value, 0.f, 255.f);
    }

    void addProperty(const std::string& name, ofFloatColor* value) {
        m_properties.push_back({name, [value, name]() -> bool {
            ImVec4 c = ImVec4(value->r, value->g, value->b, value->a);
            if (ImGui::ColorEdit4(name.c_str(), (float*)&c)) {
                value->set(c.x, c.y, c.z, c.w);
                return true;
            }
            return false;
        }});
    }

    template<typename T>
    void addEnumProperty(const std::string& name, T* value,
                         const std::vector<std::string>& options) {
        m_properties.push_back({name, [=]() -> bool {
            int cur = static_cast<int>(*value);
            if (ImGui::Combo(name.c_str(), &cur,
                [](void* data, int idx) -> const char* {
                    return (*static_cast<const std::vector<std::string>*>(data))[idx].c_str();
                }, (void*)&options, (int)options.size())) {
                *value = static_cast<T>(cur);
                return true;
            }
            return false;
        }});
        m_reflected.emplace_back(name, PinDataType::Int, value, 0.f, (float)(options.size() - 1));
    }

    // UI-only custom widget (no reflection metadata)
    void addCustomProperty(const std::string& name, std::function<void()> widget) {
        m_properties.push_back({name, [widget]() -> bool {
            widget();
            return ImGui::IsItemDeactivatedAfterEdit();
        }});
    }

    // ── Data-only reflection (no UI widget) ──────────────────────────────────

    void addReflectable(const std::string& name, float* v, float min = 0.f, float max = 100.f) {
        m_reflected.emplace_back(name, PinDataType::Float, v, min, max);
    }
    void addReflectable(const std::string& name, int* v, int min = 0, int max = 100) {
        m_reflected.emplace_back(name, PinDataType::Int, v, (float)min, (float)max);
    }
    void addReflectable(const std::string& name, bool* v) {
        m_reflected.emplace_back(name, PinDataType::Bool, v, 0.f, 1.f);
    }
    void addReflectable(const std::string& name, glm::vec2* v, float min = -1000.f, float max = 1000.f) {
        m_reflected.emplace_back(name, PinDataType::Vec2, v, min, max);
    }
    void addReflectable(const std::string& name, glm::vec3* v, float min = -1000.f, float max = 1000.f) {
        m_reflected.emplace_back(name, PinDataType::Vec3, v, min, max);
    }
    void addReflectable(const std::string& name, ofColor* v) {
        m_reflected.emplace_back(name, PinDataType::Color, v, 0.f, 255.f);
    }

    // ── Drawing & access ─────────────────────────────────────────────────────

    bool draw() {
        bool changed = false;
        if (ImGui::CollapsingHeader(m_componentName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::PushID(m_componentName.c_str());
            ImGui::PushItemWidth(-120);
            for (auto& prop : m_properties) changed |= prop.drawImGui();
            ImGui::PopItemWidth();
            ImGui::PopID();
        }
        return changed;
    }

    bool hasProperties() const { return !m_properties.empty(); }
    void clear()               { m_properties.clear(); m_reflected.clear(); }

    const std::vector<ReflectedProperty>& getReflectedProperties() const { return m_reflected; }

private:
    std::string                    m_componentName;
    std::vector<PropertyInfo>      m_properties;
    std::vector<ReflectedProperty> m_reflected;
};

} // namespace inspector
