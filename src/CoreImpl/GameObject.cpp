#include "Core/Objects/GameObject.h"
#include "Core/Components/HitboxComponent.h"
#include "Core/Components/TransformComponent.h"
#include "Core/Components/PhysicalPropertiesComponent.h"
#include "Core/World/World.h"
#include "Core/Components/Component.h"
#include <stdexcept>
#include "Core/World/ServerWorld.h"

int GameObject::next_id = 0;

GameObject::GameObject()
    : id(++next_id), shouldDestroy(false) {
        // Every GameObject has a TransformComponent by default
        AddComponent<TransformComponent>();
        AddComponent<HitboxComponent>();
        AddComponent<PhysicalPropertiesComponent>();

        LockComponent<TransformComponent>();
        LockComponent<HitboxComponent>();
        LockComponent<PhysicalPropertiesComponent>();
    }

GameObject::~GameObject() {
}

int GameObject::GetID() const { return id; }

Vector2d GameObject::GetPosition() const { return GetTransform()->GetPosition(); }

void GameObject::SetPosition(const Vector2d& pos) {
    GetTransform()->SetPosition(pos);
    Moved.Fire(pos);
}

void GameObject::Move(const Vector2d& delta) {
    GetTransform()->Translate(delta);
    if (delta.LengthSquared() != 0) Moved.Fire(GetPosition());
}

void GameObject::SetDirty(bool val) {
    Instance::SetDirty(val);
    if (dirty && world && world->IsServer())
        if (auto serverWorld = dynamic_cast<ServerWorld*>(world)) 
            serverWorld->QueueReplication(this);
}

void GameObject::Tick(float dt) {
    Ticked.Fire(dt);
    Instance::Tick(dt);
}

void GameObject::Destroy() {
    Destroyed.Fire();
    shouldDestroy = true;
}

TransformComponent* GameObject::GetTransform() const {
    return GetComponent<TransformComponent>();
}

HitboxComponent* GameObject::GetHitbox() const {
    return GetComponent<HitboxComponent>();
}

PhysicalPropertiesComponent* GameObject::GetPhysicalProperties() const {
    return GetComponent<PhysicalPropertiesComponent>();
}

void GameObject::SetVelocity(const Vector2d& v) { GetTransform()->SetVelocity(v); }
Vector2d GameObject::GetVelocity() const { return GetTransform()->GetVelocity(); }

void GameObject::SetAcceleration(const Vector2d& a) { GetTransform()->SetAcceleration(a); }
Vector2d GameObject::GetAcceleration() const { return GetTransform()->GetAcceleration(); }
