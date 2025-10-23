#pragma once

#include "Core/Objects/CollisionGroups.h"
#include "HitboxShape.h"
#include "Util/GMath.h"
#include <memory>

struct Hitbox {
    std::unique_ptr<HitboxShape> shape;
    bool isTrigger;
    CollisionGroup group;

    Hitbox(std::unique_ptr<HitboxShape> s, CollisionGroup g, bool t = false)
        : shape(std::move(s)), group(g), isTrigger(t) {}
};