#include "SessionSerializer.h"


std::vector<uint8_t> serializeSession(std::string callId,
                                                            std::string currentState,
                                                            std::optional<SessionContext> sessionContext,
                                                            std::optional<SDPDescriptor> sdpDescriptor)
{
    std::vector<uint8_t> buffer;

    auto serializeString = [&](const std::string &str)
    {
        size_t len = str.size();
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&len), reinterpret_cast<const uint8_t *>(&len) + sizeof(size_t));
        buffer.insert(buffer.end(), str.begin(), str.end());
    };

    auto serializeInt = [&](auto value)
    {
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&value), reinterpret_cast<const uint8_t *>(&value) + sizeof(value));
    };

    auto serializeByte = [&](uint8_t value)
    {
        buffer.push_back(value);
    };

    // 1. Serialize callId and currentState
    serializeString(callId);
    serializeString(currentState);

    // 2. Determine control byte (0, 1, 2, 3)
    uint8_t control = 0;
    if (sessionContext.has_value())
        control |= 1; // 0b01
    if (sdpDescriptor.has_value())
        control |= 2; // 0b10
    serializeByte(control);

    // 3. Serialize SessionContext if present
    if (sessionContext.has_value())
    {
        const SessionContext &ctx = sessionContext.value();
        serializeString(ctx.sourceIp);
        serializeString(ctx.destinationIp);
        serializeInt(ctx.protocol);
        serializeInt(ctx.sourcePort);
        serializeInt(ctx.destinationPort);
        serializeString(ctx.fromAddress);
        serializeString(ctx.toAddress);
        serializeString(ctx.fromExtension);
        serializeString(ctx.toExtension);
        serializeString(ctx.userAgent);
    }

    // 4. Serialize SDPDescriptor if present
    if (sdpDescriptor.has_value())
    {
        const SDPDescriptor &sdp = sdpDescriptor.value();
        serializeString(sdp.callid);
        serializeString(sdp.callerConnectionInfoAddress);
        serializeInt(sdp.callerAudioPort);
        serializeString(sdp.calleeConnectionInfoAddress);
        serializeInt(sdp.calleeAudioPort);

        // Serialize codecs
        serializeInt(static_cast<uint32_t>(sdp.finalCodecs.size()));
        for (const auto &codec : sdp.finalCodecs)
            serializeString(codec);
    }

    return buffer;
}

