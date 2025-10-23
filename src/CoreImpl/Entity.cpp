#include "Core/Objects/Entity.h"
#include "Core/Components/EntityAttributesComponent.h"
#include "Core/Objects/GameObject.h"

Entity::Entity()
    : age(0) {
        AddComponent<EntityAttributesComponent>();
        LockComponent<EntityAttributesComponent>();
    }

Entity::~Entity() = default;

void Entity::Tick(float dt) {
    if (IsDestroyed() || !ShouldTick()) return;
    age++;
    GameObject::Tick(dt);
}

int Entity::GetAge() const { return age; }
void Entity::SetAge(int a) { age = a; }