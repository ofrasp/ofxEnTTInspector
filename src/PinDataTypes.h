#pragma once

#include "imgui.h"
#include <string>

namespace inspector {

enum class PinDataType {
    Float,
    Int,
    Bool,
    String,
    Vec2,
    Vec3,
    Color,
    Trigger,
    Any
};

inline bool canConnect(PinDataType a, PinDataType b) {
    if (a == PinDataType::Any || b == PinDataType::Any) return true;
    return a == b;
}

inline ImU32 getPinColor(PinDataType type) {
    switch (type) {
        case PinDataType::Float:   return IM_COL32(255, 165,   0, 255);
        case PinDataType::Int:     return IM_COL32(100, 149, 237, 255);
        case PinDataType::Bool:    return IM_COL32(220,  80,  80, 255);
        case PinDataType::String:  return IM_COL32(100, 200, 100, 255);
        case PinDataType::Vec2:    return IM_COL32(100, 200, 200, 255);
        case PinDataType::Vec3:    return IM_COL32(180, 130, 220, 255);
        case PinDataType::Color:   return IM_COL32(255, 150, 200, 255);
        case PinDataType::Trigger: return IM_COL32(255, 255, 255, 255);
        case PinDataType::Any:     return IM_COL32(150, 150, 150, 255);
        default:                   return IM_COL32(200, 200, 200, 255);
    }
}

inline const char* getPinTypeName(PinDataType type) {
    switch (type) {
        case PinDataType::Float:   return "Float";
        case PinDataType::Int:     return "Int";
        case PinDataType::Bool:    return "Bool";
        case PinDataType::String:  return "String";
        case PinDataType::Vec2:    return "Vec2";
        case PinDataType::Vec3:    return "Vec3";
        case PinDataType::Color:   return "Color";
        case PinDataType::Trigger: return "Trigger";
        case PinDataType::Any:     return "Any";
        default:                   return "Unknown";
    }
}

} // namespace inspector
