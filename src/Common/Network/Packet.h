#pragma once
#include "PacketCodec.h"
#include <cstdint>

//
// Base Packet class
//
class Packet {
public:
    virtual ~Packet() = default;

    virtual uint32_t GetPacketID() const = 0;

    virtual void Encode(PacketCodec& codec) const = 0;
    virtual void Decode(PacketCodec& codec) = 0;
};
