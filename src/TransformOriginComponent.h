#pragma once

#include "glm/glm.hpp"

namespace inspector {

enum class TransformOrigin {
    Center,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};

struct transform_origin_component {
    TransformOrigin origin = TransformOrigin::Center;
};

inline glm::vec2 getOriginOffset(TransformOrigin o, float width, float height) {
    switch (o) {
        case TransformOrigin::TopLeft:     return glm::vec2(0,     0);
        case TransformOrigin::TopRight:    return glm::vec2(width, 0);
        case TransformOrigin::BottomLeft:  return glm::vec2(0,     height);
        case TransformOrigin::BottomRight: return glm::vec2(width, height);
        default:                           return glm::vec2(width * 0.5f, height * 0.5f);
    }
}

} // namespace inspector
