#include "CaptureConfig.h"
#include <INIReader.h>
#include <sstream>


std::string CaptureConfig::SUBJECT_NAME;
std::string CaptureConfig::STREAM_NAME;
std::string CaptureConfig::natsURL;
std::string CaptureConfig::inputFolderPath;
std::string CaptureConfig::packetSnifferMode;
std::string CaptureConfig::interfaceName;
int CaptureConfig::microsecondMultiplier;
int CaptureConfig::ethernetHeaderSize;
int CaptureConfig::minCaptureLength;
int CaptureConfig::udpProtocolNumber;
std::string CaptureConfig::timeFormat;

int CaptureConfig::sourceIPOffset;
int CaptureConfig::destinationIPOffset;
int CaptureConfig::sourcePortOffset;
int CaptureConfig::destinationPortOffset;
int CaptureConfig::byteShift;

std::unordered_map<std::string, bool> CaptureConfig::sourceIPs;
std::unordered_map<std::string, bool> CaptureConfig::destinationIPs;
std::unordered_map<std::string, bool> CaptureConfig::sourcePorts;
std::unordered_map<std::string, bool> CaptureConfig::destinationPorts;


// Helper to split a comma-separated string and store in map
void CaptureConfig::insertCommaSeparatedValues(const std::string& input, std::unordered_map<std::string, bool>& map) {
    std::istringstream ss(input);
    std::string token;
    while (std::getline(ss, token, ',')) {
        if (!token.empty()) {
            map[token] = true;
        }
    }
}

void CaptureConfig::loadFromINIFile(const std::string& configPath) {
    INIReader reader(configPath);

    if (reader.ParseError() < 0) {
        std::cout << "Can't load " << configPath << std::endl;
        return;
    }

    SUBJECT_NAME = reader.Get("capture", "SUBJECT_NAME", "capture.packet");
    STREAM_NAME = reader.Get("capture", "STREAM_NAME", "capture_stream");
    natsURL = reader.Get("capture", "natsURL", "nats://localhost:4222");
    inputFolderPath = reader.Get("capture", "inputFolderPath", "../inputPcapFiles");
    interfaceName = reader.Get("capture", "interfaceName", "any");
    packetSnifferMode = reader.Get("capture", "packetSnifferMode", "File");
    microsecondMultiplier = reader.GetInteger("capture", "microsecondMultiplier", 1000000);
    ethernetHeaderSize = reader.GetInteger("capture", "ethernetHeaderSize", 14);
    minCaptureLength = reader.GetInteger("capture", "minCaptureLength", 42);
    udpProtocolNumber = reader.GetInteger("capture", "udpProtocolNumber", 17);
    timeFormat = reader.Get("capture", "timeFormat", "%Y-%m-%d %H:%M:%S");

    sourceIPOffset = reader.GetInteger("offset", "sourceIPOffset", 12);
    destinationIPOffset = reader.GetInteger("offset", "destinationIPOffset", 16);
    sourcePortOffset = reader.GetInteger("offset", "sourcePortOffset", 20);
    destinationPortOffset = reader.GetInteger("offset", "destinationPortOffset", 22);
    byteShift = reader.GetInteger("offset", "byteShift", 8);

    insertCommaSeparatedValues(reader.Get("filter", "sourceIPs", ""), sourceIPs);
    insertCommaSeparatedValues(reader.Get("filter", "destinationIPs", ""), destinationIPs);
    insertCommaSeparatedValues(reader.Get("filter", "sourcePorts", ""), sourcePorts);
    insertCommaSeparatedValues(reader.Get("filter", "destinationPorts", ""), destinationPorts);
}
