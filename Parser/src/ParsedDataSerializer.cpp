#include "ParsedDataSerializer.h"
#include "Request.h"
#include "Response.h"

std::vector<uint8_t> ParsedDataSerializer::serialize(const std::unique_ptr<TransactionMessage>& message){

    std::vector<uint8_t> buffer;

    auto serializeString = [&](const std::string& str) {
        size_t len = str.size();
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&len), reinterpret_cast<const uint8_t*>(&len) + sizeof(size_t));
        buffer.insert(buffer.end(), str.begin(), str.end());
    };

    auto serializeByte = [&](uint8_t value) {
        buffer.push_back(value);
    };

    auto serializeInt = [&](auto value) {
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&value), reinterpret_cast<const uint8_t*>(&value) + sizeof(value));
    };

    // Serialize base class fields
    serializeString(message->transactionId);

    // Serialize ParsedData
    const ParsedData& data = message->parsedData;

    serializeInt(data.seqNo);
    serializeString(data.sourceIp);
    serializeString(data.destinationIp);
    serializeInt(data.protocol);
    serializeInt(data.sourcePort);
    serializeInt(data.destinationPort);

    // Serialize SIPMessage
    const SIPMessage& sip = data.sipMessage;
    serializeString(sip.callId);
    serializeString(sip.cSeq);
    serializeString(sip.fromAddress);
    serializeString(sip.toAddress);
    serializeString(sip.userAgent);
    serializeString(sip.fromExtension);
    serializeString(sip.toExtension);
    serializeString(sip.timestamp);

    // Serialize optional SDP info
    if (data.sdpInfo.has_value()) {
        serializeByte(1); // Indicates sdpInfo exists
        const auto& sdpMap = data.sdpInfo.value();
        serializeInt(sdpMap.size());

        for (const auto& [key, sdp] : sdpMap) {
            serializeString(key);
            serializeString(sdp.mediaType);
            serializeInt(sdp.mediaPort);
            serializeString(sdp.protocol);

            // Serialize codecs
            serializeInt(sdp.codecs.size());
            for (const auto& codec : sdp.codecs)
                serializeString(codec);

            // Serialize attributes
            serializeInt(sdp.attributes.size());
            for (const auto& attr : sdp.attributes) {
                serializeString(attr.name);
                serializeString(attr.value);
            }

            // Serialize connectionInfo
            serializeString(sdp.connectionInfo.networkType);
            serializeString(sdp.connectionInfo.addressType);
            serializeString(sdp.connectionInfo.address);
        }
    } else {
        serializeByte(0); // Indicates sdpInfo is absent
    }

    // Serialize derived class fields (dynamic)
    if (const Request* req = dynamic_cast<const Request*>(message.get())) {
        serializeByte(1); // Type: Request
        serializeString(req->sipMethod);
    } else if (const Response* res = dynamic_cast<const Response*>(message.get())) {
        serializeByte(2); // Type: Response
        serializeString(res->status);
        serializeInt(res->statusCode);
    } else {
        serializeByte(0); // Unknown type
    }

    return buffer;
}


