#pragma once
#include "IPacketDeserializer.h"
#include "SIPPacket.h"

class PacketDeserializer: IPacketDeserializer{
    public:
    PacketDeserializer()=default;
    SIPPacket deserialize(const uint8_t* data,size_t length) override;

};

