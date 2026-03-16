#pragma once
#include <cstdint>
#include "SIPPacket.h"


class IPacketDeserializer{
    public:
        virtual SIPPacket deserialize(const uint8_t* data,size_t length )=0;
        virtual ~IPacketDeserializer(){}
};