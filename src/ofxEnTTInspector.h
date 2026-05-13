#pragma once

// ============================================================================
// ofxEnTTInspector
// ImGui inspector UI for ofxEnTTKit ECS components.
//
// No app framework dependency. Works on Linux, Linux ARM, macOS, Windows.
// File dialogs provided by ofxImGuiFileDialog (no zenity, no native OS dialogs).
//
// Dependencies: ofxEnTTKit, ofxImGui, ofxImGuiFileDialog
//
// Quick start:
//   #include "ofxEnTTInspector.h"
//
//   // Inspect all components on a selected entity:
//   if (inspector::inspectEntity(registry, selectedEntity)) {
//       // a property was changed this frame
//   }
//
//   // Or inspect a single component:
//   inspector::ComponentInspector ci("My Component");
//   inspector::registerProperties(myComp, ci);
//   ci.draw();
//
//   // Reflection (for state capture / patch cords):
//   auto props = inspector::getEntityProperties(registry, entity);
// ============================================================================

#include "PinDataTypes.h"
#include "PropertyReflector.h"
#include "PropertyValue.h"
#include "ComponentInspector.h"
#include "TransformOriginComponent.h"
#include "entity_inspector.h"
#include "inspectors/inspectors.h"
