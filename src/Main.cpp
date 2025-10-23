#include "Core/Objects/GameObject.h"
#define SDL_MAIN_HANDLED

#include "GameLoop.h"
#include "Core/World/World.h"
#include "Core/Components/TransformComponent.h"
#include "Core/Components/HitboxComponent.h"
#include "Core/Objects/PlayerEntity.h"
#include "Client/Render/RenderSystem.h"
#include "Client/Render/Entity/PlayerRenderer.h"
#include "Client/Render/Pipelines/SDL2Renderer.h"
#include "Client/Render/World/WorldRenderer.h"
#include "Core/Objects/Hitbox/Hitbox.h"

int main() {
    auto backend = std::make_unique<SDL2Renderer>();
    RenderSystem renderSystem(std::move(backend));
    renderSystem.Initialize();
    renderSystem.SetTitle("Demo");

    // Client client;
    // auto& inputService = client.GetInputService();

    World world(true);
    auto& object = world.SpawnObject<PlayerEntity>();
    auto& wall = world.SpawnObject<GameObject>();
    object.SetPosition({5, 60});
    object.GetTransform()->SetVelocity({ 80, 0 });
    object.GetHitbox()->AddHitbox(std::make_unique<RectShape>(Rect2d { 0.0, 0.0, 15.0, 15.0 }, 0.0f));
    // object.GetHitbox()->AddHitbox(std::make_unique<CircleShape>(Vector2d(0, 0), 15.0f));
    object.GetPhysicalProperties()->SetMass(100);

    wall.SetPosition({50, 40});
    wall.GetHitbox()->AddHitbox(std::make_unique<RectShape>(Rect2d { 0.0, 0.0, 25.0, 100.0 }, 0.0f));
    wall.GetPhysicalProperties()->SetAnchored(false);
    wall.GetPhysicalProperties()->SetMass(1);
    renderSystem.GetWorldRenderer().RegisterRenderer<PlayerEntity, PlayerRenderer>();

    wall.Collided.ConnectPersistent([](GameObject* gameObject) { std::cout << "Hit"; });

    GameLoop loop(world, renderSystem);

    loop.Start();

    return 0;
}