// RenderSystem.cpp
#include "Client/Render/RenderSystem.h"
#include "Client/Render/World/WorldRenderer.h"
#include "Core/World/World.h"

RenderSystem::RenderSystem(std::unique_ptr<IRenderer> backend)
    : backend(std::move(backend)) {}

void RenderSystem::Initialize() {
    backend->Initialize();
    worldRenderer = std::make_unique<WorldRenderer>(*backend);
}

void RenderSystem::SetTitle(const std::string& title) {
    backend->SetTitle(title);
}

void RenderSystem::RenderWorld(const World& world) {
    if (!worldRenderer) return;
    backend->ClearFrame();
    worldRenderer->Render(world);
    backend->PresentFrame();
}

IRenderer& RenderSystem::GetBackend() { return *backend; }
WorldRenderer& RenderSystem::GetWorldRenderer() { return *worldRenderer; }
