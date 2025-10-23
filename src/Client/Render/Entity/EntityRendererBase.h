#pragma once
#include "Client/Render/Pipelines/IRenderer.h"
#include "Core/Objects/GameObject.h"
#include "Client/Render/Camera.h"
#include "Client/Render/RenderInfo.h"

// Base interface for any renderer that knows how to draw a GameObject
class EntityRendererBase {
public:
    virtual ~EntityRendererBase() = default;
    virtual void Render(const GameObject& object, IRenderer& backend, const Camera& camera) = 0;
};

// Template for typed entity renderers
template <typename T>
class EntityRenderer : public EntityRendererBase {
public:
    virtual void RenderEntity(const T& entity, IRenderer& backend, const Camera& camera) = 0;
    virtual RenderInfo GetRenderInfo() const = 0;

    void Render(const GameObject& object, IRenderer& backend, const Camera& camera) override {
        const T& derived = static_cast<const T&>(object);
        if (camera.OutsideCamera(object)) return;
        RenderEntity(derived, backend, camera);
    }
};