#pragma once

#include <cstdint>
#include <string>
#include <vector>


struct SDPDescriptor{
    std::string callid;
    std::string callerConnectionInfoAddress;
    int callerAudioPort;
    std::string calleeConnectionInfoAddress;
    int calleeAudioPort;
    std::vector<std::string> finalCodecs;
};
