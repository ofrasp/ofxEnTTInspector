# ofxEnTTInspector

ImGui inspector UI for [ofxEnTTKit](https://github.com/ofrasp/ofxEnTTKit) ECS components.

- No app framework dependency — works with any `entt::registry`  
- File dialogs via [ofxImGuiFileDialog](https://github.com/ofrasp/ofxImGuiFileDialog)  
- Works on Linux, Linux ARM (Raspberry Pi), macOS, Windows  
- Full component coverage: base, rendering, animation, 2D/3D graphics, hardware, LED/UV, swatches, filters, generators, modulators

## Dependencies

- [`ofxEnTT`](https://github.com/ofrasp/ofxEnTT)
- [`ofxEnTTKit`](https://github.com/ofrasp/ofxEnTTKit)
- [`ofxImGui`](https://github.com/ofrasp/ofxImGui)
- [`ofxImGuiFileDialog`](https://github.com/ofrasp/ofxImGuiFileDialog)

## Usage

```cpp
#include "ofxEnTTInspector.h"

// Inspect all components on a selected entity (call inside your ImGui block):
if (inspector::inspectEntity(registry, selectedEntity)) {
    // a property changed this frame
}
```

### Single component

```cpp
inspector::ComponentInspector ci("Node");
inspector::registerProperties(myNodeComp, ci);
ci.draw();
```

### Reflection

```cpp
// Sync node transform caches before reading:
inspector::pullNodeTransformCaches(registry);

auto props = inspector::getEntityProperties(registry, entity);
for (auto& p : props) {
    ofLogNotice() << p.name << " (" << inspector::getPinTypeName(p.type) << ")";
}

// Push cache changes back to the live node after writes:
inspector::pushNodeTransformCaches(registry);
```

### Transform Origin (optional pivot)

```cpp
// Adds an Origin combo to the Node inspector
registry.emplace<inspector::transform_origin_component>(entity);
```

## Example

See [`example-inspector/`](example-inspector/) — a three-panel demo:

- **Scene** — entity list with selection
- **Viewport** — live 3D render of demo entities  
- **Properties** — `inspector::inspectEntity` panel for the selected entity

## License

MIT
