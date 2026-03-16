#include "SIPPacketSerializer.h"
#include <cstring>  // for std::memcpy

std::vector<uint8_t> SIPPacketSerializer::serializePacket(const SIPPacket& packet) {
    size_t arrivalTimeLen = packet.arrivalTime.size();
    size_t totalSize = sizeof(packet.sequenceNo) + sizeof(size_t) + arrivalTimeLen + packet.packetData.size();

    std::vector<uint8_t> buffer(totalSize);
    uint8_t* ptr = buffer.data();

    // Copy sequenceNo
    std::memcpy(ptr, &packet.sequenceNo, sizeof(packet.sequenceNo));
    ptr += sizeof(packet.sequenceNo);

    // Copy arrivalTime length
    std::memcpy(ptr, &arrivalTimeLen, sizeof(size_t));
    ptr += sizeof(size_t);

    // Copy arrivalTime string
    std::memcpy(ptr, packet.arrivalTime.data(), arrivalTimeLen);
    ptr += arrivalTimeLen;

    // Copy packetData
    std::memcpy(ptr, packet.packetData.data(), packet.packetData.size());

    return buffer;
}
