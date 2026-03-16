#pragma once

#include "IPacketHandler.h"
#include "LogMacros.h"
#include <cstring>


class SIPPacketHandler : public IPacketHandler {
    public:
        bool isRelevantPacket(const pcap_pkthdr* header, const u_char* packet) override;
};
