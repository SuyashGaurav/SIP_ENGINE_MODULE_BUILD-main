#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include "TransactionMessage.h"

class IPacketSerializer{
    public:
        virtual std::vector<uint8_t> serializePacket(const std::unique_ptr<TransactionMessage>& message)=0;
        virtual ~IPacketSerializer(){}
};
