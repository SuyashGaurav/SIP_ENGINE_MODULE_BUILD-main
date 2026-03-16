#include <cstring>
#include "PacketDeserializer.h"

SIPPacket PacketDeserializer::deserialize(const uint8_t* data,size_t length)
{
    SIPPacket pkt;
    const uint8_t* ptr = data;

    // Read seq_no
    std::memcpy(&pkt.sequenceNo, ptr, sizeof(pkt.sequenceNo));
    ptr += sizeof(pkt.sequenceNo);

    // Read arrival_time length
    size_t arrivalTimeLen;
    std::memcpy(&arrivalTimeLen, ptr, sizeof(size_t));
    ptr += sizeof(size_t);

    // Read arrival_time string
    pkt.arrivalTime.assign(reinterpret_cast<const char*>(ptr), arrivalTimeLen);
    ptr += arrivalTimeLen;

    // Read packetData
    size_t packetDataLen = length - (sizeof(pkt.sequenceNo) + sizeof(size_t) + arrivalTimeLen);
    pkt.packetData.assign(ptr, ptr + packetDataLen);

    return pkt;
}