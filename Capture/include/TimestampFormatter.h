#pragma once

#include "ITimestampFormatter.h"


class TimestampFormatter : public ITimestampFormatter {
    public:
        std::string getFormattedTimestamp(struct timeval& ts) override;
};
