#pragma once

#include "Common/Network/Packet.h"

class ChatMessagePacket : public Packet {
public:
    std::string message;
    std::string sender;

    static constexpr uint32_t PACKET_ID = 2;

    uint32_t GetPacketID() const override { return PACKET_ID; }

    void Encode(PacketCodec& codec) const override {
        codec.WriteString(sender);
        codec.WriteString(message);
    }

    void Decode(PacketCodec& codec) override {
        sender = codec.ReadString();
        message = codec.ReadString();
    }
};