#pragma once
#include "Core/World/World.h"
#include "../Pipelines/IRenderer.h"
#include <unordered_map>
#include <typeindex>
#include <memory>

class EntityRendererBase;

class WorldRenderer {
private:
    IRenderer& backend;
    std::unordered_map<std::type_index, std::unique_ptr<EntityRendererBase>> renderers;

public:
    explicit WorldRenderer(IRenderer& backend);
    ~WorldRenderer();

    void Render(const World& world);

    template <typename T, typename RendererT>
    void RegisterRenderer();

    template <typename T>
    EntityRendererBase* GetRenderer() const;
};

#include "WorldRenderer.inl"
