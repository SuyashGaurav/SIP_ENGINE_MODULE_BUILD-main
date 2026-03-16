#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <optional>
#include "SDP.h"


class SessionContext {
public:
    std::string timestamp;
    std::string sourceIp;
    std::string destinationIp;
    int protocol;
    uint16_t sourcePort;
    uint16_t destinationPort;
    std::string cSeq;
    std::string fromAddress;
    std::string toAddress;
    std::string fromExtension;
    std::string toExtension;
    std::string userAgent;
    std::optional<std::map<std::string, SDP>> sdpInfo;
};
