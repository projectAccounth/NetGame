#pragma once
#include <typeindex>
#include "WorldRenderer.h"
#include "../Entity/EntityRendererBase.h"

template <typename T, typename RendererT>
void WorldRenderer::RegisterRenderer() {
    static_assert(std::is_base_of_v<EntityRendererBase, RendererT>,
                  "RendererT must derive from EntityRendererBase");
    renderers[typeid(T)] = std::make_unique<RendererT>();
}

template <typename T>
EntityRendererBase* WorldRenderer::GetRenderer() const {
    auto it = renderers.find(typeid(T));
    return (it != renderers.end()) ? it->second.get() : nullptr;
}
