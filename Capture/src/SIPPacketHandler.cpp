#include "CaptureConfig.h"
#include "SIPPacketHandler.h"
#include "SIPPacketFilter.h"


bool SIPPacketHandler::isRelevantPacket(const pcap_pkthdr* header, const u_char* packet) {
    if (header->caplen <= CaptureConfig::minCaptureLength){
        LOG_TRACE("Packet too short (caplen: {})", header->caplen);
        return false; // 14 (Ethernet) + 20 (IP) + 8 (UDP)
    } 

    const uint8_t* ip_header = packet + CaptureConfig::ethernetHeaderSize;
    // uint8_t protocol = ip_header[9];

    SIPPacketFilter sipPacketFilter;

    std::string sourceIP = std::to_string(ip_header[CaptureConfig::sourceIPOffset]) + "." +
                           std::to_string(ip_header[CaptureConfig::sourceIPOffset + 1]) + "." +
                           std::to_string(ip_header[CaptureConfig::sourceIPOffset + 2]) + "." +
                           std::to_string(ip_header[CaptureConfig::sourceIPOffset + 3]);

    std::string destinationIP = std::to_string(ip_header[CaptureConfig::destinationIPOffset]) + "." +
                                std::to_string(ip_header[CaptureConfig::destinationIPOffset + 1]) + "." +
                                std::to_string(ip_header[CaptureConfig::destinationIPOffset + 2]) + "." +
                                std::to_string(ip_header[CaptureConfig::destinationIPOffset + 3]);

    if(sipPacketFilter.matchIPs(sourceIP, destinationIP) == false) {
        LOG_TRACE("IP filter failed: {} -> {}", sourceIP, destinationIP);
        return false;
    }

    uint16_t sourcePort = (ip_header[CaptureConfig::sourcePortOffset] << CaptureConfig::byteShift) | ip_header[CaptureConfig::sourcePortOffset + 1];
    uint16_t destinationPort = (ip_header[CaptureConfig::destinationPortOffset] << CaptureConfig::byteShift) | ip_header[CaptureConfig::destinationPortOffset + 1];
        
    if(sipPacketFilter.matchPorts(std::to_string(sourcePort), std::to_string(destinationPort)) == false) {
        LOG_TRACE("Port filter failed: {} -> {}", sourcePort, destinationPort);
        return false;
    }

    const uint8_t* udp_payload = packet + CaptureConfig::minCaptureLength; // Skip Ethernet, IP, and UDP headers
    int payload_length = header->caplen - CaptureConfig::minCaptureLength;

    if (payload_length <= 0) return false;

    const char* sip_methods[] = {
        "INVITE", "REGISTER", "ACK", "BYE", "CANCEL", "OPTIONS", "PRACK",
        "SUBSCRIBE", "NOTIFY", "PUBLISH", "INFO", "REFER", "MESSAGE", "UPDATE",
        "SIP/2.0"
    };

    for (const char* method : sip_methods) {
        if (strncmp((const char*)udp_payload, method, strlen(method)) == 0) {
            return true;
        }
    }

    LOG_TRACE("Payload does not match any SIP method.");
    return false;
}
