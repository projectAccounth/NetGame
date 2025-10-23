#pragma once

#include "Entity.h"
#include "Core/Components/HealthComponent.h"

class AliveEntity : public Entity {
private:
    bool alive;
public:

    Signal<> Dead;

    AliveEntity()
        : alive(true) {
            AddComponent<HealthComponent>(100);
        }
    ~AliveEntity() override = default;

    bool IsAlive() const { return alive; }
    void SetAlive(bool a) { alive = a; }

    HealthComponent* GetHealthComponent() const {
        if (!HasComponent<HealthComponent>())
            throw std::runtime_error("AliveEntity missing HealthComponent");
        return GetComponent<HealthComponent>();
    }

    int GetMaxHealth() const {
        return GetHealthComponent()->GetMaxHealth();
    }

    int GetHealth() const {
        return GetHealthComponent()->GetHealth();
    }

    int GetHealAmount() const {
        return GetHealthComponent()->GetHealAmount();
    }

    void SetHealth(int hp) {
        GetHealthComponent()->SetHealth(hp);
    }

    void SetMaxHealth(int hp) {
        GetHealthComponent()->SetMaxHealth(hp);
    }

    void SetHealAmount(int amount) const {
        GetHealthComponent()->SetHealAmount(amount);
    }

    void TakeDamage(int amount) {
        GetHealthComponent()->TakeDamage(amount);
    }

    void Tick(float dt) override {
        if (IsDestroyed() && !IsAlive()) return;

        Entity::Tick(dt);

        if (GetHealth() <= 0 && alive) {
            alive = false;
            Dead.Fire();
        }
    }
};