#pragma once

#include "GameObject.h"
#include "Util/GMath.h"
#include "Util/UUID.hpp"
#include <typeindex>

class TreeObject: public GameObject {
public:
    TreeObject() = default;
    TreeObject(const Vector2d& pos, const Rect2d& hitbox)
        : GameObject() { SetPosition(pos); GetHitbox()->AddHitbox(hitbox); }
    ~TreeObject() = default;

    enum class TreeVariant {

    };
private:
    TreeVariant variant;
};