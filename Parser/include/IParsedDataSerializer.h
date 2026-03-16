#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include "TransactionMessage.h"

class IParsedDataSerializer{
    public:
        virtual std::vector<uint8_t> serialize(const std::unique_ptr<TransactionMessage>& message)=0;
        virtual ~IParsedDataSerializer(){}
};