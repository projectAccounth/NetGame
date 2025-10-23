#pragma once
#include "Common/Network/Packet.h"

class PlayerJoinPacket : public Packet {
public:
    std::string username;
    Util::UUID uuid;

    static constexpr uint32_t PACKET_ID = 1;

    uint32_t GetPacketID() const override { return PACKET_ID; }

    void Encode(PacketCodec& codec) const override {
        codec.WriteString(username);
        codec.WriteUUID(uuid);
    }

    void Decode(PacketCodec& codec) override {
        username = codec.ReadString();
        uuid = codec.ReadUUID();
    }
};