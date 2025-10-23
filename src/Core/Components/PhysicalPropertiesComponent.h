#pragma once

#include "Component.h"

class PhysicalPropertiesComponent : public Component {
private:
    float mass = 1.0f; // Default to 1 to avoid divide-by-zero
    bool anchored = false;
public:
    PhysicalPropertiesComponent() = default;
    PhysicalPropertiesComponent(float m) : mass(std::max(0.001f, m)) {}

    float GetMass() const { return mass; }
    void SetMass(float m) { mass = std::max(0.001f, m); } // Prevent zero mass

    void SetAnchored(bool val) { anchored = val; }
    bool IsAnchored() const { return anchored; }

    void Encode(PacketCodec& codec) const override {
        codec.Write(mass);
        codec.Write(anchored);
    }

    void Decode(PacketCodec& codec) override {
        mass = codec.Read<float>();
        anchored = codec.Read<bool>();
    }

    std::string Dump() const override {
        return "PhysicalPropertiesComponent(mass=" + std::to_string(mass) + ")";
    }
};