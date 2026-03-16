#pragma once

#include <string>
#include <iostream>

namespace ControllerConfig {
    extern std::string SUB_SUBJECT_NAME;
    extern std::string SUB_STREAM_NAME;
    extern std::string CONSUMER_NAME;
    extern std::string PUB_SUBJECT_NAME;
    extern std::string PUB_STREAM_NAME;
    extern std::string natsURL;
    extern std::string ruleConfigPath;
    extern int FETCH_BATCH_SIZE;
    extern int FETCH_TIMEOUT_MS;

    void loadFromINIFile(const std::string& configPath);
}
