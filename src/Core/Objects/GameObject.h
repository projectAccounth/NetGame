#pragma once

#include "Core/Components/PhysicalPropertiesComponent.h"
#include "Util/GMath.h"
#include "Util/UUID.hpp"
#include "Core/Components/TransformComponent.h"
#include "Core/Components/HitboxComponent.h"
#include "Core/Connection.h"
#include "Core/Instance.h"
#include <typeindex>

using Util::UUID;

class GameObject : public Instance {
protected:
    int id;
    static int next_id;

    bool shouldDestroy;
    bool shouldRender = true;
public:
    Signal<float> Ticked;
    Signal<Vector2d> Moved;
    Signal<GameObject*> Collided;

    GameObject();
    virtual ~GameObject();

    int GetID() const;

    bool ShouldRender() const { return shouldRender; }
    void SetShouldRender(bool val) { shouldRender = val; }

    Vector2d GetPosition() const;
    void SetPosition(const Vector2d& pos);
    void Move(const Vector2d& delta);

    bool ShouldTick() const { return true; };
    bool ShouldDestroy() const { return shouldDestroy; };

    void SetVelocity(const Vector2d& v);
    Vector2d GetVelocity() const;

    void SetAcceleration(const Vector2d& a);
    Vector2d GetAcceleration() const;

    virtual void Tick(float dt) override;
    virtual void Destroy() override;

    TransformComponent* GetTransform() const;
    HitboxComponent* GetHitbox() const;
    PhysicalPropertiesComponent* GetPhysicalProperties() const;

    void SetDirty(bool val = true) override;

    virtual void OnCollision(GameObject* other) {
        Collided.Fire(other);
    }
};