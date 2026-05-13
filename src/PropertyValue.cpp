#include "PropertyValue.h"

#include <glm/gtx/quaternion.hpp>
#include <algorithm>
#include <cmath>

namespace inspector {

// ─── from ────────────────────────────────────────────────────────────────────

PropertyValue PropertyValue::from(const ReflectedProperty& prop)
{
    PropertyValue pv;
    pv.type = prop.type;
    switch (prop.type) {
        case PinDataType::Float:  pv.f   = *prop.asFloat();  break;
        case PinDataType::Int:    pv.i   = *prop.asInt();    break;
        case PinDataType::Bool:   pv.b   = *prop.asBool();   break;
        case PinDataType::String: pv.str = *prop.asString(); break;
        case PinDataType::Vec2:   pv.v2  = *prop.asVec2();   break;
        case PinDataType::Vec3:   pv.v3  = *prop.asVec3();   break;
        case PinDataType::Vec4:   pv.v4  = *prop.asVec4();   break;
        case PinDataType::Quat:   pv.q   = *prop.asQuat();   break;
        case PinDataType::Color:  pv.c   = *prop.asColor();  break;
        default: break;
    }
    return pv;
}

// ─── apply ───────────────────────────────────────────────────────────────────

void PropertyValue::apply(const ReflectedProperty& prop) const
{
    if (prop.type != type) return;
    switch (type) {
        case PinDataType::Float:  *prop.asFloat()  = f;   break;
        case PinDataType::Int:    *prop.asInt()     = i;   break;
        case PinDataType::Bool:   *prop.asBool()    = b;   break;
        case PinDataType::String: *prop.asString()  = str; break;
        case PinDataType::Vec2:   *prop.asVec2()    = v2;  break;
        case PinDataType::Vec3:   *prop.asVec3()    = v3;  break;
        case PinDataType::Vec4:   *prop.asVec4()    = v4;  break;
        case PinDataType::Quat:   *prop.asQuat()    = q;   break;
        case PinDataType::Color:  *prop.asColor()   = c;   break;
        default: break;
    }
}

// ─── lerp ────────────────────────────────────────────────────────────────────

PropertyValue PropertyValue::lerp(const PropertyValue& a, const PropertyValue& b, float t)
{
    PropertyValue out;
    out.type = a.type;

    auto lerpF  = [](float x, float y, float t) { return x + (y - x) * t; };
    auto lerpU8 = [&lerpF](uint8_t x, uint8_t y, float t) {
        return static_cast<uint8_t>(std::round(lerpF((float)x, (float)y, t)));
    };

    switch (a.type) {
        case PinDataType::Float:
            out.f = lerpF(a.f, b.f, t);
            break;
        case PinDataType::Int:
            out.i = static_cast<int>(std::round(lerpF((float)a.i, (float)b.i, t)));
            break;
        case PinDataType::Bool:
            out.b = (t < 0.5f) ? a.b : b.b;
            break;
        case PinDataType::String:
            out.str = (t < 0.5f) ? a.str : b.str;
            break;
        case PinDataType::Vec2:
            out.v2 = glm::mix(a.v2, b.v2, t);
            break;
        case PinDataType::Vec3:
            out.v3 = glm::mix(a.v3, b.v3, t);
            break;
        case PinDataType::Vec4:
            out.v4 = glm::mix(a.v4, b.v4, t);
            break;
        case PinDataType::Quat:
            out.q = glm::slerp(a.q, b.q, t);
            break;
        case PinDataType::Color:
            out.c = ofColor(lerpU8(a.c.r, b.c.r, t),
                            lerpU8(a.c.g, b.c.g, t),
                            lerpU8(a.c.b, b.c.b, t),
                            lerpU8(a.c.a, b.c.a, t));
            break;
        default:
            out = (t < 0.5f) ? a : b;
            break;
    }
    return out;
}

// ─── toJson ──────────────────────────────────────────────────────────────────

ofJson PropertyValue::toJson() const
{
    switch (type) {
        case PinDataType::Float:  return f;
        case PinDataType::Int:    return i;
        case PinDataType::Bool:   return b;
        case PinDataType::String: return str;
        case PinDataType::Vec2:   return {v2.x, v2.y};
        case PinDataType::Vec3:   return {v3.x, v3.y, v3.z};
        case PinDataType::Vec4:   return {v4.x, v4.y, v4.z, v4.w};
        case PinDataType::Quat:   return {q.w, q.x, q.y, q.z};
        case PinDataType::Color:  return {c.r, c.g, c.b, c.a};
        default:                  return nullptr;
    }
}

// ─── fromJson ────────────────────────────────────────────────────────────────

PropertyValue PropertyValue::fromJson(const ofJson& j, PinDataType t)
{
    PropertyValue pv;
    pv.type = t;
    switch (t) {
        case PinDataType::Float:  pv.f   = j.get<float>();       break;
        case PinDataType::Int:    pv.i   = j.get<int>();         break;
        case PinDataType::Bool:   pv.b   = j.get<bool>();        break;
        case PinDataType::String: pv.str = j.get<std::string>(); break;
        case PinDataType::Vec2:
            pv.v2 = {j[0].get<float>(), j[1].get<float>()};
            break;
        case PinDataType::Vec3:
            pv.v3 = {j[0].get<float>(), j[1].get<float>(), j[2].get<float>()};
            break;
        case PinDataType::Vec4:
            pv.v4 = {j[0].get<float>(), j[1].get<float>(), j[2].get<float>(), j[3].get<float>()};
            break;
        case PinDataType::Quat:
            // Stored as [w, x, y, z]
            pv.q.w = j[0].get<float>();
            pv.q.x = j[1].get<float>();
            pv.q.y = j[2].get<float>();
            pv.q.z = j[3].get<float>();
            break;
        case PinDataType::Color:
            pv.c = ofColor(j[0].get<uint8_t>(), j[1].get<uint8_t>(),
                           j[2].get<uint8_t>(), j[3].get<uint8_t>());
            break;
        default: break;
    }
    return pv;
}

} // namespace inspector
