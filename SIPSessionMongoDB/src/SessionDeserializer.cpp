#include "SessionDeserializer.h"
#include <iostream>


void deserializeSession(const uint8_t* data, size_t length,
                        std::string& callId,
                        std::string& currentState,
                        std::optional<SessionContext>& ctx,
                        std::optional<SDPDescriptor>& sdp) {
    
    size_t offset = 0;

    auto readString = [&]() -> std::string {
        size_t len;
        std::memcpy(&len, data + offset, sizeof(size_t));
        offset += sizeof(size_t);
        std::string str(reinterpret_cast<const char*>(data + offset), len);
        offset += len;
        return str;
    };

    auto readInt = [&]<typename T>() -> T {
        T value;
        std::memcpy(&value, data + offset, sizeof(T));
        offset += sizeof(T);
        return value;
    };

    auto readByte = [&]() -> uint8_t {
        uint8_t value = data[offset];
        offset += 1;
        return value;
    };

    // 1. Read callId and currentState
    callId = readString();
    currentState = readString();

    // 2. Read control byte
    uint8_t control = readByte();

    // 3. Deserialize SessionContext if present
    if (control & 0b01) {
        SessionContext ctxOpt;
        ctxOpt.sourceIp = readString();
        ctxOpt.destinationIp = readString();
        ctxOpt.protocol = readInt.operator()<int>();
        ctxOpt.sourcePort = readInt.operator()<uint16_t>();
        ctxOpt.destinationPort = readInt.operator()<uint16_t>();
        ctxOpt.fromAddress = readString();
        ctxOpt.toAddress = readString();
        ctxOpt.fromExtension = readString();
        ctxOpt.toExtension = readString();
        ctxOpt.userAgent = readString();

        ctx = std::move(ctxOpt);
    }


    // 4. Deserialize SDPDescriptor if present
    if (control & 0b10) {
        SDPDescriptor sdpOpt;
        sdpOpt.callid = readString();
        sdpOpt.callerConnectionInfoAddress = readString();
        sdpOpt.callerAudioPort = readInt.operator()<int>();
        sdpOpt.calleeConnectionInfoAddress = readString();
        sdpOpt.calleeAudioPort = readInt.operator()<int>();

        uint32_t codecCount = readInt.operator()<uint32_t>();
        for (uint32_t i = 0; i < codecCount; ++i) {
            sdpOpt.finalCodecs.push_back(readString());
        }

        sdp = std::move(sdpOpt);
    }
}
