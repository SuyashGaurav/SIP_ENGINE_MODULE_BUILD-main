#pragma once
#include "ParsedData.h"
#include <string>


class TransactionMessage{
    public:
        std::string transactionId;
        ParsedData parsedData;
        virtual ~TransactionMessage() {}
};
