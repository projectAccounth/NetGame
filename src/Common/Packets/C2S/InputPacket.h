#pragma once

#include "Common/Network/Packet.h"

class InputPacket : public Packet {
public:
    Util::UUID entityUuid;
    

    static constexpr uint32_t PACKET_ID = 4;

    uint32_t GetPacketID() const override { return PACKET_ID; }

    void Encode(PacketCodec& codec) const override {

    }

    void Decode(PacketCodec& codec) override {

    }
};