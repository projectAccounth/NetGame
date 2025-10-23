#include "Client/Render/Pipelines/DummyRenderer.h"
#include "Core/Objects/GameObject.h"
#include "Util/GMath.h"
#include <iostream>

void DummyRenderer::Initialize() {
    std::cout << "[Renderer] Initialized.\n";
}

void DummyRenderer::ClearFrame() {
    std::cout << "[Renderer] Frame cleared.\n";
}

void DummyRenderer::DrawGameObject(const GameObject& obj) {
    auto pos = obj.GetPosition();
    std::cout << "[Renderer] Drawing object #" << obj.GetID()
              << " at (" << pos.x << ", " << pos.y << ")\n";
}

void DummyRenderer::PresentFrame() {
    std::cout << "[Renderer] Frame presented.\n";
}

void DummyRenderer::SetTitle(const std::string& title) {
    std::cout << "[Renderer] Window title: " << title << "\n";
}