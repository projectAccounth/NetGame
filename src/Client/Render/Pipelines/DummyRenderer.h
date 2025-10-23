#pragma once
#include "IRenderer.h"
#include <iostream>

class DummyRenderer : public IRenderer {
public:
    void Initialize() override;
    void ClearFrame() override;
    void DrawGameObject(const GameObject& obj) override;
    void PresentFrame() override;
    void SetTitle(const std::string& title) override;
};