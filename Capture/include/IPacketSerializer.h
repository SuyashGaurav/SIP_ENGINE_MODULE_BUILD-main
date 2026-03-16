#pragma once

#include "SIPPacket.h"


class IPacketSerializer {
    public:
        virtual ~IPacketSerializer() = default;
        virtual std::vector<uint8_t> serializePacket(const SIPPacket& packet) = 0;
};
