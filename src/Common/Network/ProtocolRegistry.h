#pragma once

#include "Common/Packets/C2S/HandshakePacket.h"
#include "Common/Packets/S2C/HandshakeAckPacket.h"
#include "Common/Packets/S2C/ReplicationPacket.h"
#include "PacketRegistry.h"
#include "Common/Packets/2W/PlayerJoinPacket.h"
#include "Common/Packets/2W/ChatMessagePacket.h"
#include "Common/Packets/2W/PlayerLeavePacket.h"

class ProtocolRegistry {
public:
    static std::unique_ptr<PacketRegistry> Create() {
        auto reg = std::make_unique<PacketRegistry>();

        // Shared packets
        reg->Register<PlayerJoinPacket>(PlayerJoinPacket::PACKET_ID);
        reg->Register<ChatMessagePacket>(ChatMessagePacket::PACKET_ID);
        reg->Register<PlayerLeavePacket>(PlayerLeavePacket::PACKET_ID);

        reg->Register<HandshakeAckPacket>(HandshakeAckPacket::PACKET_ID);
        reg->Register<HandshakePacket>(HandshakePacket::PACKET_ID);

        reg->Register<ReplicationPacket>(ReplicationPacket::PACKET_ID);
        // reg->Register<InputPacket>(PlayerLeavePacket::PACKET_ID);

        return reg;
    }
};