#pragma once

#include "TransactionMessage.h"
#include <cstdint>
#include <memory>
#include <cstring>


class IPacketDeserializer {
    public:
        virtual std::unique_ptr<TransactionMessage> deserializePacket(const uint8_t* data, size_t length) = 0;
        virtual ~IPacketDeserializer() {}
};
