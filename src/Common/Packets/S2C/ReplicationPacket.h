#pragma once

#include "Common/Network/Packet.h"
#include "Core/Objects/GameObject.h"
class ReplicationPacket : public Packet {
public:
    static constexpr uint32_t PACKET_ID = 42;
    uint32_t GetPacketID() const override { return PACKET_ID; }

    enum class ReplicationType : uint8_t {
        FullSync,
        Spawn,
        Destroy
    };

    struct ObjectState {
        Util::UUID uuid;
        ReplicationType type = ReplicationType::FullSync;
        std::unique_ptr<Instance> instance; // decoded instance data

        void Encode(PacketCodec& codec) const {
            codec.WriteUUID(uuid);
            codec.Write<uint8_t>(static_cast<uint8_t>(type));
            if (type != ReplicationType::Destroy && instance)
                instance->Encode(codec);
        }

        void Decode(PacketCodec& codec) {
            uuid = codec.ReadUUID();
            type = static_cast<ReplicationType>(codec.Read<uint8_t>());
            if (type != ReplicationType::Destroy) {
                // Create an empty instance based on the type encoded inside the object
                instance = std::make_unique<Instance>();
                instance->Decode(codec);
            }
        }
    };

private:
    std::vector<ObjectState> objects;

public:
    void AddObject(Instance* obj, ReplicationType type = ReplicationType::FullSync) {
        ObjectState state;
        state.uuid = obj->GetUUID();
        state.type = type;
        state.instance = std::make_unique<Instance>(*obj); // copy for sending
        objects.push_back(std::move(state));
    }

    const std::vector<ObjectState>& GetObjects() const { return objects; }

    void Encode(PacketCodec& codec) const override {
        codec.Write<uint32_t>(objects.size());
        for (const auto& obj : objects)
            obj.Encode(codec);
    }

    void Decode(PacketCodec& codec) override {
        uint32_t count = codec.Read<uint32_t>();
        objects.resize(count);
        for (uint32_t i = 0; i < count; ++i)
            objects[i].Decode(codec);
    }
};