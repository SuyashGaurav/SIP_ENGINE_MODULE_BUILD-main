#pragma once
#include <cstdint>
#include <string>

class SIPMessage{
    public:
        std::string callId;
        std::string cSeq;
        std::string fromAddress;
        std::string toAddress;
        std::string userAgent;
        std::string fromExtension;
        std::string toExtension;
        std::string timestamp;
};
