#pragma once

#include "Component.h"

class HealthComponent : public Component {
private:
    int health;
    int maxHealth;
    int healAmount = 0;
public:
    HealthComponent(int maxHealth)
        : health(maxHealth), maxHealth(maxHealth) {}

    HealthComponent()
        : health(100), maxHealth(100) {};

    int GetHealth() const { return health; }
    int GetMaxHealth() const { return maxHealth; }

    void SetHealth(int h) {
        health = std::clamp(h, 0, maxHealth);
        if (health == 0) {
        }
    }

    void Heal(int amount) {
        SetHealth(health + amount);
    }

    void TakeDamage(int amount) {
        SetHealth(health - amount);
    }

    void SetMaxHealth(int amount) {
        maxHealth = std::max(0, amount);
        health = std::clamp(health, 0, maxHealth);
    }

    // Network encode/decode
    void Encode(PacketCodec& codec) const override {
        codec.Write(health);
        codec.Write(maxHealth);
        codec.Write(healAmount);
    }

    void Decode(PacketCodec& codec) override{
        health = codec.Read<int>();
        maxHealth = codec.Read<int>();
        healAmount = codec.Read<int>();
    }

    int GetHealAmount() const {
        return healAmount;
    }

    void SetHealAmount(int amount) {
        healAmount = amount;
    }

    void Tick(float dt) override {
        if (healAmount > 0 && health < maxHealth) {
            Heal(healAmount);
        }
    }

    std::string Dump() const override {
        return "HealthComponent(health=" + std::to_string(health) +
               ", maxHealth=" + std::to_string(maxHealth) +
               ", healAmount=" + std::to_string(healAmount) + ")";
    }
};