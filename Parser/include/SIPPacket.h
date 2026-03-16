#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include <string>

struct SIPPacket {
    uint64_t sequenceNo;
    std::vector<uint8_t> packetData;
    std::string arrivalTime;
};
