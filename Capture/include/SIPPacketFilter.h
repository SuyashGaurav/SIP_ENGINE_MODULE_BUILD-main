#pragma once

#include "CaptureConfig.h"


class SIPPacketFilter {
    public:
        bool matchIPs(const std::string& sourceIP, const std::string& destinationIP) {
            if(CaptureConfig::sourceIPs.empty() || CaptureConfig::destinationIPs.empty()) return true;
            return CaptureConfig::sourceIPs[sourceIP] && CaptureConfig::destinationIPs[destinationIP];
        }
        bool matchPorts(const std::string& sourcePort, const std::string& destinationPort) {
            if(CaptureConfig::sourcePorts.empty() || CaptureConfig::destinationPorts.empty()) return true;
            return CaptureConfig::sourcePorts[sourcePort] || CaptureConfig::destinationPorts[destinationPort];
        }
};
