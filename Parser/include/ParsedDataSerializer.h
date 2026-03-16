#pragma once
#include "IParsedDataSerializer.h"

class ParsedDataSerializer: public IParsedDataSerializer{
    public:
    ParsedDataSerializer()=default;
    std::vector<uint8_t> serialize(const std::unique_ptr<TransactionMessage>& message) override;
    
};

