#pragma once

#include "Component.h"

class EntityAttributesComponent : public Component {
private:
    double moveSpeed = 1;
    bool canConsumeItems = false;
public:

    EntityAttributesComponent() = default;
    EntityAttributesComponent(double mvs, bool canConsumeItems): moveSpeed(mvs), canConsumeItems(canConsumeItems) {}

    void Encode(PacketCodec& codec) const override {
        codec.Write(moveSpeed);
        codec.Write(canConsumeItems);
    }

    void Decode(PacketCodec& codec) override {
        moveSpeed = codec.Read<double>();
        canConsumeItems = codec.Read<bool>();
    }

    void Tick(float dt) override {
    }

    std::string Dump() const override {
        return "EntityAttributesComponent()";
    }

    ~EntityAttributesComponent() = default;
};