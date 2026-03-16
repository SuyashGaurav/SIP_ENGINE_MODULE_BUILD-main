#pragma once 
#include <string>
#include <cstdint>

namespace SessionManagerConfig {
    extern std::string SUB_SUBJECT_NAME;
    extern std::string SUB_STREAM_NAME;
    extern std::string CONSUMER_NAME;
    extern std::string PUB_SUBJECT_NAME;
    extern std::string PUB_STREAM_NAME;
    extern std::string natsURL;
    extern int FETCH_BATCH_SIZE;
    extern int FETCH_TIMEOUT_MS;

    void loadFromINIFile(const std::string& configPath);
}
