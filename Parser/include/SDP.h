#pragma once
#include <string>
#include <vector>
#include "Attributes.h"
#include "ConnectionInformation.h"

class SDP{
    public:
        std::string mediaType;
        int mediaPort;
        std::string protocol;
        std::vector<std::string> codecs;
        std::vector<Attributes> attributes;
        ConnectionInformation connectionInfo;
};