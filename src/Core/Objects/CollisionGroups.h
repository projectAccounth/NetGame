#pragma once

#include <cstdint>

enum class CollisionGroup : uint8_t {
    DefaultCollidable,
    DefaultNonCollidable,
    Player,
    Projectile
};