#pragma once

#include <iostream>
#include <string>
#include <unordered_map>


namespace CaptureConfig {
    extern std::string SUBJECT_NAME;
    extern std::string STREAM_NAME;
    extern std::string natsURL;
    extern std::string inputFolderPath;
    extern std::string packetSnifferMode;
    extern std::string interfaceName;
    extern int microsecondMultiplier;
    extern int ethernetHeaderSize;
    extern int minCaptureLength;
    extern int udpProtocolNumber;
    extern std::string timeFormat;
    
    extern int sourceIPOffset;
    extern int destinationIPOffset;
    extern int sourcePortOffset;
    extern int destinationPortOffset;
    extern int byteShift;

    extern std::unordered_map<std::string, bool> sourceIPs;
    extern std::unordered_map<std::string, bool> destinationIPs;
    extern std::unordered_map<std::string, bool> sourcePorts;
    extern std::unordered_map<std::string, bool> destinationPorts;

    void insertCommaSeparatedValues(const std::string& input, std::unordered_map<std::string, bool>& map);
    void loadFromINIFile(const std::string& configPath);
}
