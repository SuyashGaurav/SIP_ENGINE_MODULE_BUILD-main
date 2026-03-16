#pragma once
#include "IPacketDeserializer.h"


class ParsedPacketDeserializer: public IPacketDeserializer{
    public:
        ParsedPacketDeserializer() = default;
        std::unique_ptr<TransactionMessage> deserializePacket(const uint8_t* data, size_t length) override;
};
