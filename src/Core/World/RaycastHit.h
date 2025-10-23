#pragma once

#include "Core/Objects/GameObject.h"
#include "Util/GMath.h"

struct RaycastHit {
    GameObject* object;
    Vector2d point;
    double distance;
};
