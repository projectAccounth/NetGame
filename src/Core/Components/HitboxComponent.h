#pragma once

#include "Component.h"
#include "Core/Objects/CollisionGroups.h"
#include "Util/GMath.h"
#include "Core/Objects/Hitbox/Hitbox.h"
#include "Core/Objects/Hitbox/HitboxShapeRegistry.h"

class HitboxComponent : public Component {
private:
    std::vector<Hitbox> hitboxes;

public:
    HitboxComponent() {
    }

    void AddHitbox(std::unique_ptr<HitboxShape> shape, CollisionGroup group = CollisionGroup::DefaultCollidable, bool isTrigger = false) {
        hitboxes.emplace_back(std::move(shape), group, isTrigger);
    }

    void ClearHitboxes() {
        hitboxes.clear();
    }

    const std::vector<Hitbox>& GetHitboxes() const {
        return hitboxes;
    }

    void Encode(PacketCodec& codec) const override {
        codec.Write<uint32_t>(static_cast<uint32_t>(hitboxes.size()));
        for (const auto& hb : hitboxes) {
            auto& ref = *hb.shape;
            std::string typeName = HitboxShapeRegistry::GetName(std::type_index(typeid(ref)));
            codec.WriteString(typeName);

            hb.shape->Encode(codec);
            codec.Write<bool>(hb.isTrigger);
            codec.Write<uint8_t>(static_cast<uint8_t>(hb.group));
        }
    }

    void Decode(PacketCodec& codec) override {
        hitboxes.clear();
        uint32_t count = codec.Read<uint32_t>();
        for (uint32_t i = 0; i < count; ++i) {
            std::string typeName = codec.ReadString();

            auto shape = HitboxShapeRegistry::Create(typeName);
            shape->Decode(codec);

            bool trigger = codec.Read<bool>();
            CollisionGroup group = static_cast<CollisionGroup>(codec.Read<uint8_t>());
            hitboxes.emplace_back(std::move(shape), group, trigger);
        }
    }

    std::string Dump() const override {
        std::string result = "HitboxComponent(hitboxes=[";
        for (const auto& hb : hitboxes) {
            result += "{shape=" + hb.shape->ToString() +
                      ", trigger=" + (hb.isTrigger ? "true" : "false") +
                      ", group=" + std::to_string(static_cast<int>(hb.group)) + "}, ";
        }
        if (!hitboxes.empty()) result.pop_back(), result.pop_back(); // remove trailing comma
        result += "])";
        return result;
    }
};