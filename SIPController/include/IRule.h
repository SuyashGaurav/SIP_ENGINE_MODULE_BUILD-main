#pragma once

#include "TransactionMessage.h"
#include <memory>

class IRule {
    public:
        virtual ~IRule() = default;
        virtual bool match(const std::unique_ptr<TransactionMessage>& message) = 0;
};
