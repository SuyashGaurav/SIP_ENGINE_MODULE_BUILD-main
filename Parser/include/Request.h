#pragma once
#include "TransactionMessage.h"

class Request:public TransactionMessage{
    public:
    std::string sipMethod;
};