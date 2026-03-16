#pragma once

#include <pcap.h>
#include <string>


class ITimestampFormatter {
    public:
        virtual ~ITimestampFormatter() = default;
        virtual std::string getFormattedTimestamp(struct timeval& ts) = 0;
};
