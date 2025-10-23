#pragma once
#include "Client/Render/RenderInfo.h"
#include "EntityRendererBase.h"
#include "Core/Objects/PlayerEntity.h"
#include "Core/Components/TransformComponent.h"
#include "Core/Resources/ResourceLocation.h"
#include <iostream>

class PlayerRenderer : public EntityRenderer<PlayerEntity> {
protected:
    ResourceLocation playerTexture = ResourceLocation("player");
public:
    RenderInfo GetRenderInfo() const {
        return {
            RenderLayer::Entities,
            0.0  
        };
    }

    void RenderEntity(const PlayerEntity& player, IRenderer& backend, const Camera& camera) override {
        auto* transform = player.GetComponent<TransformComponent>();
        if (!transform) return;
        auto pos = transform->GetPosition();

        backend.DrawGameObject(player);
        // std::cout << "[PlayerRenderer] Drawing Player at ("
                  // << pos.x << ", " << pos.y << ")\n";
    }
};
