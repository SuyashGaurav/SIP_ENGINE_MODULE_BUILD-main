#pragma once

#include "IPacketSerializer.h"


class SIPPacketSerializer : public IPacketSerializer {
    public:
        std::vector<uint8_t> serializePacket(const SIPPacket& packet) override;
};
