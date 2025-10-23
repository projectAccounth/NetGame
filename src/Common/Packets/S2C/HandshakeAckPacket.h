#pragma once

#include "Common/Network/Packet.h"

struct HandshakeAckPacket : public Packet {
    bool success;
    std::string message;

    HandshakeAckPacket() = default;
    HandshakeAckPacket(bool success, std::string msg): success(success), message(std::move(msg)) {}

    constexpr static uint32_t PACKET_ID = 5;

    uint32_t GetPacketID() const override { return PACKET_ID; }

    void Encode(PacketCodec& codec) const override {
        codec.Write(success);
        codec.WriteString(message);
    }

    void Decode(PacketCodec& codec) override {
        success = codec.Read<bool>();
        message = codec.ReadString();
    }
};