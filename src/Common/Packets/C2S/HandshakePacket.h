#pragma once

#include "Util/UUID.hpp"
#include "Common/Network/Packet.h"

struct HandshakePacket : public Packet {
    uint32_t clientVersion;
    Util::UUID authToken;

    constexpr static uint32_t PACKET_ID = 6;

    uint32_t GetPacketID() const override { return PACKET_ID; }

    void Encode(PacketCodec& codec) const override {
        codec.Write(clientVersion);
        codec.WriteUUID(authToken);
    }

    void Decode(PacketCodec& codec) override {
        clientVersion = codec.Read<uint32_t>();
        authToken = codec.ReadUUID();
    }
};