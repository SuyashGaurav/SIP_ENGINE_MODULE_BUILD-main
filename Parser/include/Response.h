#pragma once
#include "TransactionMessage.h"

class Response : public TransactionMessage {
    public:
    std::string status;
    int statusCode;
};