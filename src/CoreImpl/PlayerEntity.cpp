#include "Core/Objects/PlayerEntity.h"
#include "Core/Objects/Entity.h"
#include "Core/Components/TransformComponent.h"
#include "Core/Objects/AliveEntity.h"

void PlayerEntity::Tick(float dt) {
    if (IsDestroyed()) return;
    AliveEntity::Tick(dt);
}