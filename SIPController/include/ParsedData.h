#pragma once
#include <optional>
#include <map>
#include "SIPMessage.h"
#include "SDP.h"

class ParsedData {
    public:
        uint64_t seqNo;
        std::string sourceIp;
        std::string destinationIp;
        int protocol;
        uint16_t sourcePort;
        uint16_t destinationPort;
        SIPMessage sipMessage;
        std::optional<std::map<std::string,SDP>> sdpInfo;
};
