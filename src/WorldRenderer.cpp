#include "Client/Render/World/WorldRenderer.h"
#include "Client/Render/Entity/EntityRendererBase.h"
#include "Core/Objects/GameObject.h"

WorldRenderer::WorldRenderer(IRenderer& backend)
    : backend(backend) {}

WorldRenderer::~WorldRenderer() = default;

void WorldRenderer::Render(const World& world) {
    for (const auto& obj : world.GetObjects()) {
        if (!obj || !obj->ShouldRender()) continue;
        std::type_index typeIdx(typeid(*obj));
        auto it = renderers.find(typeIdx);
        if (it != renderers.end()) {
            it->second->Render(*obj, backend, Camera());
        } else {
            backend.DrawGameObject(*obj);
        }
    }
}