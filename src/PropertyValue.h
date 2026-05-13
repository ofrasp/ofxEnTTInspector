#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// PropertyValue
//
// An owned deep-copy of a single component field value. Complements
// ReflectedProperty (which holds a *live* void* pointer). Use PropertyValue
// when you need to store, interpolate, or serialize a field value independent
// of the lifetime of the component it came from.
//
// Typical uses:
//   • ofxTanim keyframe storage
//   • ofxEnTTStateCollector preset snapshots
//   • Settings serialization / deserialization via ofJSON
// ─────────────────────────────────────────────────────────────────────────────

#include "PinDataTypes.h"
#include "PropertyReflector.h"
#include "ofColor.h"
#include "ofJson.h"
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace inspector {

struct PropertyValue {
    PinDataType type = PinDataType::Float;

    // Plain data members (no union) for simplicity and safe copy semantics.
    float      f   = 0.f;
    int        i   = 0;
    bool       b   = false;
    glm::vec2  v2  = {};
    glm::vec3  v3  = {};
    glm::vec4  v4  = {};
    glm::quat  q   = glm::quat(1.f, 0.f, 0.f, 0.f);
    ofColor    c   = ofColor(0, 0, 0, 255);
    std::string str;

    // ── Factory: snapshot the current value from a live ReflectedProperty ────
    static PropertyValue from(const ReflectedProperty& prop);

    // ── Apply: write this value back into a live ReflectedProperty ───────────
    // No-op if types don't match.
    void apply(const ReflectedProperty& prop) const;

    // ── Interpolation ─────────────────────────────────────────────────────────
    // Linear interpolation (slerp for Quat, nearest for Bool/String).
    static PropertyValue lerp(const PropertyValue& a, const PropertyValue& b, float t);

    // ── JSON serialization via ofJSON (nlohmann) ──────────────────────────────
    ofJson toJson() const;
    static PropertyValue fromJson(const ofJson& j, PinDataType type);

    // ── Convenience constructors ──────────────────────────────────────────────
    static PropertyValue makeFloat (float v)       { PropertyValue p; p.type = PinDataType::Float;  p.f  = v; return p; }
    static PropertyValue makeInt   (int v)         { PropertyValue p; p.type = PinDataType::Int;    p.i  = v; return p; }
    static PropertyValue makeBool  (bool v)        { PropertyValue p; p.type = PinDataType::Bool;   p.b  = v; return p; }
    static PropertyValue makeString(std::string v) { PropertyValue p; p.type = PinDataType::String; p.str = std::move(v); return p; }
    static PropertyValue makeVec2  (glm::vec2 v)   { PropertyValue p; p.type = PinDataType::Vec2;   p.v2 = v; return p; }
    static PropertyValue makeVec3  (glm::vec3 v)   { PropertyValue p; p.type = PinDataType::Vec3;   p.v3 = v; return p; }
    static PropertyValue makeVec4  (glm::vec4 v)   { PropertyValue p; p.type = PinDataType::Vec4;   p.v4 = v; return p; }
    static PropertyValue makeQuat  (glm::quat v)   { PropertyValue p; p.type = PinDataType::Quat;   p.q  = v; return p; }
    static PropertyValue makeColor (ofColor v)     { PropertyValue p; p.type = PinDataType::Color;  p.c  = v; return p; }
};

} // namespace inspector
