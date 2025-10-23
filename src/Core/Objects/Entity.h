
#pragma once
#include "Core/Components/TransformComponent.h"
#include "GameObject.h"
#include "Util/GMath.h"

class Entity : public GameObject {
private:
    int age;
public:
    Entity();
    ~Entity() override;

    void Tick(float dt) override;

    int GetAge() const;
    void SetAge(int a);
};
