#pragma once

#include <pcap.h>


class IPacketHandler {
public:
    virtual ~IPacketHandler() = default;
    virtual bool isRelevantPacket(const pcap_pkthdr* header, const u_char* packet) = 0;
};
