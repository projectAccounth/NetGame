#pragma once

#include "Util/UUID.hpp"
#include <memory>

class GameObject;

class IWorld {
public:
    virtual Util::UUID AddObject(std::unique_ptr<GameObject> entity) = 0;
    virtual void RemoveObject(const GameObject* entity) = 0;
    virtual void Tick(float dt) = 0;
    virtual bool IsServer() = 0;
    virtual void SpawnPlayer() = 0;

    virtual ~IWorld() = default;
};