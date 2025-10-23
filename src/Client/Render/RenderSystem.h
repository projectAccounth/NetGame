#pragma once
#include "Pipelines/IRenderer.h"
#include <memory>
#include <string>

class World;
class WorldRenderer;

class RenderSystem {
private:
    std::unique_ptr<IRenderer> backend;
    std::unique_ptr<WorldRenderer> worldRenderer;

public:
    explicit RenderSystem(std::unique_ptr<IRenderer> backend);

    void Initialize();
    void SetTitle(const std::string& title);

    void RenderWorld(const World& world);

    IRenderer& GetBackend();
    WorldRenderer& GetWorldRenderer();
};