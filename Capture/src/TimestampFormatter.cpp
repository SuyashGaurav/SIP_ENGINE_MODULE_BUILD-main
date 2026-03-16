#include "TimestampFormatter.h"
#include "CaptureConfig.h"
#include <ctime>         // for std::localtime, std::strftime
#include <sstream>       // for std::ostringstream

std::string TimestampFormatter::getFormattedTimestamp(struct timeval& ts) {
    char buffer[64];
    std::tm* timeinfo = std::localtime(&ts.tv_sec);
    std::strftime(buffer, sizeof(buffer), CaptureConfig::timeFormat.c_str(), timeinfo);

    std::ostringstream oss;
    oss << buffer;
    return oss.str();
}
