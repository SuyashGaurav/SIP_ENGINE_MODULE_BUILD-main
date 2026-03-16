#pragma once 
#include "IPacketSerializer.h"


class ParsedPacketSerializer: public IPacketSerializer{
    public:
        ParsedPacketSerializer()=default;
        std::vector<uint8_t> serializePacket(const std::unique_ptr<TransactionMessage>& message) override;
};
