#include "ParsedPacketDeserializer.h"
#include "Request.h"
#include "Response.h"


std::unique_ptr<TransactionMessage> ParsedPacketDeserializer::deserializePacket(const uint8_t* data, size_t length) {
    size_t offset = 0;

    auto readString = [&]() {
        size_t len;
        std::memcpy(&len, data + offset, sizeof(size_t));
        offset += sizeof(size_t);

        std::string str(reinterpret_cast<const char*>(data + offset), len);
        offset += len;
        return str;
    };

    auto readInt = [&](auto& value) {
        std::memcpy(&value, data + offset, sizeof(value));
        offset += sizeof(value);
    };

    auto readUInt8 = [&]() -> uint8_t {
        uint8_t val;
        std::memcpy(&val, data + offset, sizeof(uint8_t));
        offset += sizeof(uint8_t);
        return val;
    };

    std::unique_ptr<TransactionMessage> message;

    // Common fields
    std::string transactionId = readString();

    ParsedData parsedData;
    readInt(parsedData.seqNo);
    parsedData.sourceIp = readString();
    parsedData.destinationIp = readString();
    readInt(parsedData.protocol);
    readInt(parsedData.sourcePort);
    readInt(parsedData.destinationPort);

    SIPMessage sip;
    sip.callId = readString();
    sip.cSeq = readString();
    sip.fromAddress = readString();
    sip.toAddress = readString();
    sip.userAgent = readString();
    sip.fromExtension = readString();
    sip.toExtension = readString();
    sip.timestamp = readString();

    parsedData.sipMessage = sip;

    // SDP Info
    uint8_t hasSDP = readUInt8();
    if (hasSDP) {
        std::map<std::string, SDP> sdpMap;
        size_t sdpCount;
        readInt(sdpCount);
        for (size_t i = 0; i < sdpCount; ++i) {
            std::string key = readString();
            SDP sdp;
            sdp.mediaType = readString();
            readInt(sdp.mediaPort);
            sdp.protocol = readString();

            size_t codecCount;
            readInt(codecCount);
            for (size_t j = 0; j < codecCount; ++j) {
                sdp.codecs.push_back(readString());
            }

            size_t attrCount;
            readInt(attrCount);
            for (size_t j = 0; j < attrCount; ++j) {
                Attributes attr;
                attr.name = readString();
                attr.value = readString();
                sdp.attributes.push_back(attr);
            }

            sdp.connectionInfo.networkType = readString();
            sdp.connectionInfo.addressType = readString();
            sdp.connectionInfo.address = readString();

            sdpMap[key] = sdp;
        }
        parsedData.sdpInfo = sdpMap;
    }

    // Type and dynamic fields
    uint8_t type = readUInt8();

    if (type == 1) {
        auto req = std::make_unique<Request>();
        req->transactionId = transactionId;
        req->parsedData = parsedData;
        req->sipMethod = readString();
        message = std::move(req);
    } else if (type == 2) {
        auto res = std::make_unique<Response>();
        res->transactionId = transactionId;
        res->parsedData = parsedData;
        res->status = readString();
        readInt(res->statusCode);
        message = std::move(res);
    } else {
        // Unknown type: fallback
        auto base = std::make_unique<TransactionMessage>();
        base->transactionId = transactionId;
        base->parsedData = parsedData;
        message = std::move(base);
    }
    
    return message;
}
